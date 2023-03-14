/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MMC_PROTOCOL_H
#define MMC_PROTOCOL_H

#include "hdf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CID_LEN 4
#define CID_BITS 128
#define CID_BYTES_LEN 16
#define CID_PNM_LEN 8
#define CID_PNM_START_BIT 96
#define CSD_LEN 4
#define CSD_BITS 128
#define EXT_CSD_BYTES_LEN 512
#define BITS_PER_BYTE 8
#define SCR_LEN 2
#define SCR_BYTES_LEN 8
#define SCR_BITS 64
#define DSR_BITS 16
#define SSR_LEN 16
#define SSR_BYTES_LEN 64
#define SSR_BITS 512

/* The basic unit data transfer is a block whose maximum size is always 512 bytes. */
#define BYTES_PER_BLOCK 512
#define MMC_MAX_BLOCKSIZE_SHIFT 9

#define SDIO_BLOCK_TRANSFER_MAX_BLKNUM 511

/* mmc commands */
enum MmcCmdCode {
    GO_IDLE_STATE = 0,     /* resets all cards to idle state */
    SEND_OP_COND = 1,      /*
                            * asks all cards in idle state to send their operation conditions register contents
                            * in the response on the CMD line.
                            */
    ALL_SEND_CID = 2,      /* asks all cards to send their CID numbers on the CMD line. */
    SET_RELATIVE_ADDR = 3, /* assigns relative address to the card. */
    SET_DSR = 4,           /* programs the DSR of all cards. */
    SWITCH = 6,            /* Switches the mode of operation of the selected card or modifies the EXT_CSD registers. */
    SELECT_CARD = 7,  /*
                       * command toggles a card between the stand-by and transfer states or between the programming and
                       * disconnect states. In both cases the card is selected by its own relative address and gets
                       * deselected by any other address; address 0 deselects all.
                       */
    SEND_EXT_CSD = 8, /* The card sends its EXT_CSD register as a block of data. */
    SEND_CSD = 9,     /* addressed card sends its card-specific data (CSD) on the CMD line. */
    SEND_CID = 10,    /* addressed card sends its card identification (CID) on CMD the line. */
    READ_DAT_UNTIL_STOP = 11, /*
                               * reads data stream from the card, starting at the given address,
                               * until a STOP_TRANSMISSION follows.
                               */
    STOP_TRANSMISSION = 12, /*
                             * Terminates a read/write stream/multiple block operation.
                             * When CMD12 is used to terminate a read transaction the card will respond with R1.
                             * When it is used to stop a write transaction the card will respondwith R1b.
                             */
    SEND_STATUS = 13,       /* addressed card sends its status register. */
    GO_INACTIVE_STATE = 15, /*
                             * sets the card to inactive state in order to protect the card stack against
                             * communication breakdowns.
                             */
    SET_BLOCKLEN = 16,      /*
                             * sets the block length (in bytes) for all following block commands (read and write).
                             * Default block length is specified in the CSD.
                             */
    READ_SINGLE_BLOCK = 17, /* reads a block of the size selected by the SET_BLOCKLEN command. */
    READ_MULTIPLE_BLOCK = 18, /*
                               * continuously transfers data blocks from card to host until interrupted
                               * by a stop command or the requested number of data block transmitted.
                               */
    WRITE_DAT_UNTIL_STOP = 20, /*
                                * writes data stream from the host, starting at the given address,
                                * until a STOP_TRANSMISSION follows.
                                */
    SEND_TUNING_BLOCK_HS200 = 21, /*
                                   * Dedicated for HS200 mode, used to optimize host sampling points.
                                   * The host sends the CMD21 command, and the device sends the data block in tuning
                                   * mode. The host collects data at different sampling points to find the best
                                   * sampling point.
                                   */
    SET_BLOCK_COUNT = 23, /*
                           * Defines the number of blocks which are going to be transferred in the immediately
                           * succeeding multiple block read or write command.
                           */
    WRITE_BLOCK = 24,     /* writes a block of the size selected by the SET_BLOCKLEN command. */
    WRITE_MULTIPLE_BLOCK = 25, /*
                                * continuously writes blocks of data until a STOP_TRANSMISSION follows
                                * or the requested number of block received.
                                */
    PROGRAM_CID = 26, /*
                       * programming of the card identification register. This command shall be issued only once
                       * per card. The card contains hardware to prevent this operation after the first programming.
                       * Normally this command is reserved for the manufacturer.
                       */
    PROGRAM_CSD = 27, /* programming of the programmable bits of the CSD. */
    SET_WRITE_PROT = 28, /*
                          * if the card has write protection features,
                          * this command sets the write protection bit of the addressed group.
                          * The properties of write protection are coded in the card specific data (WP_GRP_SIZE).
                          */
    CLR_WRITE_PROT = 29, /*
                          * if the card provides write protection features,
                          * this command clears the write protection bit of the addressed group.
                          */
    SEND_WRITE_PROT = 30,   /*
                             * if the card provides write protection features,
                             * this command asks the card to send the status of the write protection bits.
                             */
    ERASE_GROUP_START = 35, /* sets the address of the first erase group within a range to be selected for erase. */
    ERASE_GROUP_END = 36,   /*
                             * sets the address of the last erase group within a continuous
                             * range to be selected for erase.
                             */
    MMC_ERASE = 38,   /* erases all previously selected write blocks. */
    MMC_FAST_IO = 39, /*
                       * used to write and read 8 bit (register) data fields. The command addresses a card and
                       * a register and provides the data for writing if the write flag is set. The R4 response
                       * contains data read from the addressed register. This command accesses application
                       * dependent registers which are not defined in the MultiMediaCard standard.
                       */
    GO_IRQ_STATE = 40, /* Sets the system into interrupt mode. */
    LOCK_UNLOCK = 42,  /*
                        * Used to set/reset the password or lock/ unlock the card.
                        * The size of the data block is set by the SET_BLOCK_LEN command.
                        */
    APP_CMD = 55, /*
                   * Indicates to the card that the next command is an application specific command
                   * rather than a standard command.
                   */
    GEN_CMD = 56, /*
                   * Used either to transfer a data block to the card or to get a data block from the card
                   * for general purpose / application specific commands.
                   * The size of the data block shall be set by the SET_BLOCK_LEN command.
                   */
};

#define MMC_CARD_BUSY_STATUS 0x80000000 /* Card Power up status bit */

/* OCR bit definitions */
enum MmcOcrBitsMark {
    MMC_OCR_1V65_1V95 = 0x00000080, /* VDD 1.65V ~ 1.95V */
    MMC_OCR_2V0_2V1 = 0x00000100,   /* VDD 2.0V ~ 2.1V */
    MMC_OCR_2V1_2V2 = 0x00000200,   /* VDD 2.1V ~ 2.2V */
    MMC_OCR_2V2_2V3 = 0x00000400,   /* VDD 2.2V ~ 2.3V */
    MMC_OCR_2V3_2V4 = 0x00000800,   /* VDD 2.3V ~ 2.4V */
    MMC_OCR_2V4_2V5 = 0x00001000,   /* VDD 2.4V ~ 2.5V */
    MMC_OCR_2V5_2V6 = 0x00002000,   /* VDD 2.5V ~ 2.6V */
    MMC_OCR_2V6_2V7 = 0x00004000,   /* VDD 2.6V ~ 2.7V */
    MMC_OCR_2V7_2V8 = 0x00008000,   /* VDD 2.7V ~ 2.8V */
    MMC_OCR_2V8_2V9 = 0x00010000,   /* VDD 2.8V ~ 2.9V */
    MMC_OCR_2V9_3V0 = 0x00020000,   /* VDD 2.9V ~ 3.0V */
    MMC_OCR_3V0_3V1 = 0x00040000,   /* VDD 3.0V ~ 3.1V */
    MMC_OCR_3V1_3V2 = 0x00080000,   /* VDD 3.1V ~ 3.2V */
    MMC_OCR_3V2_3V3 = 0x00100000,   /* VDD 3.2V ~ 3.3V */
    MMC_OCR_3V3_3V4 = 0x00200000,   /* VDD 3.3V ~ 3.4V */
    MMC_OCR_3V4_3V5 = 0x00400000,   /* VDD 3.4V ~ 3.5V */
    MMC_OCR_3V5_3V6 = 0x00800000,   /* VDD 3.5V ~ 3.6V */
};

