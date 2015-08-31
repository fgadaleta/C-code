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
int memory_major = 60;
/* Buffer to store data */
char *memory_buffer;


int memory_init(void) {
  int result;
  unsigned char* virt;
  size_t size = 128;
  int i;
  unsigned long hyper_n, start_addr, end_addr, pagenum, cr3val;
  
  /* Registering device */
  result = register_chrdev(memory_major, "memory", &memory_fops);
  if (result < 0) {
    printk(KERN_INFO "memory: cannot obtain major number %d\n", memory_major);
    return result;
    }

  printk(KERN_INFO "\n\n ***** Inserting <kernel hypercall> module *****\n\n");


  asm ("movl %%cr3, %0\n" 
          :"=r"(cr3val)
      );
  printk(KERN_INFO "\n ***** CR3 %lx  *****\n", cr3val);


  asm volatile ("vmcall"
                : "=a" (hyper_n)               // output operand
                : "a" (4),"b"(1), "c"(1), "d"(0xcafebabe)  // obj index
               );

  return 0;
}

void memory_exit(void) {
unsigned long hyper_n;
  /* Freeing the major number */
  unregister_chrdev(memory_major, "memory");
  printk(KERN_INFO "Removing memory module\n");

}

int memory_open(struct inode *inode, struct file *filp) {

  printk(KERN_INFO "memory_open \n");
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
copy_to_user(buf, memory_buffer,1);

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
  char *tmp;
  printk(KERN_INFO "from memory_write count=%d\n", count);

  tmp=buf+count-1;
  copy_from_user(memory_buffer,tmp, 1);
  return 1;
}
