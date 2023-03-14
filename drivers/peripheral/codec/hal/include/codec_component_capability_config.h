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

#ifndef HDI_CODEC_COMPONENT_CAPABILITY_CONFIG_H
#define HDI_CODEC_COMPONENT_CAPABILITY_CONFIG_H

#include "device_resource_if.h"
#include "hdf_sbuf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t InitDataNode(const struct DeviceResourceNode *node);
int32_t LoadCapabilityData();
int32_t ClearCapabilityData();
int32_t HandleGetNumCmd(struct HdfSBuf *reply);
int32_t HandleGetAllCapablityListCmd(struct HdfSBuf *reply);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // HDI_CODEC_COMPONENT_CAPABILITY_CONFIG_H