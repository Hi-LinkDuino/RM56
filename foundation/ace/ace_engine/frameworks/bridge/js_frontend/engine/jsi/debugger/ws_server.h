/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_ARK_DEBUGGER_WS_SERVER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_ARK_DEBUGGER_WS_SERVER_H

#include <boost/asio/error.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <functional>
#include <iostream>
#include <queue>

namespace OHOS::Ace::Framework {

namespace beast = boost::beast;
namespace websocket = beast::websocket;
using localSocket = boost::asio::local::stream_protocol;

class WsServer {
public:
    WsServer(const std::string& component, const std::function<void()>& onMessage)
        : componentName_(component), wsOnMessage_(onMessage)
    {}
    ~WsServer() = default;
    void RunServer();
    void StopServer();
    void SendReply(const std::string& message) const;
    std::queue<const std::string> ideMsgQueue;

private:
    volatile bool terminateExecution_ { false };
    std::string componentName_ {};
    std::function<void()> wsOnMessage_ {};
    std::unique_ptr<websocket::stream<localSocket::socket>> webSocket_ { nullptr };
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_ARK_DEBUGGER_WS_SERVER_H