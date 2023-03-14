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

#ifndef OBEX_MP_SERVER_H
#define OBEX_MP_SERVER_H

#include "obex_server.h"

namespace OHOS {
namespace bluetooth {
class ObexPrivateMpServer : public ObexPrivateServer {
public:
    explicit ObexPrivateMpServer(
        const ObexPrivateServerConfig &config, ObexServerObserver &observer, utility::Dispatcher &dispatcher);
    ~ObexPrivateMpServer() override = default;

    void HandleTransportDataBusy(ObexServerSession &session, uint8_t isBusy) override;
    void HandlePutRequest(ObexServerSession &session, ObexHeader &req) const override;
    void HandleGetRequest(ObexServerSession &session, ObexHeader &req) override;
    void HandleAbortRequest(ObexServerSession &session, ObexHeader &req) override;

private:
    void HandlePutFinalRequest(ObexServerSession &session, ObexHeader &req) const;
};
class ObexMpServer : public ObexServer {
public:
    // create obex server
    ObexMpServer(const std::string &serviceName, const ObexServerConfig &config, ObexServerObserver &observer,
        utility::Dispatcher &dispatcher);
    // destroy obex server
    ~ObexMpServer() override = default;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_MP_SERVER_H
