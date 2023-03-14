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

#include "pbap_pce_obex_client.h"
#include "pbap_pce_def.h"
#include "pbap_pce_service.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
ObexFileBodyObject::ObexFileBodyObject(const std::string &file)
{
    OpenOutFile(file);
}

void ObexFileBodyObject::OpenOutFile(const std::string &file)
{
    ofs_.open(file, std::ios::out);
    if (!ofs_.is_open()) {
        PBAP_PCE_LOG_ERROR("%{public}s file  open failed", __PRETTY_FUNCTION__);
    } else {
        PBAP_PCE_LOG_DEBUG("%{public}s file=%{public}s  opened.", __PRETTY_FUNCTION__, file.c_str());
    }
}

size_t ObexFileBodyObject::Read(uint8_t *buf, size_t bufLen)
{
    size_t readSize = bufLen;
    return readSize;
}

size_t ObexFileBodyObject::Write(const uint8_t *buf, size_t bufLen)
{
    if (ofs_.is_open()) {
        auto buffer = reinterpret_cast<const char*>(buf);
        ofs_.write(buffer, bufLen);
        PBAP_PCE_LOG_DEBUG("%{public}s write file bufLen=%zu", __PRETTY_FUNCTION__, bufLen);
    } else {
        PBAP_PCE_LOG_ERROR("%{public}s file  open failed", __PRETTY_FUNCTION__);
    }
    return bufLen;
}

int ObexFileBodyObject::Close()
{
    ofs_.close();
    return 0;
}

PbapPceObexClient::PbapPceObexClient(const ObexClientConfig &config, PbapPceService &pceService)
    : obexConfig_(config), pceService_(pceService)
{
    observer_ = std::make_unique<PceObexObserver>(pceService_);
    client_ = std::make_unique<ObexMpClient>(obexConfig_, *observer_, *pceService.GetDispatcher());
}

PbapPceObexClient::~PbapPceObexClient()
{}

int PbapPceObexClient::Connect(bool supported)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int ret = RET_BAD_STATUS;
    if (observer_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, observer_ is null", __PRETTY_FUNCTION__);
        return ret;
    }
    if (IsBusy()) {
        PBAP_PCE_LOG_ERROR("%{public}s end, IS busy", __PRETTY_FUNCTION__);
        return ret;
    }
    SetBusy(true);
    if (supported) {
        ObexTlvParamters p;
        TlvTriplet tlv(static_cast<uint8_t>(PbapSupportedFeatures::PCE_PBAP_SUPPORTED_FEATURES), PBAP_PCE_FEATURES);
        p.AppendTlvtriplet(tlv);
        ObexConnectParams param = {.appParams_ = &p};
        ret = client_->Connect(param);
    } else {
        ret = client_->Connect();
    }

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int PbapPceObexClient::Reconnect(ObexDigestChallenge &challenge, ObexDigestResponse &digest)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int ret = RET_BAD_STATUS;
    if (observer_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, observer_ is null", __PRETTY_FUNCTION__);
        return ret;
    }

    if (IsBusy()) {
        PBAP_PCE_LOG_ERROR("%{public}s end, IS busy", __PRETTY_FUNCTION__);
        return ret;
    }
    SetBusy(true);
    ObexTlvParamters p;
    ObexConnectParams param = {&p, &challenge, &digest};
    ret = client_->Connect(param);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int PbapPceObexClient::Disconnect(bool withObexReq) const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int ret = RET_BAD_STATUS;
    if (client_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s end, client_ is null", __PRETTY_FUNCTION__);
        return ret;
    }
    ret = client_->Disconnect(withObexReq);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int PbapPceObexClient::Get(const ObexHeader &req, int reqMsgType) const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int ret = RET_BAD_STATUS;
    if ((client_ == nullptr) || (observer_ == nullptr)) {
        PBAP_PCE_LOG_ERROR("%{public}s end, client_ or observer_ is null", __PRETTY_FUNCTION__);
        return ret;
    }
#ifdef PBAP_PCE_RECEIVED_BY_OBEX_ARRAY
    auto writer = std::make_shared<ObexArrayBodyObject>();
    ret = client_->Get(req, writer);
#else
    if (reqMsgType == PCE_REQ_PULLPHONEBOOK) {
        auto &device = client_->GetClientSession().GetRemoteAddr();
        std::string file = pceService_.GetDownloadFileName(device);
        auto writer = std::make_shared<ObexFileBodyObject>(file);
        ret = client_->Get(req, writer);
    } else {
        auto writer = std::make_shared<ObexArrayBodyObject>();
        ret = client_->Get(req, writer);
    }
#endif  // PBAP_PCE_RECEIVED_BY_OBEX_ARRAY

    auto &pSession = client_->GetClientSession();
    observer_->SetPhoneBookActionInfo(pSession.GetLastOpeId(), reqMsgType);

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int PbapPceObexClient::SetPath(uint8_t flag, const std::u16string &path) const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    int ret = RET_BAD_STATUS;
    if ((client_ == nullptr) || (observer_ == nullptr)) {
        PBAP_PCE_LOG_ERROR("%{public}s end, client_ or observer_ is null", __PRETTY_FUNCTION__);
        return ret;
    }

    // absolute path
    char16_t separator = u'/';
    if (path.find(separator) != std::u16string::npos) {
        std::vector<std::u16string> paths;
        size_t prevPos = 0;
        size_t pos = 0;
        while (pos < path.size()) {
            if (path[pos] == separator) {
                paths.push_back(path.substr(prevPos, pos - prevPos));
                prevPos = pos + 1;
            }
            pos++;
        }
        paths.push_back(path.substr(prevPos, pos - prevPos));
        ret = client_->SetPath(paths);
    } else {
        ret = client_->SetPath(flag, path);
    }

    auto &pSession = client_->GetClientSession();
    observer_->SetPhoneBookActionInfo(pSession.GetLastOpeId(), PCE_REQ_SETPHONEBOOK, path, flag);

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return ret;
}

