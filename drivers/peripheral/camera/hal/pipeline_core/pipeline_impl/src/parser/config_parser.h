/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PARSE_CONFIG_H
#define PARSE_CONFIG_H
#include <optional>
#include "stream_pipeline_data_structure.h"
namespace OHOS::Camera {
class ConfigParser {
public:
    template<typename T>
    std::string CheckIdExsit(const int32_t& mode, T&& info, const int32_t& size) const
    {
        std::string result;
        for (int i = 0; i < size; i++) {
            if (info[i].id == mode) {
                result = std::string(info[i].name);
                break;
            }
        }
        return result;
    }
    std::optional<int32_t> GetTypeId(const std::string& str, G_STREAM_INFO info, const int32_t& size) const;
    std::string GetTypeStr(const int32_t& type, G_STREAM_INFO info, const int32_t& size) const;
};
}
#endif
