
//#define BOOT_INFO_A_ADDR                 (OTA_BOOT_INFO_OFFSET)
#define BOOT_INFO_A_B_SIZE               0x6000
//#define BOOT_INFO_B_ADDR                 (BOOT_INFO_A_ADDR + BOOT_INFO_A_B_SIZE)

#define FLASH_32M_ID                0x19
#define FLASH_16M_ID                0x18
#define OTA_BOOT_INFO_HEAD_LEN      (4 + 4)
#define OTA_BOOT_INFO_BODY_LEN      20
#define RD_DATA_LEN_MAX             200
#define RD_SIGN_LEN_MAX             384

typedef struct {
    int rdDataLen;      /* 研发模式明文长度 */
    unsigned char rdData[RD_DATA_LEN_MAX]; /* 研发模式明文 */
    unsigned char rdSign[RD_SIGN_LEN_MAX]; /* 研发模式签名 */
} RdModeInfo;

typedef struct {
    int headerMagic;    /* Header魔术字 */
    int crcVal;         /* CRC值 */
    int len;            /* 数据长度 */
    int curbootArea;    /* 当前启动区 */
    int upgMode;        /* 升级模式 */
    int quietUpgFlg;    /* 静默升级标记 */
    int timerRebootFlg; /* 定时重启标记 */
    int bootTimes;      /* 当前分区异常启动次数 */
    RdModeInfo rdMode;  /* 研发模式 */
} MiscDataInfo;

#define MISC_HEADER_MAGIC 0x6564636A

typedef enum {
	BOOT_AREA_A = 0,
	BOOT_AREA_B = 1,
} BootAreaVal;

typedef enum {
	UPG_MODE_NORMAL = 0, /* normal模式 */
	UPG_MODE_OTA = 1, /* OTA升级模式 */
} UpgModeVal;

typedef enum {
	NORMAL_OTA = 0,  /* 非静默升级 */
	QUIET_FIRST_STAGE = 1, /* 静默升级第一阶段 */
	QUIET_SECOND_STAGE = 2, /* 静默升级第二阶段 */
} OtaUpgTypeVal;

typedef enum {
	NORMAL_REBOOT = 0,  /* 非定时重启 */
	TIMER_REBOOT = 1,  /* 定时重启 */
} RebootTypeVal;

typedef enum {
	OTA_RESERVED_REBOOT,
	OTA_SWACTIVE_REBOOT,
	OTA_CM33_MAIN_CHECKERR_REBOOT,
	OTA_CM33_MINI_CHECKERR_REBOOT,
	OTA_WATCHDOGRESET_REBOOT,
	OTA_REBOOT_MAX
}ota_reboot_reason;

typedef enum {
	BOOTINFO_ORIGIN,
	BOOTINFO_BACKUP,
	BOOTINFO_INVALID
}bootinfo_block;

typedef enum {
	BOOTINFO_ZONE_0,
	BOOTINFO_ZONE_1,
	BOOTINFO_ZONE_2,
	BOOTINFO_ZONE_3,
	BOOTINFO_ZONE_4,
	BOOTINFO_ZONE_5,
	BOOTINFO_ZONE_MAX
}bootinfo_zone;

typedef enum {
	SECURE_CHECK_BOOT2A,
	SECURE_CHECK_BOOT2B,
	SECURE_CHECK_CM33_MAIN,
	SECURE_CHECK_CM33_MINI,
	SECURE_CHECK_MAX
}secure_check_zone;

typedef enum {
    LINK_BOOT2A,
    LINK_BOOT2B,
    LINK_CM33_MAIN,
    LINK_CM33_MINI,
    BOOT_LINK_MAX
}link_entry;

typedef enum {
    /** No error, everything OK.   */
      ERR_OK         = 0,
    /** parameter error.           */
      ERR_PARAMETER  = -1,
    /** function return error.     */
      ERR_RETURN     = -2,
    /** software exception error.  */
      ERR_SWEXCEPTION  = -3,
}err_enum;

int ota_get_bootinfo_addr(void);
int ota_get_bootinfo(MiscDataInfo *info, bootinfo_block block, bootinfo_zone zone);
bootinfo_zone ota_get_bootinfo_zone_num(bootinfo_block block);
bootinfo_block ota_get_valid_bootinfo_block(void);
int ota_set_bootinfo_to_zoneAB(MiscDataInfo *info);

