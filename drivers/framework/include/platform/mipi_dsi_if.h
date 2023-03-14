/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup MIPI DSI
 * @{
 *
 * @brief Defines standard MIPI DSI APIs for display driver development.
 *
 * This MIPI DSI module abstracts MIPI DSI capabilities of different system platforms to provide stable APIs
 * for display driver development. You can use this module to obtain/release the MIPI DSI device handle,
 * initialize the MIPI DSI device, and send/receive commands that interact with display peripherals.
 *
 * @since 1.0
 */

/**
 * @file mipi_dsi_if.h
 *
 * @brief Declares standard MIPI DSI APIs for display driver development.
 *
 *
 *
 * @since 1.0
 */

#ifndef MIPI_DSI_IF_H
#define MIPI_DSI_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates the supported DSI operation modes.
 *
 * You can specify the operation mode via {@link MipiDsiSetCfg}.
 *
 * @since 1.0
 */
enum DsiMode {
/** Invalid mode */
    DSI_NONE = 0,
/** Video mode */
    DSI_VIDEO_MODE,
/** Command mode */
    DSI_CMD_MODE,
};

/**
 * @brief Enumerates the number of data lanes connected to the DSI device.
 *
 * You can specify the number of data lanes via {@link MipiDsiSetCfg}.
 *
 * @since 1.0
 */
enum DsiLane {
/** One data lane is connected to the display device. */
    DSI_1_LANES = 1,
/** Two data lanes are connected to the display device. */
    DSI_2_LANES,
/** Three data lanes are connected to the display device */
    DSI_3_LANES,
/** Four data lanes are connected to the display device. */
    DSI_4_LANES,
};

/**
 * @brief Enumerates the synchronization modes used for data transmission in Video mode.
 * This enumeration is invalid in Command mode.
 *
 * You can specify the synchronization mode via {@link MipiDsiSetCfg}.
 *
 * @since 1.0
 */
enum DsiBurstMode {
/** Pixel data is transferred efficiently using a time-compressed burst format,
    leaving more time during a scan line for low power (LP) mode and reducing the DSI power consumption. */
    VIDEO_BURST_MODE                      = 0x0,
/** Peripherals can accurately reproduce original video timing, including synchronization pulse widths. */
    VIDEO_NON_BURST_MODE_SYNC_PULSES      = 0x1,
/** Peripherals can accurately reproduce original video timing,
    but accurate synchronization pulse widths are not required. */
    VIDEO_NON_BURST_MODE_SYNC_EVENTS      = 0x2,
};

/**
 * @brief Enumerates the output DSI image data formats.
 *
 * You can specify the image data format via {@link MipiDsiSetCfg}.
 *
 * @since 1.0
 */
enum DsiOutFormat {
/** 16-bit RGB */
    FORMAT_RGB_16_BIT          = 0x0,
/** 18-bit RGB */
    FORMAT_RGB_18_BIT          = 0x1,
/** 24-bit RGB */
    FORMAT_RGB_24_BIT          = 0x2,
/** 8-bit YUV420 */
    FORMAT_YUV420_8_BIT_NORMAL = 0x3,
/** Legacy-version 8-bit YUV420 */
    FORMAT_YUV420_8_BIT_LEGACY = 0x4,
/** 8-bit YUV422 */
    FORMAT_YUV422_8_BIT        = 0x5,
/** Invalid value */
    FORMAT_BUTT
};

/**
 * @brief Defines the MIPI DSI timing information.
 *
 * Before image data transmission, you must specify the timing parameters
 * to ensure that images can be correctly displayed on the screen.
 *
 * @since 1.0
 */
struct DsiTimingInfo {
/** Effective pixels in the X direction, that is, resolution of the peer screen in the X direction */
    uint16_t xPixels;
/** Horizontal sync active (HSA) pixels in the X direction, which is specified based on the peer screen */
    uint16_t hsaPixels;
/** Horizontal back porch (HBP) pixels in the X direction, which is specified based on the peer screen */
    uint16_t hbpPixels;
/** Total number of pixels in the X direction. The value is equal to
    the sum value of <b>xResPixels</b>, <b>hsaPixels</b>, <b>hbpPixels</b>, and <b>hfpPixels</b>. */
    uint16_t hlinePixels;
/** Vertical sync active (VSA) lines in the Y direction, which is specified based on the peer screen */
    uint16_t vsaLines;
/** Vertical back porch (VBP) lines in the Y direction, which is specified based on the peer screen */
    uint16_t vbpLines;
/** Vertical front porch (VFP) lines in the Y direction, which is specified based on the peer screen */
    uint16_t vfpLines;
/** Effective lines in the Y direction, that is, resolution of the peer screen in the Y direction */
    uint16_t ylines;
/** Number of bytes of the command for writing memory.
    This parameter is invalid in Video mode and is set to xPixels in Command mode. */
    uint16_t edpiCmdSize;
};

/**
 * @brief Defines MIPI DSI configuration parameters.
 *
 * Before image data transmission, you must set MIPI DSI configuration parameters to ensure
 * that images can be correctly displayed on the screen.
 *
 * @since 1.0
 */
struct MipiCfg {
/** Number of MIPI DSI lanes connected to the display device */
    enum DsiLane lane;
/** Refresh mode (Video mode or Command mode) supported by the display device */
    enum DsiMode mode;
/** Synchronous mode of the display device, which is valid only in Video mode */
    enum DsiBurstMode burstMode;
/** Format of the output DSI image data. For details, see {@link DsiOutFormat}. */
    enum DsiOutFormat format;
/** MIPI DSI timing information, which is set based on the display device requirements */
    struct DsiTimingInfo timing;
/** MIPI DSI clock (unit: Mbit/s) */
    uint32_t phyDataRate;
/** Pixel clock output by the MIPI DSI (unit: kHz) */
    uint32_t pixelClk;
};

/**
 * @brief Defines a MIPI DSI command.
 *
 *
 *
 * @since 1.0
 */
struct DsiCmdDesc {
/** MIPI data type. For details about available values, see the MIPI protocol. */
    uint16_t dataType;
/** Delay required after the command is sent, in ms */
    uint16_t delay;
/** Length of the transferred data */
    uint16_t dataLen;
/** Pointer to the data to be transferred */
    uint8_t *payload;
};

/**
 * @brief Obtains the MIPI DSI device handle with a specified channel ID.
 *
 * @param id Indicates the MIPI DSI channel ID.
 *
 * @return Returns the MIPI DSI device handle if the operation is successful; returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
DevHandle MipiDsiOpen(uint8_t id);

/**
 * @brief Releases the MIPI DSI device handle.
 *
 * @param handle Indicates the MIPI DSI device handle obtained via {@link MipiDsiOpen}.
 *
 * @since 1.0
 */
void MipiDsiClose(DevHandle handle);

/**
 * @brief Sets configuration parameters for a MIPI DSI device.
 *
 * @param handle Indicates the MIPI DSI device handle obtained via {@link MipiDsiOpen}.
 * @param cfg Indicates the pointer to the configuration parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiSetCfg(DevHandle handle, struct MipiCfg *cfg);

/**
 * @brief Obtains the configuration parameters of a MIPI DSI device.
 *
 * @param handle Indicates the MIPI DSI device handle obtained via {@link MipiDsiOpen}.
 * @param cfg Indicates the pointer to the configuration parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiGetCfg(DevHandle handle, struct MipiCfg *cfg);

/**
 * @brief Sets LP mode for a MIPI DSI device.
 *
 * @param handle Indicates the MIPI DSI device handle obtained via {@link MipiDsiOpen}.
 *
 * @since 1.0
 */
void MipiDsiSetLpMode(DevHandle handle);

/**
 * @brief Sets the high-speed (HS) mode for a MIPI DSI device.
 *
 * @param handle Indicates the MIPI DSI device handle obtained via {@link MipiDsiOpen}.
 *
 * @since 1.0
 */
void MipiDsiSetHsMode(DevHandle handle);

/**
 * @brief Sends a display command set (DCS) command used for sending the initial parameters of a peripheral.
 *
 * @param handle Indicates the MIPI DSI device handle obtained via {@link MipiDsiOpen}.
 * @param cmd Indicates the pointer to the command to be sent.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiTx(DevHandle handle, struct DsiCmdDesc *cmd);

/**
* @brief Receives a DCS command used for reading data, such as the status and parameters of a peripheral
 *
 * @param handle Indicates the MIPI DSI device handle obtained via {@link MipiDsiOpen}.
 * @param cmd Indicates the pointer to the command to be received.
 * @param readLen Indicates the length of the data to read, in bytes.
 * @param out Indicates the pointer to the read data.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiRx(DevHandle handle, struct DsiCmdDesc *cmd, int32_t readLen, uint8_t *out);

/**
* @brief attach a DSI device to its DSI host
 *
 * @param handle Indicates the MIPI DSI device handle obtained via {@link MipiDsiOpen}.
 * @param name Indicates the name of a peripheral.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiAttach(DevHandle handle, uint8_t *name);

/**
 * @brief Sets additional parameters for a MIPI DSI device.
 *
 * @param handle Indicates the MIPI DSI device handle obtained via {@link MipiDsiOpen}.
 * @param panelData Indicates the pointer to the additional parameters.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t MipiDsiSetDrvData(DevHandle handle, void *panelData);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MIPI_DSI_IF_H */
