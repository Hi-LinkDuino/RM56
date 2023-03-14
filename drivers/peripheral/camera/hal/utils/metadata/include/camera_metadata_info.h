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

#ifndef CAMERA_METADATA_INFO_H
#define CAMERA_METADATA_INFO_H

#include "camera_metadata_operator.h"

namespace OHOS {
namespace CameraStandard {
class CameraMetadata {
public:
    CameraMetadata(size_t itemCapacity, size_t dataCapacity);
    ~CameraMetadata();

    bool addEntry(uint32_t item, const void *data, size_t data_count);
    bool updateEntry(uint32_t tag, const void *data, size_t data_count);
    common_metadata_header_t *get();
    const common_metadata_header_t *get() const;
    bool isValid() const;

private:
    common_metadata_header_t *metadata_;

    bool resize_add_metadata(uint32_t item, const void *data, size_t data_count);
    void replace_metadata(common_metadata_header_t *newMetadata);
};
} // namespace CameraStandard
} // namespace OHOS
#endif /* CAMERA_METADATA_INFO_H */
