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

#ifndef WIFI_C_UTILS_H_
#define WIFI_C_UTILS_H_

#include <string>
#include <vector>
#include "native_c/wifi_device_config.h"
#include "native_c/wifi_error_code.h"
#include "wifi_errcode.h"

#ifndef IPV4_ARRAY_LEN
#define IPV4_ARRAY_LEN 4
#endif

namespace OHOS {
namespace Wifi {
#ifndef CHECK_PTR_RETURN
#define CHECK_PTR_RETURN(ptr, retValue)             \
    if ((ptr) == nullptr) {                         \
        WIFI_LOGE("Error: the ptr is null!");       \
        return retValue;                            \
    }
#endif

/**
 * @Description Convert c++ error code to c error code.
 *
 * @param errCode - c++ error code
 * @return WifiErrorCode - c error code
 */
WifiErrorCode GetCErrorCode(ErrCode errCode);

/**
 * @Description Convert IP address from string to int array.
 *
 * @param str - IP address of string type
 * @param ipAddr - Convert result which is a 4-bit int array, example: 127.0.0.1 -> ipAddr[ 127, 0, 0, 1 ]
 * @return WifiErrorCode - operate result
 */
WifiErrorCode IpStrToArray(const std::string& str, unsigned int ipAddr[IPV4_ARRAY_LEN]);

/**
 * @Description Convert IP address from int array to string.
 *    example: ipAddr[ 127, 0, 0, 1 ] -> 127.0.0.1
 *
 * @param ipAddr - IP address of int array
 * @return std::string - result
 */
std::string IpArrayToStr(const unsigned int ipAddr[IPV4_ARRAY_LEN]);
}  // namespace Wifi
}  // namespace OHOS

#endif
