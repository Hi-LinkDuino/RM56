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
#ifndef STUB_MSGPROC_H
#define STUB_MSGPROC_H

#include <hdf_sbuf.h>
#include "codec_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t CodecSerPackCapbility(struct HdfSBuf *reply, CodecCapbility *cap);
int32_t CodecSerParseParam(struct HdfSBuf *data, Param *param);
int32_t CodecSerPackParam(struct HdfSBuf *reply, Param *param);
int32_t CodecSerPackInputInfo(struct HdfSBuf *reply, InputInfo *inputData);
int32_t CodecSerParseInputInfo(struct HdfSBuf *data, InputInfo *inputData);
int32_t CodecSerParseOutputInfo(struct HdfSBuf *data, OutputInfo *outInfo);
int32_t CodecSerPackOutputInfo(struct HdfSBuf *reply, OutputInfo *outInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // STUB_MSGPROC_H