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

#ifndef PBAP_PSE_OBEX_SERVER_H
#define PBAP_PSE_OBEX_SERVER_H

#include <cstring>
#include <map>
#include <memory>
#include "../obex/obex_headers.h"
#include "../obex/obex_server.h"
#include "../obex/obex_session.h"
#include "../obex/obex_transport.h"
#include "base_def.h"
#include "dispatcher.h"
#include "pbap_pse_def.h"
#include "pbap_pse_service.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief obex server
 * wrap obex server
 */
class PbapPseObexServer {
public:
    /**
     * @brief constructor
     * @details construct with ObexClientConfig
     * @param serverName server Name
     * @param config obex server config
     * @param service pse service
     * @return
     */
    PbapPseObexServer(const std::string &serverName, const ObexServerConfig &config, PbapPseService &service);
    /**
     * @brief deconstructor
     * @details deconstruct
     * @return
     */
    virtual ~PbapPseObexServer() = default;
    /**
     * @brief Start obex server
     * @details Start obex server
     * @return bool @c  0 success
     *              @c -1 failure
     */
    int StartUp() const;
    /**
     * @brief shutdown obex server
     * @details shutdown obex server
     * @return bool @c  0 success
     *              @c -1 failure
     */
    int ShutDown() const;

private:
    /**
     * @brief obex observer
     * wrap obex observer
     */
    class PseObexObserver : public ObexServerObserver {
    public:
        /**
         * @brief constructor
         * @details pseService pse service pointer
         * @return
         */
        explicit PseObexObserver(PbapPseObexServer &obexServer) : pseObexServer_(obexServer){};
        /**
         * @brief Called OnTransportFailed
         * @param incomingConnect incomingConnect
         */
        void OnTransportConnect(ObexIncomingConnect &incomingConnect) override;
        /**
         * @brief Called OnConnected
         * @details Called OnConnected
         * @param session ObexServerSession
         * @param req The Request from Server
         */
        void OnConnect(ObexServerSession &session, const ObexHeader &req) override;
        /**
         * @brief Called OnDisconnected
         * @details Called OnDisconnected
         * @param session ObexServerSession
         * @param req The Request from Server
         */
        void OnDisconnect(ObexServerSession &session, const ObexHeader &req) override;
        /**
         * @brief Called OnGet
         * @details Called OnGet
         * @param session ObexServerSession
         * @param req The Request from Server
         */
        void OnGet(ObexServerSession &session, const ObexHeader &req) override;
        /**
         * @brief Called OnPut
         * @details Called OnPut
         * @param session ObexServerSession
         * @param req The Request from Server
         */
        void OnPut(ObexServerSession &session, const ObexHeader &req) override;
        /**
         * @brief Called OnSetPath
         * @details Called OnSetPath
         * @param session ObexServerSession
         * @param req The Request from Server
         */
        void OnSetPath(ObexServerSession &session, const ObexHeader &req) override;
        /**
         * @brief Called OnTransportDisconnected
         * @details Called OnTransportDisconnected
         * @param btAddr btAddr
         */
        void OnTransportDisconnected(const std::string &btAddr) override;
        /**
         * @brief Called OnConnectFailed
         * @details Called OnConnectFailed
         * @param btAddr btAddr
         * @param errCd error code
         * @param msg error msg
         */
        void OnTransportError(const std::string &btAddr, int errCd, const std::string &msg) override;
        /**
         * @brief Called OnBusy
         * @details Called OnBusy
         * @param session ObexServerSession
         * @param isBusy isBusy
         */
        void OnBusy(ObexServerSession &session, bool isBusy) const override;

    private:
        PbapPseObexServer &pseObexServer_;  // pse obex server
        BT_DISALLOW_COPY_AND_ASSIGN(PseObexObserver);
    };
    ObexServerConfig obexConfig_;                       // obex config
    std::unique_ptr<ObexServer> obexServer_ = nullptr;  // obex observer

    PbapPseService &pbapPseService_;                              // pse service
    std::unique_ptr<PseObexObserver> pseObexObserver_ = nullptr;  // pse obex observer
    BT_DISALLOW_COPY_AND_ASSIGN(PbapPseObexServer);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_OBEX_SERVER_H
