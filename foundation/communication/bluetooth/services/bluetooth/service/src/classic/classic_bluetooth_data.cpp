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

#include "classic_bluetooth_data.h"

#include <cstring>
#include "classic_defs.h"
#include "log.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
ClassicBluetoothData::ClassicBluetoothData()
{}

ClassicBluetoothData::~ClassicBluetoothData()
{
    offset_ = 0;
    maxLength_ = 0;
}

std::vector<uint8_t> ClassicBluetoothData::GetClassicBluetoothData() const
{
    int offset = 0;
    std::vector<uint8_t> btData(maxLength_);
    for (auto &data : dataStructList_) {
        ClassicDataStructure dataStructure = data;
        btData[offset] = dataStructure.GetLength();
        btData[offset + EXTEND_INQUIRY_RESPONSE_LENGTH_SIZE] = dataStructure.GetType();
        std::vector<uint8_t> dataVec = dataStructure.GetDataValue();
        errno_t ret =
            memcpy_s(&btData[offset + EXTEND_INQUIRY_RESPONSE_LENGTH_SIZE + EXTEND_INQUIRY_RESPONSE_TYPE_SIZE],
                (maxLength_ - offset - EXTEND_INQUIRY_RESPONSE_TYPE_SIZE - EXTEND_INQUIRY_RESPONSE_LENGTH_SIZE),
                &dataVec[0],
                (dataStructure.GetLength() - 1));
        if (ret != EOK) {
            LOG_ERROR("%{public}s::memcpy failed!", __func__);
        }
        offset = offset + dataStructure.GetLength() + 1;
    }

    return btData;
}

void ClassicBluetoothData::SetDataMaxLength(int len)
{
    maxLength_ = len;
}

bool ClassicBluetoothData::AddDataStructure(ClassicDataStructure dataStruct)
{
    int offset = offset_ + dataStruct.GetLength() + 1;
    if (offset > maxLength_) {
        LOG_ERROR("%{public}s::data structure over max length!", __func__);
        return false;
    } else {
        dataStructList_.push_back(dataStruct);
        offset_ = offset;
        return true;
    }
}

std::vector<ClassicDataStructure> ClassicBluetoothData::ParserData(const std::vector<uint8_t> &data) const
{
    int offset = 0;
    std::vector<ClassicDataStructure> dataList;

    while (offset < maxLength_) {
        int length = data[offset];
        if (length == 0) {
            break;
        } else {
            int type = data[offset + EXTEND_INQUIRY_RESPONSE_LENGTH_SIZE];
            std::vector<uint8_t> value;
            int valueStartIdx = offset + EXTEND_INQUIRY_RESPONSE_TYPE_SIZE + EXTEND_INQUIRY_RESPONSE_LENGTH_SIZE;
            int valueEndIdx = valueStartIdx + length - 1;
            value.insert(value.begin(), (data.begin() + valueStartIdx), (data.begin() + valueEndIdx));
            ClassicDataStructure dataBlock(length, type, value);
            dataList.push_back(dataBlock);
            offset = offset + length + 1;
        }
    }

    return dataList;
}
}  // namespace bluetooth
}  // namespace OHOS