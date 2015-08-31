// Linux Device Driver Template/Skeleton
// Kernel Module

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define SKELETON_MAJOR 32
#define SKELETON_NAME "skeleton"
#define BASEPORT 0x378
#define PARALLEL_PORT_INTERRUPT 7
#define CASE1 1
#define CASE2 2

static unsigned int counter = 0;
static char string [128];
static int data;
static int interruptcount = 0;

DECLARE_WAIT_QUEUE_HEAD(skeleton_wait);
static int data_not_ready = 1;

// open function - called when the "file" /dev/skeleton is opened in userspace
static int skeleton_open (struct inode *inode, struct file *file) {
	printk("skeleton_open\n");
	// we could do some checking on the flags supplied by "open"
	// i.e. O_NONBLOCK
	// -> set some flag to disable interruptible_sleep_on in skeleton_read
	return 0;
}

// close function - called when the "file" /dev/skeleton is closed in userspace  
static int skeleton_release (struct inode *inode, struct file *file) {
	printk("skeleton_release\n");
	return 0;
}

// read function called when from /dev/skeleton is read
static ssize_t skeleton_read (struct file *file, char *buf,
		size_t count, loff_t *ppos) {
	int len, err;
	
	// check if we have data - if not, sleep
	// wake up in interrupt_handler
	while (data_not_ready) {
		interruptible_sleep_on(&skeleton_wait);
	}
	data_not_ready = 1;
	
	if( counter <= 0 ) 
		return 0;
	err = copy_to_user(buf,string,counter);
	if (err != 0)
		return -EFAULT;
	len  = counter;
	counter = 0;
	return len;
}

// write function called when to /dev/skeleton is written
static ssize_t skeleton_write (struct file *file, const char *buf,
		size_t count, loff_t *ppos) {
	int err;
	err = copy_from_user(string,buf,count);
	if (err != 0)
		return -EFAULT;
	counter += count;
	return count;
}

// ioctl - I/O control
static int skeleton_ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg) {
	int retval = 0;
	switch ( cmd ) {
		case CASE1:/* for writing data to arg */
			if (copy_from_user(&data, (int *)arg, sizeof(int)))
			return -EFAULT;
			break;
		case CASE2:/* for reading data from arg */
			if (copy_to_user((int *)arg, &data, sizeof(int)))
			return -EFAULT;
			break;
		default:
			retval = -EINVAL;
	}
	return retval;
}

// interrupt handler
static int interrupt_handler(void)
{
	// do stuff
	interruptcount++;
	printk(">>> PARALLEL PORT INT HANDLED: interruptcount=%d\n", interruptcount);
	
	// wake up (unblock) for reading data from userspace
	// and ignore first interrupt generated in module init
	if (interruptcount > 1) {
		data_not_ready = 0;
		wake_up_interruptible(&skeleton_wait);
	}
	
	return IRQ_HANDLED;
}

// define which file operations are supported
struct file_operations skeleton_fops = {
	.owner	=	THIS_MODULE,
	.llseek	=	NULL,
	.read		=	skeleton_read,
	.write	=	skeleton_write,
	.readdir	=	NULL,
	.poll		=	NULL,
	//.ioctl	=	skeleton_ioctl,
	.mmap		=	NULL,
	.open		=	skeleton_open,
	.flush	=	NULL,
	.release	=	skeleton_release,
	.fsync	=	NULL,
	.fasync	=	NULL,
	.lock		=	NULL,
};

// initialize module (and interrupt)
static int __init skeleton_init_module (void) {
	int i;
	int ret;
	printk("initializing module\n");
	
	i = register_chrdev (SKELETON_MAJOR, SKELETON_NAME, &skeleton_fops);
	if (i != 0) return - EIO;
	
	// INIT AND GENERATE INTERRUPT
	ret = request_irq(PARALLEL_PORT_INTERRUPT, &interrupt_handler, IRQF_DISABLED, "parallelport", NULL);
	enable_irq(7);
	//set parallel port to interrupt mode; pins are output
	outb_p(0x10, BASEPORT + 2); 
	printk("Generating interrupt now on all output pins (intr/ACK = pin 10)\n");
	//generate interrupt
	outb_p(0, BASEPORT);
	outb_p(255, BASEPORT);
	outb_p(0, BASEPORT);
	printk("Interrupt generated. You should see the handler-message\n");	
	
	return 0;
}

// close and cleanup module
static void __exit skeleton_cleanup_module (void) {
	printk("cleaning up module\n");
	unregister_chrdev (SKELETON_MAJOR, SKELETON_NAME);
	disable_irq(7);
	free_irq(7, NULL);
}

module_init(skeleton_init_module);
module_exit(skeleton_cleanup_module);
MODULE_AUTHOR("www.captain.at");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Linux Device Driver Template/Skeleton with Interrupt Handler");
