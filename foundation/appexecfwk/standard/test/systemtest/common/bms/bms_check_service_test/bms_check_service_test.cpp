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

#include <fcntl.h>
#include <fstream>
#include <future>
#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "bundle_mgr_proxy.h"
#include "common_tool.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace testing::ext;
namespace OHOS {
namespace AppExecFwk {
class BmsCheckServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static sptr<IBundleMgr> GetBundleMgr();
};

sptr<IBundleMgr> BmsCheckServiceTest::GetBundleMgr()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        APP_LOGE("fail to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        APP_LOGE("fail to get bundle manager.");
        return nullptr;
    }

    APP_LOGI("get bundle manager success.");
    return iface_cast<IBundleMgr>(remoteObject);
}

void BmsCheckServiceTest::TearDownTestCase()
{}

void BmsCheckServiceTest::SetUp()
{}

void BmsCheckServiceTest::TearDown()
{}

void BmsCheckServiceTest::SetUpTestCase()
{}

/**
 * @tc.number: GetBundleMgr_0100
 * @tc.name: test get BMS
 * @tc.desc: get BMS form SystemAbilityManager
 */
HWTEST_F(BmsCheckServiceTest, GetBundleMgr_0100, Function | MediumTest | Level0)
{
    sptr<IBundleMgr> bundleMgr = GetBundleMgr();
    if (!bundleMgr) {
        APP_LOGE("bundle mgr is nullptr.");
        EXPECT_NE(bundleMgr, nullptr);
    }
}
/**
 * @tc.number: GetBundleInfos_0100
 * @tc.name: test query bundleinfos
 * @tc.desc: 1.under '/system/app/',there exist bundles
 *           2.install the bundles
 *           3.query all bundleinfos
 */
HWTEST_F(BmsCheckServiceTest, GetBundleInfos_0100, Function | MediumTest | Level0)
{
    sptr<IBundleMgr> bundleMgr = GetBundleMgr();
    if (!bundleMgr) {
        APP_LOGE("bundle mgr is nullptr.");
        EXPECT_NE(bundleMgr, nullptr);
    } else {
        std::cout << "GetBundleInfos" << std::endl;
        std::vector<BundleInfo> bundleInfos;
        bool getInfoResult = bundleMgr->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos);
        EXPECT_TRUE(getInfoResult);
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
