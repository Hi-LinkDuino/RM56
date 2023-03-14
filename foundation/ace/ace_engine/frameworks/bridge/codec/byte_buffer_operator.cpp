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

#include "frameworks/bridge/codec/byte_buffer_operator.h"

namespace OHOS::Ace::Framework {

bool ByteBufferReader::ReadData(std::map<std::string, std::string>& mapValue) const
{
    int32_t size = -1;
    if (!ReadData(size) || size < 0) {
        LOGW("Could not read map size or map size is invalid");
        return false;
    }

    mapValue.clear();

    std::string key;
    std::string value;
    for (int32_t idx = 0; idx < size; ++idx) {
        if (!ReadData(key) || !ReadData(value)) {
            LOGW("Failed to read key or value");
            return false;
        }
        mapValue.emplace(std::move(key), std::move(value));
    }
    return true;
}

bool ByteBufferReader::ReadData(std::set<std::string>& setValue) const
{
    int32_t size = -1;
    if (!ReadData(size) || size < 0) {
        LOGW("Could not read map size or map size is invalid");
        return false;
    }

    setValue.clear();

    std::string value;
    for (int32_t idx = 0; idx < size; ++idx) {
        if (!ReadData(value)) {
            LOGW("Failed to read string value");
            return false;
        }
        setValue.emplace(std::move(value));
    }
    return true;
}

void ByteBufferWriter::WriteData(const std::map<std::string, std::string>& mapValue)
{
    WriteData(static_cast<int32_t>(mapValue.size()));
    for (const auto& [key, value] : mapValue) {
        WriteData(key);
        WriteData(value);
    }
}

void ByteBufferWriter::WriteData(const std::set<std::string>& setValue)
{
    WriteData(static_cast<int32_t>(setValue.size()));
    for (const auto& value : setValue) {
        WriteData(value);
    }
}

} // namespace OHOS::Ace::Framework