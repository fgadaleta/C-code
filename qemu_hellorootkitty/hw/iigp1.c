#include "pci.h"
#include "qemu-timer.h"

#include "hw.h"
#include "dma.h"
#include "net.h"
#include "loader.h"
#include "sysemu.h"
#include "iov.h"

#define PCI_CLASS_DA_DSP (0x1100)
#define PCI_VENDOR_ID_II (0x8089)
#define PCI_VENDOR_ID_II_GP1 (0x0001)

enum registers {
	IntrMask   = 0,
	IntrStatus = 4, 
};

typedef struct IIGP1State
{
    PCIDevice dev;
    QEMUTimer *irq_timer;
    //TODO
    uint32_t IntrStatus;
    uint32_t IntrMask;
    
    MemoryRegion bar_io;
    MemoryRegion bar_mem;
} IIGP1State;

static void iigp1_on_reset (void *opaque)
{
   IIGP1State *s = opaque;
   s->IntrStatus = 1;
   return;
}

/* this updates irq timers and pulse */
static void iigp1_irq_timer_cb(void *opaque)
{
    IIGP1State *s = opaque;
    int isr;                // value of interrupt status register
    int64_t next_time;
    	
    //TODO: raising irq should be dynamic and depend on workload of trusted module
    //isr = 0;  
    isr = (s->IntrStatus & s->IntrMask) & 0xffff;
    fprintf(stderr, "Set IRQ to %d (%04x %04x)\n", isr ? 1 : 0, s->IntrStatus,
        s->IntrMask);

//    next_time = qemu_get_clock_ns(vm_clock) + (get_ticks_per_sec() / 1000);
//    next_time *= 100;
    next_time = qemu_get_clock_ns(vm_clock) + (get_ticks_per_sec()*20);
  
    qemu_mod_timer(s->irq_timer, next_time);
    ///qemu_irq_pulse(s->dev.irq[0]);
    s->IntrMask = 1;
    qemu_set_irq(s->dev.irq[0], (isr != 0));
}



static inline void pci_config_set_byte (void *config, int offset, uint8_t byte)
{
    ((uint8_t*)config)[offset] = byte;
}

/*  read/write operations */
static uint64_t iigp1_read(void *opaque, target_phys_addr_t/*uint8_t*/ addr, 
			   unsigned size)
{
    IIGP1State *s = opaque;
    uint32_t ret;

    //addr &= 0xff;

    switch (addr)
    {     
        case IntrMask:
	    fprintf(stderr, "read (w) IntrMask addr=0x%x\n", addr);
            ret = s->IntrMask;
            break;

        case IntrStatus:
            fprintf(stderr, "read (w) IntrStatus addr=0x%x\n", addr);
            ret = s->IntrStatus;
            break;

        default:
            fprintf(stderr, "not implemented read(b) addr=0x%x\n", addr);
            ret = 0;
            break;
    }

    return ret;
}

static void iigp1_write(void *opaque, target_phys_addr_t/*uint8_t*/ addr, 
			uint64_t val, unsigned size)
{
    IIGP1State *s = opaque;

   // addr &= 0xfe;

    switch (addr)
    {
        case IntrMask:
            fprintf(stderr, "write (w) IntrMask addr=0x%x val=0x%x\n", addr,val);
            //rtl8139_IntrMask_write(s, val);
	    s->IntrMask = val;
	    
            break;

        case IntrStatus:
            fprintf(stderr, "write (w) IntrStatus addr=0x%x val=0x%x\n", addr,val);
            //rtl8139_IntrStatus_write(s, val);
	    s->IntrStatus = val;
	    
            break;

        default:
            fprintf(stderr, "ioport write(w) addr=0x%x val=0x%04x via write(b)\n",
                addr, val);

            break;
    }
}

static const MemoryRegionOps iigp1_io_ops = {
    .read = iigp1_read,
    .write = iigp1_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .impl = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};


static int iigp1_initfn (PCIDevice *dev)
{
    IIGP1State *s = DO_UPCAST (IIGP1State, dev, dev);
    pci_config_set_vendor_id (dev->config, PCI_VENDOR_ID_II);
    pci_config_set_device_id (dev->config, PCI_VENDOR_ID_II_GP1);
    pci_config_set_class (dev->config, PCI_CLASS_DA_DSP);
    pci_config_set_byte (dev->config, PCI_INTERRUPT_PIN, 1);
    pci_config_set_byte (dev->config, PCI_MIN_GNT, 0x0c);
    pci_config_set_byte (dev->config, PCI_MAX_LAT, 0x80);

    memory_region_init_io(&s->bar_io, &iigp1_io_ops, s, "iigp1", 0x100);
    pci_register_bar(&s->dev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY /*PCI_BASE_ADDRESS_SPACE_IO*/, &s->bar_io);
    
    s->irq_timer = qemu_new_timer_ns(vm_clock, iigp1_irq_timer_cb, s);
    qemu_mod_timer(s->irq_timer, qemu_get_clock_ns(vm_clock) + (get_ticks_per_sec() / 1000));

    qemu_register_reset (iigp1_on_reset, s);

    return 0;
}

static PCIDeviceInfo iigp1_info = {
    .qdev.name    = "IIGP1",
    .qdev.desc    = "II GP 1",
    .qdev.size    = sizeof (IIGP1State),
    //.qdev.vmsd    = &vmstate_iigp1,
    .init         = iigp1_initfn,
};

static void iigp1_register (void)
{
    pci_qdev_register (&iigp1_info);
}
device_init (iigp1_register);
