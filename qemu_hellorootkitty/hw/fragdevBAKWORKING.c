/*
  fragdev
  security-enforcing virtual device 
*/

//FGTAG for grep
/* For crc32 */
#include <zlib.h>

#include "hw.h"
#include "pci.h"
#include "apm.h"
#include "acpi.h"
#include "ioport.h"
#include "range.h"
#include "dma.h"
#include "qemu-timer.h"
#include "net.h"
#include "loader.h"
#include "sysemu.h"
#include "iov.h"

#define SET_MASKED(input, mask, curr) \
    ( ( (input) & ~(mask) ) | ( (curr) & (mask) ) )

/* arg % size for size which is a power of 2 */
#define MOD2(input, size) \
    ( ( input ) & ( size - 1 )  )

#define PCI_CLASS_DA_DSP (0x1100)
#define PCI_VENDOR_ID_II (0x8089)
#define PCI_VENDOR_ID_II_GP1 (0x0001)

#define PCI_DEVICE_ID_FRAGDEV  PCI_VENDOR_ID_II
#define PCI_VENDOR_ID_FRAGDEV  PCI_VENDOR_ID_II_GP1
#define PCI_FREQUENCY 33000000L

/* 12 bits = 4MB */
#define EEPROM_ADDR_BITS   12         
#define EEPROM_SIZE        (1 << EEPROM_ADDR_BITS)
#define EEPROM_ADDR_MASK   (EEPROM_SIZE - 1)

struct pci_status {
  uint32_t up;
  uint32_t down;	
};