/* ocr register describe */
union MmcOcr {
    uint32_t ocrData;
    struct OcrBits {
        uint32_t reserved : 7;
        uint32_t vdd1v65To1v95 : 1; /* bit:7  voltage 1.65 ~ 1.95 */
        uint32_t vdd2v0To2v1 : 1;   /* bit:8  voltage 2.0 ~ 2.1 */
        uint32_t vdd2v1To2v2 : 1;   /* bit:9  voltage 2.1 ~ 2.2 */
        uint32_t vdd2v2To2v3 : 1;   /* bit:10  voltage 2.2 ~ 2.3 */
        uint32_t vdd2v3To2v4 : 1;   /* bit:11  voltage 2.3 ~ 2.4 */
        uint32_t vdd2v4To2v5 : 1;   /* bit:12  voltage 2.4 ~ 2.5 */
        uint32_t vdd2v5To2v6 : 1;   /* bit:13  voltage 2.5 ~ 2.6 */
        uint32_t vdd2v6To2v7 : 1;   /* bit:14  voltage 2.6 ~ 2.7 */
        uint32_t vdd2v7To2v8 : 1;   /* bit:15  voltage 2.7 ~ 2.8 */
        uint32_t vdd2v8To2v9 : 1;   /* bit:16  voltage 2.8 ~ 2.9 */
        uint32_t vdd2v9To3v0 : 1;   /* bit:17  voltage 2.9 ~ 3.0 */
        uint32_t vdd3v0To3v1 : 1;   /* bit:18  voltage 3.0 ~ 3.1 */
        uint32_t vdd3v1To3v2 : 1;   /* bit:19  voltage 3.1 ~ 3.2 */
        uint32_t vdd3v2To3v3 : 1;   /* bit:20  voltage 3.2 ~ 3.3 */
        uint32_t vdd3v3To3v4 : 1;   /* bit:21  voltage 3.3 ~ 3.4 */
        uint32_t vdd3v4To3v5 : 1;   /* bit:22  voltage 3.4 ~ 3.5 */
        uint32_t vdd3v5To3v6 : 1;   /* bit:23  voltage 3.5 ~ 3.6 */
        uint32_t s18r : 1;          /* bit:24  switch to 1.8v accepted */
        uint32_t reserved2 : 2;
        uint32_t sdioMemoryPreset : 1; /* bit:27 sdio memory present */
        uint32_t sdXpc : 1;    /* bit:28 XPC for ACMD41 */
        uint32_t sdUhsII : 1;  /* bit:29 UHSII for resp of ACMD41 */
        uint32_t hcs : 1;      /* bit:30 support high capacity  */
        uint32_t busy : 1;     /* bit:31 This bit is set to LOW if the card has not finished the power up routine */
    } bits;
};

/* SEND_STATUS rsp, card status bits */
enum MmcRspCardStatus {
    OUT_OF_RANGE = (1 << 31),  /* The command's argument was out of the allowed range for this card. */
    ADDRESS_ERROR = (1 << 30), /* A misaligned address which did not match the block length was used in the command. */
    BLOCK_LEN_ERROR = (1 << 29), /* The transferred block length is not allowed for this card,
                                  * or the number of transferred bytes does not match the block length.
                                  */
    ERASE_SEQ_ERROR = (1 << 28), /* An error in the sequence of erase commands occurred. */
    ERASE_PARAM = (1 << 27),     /* An invalid selection of erase groups for erase occurred. */
    WP_VIOLATION = (1 << 26),    /* Attempt to program a write protected block. */
    CARD_IS_LOCKED = (1 << 25),  /* When set, signals that the card is locked by the host. */
    LOCK_UNLOCK_FAILED = (1 << 24), /* Set when a sequence or password error has been detected in lock/unlock card
                                     * command or if there was an attempt to access a locked card.
                                     */
    COM_CRC_ERROR = (1 << 23),     /* The CRC check of the previous command failed. */
    ILLEGAL_COMMAND = (1 << 22),   /* Command not legal for the card state. */
    CARD_ECC_FAILED = (1 << 21),   /* Card internal ECC was applied but failed to correct the data. */
    CC_ERROR = (1 << 20),          /* Internal card controller error. */
    GENERAL_ERROR = (1 << 19),     /* A general or an unknown error occurred during the operation. */
    UNDERRUN = (1 << 18),          /* The card could not sustain data transfer in stream read mode. */
    OVERRUN = (1 << 17),           /* The card could not sustain data programming in stream write mode. */
    CID_CSD_OVERWRITE = (1 << 16), /* can be either one of the following errors:
                                    * - The CID register has been already written and can not be overwritten
                                    * - The read only section of the CSD does not match the card content.
                                    * - An attempt to reverse the copy (set as original) or permanent WP (unprotected)
                                    *   bits was made.
                                    */
    WP_ERASE_SKIP = (1 << 15),     /* Only partial address space was erased due to existing write protected blocks. */
    CARD_ECC_DISABLED = (1 << 14), /* The command has been executed without using the internal ECC. */
    ERASE_RESET = (1 << 13),       /* An erase sequence was cleared before executing because an out of erase sequence
                                    * command was received.
                                    */
    READY_FOR_DATA = (1 << 8),     /* corresponds to buffer empty signalling on the bus. */
    SWITCH_ERROR = (1 << 7),       /* If set, the card did not switch to the expected mode as requested by
                                    * the SWITCH command.
                                    */
    URGENT_BKOPS = (1 << 6),       /* If set, device needs to perform background operations urgently.
                                    * Host can check EXT_CSD field BKOPS_STATUS for the detailed level.
                                    */
    IS_APP_CMD = (1 << 5),         /* The card will expect ACMD, or indication that the command has been
                                    * interpreted as ACMD.
                                    */
};

/*
 * The state of the card when receiving the command. If the command execution causes a
 * state change, it will be visible to the host in the response to the next command.
 * The four bits([12:9]) are interpreted as a binary coded number between 0 and 15.
 */
#define MMC_CARD_CURRENT_STATE(x) (((x) & 0x00001E00) >> 9) /* sx, b (4 bits) */
enum MmcCardCurrentState {
    STATE_IDLE = 0,
    STATE_READY = 1,
    STATE_IDENT = 2,
    STATE_STBY = 3,
    STATE_TRAN = 4,
    STATE_DATA = 5,
    STATE_RCV = 6,
    STATE_PRG = 7,
    STATE_DIS = 8,
};

/* addressed (point-to-point) commands (ac) sent on CMD line, response on CMD line. */
#define MMC_CMD_TYPE_AC (0x0 << 5)

/*
 * addressed (point-to-point) data transfer commands (adtc) sent on CMD line, response on CMD line,
 * data transfer on DAT line.
 */
#define MMC_CMD_TYPE_ADTC (0x1 << 5)

/* broadcast commands (bc) sent on CMD line, no response. */
#define MMC_CMD_TYPE_BC (0x2 << 5)

/* broadcast commands with response (bcr) sent on CMD line, response (all cards simultaneously) on CMD line. */
#define MMC_CMD_TYPE_BCR (0x3 << 5)

#define MMC_CMD_RESP_SIZE 4
/* cmd resp type */
#define RESP_PRESENT (1 << 0)
#define RESP_136 (1 << 1)     /* 136 bit response */
#define RESP_CRC (1 << 2)     /* expect valid crc */
#define RESP_BUSY (1 << 3)    /* card may send busy */
#define RESP_CMDCODE (1 << 4) /* response contains opcode */

#define MMC_RESP_NONE 0
#define MMC_RESP_R1 (RESP_PRESENT | RESP_CMDCODE | RESP_CRC)
#define MMC_RESP_R1B (RESP_PRESENT | RESP_CMDCODE | RESP_BUSY | RESP_CRC)
#define MMC_RESP_R2 (RESP_PRESENT | RESP_136 | RESP_CRC)
#define MMC_RESP_R3 (RESP_PRESENT)
#define MMC_RESP_R4 (RESP_PRESENT)
#define MMC_RESP_R5 (RESP_PRESENT | RESP_CMDCODE | RESP_CRC)
#define MMC_RESP_R6 (RESP_PRESENT | RESP_CMDCODE | RESP_CRC)
#define MMC_RESP_R7 (RESP_PRESENT | RESP_CMDCODE | RESP_CRC)

#define MMC_RESP_TYPE(cmd) ((cmd)->respType & (RESP_PRESENT | RESP_136 | RESP_CRC | RESP_BUSY | RESP_CMDCODE))

#define MMC_RESP_SPI_S1 (1 << 7)
#define MMC_RESP_SPI_S2 (1 << 8)
#define MMC_RESP_SPI_B4 (1 << 9)
#define MMC_RESP_SPI_BUSY (1 << 10)

#define MMC_RESP_SPI_R1 (MMC_RESP_SPI_S1)
#define MMC_RESP_SPI_R1B (MMC_RESP_SPI_S1 | MMC_RESP_SPI_BUSY)
#define MMC_RESP_SPI_R2 (MMC_RESP_SPI_S1 | MMC_RESP_SPI_S2)
#define MMC_RESP_SPI_R3 (MMC_RESP_SPI_S1 | MMC_RESP_SPI_B4)
#define MMC_RESP_SPI_R4 (MMC_RESP_SPI_S1 | MMC_RESP_SPI_B4)
#define MMC_RESP_SPI_R5 (MMC_RESP_SPI_S1 | MMC_RESP_SPI_S2)
#define MMC_RESP_SPI_R7 (MMC_RESP_SPI_S1 | MMC_RESP_SPI_B4)

enum MmcCsdStructure {
    MMC_CSD_STRUCTURE_VER_1_0 = 0,
    MMC_CSD_STRUCTURE_VER_1_1 = 1,
    MMC_CSD_STRUCTURE_VER_1_2 = 2,      /* Specification Version 4.1-4.2-4.3 */
    MMC_CSD_STRUCTURE_VER_OTHER = 3,    /* Version is coded int the CSD_STRUCTURE byte in the EXT_CSD register. */
};

