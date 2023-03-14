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

#ifndef PBAP_PSE_HEADER_MSG_H
#define PBAP_PSE_HEADER_MSG_H

#include <cstring>
#include "../obex/obex_headers.h"
#include "../obex/obex_session.h"
#include "../obex/obex_transport.h"
#include "btstack.h"

namespace OHOS {
namespace bluetooth {
enum PbapPseMessage {
    PSE_DEVICE_CONNECT_INCOMING = 0x1001,  // when connect is incoming
    PSE_INCOMING_ACCEPT,                   // when incoming connect is accept
    PSE_INCOMING_REJECT,                   // when incoming connect is reject
    PSE_PASSWORD_INPUT,                    // when device password is inputted by user
    PSE_WAITING_CONNECTED_TO_DISCONNECT,   // when devices that were accepted but not connected, disable start
    PSE_REQ_OBEX_CONNECT,                  // when obex connect request received  from client
    PSE_DEVICE_CONNECTED,                  // when device is connected
    PSE_REQ_OBEX_GET,                      // when obex get request received  from client
    PSE_REQ_OBEX_SETPATH,                  // when obex setpath request received  from client
    PSE_API_DISCONNECT,                    // when  disconnect request from api
    PSE_REQ_OBEX_DISCONNECT,               // when obex disconnect request received  from client
    PSE_DEVICE_TRANSPORT_ERROR,            // when device transport error
    PSE_DEVICE_DISCONNECTED,               // when device is disconnected
    PSE_SHUTDOWN_COMPLETED                 // when service shutdown completed
};

class PbapPsePasswordInputMsg {
public:
    PbapPsePasswordInputMsg(const std::string &password, const std::string &userId)
        : password_(password), userId_(userId)
    {}
    explicit PbapPsePasswordInputMsg(const std::string &password) : password_(password), userId_("")
    {}
    virtual ~PbapPsePasswordInputMsg() = default;
    const std::string &GetPassword() const
    {
        return password_;
    }
    const std::string &GetUserId() const
    {
        return userId_;
    }

private:
    std::string password_ = "";
    std::string userId_ = "";
};

/**
 * @brief pbap pse message
 * pbap pse message
 */
class PbapPseObexMessage {
public:
    /**
     * @brief constructor
     * @details constructor
     * @param obexSession obex Session
     * @param ObexHeader obex header
     */
    PbapPseObexMessage(ObexServerSession &obexSession, const ObexHeader &header)
        : obexSession_(obexSession), header_(header)
    {}
    virtual ~PbapPseObexMessage() = default;
    /**
     * @brief Get Obex Session
     * @details Get Obex Session
     * @return ObexServerSession
     */
    ObexServerSession &GetObexSession() const
    {
        return obexSession_;
    }
    /**
     * @brief Get Obex header
     * @details Get Obex header
     * @return ObexHeader
     */
    const ObexHeader &GetObexHeader() const
    {
        return header_;
    }

private:
    ObexServerSession &obexSession_;
    const ObexHeader &header_;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_HEADER_MSG_H
