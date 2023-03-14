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

#include "ishared_result_set_stub.h"
#include <future>
#include "logger.h"
#include "rdb_errno.h"

namespace OHOS::NativeRdb {
std::function<sptr<ISharedResultSet>(std::shared_ptr<AbsSharedResultSet>,
    MessageParcel &)> ISharedResultSet::providerCreator_ = ISharedResultSetStub::CreateStub;
constexpr ISharedResultSetStub::Handler ISharedResultSetStub::handlers[ISharedResultSet::FUNC_BUTT];

sptr<ISharedResultSet> ISharedResultSetStub::CreateStub(std::shared_ptr<AbsSharedResultSet> result,
    OHOS::MessageParcel &parcel)
{
    sptr<ISharedResultSet> stub = new ISharedResultSetStub(result);
    parcel.WriteRemoteObject(stub->AsObject().GetRefPtr());
    result->Marshalling(parcel);
    return stub;
}

ISharedResultSetStub::ISharedResultSetStub(std::shared_ptr<AbsSharedResultSet> resultSet)
    : resultSet_(std::move(resultSet)),
      runnables_(MAX_RUNNABLE),
      thread_(&ISharedResultSetStub::Run, this)
{
    thread_.detach();
    LOG_ERROR("ISharedResultSetStub start thread(%{public}" PRIx64 ")", uint64_t(thread_.native_handle()));
}
ISharedResultSetStub::~ISharedResultSetStub()
{
    auto handle = thread_.native_handle();
    isRunning_ = false;
    // do not delete this code, this code is waiting the thread exit.
    isRunning_ = Submit([this]() -> bool { return isRunning_;}).get();
    LOG_ERROR("~ISharedResultSetStub thread(%{public}" PRIx64 ")", uint64_t(handle));
}

int ISharedResultSetStub::OnRemoteRequest(uint32_t code, OHOS::MessageParcel &data,
    OHOS::MessageParcel &reply, OHOS::MessageOption &option)
{
    if (GetDescriptor() != data.ReadInterfaceToken()) {
        LOG_ERROR("IPC descriptor is  not equal");
        return INVALID_FD;
    }

    if (code >= FUNC_BUTT) {
        LOG_ERROR("OnRemoteRequest method code(%{public}d) out of range", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    Handler handler = handlers[code];
    if (handler == nullptr) {
        LOG_ERROR("OnRemoteRequest method code(%{public}d) is not support", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    auto future = Submit([this, &data, &reply, handler]() -> int {
        return (this->*handler)(data, reply);
    });
    return future.get();
}

int ISharedResultSetStub::HandleGetRowCountRequest(MessageParcel &data, MessageParcel &reply)
{
    int count = -1;
    int errCode = resultSet_->GetRowCount(count);
    reply.WriteInt32(errCode);
    if (errCode == E_OK) {
        reply.WriteInt32(count);
    }
    LOG_DEBUG("HandleGetRowCountRequest call %{public}d", errCode);
    return NO_ERROR;
}

int ISharedResultSetStub::HandleGetAllColumnNamesRequest(MessageParcel &data, MessageParcel &reply)
{
    std::vector<std::string> names;
    int errCode = resultSet_->GetAllColumnNames(names);
    reply.WriteInt32(errCode);
    if (errCode == E_OK) {
        reply.WriteStringVector(names);
    }
    LOG_DEBUG("HandleGetAllColumnNamesRequest call %{public}d", errCode);
    return NO_ERROR;
}

int ISharedResultSetStub::HandleOnGoRequest(MessageParcel &data, MessageParcel &reply)
{
    int oldRow = data.ReadInt32();
    int newRow = data.ReadInt32();
    int errCode = resultSet_->OnGo(oldRow, newRow);
    reply.WriteInt32(errCode);
    LOG_DEBUG("HandleOnGoRequest call %{public}d", errCode);
    return NO_ERROR;
}

int ISharedResultSetStub::HandleCloseRequest(MessageParcel &data, MessageParcel &reply)
{
    int errCode = resultSet_->Close();
    reply.WriteInt32(errCode);
    LOG_DEBUG("HandleCloseRequest call %{public}d", errCode);
    return NO_ERROR;
}

void ISharedResultSetStub::Run()
{
    auto handle = thread_.native_handle();
    bool isRunning = true;
    while (isRunning) {
        auto runnable = runnables_.Pop();
        if (runnable == nullptr) {
            continue;
        }
        isRunning = runnable();
    }
    LOG_ERROR("ISharedResultSetStub thread(%{public}" PRIx64 ") is exited", uint64_t(handle));
}
} // namespace OHOS::NativeRdb