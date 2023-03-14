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

#ifndef HDF_ATTRIBUTE_SERIALIZE_H
#define HDF_ATTRIBUTE_SERIALIZE_H

#include "hdf_device_info.h"
#include "hdf_sbuf.h"

bool DeviceAttributeSerialize(const struct HdfDeviceInfo *attribute, struct HdfSBuf *sbuf);
struct HdfDeviceInfo *DeviceAttributeDeserialize(struct HdfSBuf *sbuf);
void DeviceSerializedAttributeRelease(struct HdfDeviceInfo *attribute);

#endif /* HDF_ATTRIBUTE_SERIALIZE_H */