enum MmcCsdSpecVersion {
    MMC_CSD_SPEC_VER_0 = 0,
    MMC_CSD_SPEC_VER_1 = 1,
    MMC_CSD_SPEC_VER_2 = 2,
    MMC_CSD_SPEC_VER_3 = 3,
    MMC_CSD_SPEC_VER_4 = 4,
};

enum MmcCsdCardCmdClass {
    MMC_CSD_CCC_BASIC = (1 << 0),
    MMC_CSD_CCC_BLOCK_READ = (1 << 2),
    MMC_CSD_CCC_BLOCK_WRITE = (1 << 4),
    MMC_CSD_CCC_ERASE = (1 << 5),
    MMC_CSD_CCC_WRITE_PROT = (1 << 6),
    MMC_CSD_CCC_LOCK_CARD = (1 << 7),
    MMC_CSD_CCC_APP_SPEC = (1 << 8),
    MMC_CSD_CCC_IO_MODE = (1 << 9),
    MMC_CSD_CCC_SWITCH = (1 << 10),
};

enum MmcBusMode {
    MMC_BUS_MODE_NULL = 0,
    MMC_1_2V_DDR_MODE = 1,
    MMC_1_8V_DDR_MODE = 2,
    MMC_1_2V_SDR_MODE = 3,
    MMC_1_8V_SDR_MODE = 4,
};

/* CID register define */
struct MmcCid {
    uint32_t mid;          /* Manufacturer ID */
    char pnm[CID_PNM_LEN]; /* Product name */
    uint32_t psn;          /* Product serial number */
    uint16_t oid;          /* OEM/Application ID */
    uint16_t year;         /* Manufacturing date: The y field */
    uint8_t month;         /* Manufacturing date: The m field */
    /*
     * The product revision is composed of two Binary Coded Decimal (BCD) digits, four bits each,
     * representing an "n.m" revision number.
     */
    uint8_t hwPrv;         /* Product revision: n */
    uint8_t fwPrv;         /* Product revision: m */
};

/* CSD register define */
struct MmcCsd {
    uint8_t structure;  /* CSD_STRUCTURE: [127:126] */
    uint8_t specVers;   /* emmc-->SPEC_VERS: [125:122] */
    uint16_t ccc;       /* card command classes: [95:84] */
    uint16_t taccClks;  /* data read access-time-2 in CLK(NSAC)[111:104]: The unit for NSAC is 100 clock cycles. */
    uint32_t taccNs;    /* data read access-time-1(TAAC)[119:112]: time unit[2:0] * time value[6:3] */
    uint32_t cSize;     /* device size(C_SIZE): [73:62] */
    uint32_t r2wFactor; /* write speed factor(R2W_FACTOR): [28:26] */
    uint32_t maxDtr;    /* max data transfer rate[103:96]: transfer rate unit[2:0] * time value[6:3] */
    uint32_t eraseSize; /* erase sector size: [45:39], emmc and sd are different. */
    uint32_t readBlkLen;  /* max read data block length(READ_BL_LEN): [83:80] */
    uint32_t writeBlkLen; /* max write data block length(WRITE_BL_LEN): [25:22] */
    uint32_t capacity;    /* see CSD Field C_SIZE. */
    uint32_t rdPartial : 1, /* partial blocks for read allowed: [79] */
        rdMisalign : 1,     /* read block misalignment: [77] */
        wrPartial : 1,      /* partial blocks for write allowed: [21] */
        wrMisalign : 1;     /* write block misalignment: [78] */
};

/* Emmc Extended CSD fields */
enum EmmcExtendedCsdField {
    EMMC_EXT_CSD_ENH_START_ADDR = 136,        /* R/W, 4 bytes */
    EMMC_EXT_CSD_ENH_SIZE_MULT = 140,         /* R/W, 3 bytes */
    EMMC_EXT_CSD_GP_SIZE_MULT = 143,          /* R/W, 12 bytes */
    EMMC_EXT_CSD_PARTITIONS_ATTRIBUTE = 156,  /* R/W */
    EMMC_EXT_CSD_PARTITIONING_SUPPORT = 160,  /* RO */
    EMMC_EXT_CSD_HPI_MGMT = 161,              /* R/W */
    EMMC_EXT_CSD_RST_N_FUNCTION = 162,        /* R/W */
    EMMC_EXT_CSD_WR_REL_PARAM = 166,          /* RO */
    EMMC_EXT_CSD_BOOT_WP = 173,               /* R/W */
    EMMC_EXT_CSD_ERASE_GROUP_DEF = 175,       /* R/W */
    EMMC_EXT_CSD_PARTITION_CONFIG = 179,      /* R/W */
    EMMC_EXT_CSD_ERASED_MEM_CONT = 181,       /* RO */
    EMMC_EXT_CSD_BUS_WIDTH = 183,             /* R/W */
    EMMC_EXT_CSD_STROBE_SUPPORT = 184,        /* RO */
    EMMC_EXT_CSD_HS_TIMING = 185,             /* R/W */
    EMMC_EXT_CSD_POWER_CLASS = 187,           /* R/W */
    EMMC_EXT_CSD_REV = 192,                   /* RO */
    EMMC_EXT_CSD_STRUCTURE = 194,             /* RO */
    EMMC_EXT_CSD_CARD_TYPE = 196,             /* RO */
    EMMC_EXT_CSD_OUT_OF_INTERRUPT_TIME = 198, /* RO */
    EMMC_EXT_CSD_PARTITION_SWITCH_TIME = 199, /* RO */
    EMMC_EXT_CSD_PWR_CL_52_195 = 200,         /* RO */
    EMMC_EXT_CSD_PWR_CL_26_195 = 201,         /* RO */
    EMMC_EXT_CSD_PWR_CL_52_360 = 202,         /* RO */
    EMMC_EXT_CSD_PWR_CL_26_360 = 203,         /* RO */
    EMMC_EXT_CSD_SEC_CNT = 212,               /* RO, 4 bytes */
    EMMC_EXT_CSD_S_A_TIMEOUT = 217,           /* RO */
    EMMC_EXT_CSD_HC_WP_GRP_SIZE = 221,        /* RO */
    EMMC_EXT_CSD_REL_WR_SEC_C = 222,          /* RO */
    EMMC_EXT_CSD_ERASE_TIMEOUT_MULT = 223,    /* RO */
    EMMC_EXT_CSD_HC_ERASE_GRP_SIZE = 224,     /* RO */
    EMMC_EXT_CSD_BOOT_MULTI = 226,            /* RO */
    EMMC_EXT_CSD_SEC_TRIM_MULT = 229,         /* RO */
    EMMC_EXT_CSD_SEC_ERASE_MULT = 230,        /* RO */
    EMMC_EXT_CSD_SEC_FEATURE_SUPPORT = 231,   /* RO */
    EMMC_EXT_CSD_TRIM_MULT = 232,             /* RO */
    EMMC_EXT_CSD_PWR_CL_200_195 = 236,        /* RO */
    EMMC_EXT_CSD_PWR_CL_200_360 = 237,        /* RO */
    EMMC_EXT_CSD_PWR_CL_DDR_52_195 = 238,     /* RO */
    EMMC_EXT_CSD_PWR_CL_DDR_52_360 = 239,     /* RO */
    EMMC_EXT_CSD_HPI_FEATURES = 503,          /* RO */
};

#define EMMC_EXT_CSD_SEC_CNT_BYTES 4
#define EMMC_EXT_CSD_ENH_START_ADDR_BYTES 4
#define EMMC_EXT_CSD_GP_SIZE_MULT_BYTES 12
#define EMMC_EXT_CSD_ENH_SIZE_MULT_BYTES 3

/* PARTITION_CONFIG[179], bit2-bit0: PARTITION_ACCESS. */
#define EMMC_EXT_CSD_PART_CONFIG_ACCESS_MASK (0x7)

enum EmmcExtCsdStructure {
    EMMC_EXT_CSD_STRUCTURE_VER_1_0 = 0,
    EMMC_EXT_CSD_STRUCTURE_VER_1_1 = 1,
    EMMC_EXT_CSD_STRUCTURE_VER_1_2 = 2,      /* Specification Version 4.1-4.2-4.3-4.4 */
    EMMC_EXT_CSD_STRUCTURE_VER_OTHER = 3,    /* Reserved for future use. */
};

enum EmmcExtCsdRev {
    EMMC_EXT_CSD_REV_1_0 = 0,    /* Revision 1.0(for MMC v4.0) */
    EMMC_EXT_CSD_REV_1_1 = 1,    /* Revision 1.1(for MMC v4.1) */
    EMMC_EXT_CSD_REV_1_2 = 2,    /* Revision 1.2(for MMC v4.2) */
    EMMC_EXT_CSD_REV_1_3 = 3,    /* Revision 1.3(for MMC v4.3) */
    EMMC_EXT_CSD_REV_1_4 = 4,    /* Revision 1.4(Obsolete) */
    EMMC_EXT_CSD_REV_1_5 = 5,    /* Revision 1.5(for MMC v4.4.1) */
    EMMC_EXT_CSD_REV_OTHER = 6,  /* Reserved */
};

