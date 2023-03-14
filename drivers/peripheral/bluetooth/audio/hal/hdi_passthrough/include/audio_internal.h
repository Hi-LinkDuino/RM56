/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AUDIO_INTERNAL_H
#define AUDIO_INTERNAL_H

#include <math.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <servmgr_hdi.h>
#include "audio_manager.h"
#include "audio_common.h"
#include "hdf_base.h"
namespace OHOS::HDI::Audio_Bluetooth {
#ifdef __cplusplus
extern "C" {
#endif

#define LOG_ENABLE  1
#define LOGV_ENABLE 1
#define NAME_LEN 64
#define BIT_NUM_32 32
#define BIT_NUM_24 24
#define BIT_NUM_16 16
#define BIT_NUM_8 8
#define PERIOD_SIZE 1024
#define PERIOD_COUNT 2
#define FRAME_DATA (8192 * 2)
#define PATHPLAN_LEN 64
#define PATHPLAN_COUNT 32
#define PATH_NAME_LEN 128
#define VOLUME_CHANGE 100
#define SEC_TO_NSEC 1000000000
#define HDMI_PORT_ID 12
#define MAP_MAX 100
#define FORMAT_ONE "%-5d  %-10d  %-20llu  %-15s  %s\n"
#define FORMAT_TWO "%-5d  %-10d  %s\n"
#define ERROR_LOG_MAX_NUM 8
#define ERROR_REASON_DESC_LEN 64
#define RANGE_MAX 5
#define RANGE_MIN 4
#define EXTPARAM_LEN 32

/**
 * @brief Enumerates HAL return value types.
 */
typedef enum {
    AUDIO_HAL_SUCCESS = 0,
    AUDIO_HAL_ERR_INTERNAL = -1,      /* audio system internal errors */
    AUDIO_HAL_ERR_NOT_SUPPORT = -2,   /* operation is not supported */
    AUDIO_HAL_ERR_INVALID_PARAM = -3, /* parameter is invaild */
    AUDIO_HAL_ERR_INVALID_OBJECT = -4, /**< Invalid object. */
    AUDIO_HAL_ERR_MALLOC_FAIL = -6, /**< Memory allocation fails. */

    #define HDF_AUDIO_HAL_ERR_START (-7000) /**< Defines the start of the device module error codes. */
    #define HDF_AUDIO_HAL_ERR_NUM(v) (HDF_AUDIO_HAL_ERR_START + (v)) /**< Defines the device module error codes. */
    AUDIO_HAL_ERR_NOTREADY = HDF_AUDIO_HAL_ERR_NUM(-1),      /* audio adapter is not ready */
    AUDIO_HAL_ERR_AI_BUSY = HDF_AUDIO_HAL_ERR_NUM(-2),         /* audio capture is busy now */
    AUDIO_HAL_ERR_AO_BUSY = HDF_AUDIO_HAL_ERR_NUM(-3),         /* audio render is busy now */
} AUDIO_HAL_ERR_CODE;

#ifndef LOGE
    #define LOGE(fmt, arg...) printf("[Audio:E]" fmt "\n", ##arg)
#endif
#ifndef LOGI
    #define LOGI(fmt, arg...) printf("[Audio:I]" fmt "\n", ##arg)
#endif
#ifndef LOGV
    #define LOGV(fmt, arg...) printf("[Audio:V]" fmt "\n", ##arg)
#endif

#if !LOG_ENABLE
    #undef LOGE
    #undef LOGI
    #undef LOGV

    #define LOGE(...)
    #define LOGI(...)
    #define LOGV(...)
#else
    #if !LOGV_ENABLE
        #undef LOGV
        #define LOGV(...)
    #endif // !LOGV_ENABLE
#endif // LOG_ENABLE

#ifndef UNUSED
    #define UNUSED(x) ((void)(x))
#endif

#ifndef UT_TEST
    #define STATIC_T static
#else
    #define STATIC_T
#endif

#define  USECASE_AUDIO_RENDER_DEEP_BUFFER  "deep-buffer-render"
#define  USECASE_AUDIO_RENDER_LOW_LATENCY  "low-latency-render"

#define AUDIO_ATTR_PARAM_ROUTE "attr-route"
#define ROUTE_SAMPLE "attr-route=x;"
#define AUDIO_ATTR_PARAM_FORMAT "attr-format"
#define FORMAT_SAMPLE "attr-format=xx;"
#define AUDIO_ATTR_PARAM_CHANNELS "attr-channels"
#define CHANNELS_SAMPLE "attr-channels=x;"
#define AUDIO_ATTR_PARAM_FRAME_COUNT "attr-frame-count"
#define FRAME_COUNT_SAMPLE "attr-frame-count=xxxxxxxxxxxxxxxxxxxx;"
#define AUDIO_ATTR_PARAM_SAMPLING_RATE "attr-sampling-rate"
#define SAMPLING_RATE_SAMPLE "attr-sampling-rate=xxxxx"
#define AUDIO_ATTR_PARAM_CONNECT "usb-connect"
#define AUDIO_ATTR_PARAM_DISCONNECT "usb-disconnect"
#define TELHPONE_RATE 8000
#define BROADCAST_AM_RATE 11025
#define BROADCAST_FM_RATE 22050
#define MINI_CAM_DV_RATE 32000
#define MUSIC_RATE 44100
#define HIGHT_MUSIC_RATE 48000
#define AUDIO_SAMPLE_RATE_12000 12000
#define AUDIO_SAMPLE_RATE_16000 16000
#define AUDIO_SAMPLE_RATE_24000 24000
#define AUDIO_SAMPLE_RATE_64000 64000
#define AUDIO_SAMPLE_RATE_96000 96000
#define MAX_TIME_INFO_LEN 64

#ifndef OPEN_AUDIO_LOG_WITH_TIME
#define LOG_FUN_INFO() do { \
        printf("%s: [%s]: [%d]\n", __FILE__, __func__, __LINE__); \
    } while (0)

#define LOG_FUN_ERR(fmt, arg...) do { \
        printf("%s: [%s]: [%d]:[ERROR]:" fmt"\n", __FILE__, __func__, __LINE__, ##arg); \
    } while (0)

#define LOG_PARA_INFO(fmt, arg...) do { \
        printf("%s: [%s]: [%d]:[INFO]:" fmt"\n", __FILE__, __func__, __LINE__, ##arg); \
    } while (0)
#else
#define LOG_FUN_INFO() do { \
    } while (0)

