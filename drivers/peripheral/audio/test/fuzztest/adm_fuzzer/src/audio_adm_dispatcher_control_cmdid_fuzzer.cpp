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
    struct HdfSBuf *writeBuf = nullptr;
    struct HdfSBuf *writeReply = nullptr;

    struct AudioCtlElemValue writeElemValue = {
        .id.cardServiceName = "hdf_audio_codec_dev0",
        .id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN,
        .id.itemName = "Mic Left Gain",
        .value[0] = 5,
    };

    service = HdfIoServiceBind(HDF_CONTROL_SERVICE.c_str());
    if (service == nullptr || service->dispatcher == nullptr) {
        return 0;
    }

    writeBuf = HdfSbufObtainDefaultSize();
    if (writeBuf == nullptr) {
        HdfIoServiceRecycle(service);
        return 0;
    }
    int32_t ret = WriteEleValueToBuf(writeBuf, writeElemValue);
    if (ret < 0) {
        return 0;
    }

    service->dispatcher->Dispatch(&service->object, size, writeBuf, writeReply);
    HdfSbufRecycle(writeBuf);
    HdfIoServiceRecycle(service);
    return 0;
}
