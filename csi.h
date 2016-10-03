#ifndef CSI_H_
#define CSI_H_


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



#if 0
#define ADD_REG(name, offset) (#define CSI_##name##_REF offset)
#define ADD_CCI(name, offset) (#define CCI_## offset)


ADD_REG(EN, 0x0);
ADD_REG(IF_CFG, 0x04);
ADD_REG(CAP, 0x08);
ADD_REG(SYNC_CNT, 0xC);
ADD_REG(FIFO_THRS, 0x10);
ADD_REG(PTN_LEN, 0x30);
ADD_REG(PTN_ADDR, 0x34);
ADD_REG(VER_REG, 0x3C);
ADD_REG(C0_CFG, 0x44);
ADD_REG(C0_SCALE, 0x4C);
ADD_REG(C0_F0_BUFA,0x50);
ADD_REG(C0_F1_BUFA, 0x58);
ADD_REG(C0_F2_BUFA, 0x60);
ADD_REG(C0_CAP_STA, 0x6c);
ADD_REG(C0_INT_EN, 0x70);
ADD_REG(C0_INT_STA, 0x74);
ADD_REG(C0_HSIZE, 0x80);
ADD_REG(C0_VSIZE, 0x84);
ADD_REG(BUF_LEN, 0x88);
ADD_REG(FLIP_SIZE, 0x8c);
ADD_REG(FRM_CLK_CNT, 0x90)l
ADD_REG(C0_ACC_ITNL_CLK_CNT, 0x94);
ADD_REG(C0_FIFO_STAT, 0x98);
ADD_REG(C0_PCLK_STAT, 0x9c);

ADD_CCI(CTRL, 0x3000);
ADD_CCI(CFG, 0x3004);
ADD_CCI(FMT, 0x3008);
ADD_CCI(BUS_CTRL 0x300C);
ADD_CCI(INT_CTRL 0x3014);
ADD_CCI(LC_TRIG 0x3018);
ADD_CCI(FIFO_ACC 0x3100);
ADD_CCI(RSV_REG, 0x3200);

#endif

#endif
