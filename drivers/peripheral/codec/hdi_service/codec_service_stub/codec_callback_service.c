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

#include "codec_callback_service.h"
#include <hdf_base.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int CodecCallbackOnEvent(UINTPTR comp, UINTPTR appData, EventType event,
                         uint32_t data1, uint32_t data2, UINTPTR eventData)
{
    return HDF_SUCCESS;
}

int CodecCallbackInputBufferAvailable(UINTPTR comp, UINTPTR appData, InputInfo *inBuf)
{
    return HDF_SUCCESS;
}

int CodecCallbackOutputBufferAvailable(UINTPTR comp, UINTPTR appData, OutputInfo *outBuf)
{
    return HDF_SUCCESS;
}

void CodecCallbackServiceConstruct(struct ICodecCallback *service)
{
    service->callback.OnEvent = CodecCallbackOnEvent;
    service->callback.InputBufferAvailable = CodecCallbackInputBufferAvailable;
    service->callback.OutputBufferAvailable = CodecCallbackOutputBufferAvailable;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */