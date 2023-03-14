/*
 * Copyright (c) 2021 Shenzhen Kaihong DID Co., Ltd.
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

#ifndef DRIVERS_PERIPHERAL_CODEC_CONFIG_READER_H_
#define DRIVERS_PERIPHERAL_CODEC_CONFIG_READER_H_

#include <hdf_sbuf.h>
#include "hdf_io_service_if.h"
#include "hdf_remote_service.h"
#include "codec_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfRemoteService *GetConfigService(void);
int32_t EnumrateCapability(struct HdfRemoteService *remote, int index, CodecCapbility *cap);
int32_t GetCapability(struct HdfRemoteService *remote,
    AvCodecMime mime, CodecType type, uint32_t flags, CodecCapbility *cap);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // DRIVERS_PERIPHERAL_CODEC_CONFIG_READER_H_
