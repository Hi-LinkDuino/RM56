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

#ifndef MAP_MSE_SERVER_H
#define MAP_MSE_SERVER_H

#include <string>
#include <unordered_map>
#include "../obex/obex_mp_server.h"
#include "../obex/obex_utils.h"
#include "dispatcher.h"
#include "log.h"
#include "map_mse_bmsg.h"
#include "map_mse_params.h"
#include "map_mse_resource.h"
#include "map_mse_types.h"
#include "raw_address.h"

/**
 * @brief bluetooth namespace
 *
 */
namespace OHOS {
namespace bluetooth {
/**
 * @brief Callback function of MapMseServer
 *
 */
class MseObserver {
public:
    /**
     * @brief A destructor used to delete the Mse Observer instance.
     *
     */
    virtual ~MseObserver() = default;

    /**
     * @brief The function is called when new MCE connection available
     *
     * @param session Reference to the ObexServerSession Value
     */
    virtual void OnConnected(ObexServerSession &session) = 0;

    /**
     * @brief The function is called when MCE connect is incoming
     *
     * @param incomingConnect Reference to the ObexIncomingConnect Value
     */
    virtual void OnIncomingConnect(ObexIncomingConnect &incomingConnect) = 0;

    /**
     * @brief The function is called when MCE Transport Disconnect is incoming
     *
     * @param remoteAddr Remote Bluetooth address
     */
    virtual void OnTransportDisconnected(const RawAddress &remoteAddr) = 0;

    /**
     * @brief The function is called when MCE Disconnect is incoming
     *
     * @param remoteAddr Remote Bluetooth address
     */
    virtual void OnDisconnect(const RawAddress &remoteAddr) = 0;

    /**
     * @brief The function is called when Obex busy or not busy
     *
     * @param remoteAddr Remote Bluetooth address
     * @param isBusy true:busy false:not busy
     */
    virtual void OnBusy(const RawAddress &remoteAddr, bool isBusy) = 0;
};

/**
 * @brief MSE server class
 * used to start OBEX service and respond to MCE requests
 *
 */
class MapMseServer {
public:
    /**
     * @brief Construct a new Mse Server:: Mse Server object
     *
     * @param observer Callback function
     */
    explicit MapMseServer(MapMseResource &content, utility::Dispatcher &dispatcher, MseObserver &observer,
        MapMseInstance &instance, uint8_t rfcommNo, uint16_t l2capPsm, std::string serviceName,
        const MapAccountItem &accountItem, bool smsSupport);

    /**
     * @brief Destroy the Mse Server:: Mse Server object
     *
     */
    virtual ~MapMseServer();

    /**
     * @brief The function start MSE stack in asynchronization mode
     *
     * @return int
     */
    int Enable(void);

    /**
     * @brief The function shutdown MSE stack
     *
     */
    void Disable(void);

    void SetRemoteFeatureMask(uint32_t features);

private:
    static const uint8_t TARGET_SIZE = 16;
    static const uint8_t MAS_TARGET[TARGET_SIZE];
    static const std::string HEADER_TYPE_GET_FOLDER_LISTING;
    static const std::string HEADER_TYPE_GET_MESSAGE_LISTING;
    static const std::string HEADER_TYPE_GET_CONVO_LISTING;
    static const std::string HEADER_TYPE_MESSAGE;
    static const std::string HEADER_TYPE_SET_MESSAGE_STATUS;
    static const std::string HEADER_TYPE_SET_NOTIFICATION_REGISTRATION;
    static const std::string HEADER_TYPE_MESSAGE_UPDATE;
    static const std::string HEADER_TYPE_GET_MAS_INSTANCE_INFORMATION;
    static const std::string HEADER_TYPE_OWNER_STATUS;
    static const std::string HEADER_TYPE_SET_NOTIFICATION_FILTER;
    static const int MAP_MAS_INSTANCE_INFORMATION_LENGTH = 200;
    static const int READ_BODY_BUFF_SIZE = 100;
    static const int DATABASE_VERSION_COUNTER_LENGTH = 128;
    class MseObexServer : public ObexServerObserver {
    public:
        /**
         * @brief Construct a new Mse Obex Server object
         *
         * @param mseServer Reference to the Mse Server
         */
        explicit MseObexServer(MapMseServer &mseServer);

        /**
         * @brief Destroy the Mse Obex Server object
         *
         */
        ~MseObexServer() override;

    private:
        /**
         * @brief Callback implementation for the OBex OnTransportConnect function
         *
         * @param incomingConnect Reference to the ObexIncomingConnect
         */
        void OnTransportConnect(ObexIncomingConnect &incomingConnect) override;