#if defined (DEBUG_FRAGDEV)
#  define DPRINTF(fmt, ...) \
    do { fprintf(stderr, "FRAGDEV: " fmt, ## __VA_ARGS__); } while (0)
#else

static inline GCC_FMT_ATTR(1, 2) int DPRINTF(const char *fmt, ...)
{
    return 0;
}
#endif

/* Symbolic offsets to registers. */
enum FRAGDEV_registers {
  Timer = 0x48,        /* A general-purpose counter. */
  IntrMask = 0x3C,
  IntrStatus = 0x3E,
  HltClk = 0x5B,
  MultiIntr = 0x5C,
  PCIRevisionID = 0x5E,
};

/* Interrupt register bits, using my own meaningful names. */
  enum IntrStatusBits {
    PCIErr = 0x8000,
    PCSTimeout = 0x4000,
    RxFIFOOver = 0x40,
    RxUnderrun = 0x20,
    RxOverflow = 0x10,
    TxErr = 0x08,
    TxOK = 0x04,
    RxErr = 0x02,
    RxOK = 0x01,
    RxAckBits = RxFIFOOver | RxOverflow | RxOK,
};


/* Bits in Config1 */
enum Config1Bits {
    Cfg1_PM_Enable = 0x01,
    Cfg1_VPD_Enable = 0x02,
    Cfg1_PIO = 0x04,
    Cfg1_MMIO = 0x08,
    LWAKE = 0x10,        /* not on 8139, 8139A */
    Cfg1_Driver_Load = 0x20,
    Cfg1_LED0 = 0x40,
    Cfg1_LED1 = 0x80,
    SLEEP = (1 << 1),    /* only on 8139, 8139A */
    PWRDN = (1 << 0),    /* only on 8139, 8139A */
};

enum Cfg9346Bits {
    Cfg9346_Lock = 0x00,
    Cfg9346_Unlock = 0xC0,
};


typedef struct EEprom
{
  uint16_t contents[EEPROM_SIZE];
  int      mode;
  uint32_t tick;
  uint8_t  address;
  uint16_t input;
  uint16_t output;
  
  uint8_t eecs;
  uint8_t eesk;
  uint8_t eedi;
  uint8_t eedo;
} EEprom;

enum fragdevChipOperation
{
    Chip9346_op_mask = 0xc0,          /* 10 zzzzzz */
    Chip9346_op_read = 0x80,          /* 10 AAAAAA */
    Chip9346_op_write = 0x40,         /* 01 AAAAAA D(15)..D(0) */
    Chip9346_op_ext_mask = 0xf0,      /* 11 zzzzzz */
    Chip9346_op_write_enable = 0x30,  /* 00 11zzzz */
    Chip9346_op_write_all = 0x10,     /* 00 01zzzz */
    Chip9346_op_write_disable = 0x00, /* 00 00zzzz */
};

enum fragdevChipMode
{
    Chip9346_none = 0,
    Chip9346_enter_command_mode,
    Chip9346_read_command,
    Chip9346_data_read,      /* from output register */
    Chip9346_data_write,     /* to input register, then to contents at specified address */
    Chip9346_data_write_all, /* to input register, then filling contents */
};

typedef struct FRAGDEVState
{
  PCIDevice dev;
  MemoryRegion mmio;
  MemoryRegion io;
  MemoryRegion code;         // injected code to force execution of
  IORange ioport;
  qemu_irq irq;
  qemu_irq smi_irq;
  struct pci_status pci0_status;



  uint16_t phy_reg[0x20];    // 32 registers 
  uint32_t ctl;              // control fields
  uint32_t status;
  uint32_t mempage;
    
  /* PCI interrupt timer */
  QEMUTimer *timer;
  int64_t TimerExpire;
  
  uint16_t IntrStatus;       // interrupt stuff
  uint16_t IntrMask;
  uint32_t TimerInt;
  uint16_t MultiIntr;
  
  MemoryRegion bar_io;
  MemoryRegion bar_mem;
  
  int64_t  TCTR_base;
  uint8_t  clock_enabled;
  
  EEprom eeprom;            // eeprom to restore everything back if something happens
} FRAGDEVState;


static void fragdev_update_irq(FRAGDEVState *s)
{
  int isr;
  isr = (s->IntrStatus & s->IntrMask) & 0xffff;

  DPRINTF("Set IRQ to %d (%04x %04x)\n", isr ? 1 : 0, s->IntrStatus,
	  s->IntrMask);
  qemu_set_irq(s->dev.irq[0], (isr != 0));
}

#define MIN_BUF_SIZE 60
static inline dma_addr_t rtl8139_addr64(uint32_t low, uint32_t high)
{
#if TARGET_PHYS_ADDR_BITS > 32
    return low | ((target_phys_addr_t)high << 32);
#else
    return low;
#endif
}



static void fragdev_set_next_tctr_time(FRAGDEVState *s, int64_t current_time)
{
    int64_t pci_time, next_time;
    uint32_t low_pci;
    
    DPRINTF("entered fragdev_set_next_tctr_time\n");

    if (s->TimerExpire && current_time >= s->TimerExpire) {
        s->IntrStatus |= PCSTimeout;
        fragdev_update_irq(s);
    }
    
    /* Set QEMU timer only if needed that is
     * - TimerInt <> 0 (we have a timer)
     * - mask = 1 (we want an interrupt timer)
     * - irq = 0  (irq is not already active)
     * If any of above change we need to compute timer again
     * Also we must check if timer is passed without QEMU timer
     */
    s->TimerExpire = 0;
    if (!s->TimerInt) {
      return;
    }
    
    pci_time = muldiv64(current_time - s->TCTR_base, PCI_FREQUENCY,
			get_ticks_per_sec());
    low_pci = pci_time & 0xffffffff;
    pci_time = pci_time - low_pci + s->TimerInt;
    if (low_pci >= s->TimerInt) {
      pci_time += 0x100000000LL;
    }
    next_time = s->TCTR_base + muldiv64(pci_time, get_ticks_per_sec(),
					PCI_FREQUENCY);
    s->TimerExpire = next_time;
    
    if ((s->IntrMask & PCSTimeout) != 0 && (s->IntrStatus & PCSTimeout) == 0) {
      qemu_mod_timer(s->timer, next_time);
    }
}



static void fragdev_timer(void *opaque)
{
  FRAGDEVState *s = opaque;
  
  if (!s->clock_enabled)
    {
      DPRINTF(">>> timer: clock is not running\n");
      return;
    }
  
  s->IntrStatus |= PCSTimeout;
  fragdev_update_irq(s);
  fragdev_set_next_tctr_time(s, qemu_get_clock_ns(vm_clock));
}



static void fragdev_IntrMask_write(FRAGDEVState *s, uint32_t val)
{
  DPRINTF("IntrMask write(w) val=0x%04x\n", val);
  
  /* mask unwritable bits */
  val = SET_MASKED(val, 0x1e00, s->IntrMask);
  
  s->IntrMask = val;
  
  fragdev_set_next_tctr_time(s, qemu_get_clock_ns(vm_clock));
  fragdev_update_irq(s);
}


static uint32_t fragdev_IntrMask_read(FRAGDEVState *s)
{
    uint32_t ret = s->IntrMask;
    DPRINTF("IntrMask read(w) val=0x%04x\n", ret);
    return ret;
}

static void fragdev_IntrStatus_write(FRAGDEVState *s, uint32_t val)
{
    DPRINTF("IntrStatus write(w) val=0x%04x\n", val);
#if 0
    /* writing to ISR has no effect */
    return;
#else
    uint16_t newStatus = s->IntrStatus & ~val;

    /* mask unwritable bits */
    newStatus = SET_MASKED(newStatus, 0x1e00, s->IntrStatus);

    /* writing 1 to interrupt status register bit clears it */
    s->IntrStatus = 0;
    fragdev_update_irq(s);

    s->IntrStatus = newStatus;
    /*
     * Computing if we miss an interrupt here is not that correct but
     * considered that we should have had already an interrupt
     * and probably emulated is slower is better to assume this resetting was
     * done before testing on previous rtl8139_update_irq lead to IRQ loosing
     */
    fragdev_set_next_tctr_time(s, qemu_get_clock_ns(vm_clock));
    fragdev_update_irq(s);

#endif
}

static uint32_t fragdev_IntrStatus_read(FRAGDEVState *s)
{
    rtl8139_set_next_tctr_time(s, qemu_get_clock_ns(vm_clock));
    uint32_t ret = s->IntrStatus;
    DPRINTF("IntrStatus read(w) val=0x%04x\n", ret);

#if 0
    /* reading ISR clears all interrupts */
    s->IntrStatus = 0;
    fragdev_update_irq(s);
#endif
    return ret;
}


static void fragdev_MultiIntr_write(FRAGDEVState *s, uint32_t val)
{
    DPRINTF("MultiIntr write(w) val=0x%04x\n", val);
    /* mask unwritable bits */
    val = SET_MASKED(val, 0xf000, s->MultiIntr);
    s->MultiIntr = val;
}

static uint32_t fragdev_MultiIntr_read(FRAGDEVState *s)
{
    uint32_t ret = s->MultiIntr;
    DPRINTF("MultiIntr read(w) val=0x%04x\n", ret);
    return ret;
}




static void fragdev_mmio_write(void*opaque, target_phys_addr_t addr, 
			       uint64_t val, unsigned size)
{
}

static uint64_t fragdev_mmio_read(void*opaque, target_phys_addr_t addr, unsigned size)
{
  return 0;
}


static const VMStateDescription vmstate_fragdev = {
    .name = "fragdev",
    .version_id = 4,
    .minimum_version_id = 3,
    .minimum_version_id_old = 3,
    //.post_load = rtl8139_post_load,
    //.pre_save  = rtl8139_pre_save,
    .fields      = (VMStateField []) {
        VMSTATE_PCI_DEVICE(dev, FRAGDEVState),
        VMSTATE_UINT16(IntrStatus, FRAGDEVState),
        VMSTATE_UINT16(IntrMask, FRAGDEVState),
        VMSTATE_UINT8(clock_enabled, FRAGDEVState),
	VMSTATE_UINT16(MultiIntr, FRAGDEVState),
	VMSTATE_UNUSED(4),
        VMSTATE_UINT32(TimerInt, FRAGDEVState),
        VMSTATE_INT64(TCTR_base, FRAGDEVState),
        VMSTATE_END_OF_LIST()
    },
    
};

static const MemoryRegionOps fragdev_mmio_ops = {
  .read  = fragdev_mmio_read,
  .write = fragdev_mmio_write,
  .endianness = DEVICE_LITTLE_ENDIAN,
};


static uint64_t fragdev_io_read(void*opaque, target_phys_addr_t addr, unsigned size)
{
  FRAGDEVState *s = opaque;
  (void)s;
  return 0;
}

static void fragdev_io_write(void*opaque, target_phys_addr_t addr, 
				 uint64_t val, unsigned size)
{
  FRAGDEVState *s = opaque;
  (void)s;
}



static const MemoryRegionOps fragdev_io_ops = {
  .read  = fragdev_io_read,
  .write = fragdev_io_write,
  .endianness = DEVICE_LITTLE_ENDIAN,
};

static int pci_fragdev_uninit(PCIDevice *dev)
{
    FRAGDEVState *s = DO_UPCAST(FRAGDEVState, dev, dev);
    memory_region_destroy(&s->bar_io);
    memory_region_destroy(&s->bar_mem);
    qemu_del_timer(s->timer);
    qemu_free_timer(s->timer);
    return 0;
}


static void fragdev_reset(FRAGDEVState *s)
{
  s->ctl      = 1;
  s->status   = 0x60;
  s->mempage  = 0;
  s->TimerInt = 0;
  
  /* reset interrupt mask */
  s->IntrStatus  = 0;
  s->IntrMask    = 0;
  fragdev_update_irq(s);
}

static void fragdev_on_reset (void *opaque)
{
  FRAGDEVState *s = opaque;
  fragdev_reset(s);
}


static inline void pci_config_set_byte (void *config, int offset, uint8_t byte)
{
    ((uint8_t*)config)[offset] = byte;
}

static int fragdev_exitfn(PCIDevice *dev)
{
  FRAGDEVState *s = DO_UPCAST(FRAGDEVState, dev, dev);
  memory_region_destroy(&s->io);
  return 0;
}


static int fragdev_initfn (PCIDevice *dev)
{
  FRAGDEVState *s = DO_UPCAST(FRAGDEVState, dev, dev);
  uint8_t *pci_conf;
  
  pci_conf = s->dev.config;
  pci_conf[PCI_INTERRUPT_PIN] = 1;    /* interrupt pin A */
  /* TODO: start of capability list, but no capability
   * list bit in status register, and offset 0xdc seems unused. */
  pci_conf[PCI_CAPABILITY_LIST] = 0xdc;
  
  memory_region_init_io(&s->bar_io, &fragdev_io_ops, s, "fragdev", 0x100);
  memory_region_init_io(&s->bar_mem, &fragdev_mmio_ops, s, "fragdev", 0x100);
  pci_register_bar(&s->dev, 0, PCI_BASE_ADDRESS_SPACE_IO, &s->bar_io);
  pci_register_bar(&s->dev, 1, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->bar_mem);
  
  /* prepare eeprom */
  s->eeprom.contents[0] = 0x8129;
#if 1
  /* PCI vendor and device ID should be mirrored here */
  s->eeprom.contents[1] = PCI_VENDOR_ID_FRAGDEV;
  s->eeprom.contents[2] = PCI_DEVICE_ID_FRAGDEV;
#endif
  //s->eeprom.contents[7] = s->conf.macaddr.a[0] | s->conf.macaddr.a[1] << 8;
  //s->eeprom.contents[8] = s->conf.macaddr.a[2] | s->conf.macaddr.a[3] << 8;
  //s->eeprom.contents[9] = s->conf.macaddr.a[4] | s->conf.macaddr.a[5] << 8;

  //s->nic = qemu_new_nic(&net_rtl8139_info, &s->conf,
  //dev->qdev.info->name, dev->qdev.id, s);
  //qemu_format_nic_info_str(&s->nic->nc, s->conf.macaddr.a);
  
  pci_config_set_vendor_id (dev->config, PCI_VENDOR_ID_II);
  pci_config_set_device_id (dev->config, PCI_VENDOR_ID_II_GP1);
  pci_config_set_class (dev->config, PCI_CLASS_DA_DSP);
  pci_config_set_byte (dev->config, PCI_INTERRUPT_PIN, 1);    // interrupt pin A
  pci_config_set_byte (dev->config, PCI_MIN_GNT, 0x0c);
  pci_config_set_byte (dev->config, PCI_MAX_LAT, 0x80);

  s->TimerExpire = 0;
  s->timer = qemu_new_timer_ns(vm_clock, fragdev_timer, s);
  fragdev_set_next_tctr_time(s, qemu_get_clock_ns(vm_clock));
  qemu_register_reset(fragdev_on_reset, dev);
  return 0;
    
  
  /*
    memory_region_init_io(&s->io, &fragdev_io_ops, s, "fragdev", 256);
    pci_register_bar(&s->dev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->mmio);
    pci_register_bar(&s->dev, 1, PCI_BASE_ADDRESS_SPACE_IO, &s->io);
    qemu_register_reset (fragdev_on_reset, s);
    
    fragdev_set_next_tctr_time(s, qemu_get_clock_ns(vm_clock));
    //add_boot_device_path(s->conf.bootindex, &dev->qdev, "/ethernet-phy@0");
    //fragdev_reset(s);
    */
  
}

static PCIDeviceInfo fragdev_info = {
    .qdev.name    = "fragdev",
    .qdev.desc    = "(SEVH) Security Enforcing Virtual Hardware",
    .qdev.size    = sizeof (FRAGDEVState),
    .qdev.reset   = fragdev_on_reset,
    //.qdev.vmsd  = &vmstate_fragdev,
    .init         = fragdev_initfn,
    .exit         = fragdev_exitfn,
};


static void fragdev_register (void)
{
  pci_qdev_register (&fragdev_info);
}
device_init (fragdev_register);
