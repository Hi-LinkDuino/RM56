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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_INCLUDE_MOCK_BUNDLE_STATUS_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_INCLUDE_MOCK_BUNDLE_STATUS_H

#include <future>

#include "nocopyable.h"

#include "bundle_status_callback_interface.h"

namespace OHOS {
namespace AppExecFwk {
class MockBundleStatus : public IBundleStatusCallback {
public:
    MockBundleStatus() = default;
    virtual ~MockBundleStatus() override = default;

    virtual void OnBundleStateChanged(const uint8_t installType, const int32_t resultCode, const std::string &resultMsg,
        const std::string &bundleName) override;
    virtual void OnBundleAdded(const std::string &bundleName, const int userId) override {};
    virtual void OnBundleUpdated(const std::string &bundleName, const int userId) override {};
    virtual void OnBundleRemoved(const std::string &bundleName, const int userId) override {};
    virtual sptr<IRemoteObject> AsObject() override;
    int32_t GetResultCode();

private:
    std::promise<int32_t> signal_;

    DISALLOW_COPY_AND_MOVE(MockBundleStatus);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_TEST_MOCK_INCLUDE_MOCK_BUNDLE_STATUS_H