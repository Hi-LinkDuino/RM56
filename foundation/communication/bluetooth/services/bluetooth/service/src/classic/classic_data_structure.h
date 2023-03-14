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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines data structure functions.
 *
 */

/**
 * @file classic_data_structure.h
 *
 * @brief Data structure.
 *
 */

#ifndef CLASSIC_DATA_STRUCTURE_H
#define CLASSIC_DATA_STRUCTURE_H

#include <stdint.h>
#include <vector>

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents data structure.
 *
 */
class ClassicDataStructure {
public:
    /**
     * @brief A constructor used to create a <b>ClassicDataStructure</b> instance.
     *
     */
    ClassicDataStructure();

    /**
     * @brief A constructor used to create a <b>ClassicDataStructure</b> instance.
     *
     * @param dataLength Data length.
     * @param type Data type.
     * @param value Data value.
     */
    ClassicDataStructure(uint8_t dataLength, int type, const std::vector<uint8_t> &value);

    /**
     * @brief A destructor used to delete the <b>ClassicDataStructure</b> instance.
     *
     */
    ~ClassicDataStructure();

    /**
     * @brief Get data length.
     *
     * @return Returns data length.
     */
    uint8_t GetLength() const;

    /**
     * @brief Get data value.
     *
     * @return Returns data value.
     */
    std::vector<uint8_t> GetDataValue() const;

    /**
     * @brief Get data tyoe.
     *
     * @return Returns data type.
     */
    int GetType() const;

private:
    uint8_t length_ {};
    int type_ {};
    std::vector<uint8_t> data_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  /// CLASSIC_DATA_STRUCTURE_H