#define LOG_FUN_ERR(fmt, arg...) do { \
        char s[MAX_TIME_INFO_LEN] = {0}; \
        AudioGetSysTime(s, MAX_TIME_INFO_LEN); \
        printf("%s %s: [%s]: [%d]:[ERROR]:" fmt"\n", s, __FILE__, __func__, __LINE__, ##arg); \
    } while (0)

#define LOG_PARA_INFO(fmt, arg...) do { \
    } while (0)
#endif

struct DevHandleCapture {
    void *object;
};

struct DevHandle {
    void *object;
};

struct AudioPortAndCapability {
    struct AudioPort port;
    struct AudioPortCapability capability;
    AudioPortPassthroughMode mode;
};

struct AudioHwAdapter {
    struct AudioAdapter common;
    struct AudioAdapterDescriptor adapterDescriptor;
    struct AudioPortAndCapability *portCapabilitys;
    struct HdfRemoteService *proxyRemoteHandle; // proxyRemoteHandle
    int32_t adapterMgrRenderFlag;
};

struct AudioFrameRenderMode {
    uint64_t frames;
    struct AudioTimeStamp time;
    struct AudioSampleAttributes attrs;
    AudioChannelMode mode;
    uint32_t byteRate;
    uint32_t periodSize;
    uint32_t periodCount;
    uint32_t startThreshold;
    uint32_t stopThreshold;
    uint32_t silenceThreshold;
    uint32_t silenceSize;
    char *buffer;
    uint64_t bufferFrameSize;
    uint64_t bufferSize;
    RenderCallback callback;
    void* cookie;
    struct AudioMmapBufferDescripter mmapBufDesc;
};

struct AudioGain {
    float gain;
    float gainMin;
    float gainMax;
};

struct AudioVol {
    int volMin;
    int volMax;
};

