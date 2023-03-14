/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_CORE_H
#define AUDIO_CORE_H

#include "audio_host.h"
#include "audio_control.h"
#include "audio_codec_if.h"
#include "audio_platform_if.h"
#include "audio_dai_if.h"
#include "audio_accessory_if.h"
#include "audio_dsp_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t AudioDeviceReadReg(unsigned long virtualAddress, uint32_t reg, uint32_t *value);
int32_t AudioDeviceWriteReg(unsigned long virtualAddress, uint32_t reg, uint32_t value);

int32_t AudioSocRegisterPlatform(struct HdfDeviceObject *device, struct PlatformData *platformData);
int32_t AudioSocRegisterDai(struct HdfDeviceObject *device, struct DaiData *daiData);
int32_t AudioRegisterDsp(struct HdfDeviceObject *device, struct DspData *dspData, struct DaiData *DaiData);
int32_t AudioRegisterCodec(struct HdfDeviceObject *device, struct CodecData *codecData, struct DaiData *daiData);
int32_t AudioRegisterAccessory(struct HdfDeviceObject *device,
    struct AccessoryData *accessoryData, struct DaiData *daiData);

int32_t AudioBindDaiLink(struct AudioCard *audioCard, const struct AudioConfigData *configData);

int32_t AudioUpdateCodecRegBits(struct CodecDevice *codec, uint32_t reg,
    const uint32_t mask, const uint32_t shift, uint32_t value);
int32_t AudioUpdateAccessoryRegBits(struct AccessoryDevice *accessory, uint32_t reg,
    const uint32_t mask, const uint32_t shift, uint32_t value);
int32_t AudioUpdateDaiRegBits(const struct DaiDevice *dai, uint32_t reg,
    const uint32_t mask, const uint32_t shift, uint32_t value);

struct DaiDevice *AudioKcontrolGetCpuDai(const struct AudioKcontrol *kcontrol);
struct CodecDevice *AudioKcontrolGetCodec(const struct AudioKcontrol *kcontrol);
struct AccessoryDevice *AudioKcontrolGetAccessory(const struct AudioKcontrol *kcontrol);

int32_t AudioAddControls(struct AudioCard *audioCard,
    const struct AudioKcontrol *controls, int32_t controlMaxNum);
struct AudioKcontrol *AudioAddControl(const struct AudioCard *audioCard, const struct AudioKcontrol *ctl);

int32_t AudioGetCtrlOpsRReg(struct AudioCtrlElemValue *elemValue,
    const struct AudioMixerControl *mixerCtrl, uint32_t rcurValue);
int32_t AudioGetCtrlOpsReg(struct AudioCtrlElemValue *elemValue,
    const struct AudioMixerControl *mixerCtrl, uint32_t curValue);
int32_t AudioSetCtrlOpsReg(const struct AudioKcontrol *kcontrol, const struct AudioCtrlElemValue *elemValue,
    const struct AudioMixerControl *mixerCtrl, uint32_t *value);
int32_t AudioSetCtrlOpsRReg(const struct AudioCtrlElemValue *elemValue,
    struct AudioMixerControl *mixerCtrl, uint32_t *rvalue, bool *updateRReg);
int32_t AudioDaiReadReg(const struct DaiDevice *dai, uint32_t reg, uint32_t *val);
int32_t AudioDaiWriteReg(const struct DaiDevice *dai, uint32_t reg, uint32_t val);

int32_t AudioCodecReadReg(const struct CodecDevice *codec, uint32_t reg, uint32_t *val);
int32_t AudioCodecWriteReg(const struct CodecDevice *codec, uint32_t reg, uint32_t val);
int32_t AudioAccessoryReadReg(const struct AccessoryDevice *accessory, uint32_t reg, uint32_t *val);
int32_t AudioAccessoryWriteReg(const struct AccessoryDevice *accessory, uint32_t reg, uint32_t val);

int32_t AudioInfoCtrlOps(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemInfo *elemInfo);
int32_t AudioCodecGetCtrlOps(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemValue *elemValue);
int32_t AudioCodecSetCtrlOps(const struct AudioKcontrol *kcontrol, const struct AudioCtrlElemValue *elemValue);
int32_t AudioAccessoryGetCtrlOps(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemValue *elemValue);
int32_t AudioAccessorySetCtrlOps(const struct AudioKcontrol *kcontrol, const struct AudioCtrlElemValue *elemValue);

int32_t AudioCpuDaiSetCtrlOps(const struct AudioKcontrol *kcontrol, const struct AudioCtrlElemValue *elemValue);
int32_t AudioCpuDaiGetCtrlOps(const struct AudioKcontrol *kcontrol, struct AudioCtrlElemValue *elemValue);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* AUDIO_CORE_H */
