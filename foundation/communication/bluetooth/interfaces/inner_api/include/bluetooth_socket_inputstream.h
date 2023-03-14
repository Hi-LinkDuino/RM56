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
 * @brief Defines a bluetooth system that provides basic blurtooth connection and profile functions,
 *        including A2DP, AVRCP, BLE, GATT, HFP, MAP, PBAP, and SPP, etc.
 *
 * @since 6
 */

/**
 * @file bluetooth_socket_inputstream.h
 *
 * @brief Declares spp outputstream framework functions, including basic functions.
 *
 * @since 6
 */

#ifndef BLUETOOTH_INPUTSTREAM_H
#define BLUETOOTH_INPUTSTREAM_H

#include "bluetooth_def.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Class for spp input stream functions.
 *
 * @since 6
 */
class BLUETOOTH_API InputStream {
public:
    /**
     * @brief A constructor used to create an inputStream instance.
     *
     * @param socketFd Socket fd.
     * @since 6
     */
    InputStream(int socketFd);

    /**
     * @brief Destroy the inputStream object.
     *
     * @since 6
     */
    virtual ~InputStream();

    /**
     * @brief Socket read.
     *
     * @param buf Data to be read.
     * @param length The length of data to be read.
     * @return Returns the length greater than <b>0</b> as read the actual length.
     *         Returns <b>0</b> if the socket is closed.
     *         Returns <b>-1</b> if the operation fails.
     * @since 6
     */
    ssize_t Read(char *buf, const size_t length);

private:
    int socketFd_;
    InputStream() = delete;
};
} // namespace Bluetooth
} // namespace OHOS
#endif  // BLUETOOTH_INPUTSTREAM_H