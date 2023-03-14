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

#include "setlocalname_fuzzer.h"
#include "bluetooth_host.h"
#include "bluetooth_log.h"
#include "securec.h"

using namespace std;
using namespace OHOS::Bluetooth;
const size_t FUZZ_MAX_NAME_LEN = 10;
namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        HILOGI("start");
        if ((data == nullptr) || (size < sizeof(int32_t))) {
            return false;
        }
        bool result = false;
        int len = (size > FUZZ_MAX_NAME_LEN) ? FUZZ_MAX_NAME_LEN : size;
        std::string localName(reinterpret_cast<const char*>(data), len);
        for (int i = 0; i < len; i++) {
            char c = localName[i];
            bool ret = (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
            if (!ret) {
                localName[i] = 'a';
            }
        }
        BluetoothHost *host = &BluetoothHost::GetDefaultHost();
        HILOGI("name:%{public}s", localName.c_str());
        result = host->SetLocalName(localName);
        HILOGI("result:%{public}d", result);
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

