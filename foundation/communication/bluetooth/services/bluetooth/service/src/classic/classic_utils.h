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

#ifndef CLASSIC_UTILS_H
#define CLASSIC_UTILS_H

#include <vector>
#include <string>

#include "bt_uuid.h"
#include "log.h"

/*
 * @brief The bluetooth system.
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief Classic Utils.
 */
class ClassicUtils {
public:
    /**
     * @brief Convert int to hex string
     *
     * @param  value: std::vector<uint8_t>
     * @return Returns string.
     */
    static std::string ConvertIntToHexString(const std::vector<uint8_t> &value);

    /**
     * @brief Convert hex string to int
     *
     * @param  str: string
     * @return Returns std::vector<uint8_t> value.
     */
    static void ConvertHexStringToInt(const std::string &str, std::vector<uint8_t> &value);

    /**
     * @brief Convert Uuid list to string
     *
     * @param  uuids: the Uuid list
     * @return Returns string value.
     */
    static std::string ConvertUuidToString(const std::vector<Uuid> &uuids);

    /**
     * @brief Convert string to Uuid list
     *
     * @param  value: string
     * @return Returns std::vector<Uuid> value.
     */
    static std::vector<Uuid> ConvertStringToUuid(const std::string &value);

    /**
     * @brief Check the return value.
     *        If the ret is false, output the error log.
     *
     * @param  fileName: file name
     * @param  funcName: function name
     * @param  ret: the return value need to check
     */
    static inline void CheckReturnValue(const std::string &fileName, const std::string &funcName, bool ret)
    {
        if (!ret) {
            LOG_ERROR("[%{public}s]:%{public}s(), ret[%{public}d]", fileName.c_str(), funcName.c_str(), ret);
        }
    }
private:
    /**
     * @brief Convert int to hex string
     *
     * @param  value: uint8_t
     * @return Returns hex string.
     */
    static std::string IntToHexString(uint8_t value);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // CLASSIC_UTILS_H