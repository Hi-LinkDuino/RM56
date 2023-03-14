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

#include "sys_event_service_proxy.h"

#include "errors.h"
#include "hilog/log.h"
#include "parcelable_vector_rw.h"

namespace OHOS {
namespace HiviewDFX {
static constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "HiView-SysEventServiceProxy" };
bool SysEventServiceProxy::AddListener(const std::vector<SysEventRule>& rules, const sptr<ISysEventCallback>& callback)
{
    auto remote = Remote();
    bool result = false;
    if (remote == nullptr) {
        HiLog::Error(LABEL, "SysEventService Remote is NULL.");
        return result;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(SysEventServiceProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "write descriptor failed.");
        return result;
    }
    bool ret = WriteVectorToParcel(data, rules);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write rules failed.");
        return result;
    }
    ret = data.WriteRemoteObject(callback->AsObject());
    if (!ret) {
        HiLog::Error(LABEL, "parcel write callback failed.");
        return result;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t res = remote->SendRequest(ADD_SYS_EVENT_LISTENER, data, reply, option);
    if (res != ERR_OK) {
        HiLog::Error(LABEL, "send request failed, error is %{public}d.", res);
        return result;
    }
    ret = reply.ReadBool(result);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read result failed.");
    }
    return result;
}

bool SysEventServiceProxy::RemoveListener(const sptr<ISysEventCallback> &callback)
{
    auto remote = Remote();
    bool result = false;
    if (remote == nullptr) {
        HiLog::Error(LABEL, "SysEventService Remote is null.");
        return result;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(SysEventServiceProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "write descriptor failed.");
        return result;
    }
    bool ret = data.WriteRemoteObject(callback->AsObject());
    if (!ret) {
        HiLog::Error(LABEL, "parcel write object in callback failed.");
        return result;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t res = remote->SendRequest(REMOVE_SYS_EVENT_LISTENER, data, reply, option);
    if (res != ERR_OK) {
        HiLog::Error(LABEL, "send request failed, error is %{public}d.", res);
    }
    ret = reply.ReadBool(result);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read result failed.");
    }
    return result;
}

bool SysEventServiceProxy::QuerySysEvent(int64_t beginTime, int64_t endTime, int32_t maxEvents,
    const std::vector<SysEventQueryRule>& rules, const sptr<IQuerySysEventCallback>& callback)
{
    bool result = false;
    auto remote = Remote();
    if (remote == nullptr) {
        HiLog::Error(LABEL, "SysEventService Remote is null.");
        return result;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(SysEventServiceProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "write descriptor failed.");
        return result;
    }
    bool ret = data.WriteInt64(beginTime);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write begin time failed.");
        return result;
    }
    ret = data.WriteInt64(endTime);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write end time failed.");
        return result;
    }
    ret = data.WriteInt32(maxEvents);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write max events failed.");
        return result;
    }
    ret = WriteVectorToParcel(data, rules);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write query rules failed.");
        return result;
    }
    ret = data.WriteRemoteObject(callback->AsObject());
    if (!ret) {
        HiLog::Error(LABEL, "parcel write callback failed.");
        return result;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t res = remote->SendRequest(QUERY_SYS_EVENT, data, reply, option);
    if (res != ERR_OK) {
        HiLog::Error(LABEL, "send request failed, error is %{public}d.", res);
        return result;
    }
    ret = reply.ReadBool(result);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read result failed.");
    }
    return result;
}

bool SysEventServiceProxy::SetDebugMode(const sptr<ISysEventCallback>& callback, bool mode)
{
    bool result = false;
    auto remote = Remote();
    if (remote == nullptr) {
        HiLog::Error(LABEL, "SysEventService Remote is null.");
        return result;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(SysEventServiceProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "write descriptor failed.");
        return result;
    }
    bool ret = data.WriteRemoteObject(callback->AsObject());
    if (!ret) {
        HiLog::Error(LABEL, "parcel write callback failed.");
        return result;
    }
    ret = data.WriteBool(mode);
    if (!ret) {
        HiLog::Error(LABEL, "parcel write mode failed.");
        return result;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t res = remote->SendRequest(SET_DEBUG_MODE, data, reply, option);
    if (res != ERR_OK) {
        HiLog::Error(LABEL, "send request failed, error is %{public}d.", res);
        return result;
    }
    ret = reply.ReadBool(result);
    if (!ret) {
        HiLog::Error(LABEL, "parcel read result failed.");
        return result;
    }
    return result;
}
} // namespace HiviewDFX
} // namespace OHOS
