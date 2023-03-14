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

#ifndef OPP_OBEX_SERVER_H
#define OPP_OBEX_SERVER_H

#include <cstring>
#include <fstream>
#include <memory>
#include "../obex/obex_headers.h"
#include "../obex/obex_server.h"
#include "../obex/obex_session.h"
#include "../obex/obex_transport.h"
#include "context.h"
#include "base_def.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
class OppReceiveFileBodyObject : public ObexBodyObject {
public:
    explicit OppReceiveFileBodyObject(const std::string &file, const std::string address);
    OppReceiveFileBodyObject() = default;
    ~OppReceiveFileBodyObject() override;
    size_t Read(uint8_t *buf, size_t bufLen) override;
    size_t Write(const uint8_t *buf, size_t bufLen) override;
    int Close() override;
    void SetFileReceiveSuccess();

private:
    void OpenFile(const std::string &file);
    std::ofstream ofs_ {};
    size_t fileReceiveSize_ = 0;
    std::string address_;
    std::string file_;
    bool fileReceiveSuccess_ = false;
};

class OppObexServer {
public:
    OppObexServer(const ObexServerConfig &config, utility::Dispatcher &dispatcher);
    virtual ~OppObexServer() = default;
    int StartUp() const;
    int ShutDown() const;

private:
    class OppObexObserver : public ObexServerObserver {
    public:
        void OnTransportConnect(ObexIncomingConnect &incomingConnect) override;
        void OnConnect(ObexServerSession &session, const ObexHeader &req) override;
        void OnDisconnect(ObexServerSession &session, const ObexHeader &req) override;
        void OnPut(ObexServerSession &session, const ObexHeader &req) override;
        void OnTransportDisconnected(const std::string &btAddr) override;
        void OnTransportError(const std::string &btAddr, int errCd, const std::string &msg) override;
        void OnBusy(ObexServerSession &session, bool isBusy) const override;
        void OnAbort(ObexServerSession &session, const ObexHeader &req) override;

    private:
        void SendOppDisconnected(const std::string &btAddr) const;
        int ReceiveFileHeader(ObexServerSession &session, const ObexHeader &req);
        void ReceiveFileBody(ObexServerSession &session, const ObexHeader &req, bool isHead) const;
        std::string U16stringToString(const std::u16string &u16str) const;
        std::string RenameFile(std::string fileName) const;
        bool HasSameName(std::string path, std::string name) const;
        bool NeedRejectFileForPts(std::string fileName) const;

        uint32_t connectionId_ = 1;
    };
    std::unique_ptr<ObexServer> obexServer_ = nullptr;
    std::unique_ptr<OppObexObserver> oppObexObserver_ = nullptr;

    BT_DISALLOW_COPY_AND_ASSIGN(OppObexServer);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OPP_OBEX_SERVER_H
