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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DEBUGGER_CONNECT_SERVER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DEBUGGER_CONNECT_SERVER_H

#include <boost/asio/error.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <functional>
#include <iostream>

namespace OHOS::Ace {
namespace beast = boost::beast;
namespace websocket = beast::websocket;
using localSocket = boost::asio::local::stream_protocol;

class ConnectServer {
public:
    ConnectServer(const std::string& bundle, std::function<void(std::string)> onMessage, bool flagNeedDebugBreakPoint)
        : bundleName_(bundle), wsOnMessage_(std::move(onMessage))
    {
        waitingForDebugger_ = flagNeedDebugBreakPoint;
    }
    ~ConnectServer() = default;
    void RunServer();
    void StopServer();
    void SendMessage(const std::string& message) const;
    void WaitMessage() const;
    void Register(int32_t pid);
    bool waitingForDebugger_;

private:
    volatile bool terminateExecution_ = false;
    std::string bundleName_;
    std::function<void(std::string)> wsOnMessage_;
    std::unique_ptr<websocket::stream<localSocket::socket>> webSocket_ = nullptr;
    static constexpr int DEBUGGER_WAIT_SEND_SIGNAL_TIME = 100;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DEBUGGER_CONNECT_SERVER_H
