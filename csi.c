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

//#include "csi.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("skukreti");
MODULE_DESCRIPTION("pine64-csi");

/* Reg Definition */

#define CCI_CTRL_REG 0x3000
#define CCU_BASE 0x01C20000
#define CCU_LEN 0x324
#define CSI_MISC 0x130
#define CSI_CLK 0x134

#define MIPI_NAME "pine64-csi"


struct csi_cci_platform_data {
	void (*enable_csi) (struct csi_cci_platform_data *pdata);
	void (*enable_cci) (struct csi_cci_platform_data *pdata);

}pine64_pdata;

struct csi_cci_platform_device {
	struct resource *r_mem;
	struct platform_device *pdev;
	void __iomem *csi_base;
	void __iomem *ccu;
	unsigned int virq;
	struct task_struct *log_thread;

}pine64_drv;


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


void pdev_release (struct device *dev)
{

pr_info("PDEV Release\n");
return;
}


static int csi_cci_probe (struct platform_device *pdev)
{
	printk(KERN_INFO "CSI CCI Probe\n");
	uint32_t reg=0;
	pine64_drv.r_mem = platform_get_resource (pdev, IORESOURCE_MEM, 0);
	pdev->dev.release = pdev_release;
	
	if(!pine64_drv.r_mem)
		pr_err("Resource CSI-IO Resource not availablr\n");

	if(!(request_mem_region(pine64_drv.r_mem->start, resource_size(pine64_drv.r_mem),MIPI_NAME)))
		pr_err("CSI-IO_MEMORY Not Available\n");	
	if(!(request_mem_region(CCU_BASE, CCU_LEN, "CCU-CLK")))
		pr_err("CCU CSI MISC Not available\n");

	pine64_drv.ccu = ioremap_nocache(CCU_BASE, CCU_LEN);
	if(pine64_drv.ccu ==NULL)
		pr_err("Cannot get CCU Memory\n");
	pine64_drv.csi_base = ioremap_nocache(pine64_drv.r_mem->start, resource_size(pine64_drv.r_mem));
	
	reg = ioread32(pine64_drv.ccu + 0x28);
	iowrite32(reg| 1<<18|1<<24, pine64_drv.ccu + 0x28);
	pr_info("PLL PER0: %x\n", ioread32(pine64_drv.ccu + 0x28));

	reg = ioread32(pine64_drv.ccu + 0x2c);
	iowrite32(reg| 1<<18|1<<24, pine64_drv.ccu + 0x2c);
	pr_info("PLL PER1: %x\n", ioread32(pine64_drv.ccu + 0x2c));

	iowrite32(1<<8, pine64_drv.ccu + 0x64);
	pr_info("CSI BUS CTRL: %x\n", ioread32(pine64_drv.ccu + 0x64));
	
	iowrite32(0xffffffff, pine64_drv.ccu+0x2c4);
	iowrite32(1<<31, pine64_drv.ccu + 0x130);
	pr_info("CSI MISC: %x\n", ioread32(pine64_drv.ccu + 0x130));
	
	iowrite32((1<<31|1<<15 | 1<<9), pine64_drv.ccu + 0x134);
	pr_info("CSI CLK: %x\n", ioread32(pine64_drv.ccu + 0x134 ));

	iowrite32(0x01, pine64_drv.csi_base);
	pr_info("CSI EN: %x\n", ioread32(pine64_drv.csi_base));
	iowrite32(0x01, pine64_drv.csi_base + CCI_CTRL_REG);
	pr_info("CCI_CTRL: %x\n", ioread32(pine64_drv.csi_base + CCI_CTRL_REG ));
		

	return 0;
}


static int csi_cci_remove (struct platform_device *pdev)
{

	printk(KERN_INFO "CSI CCI Remove \n");
	iounmap(pine64_drv.csi_base);
	iounmap(pine64_drv.ccu);
	release_mem_region(pine64_drv.r_mem->start, resource_size(pine64_drv.r_mem));
	release_mem_region(CCU_BASE , CCU_LEN);
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
	return 0;
}


static void __exit csi_cci_exit(void)
{
	printk(KERN_INFO "CSI CCI EXIT\n");
	platform_driver_unregister(&csi_cci_driver);
	platform_device_unregister(&csi_device);

}

module_init(csi_cci_init);
module_exit(csi_cci_exit);
