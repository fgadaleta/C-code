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

#define BUFSIZE 128

MODULE_LICENSE("Dual BSD/GPL");

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
unsigned long memory_buffer[6];
int memory_buffer_size;  


/* e.g.: vmcall(4, shared, shared_pa, 0xdeadbeaf) 
raise hypercall num and arguments  */
static void vmcall(int num, ulong b, ulong c, ulong d) {
  b=c=d=0xdead;
  asm volatile ("vmcall" 
		: "+a"(num), "=b"(b), "=c"(c), "=d"(d));
}


static void check_operation(unsigned long *buffer)
{
 unsigned long arg1, arg2, arg3;
 int hypernum = (int) buffer[0];
 arg1 = buffer[1]; 
 arg2 = buffer[2];
 arg3 = buffer[3];

 switch(hypernum) {
	case 1: 
	  printk(KERN_INFO "calling hypercall 1\n");
  	  //vmcall(1, arg1,arg2,arg3);
	  break;
	
	case 2:
	  printk(KERN_INFO "calling hypercall 2, arg1=%x arg2=%x arg3=%x\n", arg1,arg2,arg3);
  	  //vmcall(2, arg1,arg2,arg3);
	  break;

	case 3:
	  printk(KERN_INFO "calling hypercall 3, arg1=%x arg2=%x arg3=%x\n", arg1,arg2,arg3);
  	  //vmcall(3, arg1,arg2,arg3);
	  break;
	
	case 4:
	  printk(KERN_INFO "calling hypercall 4, arg1=%x arg2=%x arg3=%x\n", arg1,arg2,arg3);
  	  //vmcall(4, arg1,arg2,arg3);
	  break;
	
  	case 5:
	  printk(KERN_INFO "calling hypercall 5, arg1=%x arg2=%x arg3=%x\n", arg1,arg2,arg3);
  	  //vmcall(5, arg1,arg2,arg3);
	  break;
	
	case 6:
	  printk(KERN_INFO "calling hypercall 6, arg1=%x arg2=%x arg3=%x\n", arg1,arg2,arg3);
  	  //vmcall(6, arg1,arg2,arg3);
	  break;

	default:
	  printk(KERN_INFO "Unknown hypercall\n");
	  break;
     }

}



int memory_init(void) {
  int result;

  /* Registering device */
  result = register_chrdev(memory_major, "trusted", &memory_fops);
  if (result < 0) {
    printk(
      "<1>trusted: cannot obtain major number %d\n", memory_major);
    return result;
  }

  /* Allocating memory for the buffer */
  /*
  memory_buffer = kmalloc(BUFSIZE, GFP_KERNEL);
  if (!memory_buffer) {
    result = -ENOMEM;
    goto fail;
  }
  memset(memory_buffer, 0, BUFSIZE);
  memory_buffer[BUFSIZE-1] = 0;
 */

  printk("<1>Inserting trusted module\n");
  return 0;

  fail:
    memory_exit();
    return result;
}

void memory_exit(void) {
  /* Freeing the major number */
  unregister_chrdev(memory_major, "trusted");

  /* Freeing buffer memory */
  /*
  if (memory_buffer) {
    kfree(memory_buffer);
  }
  */

  printk("<1>Removing trusted module\n");

}

int memory_open(struct inode *inode, struct file *filp) {

  printk("<1>Opening trusted module\n");
  /* Success */
  return 0;
}

int memory_release(struct inode *inode, struct file *filp) {

  /* Success */
  return 0;
}

/* Read device with fread and similar: 
   read from memory_buffer (kernel) to userspace 
   user must allocate a proper buf before reading device
*/
ssize_t memory_read(struct file *filp, char *buf,
                    size_t count, loff_t *f_pos) {

  if(*f_pos >= BUFSIZE)
   return 0;  


  /* Transfering data to user space */
  if(copy_to_user(buf,memory_buffer,count))
  {
	printk("<1>Error reading from device\n");
        return -EFAULT;
  }

  /* Changing reading position as best suits */
    *f_pos+=count;


printk("Reading from device f_pos = %d\n", *f_pos);
    return count;
}


/* Write to device: buf is where userspace program writes to and then call fwrite 
count number of bytes to transfer */
ssize_t memory_write( struct file *filp, char *buf,
                      size_t count, loff_t *f_pos) {

  char *tmp;
  tmp = buf;
  
  if(copy_from_user(&memory_buffer/*to*/,tmp/*from*/,count)) 
   {
  	printk("<1>Error writing to device\n");
	return -EFAULT;
   }
  

 *f_pos += count;
 memory_buffer_size = count;
 
 check_operation(memory_buffer);
 return count;
}


