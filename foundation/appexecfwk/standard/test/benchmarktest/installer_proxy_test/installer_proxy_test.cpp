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

#include <benchmark/benchmark.h>

#include "bundle_constants.h"
#include "bundle_installer_interface.h"
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "status_receiver_host.h"

using namespace std::chrono_literals;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
namespace {
const std::string THIRD_BUNDLE_PATH = "/data/test/benchmark/";

class InstallerProxyTest : public StatusReceiverHost {
public:
    InstallerProxyTest();
    virtual ~InstallerProxyTest() override;
    virtual void OnStatusNotify(const int progress) override;
    virtual void OnFinished(const int32_t resultCode, const std::string &resultMsg) override;
};

InstallerProxyTest::InstallerProxyTest()
{}

InstallerProxyTest::~InstallerProxyTest()
{}

void InstallerProxyTest::OnStatusNotify(const int progress)
{}

void InstallerProxyTest::OnFinished(const int32_t resultCode, const std::string &resultMsg)
{}

sptr<IBundleMgr> GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        return nullptr;
    }

    return iface_cast<IBundleMgr>(remoteObject);
}

sptr<IBundleInstaller> GetInstallerProxy()
{
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    if (!bundleMgrProxy) {
        return nullptr;
    }

    sptr<IBundleInstaller> installerProxy = bundleMgrProxy->GetBundleInstaller();
    if (!installerProxy) {
        return nullptr;
    }

    return installerProxy;
}

/**
 * @tc.name: BenchmarkTestInstallerProxyInfo
 * @tc.desc: Testcase for testing Installs an application through the proxy object.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestInstallerProxyInfo(benchmark::State &state)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "test.hap";
    InstallParam installParam;
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    sptr<InstallerProxyTest> statusReceiver(new (std::nothrow) InstallerProxyTest());
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    }
}

/**
 * @tc.name: BenchmarkTestInstallerProxyInfo
 * @tc.desc: Testcase for testing Installs multiple haps.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestMultipleInstallerProxyInfo(benchmark::State &state)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    std::vector<std::string> bundleFilePaths;
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "test.hap";
    bundleFilePaths.push_back(bundleFilePath);
    InstallParam installParam;
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    sptr<InstallerProxyTest> statusReceiver(new (std::nothrow) InstallerProxyTest());
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        installerProxy->Install(bundleFilePaths, installParam, statusReceiver);
    }
}

/**
 * @tc.name: BenchmarkTestInstallerProxyInfo
 * @tc.desc: Testcase for testing Uninstalls an application through the proxy object.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

static void BenchmarkTestUninstallerApplication(benchmark::State &state)
{
    sptr<IBundleInstaller> installerProxy = GetInstallerProxy();
    InstallParam installParam;
    installParam.installFlag = InstallFlag::REPLACE_EXISTING;
    installParam.userId = Constants::DEFAULT_USERID;
    sptr<InstallerProxyTest> statusReceiver = (new (std::nothrow) InstallerProxyTest());
    const std::string bundleName = "com.example.l3jsdemo";
    std::string bundleFilePath = THIRD_BUNDLE_PATH + "test.hap";
    installerProxy->Install(bundleFilePath, installParam, statusReceiver);
    for (auto _ : state) {
        /* @tc.steps: step1.call ReadFromParcel in loop */
        installerProxy->Uninstall(bundleName, installParam, statusReceiver);
    }
}

BENCHMARK(BenchmarkTestInstallerProxyInfo)->Iterations(1000);
BENCHMARK(BenchmarkTestMultipleInstallerProxyInfo)->Iterations(1000);
BENCHMARK(BenchmarkTestUninstallerApplication)->Iterations(1000);
}  // namespace

BENCHMARK_MAIN();