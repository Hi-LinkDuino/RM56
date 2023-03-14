/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_DDC_H
#define HDMI_DDC_H

#include "hdf_base.h"
#include "osal_mutex.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * DDC(Display Data Channel)
 * The DDC channel is used by an HDMI Source to determine the capabilities and characteristics of
 * the Sink by reading the E-EDID data structure.
 * The DDC is also used to exchange point-to-point dynamic data between the Source and the Sink
 * using a new DDC address for the HDMI Status and Control Data Channel(SCDC).
 * In addition, The DDC is also used for HDCP to exchange key selection vector and other information.
 */
#define HDMI_DDC_EDID_DEV_ADDRESS 0xA0
#define HDMI_DDC_SCDC_DEV_ADDRESS 0xA8
#define HDMI_DDC_HDCP_DEV_ADDRESS 0x74

enum HdmiDdcDeviceType {
    HDMI_DDC_DEV_EDID = 0,
    HDMI_DDC_DEV_SCDC = 1,
    HDMI_DDC_DEV_HDCP = 2,
    HDMI_DDC_DEV_BUTT,
};

enum HdmiDdcMode {
    HDMI_DDC_MODE_READ_SINGLE_NO_ACK = 0,
    HDMI_DDC_MODE_READ_SINGLE_ACK = 1,
    HDMI_DDC_MODE_READ_MUTIL_NO_ACK = 2,
    HDMI_DDC_MODE_READ_MUTIL_ACK = 3,
    HDMI_DDC_MODE_READ_SEGMENT_NO_ACK = 4,
    HDMI_DDC_MODE_READ_SEGMENT_ACK = 5,
    HDMI_DDC_MODE_WRITE_MUTIL_NO_ACK = 6,
    HDMI_DDC_MODE_WRITE_MUTIL_ACK = 7,
    HDMI_DDC_MODE_BUTT,
};

struct HdmiDdcCfg {
    enum HdmiDdcDeviceType type;
    enum HdmiDdcMode mode;
    bool readFlag;
    uint32_t devAddr;
    uint8_t offset;
    uint8_t segment;  /* see VESA spec. 1 segment is 256 bytes. */
    uint32_t dataLen;
    uint8_t *data;
};

struct HdmiDdc {
    struct OsalMutex ddcMutex;
    bool init;
    void *priv;
};

int32_t HdmiDdcTransfer(struct HdmiDdc *ddc, struct HdmiDdcCfg *cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_DDC_H */