#define EMMC_EXT_CSD_CARD_TYPE_MASK 0x3F  /* Mask out reserved bits */

enum EmmcExtCsdCardType {
    EMMC_EXT_CSD_CARD_TYPE_26 = 0x01,          /* Card can run at 26MHz */
    EMMC_EXT_CSD_CARD_TYPE_52 = 0x02,          /* Card can run at 52MHz */
    EMMC_EXT_CSD_CARD_TYPE_DDR_1_8V = 0x04,    /* DDR mode @1.8V or 3V I/O, Card can run at 52MHz */
    EMMC_EXT_CSD_CARD_TYPE_DDR_1_2V = 0x08,    /* DDR mode @1.2V I/O, Card can run at 52MHz */
    EMMC_EXT_CSD_CARD_TYPE_DDR_52 = 0x0C,      /* EMMC_EXT_CSD_CARD_TYPE_DDR_1_8V | EMMC_EXT_CSD_CARD_TYPE_DDR_1_2V */
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V = 0x10,    /* Card can run at 200MHz */
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V_ALL = 0x13,
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V_ALL_DDR_1_8V = 0x17,
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V_ALL_DDR_1_2V = 0x1B,
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_8V_ALL_DDR_52 = 0x1F,
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V = 0x20,    /* SDR mode @1.2V I/O, Card can run at 200MHz */
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V_ALL = 0x23,
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V_ALL_DDR_1_8V = 0x27,
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V_ALL_DDR_1_2V = 0x2B,
    EMMC_EXT_CSD_CARD_TYPE_SDR_1_2V_ALL_DDR_52 = 0x2F,
    EMMC_EXT_CSD_CARD_TYPE_SDR_200 = 0x30,
    EMMC_EXT_CSD_CARD_TYPE_SDR_ALL = 0x33,
    EMMC_EXT_CSD_CARD_TYPE_SDR_ALL_DDR_1_8V = 0x37,
    EMMC_EXT_CSD_CARD_TYPE_SDR_ALL_DDR_1_2V = 0x3B,
    EMMC_EXT_CSD_CARD_TYPE_SDR_ALL_DDR_52 = 0x3F,
    EMMC_EXT_CSD_CARD_TYPE_HS400_1_8V = 0x40,  /* Card can run at 200MHz DDR, 1.8V */
    EMMC_EXT_CSD_CARD_TYPE_HS400_1_2V = 0x80,  /* Card can run at 200MHz DDR, 1.2V */
    EMMC_EXT_CSD_CARD_TYPE_HS400 = 0xC0,
    EMMC_EXT_CSD_CARD_TYPE_HS400ES = 0x100,    /* Card can run at HS400ES */
};

enum EmmcExtCsdHighSpeedMaxDtr {
    EMMC_EXT_CSD_HIGH_SPEED_26 = 26000000,   /* 26M */
    EMMC_EXT_CSD_HIGH_SPEED_52 = 52000000,   /* 52M */
    EMMC_EXT_CSD_HIGH_SPEED_200 = 200000000, /* 200M */
};

/* High Priority Interrupt */
enum EmmcExtCsdHpiFeatures {
    EMMC_EXT_CSD_HPI_SUPPORT = (1 << 0),        /* HPI mechanism supported flag. */
    EMMC_EXT_CSD_HPI_IMPLEMENTATION = (1 << 1), /* 0x0 : HPI mechanism implementation based on CMD13.
                                                 * 0x1 : HPI mechanism implementation based on CMD12.
                                                 */
};

/* EXT_CSD access modes */
enum EmmcExtCsdAccessMode {
    EMMC_EXT_CSD_CMD_SET = 0x00, /* The command set is changed according to the Cmd Set field of the argument. */
    EMMC_EXT_CSD_SET_BITS = 0x01, /*
                                   * The bits in the pointed byte are set,
                                   * according to the '1' bits in the Value field.
                                   */
    EMMC_EXT_CSD_CLEAR_BITS = 0x02, /*
                                     * The bits in the pointed byte are cleared,
                                     * according to the '1' bits in the Value field.
                                     */
    EMMC_EXT_CSD_WRITE_BYTE = 0x03, /* The Value field is written into the pointed byte. */
};

enum EmmcExtCsdBusWidth {
    EMMC_EXT_CSD_BUS_WIDTH_1 = 0,             /* 1 bit mode */
    EMMC_EXT_CSD_BUS_WIDTH_4 = 1,             /* 4 bit mode */
    EMMC_EXT_CSD_BUS_WIDTH_8 = 2,             /* 8 bit mode */
    EMMC_EXT_CSD_DDR_BUS_WIDTH_4 = 5,         /* 4 bit DDR mode */
    EMMC_EXT_CSD_DDR_BUS_WIDTH_8 = 6,         /* 8 bit DDR mode */
    EMMC_EXT_CSD_BUS_WIDTH_STROBE = (1 << 7), /* Enhanced strobe mode */
};

enum EmmcExtCsdBusTiming {
    EMMC_EXT_CSD_BUS_TIMING_HS = 1,     /* High speed */
    EMMC_EXT_CSD_BUS_TIMING_HS200 = 2,  /* HS200 */
    EMMC_EXT_CSD_BUS_TIMING_HS400 = 3,  /* HS400 */
    EMMC_EXT_CSD_BUS_DRV_STR_SHIFT = 4, /* Driver Strength shift */
};

enum EmmcExtCsdCmdSet {
    EMMC_EXT_CSD_CMD_SET_NORMAL = (1 << 0),
    EMMC_EXT_CSD_CMD_SET_SECURE = (1 << 1),
    EMMC_EXT_CSD_CMD_SET_CPSECURE = (1 << 2),
};

/*
 * PWR_CL_ff_vvv, PWR_CL_DDR_ff_vvv.
 * Bits [7:4] code the current consumption for the 8 bit bus configuration.
 * Bits [3:0] code the current consumption for the 4 bit bus configuration.
 */
#define EMMC_EXT_CSD_PWR_CL_8BIT_MASK 0xF0 /* 8 bit PWR CLS */
#define EMMC_EXT_CSD_PWR_CL_4BIT_MASK 0x0F /* 8 bit PWR CLS */
#define EMMC_EXT_CSD_PWR_CL_8BIT_SHIFT 4
#define EMMC_EXT_CSD_PWR_CL_4BIT_SHIFT 0

/*
 * S_A_TIMEOUT [217].
 * Max register value defined is 0x17 which equals 838.86ms timeout. Values between 0x18 and 0xFF are reserved.
 */
#define MAX_S_A_TIMEOUT_VALUE 0x17

/*
 * PARTITIONING_SUPPORT [160].
 * Bit[7:2]: Reserved.
 * Bit[1]: ENH_ATTRIBUTE_EN.
 * 0x0: Device can not have enhanced technological features in partitions and user data area.
 * 0x1: Device can have enhanced technological features in partitions and user data area.
 * Bit[0]: PARTITIONING_EN.
 * 0x0: Device does not support partitioning features.
 * 0x1: Device supports partitioning features.
 * Partitioning feature is optional for device.
 */
#define PARTITIONING_SUPPORT_ENH_ATTRIBUTE_EN (1 << 1)
#define PARTITIONING_SUPPORT_PARTITIONING_EN (1 << 0)

/*
 * PARTITIONS_ATTRIBUTE [156].
 * Bit[7:5]: Reserved.
 * Bit[4]: ENH_4. 0x0: Default; 0x1: Set Enhanced attribute in General Purpose partition 4.
 * Bit[3]: ENH_3. 0x0: Default; 0x1: Set Enhanced attribute in General Purpose partition 3.
 * Bit[2]: ENH_2. 0x0: Default; 0x1: Set Enhanced attribute in General Purpose partition 2.
 * Bit[1]: ENH_1. 0x0: Default; 0x1: Set Enhanced attribute in General Purpose partition 1.
 * Bit[0]: ENH_USR. 0x0: Default; 0x1: Set Enhanced attribute in User Data Area.
 */
#define PARTITIONS_ATTRIBUTE_ENH_4   (1 << 4)
#define PARTITIONS_ATTRIBUTE_ENH_3   (1 << 3)
#define PARTITIONS_ATTRIBUTE_ENH_2   (1 << 2)
#define PARTITIONS_ATTRIBUTE_ENH_1   (1 << 1)
#define PARTITIONS_ATTRIBUTE_ENH_USR (1 << 0)

/*
 * The Extended CSD register defines the card properties and selected modes. It is 512 bytes long. The most
 * significant 320 bytes are the Properties segment, which defines the card capabilities and cannot be modified
 * by the host. The lower 192 bytes are the Modes segment, which defines the configuration the card is
 * working in. These modes can be changed by the host by means of the SWITCH command.
 */
