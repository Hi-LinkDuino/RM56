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

#ifndef OHOS_CAMERA_METADATA_UTILS_H
#define OHOS_CAMERA_METADATA_UTILS_H

#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "camera_metadata_info.h"
#include "message_parcel.h"

namespace OHOS {
namespace CameraStandard {
class MetadataUtils {
public:
    static bool EncodeCameraMetadata(const std::shared_ptr<CameraStandard::CameraMetadata> &metadata,
                                     MessageParcel &data);
    static void DecodeCameraMetadata(MessageParcel &data, std::shared_ptr<CameraStandard::CameraMetadata> &metadata);

private:
    static bool WriteMetadata(const camera_metadata_item_t &item, MessageParcel &data);
    static bool ReadMetadata(camera_metadata_item_t &item, MessageParcel &data);
    static void ItemDataToBuffer(const camera_metadata_item_t &item, void **buffer);
};
} // namespace CameraStandard
} // namespace OHOS
#endif // OHOS_CAMERA_METADATA_UTILS_H
