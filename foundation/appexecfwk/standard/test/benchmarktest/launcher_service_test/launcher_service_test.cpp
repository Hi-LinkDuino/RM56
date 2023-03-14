/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "launcher_service.h"

#include <benchmark/benchmark.h>

namespace OHOS {
namespace AppExecFwk {
        class TestBundleStatusCallback : public IBundleStatusCallback {
        public:
            TestBundleStatusCallback() = default;
            explicit TestBundleStatusCallback(std::string code) : testCode_(code)
            {}
            ~TestBundleStatusCallback() = default;
            virtual void OnBundleStateChanged(const uint8_t installType, const int32_t resultCode,
                const std::string &resultMsg, const std::string &bundleName) override;
            virtual void OnBundleAdded(const std::string &bundleName, const int userId) override;
            virtual void OnBundleUpdated(const std::string &bundleName, const int userId) override;
            virtual void OnBundleRemoved(const std::string &bundleName, const int userId) override;
            virtual sptr<IRemoteObject> AsObject() override;
        private:
            std::string testCode_ {};
        };
        void TestBundleStatusCallback::OnBundleStateChanged(const uint8_t installType, const int32_t resultCode,
            const std::string &resultMsg, const std::string &bundleName)
        {
        }
        void TestBundleStatusCallback::OnBundleAdded(const std::string &bundleName, const int userId)
        {
        }

        void TestBundleStatusCallback::OnBundleUpdated(const std::string &bundleName, const int userId)
        {
        }

        void TestBundleStatusCallback::OnBundleRemoved(const std::string &bundleName, const int userId)
        {
        }

        sptr<IRemoteObject> TestBundleStatusCallback::AsObject()
        {
            return nullptr;
        }
    }
}

using namespace std;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
    /**
     * @tc.name: BenchmarkTestForRegisterCallback
     * @tc.desc: Testcase for testing 'RegisterCallback' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForRegisterCallback(benchmark::State &state)
    {
        LauncherService launcherservice;
        sptr<TestBundleStatusCallback> callback = new TestBundleStatusCallback();
        for (auto _ : state) {
            /* @tc.steps: step1.call RegisterCallback in loop */
            launcherservice.RegisterCallback(callback);
        }
    }

    /**
     * @tc.name: BenchmarkTestForUnRegisterCallback
     * @tc.desc: Testcase for testing 'UnRegisterCallback' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForUnRegisterCallback(benchmark::State &state)
    {
        LauncherService launcherservice;
        for (auto _ : state) {
            /* @tc.steps: step1.call UnRegisterCallback in loop */
            launcherservice.UnRegisterCallback();
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetAbilityList
     * @tc.desc: Testcase for testing 'GetAbilityList' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetAbilityList(benchmark::State &state)
    {
        LauncherService launcherservice;
        std::string bundleName = "ohos.global.systemres";
        int userId = 100;
        std::vector<LauncherAbilityInfo> launcherAbilityInfos;
        for (auto _ : state) {
            /* @tc.steps: step1.call GetAbilityList in loop */
            launcherservice.GetAbilityList(bundleName, userId, launcherAbilityInfos);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetAbilityInfo
     * @tc.desc: Testcase for testing 'GetAbilityInfo' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetAbilityInfo(benchmark::State &state)
    {
        LauncherService launcherservice;
        OHOS::AAFwk::Want want;
        int userId = 100;
        LauncherAbilityInfo launcherAbilityInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call GetAbilityInfo in loop */
            launcherservice.GetAbilityInfo(want, userId, launcherAbilityInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetApplicationInfo
     * @tc.desc: Testcase for testing 'GetApplicationInfo' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetApplicationInfo(benchmark::State &state)
    {
        LauncherService launcherservice;
        std::string bundleName = "ohos.global.systemres";
        int userId = 100;
        ApplicationInfo applicationInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call GetApplicationInfo in loop */
            launcherservice.GetApplicationInfo(bundleName,
                ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, applicationInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForIsBundleEnabled
     * @tc.desc: Testcase for testing 'IsBundleEnabled' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForIsBundleEnabled(benchmark::State &state)
    {
        LauncherService launcherservice;
        std::string bundleName = "ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call IsBundleEnabled in loop */
            launcherservice.IsBundleEnabled(bundleName);
        }
    }

    /**
     * @tc.name: BenchmarkTestForIsAbilityEnabled
     * @tc.desc: Testcase for testing 'IsAbilityEnabled' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForIsAbilityEnabled(benchmark::State &state)
    {
        LauncherService launcherservice;
        AbilityInfo abilityInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call IsAbilityEnabled in loop */
            launcherservice.IsAbilityEnabled(abilityInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetShortcutInfos
     * @tc.desc: Testcase for testing 'GetShortcutInfos' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetShortcutInfos(benchmark::State &state)
    {
        LauncherService launcherservice;
        std::string bundleName = "ohos.global.systemres";
        std::vector<ShortcutInfo> shortcutInfo;
        for (auto _ : state) {
            /* @tc.steps: step1.call GetShortcutInfos in loop */
            launcherservice.GetShortcutInfos(bundleName, shortcutInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetAllLauncherAbilityInfos
     * @tc.desc: Testcase for testing 'GetAllLauncherAbilityInfos' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetAllLauncherAbilityInfos(benchmark::State &state)
    {
        LauncherService launcherservice;
        int32_t userId = 100;
        std::vector<LauncherAbilityInfo> launcherAbilityInfos;
        for (auto _ : state) {
            /* @tc.steps: step1.call GetAllLauncherAbilityInfos in loop */
            launcherservice.GetAllLauncherAbilityInfos(userId, launcherAbilityInfos);
        }
    }

    BENCHMARK(BenchmarkTestForRegisterCallback)->Iterations(1000);
    BENCHMARK(BenchmarkTestForUnRegisterCallback)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetAbilityList)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetAbilityInfo)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetApplicationInfo)->Iterations(1000);
    BENCHMARK(BenchmarkTestForIsBundleEnabled)->Iterations(1000);
    BENCHMARK(BenchmarkTestForIsAbilityEnabled)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetShortcutInfos)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetAllLauncherAbilityInfos)->Iterations(1000);
}

BENCHMARK_MAIN();