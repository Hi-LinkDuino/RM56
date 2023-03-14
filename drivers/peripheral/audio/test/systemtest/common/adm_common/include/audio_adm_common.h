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
 * @brief Defines audio ADM test-related APIs, including data types and functions for writting data
to buffer
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_adm_common.h
 *
 * @brief Declares APIs for operations related to the audio ADM testing.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AUDIO_ADM_COMMON_H
#define AUDIO_ADM_COMMON_H

#include "osal_mem.h"
#include "audio_hdi_common.h"

namespace HMOS {
namespace Audio {
const int REGISTER_STATUS_ON = 0;
const int REGISTER_STATUS_OFF = 1;
const int AUDIO_SIZE_FRAME = 16 * 1024;
const int AUDIO_REPLY_EXTEND = 16;
const int CIR_BUFF_EMPTY = 3;
const std::string CARD_SEVICE_NAME = "hdf_audio_codec_dev0";

enum StreamDispMethodCmd {
    AUDIO_DRV_PCM_IOCTRL_HW_PARAMS,
    AUDIO_DRV_PCM_IOCTRL_RENDER_PREPARE,
    AUDIO_DRV_PCM_IOCTRL_CAPTURE_PREPARE,
    AUDIO_DRV_PCM_IOCTRL_WRITE,
    AUDIO_DRV_PCM_IOCTRL_READ,
    AUDIO_DRV_PCM_IOCTRL_RENDER_START,
    AUDIO_DRV_PCM_IOCTRL_RENDER_STOP,
    AUDIO_DRV_PCM_IOCTRL_CAPTURE_START,
    AUDIO_DRV_PCM_IOCTRL_CAPTURE_STOP,
    AUDIO_DRV_PCM_IOCTRL_RENDER_PAUSE,
    AUDIO_DRV_PCM_IOCTRL_CAPTURE_PAUSE,
    AUDIO_DRV_PCM_IOCTRL_RENDER_RESUME,
    AUDIO_DRV_PCM_IOCTRL_CAPTURE_RESUME,
    AUDIO_DRV_PCM_IOCTRL_RENDER_OPEN = 17,
    AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN = 19,
    AUDIO_DRV_PCM_IOCTRL_BUTT,
};

struct AudioPcmHwParams {
    enum AudioStreamType streamType;
    uint32_t channels;
    uint32_t rate;
    uint32_t periodSize;
    uint32_t periodCount;
    enum AudioFormat format;
    const char *cardServiceName;
    uint32_t period;
    uint32_t frameSize;
    bool isBigEndian;
    bool isSignedData;
    uint32_t startThreshold;
    uint32_t stopThreshold;
    uint32_t silenceThreshold;
};

struct AudioXferi {
    char *buf;
    uint32_t bufsize;
    uint32_t frameSize;
};

int32_t WriteHwParamsToBuf(struct HdfSBuf *sBuf, struct AudioPcmHwParams hwParams);

int32_t AdmRenderFramePrepare(const std::string& path, char *&frame, uint32_t& numRead, uint32_t& frameSize);

int32_t WriteFrameToSBuf(struct HdfSBuf *&sBufT, const std::string& path);

int32_t ObtainBuf(struct HdfSBuf *&readBuf, struct HdfSBuf *&readReply);

int32_t WriteCtrlInfo(struct HdfIoService *service, struct AudioCtlElemValue writeElemValue);

int32_t ReadCtrlInfo(struct HdfIoService *service, struct AudioCtlElemId id, int32_t expectValue);

int32_t WriteHwParams(std::string serviceName, struct HdfIoService *&service, struct AudioPcmHwParams hwParams);

int32_t WriteToSBuf(struct HdfSBuf *&sBufT);
}
}
#endif // AUDIO_ADM_COMMON_H

