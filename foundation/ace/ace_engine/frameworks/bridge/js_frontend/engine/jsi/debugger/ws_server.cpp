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

#include "frameworks/bridge/js_frontend/engine/jsi/debugger/ws_server.h"

#include <fstream>
#include <iostream>
#include <sys/types.h>

#include "base/log/log.h"

namespace OHOS::Ace::Framework {

void WsServer::RunServer()
{
    terminateExecution_ = false;
    try {
        boost::asio::io_context ioContext;
        int appPid = getpid();
        std::string pidStr = std::to_string(appPid);
        std::string sockName = '\0' + pidStr + componentName_;
        LOGI("WsServer RunServer: %{public}d%{public}s", appPid, componentName_.c_str());
        localSocket::endpoint endPoint(sockName);
        localSocket::socket socket(ioContext);
        localSocket::acceptor acceptor(ioContext, endPoint);
        acceptor.accept(socket);
        webSocket_ = std::unique_ptr<websocket::stream<localSocket::socket>>(
            std::make_unique<websocket::stream<localSocket::socket>>(std::move(socket)));
        webSocket_->accept();
        while (!terminateExecution_) {
            beast::flat_buffer buffer;
            webSocket_->read(buffer);
            std::string message = boost::beast::buffers_to_string(buffer.data());
            LOGI("WsServer OnMessage: %{public}s", message.c_str());
            ideMsgQueue.push(std::move(message));
            wsOnMessage_();
        }
    } catch (const beast::system_error& se) {
        if (se.code() != websocket::error::closed) {
            LOGE("Error system_error");
        }
    } catch (const std::exception& e) {
        LOGE("Error exception, %{public}s", e.what());
    }
}

void WsServer::StopServer()
{
    LOGI("WsServer StopServer");
    terminateExecution_ = true;
}

void WsServer::SendReply(const std::string& message) const
{
    LOGI("WsServer SendReply: %{public}s", message.c_str());
    try {
        boost::beast::multi_buffer buffer;
        boost::beast::ostream(buffer) << message;

        webSocket_->text(webSocket_->got_text());
        webSocket_->write(buffer.data());
    } catch (const beast::system_error& se) {
        if (se.code() != websocket::error::closed) {
            LOGE("SendReply Error system_error");
        }
    } catch (const std::exception& e) {
        LOGE("SendReply Error exception");
    }
}

} // namespace OHOS::Ace::Framework
