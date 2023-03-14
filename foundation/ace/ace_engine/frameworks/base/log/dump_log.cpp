/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "base/log/dump_log.h"

#include <string>

namespace OHOS::Ace {

DumpLog::DumpLog() = default;
DumpLog::~DumpLog() = default;

void DumpLog::Print(int32_t depth, const std::string& className, int32_t childSize)
{
    if (!ostream_->good()) {
        return;
    }
    std::string space = "  ";
    for (int32_t i = 0; i < depth; ++i) {
        ostream_->write(space.c_str(), space.length());
    }
    ostream_->write(space.c_str(), space.length());
    std::string data = "|-> ";
    data.append(className);
    data.append(" childSize:" + std::to_string(childSize));
    data.append("\n");
    ostream_->write(data.c_str(), data.length());

    for (auto& desc : description_) {
        for (int32_t i = 0; i < depth; ++i) {
            ostream_->write(space.c_str(), space.length());
        }
        std::string data = "";
        if (childSize == 0) {
            data = "      ";
        } else {
            data = "    | ";
        }
        data.append(desc);
        ostream_->write(data.c_str(), data.length());
    }
    ostream_->flush();
    description_.clear();
    description_.shrink_to_fit();
}

void DumpLog::Print(const std::string& content)
{
    Print(0, content);
}

void DumpLog::Print(int32_t depth, const std::string& content)
{
    std::string space = " ";
    for (int32_t i = 0; i < depth; ++i) {
        ostream_->write(space.c_str(), space.length());
    }

    std::string data = content + "\n";
    ostream_->write(data.c_str(), data.length());
}

void DumpLog::Reset()
{
    ostream_.reset();
}

} // namespace OHOS::Ace