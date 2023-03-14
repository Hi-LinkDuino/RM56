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

#ifndef DATAABILITY_I_SHARED_RESULT_SET_STUB_H
#define DATAABILITY_I_SHARED_RESULT_SET_STUB_H
#include <functional>
#include <future>
#include "safe_block_queue.h"
#include "ishared_result_set.h"
#include "iremote_stub.h"
namespace OHOS::NativeRdb {
class ISharedResultSetStub : public IRemoteStub<ISharedResultSet> {
public:
    explicit ISharedResultSetStub(std::shared_ptr<AbsSharedResultSet> resultSet);
    ~ISharedResultSetStub();
    static sptr<ISharedResultSet> CreateStub(std::shared_ptr<AbsSharedResultSet> resultSet, MessageParcel &parcel);
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    
protected:
    int HandleGetRowCountRequest(MessageParcel &data, MessageParcel &reply);
    int HandleGetAllColumnNamesRequest(MessageParcel &data, MessageParcel &reply);
    int HandleOnGoRequest(MessageParcel &data, MessageParcel &reply);
    int HandleCloseRequest(MessageParcel &data, MessageParcel &reply);
    template<typename F, typename... Args>
    auto Submit(F&& func, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
    {
        auto exec = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
        auto task = std::make_shared<std::packaged_task<typename std::result_of<F(Args...)>::type()>>(std::move(exec));
        auto future = task->get_future();
        runnables_.Push([task, this] {
            bool isRunning = isRunning_;
            (*task)();
            return isRunning;
        });
        return future;
    }

private:
    void Run();
    using Handler = int(ISharedResultSetStub::*)(MessageParcel &request, MessageParcel &reply);
    static constexpr int MAX_RUNNABLE = 1024;
    std::shared_ptr<AbsSharedResultSet> resultSet_;
    SafeBlockQueue<std::function<bool()>> runnables_;
    bool isRunning_ = true;
    std::thread thread_;
    static constexpr Handler handlers[FUNC_BUTT] {
        &ISharedResultSetStub::HandleGetRowCountRequest,
        &ISharedResultSetStub::HandleGetAllColumnNamesRequest,
        &ISharedResultSetStub::HandleOnGoRequest,
        &ISharedResultSetStub::HandleCloseRequest,
    };
};
} // namespace OHOS::NativeRdb

#endif // DATAABILITY_I_SHARED_RESULT_SET_STUB_H
