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

#include "bundle_mgr_client.h"

#include <benchmark/benchmark.h>

using namespace std;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
    /**
     * @tc.name: BenchmarkTestForGetBundleNameForUid
     * @tc.desc: Testcase for testing 'GetBundleNameForUid' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetBundleNameForUid(benchmark::State &state)
    {
        BundleMgrClient client;
        int uid = 100;
        std::string bundleName = "ohos.global.systemres";
        for (auto _ : state) {
            /* @tc.steps: step1.call GetBundleNameForUid in loop */
            client.GetBundleNameForUid(uid, bundleName);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetBundleInfo
     * @tc.desc: Testcase for testing 'GetBundleInfo' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetBundleInfo(benchmark::State &state)
    {
        BundleMgrClient client;
        std::string bundleName = "ohos.global.systemres";
        BundleInfo bundleInfo;
        Parcel parcel;
        bundleInfo.Marshalling(parcel);
        for (auto _ : state) {
            /* @tc.steps: step1.call GetBundleInfo in loop */
            client.GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetHapModuleInfo
     * @tc.desc: Testcase for testing 'GetHapModuleInfo' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetHapModuleInfo(benchmark::State &state)
    {
        BundleMgrClient client;
        std::string bundleName = "ohos.global.systemres";
        std::string hapName = "ohos_global_systemres";
        HapModuleInfo hapModuleInfo;
        Parcel parcel;
        hapModuleInfo.Marshalling(parcel);
        for (auto _ : state) {
            /* @tc.steps: step1.call GetHapModuleInfo in loop */
            client.GetHapModuleInfo(bundleName, hapName, hapModuleInfo);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetResConfigFile1
     * @tc.desc: Testcase for testing 'GetResConfigFile' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetResConfigFile1(benchmark::State &state)
    {
        BundleMgrClient client;
        HapModuleInfo hapModuleInfo;
        Parcel parcel;
        hapModuleInfo.Marshalling(parcel);
        std::string metadataName = "";
        std::vector<std::string> profileInfos;
        for (auto _ : state) {
            /* @tc.steps: step1.call GetResConfigFile in loop */
            client.GetResConfigFile(hapModuleInfo, metadataName, profileInfos);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetResConfigFile2
     * @tc.desc: Testcase for testing 'GetResConfigFile' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetResConfigFile2(benchmark::State &state)
    {
        BundleMgrClient client;
        ExtensionAbilityInfo extensionInfo;
        Parcel parcel;
        extensionInfo.Marshalling(parcel);
        std::string metadataName = "";
        std::vector<std::string> profileInfos;
        for (auto _ : state) {
            /* @tc.steps: step1.call GetResConfigFile in loop */
            client.GetResConfigFile(extensionInfo, metadataName, profileInfos);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetResConfigFile3
     * @tc.desc: Testcase for testing 'GetResConfigFile' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetResConfigFile3(benchmark::State &state)
    {
        BundleMgrClient client;
        AbilityInfo abilityInfo;
        Parcel parcel;
        abilityInfo.Marshalling(parcel);
        std::string metadataName = "";
        std::vector<std::string> profileInfos;
        for (auto _ : state) {
            /* @tc.steps: step1.call GetResConfigFile in loop */
            client.GetResConfigFile(abilityInfo, metadataName, profileInfos);
        }
    }

    /**
     * @tc.name: BenchmarkTestForGetAccessibleAppCodePaths
     * @tc.desc: Testcase for testing 'GetAccessibleAppCodePaths' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForGetAccessibleAppCodePaths(benchmark::State &state)
    {
        BundleMgrClient client;
        int32_t userId = 100;
        for (auto _ : state) {
            /* @tc.steps: step1.call GetAccessibleAppCodePaths in loop */
            client.GetAccessibleAppCodePaths(userId);
        }
    }

    BENCHMARK(BenchmarkTestForGetBundleNameForUid)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetBundleInfo)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetHapModuleInfo)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetResConfigFile1)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetResConfigFile2)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetResConfigFile3)->Iterations(1000);
    BENCHMARK(BenchmarkTestForGetAccessibleAppCodePaths)->Iterations(1000);
}

BENCHMARK_MAIN();