struct EmmcExtCsd {
    uint8_t rev;                    /* EXT_CSD_REV: [192] */
    uint8_t eraseGroupDef;          /* ERASE_GROUP_DEF: [175] */
    uint8_t secFeatureSupport;      /* SEC_FEATURE_SUPPORT: [231] */
    uint8_t relWrSecorCount;        /* Reliable write sector count(REL_WR_SEC_C): [222] */
    uint8_t wrRelParam;             /* Write reliability parameter register(WR_REL_PARAM): [166] */
    uint8_t partConfig;             /* PARTITION_CONFIG: [179] */
    uint32_t partSwitchTime;        /* PARTITION_SWITCH_TIME:[199], ms */
    uint32_t saTimeout;             /* Sleep/Awake Timeout = 100ns * 2^S_A_timeout, S_A_TIMEOUT: [217], Units: 100ns */
    uint32_t hsMaxDtr;              /* high-speed max data transfer rate, according to cardType */
    uint32_t sectors;               /* SEC_COUNT: [215:212], 512B/secter */
    uint32_t cardType;              /* see rawCardType */
    uint32_t hcEraseSize;           /* High-capacity erase unit size: [224] */
    uint32_t hcEraseTimeout;        /* High-capacity erase timeout: [223], ms */
    uint32_t secTrimMult;           /* Secure TRIM Multiplier: [229]  */
    uint32_t secEraseMult;          /* Secure Erase Multiplier: [230] */
    uint32_t trimTimeout;           /* TRIM Multiplier: [232], TRIM Timeout = 300ms * TRIM_MULT */
    bool enhAreaEnable;             /* enable bit */
    uint64_t enhAreaOffset;         /* Enhanced User Data Start Address(ENH_START_ADDR): [139:136] */
    uint32_t enhAreaSize;           /* Enhanced User Data Area Size(ENH_SIZE_MULT): [142:140] */
    uint32_t cache_size;            /* Units: KB */
    bool hpiEnable;                 /* HPI enable bit */
    bool hpi;                       /* HPI_FEATURES: [503], Bit[0]: HPI_SUPPORT */
    uint32_t hpiCmd;                /* HPI_FEATURES: [503],  Bit[1]: HPI_IMPLEMENTATION, cmd used as HPI */
    uint32_t dataSectorSize;        /* 512 bytes or 4KB */
    uint32_t data_tag_unit_size;    /* DATA TAG UNIT size */
    uint32_t boot_ro_lock;          /* ro lock support */
    uint32_t bootSize;              /* Boot Partition size = 128Kbytes * BOOT_SIZE_MULT, BOOT_SIZE_MULT: [226] */
    uint8_t pwrClF52V195;           /* Power class for 52MHz at 1.95V: [200] */
    uint8_t pwrClF26V195;           /* Power class for 26MHz at 1.95V: [201] */
    uint8_t pwrClF52V360;           /* Power class for 52MHz at 3.6V: [202] */
    uint8_t pwrClF26V360;           /* Power class for 26MHz at 3.6V: [203] */
    uint8_t pwrClF200V195;          /* Power class for 200MHz at 1.95V: [236] */
    uint8_t pwrClF200V360;          /* Power class for 200MHz at 1.95V: [237] */
    uint8_t pwrClDdrF52V195;        /* Power class for 52MHz, DDR at 3.6V: [238] */
    uint8_t pwrClDdrF52V360;        /* Power class for 52MHz, DDR at 3.6V: [239] */
    uint8_t rawPartitionSupport;    /* PARTITIONING_SUPPORT: [160] */
    uint8_t rawErasedMemCount;      /* ERASED_MEM_CONT: [181] */
    uint8_t strobeSupport;          /* strobe Support:[184] */
    uint8_t rawCsdStructure;        /* CSD_STRUCTURE: [194] */
    uint8_t rawCardType;            /* CARD_TYPE: [196] */
    uint8_t outOfInterruptTime;     /* OUT_OF_INTERRUPT_TIME: [198], Units: 10ms */
    uint8_t rawSaTimeout;           /* S_A_TIMEOUT: [217], 100ns */
    uint8_t rawHcWpGrpSize;         /* High-capacity write protect group size(HC_WP_GRP_SIZE): [221] */
    uint8_t rawEraseTimeoutMult;    /* ERASE_TIMEOUT_MULT: [223] */
    uint8_t rawHcEraseGrpSize;      /* HC_ERASE_GRP_SIZE: [224] */
    uint8_t rawSecTrimMult;         /* SEC_TRIM_MULT: [229] */
    uint8_t rawSecEraseMult;        /* SEC_ERASE_MULT: [230] */
    uint8_t rawSecFeatureSupport;   /* SEC_FEATURE_SUPPORT: [231] */
    uint8_t rawTrimMult;            /* TRIM_MULT: [232] */
    uint8_t rawSectors[EMMC_EXT_CSD_SEC_CNT_BYTES];  /* SEC_COUNT: [215:212] */
};

/* SD_SWITCH_FUNC mode */
#define SD_SWITCH_FUNC_CHECK 0
#define SD_SWITCH_FUNC_SET 1

/* SD_SWITCH_FUNC function groups */
#define SD_SWITCH_FUNC_GRP_ACCESS 0

/* SD_SWITCH_FUNC access modes */
#define SD_SWITCH_FUNC_ACCESS_DEF 0
#define SD_SWITCH_FUNC_ACCESS_HS 1

/*
 * As a response to the switch function command, the SD Memory Card will send R1 response on the CMD line,
 * and 512 bits of status on the DAT lines.
 */
#define SD_SWITCH_FUNCTION_STATUS_BYTES_LEN 64

enum SdCmdCode {
    SD_CMD_SEND_RELATIVE_ADDR = 3, /* ask the card to publish a new relative address (RCA). */
    SD_CMD_SWITCH_FUNC = 6, /* Checks switchable function (mode 0) and switches card function (mode 1). */
    SD_CMD_SEND_IF_COND = 8, /*
                              * Sends SD Memory Card interface condition, which includes host supply voltage
                              * information and asks the card whether card supports voltage.
                              */
    SD_CMD_SWITCH_VOLTAGE = 11, /* Switch to 1.8V bus signaling level. */
    SD_CMD_SEND_TUNING_BLOCK = 19, /* 64 bytes tuning pattern is sent for SDR50 and SDR104. */
    SD_CMD_ERASE_WR_BLK_START = 32, /* sets the address of the first writeblock to be erased. */
    SD_CMD_ERASE_WR_BLK_END = 33, /* sets the address of the last write block of the continuous range to be erased. */
    SD_ACMD_SET_BUS_WIDTH = 6,  /*
                                 * Defines the data bus width ('00' =1bit or '10' =4 bits bus) to be used for data
                                 * transfer. The allowed data bus widths are given in SCR register.
                                 */
    SD_ACMD_SD_STATUS = 13,     /* Send the SD Memory Card status. */
    SD_ACMD_SEND_NUM_WR_BLKS = 22, /* Send the number of the written (without errors) write blocks. */
    SD_ACMD_OP_COND = 41, /*
                           * Asks the accessed card to send its operating condition register(OCR) content
                           * in the response on the CMD line.
                           */
    SD_ACMD_SEND_SCR = 51, /* Reads the SD Configuration Register(SCR). */
};

enum SdSwitchFuncMode {
    SD_SWITCH_FUNC_MODE_CHECK = 0, /*
                                    * Check function is used to query if the card supports
                                    * a specific function or functions.
                                    */
    SD_SWITCH_FUNC_MODE_SET = 1,   /* Set function is used to switch the functionality of the card. */
};

enum SdSwitchFuncGroup {
    SD_SWITCH_FUNC_GROUP_1 = 0, /* Function Group 1 is defined as Bus Speed Mode switch. */
    SD_SWITCH_FUNC_GROUP_2 = 1, /* Function Group 2 is defined for Command System extension. */
    SD_SWITCH_FUNC_GROUP_3 = 2, /* Function Group 3 is defined as driver strength selection for UHS-I modes. */
    SD_SWITCH_FUNC_GROUP_4 = 3, /* Function Group 4 is defined as current limit switch for each UHS-I mode. */
    SD_SWITCH_FUNC_GROUP_5 = 4, /* reserved. */
    SD_SWITCH_FUNC_GROUP_6 = 5, /* reserved. */
};

struct SdSwitchFuncParam {
    uint32_t mode;
    uint32_t group;
    uint32_t value;
};

enum SdBusSpeedMode {
    SD_BUS_SPEED_MODE_DS = 0,         /* Default Speed up to 25MHz 3.3V signaling */
    SD_BUS_SPEED_MODE_UHS_SDR12 = 0,  /* SDR up to 25MHz 1.8V signaling */
    SD_BUS_SPEED_MODE_HS = 1,         /* High Speed 50MHz 3.3V signaling */
    SD_BUS_SPEED_MODE_UHS_SDR25 = 1,  /* SDR up to 50MHz 1.8V signaling */
    SD_BUS_SPEED_MODE_UHS_SDR50 = 2,  /* SDR up to 100MHz 1.8V signaling */
    SD_BUS_SPEED_MODE_UHS_SDR104 = 3, /* SDR up to 208MHz 1.8V signaling */
    SD_BUS_SPEED_MODE_UHS_DDR50 = 4,  /* DDR up to 50MHz 1.8V signaling */
};