struct AudioCtlParam {
    bool mute;
    float volume;
    float speed;
    bool pause;
    bool stop;
    pthread_mutex_t mutex;
    bool mutexFlag;
    pthread_cond_t functionCond;
    struct AudioVol volThreshold;
    struct AudioGain audioGain;
};

enum PathRoute {
    DEEP_BUFF = 0,
    RECORD,
    RECORD_LOW_LATRNCY,
    LOW_LATRNCY,
};

struct PathPlan {
    char pathPlanName[PATHPLAN_LEN];
    int value;
};

struct PathDeviceSwitch {
    char deviceSwitch[PATHPLAN_LEN];
    int32_t value;
};

struct PathDeviceInfo {
    char deviceType[NAME_LEN];
    int32_t deviceNum;
    struct PathDeviceSwitch deviceSwitchs[PATHPLAN_COUNT];
};

struct PathSelect {
    char useCase[NAME_LEN];
    struct PathDeviceInfo deviceInfo;
    int useCaseDeviceNum;
    struct PathPlan pathPlan[PATHPLAN_COUNT];
};

struct HwInfo {
    uint32_t card;
    uint32_t device;
    int flags;
    char adapterName[NAME_LEN];
    struct AudioPort portDescript;
    struct AudioDeviceDescriptor deviceDescript;
    PathRoute pathroute;
    struct PathSelect pathSelect;
};

struct AudioHwRenderMode {
    struct AudioCtlParam ctlParam;
    struct HwInfo hwInfo;
};

struct AudioHwRenderParam {
    struct AudioHwRenderMode renderMode;
    struct AudioFrameRenderMode frameRenderMode;
};

struct ErrorDump {
    int32_t errorCode;
    int32_t count;
    uint64_t frames;
    char* reason;                       // Specific reasons for failure
    char* currentTime;
};

struct ErrorLog {
    uint32_t totalErrors;
    uint32_t iter;
    struct ErrorDump errorDump[ERROR_LOG_MAX_NUM];
};

struct AudioHwRender {
    struct AudioRender common;
    struct AudioHwRenderParam renderParam;
    struct HdfRemoteService *proxyRemoteHandle; // proxyRemoteHandle
    struct ErrorLog errorLog;
};

struct ParamValMap {
    char key[EXTPARAM_LEN];
    char value[EXTPARAM_LEN];
};

struct ExtraParams {
    int32_t route;
    int32_t format;
    uint32_t channels;
    uint64_t frames;
    uint32_t sampleRate;
    bool flag;
};

enum ErrorDumpCode {
    WRITE_FRAME_ERROR_CODE = -5,
};

enum AudioServiceNameType {
    AUDIO_SERVICE_IN = 0,
    AUDIO_SERVICE_OUT,
    AUDIO_SERVICE_MAX,
};

int32_t GetAudioRenderFunc(struct AudioHwRender *hwRender);
int32_t CheckParaDesc(const struct AudioDeviceDescriptor *desc, const char *type);
int32_t CheckParaAttr(const struct AudioSampleAttributes *attrs);
int32_t AttrFormatToBit(const struct AudioSampleAttributes *attrs, int32_t *format);
int32_t InitHwRenderParam(struct AudioHwRender *hwRender, const struct AudioDeviceDescriptor *desc,
                          const struct AudioSampleAttributes *attrs);
int32_t InitForGetPortCapability(struct AudioPort portIndex, struct AudioPortCapability *capabilityIndex);
void AudioAdapterReleaseCapSubPorts(const struct AudioPortAndCapability *portCapabilitys, int32_t num);
int32_t AudioAdapterInitAllPorts(struct AudioAdapter *adapter);
void AudioReleaseRenderHandle(struct AudioHwRender *hwRender);
int32_t AudioAdapterCreateRenderPre(struct AudioHwRender *hwRender, const struct AudioDeviceDescriptor *desc,
                                    const struct AudioSampleAttributes *attrs, const struct AudioHwAdapter *hwAdapter);
int32_t AudioAdapterBindServiceRender(struct AudioHwRender *hwRender);
int32_t AudioAdapterCreateRender(struct AudioAdapter *adapter, const struct AudioDeviceDescriptor *desc,
                                 const struct AudioSampleAttributes *attrs, struct AudioRender **render);
