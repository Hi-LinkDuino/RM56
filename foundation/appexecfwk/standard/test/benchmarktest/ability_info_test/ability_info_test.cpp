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

#include "ability_info.h"

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
        AbilityInfo info;
        info.name = "allLogAbility";
        info.description = "dataability_description";
        info.iconPath = "$media:icon";
        info.labelId = 0;
        info.descriptionId = 0;
        info.iconId = 0;
        info.kind = "data";
        info.deviceTypes = {"smartVision"};
        info.uri = "dataability://com.ohos.callogability";
        info.readPermission = "ohos.permission.READ_CALL_LOG";
        info.writePermission = "ohos.permission.WRITE_CALL_LOG";
        info.package = "com.ohos.contactsdataability";
        info.bundleName = "com.ohos.contactsdataability";
        info.moduleName = "entry";
        info.applicationName = "com.ohos.contactsdataability";
        info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
            "/com.ohos.contactsdataability/assets/entry/resources.index";
        info.versionName = "1.0";
        info.applicationInfo.name = "com.ohos.contactsdataability";
        info.applicationInfo.description = "dataability_description";
        info.applicationInfo.cpuAbi = "armeabi";
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
        AbilityInfo info;
        info.name = "allLogAbility";
        info.description = "dataability_description";
        info.iconPath = "$media:icon";
        info.labelId = 0;
        info.descriptionId = 0;
        info.iconId = 0;
        info.kind = "data";
        info.deviceTypes = {"smartVision"};
        info.uri = "dataability://com.ohos.callogability";
        info.readPermission = "ohos.permission.READ_CALL_LOG";
        info.writePermission = "ohos.permission.WRITE_CALL_LOG";
        info.package = "com.ohos.contactsdataability";
        info.bundleName = "com.ohos.contactsdataability";
        info.moduleName = "entry";
        info.applicationName = "com.ohos.contactsdataability";
        info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
            "/com.ohos.contactsdataability/assets/entry/resources.index";
        info.versionName = "1.0";
        info.applicationInfo.name = "com.ohos.contactsdataability";
        info.applicationInfo.description = "dataability_description";
        info.applicationInfo.cpuAbi = "armeabi";
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
        AbilityInfo info;
        info.name = "allLogAbility";
        info.description = "dataability_description";
        info.iconPath = "$media:icon";
        info.labelId = 0;
        info.descriptionId = 0;
        info.iconId = 0;
        info.kind = "data";
        info.deviceTypes = {"smartVision"};
        info.uri = "dataability://com.ohos.callogability";
        info.readPermission = "ohos.permission.READ_CALL_LOG";
        info.writePermission = "ohos.permission.WRITE_CALL_LOG";
        info.package = "com.ohos.contactsdataability";
        info.bundleName = "com.ohos.contactsdataability";
        info.moduleName = "entry";
        info.applicationName = "com.ohos.contactsdataability";
        info.resourcePath = "/data/app/el1/budle/public/com.ohos.contactsdataability"\
            "/com.ohos.contactsdataability/assets/entry/resources.index";
        info.versionName = "1.0";
        info.applicationInfo.name = "com.ohos.contactsdataability";
        info.applicationInfo.description = "dataability_description";
        info.applicationInfo.cpuAbi = "armeabi";
        Parcel parcel;
        info.Marshalling(parcel);
        for (auto _ : state) {
            /* @tc.steps: step1.call Unmarshalling in loop */
            info.Unmarshalling(parcel);
        }
    }

    BENCHMARK(BenchmarkTestForReadFromParcel)->Iterations(1000);
    BENCHMARK(BenchmarkTestForMarshalling)->Iterations(1000);
    BENCHMARK(BenchmarkTestForUnmarshalling)->Iterations(1000);
}

BENCHMARK_MAIN();