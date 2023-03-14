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

#include "sys_event_service_stub.h"

#include "errors.h"
#include "hilog/log.h"
#include "parcelable_vector_rw.h"

namespace OHOS {
namespace HiviewDFX {
static constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "HiView-SysEventServiceStub" };
int32_t SysEventServiceStub::HandleAddSysEventListener(MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    std::vector<SysEventRule> rules;
    bool ret = ReadVectorFromParcel(data, rules);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read rules failed.");
        return ERR_FLATTEN_OBJECT;
    }
    sptr<IRemoteObject> remoteObject = data.ReadRemoteObject();
    sptr<ISysEventCallback> callback = iface_cast<ISysEventCallback>(remoteObject);
    ret = reply.WriteInt32(AddListener(rules, callback));
    if (!ret) {
        HiLog::Error(LABEL, "parcel write return-value of AddListener failed.");
        return ERR_FLATTEN_OBJECT;
    }
    return ERR_OK;
}

int32_t SysEventServiceStub::HandleRemoveSysEventListener(MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    sptr<IRemoteObject> remoteObject = data.ReadRemoteObject();
    sptr<ISysEventCallback> callback = iface_cast<ISysEventCallback>(remoteObject);
    if (callback == nullptr) {
        HiLog::Error(LABEL, "parcel read callback failed.");
        return ERR_FLATTEN_OBJECT;
    }
    RemoveListener(callback);
    return ERR_OK;
}

int32_t SysEventServiceStub::HandleQueryEvent(MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    int64_t beginTime = 0;
    bool ret = data.ReadInt64(beginTime);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read begin time failed.");
        return ERR_FLATTEN_OBJECT;
    }
    int64_t endTime = 0;
    ret = data.ReadInt64(endTime);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read end time failed.");
        return ERR_FLATTEN_OBJECT;
    }
    int32_t maxEvents = 0;
    ret = data.ReadInt32(maxEvents);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read max events failed.");
        return ERR_FLATTEN_OBJECT;
    }
    std::vector<SysEventQueryRule> queryRules;
    ret = ReadVectorFromParcel(data, queryRules);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read query rules failed.");
        return ERR_FLATTEN_OBJECT;
    }
    sptr<IRemoteObject> remoteObject = data.ReadRemoteObject();
    sptr<IQuerySysEventCallback> callback = iface_cast<IQuerySysEventCallback>(remoteObject);
    ret = reply.WriteBool(QuerySysEvent(beginTime, endTime, maxEvents, queryRules, callback));
    if (!ret) {
        HiLog::Error(LABEL, "parcel write return-value of QuerySysEvent failed.");
        return ERR_FLATTEN_OBJECT;
    }
    return ERR_OK;
}

int32_t SysEventServiceStub::HandleSetDebugMode(MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    sptr<IRemoteObject> remoteObject = data.ReadRemoteObject();
    sptr<ISysEventCallback> callback = iface_cast<ISysEventCallback>(remoteObject);
    bool mode = false;
    bool ret = data.ReadBool(mode);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read mode failed.");
        return ERR_FLATTEN_OBJECT;
    }
    ret = reply.WriteBool(SetDebugMode(callback, mode));
    if (!ret) {
        HiLog::Error(LABEL, "parcel write return-value of SetDebugMode failed.");
        return ERR_FLATTEN_OBJECT;
    }
    return ERR_OK;
}

int32_t SysEventServiceStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    std::u16string descripter = SysEventServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        HiLog::Error(LABEL, "read descriptor failed.");
        return ERR_INVALID_VALUE;
    }
    switch (code) {
        case ADD_SYS_EVENT_LISTENER: {
            return HandleAddSysEventListener(data, reply, option);
        }
        case REMOVE_SYS_EVENT_LISTENER: {
            return HandleRemoveSysEventListener(data, reply, option);
        }
        case QUERY_SYS_EVENT: {
            return HandleQueryEvent(data, reply, option);
        }
        case SET_DEBUG_MODE: {
            return HandleSetDebugMode(data, reply, option);
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
} // namespace HiviewDFX
} // namespace OHOS