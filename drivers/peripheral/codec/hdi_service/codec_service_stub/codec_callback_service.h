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

#ifndef HDI_CODEC_CALLBACK_CODECCALLBACKSERVICE_H
#define HDI_CODEC_CALLBACK_CODECCALLBACKSERVICE_H

#include "codec_callback.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void CodecCallbackServiceConstruct(struct ICodecCallback *service);
int CodecCallbackOnEvent(UINTPTR comp, UINTPTR appData, EventType event,
                         uint32_t data1, uint32_t data2, UINTPTR eventData);
int CodecCallbackInputBufferAvailable(UINTPTR comp, UINTPTR appData, InputInfo *inBuf);
int CodecCallbackOutputBufferAvailable(UINTPTR comp, UINTPTR appData, OutputInfo *outBuf);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // HDI_CODEC_CALLBACK_CODECCALLBACKSERVICE_H