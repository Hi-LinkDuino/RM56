/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "classic_data_structure.h"

#include <string>
#include "classic_data_type_defs.h"

namespace OHOS {
namespace bluetooth {
ClassicDataStructure::ClassicDataStructure()
{}

ClassicDataStructure::ClassicDataStructure(uint8_t dataLength, int type, const std::vector<uint8_t> &value)
    : length_(dataLength), type_(type), data_(value)
{
}

ClassicDataStructure::~ClassicDataStructure()
{
    length_ = 0;
    type_ = 0;
}

uint8_t ClassicDataStructure::GetLength() const
{
    return length_;
}

std::vector<uint8_t> ClassicDataStructure::GetDataValue() const
{
    return data_;
}

int ClassicDataStructure::GetType() const
{
    return type_;
}
}  // namespace bluetooth
}  // namespace OHOS