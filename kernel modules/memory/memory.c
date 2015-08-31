/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */

#include <linux/kmemleak.h>
#include <asm/page.h>

MODULE_LICENSE("Dual BSD/GPL");

#define DEVSIZE 64
#define MAX_PROT_OBJECTS 1

#define SYSTABLE_ADDR 0xc12c80f0


/* Declaration of memory.c functions */
int memory_open(struct inode *inode, struct file *filp);
int memory_release(struct inode *inode, struct file *filp);
ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t memory_write(struct file *filp, char *buf, size_t count, loff_t *f_pos);
void memory_exit(void);
int memory_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations memory_fops = {
  read: memory_read,
  write: memory_write,
  open: memory_open,
  release: memory_release
};

/* Declaration of the init and exit functions */
module_init(memory_init);
module_exit(memory_exit);

/* Global variables of the driver */
/* Major number */
int memory_major = 64;
unsigned char* virt;
unsigned char* synt;
size_t size = 128;

int memory_init(void) {
  int result, i;
  unsigned long hyper_n, start_addr, end_addr, pagenum, synt_start, synt_end;
  char fixed[] = "i_am_content_to_be_protected_\
i_am_content_to_be_protected_i_am_content_to_be_\
protected_i_am_content_to_be_protected_i_am_conten";

  /* Registering device */
  result = register_chrdev(memory_major, "memory", &memory_fops);
  if (result < 0) {
    printk(KERN_INFO "memory: cannot obtain major number %d\n", memory_major);
    return result;
  }

  printk(KERN_INFO "\n\n ***** Inserting <hello rootKitty> trusted module *****\n\n");
 
//for(i=0;i<MAX_PROT_OBJECTS;i++) { 
  i = 0;
  virt = kmalloc(size, GFP_KERNEL);
  if(!virt) { 
 	result = -ENOMEM;
	goto fail;
  } 
  
  synt = kmalloc(sizeof(fixed), GFP_KERNEL);
  if(!synt) {
      result = -ENOMEM;
      goto fail;
  }

  memcpy(synt, fixed, sizeof(fixed));

  // TEST: this protects the system call table
  virt = SYSTABLE_ADDR;

  // TODO: here the difference should be size!! why it's not??
  //printk(KERN_INFO "Allocated obj in range [0x%lx-0x%lx]\n", virt, (virt+size));

  /* physical addresses */
  start_addr = __pa(virt);
  end_addr   = __pa(virt+size);
  synt_start = __pa(synt);
  synt_end   = __pa(synt+size);


  //printk(KERN_INFO "Contents of protected obj %s\n", virt);
  /* in most cases kernel obj are small and lay on 1 physical page */  
  if((start_addr>>12 == end_addr>>12) && (synt_start>>12 == synt_end>>12) )
    {  
      printk (KERN_INFO "sending obj (size %d) at [virt=0x%lx phys=0x%lx]\n", size, virt, start_addr);
  
      asm volatile ("vmcall"
                    : "=a" (hyper_n)               // output operand
                    : "a" (4),"b"(start_addr), "c"(size), "d"(i)  // obj index
                                                   // input operands
                                                   // clobbered registers 
                  );

      printk (KERN_INFO "sending obj (size %d) at [virt=0x%lx phys=0x%lx]\n", size, synt, synt_start);


      asm volatile ("vmcall"
                    : "=a" (hyper_n)               // output operand
                    : "a" (4),"b"(synt_start), "c"(size), "d"(0xcafebabe) 
                  );
  }
  
  else  // object is on different physical pages 
 	// TODO: handle page translation	
  {
    //initial physical page
    pagenum = (unsigned long)virt >> 12;
    unsigned long offset = ((unsigned long)virt - pagenum);   // offset within page
    unsigned long bytes_in = 4096-offset;
    unsigned long last_in_page = ((unsigned long)virt + bytes_in);
    unsigned long bytes_out = size - bytes_in;
    unsigned int pieces = size%bytes_in;
    printk(KERN_INFO "piece of object virtual (0x%lx-0x%lx) phys (0x%lx-0x%lx)\n", virt ,(virt+bytes_in), __pa(virt), __pa(virt+bytes_in) ); 
    
/*
  asm volatile ("vmcall"
                       : "=a" (hyper_n)          
                       : "a" (4),"b"(start_addr), "c"(bytes_in), "d"(1)                 );

    virt=virt+bytes_in+1;  // last_in_page
    start_addr = __pa(virt);
      
    printk(KERN_INFO "piece of object virtual (0x%lx-0x%lx) phys (0x%lx-0x%lx)\n", virt ,(virt+bytes_out), __pa(virt), __pa(virt+bytes_out));
 
    asm volatile ("vmcall"
                       : "=a" (hyper_n)          
                       : "a" (4),"b"(start_addr), "c"(bytes_out), "d"(0)
                 );
*/
  }


//}  // for

// inform hypervisor that trusted module finished
/*
asm volatile ("vmcall"
               : "=a" (hyper_n)               // output operand
               : "a" (4),"b"(1), "c"(1), "d"(0xcafebabe)  // obj index
                                               // input operands
                                               // clobbered registers 
              );
*/

  return 0;

  fail: 
    memory_exit(); 
    return result;
}

void memory_exit(void) {
unsigned long hyper_n;
  /* Freeing the major number */
  unregister_chrdev(memory_major, "memory");
  
  /* Switch off protections */
  printk(KERN_INFO "Switching off protection\n");
  asm volatile ("vmcall"
               : "=a" (hyper_n)               // output operand
               : "a" (4),"b"(1), "c"(1), "d"(0xdeadbeaf)  // obj index
                                               // input operands
                                               // clobbered registers 
              );
  printk(KERN_INFO "DEBUG Protected object at 0x%lx [0x%lx]\n", virt, virt[0]);
  printk(KERN_INFO "Removing memory module\n");

}

int memory_open(struct inode *inode, struct file *filp) {
/*
printk(KERN_INFO "Synthetic compromission of kernel data\n");
printk(KERN_INFO "**********************************\n");
printk(KERN_INFO "DEBUG Compromising protected object at 0x%lx [0x%lx]\n", virt, virt[0]);
memset(virt, 'b', size);
memcpy(virt, "HACKED!", 12);
printk(KERN_INFO "DEBUG Compromised protected object at 0x%lx [0x%lx]\n", virt, virt[0]);
printk(KERN_INFO "**********************************\n");
*/
  /* Success */
  return 0;
}

int memory_release(struct inode *inode, struct file *filp) {
 
  printk(KERN_INFO "memory_release \n");
  /* Success */
  return 0;
}

/* buf is the buffer from which the user space function will read */
ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) { 
  ssize_t retval=-EFAULT;
 
printk(KERN_INFO "from memory_read count=%d\n", count);


/* Changing reading position as best suits */
if(*f_pos == 0) {
    *f_pos+=1;
    return 1;    /* memory_read must return the number of bytes read */
} else {
    return 0;
  }
}

/* buf is the buffer in which the user space function will write */
ssize_t memory_write(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
   printk(KERN_INFO "from memory_write count=%d\n", count);
   return 1;
}
