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

#ifndef OBEX_CLIENT_H
#define OBEX_CLIENT_H

#include <cstdint>
#include <cstring>
#include "btstack.h"
#include "dispatcher.h"
#include "obex_body.h"
#include "obex_headers.h"
#include "obex_session.h"
#include "obex_transport.h"
#include "obex_types.h"

namespace OHOS {
namespace bluetooth {
enum class ObexClientState : uint8_t {
    INIT = 0,
    TRANSPORT_CONNECTED,
    RELIABLE_SESSION_CREATED,
    OBEX_CONNECTED,
    OBEX_DISCONNECTED,
    RELIABLE_SESSION_SUSPENDED,
    RELIABLE_SESSION_RESUMED,
    RELIABLE_SESSION_CLOSED,
    TRANSPORT_DISCONNECTED
};

struct ObexClientConfig {
    BtAddr addr_ {};                         // Remote bluetooth address
    uint16_t lpsm_ = 0;                      // l2cap's local psm while use l2cap
    uint16_t scn_ = 0;                       // Rfcomm's channel num/ l2cap's psm
    uint16_t mtu_ = 0;                       // The Maximum OBEX Packet Length.Default 1024byte
    bool isGoepL2capPSM_ = false;            // L2cap:true, rfcomm:false
    bool isSupportSrm_ = false;              // Using Single Response Mode
    bool isSupportReliableSession_ = false;  // Using reliable session
    BtUuid serviceUUID_ {};                  // Service's UUID128
};

struct ObexConnectParams {
    ObexTlvParamters *appParams_ = nullptr;          // Application Parameters
    ObexDigestChallenge *authChallenges_ = nullptr;  // Auth Challenges
    ObexDigestResponse *authResponses_ = nullptr;    // Auth Responses
    uint32_t *count = nullptr;                      // Count
};

class ObexClient;
/**
 * @brief Callback function \n
 * used to notify the upper layer of the event and related information.
 */
class ObexClientObserver {
public:
    virtual ~ObexClientObserver() = default;
    /**
     * @brief Called OnTransportFailed
     * @param client ObexClient
     * @param errCd error code
     */
    virtual void OnTransportFailed(ObexClient &client, int errCd) = 0;
    /**
     * @brief Called OnConnected
     *
     * @param client ObexClient
     * @param resp The Response from Server
     */
    virtual void OnConnected(ObexClient &client, const ObexHeader &resp) = 0;
    /**
     * @brief Called OnConnectFailed
     *
     * @param client ObexClient
     * @param resp The Response from Server
     */
    virtual void OnConnectFailed(ObexClient &client, const ObexHeader &resp) = 0;
    /**
     * @brief Called OnDisconnected
     *
     * @param client ObexClient
     */
    virtual void OnDisconnected(ObexClient &client) = 0;
    /**
     * @brief Called OnActionCompleted
     *
     * @param client ObexClient
     * @param resp The Response from Server
     */
    virtual void OnActionCompleted(ObexClient &client, const ObexHeader &resp) = 0;
    /**
     * @brief Called OnBusy
     *
     * @param client ObexClient
     * @param isBusy true:become busy false:become not busy
     */
    virtual void OnBusy(ObexClient &client, bool isBusy);
};

/**
 *  @brief ObexClient\n
 *  ObexClient
 */
class ObexClient {
public:
    // create obex client
    explicit ObexClient(const ObexClientConfig &config, ObexClientObserver &observer, utility::Dispatcher &dispatcher);

    // destroy obex client
    virtual ~ObexClient() = default;

    /**
     * @brief send obex request
     *
     * @param req the data of obex request
     * @return int Request processing result:0:succeeded -1:failed
     */
    int SendRequest(const ObexHeader &req);

    /**
     * @brief send obex connect request
     *
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Connect();
    /**
     * @brief send obex connect request with Parameters
     *
     * @param connectParams Connect Parameters
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Connect(ObexConnectParams &connectParams);
    /**
     * @brief send obex disconnect request
     * @param withObexReq is send obex disconnect request
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Disconnect(bool withObexReq = true);
    /**
     * @brief send obex abort request
     *
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Abort();

    /**
     * @brief send obex put request
     *
     * @param req the data of obex request
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Put(const ObexHeader &req);

    /**
     * @brief send obex get request
     *
     * @param req the data of obex request for get
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Get(const ObexHeader &req);

    /**
     * @brief send obex set_path request
     *
     * @param flag SETPATH flag \n
     *             bit 0:backup a level before applying name (equivalent to ../ on many systems)\n
     *                   @see OBEX_SETPATH_BACKUP
     *             bit 1:Don’t create folder if it does not exist, return an error instead.
     *                   @see OBEX_SETPATH_NOCREATE
     * @param path path
     * @return int Request processing result:0:succeeded -1:failed
     */
    int SetPath(uint8_t flag, const std::u16string &path);

    /**
     * @brief send obex copy action request
     *
     * @param srcName Copy source name
     * @param destName Copy destination name
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Copy(const std::u16string &srcName, const std::u16string &destName);
    /**
     * @brief send obex move action request
     *
     * @param srcName Move source name
     * @param destName Move destination name
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Move(const std::u16string &srcName, const std::u16string &destName);
    /**
     * @brief send obex set permissions action request
     *
     * @param name Indicates the name of the object to perform the action upon.
     * @param permissions Indicates the permissions for a Set Permissions action.
     * @return int Request processing result:0:succeeded -1:failed
     */
    int SetPermissions(const std::u16string &name, const uint32_t permissions);

