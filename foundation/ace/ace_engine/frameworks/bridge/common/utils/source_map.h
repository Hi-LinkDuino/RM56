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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_SOURCE_MAP_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_SOURCE_MAP_H

#include <fstream>
#include <vector>

#include "base/log/log.h"
#include "base/memory/referenced.h"
#include "base/utils/macros.h"

namespace OHOS::Ace::Framework {

struct SourceMapInfo {
    int32_t beforeRow = 0;
    int32_t beforeColumn = 0;
    int32_t afterRow = 0;
    int32_t afterColumn = 0;
    int32_t sourcesVal = 0;
    int32_t namesVal = 0;
};

struct MappingInfo {
    int32_t row = 0;
    int32_t col = 0;
    std::string sources;
};

class ACE_EXPORT RevSourceMap final : public Referenced {
public:
    MappingInfo Find(int32_t row, int32_t col);
    void ExtractKeyInfo(const std::string& sourceMap, std::vector<std::string>& sourceKeyInfo);
    void Init(const std::string& sourceMap);

private:
    SourceMapInfo nowPos_;
    std::vector<std::string> files_;
    std::vector<std::string> sources_;
    std::vector<std::string> names_;
    std::vector<std::string> mappings_;
    std::vector<SourceMapInfo> afterPos_;

    std::vector<std::string> HandleMappings(const std::string& mapping);
    uint32_t Base64CharToInt(char charCode);
    bool VlqRevCode(const std::string& vStr, std::vector<int32_t>& ans);
};

}  // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_SOURCE_MAP_H
