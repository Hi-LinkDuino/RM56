/**
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

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Defines audio-related APIs, including custom data types and functions for loading drivers,
 * accessing a driver adapter, and rendering and capturing audios.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_adapter.h
 *
 * @brief Declares APIs for operations related to the audio adapter.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AUDIO_HDI_COMMON_H
#define AUDIO_HDI_COMMON_H

#include <gtest/gtest.h>
#include "audio_adapter.h"
#include "audio_internal.h"
#include "audio_types.h"
#include "hdf_io_service_if.h"
#include "hdf_sbuf.h"
#include "audio_proxy_manager.h"

namespace HMOS {
namespace Audio {
#ifdef AUDIO_ADM_SO
    const std::string FUNCTION_NAME = "GetAudioManagerFuncs";
    const std::string RESOLVED_PATH = HDF_LIBRARY_FULL_PATH("libhdi_audio");
    using TestAudioManager = struct AudioManager;
    const int IS_ADM = true;
#endif
#ifdef AUDIO_MPI_SO
    const std::string FUNCTION_NAME = "GetAudioManagerFuncs";
    const std::string RESOLVED_PATH = HDF_LIBRARY_FULL_PATH("libhdi_audio");
    using TestAudioManager = struct AudioManager;
    const int IS_ADM = false;
#endif
#ifdef AUDIO_ADM_SERVICE
    const std::string FUNCTION_NAME = "GetAudioProxyManagerFuncs";
    const std::string RESOLVED_PATH = HDF_LIBRARY_FULL_PATH("libhdi_audio_client");
    using TestAudioManager = struct AudioProxyManager;
    const int IS_ADM = true;
#endif
#ifdef AUDIO_MPI_SERVICE
    const std::string FUNCTION_NAME = "GetAudioProxyManagerFuncs";
    const std::string RESOLVED_PATH = HDF_LIBRARY_FULL_PATH("libhdi_audio_client");
    using TestAudioManager = struct AudioProxyManager;
    const int IS_ADM = false;
#endif
#ifdef __LITEOS__
    const std::string FUNCTION_NAME = "GetAudioManagerFuncs";
    const std::string RESOLVED_PATH = "/usr/lib/libhdi_audio.so";
    using TestAudioManager = struct AudioManager;
    const int IS_ADM = true;
    const std::string AUDIO_FILE = "/userdata/audiorendertest.wav";
    const std::string LOW_LATENCY_AUDIO_FILE = "/userdata/lowlatencyrendertest.wav";
    const std::string AUDIO_CAPTURE_FILE = "/userdata/audiocapture.wav";
    const std::string AUDIO_LOW_LATENCY_CAPTURE_FILE = "/userdata/lowlatencycapturetest.wav";
#else
    const std::string AUDIO_FILE = "//bin/audiorendertest.wav";
    const std::string LOW_LATENCY_AUDIO_FILE = "//bin/lowlatencyrendertest.wav";
    const std::string AUDIO_CAPTURE_FILE = "//bin/audiocapture.wav";
    const std::string AUDIO_LOW_LATENCY_CAPTURE_FILE = "//bin/lowlatencycapturetest.wav";
#endif

const std::string AUDIO_RIFF = "RIFF";
const std::string AUDIO_WAVE = "WAVE";
const std::string AUDIO_DATA = "data";
const uint32_t INT_32_MAX = 0x7fffffff;
const uint32_t INDEX_END = 555;
const uint32_t MOVE_RIGHT_NUM = 3;
const int MOVE_LEFT_NUM = 8;
const int CHANNELCOUNT = 2;
const int SAMPLERATE = 48000;
const int DEEP_BUFFER_RENDER_PERIOD_SIZE = 4*1024;
const float GAIN_MIN = 0;
const float GAIN_MAX = 15;
const uint64_t INITIAL_VALUE = 0;
const int BUFFER_LENTH = 1024 * 16;
const int FILE_CAPTURE_SIZE = 1024 * 1024 * 1;
const uint64_t MEGABYTE = 1024;
const int FRAME_SIZE = 1024;
const int FRAME_COUNT = 4;
const int ADAPTER_COUNT = 32;
const int TRY_NUM_FRAME = 20;
const int AUDIO_ADAPTER_MAX_NUM = 4;
const int AUDIO_WRITE_COMPELETED_VALUE = 1;
const int AUDIO_RENDER_FULL_VALUE = 2;
const int AUDIO_FLUSH_COMPLETED_VALUE = 3;
const int64_t SECTONSEC = 1000000000;
const int MICROSECOND = 1000000;
const int RANGE = 3;
const std::string HDF_CONTROL_SERVICE = "hdf_audio_control";
const std::string HDF_RENDER_SERVICE = "hdf_audio_render";
const std::string HDF_CAPTURE_SERVICE = "hdf_audio_capture";

const int AUDIODRV_CTL_ELEM_IFACE_DAC = 0; /* virtual dac device */
const int AUDIODRV_CTL_ELEM_IFACE_ADC = 1; /* virtual adc device */
const int AUDIODRV_CTL_ELEM_IFACE_GAIN = 2; /* virtual adc device */
const int AUDIODRV_CTL_ELEM_IFACE_MIXER = 3; /* virtual mixer device */
const int AUDIODRV_CTL_ELEM_IFACE_ACODEC = 4; /* Acodec device */
const int AUDIODRV_CTL_ELEM_IFACE_PGA = 5; /* PGA device */
const int AUDIODRV_CTL_ELEM_IFACE_AIAO = 6; /* AIAO device */

