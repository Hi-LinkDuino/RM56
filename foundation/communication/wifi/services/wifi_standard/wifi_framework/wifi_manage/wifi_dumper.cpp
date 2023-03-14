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

#include "wifi_dumper.h"
#include <functional>
#include "wifi_logger.h"

DEFINE_WIFILOG_LABEL("WifiDumper");
namespace OHOS {
namespace Wifi {
const std::string ARGS_HELP = "-h";

void WifiDumper::PrintArgs(const std::vector<std::string>& vecArgs)
{
    std::string strArgs;
    for (auto& each: vecArgs) {
        strArgs += each + "|";
    }
    WIFI_LOGD("Dumper[%{public}zu] args: %{public}s", vecArgs.size(), strArgs.c_str());
}

std::string WifiDumper::ShowDeviceDumpUsage() const
{
    std::string help;
    return help.append("WiFi device dump options:\n")
        .append("  [-h]\n")
        .append("  description of the cmd option:\n")
        .append("    -h: show help.\n");
}

bool WifiDumper::DeviceDump(std::function<void(std::string&)> saBasicDumpFunc,
    const std::vector<std::string>& vecArgs, std::string& result)
{
    PrintArgs(vecArgs);
    result.clear();
    if (!vecArgs.empty() && vecArgs[0] == ARGS_HELP) {
        result = ShowDeviceDumpUsage();
        return true;
    }

    saBasicDumpFunc(result);
    return true;
}

std::string WifiDumper::ShowScanDumpUsage() const
{
    std::string help;
    return help.append("WiFi scan dump options:\n")
        .append("  [-h]\n")
        .append("  description of the cmd option:\n")
        .append("    -h: show help.\n");
}

bool WifiDumper::ScanDump(std::function<void(std::string&)> saBasicDumpFunc,
    const std::vector<std::string>& vecArgs, std::string& result)
{
    PrintArgs(vecArgs);
    result.clear();
    if (!vecArgs.empty() && vecArgs[0] == ARGS_HELP) {
        result = ShowScanDumpUsage();
        return true;
    }

    saBasicDumpFunc(result);
    return true;
}

std::string WifiDumper::ShowP2pDumpUsage() const
{
    std::string help;
    return help.append("WiFi P2P dump options:\n")
        .append("  [-h]\n")
        .append("  description of the cmd option:\n")
        .append("    -h: show help.\n");
}

bool WifiDumper::P2pDump(std::function<void(std::string&)> saBasicDumpFunc,
    const std::vector<std::string>& vecArgs, std::string& result)
{
    PrintArgs(vecArgs);
    result.clear();
    if (!vecArgs.empty() && vecArgs[0] == ARGS_HELP) {
        result = ShowP2pDumpUsage();
        return true;
    }

    saBasicDumpFunc(result);
    return true;
}

std::string WifiDumper::ShowHotspotDumpUsage() const
{
    std::string help;
    return help.append("WiFi hotspot dump options:\n")
        .append("  [-h]\n")
        .append("  description of the cmd option:\n")
        .append("    -h: show help.\n");
}

bool WifiDumper::HotspotDump(std::function<void(std::string&)> saBasicDumpFunc,
    const std::vector<std::string>& vecArgs, std::string& result)
{
    PrintArgs(vecArgs);
    result.clear();
    if (!vecArgs.empty() && vecArgs[0] == ARGS_HELP) {
        result = ShowHotspotDumpUsage();
        return true;
    }

    saBasicDumpFunc(result);
    return true;
}
}  // namespace Wifi
}  // namespace OHOS
