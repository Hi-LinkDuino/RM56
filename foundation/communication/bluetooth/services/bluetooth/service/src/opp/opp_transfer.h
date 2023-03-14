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

#ifndef OPP_TRANSFER_H
#define OPP_TRANSFER_H

#include "opp_obex_client.h"

#include "timer.h"

namespace OHOS {
namespace bluetooth {
class OppTransfer {
public:
    OppTransfer(const std::string &address, const std::vector<std::string> filePaths,
        const std::vector<std::string> mimeTypes, int direction);
    ~OppTransfer();

    void CreateOppTransfer(const std::string &address, const std::vector<std::string> filePaths,
        const std::vector<std::string> mimeTypes, int direction);
    int ConnectObex(const ObexClientConfig &config, utility::Dispatcher &dispatcher);
    int DisconnectObex() const;
    int CancelTransfer();
    IOppTransferInformation GetCurrentTransferInformation();
    int AcceptConnect();
    int SetIncomingFileConfirmation(bool accept);
    int StartTransfer();
    std::string GetDeviceAddress();
    int GetDirection() const;
    int GetFileNumber() const;
    void OnObexDisconnected();
    void OnReceiveIncomingConnect(ObexServerSession &session, uint32_t connectId);
    void OnReceiveIncomingFile(IOppTransferInformation info);
    void OnTransferStateChange(int state, int reason);
    void OnTransferPositionChange(size_t position) const;

private:
    void IncomingConnectTimeout();
    void IncomingFileTimeout();
    void OnTransferStateChangeRunning();
    void OnTransferStateChangeSuccess();
    void OnTransferStateChangeFaild(int reason);
    size_t GetFileLength(std::string filePath) const;
    std::string GetFileNameFromPath(std::string filePath) const;

    std::queue<IOppTransferInformation> fileList_ {};
    std::unique_ptr<IOppTransferInformation> curretTransferInfo_ {nullptr};
    std::unique_ptr<OppObexClient> obexClient_ {nullptr};
    static int currentTransferId_;
    bool isConnected_ = false;
    time_t timeStamp_ = 0;
    int direction_ = OPP_TRANSFER_DIRECTION_OUTBOND;
    int confirm_ = OPP_TRANSFER_CONFIRM_PENDING;
    ObexServerSession *obexSession_ = nullptr;
    uint32_t connectId_ = 0;
    std::string address_;
    std::string deviceName_;
    std::unique_ptr<utility::Timer> incomingConnectTimer_ {nullptr};
    std::unique_ptr<utility::Timer> incomingFileTimer_ {nullptr};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PBAP_PSE_OBEX_SERVER_H
