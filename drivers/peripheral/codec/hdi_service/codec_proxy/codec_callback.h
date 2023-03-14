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

#ifndef HDI_CODEC_CALLBACK_CODECCALLBACK_H
#define HDI_CODEC_CALLBACK_CODECCALLBACK_H
#include "codec_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfRemoteService;

enum {
    CMD_CODEC_ON_EVENT,
    CMD_CODEC_INPUT_BUFFER_AVAILABLE,
    CMD_CODEC_OUTPUT_BUFFER_AVAILABLE,
};

struct ICodecCallback {
    struct HdfRemoteService *remote;
    CodecCallback callback;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // HDI_CODEC_CALLBACK_CODECCALLBACK_H