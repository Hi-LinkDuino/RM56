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

#ifndef OHOS_BT_SPP_H
#define OHOS_BT_SPP_H

#include <stdbool.h>
#include "ohos_bt_def.h"

#define BT_SPP_READ_SOCKET_CLOSED (0)
#define BT_SPP_READ_FAILED (-1)
#define BT_SPP_WRITE_FAILED (-1)
#define BT_SPP_INVALID_ID (-1)

typedef enum {
    OHOS_SPP_SOCKET_RFCOMM = 0x0,
} BtSppSocketType;

typedef struct {
    BtUuid uuid;
    BtSppSocketType socketType;
    bool isEncrypt;
} BtCreateSocketPara;

/**
 * @brief Creates an server listening socket based on the service record.
 *
 * @param socketPara The parameters to create a server socket.
 * @param name The service's name.
 * @param len The length of the service's name.
 * @return Returns a server ID, if create fail return {@link BT_SPP_INVALID_ID}.
 */
int SppServerCreate(BtCreateSocketPara *socketPara, const char *name, unsigned int len);

/**
 * @brief Waits for a remote device to connect to this server socket.
 *
 * This method return a client ID indicates a client socket
 * can be used to read data from and write data to remote device.
 *
 * @param serverId The relative ID used to identify the current server socket, obtain the value by calling
 * {@link SppServerCreate}.
 * @return Returns a client ID, if accept fail return {@link BT_SPP_INVALID_ID}.
 */
int SppServerAccept(int serverId);

/**
 * @brief Disables an spp server socket and releases related resources.
 *
 * @param serverId The relative ID used to identify the current server socket, obtain the value by calling
 * {@link SppServerCreate}.
 * @return Returns the operation result status {@link BtStatus}.
 */
int SppServerClose(int serverId);

/**
 * @brief Connects to a remote device over the socket.
 *
 * @param socketPara The param to create a client socket and connect to a remote device.
 * @return Returns a client ID, if connect fail return {@link BT_SPP_INVALID_ID}.
 */
int SppConnect(BtCreateSocketPara *socketPara, const BdAddr *bdAddr);

/**
 * @brief Disables a connection and releases related resources.
 *
 * @param clientId The relative ID used to identify the current client socket.
 * @return Returns the operation result status {@link BtStatus}.
 */
int SppDisconnect(int clientId);

/**
 * @brief Get the connection status of this socket.
 *
 * @param clientId The relative ID used to identify the current client socket.
 * @return Returns true is connected or false is not connected.
 */
bool IsSppConnected(int clientId);

/**
 * @brief Spp get remote device's address.
 *
 * @param clientId The relative ID used to identify the current client socket.
 * @param remoteAddr Remote device's address, memory allocated by caller.
 * @return Returns the operation result status {@link BtStatus}.
 */
int SppGetRemoteAddr(int clientId, BdAddr *remoteAddr);

/**
 * @brief Read data from socket.
 *
 * @param clientId The relative ID used to identify the current client socket.
 * @param buf Indicate the buffer which read in, memory allocated by caller.
 * @param bufLen Indicate the buffer length.
 * @return Returns the length greater than 0 as read the actual length.
 * Returns {@link BT_SPP_READ_SOCKET_CLOSED} if the socket is closed.
 * Returns {@link BT_SPP_READ_FAILED} if the operation failed.
 */
int SppRead(int clientId, char *buf, const unsigned int bufLen);

/**
 * @brief Client write data to socket.
 *
 * @param clientId The relative ID used to identify the current client socket.
 * @param data Indicate the data to be written.
 * @param len Indicates the length of the data to be written.
 * @return Returns the actual write length.
 * Returns {@link BT_SPP_WRITE_FAILED} if the operation failed.
 */
int SppWrite(int clientId, const char *data, const unsigned int len);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif