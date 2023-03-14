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

#include "distributed_cfg.h"

#include <iostream>
#include <fstream>

#include "hilog/log.h"

namespace OHOS {
namespace DistributeSystemTest {
using namespace std;
static constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DistributeTestEnvironment"};

DistributedCfg::DistributedCfg()
{
}

DistributedCfg::~DistributedCfg()
{
}

bool DistributedCfg::OpenCfg(std::string fileName)
{
    ifstream cfgFile;
    size_t  position = 0;
    std::string strline;

    cfgFile.open(fileName.c_str());
    if (!cfgFile.is_open()) {
        return false;
    }

    std::string key;
    std::string value;
    while (getline(cfgFile, strline)) {
        if (strline == "") {
            continue;
        }
        position = strline.find(":");
        if (position == std::string::npos) {
            continue;
        }
        key = strline.substr(0, position);
        value = strline.substr(position + 1);
        cfgMap_.insert(std::pair<std::string, std::string>(key, value));
    }

    cfgFile.close();
    return true;
}

bool DistributedCfg::GetCfgVal(std::string key, std::string &value)
{
    std::string iplist = "";
    std::map<std::string, std::string>::iterator cfgIterator;
    cfgIterator = cfgMap_.find(key);
    if (cfgIterator == cfgMap_.end()) {
        return false;
    }
    value = cfgIterator->second;
    return true;
}

std::string DistributedCfg::GetValueInString(std::string str, size_t devNo)
{
    size_t pos = 0;
    size_t posend = 0;
    for (size_t i = 0; i < devNo; i++) {
        posend = str.find(",", pos);
        if (posend == std::string::npos) {
            return "";
        }
        pos = posend + 1;
    }
    std::string ipaddr;
    posend = str.find(",", pos);
    if (posend != std::string::npos) {
        ipaddr = str.substr(pos, posend - pos);
    } else {
        ipaddr = "";
    }
    return ipaddr;
}

std::string DistributedCfg::GetDeviceIp(std::string fileName, size_t devNo)
{
    devNo--;
    if (!OpenCfg(fileName)) {
        HiviewDFX::HiLog::Error(LABEL,
                                "OpenCfg() failed! make sure the filename:%s of major or agent",
                                fileName.c_str());
        return "";
    }
    std::string valueOfIps;
    if (!GetCfgVal("agentlist", valueOfIps)) {
        HiviewDFX::HiLog::Error(LABEL,
                                "GetCfgVal() failed! make sure the filename:%s of major or agent",
                                fileName.c_str());
        return "";
    }
    std::string ip = GetValueInString(valueOfIps, devNo);
    if (!ip.compare("")) {
        HiviewDFX::HiLog::Error(LABEL,
                                "GetValueOfString() return ""! %zu maybe bigger than the sum of devices_online",
                                devNo + 1);
        return "";
    }
    HiviewDFX::HiLog::Info(LABEL, "get %zu device's ip :  %s", devNo + 1, ip.c_str());
    return ip;
}

std::string DistributedCfg::GetDeviceUuid(std::string fileName, size_t devNo)
{
    if (!OpenCfg(fileName)) {
        HiviewDFX::HiLog::Error(LABEL,
                                "OpenCfg() failed! make sure the filename:%s of major or agent",
                                fileName.c_str());
        return "";
    }
    std::string valueOfUuids;
    if (!GetCfgVal("devicesuuid", valueOfUuids)) {
        HiviewDFX::HiLog::Error(LABEL,
                                "GetCfgVal() failed! make sure the filename:%s of major or agent",
                                fileName.c_str());
        return "";
    }
    std::string uuid = GetValueInString(valueOfUuids, devNo);
    if (!uuid.compare("")) {
        printf("device:%zu uuid is null \n", devNo);
        HiviewDFX::HiLog::Error(LABEL,
                                "GetValueOfString() return ""! %zu maybe bigger than the sum of devices_online",
                                devNo);
        return "";
    }
    HiviewDFX::HiLog::Info(LABEL, "get %zu device's uuid :  %s", devNo, uuid.c_str());
    return uuid;
}

std::unique_ptr<DistributedCfg>& DistributedCfg::GetInstance()
{
    if (DistributedCfg::getCfg_ == nullptr) {
        DistributedCfg::getCfg_ =  std::make_unique<DistributedCfg>();
    }
    return DistributedCfg::getCfg_;
}

std::unique_ptr<DistributedCfg> DistributedCfg::getCfg_ = nullptr;
} // namespace DistributeSystemTest
} // namespace OHOS
