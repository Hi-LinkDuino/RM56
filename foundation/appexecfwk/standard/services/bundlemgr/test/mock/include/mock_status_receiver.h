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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_INCLUDE_MOCK_STATUS_RECEIVER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_INCLUDE_MOCK_STATUS_RECEIVER_H

#include <future>

#include "nocopyable.h"

#include "status_receiver_interface.h"

namespace OHOS {
namespace AppExecFwk {
class MockStatusReceiver : public IStatusReceiver {
public:
    MockStatusReceiver() = default;
    virtual ~MockStatusReceiver() override = default;

    virtual void OnStatusNotify(const int32_t progress) override;
    virtual void OnFinished(const int32_t resultCode, [[maybe_unused]] const std::string &resultMsg) override;
    virtual sptr<IRemoteObject> AsObject() override;
    virtual void SetStreamInstallId(uint32_t installerId) override;
    int32_t GetResultCode();

private:
    std::promise<int32_t> signal_;

    DISALLOW_COPY_AND_MOVE(MockStatusReceiver);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_INCLUDE_MOCK_STATUS_RECEIVER_H