/* sd max data transfer rate */
enum SdMaxDtr {
    SD_HIGH_SPEED_MAX_DTR = 50000000,
    SD_UHS_SDR104_MAX_DTR = 208000000,
    SD_UHS_SDR50_MAX_DTR = 100000000,
    SD_UHS_DDR50_MAX_DTR = 50000000,
    SD_UHS_SDR25_MAX_DTR = 50000000,
    SD_UHS_SDR12_MAX_DTR = 25000000,
};

enum SdDrvType {
    SD_DRV_TYPE_B = 0x01,
    SD_DRV_TYPE_A = 0x02,
    SD_DRV_TYPE_C = 0x04,
    SD_DRV_TYPE_D = 0x08,
};

enum SdMaxCurrentLimitBit {
    SD_MAX_CURRENT_LIMIT_200 = (1 << 0),   /* 200mA bit */
    SD_MAX_CURRENT_LIMIT_400 = (1 << 1),   /* 400mA bit */
    SD_MAX_CURRENT_LIMIT_600 = (1 << 2),   /* 600mA bit */
    SD_MAX_CURRENT_LIMIT_800 = (1 << 3),   /* 800mA bit */
};

enum SdMaxCurrentLimitValue {
    SD_MAX_CURRENT_LIMIT_200_VALUE = 0,   /* 200mA */
    SD_MAX_CURRENT_LIMIT_400_VALUE = 1,   /* 400mA */
    SD_MAX_CURRENT_LIMIT_600_VALUE = 2,   /* 600mA */
    SD_MAX_CURRENT_LIMIT_800_VALUE = 3,   /* 800mA */
};

enum SdScrSpec {
    SD_SCR_SPEC_0 = 0,
    SD_SCR_SPEC_1 = 1,
    SD_SCR_SPEC_2 = 2,
};

enum SdScrBusWidths {
    SD_SCR_BUS_WIDTHS_1 = (1 << 0),
    SD_SCR_BUS_WIDTHS_4 = (1 << 2),
};

#define SD_SCR_SPEED_CLASS_CONTROL_SUPPORT (0x1 << 0)  /* cmd20 */
#define SD_SCR_SET_BLOCK_COUNT_SUPPORT (0x1 << 1)      /* cmd23 */

/* SD Configuration register */
struct SdScr {
    uint8_t sdSpec;
    uint8_t sdSpec3;
    uint8_t sdBusWidths;
    uint8_t cmdSupport;
};

/*
 * SPEED_CLASS Code Field, SEPPD_CLASS : VAL.
 * 0 : 0, 1 : 2, 2 : 4, 3 : 6, 4 : 10, 5-FF : reversed.
 */
enum SdSsrSpeedClass {
    SD_SSR_SPEED_CLASS_0 = 0,
    SD_SSR_SPEED_CLASS_1 = 1,
    SD_SSR_SPEED_CLASS_2 = 2,
    SD_SSR_SPEED_CLASS_3 = 3,
    SD_SSR_SPEED_CLASS_4 = 4,
    SD_SSR_SPEED_CLASS_REV = 5,
};

enum SdSsrSpeedClassVal {
    SD_SSR_SPEED_CLASS_0_VAL = 0,
    SD_SSR_SPEED_CLASS_1_VAL = 2,
    SD_SSR_SPEED_CLASS_2_VAL = 4,
    SD_SSR_SPEED_CLASS_3_VAL = 6,
    SD_SSR_SPEED_CLASS_4_VAL = 10,
    SD_SSR_SPEED_CLASS_REV_VAL = 20,
};

enum SdSsrUhsSpeedGrade {
    SD_SSR_UHS_SPEED_GRADE_0 = 0,  /* Less than 10MB/sec */
    SD_SSR_UHS_SPEED_GRADE_1 = 1,  /* 10MB/sec and above */
    SD_SSR_UHS_SPEED_GRADE_2 = 2,  /* Reversed */
    SD_SSR_UHS_SPEED_GRADE_3 = 3,  /* 30MB/sec and above */
    SD_SSR_UHS_SPEED_GRADE_4 = 4,  /* 4h-Fh, Reversed */
};

/* SD Status register */
struct SdSsr {
    uint8_t speedClass;
    uint8_t auSize;
    uint16_t eraseSize;
    uint8_t eraseTimeout;
    uint8_t eraseOffset;
    uint8_t uhsSpeedGrade;
    uint32_t auValue;
};

union SdSpec3BusMode {
    uint32_t data;
    struct dataBits {
        uint32_t uhsSdr12 : 1;
        uint32_t uhsSdr25 : 1;
        uint32_t uhsSdr50 : 1;
        uint32_t uhsSdr104 : 1;
        uint32_t uhsDdr50 : 1;

        uint32_t reserved : 27;
    } bits;
};

struct SdSwitchCaps {
    enum SdMaxDtr hsMaxDtr;
    enum SdMaxDtr uhsMaxDtr;
    union SdSpec3BusMode sdSpec3BusMode;
    enum SdDrvType sdSpec3DrvType;
    enum SdMaxCurrentLimitBit sdSpec3CurrLimit;
};

enum SdioCmdCode {
    /*
     * It is similar to the operation of ACMD41 for SD memory cards.
     * It is used to inquire about the valtage range needed by the I/O card.
     */
    SDIO_SEND_OP_COND = 5,
    /*
     * It is the simplest means to access a single register within the total 128K of register space in any I/O function,
     * including the common I/O area (CIA). This command reads or writes 1 byte using only 1 command/response pair.
     * A common use is to initialize registers or monitor status values for I/O functions. This command is the fastest
     * means to read or write single I/O registers, as it requires only a single command/response pair.
     */
    SDIO_RW_DIRECT = 52,
    /*
     * This command allows the reading or writing of a large number of I/O registers with a single command.
     * Since this is a data transfer command, it provides the highest possible transfer rate.
     */
    SDIO_RW_EXTENDED = 53,
};

/* Card Common Control Registers (CCCR), Register Name--Address. */
enum SdioCccrAddr {
    CCCR_SDIO_REVISION = 0x00, /* bit3-bit0: CCCR_REVISION, bit7-bit4: SDIO_REVISION. */
    SD_SPECIFICATION_REVISION = 0x01, /* bit3-bit0: SD_REVISION, bit7-bit4: RFU. */
    IO_ENABLE = 0x02, /* bit7-bit1: IOEx, bit0: RFU. */
    IO_READY = 0x03, /* bit7-bit1: IORx, bit0: RFU. */
    INT_ENABLE = 0x04, /* bit7-bit1: IENx, bit0: IENM. */
    INT_PENDING = 0x05, /* bit7-bit1: INTx, bit0: RFU. */
    IO_ABORT = 0x06, /* bit2-bit0: ASx, bit3: RES, bit7-bit4: RFU. */
    BUS_INTERFACE_CONTROL = 0x07, /*
                                   * bit1-bit0: Bus Width, bit2: S8B, bit3: RFU, bit4:RFU, bit5: ECSI,
                                   * bit6: SCSI, bit7: CD Disable.
                                   */
    CARD_CAPBILITY = 0x08, /*
                            * bit0: SDC, bit1: SMB, bit2: SRW, bit3: SBS, bit4: S4MI, bit5: E4MI,
                            * bit6: LSC, bit7: 4BLS.
                            */
    COMMON_CIS_POINTER = 0x09, /* 09h-0Bh, Pointer to card's common CIS. */
    BUS_SUSPEND = 0x0C, /* bit0: BS, bit1: BR, bit7-bit2: RFU. */
    FUNCTION_SELECT = 0x0D, /* bit3-bit0: FSx, bit6-bit4: RFU, bit7: DF. */
    EXEC_FLAG = 0x0E, /* bit7-bit0: EXx. */
    FN0_BLOCK_SIZE = 0x10, /* 11h-10h, I/O block size for Function 0. */
    POWER_CONTROL = 0x12, /* bit0: SMPC, bit1: EMPC, bit7-bit2: RFU. */
    BUS_SPEED_SELECT = 0x13, /* bit0: SHS, bit3-bit1: BSSx, bit7-bit4: RFU. */
    UHS_I_SUPPORT = 0x14, /* bit0: SSDR50, bit1: SSDR104, bit2: SDDR50, bit7-bit3: RFU. */
    DRIVER_STRENGTH = 0x15, /* bit0: SDTA, bit1: SDTC, bit2: SDTD, bit3: RFU, bit4: DTS0, bit5: DTS1, bit7-6: RFU. */
    INTERRUPT_EXTENSION = 0x16, /* bit0: SAI, bit1: EAI, bit7-bit2: RFU. */
};

/* CCCR_SDIO_REVISION-->bit3-bit0: CCCR_REVISION. */
enum SdioCccrRev {
    SDIO_CCCR_VERSION_1_00 = 0x00, /* CCCR/FBR Version 1.00 */
    SDIO_CCCR_VERSION_1_10 = 0x01, /* CCCR/FBR Version 1.10 */
    SDIO_CCCR_VERSION_1_20 = 0x02, /* CCCR/FBR Version 1.20 */
    SDIO_CCCR_VERSION_3_00 = 0x03, /* CCCR/FBR Version 3.00 */
};

