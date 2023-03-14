/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OPP_OBEX_CLIENT_H
#define OPP_OBEX_CLIENT_H

#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include "../obex/obex_body.h"
#include "../obex/obex_client.h"
#include "../obex/obex_headers.h"
#include "context.h"
#include "opp_defines.h"
#include "opp_transfer_information.h"

namespace OHOS {
namespace bluetooth {
class OppSendFileBodyObject : public ObexBodyObject {
public:
    explicit OppSendFileBodyObject(const std::string &file);
    OppSendFileBodyObject() = default;
    ~OppSendFileBodyObject() override;
    size_t Read(uint8_t *buf, size_t bufLen) override;
    size_t Write(const uint8_t *buf, size_t bufLen) override;
    int Close() override;
    size_t GetFileSize() const;
    size_t GetFileSendSize() const;

private:
    void OpenFile(const std::string &file);
    std::ifstream ifs_ {};
    size_t fileSize_ = 0;
    size_t fileSendSize_ = 0;
};

class OppObexClient {
public:
    explicit OppObexClient(const ObexClientConfig &config, utility::Dispatcher &dispatcher);
    virtual ~OppObexClient();
    int Connect(uint32_t fileCount) const;
    int Disconnect(bool withObexReq = true) const;
    int CancelSendFile();
    int SendFile(IOppTransferInformation fileInfo);

    void OnTransportFailed(const ObexClient &client, int errCd);
    void OnConnected(const ObexClient &client, const ObexHeader &resp);
    void OnConnectFailed(const ObexClient &client);
    void OnDisconnected(const ObexClient &client);
    void OnActionCompleted(const ObexClient &client, const ObexHeader &resp);
    void OnBusy(ObexClient &client, bool isBusy);

    class OppObexObserver : public ObexClientObserver {
    public:
        explicit OppObexObserver(OppObexClient *oppObexClient) : oppObexClient_(oppObexClient) {}
        ~OppObexObserver() override {}
        void OnTransportFailed(ObexClient &client, int errCd) override;
        void OnConnected(ObexClient &client, const ObexHeader &resp) override;
        void OnConnectFailed(ObexClient &client, const ObexHeader &resp) override;
        void OnDisconnected(ObexClient &client) override;
        void OnActionCompleted(ObexClient &client, const ObexHeader &resp) override;
        void OnBusy(ObexClient &client, bool isBusy) override;

        OppObexClient *oppObexClient_ {nullptr};
    };

private:
    std::unique_ptr<OppObexObserver> observer_ {nullptr};  // obex observer
    std::unique_ptr<ObexClient> client_ {nullptr};         // obex client
    std::shared_ptr<ObexBodyObject> fileObject_ {nullptr};
    uint32_t connectionId_ = 0;
    bool isBusy_ = false;
    bool startSendFile_ = false;
    bool sendAbort_ = false;
    int status_ = OPP_OBEX_STATUS_IDLE;
    std::mutex mutexBusyChanged_;
    std::condition_variable cvWaitBusyChanged_;
    std::unique_ptr<utility::Dispatcher> SendFileThread_ {};
    std::string address_;
    bool isRegisterL2capLPsm_ = false;
    uint16_t lpsm_ = 0;
    bool isSupportSrm_ = false;

    void SendFileBody();
    void SendOppDisconnected();
    void SendOppConnected();
    void OnTransferStateChangeFaild(int reason);
    std::u16string StringToU16string(const std::string &str) const;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OPP_OBEX_CLIENT_H
