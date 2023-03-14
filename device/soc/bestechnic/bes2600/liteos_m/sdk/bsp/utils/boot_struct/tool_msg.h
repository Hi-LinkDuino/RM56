/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#ifndef __TOOL_MSG_H__
#define __TOOL_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_MAGIC_NUMBER                   0xBE57EC1C
#define BOOT_HASH_TYPE_MD5                  1
#define BOOT_HASH_TYPE_SHA256               2
#define BOOT_KEY_TYPE_RSA2048               1
#define BOOT_KEY_TYPE_ECDSA192              2
#define BOOT_KEY_TYPE_ECDSA256              3

#define BOOT_STRUCT_FLAG_SECURE_BOOT        (1 << 0)

#define PREFIX_CHAR                         0xBE

#define RSA_KEY_LEN_V1                      (4 + 256 + 256)
#define RSA_SIG_LEN_V1                      256

#define RSA_KEY_LEN                         (4 + 384 + 384)
#define RSA_SIG_LEN                         384

#define ED25519_KEY_LEN                     32
#define ED25519_SIG_LEN                     64

#define HASH_SHA256_LEN                     32
#define HASH_SHA512_LEN                     64

#define MAX_KEY_LEN_V1                      RSA_KEY_LEN_V1
#define MAX_SIG_LEN_V1                      RSA_SIG_LEN_V1
#define MAX_KEY_LEN                         RSA_KEY_LEN
#define MAX_SIG_LEN                         RSA_SIG_LEN
#define MAX_KEY_HASH_LEN                    HASH_SHA256_LEN

#ifndef MAX_TOOL_MSG_DATA_LEN
// Max msg: 1-byte cmd and 510-byte data(1 + 510)
#ifdef NETWORK_WATCHDOG_SUPPORT
#define MAX_TOOL_MSG_DATA_LEN               255
#else
// Max msg: flash cmd msg with 4-byte address and 16-byte data (1 + 4 + 16)
#define MAX_TOOL_MSG_DATA_LEN               21
#endif
#endif

#define MAX_READ_DATA_LEN                   16
#define MAX_WRITE_DATA_LEN                  16
#define MAX_FLASH_CMD_DATA_LEN              16

#define MIN_SIGNED_CODE_SIZE                32

#define BOOT_STRUCT_V1_OFFSET_TO_SIGN(b)    \
    ((unsigned char *)&((struct boot_struct_v1_t *)(b))->hdr.security)
#define BOOT_STRUCT_V1_LEN_TO_SIGN          \
    ((unsigned long)&((struct boot_struct_v1_t *)0)->sig[0] - \
    (unsigned long)&((struct boot_struct_v1_t *)0)->hdr.security)

#define BOOT_STRUCT_V2_OFFSET_TO_SIGN(b)    \
    ((unsigned char *)&((struct boot_struct_v2_t *)(b))->hdr.security)
#define BOOT_STRUCT_V2_LEN_TO_SIGN          \
    ((unsigned long)&((struct boot_struct_v2_t *)0)->sig[0] - \
    (unsigned long)&((struct boot_struct_v2_t *)0)->hdr.security)

#define SEC_KEY_V2_OFFSET_TO_HASH(b)        \
    ((unsigned char *)&((struct sec_key_struct_v2_t *)(b))->version)
#define SEC_KEY_V2_LEN_TO_HASH              \
    ((unsigned long)&((struct sec_key_struct_v2_t *)0)->hash[0] - \
    (unsigned long)&((struct sec_key_struct_v2_t *)0)->version)

#define BOOT_STRUCT_V3_OFFSET_TO_SIGN(b)       \
    ((unsigned char *)&((struct boot_struct_v3_t *)(b))->security)
#define BOOT_STRUCT_V3_LEN_TO_SIGN             \
    ((unsigned long)&((struct boot_struct_v3_t *)0)->sig[0] - \
    (unsigned long)&((struct boot_struct_v3_t *)0)->security)

#define BOOT_STRUCT_OFFSET_TO_SIGN(b)       \
    ((unsigned char *)&((struct boot_struct_t *)(b))->security)
#define BOOT_STRUCT_LEN_TO_SIGN             \
    ((unsigned long)&((struct boot_struct_t *)0)->sig[0] - \
    (unsigned long)&((struct boot_struct_t *)0)->security)

#define SIG_MSG_OVERHEAD                    8
#define SIG_MSG_V1_EXTRA_DATA_LEN           (sizeof(struct boot_struct_v1_t) + sizeof(struct code_sig_struct_v1_t))
#define SIG_MSG_V1_TOTAL_LEN                (SIG_MSG_OVERHEAD + SIG_MSG_V1_EXTRA_DATA_LEN)
#define SIG_MSG_V2_EXTRA_DATA_LEN           (sizeof(struct boot_struct_v2_t) + sizeof(struct code_sig_struct_v2_t))
#define SIG_MSG_V2_TOTAL_LEN                (SIG_MSG_OVERHEAD + SIG_MSG_V2_EXTRA_DATA_LEN)
#define SIG_MSG_V3_EXTRA_DATA_LEN           (sizeof(struct boot_struct_v3_t) + sizeof(struct code_sig_struct_v3_t))
#define SIG_MSG_V3_TOTAL_LEN                (SIG_MSG_OVERHEAD + SIG_MSG_V3_EXTRA_DATA_LEN)
#define SIG_MSG_EXTRA_DATA_LEN              (sizeof(struct boot_struct_t) + sizeof(struct code_sig_struct_t))
#define SIG_MSG_TOTAL_LEN                   (SIG_MSG_OVERHEAD + SIG_MSG_EXTRA_DATA_LEN)

