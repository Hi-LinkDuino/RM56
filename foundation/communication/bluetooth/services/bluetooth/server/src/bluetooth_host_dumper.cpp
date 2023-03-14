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

#include "bluetooth_host_dumper.h"

#include "bluetooth_host_server.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
namespace {
constexpr size_t MIN_ARGS_SIZE = 1;
const std::string ARGS_HELP = "-h";
const std::string ARGS_BR = "-br";
}

void BluetoothHostDumper::BluetoothDump(const std::vector<std::string>& args, std::string& result)
{
    result.clear();
    if (args.size() < MIN_ARGS_SIZE) {
        ShowDumpHelp(result);
        return;
    }

    if (args.size() == MIN_ARGS_SIZE) {
        // -h
        if (args[0] == ARGS_HELP) {
            ShowDumpHelp(result);
            return;
        }
        // -br
        if (args[0] == ARGS_BR) {
            BtCommStateDump(result);
            return;
        }
    }
    IllegalDumpInput(result);
}

void BluetoothHostDumper::ShowDumpHelp(std::string& result)
{
    result.append("Bluetooth Dump options:\n")
        .append("[-h]: show cmd help.\n")
        .append("[-br]: show common state.\n");
}

void BluetoothHostDumper::BtCommStateDump(std::string& result)
{
    sptr<BluetoothHostServer> hostServer = BluetoothHostServer::GetInstance();
    // bt conn state
    std::string btState = hostServer->GetBtState() ? "enabled\n" : "disabled\n";
    result.append("Bt enable state: ").append(btState);

    // ble conn state
    std::string bleState = hostServer->IsBleEnabled() ? "enabled\n" : "disabled\n";
    result.append("Ble enable state: ").append(bleState);
}

void BluetoothHostDumper::IllegalDumpInput(std::string& result)
{
    result.append("The dump args are illegal and you can enter '-h' for help.\n");
}
}  // namespace Bluetooth
}  // namespace OHOS
