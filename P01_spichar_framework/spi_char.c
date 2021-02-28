#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/platform_data/serial-omap.h>
#include "spi_char.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1

static struct omap2_mcspi *mcspi;

static int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "SPI driver: %s\n",__func__);
	return 0;
}
static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "SPI driver: %s\n",__func__);
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "SPI driver: %s\n",__func__);
	// TODO 8: Invoke the low level TX/RX function
	return 0;
}

static struct file_operations driver_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
};

int chrdev_init(struct omap2_mcspi *lmcspi)
{
	int ret = 0;
	struct device *dev_ret = NULL;
	mcspi = lmcspi;

	ENTER_1()
	/*STEP1: get a free major number and minor number */
	if ((ret = alloc_chrdev_region(&mcspi->devt, FIRST_MINOR, MINOR_CNT, "spi_driver")) < 0)
	{
		return ret;
	}

	ENTER_1()
    // TODO 3: Register the file_operations
    /*STEP 1.1: this step is independ of other steps*/
	cdev_init(&mcspi->cdev, &driver_fops);

	/*STEP2: create the class with class name "spi"*/
	if (IS_ERR(lmcspi->spi_class = class_create(THIS_MODULE, "spi")))
	{
		cdev_del(&mcspi->cdev);
		unregister_chrdev_region(mcspi->devt, MINOR_CNT);
		return PTR_ERR(lmcspi->spi_class);
	}

	ENTER_1()
	// TODO 4: Create the device file with name spi0
	/*STEP3: create the device with the class*/
	dev_ret = device_create(mcspi->spi_class, NULL, mcspi->devt, NULL, "spio");
	if (IS_ERR(dev_ret))
	{
		class_destroy(mcspi->spi_class);
		cdev_del(&mcspi->cdev);
		unregister_chrdev_region(mcspi->devt, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}	

	ENTER_1()
	/* STEP4: add the device number to the device*/
	ret = cdev_add(&mcspi->cdev, mcspi->devt, 1);

	if (ret < 0)
	{
		device_destroy(mcspi->spi_class, mcspi->devt); //only class and device file no
		class_destroy(mcspi->spi_class);
		cdev_del(&mcspi->cdev);
		unregister_chrdev_region(mcspi->devt, MINOR_CNT);
		return ret;
	}
	
	ENTER_1()
	return 0;
}

void chrdev_exit(void)
{

	ENTER_1()
	// TODO 5: Delete the device file & the class
	device_destroy(mcspi->spi_class, mcspi->devt); //only class and device file no
	class_destroy(mcspi->spi_class);

    // TODO 6: Unregister file operations
  	cdev_del(&mcspi->cdev);

    // TODO 7: Unregister character driver
 	unregister_chrdev_region(mcspi->devt, MINOR_CNT);
 	ENTER_1()
}
