#ifndef CSI_H_
#define CSI_H_



/* CCU Register */
#define PLL_PERIPH0_CTRL_REG 0x28
#define PLL_PERIPH1_CTRL_REG 0x2C
#define BUS_CLK_GATING_REG1 0x64 
#define CSI_MISC_CLK_REG 0x130
#define CSI_CLK_REG 0x134
#define BUS_SOFT_RST_REG1 0x2C4



/* CSI Register */
#define CSI0_EN_REG 0x0
#define CSI0_IF_CFG_REG 0x04
#define CSIO_CAP_REG 0x08
#define CSI0_SYNC_CNT_REG 0x0C
#define CSI0_FIFO_THRS_REG 0x10
#define CSI0_PTN_LEN_REG 0x30
#define CSI0_PTN_ADDR_REG 0x34
#define CSIO_VER_REG 0x3C
#define CSI0_C0_CFG_REG 0x44
#define CSI0_C0_SCALE_REG 0x4C
#define CSI0_C0_F0_BUFA_REG 0x50
#define CSI0_C0_F1_BUFA_REG 0x58
#define CSI0_C0_F1_BUFA_REG 0x60
#define CSI_C0_CAP_STA_REG 0x6C



#define CCI_CTRL 0x3000
#define CCI_CFG 0x3004
#define CCI_FMT 0x3008
#define CCI_BUS_CTRL 0x300C
#define CCI_INT_CTRL 0x3014
#define CCI_LC_TRIG 0x3018
#define CCI_FIFO_ACC 0x3100
#define CCI_RSV_REG 0x3200


struct csi_cci_platform_data {
        void (*enable_csi) (struct csi_cci_platform_data *pdata);
        void (*enable_cci) (struct csi_cci_platform_data *pdata);

};


struct csi_cci_platform_device {
        struct resource *r_mem;
        struct platform_device *pdev;
        void __iomem *csi_base;
        void __iomem *ccu;
        unsigned int virq;
        struct task_struct *log_thread;
};


/* SYSFS Functions called from probe */
int sysfs_init(void);
void sysfs_cleanup(void);


#endif