    /**
     * @brief send obex CREATESESSION request \n
     * The CREATESESSION command is sent by the Client to create a new session.
     * @return int Request processing result:0:succeeded -1:failed
     */
    int CreateSession();
    /**
     * @brief send obex CREATESESSION request \n
     * The CREATESESSION command is sent by the Client to create a new session.
     * @param timeoutSec  timeout specifying the number of seconds.
     *                    default is OBEX_SESSION_MAX_TIMEOUT_SEC
     * @return int Request processing result:0:succeeded -1:failed
     */
    static int CreateSession(uint32_t timeoutSec);

    /**
     * @brief send obex SUSPENDSESSION request \n
     * SUSPENDSESSION is used to gracefully suspend the active session.
     * @return int Request processing result:0:succeeded -1:failed
     */
    static int SuspendSession();

    /**
     * @brief send obex RESUMESESSION request \n
     * RESUMESESSION is used to resume a session that has been suspended.
     * @return int Request processing result:0:succeeded -1:failed
     */
    static int ResumeSession();

    /**
     * @brief send obex CLOSESESSION request \n
     * CLOSESESSION is used to gracefully close an existing session.
     * @return int Request processing result:0:succeeded -1:failed
     */
    static int CloseSession();

    /**
     * @brief send obex SETTIMEOUT request \n
     * The SETTIMEOUT command is used to negotiate a new timeout specifying the number of seconds a \n
     * session should be maintained while the session is suspended.
     * @param timeoutSec timeout specifying the number of seconds
     * @return int Request processing result:0:succeeded -1:failed
     */
    static int SetSessionTimeout(uint32_t timeoutSec = OBEX_SESSION_MAX_TIMEOUT_SEC);

    // Get ClientSession
    ObexClientSession &GetClientSession() const;

    // Get GetClientId
    const std::string &GetClientId();

    static int RegisterL2capLPsm(uint16_t lpsm);
    static void DeregisterL2capLPsm(uint16_t lpsm);

protected:
    // Client Transport Obsverver
    class ObexClientTransportObserver : public ObexTransportObserver {
    public:
        explicit ObexClientTransportObserver(ObexClient &obexClient);
        ~ObexClientTransportObserver() override = default;
        // call back from transport
        void OnTransportConnected(ObexTransport &transport) override;
        void OnTransportDisconnected(ObexTransport &transport) override;
        void OnTransportDataAvailable(ObexTransport &transport, ObexPacket &obexPacket) override;
        void OnTransportDataBusy(ObexTransport &transport, uint8_t isBusy) override;
        void OnTransportError(ObexTransport &transport, int errCd) override;

    private:
        ObexClient &obexClient_;
        std::unique_ptr<bluetooth::ObexHeader> GetObexHeaderFromPacket(ObexPacket &obexPacket);
        void HandleDataAvailableConnect(const ObexHeader &resp);
        void HandleDataAvailableDisconnect(const ObexHeader &resp);
        void HandleDataAvailablePut(const ObexHeader &resp);
        void HandleDataAvailableGet(const ObexHeader &resp);
        void HandleDataAvailableSetPath(const ObexHeader &resp);
        void HandleDataAvailableSession(const ObexHeader &resp);
        void HandleDataAvailableAbort(const ObexHeader &resp);
        void HandleDataAvailableAction(const ObexHeader &resp);
        BT_DISALLOW_COPY_AND_ASSIGN(ObexClientTransportObserver);
    };

    int ProcessAbort();
    int SendAbortRequest();
    int SendConnectRequest(ObexHeader &header);
    bool CheckBeforeRequest(uint8_t opeId) const;
    static bool CheckBeforeSession(uint8_t sessionOpcode);
    virtual void PutDataAvailable(const ObexHeader &resp);
    virtual void GetDataAvailable(const ObexHeader &resp);
    virtual void SetPathDataAvailable(const ObexHeader &resp);
    virtual void AbortDataAvailable(const ObexHeader &resp);
    virtual void HandleTransportDataBusy(uint8_t isBusy);
    virtual void SetBusy(bool isBusy);
    std::unique_ptr<ObexClientTransportObserver> transportObserver_ = nullptr;
    std::unique_ptr<ObexClientSession> clientSession_ = nullptr;
    std::unique_ptr<ObexClientTransport> clientTransport_ = nullptr;
    std::unique_ptr<ObexHeader> connectReqHeader_ = nullptr;
    std::unique_ptr<ObexHeader> reliableSessionReqHeader_ = nullptr;
    ObexClientObserver &clientObserver_;
    ObexClientState clientState_ = ObexClientState::INIT;
    bool isObexConnected_ = false;
    bool isWaitingSendAbort_ = false;
    bool isAbortSended_ = false;
    bool isReliableSessionCreated_ = false;
    bool isSupportSrm_ = false;
    bool isSupportReliableSession_ = false;
    bool isProcessing_ = false;
    std::string clientId_ = "";
    utility::Dispatcher &dispatcher_;
    BT_DISALLOW_COPY_AND_ASSIGN(ObexClient);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_CLIENT_H