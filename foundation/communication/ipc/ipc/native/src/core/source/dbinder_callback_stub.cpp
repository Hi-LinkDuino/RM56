/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dbinder_callback_stub.h"

#include <cinttypes>

#include "log_tags.h"
#include "ipc_debug.h"
#include "ipc_process_skeleton.h"
#include "ipc_skeleton.h"
#include "ipc_thread_skeleton.h"
#include "ipc_types.h"
#include "string_ex.h"
#include "sys_binder.h"

namespace OHOS {
#ifndef TITLE
#define TITLE __PRETTY_FUNCTION__
#endif

static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, LOG_ID_RPC, "DBinderCallbackStub" };
#define DBINDER_LOGE(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "%{public}s %{public}d: " fmt, TITLE, __LINE__, ##args)
#define DBINDER_LOGI(fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "%{public}s %{public}d: " fmt, TITLE, __LINE__, ##args)

DBinderCallbackStub::DBinderCallbackStub(const std::string &service, const std::string &device,
    const std::string &localDevice, uint64_t stubIndex, uint32_t handle)
    : IPCObjectStub(Str8ToStr16("DBinderCallback" + device + service)),
      serviceName_(service),
      deviceID_(device),
      localDeviceID_(localDevice),
      stubIndex_(stubIndex),
      handle_(handle)
{
    DBINDER_LOGI("serviceName:%{public}s, deviceId:%{public}s, handle:%{public}u, stubIndex_:%{public}" PRIu64 "",
        serviceName_.c_str(), deviceID_.c_str(), handle_, stubIndex_);
}

DBinderCallbackStub::~DBinderCallbackStub()
{
    DBINDER_LOGI("DBinderCallbackStub delete");
}

const std::string &DBinderCallbackStub::GetServiceName()
{
    return serviceName_;
}

const std::string &DBinderCallbackStub::GetDeviceID()
{
    return deviceID_;
}

uint64_t DBinderCallbackStub::GetStubIndex() const
{
    return stubIndex_;
}

int32_t DBinderCallbackStub::ProcessProto(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    int uid = IPCSkeleton::GetCallingUid();
    int pid = IPCSkeleton::GetCallingPid();
    if (uid < 0 || pid < 0) {
        DBINDER_LOGE("uid or pid err");
        return DBINDER_SERVICE_PROCESS_PROTO_ERR;
    }
    sptr<IRemoteObject> object = IPCProcessSkeleton::GetCurrent()->GetSAMgrObject();
    if (object == nullptr) {
        DBINDER_LOGE("get sa object is null");
        return DBINDER_CALLBACK_READ_OBJECT_ERR;
    }
    IPCObjectProxy *samgr = reinterpret_cast<IPCObjectProxy *>(object.GetRefPtr());
    std::string sessionName = samgr->TransDataBusName(uid, pid);
    if (sessionName.empty()) {
        DBINDER_LOGE("grans session name failed");
        return DBINDER_SERVICE_WRONG_SESSION;
    }

    MessageParcel authData, authReply;
    MessageOption authOption;
    if (!authData.WriteUint32(pid) || !authData.WriteUint32(uid) || !authData.WriteString(localDeviceID_) ||
        !authData.WriteUint64(stubIndex_)) {
        DBINDER_LOGE("write to MessageParcel fail");
        return ERR_INVALID_DATA;
    }
    IRemoteInvoker *dbinderInvoker = IPCThreadSkeleton::GetRemoteInvoker(IRemoteObject::IF_PROT_DATABUS);
    if (dbinderInvoker == nullptr) {
        DBINDER_LOGE("no databus thread and invoker");
        return RPC_DATABUS_INVOKER_ERR;
    }
    int err = dbinderInvoker->SendRequest(handle_, DBINDER_TRANS_COMMAUTH, authData, authReply, authOption);
    if (err != ERR_NONE) {
        DBINDER_LOGE("send auth info to remote fail");
        return BINDER_CALLBACK_AUTHCOMM_ERR;
    }
    DBINDER_LOGI("send to stub ok!stubIndex:%{public}" PRIu64 ", peerDevice = %{public}s, localDeviceID_ = %{public}s,"
        "serviceName_ = %{public}s, uid:%{public}d, pid:%{public}d, sessionName = %{public}s",
        stubIndex_, deviceID_.c_str(), localDeviceID_.c_str(), serviceName_.c_str(), uid, pid, sessionName.c_str());
    if (!reply.WriteUint32(IRemoteObject::IF_PROT_DATABUS) || !reply.WriteUint64(stubIndex_) ||
        !reply.WriteString(serviceName_) || !reply.WriteString(deviceID_) || !reply.WriteString(localDeviceID_) ||
        !reply.WriteString(sessionName)) {
        DBINDER_LOGE("write to parcel fail");
        return ERR_INVALID_DATA;
    }
    return 0;
}

int32_t DBinderCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    int32_t result = ERR_NONE;
    DBINDER_LOGI("code = %{public}u", code);
    switch (code) {
        case GET_PROTO_INFO: {
            result = ProcessProto(code, data, reply, option);
            break;
        }
        default: {
            DBINDER_LOGI("unknown code = %{public}u", code);
            result = DBINDER_CALLBACK_ERR;
            break;
        }
    }

    return result;
}
} // namespace OHOS
