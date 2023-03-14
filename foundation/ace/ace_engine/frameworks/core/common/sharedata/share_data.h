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

#ifndef FRAMEWORKS_CORE_COMMON_SHARE_DATA_SHARE_DATA_H
#define FRAMEWORKS_CORE_COMMON_SHARE_DATA_SHARE_DATA_H

#include <map>

#include "base/memory/ace_type.h"

namespace OHOS::Ace {

class ACE_EXPORT ShareData {
public:

    ShareData() = delete;
    ~ShareData() = delete;

    static void InsertBuffer(int32_t bufferId, std::shared_ptr<std::vector<uint8_t>> dataArray);

    static std::shared_ptr<std::vector<uint8_t>> GetShareBufferById(int32_t id);

    static void ReleaseShareBufferById(int32_t id);

private:
    static std::map<int32_t, std::shared_ptr<std::vector<uint8_t>>> shareDataMap_;
};

} // namespace OHOS::Ace
#endif // FRAMEWORKS_CORE_COMMON_SHARE_DATA_SHARE_DATA_H
