/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "setbluetoothscanmode_fuzzer.h"
#include "bluetooth_host.h"
#include "bluetooth_log.h"

using namespace std;
using namespace OHOS::Bluetooth;
const int FUZZ_DATA_LEN = 1;
namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        HILOGI("start");
        if ((data == nullptr) || (size < FUZZ_DATA_LEN)) {
            return false;
        }
        bool result = false;
        int scanMode = data[0] % 5;
        BluetoothHost *host = &BluetoothHost::GetDefaultHost();
        result = host->SetBtScanMode(scanMode, 0);
        return result;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

