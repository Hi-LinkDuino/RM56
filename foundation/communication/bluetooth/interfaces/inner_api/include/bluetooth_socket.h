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
 * @file bluetooth_socket.h
 *
 * @brief Declares spp socket framework functions, including basic functions.
 *
 * @since 6
 */

#ifndef BLUETOOTH_SOCKET_H
#define BLUETOOTH_SOCKET_H

#include <string>
#include <vector>
#include <memory>

#include "bluetooth_remote_device.h"
#include "bluetooth_socket_inputstream.h"
#include "bluetooth_socket_outputstream.h"

namespace OHOS {
namespace Bluetooth {
enum SppSocketType {
    TYPE_RFCOMM,
    TYPE_L2CAP,
};

enum SppSocketState {
    SOCKET_INIT,
    SOCKET_CONNECTED,
    SOCKET_LISTENING,
    SOCKET_CLOSED,
};

const int FLAG_ENCRYPT = 1;
const int FLAG_AUTH = 1 << 1;

/**
 * @brief Class for spp client socket functions.
 *
 * @since 6
 */
class BLUETOOTH_API SppClientSocket {
public:
    /**
     * @brief A constructor used to create an SppClientSocket instance.
     *
     * @param bda Remote device object.
     * @param uuid Uuid.
     * @param type Socket type.
     * @param auth Connection state.
     * @since 6
     */
    SppClientSocket(const BluetoothRemoteDevice &bda, UUID uuid, SppSocketType type, bool auth);

    /**
     * @brief A constructor used to create an SppClientSocket instance. This constructor to construct the
     * SppClientSocket object when the Accept function is called.
     *
     * @param fd Socket fd.
     * @param address Remote bluetooth address.
     * @since 6
     */
    SppClientSocket(int fd, std::string address);

    /**
     * @brief Destroy the SppClientSocket object.
     *
     * @since 6
     */
    virtual ~SppClientSocket();

    /**
     * @brief The function is used to connect to a remote device.
     *
     * @return Returns <b>0</b> if the operation is successful.
     *         Returns <b>-1</b> if the operation fails.
     * @since 6
     */
    int Connect();

    /**
     * @brief Client disconnected.
     *
     * @since 6
     */
    void Close();

    /**
     * @brief Get the input stream with this socket.
     *
     * @return Returns the object of the InputStream class.
     * @since 6
     */
    InputStream &GetInputStream();

    /**
     * @brief Get the output stream with this socket.
     *
     * @return Returns the object of the OutputStream class.
     * @since 6
     */
    OutputStream &GetOutputStream();

    /**
     * @brief Get the remote device with this socket.
     *
     * @return Remote device.
     * @since 6
     */
    BluetoothRemoteDevice &GetRemoteDevice();

    /**
     * @brief Get the connection status of this socket.
     *
     * @return Returns <b>true</b> is connected.
     *         Returns <b>false</b> is not connected.
     * @since 6
     */
    bool IsConnected() const;

private:
    SppClientSocket() = delete;
    BLUETOOTH_DECLARE_IMPL();
};

/**
 * @brief Class for spp server socket functions.
 *
 * @since 6
 */
class BLUETOOTH_API SppServerSocket {
public:
    /**
     * @brief A constructor used to create an SppServerSocket instance.
     *
     * @param name Server name.
     * @param uuid Uuid.
     * @param type Socket type.
     * @param encrypt Remote device auth and encrypt connection.
     * @since 6
     */
    SppServerSocket(const std::string &name, UUID uuid, SppSocketType type, bool encrypt);

    /**
     * @brief Destroy the SppServerSocket object.
     *
     * @since 6
     */
    ~SppServerSocket();

    /**
     * @brief Accept a client connection and return an acceptClientSocket to interact with the client.
     *
     * @param timeout Timeout for the accept.
     * @return A SppClientSocket.
     * @since 6
     */
    std::unique_ptr<SppClientSocket> Accept(int timeout);

    /**
     * @brief Server disconnected.
     *
     * @since 6
     */
    void Close();

    /**
     * @brief Get the server socket type and server name.
     *
     * @return A string.
     * @since 6
     */
    const std::string &GetStringTag();

private:
    BLUETOOTH_DECLARE_IMPL();
};

class BLUETOOTH_API SocketFactory {
public:
    /**
     * @brief Create a server record to listen to the insecure rfcomm.
     *
     * @param name Server name.
     * @param uuid Uuid.
     * @return A SppServerSocket.
     * @since 6
     */
    static SppServerSocket *DataListenInsecureRfcommByServiceRecord(const std::string &name, const UUID &uuid);

    /**
     * @brief Create a server record to listen to the rfcomm.
     *
     * @param name Server name.
     * @param uuid Uuid.
     * @return A SppServerSocket.
     * @since 6
     */
    static SppServerSocket *DataListenRfcommByServiceRecord(const std::string &name, const UUID &uuid);

    /**
     * @brief Build insecure rfcomm data socket by service record.
     *
     * @param device Remote device object.
     * @param uuid Uuid.
     * @return A SppClientSocket.
     * @since 6
     */
    static SppClientSocket *BuildInsecureRfcommDataSocketByServiceRecord(
        const BluetoothRemoteDevice &device, const UUID &uuid);

    /**
     * @brief Build rfcomm data socket by service record.
     *
     * @param device Remote device object.
     * @param uuid Uuid.
     * @return A SppClientSocket.
     * @since 6
     */
    static SppClientSocket *BuildRfcommDataSocketByServiceRecord(const BluetoothRemoteDevice &device, const UUID &uuid);
};
} // namespace Bluetooth
} // namespace OHOS
#endif  // BLUETOOTH_SOCKET_H
