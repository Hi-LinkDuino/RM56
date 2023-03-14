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

#include "distributed_bundle_info.h"

#include <benchmark/benchmark.h>

using namespace std;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
    /**
     * @tc.name: BenchmarkTestForReadFromParcel
     * @tc.desc: Testcase for testing 'ReadFromParcel' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForReadFromParcel(benchmark::State &state)
    {
        DistributedBundleInfo info;
        info.bundleName = "com.ohos.contactsdataability";
        info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
            "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
        Parcel parcel;
        info.Marshalling(parcel);
        for (auto _ : state) {
            /* @tc.steps: step1.call ReadFromParcel in loop */
            info.ReadFromParcel(parcel);
        }
    }

    /**
     * @tc.name: BenchmarkTestForMarshalling
     * @tc.desc: Testcase for testing 'Marshalling' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForMarshalling(benchmark::State &state)
    {
        DistributedBundleInfo info;
        info.bundleName = "com.ohos.contactsdataability";
        info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
            "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
        Parcel parcel;
        for (auto _ : state) {
            /* @tc.steps: step1.call Marshalling in loop */
            info.Marshalling(parcel);
        }
    }

    /**
     * @tc.name: BenchmarkTestForUnmarshalling
     * @tc.desc: Testcase for testing 'Unmarshalling' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForUnmarshalling(benchmark::State &state)
    {
        DistributedBundleInfo info;
        info.bundleName = "com.ohos.contactsdataability";
        info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
            "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
        Parcel parcel;
        info.Marshalling(parcel);
        for (auto _ : state) {
            /* @tc.steps: step1.call Unmarshalling in loop */
            info.Unmarshalling(parcel);
        }
    }

    /**
     * @tc.name: BenchmarkTestForToString
     * @tc.desc: Testcase for testing 'ToString' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForToString(benchmark::State &state)
    {
        DistributedBundleInfo info;
        info.bundleName = "com.ohos.contactsdataability";
        info.appId = "com.ohos.contactsdataability_BNtg4JBClbl92Rgc3jm"\
            "/RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=";
        for (auto _ : state) {
            /* @tc.steps: step1.call ToString in loop */
            info.ToString();
        }
    }

    /**
     * @tc.name: BenchmarkTestForFromJsonString
     * @tc.desc: Testcase for testing 'FromJsonString' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForFromJsonString(benchmark::State &state)
    {
        DistributedBundleInfo info;
        const std::string jsonString = R"({"appId": "ohos.global.systemres_BNtg4JBClbl92Rgc3jm/
            RfcAdrHXaM8F0QOiwVEhnV5ebE5jNIYnAx+weFRT3QTyUjRNdhmc2aAzWyi+5t5CoBM=",
            "moduleInfos": [{
                "moduleName":"ohos.global.systemres",
                "abilities": [{
                    "abilityName":"",
                    "permissions":[],
                    "type":"page",
                    "enabled":true,
                }]
            }],
            "compatibleVersionCode": 3,
            "mainAbility": "",
            "minCompatibleVersion": 1,
            "bundleName": "ohos.global.systemres",
            "targetVersionCode": 3,
            "version": 1,
            "versionCode": 1,
            "versionName": "1.0.0.1"})";
        for (auto _ : state) {
            /* @tc.steps: step1.call FromJsonString in loop */
            info.FromJsonString(jsonString);
        }
    }

    BENCHMARK(BenchmarkTestForReadFromParcel)->Iterations(1000);
    BENCHMARK(BenchmarkTestForMarshalling)->Iterations(1000);
    BENCHMARK(BenchmarkTestForUnmarshalling)->Iterations(1000);
    BENCHMARK(BenchmarkTestForToString)->Iterations(1000);
    BENCHMARK(BenchmarkTestForFromJsonString)->Iterations(1000);
}

BENCHMARK_MAIN();