#define CODE_MSG_OVERHEAD                   8
#define BURN_DATA_MSG_OVERHEAD              16

#define SECTOR_SIZE_64K                     (1 << 16)
#define SECTOR_SIZE_32K                     (1 << 15)
#define SECTOR_SIZE_16K                     (1 << 14)
#define SECTOR_SIZE_4K                      (1 << 12)

#define MSG_TOTAL_LEN(msg)                  (sizeof(struct msg_hdr_t) + ((struct msg_hdr_t *)(msg))->len + 1)

enum MSG_TYPE {
    TYPE_SYS = 0x00,
    TYPE_READ = 0x01,
    TYPE_WRITE = 0x02,
    TYPE_BULK_READ = 0x03,
#if defined(TEST_OVER_THE_AIR_ENANBLED)
    TYPE_EXT_WRITE = 0x04,
    TYPE_BULK_WRITE_START = 0x05,
    TYPE_BULK_WRITE_DATA = 0x06,
#endif
    TYPE_NOTIF = 0x10,
    TYPE_SYNC = 0x50,
    TYPE_SIG_INFO = 0x51,
    TYPE_SIG = 0x52,
    TYPE_CODE_INFO = 0x53,
    TYPE_CODE = 0x54,
    TYPE_RUN = 0x55,
    TYPE_SECTOR_SIZE = 0x60,
    TYPE_ERASE_BURN_START = 0x61,
    TYPE_ERASE_BURN_DATA = 0x62,
    TYPE_OBSOLETED_63 = 0x63,
    TYPE_OBSOLETED_64 = 0x64,
    TYPE_FLASH_CMD = 0x65,
    TYPE_GET_SECTOR_INFO = 0x66,
    TYPE_SEC_REG_ERASE_BURN_START = 0x67,
    TYPE_SEC_REG_ERASE_BURN_DATA = 0x68,

#ifdef NETWORK_WATCHDOG_SUPPORT
    // hmos network watchdog
    TYPE_NETWORK_WATCHDOG = 0x70,
#endif

    // Extended types
    TYPE_PROD_TEST = 0x81,
    TYPE_RUNTIME_CMD = 0x82,
    TYPE_BT_CALIB_CMD = 0x83,
    TYPE_PROTO_EL = 0xA0,

    TYPE_INVALID = 0xFF,
};

enum SYS_CMD_TYPE {
    SYS_CMD_REBOOT = 0xF1,
    SYS_CMD_SHUTDOWN = 0xF2,
    SYS_CMD_FLASH_BOOT = 0xF3,
    SYS_CMD_SET_BOOTMODE = 0xE1,
    SYS_CMD_CLR_BOOTMODE = 0xE2,
    SYS_CMD_GET_BOOTMODE = 0xE3,
    SYS_CMD_SET_DLD_RATE = 0xD1,
};

enum ERR_CODE {
    ERR_NONE = 0x00,
    ERR_LEN = 0x01,
    ERR_CHECKSUM = 0x02,
    ERR_NOT_SYNC = 0x03,
    ERR_NOT_SEC = 0x04,
    ERR_SYNC_WORD = 0x05,
    ERR_SYS_CMD = 0x06,
    ERR_DATA_ADDR = 0x07,
    ERR_DATA_LEN = 0x08,
    ERR_ACCESS_RIGHT = 0x09,

    ERR_TYPE_INVALID = 0x0F,

    //ERR_BOOT_OK = 0x10,
    ERR_BOOT_MAGIC = 0x11,
    ERR_BOOT_SEC = 0x12,
    ERR_BOOT_HASH_TYPE = 0x13,
    ERR_BOOT_KEY_TYPE = 0x14,
    ERR_BOOT_KEY_LEN = 0x15,
    ERR_BOOT_SIG_LEN = 0x16,
    ERR_BOOT_SIG = 0x17,
    ERR_BOOT_CRC = 0x18,
    ERR_BOOT_LEN = 0x19,
    ERR_SIG_CODE_SIZE = 0x1A,
    ERR_SIG_SIG_LEN = 0x1B,
    ERR_SIG_INFO_MISSING = 0x1C,
    ERR_BOOT_KEY_ID = 0x1D,
    ERR_BOOT_HASH = 0x1E,

