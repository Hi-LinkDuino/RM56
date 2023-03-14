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

#include "audio_hdi_common.h"
#include "audio_adm_common.h"

using namespace HMOS::Audio;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    struct HdfIoService *service = nullptr;
    struct HdfSBuf *sBuf = nullptr;
    struct HdfSBuf *reply = nullptr;
    struct AudioPcmHwParams hwParams {
        .streamType = AUDIO_CAPTURE_STREAM, .channels = 2, .rate = 11025, .periodSize = 8192,
        .periodCount = 32, .format = AUDIO_FORMAT_PCM_24_BIT, .cardServiceName = "hdf_audio_codec_dev0",
        .isBigEndian = 0, .isSignedData = 1, .silenceThreshold = 16385
    };

    service = HdfIoServiceBind(HDF_CAPTURE_SERVICE.c_str());
    if (service == nullptr || service->dispatcher == nullptr) {
        return 0;
    }
    sBuf = HdfSbufObtainDefaultSize();
    if (sBuf == nullptr) {
        return 0;
    }
    int32_t ret = WriteHwParamsToBuf(sBuf, hwParams);
    if (ret < 0) {
        return 0;
    }
    ret = service->dispatcher->Dispatch(&service->object, size, sBuf, reply);
    HdfSbufRecycle(sBuf);
    HdfIoServiceRecycle(service);
    return 0;
}
