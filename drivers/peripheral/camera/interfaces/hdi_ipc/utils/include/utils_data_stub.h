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

#ifndef HDI_UTILS_DATA_STUB_INF_H
#define HDI_UTILS_DATA_STUB_INF_H

#include <list>
#include <map>
#include <vector>
#include <message_parcel.h>
#include <iservmgr_hdi.h>
#include "types.h"
#include "camera_metadata_info.h"

namespace OHOS::Camera {
class UtilsDataStub {
public:
    static bool EncodeCameraMetadata(const std::shared_ptr<CameraStandard::CameraMetadata> &metadata,
        MessageParcel &data);
    static void DecodeCameraMetadata(MessageParcel &data, std::shared_ptr<CameraStandard::CameraMetadata> &metadata);
    static bool EncodeStreamInfo(const std::shared_ptr<StreamInfo> &pInfo, MessageParcel &parcel);
    static void DecodeStreamInfo(MessageParcel &parcel, std::shared_ptr<StreamInfo> &pInfo);
private:
    static int32_t GetDataSize(uint8_t type);
    static bool WriteMetadata(const camera_metadata_item_t &entry, MessageParcel &data);
    static bool ReadMetadata(camera_metadata_item_t &entry, MessageParcel &data);
    static void EntryDataToBuffer(const camera_metadata_item_t &entry, void **buffer);
};
}
#endif // HDI_UTILS_DATA_STUB_INF_H
