/*
 * Copyright (c) 2022 Shenzhen Kaihong DID Co., Ltd.
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

#ifndef HDI_OMXCALLBACKTYPESTUB_H
#define HDI_OMXCALLBACKTYPESTUB_H

#include "codec_callback_if.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct CodecCallbackType* CodecCallbackTypeStubGetInstance(void);

void CodecCallbackTypeStubRelease(struct CodecCallbackType *instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // HDI_OMXCALLBACKTYPESTUB_H