/* CCCR_SDIO_REVISION-->bit7-bit4: SDIO_REVISION. */
enum SdioCccrSdioRev {
    SDIO_CCCR_SDIO_REV_1_00 = 0x00, /* SDIO Spec Version 1.00 */
    SDIO_CCCR_SDIO_REV_1_10 = 0x01, /* SDIO Spec Version 1.10 */
    SDIO_CCCR_SDIO_REV_1_20 = 0x02, /* SDIO Spec Version 1.20 */
    SDIO_CCCR_SDIO_REV_2_00 = 0x03, /* SDIO Spec Version 2.00 */
    SDIO_CCCR_SDIO_REV_3_00 = 0x04, /* SDIO Spec Version 3.00 */
};

/* SD_SPECIFICATION_REVISION-->bit3-bit0: SD_REVISION. */
enum SdioCccrSdRev {
    SDIO_CCCR_SD_REV_1_01 = 0x00, /* SD Physical Spec Version 1.01 */
    SDIO_CCCR_SD_REV_1_10 = 0x01, /* SD Physical Spec Version 1.10 */
    SDIO_CCCR_SD_REV_2_00 = 0x02, /* SD Physical Spec Version 2.00 */
    SDIO_CCCR_SD_REV_3_0x = 0x03, /* SD Physical Spev Version 3.0x */
};

/* IO_ABORT-->bit3: RES(I/O Card Reset).
 * Setting the RES to 1 shall cause I/O functions perform a soft reset. This does not affect the current card
 * protocol selection and CD Disable. It is auto cleared, so there is no need to rewrite a value of 0.
 */
#define SDIO_CCCR_RES 0x08

/* BUS_INTERFACE_CONTROL-->bit1-bit0: Bus Width. */
enum SdioCccrWidth {
    SDIO_CCCR_WIDTH_1BIT = 0x00,  /* 1-bit */
    SDIO_CCCR_WIDTH_4BIT = 0x02,  /* 4-bit bus */
    SDIO_CCCR_WIDTH_8BIT = 0x03   /* 8-bit bus(only for embedded SDIO) */
};

/* BUS_INTERFACE_CONTROL-->bit5: ECSI. Enable Continuous SPI Interrupt. */
#define SDIO_CCCR_ECSI 0x20
/* BUS_INTERFACE_CONTROL-->bit6: SCSI. Support Continuous SPI Interrupt. */
#define SDIO_CCCR_SCSI 0x40
/*
 * BUS_INTERFACE_CONTROL-->bit7: CD Disable. Card Detect Disable.
 * Connect[0]/DisConnect[1] the 10K-90K ohm pull-up resistor on CD/DAT[3](pin1) of the card.
 * The pull-up may be used for card detection. This bit shall be set to 1 before issuing CMD53.
 */
#define SDIO_CCCR_CD_DISABLE 0x80

/* CARD_CAPBILITY */
enum SdioCccrCapbility {
    SDIO_CCCR_CAP_SDC = 0x01,  /* Support Direct Command(CMD52). If set, all functions shall accept and execute
                                * the CMD52 while data transfer is underway on the DAT[x] lines.
                                */
    SDIO_CCCR_CAP_SMB = 0x02,  /* Support Multiple Block Transfer. If set, all functions shall accept and execute
                                * CMD53 with the optional block mode bit set.
                                */
    SDIO_CCCR_CAP_SRW = 0x04,  /* Support Read Wait. If set, all functions on the card are able to accept
                                * the wait signal on DAT[2].
                                */
    SDIO_CCCR_CAP_SBS = 0x08,  /* Support Bus Control. If set, all functions except 0 shall accept a request to
                                * suspend operations and resume under host control.
                                */
    SDIO_CCCR_CAP_S4MI = 0x10, /* Support Block Gap Interrupt. Support bit of interrupt between blocks of data
                                * in 4-bit SD mode. If set, the SDIO card is able to signal an interrupt between
                                * blocks while data transfer is in progress.
                                */
    SDIO_CCCR_CAP_E4MI = 0x20, /* Enable Block Gap Interrupt. If set, the card shall generate interrupts
                                * during 4 bit multi-block data transfer.
                                */
    SDIO_CCCR_CAP_LSC = 0x40,  /* Low-Speed Card. If set, it indicates that the SDIO card is a Low-Speed Card.
                                * if this bit is clear, The SDIO card is Full-Speed card.
                                */
    SDIO_CCCR_CAP_4BLS = 0x80, /* 4-bit Mode Support for Low-Speed Card. if the SDIO card is a Low-Speed Card and
                                * it supports 4-bit data transfer, then this bit shall be set.
                                */
};

/* POWER_CONTROL-->bit0: SMPC, bit1: EMPC */
enum SdioCccrPower {
    SDIO_CCCR_POWER_SMPC = 0x01, /* Support Master Power Control. */
    SDIO_CCCR_POWER_EMPC = 0x02, /* Enable Master Power Control. */
};

/* BUS_SPEED_SELECT-->bit0: SHS. Support High-Speed. */
#define SDIO_CCCR_BUS_SPEED_SHS 0x01
/*
 * BUS_SPEED_SELECT-->BSS0. BSS0 = 0, Default Speed(25MHz); BSS0 = 1, High Speed(50MHz).
 * If the card is initialized 3.3V signaling, then BSS0 is effective. When SHS is set to 0,
 * writing to this bit is ignored and always indicates 0. The card operates in High-Speed timing mode
 * with a clock rate up to 50MHz.
 */
#define SDIO_CCCR_BUS_SPEED_EHS 0x02

/*
 * BUS_SPEED_SELECT-->bit3-bit1: BSSx.
 * If the card is initialized 1.8V signaling, then BSS[2:0] is effective. A card that supports UHS-I shall
 * support High Speed mode and SHS shall be set to 1.
 */
enum SdioCccrBusSpeed {
    SDIO_CCCR_BUS_SPEED_SDR12 = 0,   /* Max Clock Freq: 25MHz. */
    SDIO_CCCR_BUS_SPEED_SDR25 = 1,   /* Max Clock Freq: 50MHz. */
    SDIO_CCCR_BUS_SPEED_SDR50 = 2,   /* Max Clock Freq: 100MHz. */
    SDIO_CCCR_BUS_SPEED_SDR104 = 3,  /* Max Clock Freq: 208MHz. */
    SDIO_CCCR_BUS_SPEED_DDR50 = 4,   /* Max Clock Freq: 50MHz. */
};

/* UHS_I_SUPPORT-->bit0: SSDR50, bit1: SSDR104, bit2: SDDR50. */
enum SdioCccrUhsISupport {
    SDIO_CCCR_UHS_I_SSDR50 = (1 << 0),  /* Support SDR50. If this bit set to 1, SDR12 and SDR25 shall be supported. */
    SDIO_CCCR_UHS_I_SSDR104 = (1 << 1), /*
                                         * Support SDR104. If this bit set to 1,
                                         * SDR12, SDR25 and SDR50 shall be supported.
                                         */
    SDIO_CCCR_UHS_I_SDDR50 = (1 << 2),  /*
                                         * Support DDR50. If this bit set to 1,
                                         * SDR12, SDR25 and SDR50 shall be supported.
                                         */
};

/* DRIVER_STRENGTH-->bit0: SDTA, bit1: SDTC, bit2: SDTD, bit3: RFU, bit4: DTS0, bit5: DTS1, bit7-bit6: RFU. */
enum SdioCccrDriveTypeSupport {
    SDIO_CCCR_DRIVE_SDTA = (1 << 0),
    SDIO_CCCR_DRIVE_SDTC = (1 << 1),
    SDIO_CCCR_DRIVE_SDTD = (1 << 2),
};

/* DRIVER_STRENGTH-->bit4: DTS0, bit5: DTS1. */
enum SdioCccrDriveTypeSelect {
    SDIO_CCCR_DTS_TYPE_B = (0 << 4),
    SDIO_CCCR_DTS_TYPE_A = (1 << 4),
    SDIO_CCCR_DTS_YPE_C = (2 << 4),
    SDIO_CCCR_DTS_TYPE_D = (3 << 4),
};

/* INTERRUPT_EXTENSION-->bit0: SAI, bit1: EAI. */
#define SDIO_CCCR_SAI 0x01  /* Support Asynchronous Interrupt. */
#define SDIO_CCCR_EAI 0x02  /* Enable Asynchronous Interrupt. */

/* Function Basic Registers (FBR), Register Name--Address. */
enum SdioFbrAddr {
    SDIO_FBR_STD_FUNCTION_INTERFACE_CODE = 0x00,    /*
                                                     * bit7: CSA enable, bit6: support CSA, bit5-bit4L: RFU,
                                                     * bit3-bit0: Standard SDIO Function Interface Code.
                                                     */
    SDIO_FBR_EXT_STD_FUNCTION_INTERFACE_CODE = 0x01, /* Extended Standard SDIO Function Interface Code. */
    SDIO_FBR_POWER_SELECTION = 0x02, /* bit0: SPS, bit1: EPS, bit3-bit2: RFU, bit7-bit4: PSx. */
    SDIO_FBR_POINTER_CIS = 0x09,     /* n09h-n0Bh: (Card Information Structure)CIS pointer */
    SDIO_FBR_POINTER_CSA = 0x0C,     /* n0Ch-n0Eh: (Code Storage Area)CSA pointer */
    SDIO_FBR_CSA_DATA_ACCESS = 0x0F, /* Data access window to CSA. */
    SDIO_FBR_IO_BLOCK_SIZE = 0x10,   /* I/O block size. */
};

