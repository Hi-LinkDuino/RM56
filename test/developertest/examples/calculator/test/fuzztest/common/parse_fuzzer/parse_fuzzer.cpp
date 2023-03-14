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

#include <cstddef>
#include <cstdint>

const int FUZZ_DATA_LEN = 3;
const int FUZZ_FST_DATA = 0;
const int FUZZ_SND_DATA = 1;
const int FUZZ_TRD_DATA = 2;
const int FUZZ_FTH_DATA = 3;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        bool result = false;
        if (size >= FUZZ_DATA_LEN) {
            result = data[FUZZ_FST_DATA] == 'F' &&
                data[FUZZ_SND_DATA] == 'U' &&
                data[FUZZ_TRD_DATA] == 'Z' &&
                data[FUZZ_FTH_DATA] == 'Z';
        }
        return result;
    }
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