enum ControlDispMethodCmd {
    AUDIODRV_CTRL_IOCTRL_ELEM_INFO,
    AUDIODRV_CTRL_IOCTRL_ELEM_READ,
    AUDIODRV_CTRL_IOCTRL_ELEM_WRITE,
    AUDIODRV_CTRL_IOCTRL_ELEM_BUTT,
};

enum AudioPCMBit {
    PCM_8_BIT  = 8,
    PCM_16_BIT = 16,
    PCM_24_BIT = 24,
    PCM_32_BIT = 32,
};

struct AudioCtlElemId {
    const char *cardServiceName;
    int32_t iface;
    const char *itemName; /* ASCII name of item */
};

struct AudioCtlElemValue {
    struct AudioCtlElemId id;
    int32_t value[2];
};

struct AudioHeadInfo {
    uint32_t testFileRiffId;
    uint32_t testFileRiffSize;
    uint32_t testFileFmt;
    uint32_t audioFileFmtId;
    uint32_t audioFileFmtSize;
    uint16_t audioFileFormat;
    uint16_t audioChannelNum;
    uint32_t audioSampleRate;
    uint32_t audioByteRate;
    uint16_t audioBlockAlign;
    uint16_t audioBitsPerSample;
    uint32_t dataId;
    uint32_t dataSize;
};

struct AudioCharacteristic {
    bool setmute;
    bool getmute;
    float setvolume;
    float getvolume;
    float setgain;
    float getgain;
    float gainthresholdmin;
    float gainthresholdmax;
    uint64_t getframes;
    uint64_t getframesize;
    uint64_t getframecount;
    uint32_t getcurrentchannelId;
    enum AudioChannelMode setmode;
    enum AudioChannelMode getmode;
    bool supported;
    uint32_t latencyTime;
};

struct PrepareAudioPara {
    TestAudioManager *manager;
    enum AudioPortDirection portType;
    const char *adapterName;
    struct AudioAdapter *adapter;
    struct AudioPort *audioPort;
    void *self;
    void *result;
    pthread_t tids;
    enum AudioPortPin pins;
    const char *path;
    struct AudioRender *render;
    struct AudioCapture *capture;
    struct AudioHeadInfo headInfo;
    struct AudioAdapterDescriptor *desc;
    struct AudioAdapterDescriptor *descs;
    char *frame;
    uint64_t requestBytes;
    uint64_t replyBytes;
    uint64_t fileSize;
    struct AudioSampleAttributes attrs;
    struct AudioCharacteristic character;
    struct AudioSampleAttributes attrsValue;
    struct AudioSceneDescriptor scenes;
    struct AudioPortCapability capability;
    enum AudioPortPassthroughMode mode;
    struct AudioTimeStamp time;
    struct timeval start;
    struct timeval end;
    int64_t delayTime;
    int64_t totalTime;
    float averageDelayTime;
    struct AudioDeviceDescriptor devDesc;
};

int32_t InitAttrs(struct AudioSampleAttributes &attrs);