/* The address of FBR is from 00n00h to 00nFFh where n is the function number(1 to 7). */
#define SDIO_FBR_BASE_ADDR(f) ((f) * 0x100)

/* SDIO_FBR_STD_FUNCTION_INTERFACE_CODE-->bit3-bit0: Standard SDIO Function Interface Code. */
enum SdioFbrStdFuncIfCode {
    SDIO_FBR_NO_STD_IF = 0x00, /* No SDIO standard interface supported by this function. */
    SDIO_FBR_STD_UART = 0x01,  /* This function support SDIO standard UART. */
    SDIO_FBR_STD_BLUETOOTH_TYPE_A = 0x02, /* This function support SDIO Bluetooth Type-A standard interface. */
    SDIO_FBR_STD_BLUETOOTH_TYPE_B = 0x03, /* This function support SDIO Bluetooth Type-B standard interface. */
    SDIO_FBR_STD_GPS = 0x04,    /* This function support SDIO GPS standard interface. */
    SDIO_FBR_STD_CAMERA = 0x05, /* This function support SDIO Camera standard interface. */
    SDIO_FBR_STD_PHS = 0x06,    /* This function support SDIO PHS standard interface. */
    SDIO_FBR_STD_WLAN = 0x07,   /* This function support SDIO WLAN interface. */
    SDIO_FBR_STD_EMBEDDED_SDIO_ATA = 0x08,    /* This function support Embedded SDIO-ATA standard interface. */
    SDIO_FBR_STD_BLUETOOTH_TYPE_A_AMP = 0x09, /* This function support SDIO Bluetooth Type-A AMP standard interface. */
    SDIO_FBR_STD_SDIO_IF = 0x0F, /*
                                  * This function support an SDIO standard interface number greater than Eh.
                                  * In this case, the value in SDIO_FBR_EXT_STD_FUNCTION_INTERFACE_CODE identifies
                                  * the standard SDIO interfaces type.
                                  */
};

/* SDIO_FBR_STD_FUNCTION_INTERFACE_CODE-->bit6: support CSA. */
#define SDIO_FBR_SUPPORTS_CSA 0x40
/*
 * SDIO_FBR_STD_FUNCTION_INTERFACE_CODE-->bit7: CSA enable.
 * If this function does not support CSA, then this bit shall be R/O and always read as 0.
 */
#define SDIO_FBR_CSA_ENABLE 0x80

/*
 * SDIO_FBR_POWER_SELECTION-->bit0: SPS(Support Power Selection).
 * SPS = 0: This function has no Power Selection. EPS shall be zero.
 * SPS = 1: This function has 2 Power modes which are selected by EPS.
 * This bit is effective when EMPC = 1 in CCCR and PS[3:0] = 0.
 */
#define SDIO_FBR_POWER_SPS 0x01
/*
 * SDIO_FBR_POWER_SELECTION-->bit1: EPS(Enable Power Selection).
 * Enable (low) Power Selection.
 */
#define SDIO_FBR_POWER_EPS 0x02
/* Sdio function 1-7. */
#define SDIO_MAX_FUNCTION_NUMBER 7

/* SDIO CMD5 response R4, [24]S18A(Switching to 1.8V Accepted). */
#define SDIO_R4_S18A (1 << 24)
/* R4, [27]Memory Present. Set to 1 if the card also contains SD memory. Set to 0 if the card is I/O only. */
#define SDIO_R4_MEMORY_PRESENT (1 << 27)

/* R5: [15:8]Response flags, indicating the status of the SDIO card. */
enum SdioR5CardStatusBits {
    SDIO_R5_COM_CRC_ERROR = (1 << 15),   /* The CRC chack of the previous command failed. */
    SDIO_R5_ILLEGAL_COMMAND = (1 << 14), /* Command not legal for the card State. */
    SDIO_R5_ERROR = (1 << 11),           /* A general or an unknuwn error occurred during the operation. */
    SDIO_R5_FUNCTION_NUMBER = (1 << 9),  /* An invalid function number was requested. */
    SDIO_R5_OUT_OF_RANGE = (1 << 8),     /* The command's argument was out of the allowed range for this card. */
};

/* R5: [13:12]IO_CURRENT_STATE. */
enum SdioCurrentState {
    SDIO_CURRENT_STATE_DIS = 0,  /* Disable. Initialize, Standby and Inactive States(card not selected). */
    SDIO_CURRENT_STATE_CMD = 1,  /* DAT lines free. Command waiting, Executing CMD52 in CMD State. */
    SDIO_CURRENT_STATE_TRN = 2,  /* Transfer. Command executing with data transfer using DAT[0] or DAT[3:0] lines. */
};

/* Card Common Control Registers(CCCR). */
struct SdioCccr {
    uint32_t sdioRev;
    uint32_t sdRev;
    uint32_t multiBlock : 1;
    uint32_t lowSpeed : 1;
    uint32_t lowSpeed4Bit : 1;
    uint32_t highPower : 1;
    uint32_t highSpeed : 1;
    uint32_t disableCd : 1;
};

/*
 * n09-n0B, these three bytes make up a 24-bit pointer(only the lower 17 bits are used) to the start of the
 * Card Information Structure(CIS) that is associated with each function.
 */
#define SDIO_CCCR_CIS_START_ADDR_BYTES 3

/*
 * There are 2 tuples with only a tuple code, the CISTPL_NULL and the CISTPL_END. These tuples do not have any
 * additional bytes. For all other tuples, bytes 1 of each tuple contains a link to the next tuple in the chain.
 * If the link field is 0, the the tuple body is empty.
 * If the link contains FFh, the this tuple is the last tuple in its chain.
 */
enum SdioCisTupleCode {
    SDIO_CIS_TPL_NULL = 0x00,       /* Null tuple. */
    SDIO_CIS_TPL_CHECKSUM = 0x10,   /* Checksum Control. */
    SDIO_CIS_TPL_VERS_1 = 0x15,     /* Level 1 version/product-information. */
    SDIO_CIS_TPL_ALTSTR = 0x16,     /* The Alternate Language String Tuple. */
    SDIO_CIS_TPL_MANFID = 0x20,     /* Manufacturer Identification String Tuple. */
    SDIO_CIS_TPL_FUNCID = 0x21,     /* Function Identification Tuple. */
    SDIO_CIS_TPL_FUNCE = 0x22,      /* Function Extensions. */
    SDIO_CIS_TPL_SDIO_STD = 0x91,   /* Additional information for functions built to support application
                                     * specifications for standard SDIO functions.
                                     */
    SDIO_CIS_TPL_SDIO_EXT = 0x92,   /* Reserved for future use with SDIO devices. */
    SDIO_CIS_TPL_END  = 0xFF,       /* The End-0f-chain Tuple. */
};

#define SDIO_CIS_TPL_MANFID_MIN_SIZE 0x04
#define SDIO_CIS_TPL_FUNCE_COMMON_MIN_SIZE 0x04
#define SDIO_CIS_TPL_FUNCE_FUNCTION_PC_MIN_SIZE 0x1C
#define SDIO_CIS_TPL_FUNCE_FUNCTION_PC_MAX_SIZE 0x2A
#define SDIO_CIS_TPLFE_ENABLE_TIMEOUT_VAL_DEF 100

enum SdioCisTupleFunceType {
    SDIO_CIS_TPL_FUNCE_COMMON = 0,      /* This tuple gives the host common information about the card. */
    SDIO_CIS_TPL_FUNCE_FUNCTION_PC = 1, /* Extended Data 01h is defined for Power Control. */
    SDIO_CIS_TPL_FUNCE_FUNCTION_PS = 2, /* Extended of this tuple indicates that the card supports the Power State. */
};

/* Card Information Structure(CIS). */
struct SdioCis {
    uint16_t vendorId;
    uint16_t deviceId;
    uint16_t blkSize;
    uint32_t maxDtr;
};

/*
 * The Card Information Structure is one or more chains(or linked lists) of data blocks or tuples.
 * Basic Tuple Format: TPL_CODE, TPL_LINK, The tuple body.
 */
struct SdioCisTuple {
    uint8_t tplCode;    /* Tuple code: CISTPL_XXX */
    uint8_t tplLink;    /* TPL_LINK Offset to next tuple in chains. This is the number of bytes in the tuple body. */
    uint8_t tplBody[0]; /* The tuple body(n bytes). */
};

struct SdioCmdParam {
    uint32_t funcNum;
    uint32_t regAddr;
    uint8_t writeData;
    bool incrAddrFlag;
    bool writeflag;
    bool scatterFlag;
    uint32_t scatterLen;
};

struct SdioRwBlockInfo {
    uint32_t addr;
    uint8_t *buf;
    uint32_t size;
    uint32_t scatterLen;
    bool writeFlag;
    bool incrAddrFlag;
    bool scatterFlag;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _MMC_PROTOCOL_H */
