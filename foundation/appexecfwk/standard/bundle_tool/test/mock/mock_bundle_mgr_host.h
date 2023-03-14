/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_STANDARD_TOOLS_TEST_MOCK_MOCK_BUNDLE_MGR_HOST_H
#define FOUNDATION_APPEXECFWK_STANDARD_TOOLS_TEST_MOCK_MOCK_BUNDLE_MGR_HOST_H

#include "gmock/gmock.h"

#include "app_log_wrapper.h"
#include "bundle_mgr_host.h"

namespace OHOS {
namespace AppExecFwk {
class MockBundleMgrHost : public BundleMgrHost {
public:
    bool CleanBundleCacheFiles(const std::string &bundleName, const sptr<ICleanCacheCallback> &cleanCacheCallback,
        int32_t userId = Constants::UNSPECIFIED_USERID) override;
    bool CleanBundleDataFiles(const std::string &bundleName, const int userId = 0);
    bool DumpInfos(
        const DumpFlag flag, const std::string &bundleName, int32_t userId, std::string &result);
    bool SetApplicationEnabled(const std::string &bundleName, bool isEnable,
        int32_t userId = Constants::UNSPECIFIED_USERID) override;
    bool SetAbilityEnabled(const AbilityInfo &abilityInfo, bool isEnable,
        int32_t userId = Constants::UNSPECIFIED_USERID) override;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_STANDARD_TOOLS_TEST_MOCK_MOCK_BUNDLE_MGR_HOST_H