        void OnTransportDisconnected(const std::string &btAddr) override;

        void OnTransportError(const std::string &btAddr, int errCd, const std::string &msg) override;

        void OnError(const int errCd, const std::string &msg) override;

        /**
         * @brief Response to MAS connection function and OBEX connection callback function
         *
         * @param session Obex session
         * @param req Obex connection request
         */
        void OnConnect(ObexServerSession &session, const ObexHeader &req) override;

        void OnDisconnect(ObexServerSession &session, const ObexHeader &req) override;

        void OnPut(ObexServerSession &session, const ObexHeader &req) override;

        /**
         * @brief Callback implementation for the OBex onGet function
         *
         * @param session obex session
         * @param req obex headers request
         */
        void OnGet(ObexServerSession &session, const ObexHeader &req) override;

        void OnAbort(ObexServerSession &session, const ObexHeader &req) override;

        void OnSetPath(ObexServerSession &session, const ObexHeader &req) override;

        void OnBusy(ObexServerSession &session, bool isBusy) const override;

        MapMseServer *mseServer_;
        BT_DISALLOW_COPY_AND_ASSIGN(MseObexServer);
    };

    std::unique_ptr<MseObexServer> mseObexServer_ = nullptr;
    /**
     * @brief Response processing of messages
     *
     * @param session obex server session
     * @param req obex headers request
     */
    void GetMessageResponse(ObexServerSession &session, const ObexHeader &req);
    void GetMessageListing(ObexServerSession &session, const ObexHeader &req);
    void CreateOutputAppPrarams(ObexTlvParamters &obexAppPrarams, const uint16_t &listSize, const uint8_t &unRead);
    void GetConversationListing(ObexServerSession &session, const ObexHeader &req);
    void GetFolderListing(ObexServerSession &session, const ObexHeader &req);
    void GetOwnerStatus(const ObexServerSession &session, const ObexHeader &req);
    void GetMASInstanceInformation(ObexServerSession &session, const ObexHeader &req);
    void GetImEmailMasInfo(std::string &masStr, std::string &ownerUci);
    void PushMessageResponse(ObexServerSession &session, const ObexHeader &req);
    int CreateBMessage(ObexServerSession &session, const ObexHeader &req, const uint8_t charset,
        std::string &folderName);
    void SetNotificationRegistration(const ObexServerSession &session, const ObexHeader &req);
    void SetNotificationFilter(const ObexServerSession &session, const ObexHeader &req);
    void SetMessageStatus(const ObexServerSession &session, const ObexHeader &req);
    void UpdateInbox(const ObexServerSession &session, const ObexHeader &req);
    int SetStatusParam(const MapMseParams &appParams, stub::OwnerStatusParam &statusParam);
    void SetOwnerStatus(const ObexServerSession &session, const ObexHeader &req);
    void CreateFolder(void);
    static void InitDefaultFolder(MapMseFolder &root);
    static void InitSmsMmsFolder(MapMseFolder &root);
    static void InitImFolder(MapMseFolder &root);
    void InitEmailFolder(MapMseFolder &parentFolder, int depth = 0);
    static std::unordered_map<int, std::string> GetEmailInfo(
        std::unique_ptr<DataAccess> &dataAccess, MapMseFolder &parent);
    static std::string ToUtf8String(std::u16string u16str);
    static std::u16string ToU16String(std::string str);
    static std::string GetMseTime(void);
    static bool CheckConversationId(std::string id);
    void LoadAdapterConfig();

    MapMseFolder *currentFolder_ = nullptr;

    std::unique_ptr<MapMseFolder> rootFolder_ = nullptr;

    MapMseResource &content_;

    utility::Dispatcher &dispatcher_;

    MseObserver &observer_;

    MapMseInstance &instance_;

    // SDP Register FRCOMM No
    uint8_t rfcommNo_ = 0;

    // SDP Register GOEP L2cap Psm
    uint16_t goepL2capPsm_ = 0;

    // SDP Register Service Name
    std::string serviceName_ = "";

    MapAccountItem accountItem_ {};

    bool smsSupport_ = false;

    std::unique_ptr<ObexServer> obexServer_ = nullptr;

    uint32_t supportedFeatureMask_ = 0;

    std::string messageVersion_ = "";

    std::string messageListingVersion_ = "";

    static MessageType ParseHandle(uint64_t handle);

    std::unique_ptr<MapMseBmsg> bMessage_ = nullptr;

    std::atomic_uint32_t connectId_ = 0;

    bool srmEnable_ = true;

    int rfcommMtu_ = 0;

    int l2capMtu_ = 0;

    BT_DISALLOW_COPY_AND_ASSIGN(MapMseServer);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_SERVER_H
