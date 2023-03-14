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

#include "config_parser.h"

namespace OHOS::Camera {
std::optional<int32_t> ConfigParser::GetTypeId(const std::string& str, G_STREAM_INFO info, const int32_t& size) const
{
    if (str.empty() || info == nullptr) {
        return std::nullopt;
    }

    for (int i = 0; i < size; i++) {
        if (info[i].name != nullptr) {
            if (std::string(info[i].name) == str) {
                return info[i].id;
            }
        }
    }
    return std::nullopt;
}

std::string ConfigParser::GetTypeStr(const int32_t& type, G_STREAM_INFO info, const int32_t& size) const
{
    if (info == nullptr) {
        CAMERA_LOGE("info is nullptr.");
    }

    std::string result;
    for (int i = 0; i < size; i++) {
        if (((info + i) != nullptr) && (info[i].name != nullptr)) {
            if (info[i].id == type) {
                result = info[i].name;
                break;
            }
        }
    }
    return result;
}
}