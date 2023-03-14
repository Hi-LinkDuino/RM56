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

#ifndef PBAP_PCE_OBEX_CLIENT_H
#define PBAP_PCE_OBEX_CLIENT_H

#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include "../obex/obex_body.h"
#include "../obex/obex_client.h"
#include "../obex/obex_headers.h"
#include "../obex/obex_mp_client.h"
#include "message.h"
#include "pbap_pce_header_msg.h"
#include "pbap_pce_service.h"

namespace OHOS {
namespace bluetooth {
class ObexFileBodyObject : public ObexBodyObject {
public:
    explicit ObexFileBodyObject(const std::string &file);
    ObexFileBodyObject() = default;
    ~ObexFileBodyObject() override = default;
    size_t Read(uint8_t *buf, size_t bufLen) override;
    size_t Write(const uint8_t *buf, size_t bufLen) override;
    int Close() override;

private:
    void OpenOutFile(const std::string &file);
    std::ofstream ofs_ {};
};

/**
 * @brief obex client
 * wrap obex client
 */
class PbapPceObexClient {
public:
    /**
     * @brief constructor
     * @details construct with ObexClientConfig
     * @param config obex client config
     * @return
     */
    explicit PbapPceObexClient(const ObexClientConfig &config, PbapPceService &pceService);

    /**
     * @brief destructor
     * @details constructor
     */
    virtual ~PbapPceObexClient();

    /**
     * @brief connect to obex client
     * @details connect to obex client
     * @return @c  0 success
     *         @c -1 failure
     */
    int Connect(bool supported);

    /**
     * @brief connect to obex client again
     * @details connect to obex client again
     * @param challenge challenge header
     * @param digest digest header
     * @return @c  0 success
     *         @c -1 failure
     */
    int Reconnect(ObexDigestChallenge &challenge, ObexDigestResponse &digest);

    /**
     * @brief send obex disconnect request
     * @param withObexReq is send obex disconnect request
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Disconnect(bool withObexReq = true) const;

    /**
     * @brief disconnect to obex client
     * @details disconnect to obex client
     * @param req challenge header
     * @param reqMsgType request message type
     * @return @c  0 success
     *         @c -1 failure
     */
    int Get(const ObexHeader &req, int reqMsgType) const;
    /**
     * @brief set path
     * @details set path
     * @param flag go direction
     * @param path relative path or absolute path
     * @return @c  0 success
     *         @c -1 failure
     */
    int SetPath(uint8_t flag, const std::u16string &path) const;
    /**
     * @brief abort downloading phonebook
     * @details abort downloading phonebook
     * @return @c  0 success
     *         @c -1 failure
     */
    int Abort() const;

    /**
     * @brief GetClient
     * @return pointer to ObexMpClient
     */
    ObexMpClient *GetClient() const;

    struct PhoneBookActionInfo {
        int operationId_ = 0;        // pce last operation id
        int reqMsgType_ = 0;         // pce last command id
        std::u16string path_ = u"";  // pce last command path
        uint8_t flags_ = 0;          // pce last flags
        bool isAbort = false;
    };
    /**
     * @brief obex observer
     * wrap obex observer
     */
    class PceObexObserver : public ObexClientObserver {
    public:
        /**
         * @brief constructor
         * @details pceService pce service pointer
         * @return
         */
        explicit PceObexObserver(PbapPceService &pceService) : pceService_(pceService)
        {}

        /**
         * @brief destructor
         * @details constructor
         */
        virtual ~PceObexObserver()
        {}

        /**
         * @brief Called OnTransportFailed
         * @param client ObexClient
         * @param errCd error code
         */
        void OnTransportFailed(ObexClient &client, int errCd) override;

        /**
         * @brief Called OnConnected
         * @details Called OnConnected
         * @param client ObexClient
         * @param resp The Response from Server
         */
        void OnConnected(ObexClient &client, const ObexHeader &resp) override;

        /**
         * @brief Called OnConnectFailed
         * @details Called OnConnectFailed
         * @param client ObexClient
         * @param resp The Response from Server
         */
        void OnConnectFailed(ObexClient &client, const ObexHeader &resp) override;

        /**
         * @brief Called OnDisconnected
         * @details Called OnDisconnected
         * @param client ObexClient
         */
        void OnDisconnected(ObexClient &client) override;

        /**
         * @brief Called OnActionCompleted
         * @details Called OnActionCompleted
         * @param client ObexClient
         * @param resp The Response from Server
         */
        void OnActionCompleted(ObexClient &client, const ObexHeader &resp) override;

        /**
         * @brief Called OnBusy
         *
         * @param client ObexClient
         * @param isBusy true:become busy false:become not busy
         */
        void OnBusy(ObexClient &client, bool isBusy) override;

        /**
         * @brief Get PhoneBook Action Info
         * @details Get PhoneBook Action Info
         */
        bool GetPhoneBookActionInfo(
            int operationId, int &retReqMsgType, std::u16string &retPath, uint8_t &retFlags) const;

        /**
         * @brief Set PhoneBook Action Info
         * @details Set PhoneBook Action Info when action starting
         */
        void SetPhoneBookActionInfo(int operationId, int reqMsgType, const std::u16string &path = u"",
            uint8_t flags = 0, bool isBusy = true);

        PbapPceService &pceService_;                  // pce service pointer
        std::atomic_bool isBusy_ {false};             // pce start downloading;
        std::atomic_bool isAbort_ {false};            // pce abort downloading;
        PhoneBookActionInfo phoneBookActionInfo_ {};  // action info
    };

    /**
     * @brief Set Busy
     * @details Set Busy as proxy observer_
     */
    void SetBusy(bool isBusy) const;

    /**
     * @brief Is Busy
     * @details Is Busy as proxy observer_
     */
    bool IsBusy() const;

    /**
     * @brief Set Abort
     * @details Set Abort as proxy observer_
     */
    void SetAbort(bool isAbort) const;

private:
    std::unique_ptr<PceObexObserver> observer_ {nullptr};  // obex observer
    ObexClientConfig obexConfig_;                          // obex config
    PbapPceService &pceService_;                           // pce service
    std::unique_ptr<ObexMpClient> client_ {nullptr};       // obex client

    // pce feature
    static const uint32_t PBAP_PCE_FEATURES = PBAP_PCE_FEATURES_DOWNLOAD |
        PBAP_PCE_FEATURES_BROWSING |
        PBAP_PCE_FEATURES_DATABASE_IDENTIFIER |
        PBAP_PCE_FEATURES_FOLDER_VERSION_COUNTERS |
        PBAP_PCE_FEATURES_VCARD_SELECTING |
        PBAP_PCE_FEATURES_ENHANCED_MISSED_CALLS |
        PBAP_PCE_FEATURES_X_BT_UCI_VCARD_PROPERTY |
        PBAP_PCE_FEATURES_X_BT_UID_VCARD_PROPERTY |
        PBAP_PCE_FEATURES_CONTACT_REFERENCING |
        PBAP_PCE_FEATURES_DEFAULT_CONTACT_IMAGE_FORMAT;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PCE_OBEX_CLIENT_H