int PbapPceObexClient::Abort() const
{
    int ret = RET_BAD_STATUS;
    if ((client_ == nullptr) || (observer_ == nullptr)) {
        PBAP_PCE_LOG_ERROR("%{public}s client_ or observer_ is null", __PRETTY_FUNCTION__);
        return ret;
    }
    ret = client_->Abort();
    SetAbort(true);
    return ret;
}

ObexMpClient *PbapPceObexClient::GetClient() const
{
    if (client_ == nullptr) {
        PBAP_PCE_LOG_ERROR("%{public}s client_ is null", __PRETTY_FUNCTION__);
        return nullptr;
    }
    return client_.get();
}

void PbapPceObexClient::PceObexObserver::OnTransportFailed(ObexClient &client, int errCd)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    PBAP_PCE_LOG_ERROR("Transport failed with error %{public}d", errCd);
    isBusy_ = false;
    const auto &device = client.GetClientSession().GetRemoteAddr();
    utility::Message msg(PCE_TRANSPORT_FAILED, errCd);
    pceService_.ProcessObexRespMessage(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceObexClient::PceObexObserver::OnConnected(ObexClient &client, const ObexHeader &resp)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    isBusy_ = false;
    const auto &device = client.GetClientSession().GetRemoteAddr();
    std::unique_ptr<PbapPceObexMessage> obexMsg = std::make_unique<PbapPceObexMessage>(client, resp);
    utility::Message msg(PCE_OBEX_CONNECTED, 0, static_cast<void *>(obexMsg.get()));
    pceService_.ProcessObexRespMessage(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceObexClient::PceObexObserver::OnConnectFailed(ObexClient &client, const ObexHeader &resp)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    isBusy_ = false;
    const auto &device = client.GetClientSession().GetRemoteAddr();
    std::unique_ptr<PbapPceObexMessage> obexMsg = std::make_unique<PbapPceObexMessage>(client, resp);
    utility::Message msg(PCE_OBEX_CONNECT_FAILED, 0, static_cast<void *>(obexMsg.get()));
    pceService_.ProcessObexRespMessage(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceObexClient::PceObexObserver::OnDisconnected(ObexClient &client)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    isBusy_ = false;
    const auto &device = client.GetClientSession().GetRemoteAddr();
    utility::Message msg(PCE_OBEX_DISCONNECTED);
    pceService_.ProcessObexRespMessage(device, msg);

    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceObexClient::PceObexObserver::OnActionCompleted(ObexClient &client, const ObexHeader &resp)
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    isBusy_ = false;
    auto &pSession = client.GetClientSession();
    int lastCommandMsgType = 0;
    std::u16string path;
    uint8_t flags = 0;
    GetPhoneBookActionInfo(pSession.GetLastOpeId(), lastCommandMsgType, path, flags);
    const int offsetToComp = 1;
    if (isAbort_) {
        isAbort_ = false;
        // req id to completed id
        lastCommandMsgType = PCE_REQ_ABORTDOWNLOADING + offsetToComp;
    } else {
        // req id to completed id
        lastCommandMsgType = lastCommandMsgType + offsetToComp;
    }

    const auto &device = client.GetClientSession().GetRemoteAddr();
    PbapPceActionObexMessage obexMsg(client, resp, path, flags);
    utility::Message msg(lastCommandMsgType, 0, static_cast<void *>(&obexMsg));
    pceService_.ProcessObexRespMessage(device, msg);
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

void PbapPceObexClient::PceObexObserver::OnBusy(ObexClient &client, bool isBusy)
{
    auto &device = client.GetClientSession().GetRemoteAddr();
    HILOGI("device=%{public}s, isBusy=%{public}d", GET_ENCRYPT_ADDR(device), isBusy ? 1 : 0);
}

bool PbapPceObexClient::PceObexObserver::GetPhoneBookActionInfo(
    int operationId, int &retReqMsgType, std::u16string &retPath, uint8_t &retFlags) const
{
    bool ret = true;
    retReqMsgType = phoneBookActionInfo_.reqMsgType_;
    retPath = phoneBookActionInfo_.path_;
    retFlags = phoneBookActionInfo_.flags_;
    return ret;
}

void PbapPceObexClient::PceObexObserver::SetPhoneBookActionInfo(
    int operationId, int reqMsgType, const std::u16string &path, uint8_t flags, bool isBusy)
{
    phoneBookActionInfo_.operationId_ = operationId;
    phoneBookActionInfo_.reqMsgType_ = reqMsgType;
    phoneBookActionInfo_.path_ = path;
    phoneBookActionInfo_.flags_ = flags;
    isBusy_ = isBusy;
}

void PbapPceObexClient::SetBusy(bool isBusy) const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (observer_ != nullptr) {
        observer_->isBusy_ = isBusy;
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}

bool PbapPceObexClient::IsBusy() const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (observer_ != nullptr) {
        return observer_->isBusy_;
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
    return false;
}

void PbapPceObexClient::SetAbort(bool isAbort) const
{
    PBAP_PCE_LOG_INFO("%{public}s start", __PRETTY_FUNCTION__);
    if (observer_ != nullptr) {
        observer_->isAbort_ = isAbort;
    }
    PBAP_PCE_LOG_INFO("%{public}s end", __PRETTY_FUNCTION__);
}
}  // namespace bluetooth
}  // namespace OHOS
