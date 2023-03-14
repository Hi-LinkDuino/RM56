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

#include "frameworks/core/common/sharedata/share_data.h"

#include <functional>
#include <set>
#include <string>

namespace OHOS::Ace {

std::map<int32_t, std::shared_ptr<std::vector<uint8_t>>> ShareData::shareDataMap_;

void ShareData::InsertBuffer(int32_t bufferId, std::shared_ptr<std::vector<uint8_t>> dataArray)
{
    shareDataMap_[bufferId] = dataArray;
}

std::shared_ptr<std::vector<uint8_t>> ShareData::GetShareBufferById(int32_t id)
{
    auto data = ShareData::shareDataMap_.find(id);
    if (data != ShareData::shareDataMap_.end()) {
        return data->second;
    }
    return std::shared_ptr<std::vector<uint8_t>>();
}

void ShareData::ReleaseShareBufferById(int32_t id)
{
    auto data = ShareData::shareDataMap_.find(id);
    if (data != ShareData::shareDataMap_.end()) {
        ShareData::shareDataMap_.erase(data);
    }
}

} // namespace OHOS::Ace
