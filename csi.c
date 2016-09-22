/*
CSI-MIPI Driver
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("skukreti");
MODULE_DESCRIPTION("pine64-csi");

#define MIPI_NAME "pine64-csi"


struct csi_cci_platform_data {
	void (*enable_csi) (struct csi_cci_platform_data *pdata);
	void (*enable_cci) (struct csi_cci_platform_data *pdata);

}pine64_pdata;


static struct resource csi_resources[] = {
	{
		.start	= 0x01CB0000,
		.end	= 0x01CFFFFF,
		.flags	= IORESOURCE_MEM,
		.name	= "csi-io-memory"
	},
	{
		.start	= 117,
		.end	= 117,
		.flags	= IORESOURCE_IRQ,
		.name	= "cci-irq",
	}
    };

 static struct platform_device csi_device = {
	.name 		= MIPI_NAME,
	.resource	= csi_resources,
	.num_resources	= ARRAY_SIZE(csi_resources),
	.id = -1,
	.dev.platform_data = &pine64_pdata,
    };

static int csi_cci_probe (struct platform_device *pdev)
{
	printk(KERN_INFO "CSI CCI Probe\n");

return 0;
}


static int csi_cci_remove (struct platform_device *pdev)
{

	printk(KERN_INFO "CSI CCI Remove \n");

return 0;
}

static struct platform_driver csi_cci_driver = {
	.probe = csi_cci_probe,
	.remove = csi_cci_remove,
	.driver = {
		.name = MIPI_NAME,
		.owner = THIS_MODULE
	},	

};

static int __init csi_cci_init(void) 
{
	int ret;
	ret = platform_device_register (&csi_device);
	if(ret < 0) {
		printk(KERN_ERR "Cannot Register Pine64 CSI Device\n");
		return -ENODEV;
	}
	ret = platform_driver_register (&csi_cci_driver);
	if(ret < 0) {

		printk(KERN_ERR "Cannot Register Pine64 CSI Driver\n");
		
	}
}


static void __exit csi_cci_exit(void)
{
	printk(KERN_INFO "CSI CCI EXIT\n");
	platform_driver_unregister(&csi_cci_driver);
	platform_device_unregister(&csi_device);

}

module_init(csi_cci_init);
module_exit(csi_cci_exit);
