/***************************************************************************
 *
 * Copyright 2015-2022 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __HAL_SDMMC_PRI_H__
#define __HAL_SDMMC_PRI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_dma.h"
#include "hal_sdmmc.h"

/*************************Functional configuration area************************/
#define MMC_WRITE
#define MMC_UHS_SUPPORT
#define MMC_IO_VOLTAGE                  //Close later
//#define MMC_SUPPORTS_TUNING           //Close later
//#define MMC_HS200_SUPPORT             //Close later
//#define MMC_HS400_SUPPORT             //Close later
//#define MMC_HS400_ES_SUPPORT          //Close later
//#define MMC_HW_PARTITIONING           //Close later
//#define CONFIG_CMD_BKOPS_ENABLE       //enable background operations handshake on device
//#define CONFIG_MMC_SPEED_MODE_SET     //default close, user_speed_mode needs to be defined after opening
#define MMC_QUIRKS                      //default close, send cmd retry cnt, open=4, close=0
#define CONFIG_MMC_QUIRKS               //default close
//#define CONFIG_MMC_BROKEN_CD          //default close, Use polling to detect
//#define MMC_TINY                      //default close
//#define DM_MMC                        //default close
//#define DM_REGULATOR                  //default close
//#define CONFIG_MMC_SPI                //default close
//#define BLK                           //default close, block drivers and devices
//#define CONFIG_SPL_BUILD              //default close
//#define CONFIG_SPL_LIBCOMMON_SUPPORT  //default close

#define BLK_VEN_SIZE                        40
#define BLK_PRD_SIZE                        20
#define BLK_REV_SIZE                        8

struct mmc_cmd {
    uint16_t cmdidx;
    uint32_t resp_type;
    uint32_t cmdarg;
    uint32_t response[4];
};

struct mmc_data {
    union {
        char *dest;
        const char *src;
    };
    uint32_t flags;
    uint32_t blocks;
    uint32_t blocksize;
};

struct mmc_config {
    const char *name;
#ifndef DM_MMC
    const struct mmc_ops *ops;
#endif
    uint32_t host_caps;
    uint32_t voltages;
    uint32_t f_min;
    uint32_t f_max;
    uint32_t b_max;
    uint8_t part_type;
#ifdef CONFIG_MMC_PWRSEQ
    struct udevice *pwr_dev;
#endif
};

struct sd_ssr {
    uint32_t au;            /* In sectors */
    uint32_t erase_timeout; /* In milliseconds */
    uint32_t erase_offset;  /* In milliseconds */
};

enum bus_mode {
    MMC_LEGACY,
    MMC_HS,
    SD_HS,
    MMC_HS_52,
    MMC_DDR_52,
    UHS_SDR12,
    UHS_SDR25,
    UHS_SDR50,
    UHS_DDR50,
    UHS_SDR104,
    MMC_HS_200,
    MMC_HS_400,
    MMC_HS_400_ES,
    MMC_MODES_END
};

enum if_type {
    IF_TYPE_UNKNOWN = 0,
    IF_TYPE_IDE,
    IF_TYPE_SCSI,
    IF_TYPE_ATAPI,
    IF_TYPE_USB,
    IF_TYPE_DOC,
    IF_TYPE_MMC,
    IF_TYPE_SD,
    IF_TYPE_SATA,
    IF_TYPE_HOST,
    IF_TYPE_NVME,
    IF_TYPE_EFI_LOADER,
    IF_TYPE_PVBLOCK,
    IF_TYPE_VIRTIO,
    IF_TYPE_EFI_MEDIA,

    IF_TYPE_COUNT,
};

//Identifies the partition table type (ie. MBR vs GPT GUID) signature
enum sig_type {
    SIG_TYPE_NONE,
    SIG_TYPE_MBR,
    SIG_TYPE_GUID,

    SIG_TYPE_COUNT
};

typedef struct {
    uint8_t b[16];
} efi_guid_t __attribute__((aligned(8)));

/****************standard related macros and type definitions******************/
#ifdef CONFIG_SYS_64BIT_LBA
    typedef uint64_t lbaint_t;
    #define LBAFlength ""
#else
    typedef uint32_t lbaint_t;
    #define LBAFlength ""
#endif
#define LBAF "%" LBAFlength "x"
#define LBAFU "%" LBAFlength "u"

