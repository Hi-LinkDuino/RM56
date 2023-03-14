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
 * @file bluetooth_socket_outputstream.h
 *
 * @brief Declares spp inputstream framework functions, including basic functions.
 *
 * @since 6
 */

#ifndef BLUETOOTH_OUTPUTSTREAM_H
#define BLUETOOTH_OUTPUTSTREAM_H

#include "bluetooth_def.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Class for spp output stream functions.
 *
 * @since 6
 */
class BLUETOOTH_API OutputStream {
public:
    /**
     * @brief A constructor used to create an OutputStream instance.
     *
     * @param socketFd Socket fd.
     * @since 6
     */
    OutputStream(int socketFd);

    /**
     * @brief Destroy the OutputStream object.
     *
     * @since 6
     */
    virtual ~OutputStream();

    /**
     * @brief Socket write.
     *
     * @param buf Data to be written.
     * @param length The length of data to be written.
     * @return Returns <b> >0 </b> operation succeeded.The return value is length of the written data.
     *         Returns <b> 0 </b> operation succeeded,but the data length is 0.
     *         Returns <b> -1 </b> operation failed.
     * @since 6
     */
    size_t Write(const char *buf, const size_t length);

private:
    int socketFd_;
    OutputStream() = delete;
};
} // namespace Bluetooth
} // namespace OHOS
#endif  // BLUETOOTH_OUTPUTSTREAM_H