    ERR_CODE_OK = 0x20,
    ERR_BOOT_MISSING = 0x21,
    ERR_CODE_SIZE_SIG = 0x22,
    ERR_CODE_ADDR_SIZE = 0x23,
    ERR_CODE_INFO_MISSING = 0x24,
    ERR_CODE_CRC = 0x25,
    ERR_CODE_SIG = 0x26,

    ERR_CODE_MISSING = 0x31,
    ERR_VERSION = 0x32,

    ERR_BURN_OK = 0x60,
    ERR_SECTOR_SIZE = 0x61,
    ERR_SECTOR_SEQ_OVERFLOW = 0x62,
    ERR_BURN_INFO_MISSING = 0x63,
    ERR_SECTOR_DATA_LEN = 0x64,
    ERR_SECTOR_DATA_CRC = 0x65,
    ERR_SECTOR_SEQ = 0x66,
    ERR_ERASE_FLSH = 0x67,
    ERR_BURN_FLSH = 0x68,
    ERR_VERIFY_FLSH = 0x69,
    ERR_FLASH_CMD = 0x6A,

    ERR_TYPE_MISMATCHED = 0xE1,
    ERR_SEQ_MISMATCHED = 0xE2,
    ERR_BUF_TOO_SMALL = 0xE3,

    ERR_INTERNAL = 0xFF,
};

enum PARSE_STATE {
    PARSE_HEADER,
    PARSE_DATA,
    PARSE_EXTRA,
};

enum SECURE_BOOT_MODE_T {
    SECURE_BOOT_MODE_ROOT_KEY   = 0,
    SECURE_BOOT_MODE_OTP_KEY    = 1,

    SECURE_BOOT_MODE_QTY,
};

enum SECURE_BOOT_SIG_TYPE_T {
    SECURE_BOOT_SIG_RSA         = 0,
    SECURE_BOOT_SIG_ED25519     = 1,

    SECURE_BOOT_SIG_TYPE_QTY,
};

struct msg_hdr_t {
    unsigned char prefix;
    unsigned char type;
    unsigned char seq;
    unsigned char len;
};

struct message_t {
    struct msg_hdr_t hdr;
    // data and 1-byte check_sum
    unsigned char data[MAX_TOOL_MSG_DATA_LEN + 1];
};

struct sec_key_struct_v2_t {
    unsigned int magic;
    unsigned short version;
    unsigned char hash_type;
    unsigned char key_type;
    unsigned short key_len;
    unsigned short sig_len;
    unsigned int reserved[5];
    unsigned char key[MAX_KEY_LEN_V1];
    unsigned char hash[MAX_KEY_HASH_LEN];
};

struct boot_hdr_v1_t {
    unsigned int magic;
    unsigned short security;
    unsigned char hash_type;
    unsigned char key_type;
    unsigned short key_len;
    unsigned short sig_len;
    unsigned int build_info_start;
};

struct boot_struct_v1_t {
    struct boot_hdr_v1_t hdr;
    unsigned char key[MAX_KEY_LEN_V1];
    unsigned char sig[MAX_SIG_LEN_V1];
};

struct code_sig_struct_v1_t {
    unsigned int code_size;
    unsigned short sig_len;
    unsigned short reserved;
    unsigned char sig[MAX_SIG_LEN_V1];
};

struct boot_hdr_v2_t {
    unsigned int magic;
    unsigned short security;
    unsigned short version;
    unsigned int reserved;
    unsigned int build_info_start;
};

struct boot_struct_v2_t {
    struct boot_hdr_v2_t hdr;
    unsigned int reserved[4];
};

struct code_sig_struct_v2_t {
    unsigned int code_size;
    unsigned short sig_len;
    unsigned short reserved;
    unsigned char sig[MAX_SIG_LEN_V1];
};

struct boot_hdr_t {
    unsigned int magic;
    unsigned short flag;
    unsigned short version;
    unsigned int reserved_008;
    unsigned int build_info_start;
};

struct boot_struct_v3_t {
    struct boot_hdr_t hdr;
    unsigned short security;
    unsigned short reserved_012;
    unsigned char key[MAX_KEY_LEN_V1];
    unsigned char sig[MAX_SIG_LEN_V1];
};

struct code_sig_struct_v3_t {
    unsigned int code_size;
    unsigned char sig[MAX_SIG_LEN_V1];
};

struct boot_struct_t {
    struct boot_hdr_t hdr;
    unsigned short security;
    unsigned short reserved_012;
    unsigned char key[MAX_KEY_LEN];
    unsigned char sig[MAX_SIG_LEN];
};

struct code_sig_struct_t {
    unsigned int code_size;
    unsigned char sig[MAX_SIG_LEN];
};

struct code_ver_struct_t {
    unsigned short code_version;
    unsigned short reserved;
};

struct exec_struct_t {
    unsigned int entry;
    unsigned int param;
    unsigned int sp;
    unsigned int exec_addr;
};

struct nand_boot_struct_t {
    unsigned int load_start;
    unsigned int load_end;
};

#ifdef __cplusplus
}
#endif

#endif