int32_t InitDevDesc(struct AudioDeviceDescriptor &devDesc, const uint32_t portId, enum AudioPortPin pins);

int32_t SwitchAdapter(struct AudioAdapterDescriptor *descs, const std::string &adapterNameCase,
    enum AudioPortDirection portFlag, struct AudioPort *&audioPort, int size);

uint32_t PcmFormatToBits(enum AudioFormat format);

uint32_t PcmFramesToBytes(const struct AudioSampleAttributes attrs);

int32_t WavHeadAnalysis(struct AudioHeadInfo &wavHeadInfo, FILE *file, struct AudioSampleAttributes &attrs);

int32_t GetAdapters(TestAudioManager *manager, struct AudioAdapterDescriptor **descs, int &size);

int32_t GetLoadAdapter(TestAudioManager *manager, enum AudioPortDirection portType,
    const std::string &adapterName, struct AudioAdapter **adapter, struct AudioPort *&audioPort);

int32_t AudioCreateRender(TestAudioManager *manager, enum AudioPortPin pins, const std::string &adapterName,
    struct AudioAdapter **adapter, struct AudioRender **render);

int32_t AudioCreateCapture(TestAudioManager *manager, enum AudioPortPin pins, const std::string &adapterName,
    struct AudioAdapter **adapter, struct AudioCapture **capture);

int32_t FrameStart(struct AudioHeadInfo wavHeadInfo, struct AudioRender *render, FILE *file,
    struct AudioSampleAttributes attrs);

int32_t FrameStartCapture(struct AudioCapture *capture, FILE *file, const struct AudioSampleAttributes attrs);

int32_t RenderFramePrepare(const std::string &path, char *&frame, uint64_t &numRead);

void FrameStatus(int status);

int32_t StartRecord(struct AudioCapture *capture, FILE *file, uint64_t filesize);

int32_t WriteIdToBuf(struct HdfSBuf *sBuf, struct AudioCtlElemId id);

int32_t WriteEleValueToBuf(struct HdfSBuf *sBuf, struct AudioCtlElemValue elemvalue);

int32_t PowerOff(struct AudioCtlElemValue firstElemValue, struct AudioCtlElemValue secondElemValue);

int32_t CheckRegisterStatus(const struct AudioCtlElemId firstId, const struct AudioCtlElemId secondId,
    const int firstStatus, const int secondStatus);

int32_t AudioCreateStartRender(TestAudioManager *manager, struct AudioRender **render, struct AudioAdapter **adapter,
    const std::string &adapterName);

int32_t AudioRenderStartAndOneFrame(struct AudioRender *render);

int32_t StopAudio(struct PrepareAudioPara &audiopara);

int32_t ThreadRelease(struct PrepareAudioPara &audiopara);

int32_t AudioCreateStartCapture(TestAudioManager *manager, struct AudioCapture **capture,
    struct AudioAdapter **adapter, const std::string &adapterName);

int32_t AudioCaptureStartAndOneFrame(struct AudioCapture *capture);

int32_t PlayAudioFile(struct PrepareAudioPara &audiopara);

int32_t RecordAudio(struct PrepareAudioPara &audiopara);

int32_t InitAttrsUpdate(struct AudioSampleAttributes &attrs, enum AudioFormat format, uint32_t channelCount,
    uint32_t sampleRate);

int32_t AudioRenderSetGetSampleAttributes(struct AudioSampleAttributes attrs, struct AudioSampleAttributes &attrsValue,
    struct AudioRender *render);

int32_t AudioCaptureSetGetSampleAttributes(struct AudioSampleAttributes attrs, struct AudioSampleAttributes &attrsValue,
    struct AudioCapture *capture);

int32_t InitMmapDesc(FILE *fp, struct AudioMmapBufferDescripter &desc, int32_t &reqSize, bool flag);

int32_t PlayMapAudioFile(struct PrepareAudioPara &audiopara);

int32_t RecordMapAudio(struct PrepareAudioPara &audiopara);
int32_t AudioRenderCallback(enum AudioCallbackType type, void *reserved, void *cookie);
int32_t CheckFlushValue();
int32_t CheckRenderFullValue();
int32_t CheckWriteCompleteValue();
}
}
#endif // AUDIO_HDI_COMMON_H

