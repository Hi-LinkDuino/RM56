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

#include "frameworks/core/common/debugger/connect_server.h"
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/unistd.h>
#include <thread>
#include "base/log/log.h"
#include "frameworks/core/common/debugger/unix_socket.h"

namespace OHOS::Ace {
std::unique_ptr<UnixSocketClient> g_unixSocket = nullptr;

void ConnectServer::RunServer()
{
    terminateExecution_ = false;

    try {
        boost::asio::io_context ioContext;
        int appPid = getpid();
        std::string pidStr = std::to_string(appPid);
        std::string sockName = '\0' + pidStr + bundleName_;
        LOGI("App Connect Server Run: %{private}d%{private}s", appPid, bundleName_.c_str());
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
            LOGI("Connect Server OnMessage: %{private}s", message.c_str());
            wsOnMessage_(std::move(message));
            usleep(DEBUGGER_WAIT_SEND_SIGNAL_TIME);
        }
    } catch (const beast::system_error& se) {
        if (se.code() != websocket::error::closed) {
            LOGE("Error system_error");
        }
    } catch (const std::exception& e) {
        LOGE("Error exception, %{private}s", e.what());
    }
}

void ConnectServer::StopServer()
{
    LOGI("App Connect Server Stop");
    terminateExecution_ = true;
}

void ConnectServer::SendMessage(const std::string& message) const
{
    try {
        LOGI("send msg to Debug Manager message=%s", message.c_str());
        boost::beast::multi_buffer buffer;
        boost::beast::ostream(buffer) << message;

        webSocket_->text(webSocket_->got_text());
        webSocket_->write(buffer.data());
    } catch (beast::system_error const& se) {
        if (se.code() != websocket::error::closed) {
            LOGE("Error system_error");
        }
    } catch (std::exception const& e) {
        LOGE("Error exception");
    }
}

void ConnectServer::WaitMessage() const
{
    beast::flat_buffer buffer;
    webSocket_->read(buffer);
    std::string message = boost::beast::buffers_to_string(buffer.data());
    LOGI("receive msg from Debug Manager message=%s", message.c_str());
    wsOnMessage_(std::move(message));
}

void ConnectServer::Register(int32_t pid)
{
    g_unixSocket = std::make_unique<UnixSocketClient>();
    int connRes = g_unixSocket->UnixSocketConn();
    if (connRes < 0) {
        return;
    }
    LOGI("Unix Socket Connect Successfully");
    int res = g_unixSocket->SendMessage(pid);
    if (res < 0) {
        LOGE("Register Failed!");
        return;
    }
    LOGI("Register Successfully!");
}

} // namespace OHOS::Ace
