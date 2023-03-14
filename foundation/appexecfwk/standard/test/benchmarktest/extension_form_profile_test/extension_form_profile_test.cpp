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

#include "extension_form_profile.h"

#include <benchmark/benchmark.h>

using namespace std;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
    /**
     * @tc.name: BenchmarkTestForTransformTo
     * @tc.desc: Testcase for testing 'TransformTo' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    static void BenchmarkTestForTransformTo(benchmark::State &state)
    {
        ExtensionFormProfile info;
        const std::string formProfile;
        std::vector<ExtensionFormInfo> infos;
        infos[0].name = "com.ohos.contactsdataability";
        infos[0].description = "dataability_description";
        for (auto _ : state) {
            /* @tc.steps: step1.call TransformTo in loop */
            info.TransformTo(formProfile, infos);
        }
    }

    BENCHMARK(BenchmarkTestForTransformTo)->Iterations(1000);
}

BENCHMARK_MAIN();