struct blk_desc {
    enum if_type    if_type;    /* type of the interface */
    uint32_t devnum;            /* device number */
    uint8_t part_type;          /* partition type */
    uint8_t target;             /* target SCSI ID */
    uint8_t lun;                /* target LUN */
    uint8_t hwpart;             /* HW partition, e.g. for eMMC */
    uint8_t type;               /* device type */
    uint8_t removable;          /* removable device */
#ifdef CONFIG_LBA48
    /* device can use 48bit addr (ATA/ATAPI v7) */
    uint8_t lba48;
#endif
    lbaint_t    lba;            /* number of blocks */
    uint16_t    blksz;          /* block size */
    uint32_t    log2blksz;      /* for convenience: log2(blksz) */
    char        vendor[BLK_VEN_SIZE + 1];   /* device vendor string */
    char        product[BLK_PRD_SIZE + 1];  /* device product number */
    char        revision[BLK_REV_SIZE + 1]; /* firmware revision */
    enum sig_type   sig_type;   /* Partition table signature type */
    union {
        uint32_t mbr_sig;       /* MBR integer signature */
        efi_guid_t guid_sig;    /* GPT GUID Signature */
    };
    lbaint_t (*block_read)(struct blk_desc *block_dev,
                           lbaint_t start,
                           lbaint_t blkcnt,
                           void *buffer);
    lbaint_t (*block_write)(struct blk_desc *block_dev,
                            lbaint_t start,
                            lbaint_t blkcnt,
                            const void *buffer);
    lbaint_t (*block_erase)(struct blk_desc *block_dev,
                            lbaint_t start,
                            lbaint_t blkcnt);
    void        *priv;          /* driver private struct pointer */
};

struct mmc {
#ifndef BLK
    //struct list_head link;
#endif
    const struct mmc_config *cfg;   /* provided configuration */
    uint32_t version;
    void *priv;
    uint32_t has_init;
    uint8_t high_capacity;
    bool clk_disable; /* true if the clock can be turned off */
    bool clk_disable_copy;
    uint32_t bus_width;
    uint32_t clock;
    uint32_t saved_clock;
    enum mmc_voltage signal_voltage;
    uint32_t card_caps;
    uint32_t host_caps;
    uint32_t ocr;
    uint32_t dsr;
    uint32_t dsr_imp;
    uint32_t scr[2];
    uint32_t csd[4];
    uint32_t cid[4];
    uint16_t rca;
    uint8_t part_support;
    uint8_t part_attr;
    uint8_t wr_rel_set;
    uint8_t part_config;
    uint8_t cache_ctrl;
    uint32_t cache_size;
    uint8_t gen_cmd6_time;      /* units: 10 ms */
    uint8_t part_switch_time;   /* units: 10 ms */
    uint32_t tran_speed;
    uint32_t legacy_speed;      /* speed for the legacy mode provided by the card */
    uint32_t read_bl_len;
#ifdef MMC_WRITE
    uint32_t write_bl_len;
    uint32_t erase_grp_size;    /* in 512-byte sectors */
#endif
#ifdef MMC_HW_PARTITIONING
    uint32_t hc_wp_grp_size;    /* in 512-byte sectors */
#endif
#ifdef MMC_WRITE
    struct sd_ssr   ssr;        /* SD status register */
#endif
    uint64_t capacity;
    uint64_t capacity_user;
    uint64_t capacity_boot;
    uint64_t capacity_rpmb;
    uint64_t capacity_gp[4];
#ifndef CONFIG_SPL_BUILD
    uint64_t enh_user_start;
    uint64_t enh_user_size;
#endif
#ifndef BLK
    struct blk_desc block_dev;
#endif
    char op_cond_pending;   /* 1 if we are waiting on an op_cond command */
    char init_in_progress;  /* 1 if we have done mmc_start_init() */
    char preinit;           /* start init as early as possible */
    char ddr_mode;
#ifdef DM_MMC
    struct udevice *dev;    /* Device for this MMC controller */
#ifdef DM_REGULATOR
    struct udevice *vmmc_supply;    /* Main voltage regulator (Vcc)*/
    struct udevice *vqmmc_supply;   /* IO voltage regulator (Vccq)*/
#endif
#endif
    uint8_t *ext_csd;
    uint32_t cardtype;              /* cardtype read from the MMC */
    enum mmc_voltage current_voltage;
    enum bus_mode selected_mode;    /* mode currently used */
    enum bus_mode best_mode;        /* best mode is the supported mode with the
                                      * highest bandwidth. It may not always be the
                                      * operating mode due to limitations when
                                      * accessing the boot partitions
                                      */
    uint32_t quirks;
    uint8_t hs400_tuning;

    enum bus_mode user_speed_mode;  /* input speed mode from user */
};

struct sdmmc_ip_host {
    uint8_t host_id;
    uint32_t clock;
    uint32_t bus_hz;
    uint32_t div;
    uint8_t buswidth;
    uint8_t yield;
    uint8_t volt_switch;
    volatile uint8_t volt_switch_flag;
    uint32_t fifoth_val;
    uint32_t period_st_ns;
    uint32_t final_bus_speed;

    void *ioaddr;
    void *priv;

    struct mmc_config cfg;
    struct mmc *mmc;

