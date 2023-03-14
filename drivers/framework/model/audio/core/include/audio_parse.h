/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_PARSE_H
#define AUDIO_PARSE_H

#include "audio_host.h"
#include "audio_control.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AUDIO_CONFIG_MAX_ITEM 500
#define AUDIO_CTRL_LIST_MAX 10
#define AUDIO_SAPM_COMP_NAME_LIST_MAX 9
#define AUDIO_SAPM_CFG_NAME_LIST_MAX 4

enum AudioRegOpsType {
    AUDIO_RSET_GROUP = 0,
    AUDIO_INIT_GROUP,
    AUDIO_CTRL_PATAM_GROUP,
    AUDIO_CTRL_SAPM_PATAM_GROUP,
    AUDIO_DAI_STARTUP_PATAM_GROUP,
    AUDIO_DAI_PATAM_GROUP,
    AUDIO_DAI_TRIGGER_GROUP,
    AUDIO_CTRL_CFG_GROUP,
    AUDIO_SAPM_COMP_GROUP,
    AUDIO_SAPM_CFG_GROUP,
    AUDIO_GROUP_MAX
};

struct AudioIdInfo {
    const char *chipName;
    uint32_t chipIdRegister;
    uint32_t chipIdSize;
};

struct AudioControlConfig {
    uint8_t arrayIndex;
    uint8_t iface;
    uint8_t enable;
};

struct AudioAddrConfig {
    uint32_t addr;
    uint32_t value;
};

struct AudioSapmCtrlConfig {
    uint8_t sapmType;
    uint8_t compNameIndex;
    uint8_t reg;
    uint8_t mask;
    uint8_t shift;
    uint8_t invert;
    uint8_t kcontrolNews;
    uint8_t kcontrolsNum;
};


struct AudioRegCfgGroupNode {
    uint8_t itemNum;
    enum AudioRegOpsType groupIndex;
    struct AudioAddrConfig *addrCfgItem;
    struct AudioMixerControl *regCfgItem;
    struct AudioControlConfig *ctrlCfgItem;
    struct AudioSapmCtrlConfig *sapmCompItem;
};

struct AudioRegCfgData {
    struct AudioIdInfo audioIdInfo;
    struct AudioRegCfgGroupNode *audioRegParams[AUDIO_GROUP_MAX];
};

int32_t AudioFillConfigData(const struct HdfDeviceObject *device, struct AudioConfigData *configData);

int32_t CodecGetRegConfig(const struct HdfDeviceObject *device, struct AudioRegCfgData *configData);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_PARSE_H */
