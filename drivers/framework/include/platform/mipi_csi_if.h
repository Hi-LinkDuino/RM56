/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup MIPI CSI
 * @{
 *
 * @brief Defines standard MIPI CSI APIs for display driver development.
 *
 * This MIPI CSI module abstracts MIPI CSI capabilities of different system platforms to provide stable APIs
 * for display driver development. You can use this module to obtain/release the MIPI CSI device handle,
 * initialize the MIPI CSI device, and send/receive commands that interact with display peripherals.
 *
 * @since 1.0
 */

/**
 * @file mipi_csi_if.h
 *
 * @brief Declares standard MIPI CSI APIs for display driver development.
 *
 *
 *
 * @since 1.0
 */

#ifndef MIPI_CSI_IF_H
#define MIPI_CSI_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Maximum number of lanes supported by MIPI RX's Mipi device.
 *
 * @since 1.0
 */
#define MIPI_LANE_NUM                4

/**
 * @brief The maximum number of lanes supported by the LVDS device of Mipi Rx.
 *
 * @since 1.0
 */
#define LVDS_LANE_NUM                4

/**
 * @brief Defines the maximum number of virtual channels supported.
 *
 * @since 1.0
 */
#define WDR_VC_NUM                   4

/**
 * @brief Define the number of synchronization codes for each virtual channel of LVDS.
 *
 * @since 1.0
 */
#define SYNC_CODE_NUM                4

/**
 * @brief Maximum 3 groups of extended data types.
 *
 * @since 1.0
 */
#define MAX_EXT_DATA_TYPE_NUM        3

/**
 * @brief Lane distribution of Mipi Rx.
 *
 * @since 1.0
 */
typedef enum {
    LANE_DIVIDE_MODE_0 = 0,
    LANE_DIVIDE_MODE_1 = 1,
    LANE_DIVIDE_MODE_BUTT
} LaneDivideMode;

/**
 * @brief MIPI RX input interface type.
 *
 * @since 1.0
 */
typedef enum {
    /** mipi */
    INPUT_MODE_MIPI    = 0x0,
    /** SUB_LVDS */
    INPUT_MODE_SUBLVDS = 0x1,
    /** LVDS */
    INPUT_MODE_LVDS    = 0x2,
    /* HISPI */
    INPUT_MODE_HISPI   = 0x3,
    /** CMOS */
    INPUT_MODE_CMOS    = 0x4,
    /** BT601 */
    INPUT_MODE_BT601   = 0x5,
    /** BT656 */
    INPUT_MODE_BT656   = 0x6,
    /** BT1120 */
    INPUT_MODE_BT1120  = 0x7,
    /** MIPI Bypass */
    INPUT_MODE_BYPASS  = 0x8,
    INPUT_MODE_BUTT
} InputMode;

/**
 * @brief Mipi Rx, SLVS input rate.
 *
 * @since 1.0
 */
typedef enum {
    /** output 1 pixel per clock */
    MIPI_DATA_RATE_X1 = 0,
    /** output 2 pixel per clock */
    MIPI_DATA_RATE_X2 = 1,
    MIPI_DATA_RATE_BUTT
} MipiDataRate;

/**
 * @brief Mipi crop area properties.
 *
 * @since 1.0
 */
typedef struct {
    int x;
    int y;
    unsigned int width;
    unsigned int height;
} ImgRect;

/**
 * @brief Type of data transmitted.
 *
 * @since 1.0
 */
typedef enum {
    DATA_TYPE_RAW_8BIT = 0,
    DATA_TYPE_RAW_10BIT,
    DATA_TYPE_RAW_12BIT,
    DATA_TYPE_RAW_14BIT,
    DATA_TYPE_RAW_16BIT,
    DATA_TYPE_YUV420_8BIT_NORMAL,
    DATA_TYPE_YUV420_8BIT_LEGACY,
    DATA_TYPE_YUV422_8BIT,
    /** yuv422 8bit transform user define 16bit raw */
    DATA_TYPE_YUV422_PACKED,
    DATA_TYPE_BUTT
} DataType;

/**
 * @brief Define YUV and RAW data format and bit depth.
 *
 * @since 1.0
 */
typedef struct {
    uint8_t devno;
    unsigned int num;
    unsigned int extDataBitWidth[MAX_EXT_DATA_TYPE_NUM];
    unsigned int extDataType[MAX_EXT_DATA_TYPE_NUM];
} ExtDataType;

/**
 * @brief MIPI D-PHY WDR MODE defines
 *
 * @since 1.0
 */
typedef enum {
    HI_MIPI_WDR_MODE_NONE = 0x0,
    /** Virtual Channel */
    HI_MIPI_WDR_MODE_VC   = 0x1,
    /** Data Type */
    HI_MIPI_WDR_MODE_DT   = 0x2,
    /** DOL Mode */
    HI_MIPI_WDR_MODE_DOL  = 0x3,
    HI_MIPI_WDR_MODE_BUTT
} MipiWdrMode;

/**
 * @brief Mipi device properties.
 *
 * @since 1.0
 */
typedef struct {
    /** data type: 8/10/12/14/16 bit */
    DataType inputDataType;
    /** MIPI WDR mode */
    MipiWdrMode wdrMode;
    /** laneId: -1 - disable */
    short laneId[MIPI_LANE_NUM];

    union {
        /** used by the HI_MIPI_WDR_MODE_DT */
        short dataType[WDR_VC_NUM];
    };
} MipiDevAttr;

/**
 * @brief LVDS WDR mode.
 *
 * @since 1.0
 */
typedef enum {
    HI_WDR_MODE_NONE = 0x0,
    HI_WDR_MODE_2F = 0x1,
    HI_WDR_MODE_3F = 0x2,
    HI_WDR_MODE_4F = 0x3,
    HI_WDR_MODE_DOL_2F = 0x4,
    HI_WDR_MODE_DOL_3F = 0x5,
    HI_WDR_MODE_DOL_4F = 0x6,
    HI_WDR_MODE_BUTT
} WdrMode;

/**
 * @brief LVDS synchronization mode.
 *
 * @since 1.0
 */
typedef enum {
    /** sensor SOL, EOL, SOF, EOF */
    LVDS_SYNC_MODE_SOF = 0,
    /** SAV, EAV */
    LVDS_SYNC_MODE_SAV,
    LVDS_SYNC_MODE_BUTT
} LvdsSyncMode;

/**
 * @brief LVDS Vsync type.
 *
 * @since 1.0
 */
typedef enum {
    LVDS_VSYNC_NORMAL   = 0x00,
    LVDS_VSYNC_SHARE    = 0x01,
    LVDS_VSYNC_HCONNECT = 0x02,
    LVDS_VSYNC_BUTT
} LvdsVsyncType;

/**
 * @brief LVDS Vsync column synchronization parameters.
 *
 * @since 1.0
 */
typedef struct {
    LvdsVsyncType syncType;

    /* hconnect vsync blanking len, valid when the syncType is LVDS_VSYNC_HCONNECT */
    unsigned short hblank1;
    unsigned short hblank2;
} LvdsVsyncAttr;

/**
 * @brief Frame ID type.
 *
 * @since 1.0
 */
typedef enum {
    LVDS_FID_NONE    = 0x00,
    /** frame identification id in SAV 4th */
    LVDS_FID_IN_SAV  = 0x01,
    /** frame identification id in first data */
    LVDS_FID_IN_DATA = 0x02,
    LVDS_FID_BUTT
} LvdsFidType;

/**
 * @brief Frame ID configuration information.
 *
 * @since 1.0
 */
typedef struct {
    LvdsFidType fidType;

    /** Sony DOL has the Frame Information Line, in DOL H-Connection mode,
        should configure this flag as false to disable output the Frame Information Line */
    unsigned char outputFil;
} LvdsFidAttr;

/**
 * @brief LVDS bit size end mode.
 *
 * @since 1.0
 */
typedef enum {
    LVDS_ENDIAN_LITTLE = 0x0,
    LVDS_ENDIAN_BIG = 0x1,
    LVDS_ENDIAN_BUTT
} LvdsBitEndian;

/**
 * @brief LVDS / SubLVDS / HiSPi device properties.
 *
 * @since 1.0
 */
typedef struct {
    /** data type: 8/10/12/14 bit */
    DataType inputDataType;
    /** WDR mode */
    WdrMode wdrMode;

    /** sync mode: SOF, SAV */
    LvdsSyncMode syncMode;
    /** normal, share, hconnect */
    LvdsVsyncAttr vsyncAttr;
    /** frame identification code */
    LvdsFidAttr fidAttr;

    /** data endian: little/big */
    LvdsBitEndian dataEndian;
    /** sync code endian: little/big */
    LvdsBitEndian syncCodeEndian;
    /** laneId: -1 - disable */
    short laneId[LVDS_LANE_NUM];

    /** each vc has 4 params, syncCode[i]:
        syncMode is SYNC_MODE_SOF: SOF, EOF, SOL, EOL
        syncMode is SYNC_MODE_SAV: invalid sav, invalid eav, valid sav, valid eav */
    unsigned short syncCode[LVDS_LANE_NUM][WDR_VC_NUM][SYNC_CODE_NUM];
} LvdsDevAttr;

/**
 * @brief The attribute of the combo device.
 *
 * Since MIPI RX can interface with CSI-2, LVDS, HiSPi and other timing, Mipi Rx is called a combo device.
 *
 * @since 1.0
 */
typedef struct {
    /** device number */
    uint8_t devno;
    /** input mode: MIPI/LVDS/SUBLVDS/HISPI/DC */
    InputMode inputMode;
    MipiDataRate dataRate;
    /** MIPI Rx device crop area (corresponding to the oringnal sensor input image size) */
    ImgRect imgRect;

    union {
        MipiDevAttr mipiAttr;
        LvdsDevAttr lvdsAttr;
    };
} ComboDevAttr;

/**
 * @brief PHY common mode voltage mode.
 *
 * @since 1.0
 */
typedef enum {
    PHY_CMV_GE1200MV = 0x00,
    PHY_CMV_LT1200MV = 0x01,
    PHY_CMV_BUTT
} PhyCmvMode;

/**
 * @brief Obtains the MIPI CSI device handle with a specified channel ID.
 *
 * @param id Indicates the MIPI CSI channel ID.
 *
 * @return Returns the MIPI CSI device handle if the operation is successful; returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
DevHandle MipiCsiOpen(uint8_t id);

/**
 * @brief Releases the MIPI CSI device handle.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 *
 * @since 1.0
 */
void MipiCsiClose(DevHandle handle);

/**
 * @brief Set the parameters of Mipi, CMOS or LVDS camera to the controller.
 *
 * The parameters including working mode, image area, image depth, data rate and physical channel.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param pAttr Indicates the pointer to the attribute.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiSetComboDevAttr(DevHandle handle, ComboDevAttr *pAttr);

/**
 * @brief Set common mode voltage mode.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param devno There are 2 device numbers in total, pointing to 0 or 1.
 * @param cmvMode Common mode voltage mode parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiSetPhyCmvmode(DevHandle handle, uint8_t devno, PhyCmvMode cmvMode);

/**
 * @brief Reset sensor.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param snsResetSource The reset signal line number of sensor is called the reset source of sensor in software.
 * sns is the abbreviation of sensor.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiResetSensor(DevHandle handle, uint8_t snsResetSource);

/**
 * @brief Unreset sensor.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param snsResetSource The reset signal line number of sensor is called the reset source of sensor in software.
 * sns is the abbreviation of sensor.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiUnresetSensor(DevHandle handle, uint8_t snsResetSource);

/**
 * @brief Reset Mipi Rx.
 *
 * Different s32WorkingViNum have different enSnsType.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param comboDev MIPI RX or LVDS device type.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiResetRx(DevHandle handle, uint8_t comboDev);

/**
 * @brief Uneset MIPI RX.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param comboDev MIPI RX or LVDS device type.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiUnresetRx(DevHandle handle, uint8_t comboDev);

/**
 * @brief Set the lane distribution of Mipi Rx.
 *
 * Select the specific mode according to the form of hardware connection.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param laneDivideMode Lane division mode parameters.
 *
 * @since 1.0
 */
int32_t MipiCsiSetHsMode(DevHandle handle, LaneDivideMode laneDivideMode);

/**
 * @brief Enable Mipi clock.
 *
 * Decide whether to use Mipi or LVDS according to the ensnstype parameter
 * passed by the upper layer function.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param comboDev MIPI RX or LVDS device type.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiEnableClock(DevHandle handle, uint8_t comboDev);

/**
 * @brief Disable the clock of Mipi device.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param comboDev MIPI RX or LVDS device type.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDisableClock(DevHandle handle, uint8_t comboDev);

/**
 * @brief Enable the sensor clock on Mipi.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param snsClkSource The clock signal line number of sensor, which is called the clock source of sensor in software.
 * sns is the abbreviation of sensor.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiEnableSensorClock(DevHandle handle, uint8_t snsClkSource);

/**
 * @brief Disable the sensor clock.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param snsClkSource The clock signal line number of sensor, which is called the clock source of sensor in software.
 * sns is the abbreviation of sensor.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiDisableSensorClock(DevHandle handle, uint8_t snsClkSource);

/**
 * @brief Set YUV and RAW data format and bit depth.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param dataType Pointer to image data format.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiSetExtDataType(DevHandle handle, ExtDataType* dataType);

/**
 * @brief Sets additional parameters for a MIPI CSI device.
 *
 * @param handle Indicates the MIPI CSI device handle obtained via {@link MipiCsiOpen}.
 * @param drvData Indicates the pointer to the additional parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiCsiSetDrvData(DevHandle handle, void *drvData);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MIPI_CSI_IF_H */