    uint8_t dma_en;
    uint8_t dma_ch;
    uint8_t dma_in_use;
    volatile uint8_t sdmmc_dma_lock;
    HAL_DMA_IRQ_HANDLER_T tx_dma_handler;
    HAL_DMA_IRQ_HANDLER_T rx_dma_handler;

    struct HAL_SDMMC_CB_T *callback;
};

#define MMC_CMD_GO_IDLE_STATE               0
#define MMC_CMD_GO_PRE_IDLE_STATE           0
#define MMC_CMD_BOOT_INITIATION             0
#define MMC_CMD_SEND_OP_COND                1
#define MMC_CMD_ALL_SEND_CID                2
#define MMC_CMD_SET_RELATIVE_ADDR           3
#define MMC_CMD_SET_DSR                     4
#define MMC_CMD_SLEEP_AWAKE                 5
#define MMC_CMD_SWITCH                      6
#define MMC_CMD_SELECT_CARD                 7
#define MMC_CMD_DESELECT_CARD               7
#define MMC_CMD_SEND_EXT_CSD                8
#define MMC_CMD_SEND_CSD                    9
#define MMC_CMD_SEND_CID                    10
#define MMC_CMD_READ_DAT_UNTIL_STOP         11
#define MMC_CMD_STOP_TRANSMISSION           12
#define MMC_CMD_SEND_STATUS                 13
#define MMC_CMD_BUS_TEST_R                  14
#define MMC_CMD_GO_INACTIVE_STATE           15
#define MMC_CMD_SEND_TUNING_BLOCK           19
#define MMC_CMD_SET_BLOCKLEN                16
#define MMC_CMD_READ_SINGLE_BLOCK           17
#define MMC_CMD_READ_MULTIPLE_BLOCK         18
#define MMC_CMD_SEND_TUNING_BLOCK_HS200     21
#define MMC_CMD_WRITE_DAT_UNTIL_STOP        20
#define MMC_CMDC_CMD22                      22
#define MMC_CMD_SET_BLOCK_COUNT             23
#define MMC_CMD_WRITE_SINGLE_BLOCK          24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK        25
#define MMC_CMD_PROGRAM_CID                 26
#define MMC_CMD_PROGRAM_CSD                 27
#define MMC_CMD_SET_TIME                    49
#define MMC_CMD_SET_WRITE_PROT              28
#define MMC_CMD_CLR_WRITE_PROT              29
#define MMC_CMD_SEND_WRITE_PROT             30
#define MMC_CMD_SEND_WRITE_PROT_TYPE        31
#define MMC_CMD_CMD32                       32
#define MMC_CMD_CMD33                       33
#define MMC_CMD_CMD34                       34
#define MMC_CMD_ERASE_GROUP_START           35
#define MMC_CMD_ERASE_GROUP_END             36
#define MMC_CMD_CMD37                       37
#define MMC_CMD_ERASE                       38
#define MMC_CMD_FAST_IO                     39
#define MMC_CMD_GO_IRQ_STATE                40
#define MMC_CMD_CMD41                       41
#define MMC_CMD_LOCK_UNLOCK                 42
#define MMC_CMD_CMD43                       43
#define MMC_CMD_APP_CMD                     55
#define MMC_CMD_GEN_CMD                     56
#define MMC_CMD_CMD57                       57
#define MMC_CMD_SPI_READ_OCR                58
#define MMC_CMD_SPI_CRC_ON_OFF              59
#define MMC_CMD_CMD60                       60
#define MMC_CMD_CMD61                       61
#define MMC_CMD_RES_MAN                     62
#define MMC_CMD_CMD63                       63
#define MMC_CMD_CMD50                       50
#define MMC_CMD_CMD51                       51
#define MMC_CMD_CMD52                       52
#define MMC_CMD_PROTOCOL_RD                 53
#define MMC_CMD_PROTOCOL_WR                 54
#define MMC_CMD_QUEUED_TASK_PARAMS          44
#define MMC_CMD_QUEUED_TASK_ADDR            45
#define MMC_CMD_EXECUTE_READ_TASK           46
#define MMC_CMD_EXECUTE_WRITE_TASK          47
#define MMC_CMD_CMDQ_TASK_MGMT              48

int hal_sdmmc_set_clock(struct mmc *mmc, uint32_t clock, bool disable);
int hal_sdmmc_select_mode(struct mmc *mmc, enum bus_mode mode);
int hal_sdmmc_set_bus_width(struct mmc *mmc, uint32_t width);
int hal_sdmmc_ip_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data);
uint32_t hal_sdmmc_get_host_addr(enum HAL_SDMMC_ID_T id);
uint32_t hal_sdmmc_get_ext_csd(enum HAL_SDMMC_ID_T id);

int hal_sdmmc_host_int_enable(struct sdmmc_ip_host *host);
int hal_sdmmc_host_int_disable(struct sdmmc_ip_host *host);

#ifdef __cplusplus
}
#endif

#endif

