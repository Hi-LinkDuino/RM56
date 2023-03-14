/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_WS_SERVER_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_WS_SERVER_H

#include <boost/asio/error.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <functional>
#include <iostream>
#include <vector>

namespace V8Debugger {

namespace beast = boost::beast;
namespace websocket = beast::websocket;
using localSocket = boost::asio::local::stream_protocol;
class WsServer {
public:
    explicit WsServer(std::function<void(std::string)> onMessage, std::string sockName) : tid(0), instanceId(0),
        endPoint(sockName), socket(ioContext), acceptor(ioContext, endPoint), wsOnMessage(std::move(onMessage)) {}
    ~WsServer() {}
    void RunServer();
    void SendMessage(const std::string& message) const;
    void SetTerminateExecutionFlag(bool flag);
    pthread_t tid;
    int32_t instanceId;
    localSocket::endpoint endPoint;
    boost::asio::io_context ioContext;
    localSocket::socket socket;
    localSocket::acceptor acceptor;
private:
    void StartListening();
    void WaitFrontendMessage() const;
    volatile bool terminateExecution = false;
    static constexpr int DEBUGGER_WAIT_SEND_SIGNAL_TIME = 100;

    std::function<void(std::string)> wsOnMessage;
    std::unique_ptr<websocket::stream<localSocket::socket>> ws = nullptr;
};

} // namespace V8Debugger

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_V8_DEBUGGER_WS_SERVER_H
