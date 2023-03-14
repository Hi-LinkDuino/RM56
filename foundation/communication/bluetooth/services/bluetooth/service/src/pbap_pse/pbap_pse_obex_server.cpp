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

#include "pbap_pse_obex_server.h"
#include <cstring>
#include <memory>
#include "../obex/obex_mp_server.h"
#include "log.h"
#include "pbap_pse_header_msg.h"

namespace OHOS {
namespace bluetooth {
PbapPseObexServer::PbapPseObexServer(
    const std::string &serverName, const ObexServerConfig &config, PbapPseService &service)
    : obexConfig_(config), pbapPseService_(service)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    pseObexObserver_ = std::make_unique<PseObexObserver>(*this);
    obexServer_ = std::make_unique<ObexMpServer>(serverName, obexConfig_, *pseObexObserver_, *service.GetDispatcher());
}

int PbapPseObexServer::StartUp() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    int ret = obexServer_->Startup();
    if (ret != RET_NO_ERROR) {
        PBAP_PSE_LOG_ERROR("Obex Startup Error ret = %{public}d", ret);
    }
    return ret;
}

int PbapPseObexServer::ShutDown() const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    obexServer_->Shutdown();
    utility::Message msg(PSE_SHUTDOWN_COMPLETED);
    pbapPseService_.PostMessage(msg);
    return 0;
}

void PbapPseObexServer::PseObexObserver::OnTransportDisconnected(const std::string &btAddr)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    RawAddress device(btAddr);
    pseObexServer_.pbapPseService_.ProcessObexRequest(PSE_DEVICE_DISCONNECTED, device);
}

void PbapPseObexServer::PseObexObserver::OnTransportError(const std::string &btAddr, int errCd, const std::string &msg)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    PBAP_PSE_LOG_ERROR("%{public}s transport error, errCd=%{public}d, msg=%{public}s", btAddr.c_str(), errCd, msg.c_str());
    RawAddress device(btAddr);
    pseObexServer_.pbapPseService_.ProcessObexRequest(PSE_DEVICE_TRANSPORT_ERROR, device);
}

void PbapPseObexServer::PseObexObserver::OnTransportConnect(ObexIncomingConnect &incomingConnect)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    pseObexServer_.pbapPseService_.ProcessObexRequest(
        PSE_DEVICE_CONNECT_INCOMING, incomingConnect.GetRemoteAddress(), incomingConnect);
}

void PbapPseObexServer::PseObexObserver::OnConnect(ObexServerSession &session, const ObexHeader &req)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    pseObexServer_.pbapPseService_.ProcessObexRequest(PSE_REQ_OBEX_CONNECT, session.GetRemoteAddr(), session, req);
}

void PbapPseObexServer::PseObexObserver::OnDisconnect(ObexServerSession &session, const ObexHeader &req)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    pseObexServer_.pbapPseService_.ProcessObexRequest(PSE_REQ_OBEX_DISCONNECT, session.GetRemoteAddr(), session, req);
}

void PbapPseObexServer::PseObexObserver::OnPut(ObexServerSession &session, const ObexHeader &req)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    session.SendSimpleResponse(ObexRspCode::BAD_REQUEST);
}

void PbapPseObexServer::PseObexObserver::OnGet(ObexServerSession &session, const ObexHeader &req)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    pseObexServer_.pbapPseService_.ProcessObexRequest(PSE_REQ_OBEX_GET, session.GetRemoteAddr(), session, req);
}

void PbapPseObexServer::PseObexObserver::OnSetPath(ObexServerSession &session, const ObexHeader &req)
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    pseObexServer_.pbapPseService_.ProcessObexRequest(PSE_REQ_OBEX_SETPATH, session.GetRemoteAddr(), session, req);
}

void PbapPseObexServer::PseObexObserver::OnBusy(ObexServerSession &session, bool isBusy) const
{
    PBAP_PSE_LOG_INFO("Call %{public}s", __PRETTY_FUNCTION__);
    pseObexServer_.pbapPseService_.ProcessObexBusy(session.GetRemoteAddr(), isBusy);
}
}  // namespace bluetooth
}  // namespace OHOS
