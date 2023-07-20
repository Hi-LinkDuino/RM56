#ifndef __DSI_REG_H__
#define __DSI_REG_H__

#include "plat_types.h"

struct DSI_REG_T {
    __IO uint32_t REG_00;
    __IO uint32_t REG_04;
    __IO uint32_t REG_08;
    __IO uint32_t REG_0C;
    __IO uint32_t REG_10;
    __IO uint32_t REG_14;
    __IO uint32_t REG_18;
    __IO uint32_t REG_1C;
    __IO uint32_t REG_20;
    __IO uint32_t REG_24;
    __IO uint32_t REG_28;
    __IO uint32_t REG_2C;
    __IO uint32_t REG_30;
    __IO uint32_t REG_34;
    __IO uint32_t REG_38;
    __IO uint32_t REG_3C;
    __IO uint32_t REG_40;
    __IO uint32_t REG_44;
    __IO uint32_t REG_48;
};

// reg_00
#define DSI_REG00_LANE_NUM(n)  (((n)&0x3)<<0)
#define DSI_REG00_LPCD_EN      (1<<2)
#define DSI_REG00_LPCD_DLY(n)  (((n)&0x3)<<3)
#define DSI_REG00_HSA_LP       (1<<5)
#define DSI_REG00_HSE_EN       (1<<6)
#define DSI_REG00_HBP_LP       (1<<7)
#define DSI_REG00_HFP_LP       (1<<8)
#define DSI_REG00_EOTP_EN      (1<<9)
#define DSI_REG00_CLANE_LP_EN  (1<<10)
#define DSI_REG00_VIDEO_MODE   (1<<11)
#define DSI_REG00_T_BTA(n)     (((n)&0xF)<<12)
#define DSI_REG00_T_LPX(n)     (((n)&0xF)<<16)
#define DSI_REG00_CLK_T_LPX(n) (((n)&0xF)<<20)
#define DSI_REG00_CLK_PRE(n)   (((n)&0x3)<<24)
#define DSI_REG00_CLK_POST     (((n)&0x3F)<<26)

// reg_04
#define DSI_REG04_HS_EXIT_TIME(n)  (((n)&0x3F)<<0)
#define DSI_REG04_HS_PRPR_TIME(n)  (((n)&0xF)<<6)
#define DSI_REG04_HS_ZERO_TIME(n)  (((n)&0x3F)<<10)
#define DSI_REG04_HS_TRAIL_TIME(n) (((n)&0x3F)<<16)
#define DSI_REG04_HS_T_WAKEUP(n)   (((n)&0x3FF)<<22)

// reg_08
#define DSI_REG08_CLK_EXIT_TIME(n)  (((n)&0x3F)<<0)
#define DSI_REG08_CLK_PRPR_TIME(n)  (((n)&0xF)<<6)
#define DSI_REG08_CLK_ZERO_TIME(n)  (((n)&0x3F)<<10)
#define DSI_REG08_CLK_TRAIL_TIME(n) (((n)&0x3F)<<16)
#define DSI_REG08_CLK_T_WAKEUP(n)   (((n)&0x3FF)<<22)

// reg_0C
#define DSI_REG0C_DTYPE(n)         (((n)&0x3F)<<0)
#define DSI_REG0C_VCID(n)          (((n)&0x3)<<6)
#define DSI_REG0C_VIDEO_PKT_LEN(n) (((n)&0xFFFF)<<8)
#define DSI_REG0C_INPUT_TYPE(n)    (((n)&0x3)<<24)
#define DSI_REG0C_DLANE_AD_TIME(n) (((n)&0x3F)<<26)

// reg_10
#define DSI_REG10_INTR_MASK(n)     (((n)&0xFF)<<24)
#define DSI_REG10_LPRX_RD_EN       (1<<15)
#define DSI_REG10_HS_CMD_RD_EN     (1<<14)

// reg_14
#define DSI_REG14_CMD_TYPE(n)      (((n)&0x7)<<0)
#define DSI_REG14_FLUSH_CMD_PRE    (1<<3)
#define DSI_REG14_ESC_CMD(n)       (((n)&0xFF)<<8)
#define DSI_REG14_SOFT_CMD_LEN(n)  (((n)&0xFFF)<<16)

// reg_18
#define DSI_REG18_HDR_TYPE(n)      (((n)&0xFF)<<0)
#define DSI_REG18_HDR_CMD(n)       (((n)&0xFFFF)<<8)

// reg_40
#define DSI_REG40_HTOTAL(n)        (((n)&0xFFF)<<0)
#define DSI_REG40_HSTART(n)        (((n)&0xFF)<<12)
#define DSI_REG40_HWIDTH(n)        (((n)&0xFFF)<<20)

// reg_44
#define DSI_REG44_VTOTAL(n)        (((n)&0xFFF)<<0)
#define DSI_REG44_VSTART(n)        (((n)&0xFF)<<12)
#define DSI_REG44_VWIDTH(n)        (((n)&0xFFF)<<20)

// reg_48
#define DSI_REG48_CL_DATA_SEL        (1<<0)
#define DSI_REG48_TE_EDGE_SEL        (1<<1)
#define DSI_REG48_VIDEO_SEL          (1<<2)
#define DSI_REG48_VIDEO_BIST_EN      (1<<3)
#define DSI_REG48_VIDEO_BIST_PATT    (((n)&0x7)<<4)
#define DSI_REG48_COLOR_BAR_WIDTH(n) (((n)&0xFFF)<<8)
#define DSI_REG48_HSYNC_DLY_NUM      (((N)&20)<<20)

#define DSI_REG_BASE 0x40040000

#define dsi_reg_write(val, addr) (*(volatile uint32_t *)(addr) = (val))
#define dsi_reg_read(addr) (*(volatile uint32_t *)(addr))

extern struct DSI_REG_T *dsi_reg;

#endif /* __DSI_REG_H__ */

