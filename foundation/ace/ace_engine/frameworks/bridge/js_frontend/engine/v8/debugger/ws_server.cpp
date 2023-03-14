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

#include "frameworks/bridge/js_frontend/engine/v8/debugger/ws_server.h"

#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <sys/unistd.h>

#include "frameworks/bridge/js_frontend/engine/v8/debugger/utils.h"

namespace V8Debugger {
bool g_attachState;

void WsServer::SetTerminateExecutionFlag(bool flag)
{
    terminateExecution = flag;
}

void AcceptHandler(const boost::system::error_code& error)
{
    if (!error) {
        g_attachState = true;
    }
}

void WsServer::RunServer()
{
    try {
        acceptor.async_accept(socket, AcceptHandler);
        ioContext.run();
        if (!g_attachState) {
            return;
        }
        ws = std::unique_ptr<websocket::stream<localSocket::socket>>(
            std::make_unique<websocket::stream<localSocket::socket>>(std::move(socket)));
        StartListening();
    } catch (const std::exception& e) {
        LOGE("Error exception, %s", e.what());
        return;
    } catch (beast::system_error const& se) {
        LOGE("Error system_error");
        return;
    }
}

void WsServer::SendMessage(const std::string& message) const
{
    try {
        LOGI("send msg to IDE message=%s", message.c_str());
        boost::beast::multi_buffer buffer;
        boost::beast::ostream(buffer) << message;

        ws->text(ws->got_text());
        ws->write(buffer.data());
    } catch (beast::system_error const& se) {
        if (se.code() != websocket::error::closed) {
            LOGE("Error system_error");
        }
    } catch (std::exception const& e) {
        LOGE("Error exception");
    }
}

void WsServer::StartListening()
{
    try {
        terminateExecution = false;
        ws->accept();
        while (!terminateExecution) {
            WaitFrontendMessage();
            usleep(DEBUGGER_WAIT_SEND_SIGNAL_TIME);
        }
    } catch (beast::system_error const& se) {
        if (se.code() != websocket::error::closed) {
            LOGE("Error system_error");
        }
    } catch (std::exception const& e) {
        LOGE("Error exception");
    }
}

void WsServer::WaitFrontendMessage() const
{
    beast::flat_buffer buffer;
    ws->read(buffer);
    std::string message = boost::beast::buffers_to_string(buffer.data());
    LOGI("receive msg from IDE message=%s", message.c_str());
    wsOnMessage(std::move(message));
}

} // namespace V8Debugger