int32_t AudioAdapterDestroyRender(struct AudioAdapter *adapter, struct AudioRender *render);
int32_t AudioAdapterGetPortCapability(struct AudioAdapter *adapter, const struct AudioPort *port,
                                      struct AudioPortCapability *capability);
int32_t AudioAdapterSetPassthroughMode(struct AudioAdapter *adapter, const struct AudioPort *port,
                                       AudioPortPassthroughMode mode);
int32_t AudioAdapterGetPassthroughMode(struct AudioAdapter *adapter, const struct AudioPort *port,
                                       AudioPortPassthroughMode *mode);
int32_t PcmBytesToFrames(const struct AudioFrameRenderMode *frameRenderMode, uint64_t bytes, uint32_t *frameCount);
int32_t AudioRenderStart(AudioHandle handle);
int32_t AudioRenderStop(AudioHandle handle);
int32_t AudioRenderPause(AudioHandle handle);
int32_t AudioRenderResume(AudioHandle handle);
int32_t AudioRenderFlush(AudioHandle handle);
int32_t AudioRenderGetFrameSize(AudioHandle handle, uint64_t *size);
int32_t AudioRenderGetFrameCount(AudioHandle handle, uint64_t *count);
int32_t AudioRenderSetSampleAttributes(AudioHandle handle, const struct AudioSampleAttributes *attrs);
int32_t AudioRenderGetSampleAttributes(AudioHandle handle, struct AudioSampleAttributes *attrs);
int32_t AudioRenderGetCurrentChannelId(AudioHandle handle, uint32_t *channelId);
int32_t AudioRenderCheckSceneCapability(AudioHandle handle, const struct AudioSceneDescriptor *scene,
                                        bool *supported);
int32_t AudioRenderSelectScene(AudioHandle handle, const struct AudioSceneDescriptor *scene);
int32_t AudioRenderSetMute(AudioHandle handle, bool mute);
int32_t AudioRenderGetMute(AudioHandle handle, bool *mute);
int32_t AudioRenderSetVolume(AudioHandle handle, float volume);
int32_t AudioRenderGetVolume(AudioHandle handle, float *volume);
int32_t AudioRenderGetGainThreshold(AudioHandle handle, float *min, float *max);
int32_t AudioRenderGetGain(AudioHandle handle, float *gain);
int32_t AudioRenderSetGain(AudioHandle handle, float gain);
int32_t AudioRenderGetLatency(struct AudioRender *render, uint32_t *ms);
int32_t AudioRenderRenderFrame(struct AudioRender *render, const void *frame,
                               uint64_t requestBytes, uint64_t *replyBytes);
int32_t AudioRenderGetRenderPosition(struct AudioRender *render, uint64_t *frames, struct AudioTimeStamp *time);
int32_t AudioRenderSetRenderSpeed(struct AudioRender *render, float speed);
int32_t AudioRenderGetRenderSpeed(struct AudioRender *render, float *speed);
int32_t AudioRenderSetChannelMode(struct AudioRender *render, AudioChannelMode mode);
int32_t AudioRenderGetChannelMode(struct AudioRender *render, AudioChannelMode *mode);
int32_t AudioRenderSetExtraParams(AudioHandle handle, const char *keyValueList);
int32_t AudioRenderGetExtraParams(AudioHandle handle, char *keyValueList, int32_t listLenth);
int32_t AudioRenderReqMmapBuffer(AudioHandle handle, int32_t reqSize, struct AudioMmapBufferDescripter *desc);
int32_t AudioRenderGetMmapPosition(AudioHandle handle, uint64_t *frames, struct AudioTimeStamp *time);
int32_t AudioRenderTurnStandbyMode(AudioHandle handle);
int32_t AudioRenderAudioDevDump(AudioHandle handle, int32_t range, int32_t fd);
int32_t AudioRenderRegCallback(struct AudioRender *render, RenderCallback callback, void* cookie);
int32_t AudioRenderDrainBuffer(struct AudioRender *render, AudioDrainNotifyType *type);

#ifdef __cplusplus
}
#endif
}
#endif
