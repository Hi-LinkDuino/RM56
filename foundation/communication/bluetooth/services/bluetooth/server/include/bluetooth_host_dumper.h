/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_BLUETOOTH_STANDARD_HOST_DUMPER_H
#define OHOS_BLUETOOTH_STANDARD_HOST_DUMPER_H

#include <vector>
#include <string>

namespace OHOS {
namespace Bluetooth {
class BluetoothHostDumper {
public:
    static void BluetoothDump(const std::vector<std::string>& args, std::string& result);

private:
    BluetoothHostDumper() = default;
    ~BluetoothHostDumper() = default;

    static void ShowDumpHelp(std::string& result);
    static void BtCommStateDump(std::string& result);
    static void IllegalDumpInput(std::string& result);
    static bool DumpDefault(std::string& result);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_HOST_DUMPER_H