/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
 * @brief Defines operate bluetooth data functions.
 *
 */

/**
 * @file bluetooth_data.h
 *
 * @brief Bluetooth data class.
 *
 */

#ifndef CLASSIC_BLUETOOTH_DATA_H
#define CLASSIC_BLUETOOTH_DATA_H

#include <cstdlib>
#include <vector>
#include "base_def.h"
#include "classic_data_structure.h"

namespace OHOS {
namespace bluetooth {
static constexpr int BLUETOOTH_DATA_MAX_LENGTH = 240;

/**
 * @brief Represents bluetooth data.
 *
 */
class ClassicBluetoothData {
public:
    /**
     * @brief A constructor used to create a <b>ClassicBluetoothData</b> instance.
     *
     */
    ClassicBluetoothData();

    /**
     * @brief A destructor used to delete the <b>ClassicBluetoothData</b> instance.
     *
     */
    ~ClassicBluetoothData();

    /**
     * @brief Get bluetooth data.
     *
     * @return Returns bluetooth data.
     */
    std::vector<uint8_t> GetClassicBluetoothData() const;

    /**
     * @brief Set bluetooth data max length.
     *
     * @param len Data length.
     */
    void SetDataMaxLength(int len);

    /**
     * @brief Add bluetooth data structure.
     *
     * @param dataStruct Data structure.
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     */
    bool AddDataStructure(ClassicDataStructure dataStruct);

    /**
     * @brief Parser bluetooth data.
     *
     * @param data Bluetooth data.
     * @return Returns the parsed data structure list.
     */
    std::vector<ClassicDataStructure> ParserData(const std::vector<uint8_t> &data) const;

private:
    std::vector<ClassicDataStructure> dataStructList_ {};
    int offset_ {};
    int maxLength_ {};

    BT_DISALLOW_COPY_AND_ASSIGN(ClassicBluetoothData);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  /// CLASSIC_BLUETOOTH_DATA_H