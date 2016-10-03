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
#include <linux/kobject.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include "csi.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("skukreti");
MODULE_DESCRIPTION("pine64-csi");

/* Reg Definition */

#define CCU_BASE 0x01C20000
#define CCU_LEN 0x324

#define MIPI_NAME "pine64-csi"

struct csi_cci_platform_device pine64_drv;
struct csi_cci_platform_data pine64_pdata; 


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
		uint32_t reg=0;
		printk(KERN_INFO "CSI CCI Probe\n");
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

		reg = ioread32(pine64_drv.ccu + PLL_PERIPH0_CTRL_REG);
		iowrite32(reg| 1<<18|1<<24, pine64_drv.ccu + PLL_PERIPH0_CTRL_REG);
		pr_info("PLL PER0: %x\n", ioread32(pine64_drv.ccu + PLL_PERIPH0_CTRL_REG));

		reg = ioread32(pine64_drv.ccu + PLL_PERIPH1_CTRL_REG);
		iowrite32(reg| 1<<18|1<<24, pine64_drv.ccu + PLL_PERIPH1_CTRL_REG);
		pr_info("PLL PER1: %x\n", ioread32(pine64_drv.ccu + PLL_PERIPH1_CTRL_REG));

		iowrite32(1<<8, pine64_drv.ccu + BUS_CLK_GATING_REG1);
		pr_info("CSI BUS CTRL: %x\n", ioread32(pine64_drv.ccu + BUS_CLK_GATING_REG1));

		iowrite32(0xffffffff, pine64_drv.ccu + BUS_SOFT_RST_REG1);
		iowrite32(1<<31, pine64_drv.ccu + CSI_MISC_CLK_REG);
		pr_info("CSI MISC: %x\n", ioread32(pine64_drv.ccu + CSI_MISC_CLK_REG));

		iowrite32((1<<31|1<<15 | 1<<9), pine64_drv.ccu + CSI_CLK_REG);
		pr_info("CSI CLK: %x\n", ioread32(pine64_drv.ccu + CSI_CLK_REG ));

		iowrite32(0x01, pine64_drv.csi_base + CSI0_EN_REG);
		pr_info("CSI EN: %x\n", ioread32(pine64_drv.csi_base + CSI0_EN_REG));
		iowrite32(0x01, pine64_drv.csi_base + CCI_CTRL);
		pr_info("CCI_CTRL: %x\n", ioread32(pine64_drv.csi_base + CCI_CTRL ));

		sysfs_init();

		return 0;
}


static int csi_cci_remove (struct platform_device *pdev)
{

		printk(KERN_INFO "CSI CCI Remove \n");
		sysfs_cleanup();
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
