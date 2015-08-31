#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/pci.h>

static struct pci_device_id iigp1_table[] __devinitdata = {
	{ PCI_DEVICE(0x8089, 0x0001) },
	{ },
};

MODULE_DEVICE_TABLE(pci, iigp1_table);

struct iigp1_priv {
	struct pci_dev *dev;
	spinlock_t lock;
	s64 time;
	s64 diff;
	s64 max_diff;
	s64 min_diff;
};

static irqreturn_t iigp1_irq_handler(int i, void *dev)
{
	unsigned long flags = 0;
	struct iigp1_priv *p = dev;
	struct timeval tv;
	s64 now;

	do_gettimeofday(&tv);
	now = timeval_to_ns(&tv);

	spin_lock_irqsave(&p->lock, flags);
	WARN_ONCE(1, "i = %d, dev = %p, flags = %lx\n", i, dev, flags);
	if (p->time) {
		p->diff = now - p->time;
		if (p->max_diff == 0)
			p->max_diff = p->min_diff = p->diff;
		if (p->diff > p->max_diff)
			p->max_diff = p->diff;
		if (p->diff < p->min_diff)
			p->min_diff = p->diff;
	}
	p->time = now;
	spin_unlock_irqrestore(&p->lock, flags);

	return IRQ_HANDLED;
}

static ssize_t iigp1_time_read(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t sz = 0;
	struct iigp1_priv *p = dev_get_drvdata(dev);

	if (p) {
		unsigned long flags;
		spin_lock_irqsave(&p->lock, flags);
		sz = sprintf(buf, "time = %lld, diff = %lld, "
				"min = %lld, max = %lld\n",
				p->time, p->diff, p->min_diff, p->max_diff);
		spin_unlock_irqrestore(&p->lock, flags);
	}
	return sz;
}

static DEVICE_ATTR(time, 0444,
		iigp1_time_read, NULL);

static int __devinit iigp1_probe(struct pci_dev *pdev,
		const struct pci_device_id *id)
{
	int rc;
	struct iigp1_priv *p;

	printk(KERN_INFO "%s pdev = %p\n", __func__, pdev);

	p = kzalloc(sizeof(*p), GFP_ATOMIC);

	if (!p) {
		printk(KERN_ERR "%s: kmalloc failure\n", __func__);
		rc = -ENOMEM;
		goto out;
	}

	p->dev = pdev;
	spin_lock_init(&p->lock);

	rc = device_create_file(&pdev->dev, &dev_attr_time);
	if (rc) {
		printk(KERN_ERR "%s: device_create_file failure\n", __func__);
		goto out_free;
	}

	rc = pci_enable_device(pdev);
	if (rc) {
		printk(KERN_ERR "%s: pci_enable_device failure\n", __func__);
		goto out_free;
	}

	rc = request_irq(pdev->irq, iigp1_irq_handler,
			IRQF_SHARED, "iigp1", p);

	if (rc) {
		printk(KERN_ERR "%s: request_irq failure\n", __func__);
		goto out_free;
	}

	pci_set_drvdata(pdev, p);
	return rc;

out_free:
	kfree(p);
out:
	return rc;
}

static void __devexit iigp1_remove(struct pci_dev *pdev)
{
	struct iigp1_priv *p = pci_get_drvdata(pdev);

	printk(KERN_INFO "%s drvdata = %p\n", __func__, p);
	pci_disable_device(pdev);
	if (p) {
		free_irq(pdev->irq, p);
		device_remove_file(&pdev->dev, &dev_attr_time);
	}
	kfree(p);
}

static struct pci_driver iigp1_driver = {
	.name		= "iigp1",
	.id_table	= iigp1_table,
	.probe		= iigp1_probe,
	.remove		= __devexit_p(iigp1_remove),
};

static int __init iigp1_init(void)
{
	printk(KERN_INFO "%s\n", __func__);
	return pci_register_driver(&iigp1_driver);
}

static void __exit iigp1_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);
	pci_unregister_driver(&iigp1_driver);
}

module_init(iigp1_init);
module_exit(iigp1_exit);

MODULE_AUTHOR("jcmvbkbc");
MODULE_LICENSE("GPL");
