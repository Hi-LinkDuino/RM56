/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "hidump_helper.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const int32_t MIN_ARGS_SIZE = 1;
const int32_t MAX_ARGS_SIZE = 2;
const int32_t FIRST_PARAM = 0;
const int32_t SECOND_PARAM = 1;
const std::string ARGS_HELP = "-h";
const std::string ARGS_ABILITY = "-ability";
const std::string ARGS_ABILITY_LIST = "-ability-list";
const std::string ARGS_BUNDLE = "-bundle";
const std::string ARGS_BUNDLE_LIST = "-bundle-list";
const std::string ARGS_DEVICEID = "-device";
const std::string ILLEGAL_INFOMATION = "The arguments are illegal and you can enter '-h' for help.\n";
const std::string NO_INFOMATION = "no such infomation\n";

const std::unordered_map<std::string, HidumpFlag> ARGS_MAP = {
    { ARGS_HELP, HidumpFlag::GET_HELP },
    { ARGS_ABILITY, HidumpFlag::GET_ABILITY },
    { ARGS_ABILITY_LIST, HidumpFlag::GET_ABILITY_LIST },
    { ARGS_BUNDLE, HidumpFlag::GET_BUNDLE },
    { ARGS_BUNDLE_LIST, HidumpFlag::GET_BUNDLE_LIST },
    { ARGS_DEVICEID, HidumpFlag::GET_DEVICEID },
};
}

HidumpHelper::HidumpHelper(const std::weak_ptr<BundleDataMgr> &dataMgr)
    : dataMgr_(dataMgr) {}

bool HidumpHelper::Dump(const std::vector<std::string>& args, std::string &result)
{
    result.clear();
    ErrCode errCode = ERR_OK;
    int32_t argsSize = static_cast<int32_t>(args.size());
    switch (argsSize) {
        case MIN_ARGS_SIZE: {
            errCode = ProcessOneParam(args[FIRST_PARAM], result);
            break;
        }
        case MAX_ARGS_SIZE: {
            errCode = ProcessTwoParam(args[FIRST_PARAM], args[SECOND_PARAM], result);
            break;
        }
        default: {
            errCode = ERR_APPEXECFWK_HIDUMP_INVALID_ARGS;
            break;
        }
    }

    bool ret = false;
    switch (errCode) {
        case ERR_OK: {
            ret = true;
            break;
        }
        case ERR_APPEXECFWK_HIDUMP_INVALID_ARGS: {
            ShowIllealInfomation(result);
            ret = true;
            break;
        }
        case ERR_APPEXECFWK_HIDUMP_UNKONW: {
            result.append(NO_INFOMATION);
            ret = true;
            break;
        }
        case ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR: {
            ret = false;
            break;
        }
        default: {
            break;
        }
    }

    return ret;
}

ErrCode HidumpHelper::ProcessOneParam(const std::string& args, std::string &result)
{
    HidumpParam hidumpParam;
    auto operatorIter = ARGS_MAP.find(args);
    if (operatorIter != ARGS_MAP.end()) {
        hidumpParam.hidumpFlag = operatorIter->second;
    }

    if (hidumpParam.hidumpFlag == HidumpFlag::GET_HELP) {
        ShowHelp(result);
        return ERR_OK;
    }

    return ProcessDump(hidumpParam, result);
}

ErrCode HidumpHelper::ProcessTwoParam(
    const std::string& firstParam, const std::string& secondParam, std::string &result)
{
    HidumpParam hidumpParam;
    hidumpParam.args = secondParam;
    auto operatorIter = ARGS_MAP.find(firstParam);
    if (operatorIter != ARGS_MAP.end()) {
        hidumpParam.hidumpFlag = operatorIter->second;
    }

    switch (hidumpParam.hidumpFlag) {
        case HidumpFlag::GET_ABILITY: {
            hidumpParam.hidumpFlag = HidumpFlag::GET_ABILITY_BY_NAME;
            break;
        }
        case HidumpFlag::GET_BUNDLE: {
            hidumpParam.hidumpFlag = HidumpFlag::GET_BUNDLE_BY_NAME;
            break;
        }
        default: {
            break;
        }
    }

    return ProcessDump(hidumpParam, result);
}

ErrCode HidumpHelper::ProcessDump(const HidumpParam& hidumpParam, std::string &result)
{
    result.clear();
    ErrCode errCode = ERR_APPEXECFWK_HIDUMP_ERROR;
    switch (hidumpParam.hidumpFlag) {
        case HidumpFlag::GET_ABILITY: {
            errCode = GetAllAbilityInfo(result);
            break;
        }
        case HidumpFlag::GET_ABILITY_LIST: {
            errCode = GetAllAbilityNameList(result);
            break;
        }
        case HidumpFlag::GET_ABILITY_BY_NAME: {
            errCode = GetAbilityInfoByName(hidumpParam.args, result);
            break;
        }
        case HidumpFlag::GET_BUNDLE: {
            errCode = GetAllBundleInfo(result);
            break;
        }
        case HidumpFlag::GET_BUNDLE_LIST: {
            errCode = GetAllBundleNameList(result);
            break;
        }
        case HidumpFlag::GET_BUNDLE_BY_NAME: {
            errCode = GetBundleInfoByName(hidumpParam.args, result);
            break;
        }
        case HidumpFlag::GET_DEVICEID: {
            errCode = GetAllDeviced(result);
            break;
        }
        default: {
            errCode = ERR_APPEXECFWK_HIDUMP_INVALID_ARGS;
            break;
        }
    }

    return errCode;
}

ErrCode HidumpHelper::GetAllAbilityInfo(std::string &result)
{
    auto shareDataMgr = dataMgr_.lock();
    if (!shareDataMgr) {
        return ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR;
    }

    std::vector<BundleInfo> bundleInfos;
    if (!shareDataMgr->GetBundleInfos(static_cast<int32_t>(
        BundleFlag::GET_BUNDLE_WITH_ABILITIES |
        BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO |
        BundleFlag::GET_BUNDLE_WITH_HASH_VALUE),
        bundleInfos, Constants::ANY_USERID)) {
        APP_LOGE("get bundleInfos failed.");
        return ERR_APPEXECFWK_HIDUMP_ERROR;
    }

    for (auto &bundleInfo : bundleInfos) {
        for (auto &abilityInfo :  bundleInfo.abilityInfos) {
            result.append(abilityInfo.name);
            result.append(":\n");
            nlohmann::json jsonObject = abilityInfo;
            result.append(jsonObject.dump(Constants::DUMP_INDENT));
            result.append("\n");
        }
    }

    APP_LOGD("get all ability info success");
    return ERR_OK;
}

ErrCode HidumpHelper::GetAllAbilityNameList(std::string &result)
{
    auto shareDataMgr = dataMgr_.lock();
    if (!shareDataMgr) {
        return ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR;
    }

    std::vector<BundleInfo> bundleInfos;
    if (!shareDataMgr->GetBundleInfos(static_cast<int32_t>(
        BundleFlag::GET_BUNDLE_WITH_ABILITIES |
        BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO |
        BundleFlag::GET_BUNDLE_WITH_HASH_VALUE),
        bundleInfos, Constants::ANY_USERID)) {
        APP_LOGE("get bundleInfos failed.");
        return ERR_APPEXECFWK_HIDUMP_ERROR;
    }

    for (auto &bundleInfo : bundleInfos) {
        for (auto abilityInfo :  bundleInfo.abilityInfos) {
            result.append(abilityInfo.name);
            result.append("\n");
        }
    }

    APP_LOGD("get all ability list info success");
    return ERR_OK;
}

ErrCode HidumpHelper::GetAbilityInfoByName(const std::string &name, std::string &result)
{
    auto shareDataMgr = dataMgr_.lock();
    if (!shareDataMgr) {
        return ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR;
    }

    std::vector<BundleInfo> bundleInfos;
    if (!shareDataMgr->GetBundleInfos(static_cast<int32_t>(
        BundleFlag::GET_BUNDLE_WITH_ABILITIES |
        BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO |
        BundleFlag::GET_BUNDLE_WITH_HASH_VALUE),
        bundleInfos, Constants::ANY_USERID)) {
        APP_LOGE("get bundleInfos failed.");
        return ERR_APPEXECFWK_HIDUMP_ERROR;
    }

    for (auto &bundleInfo : bundleInfos) {
        for (auto abilityInfo :  bundleInfo.abilityInfos) {
            if (abilityInfo.name == name) {
                result.append(abilityInfo.name);
                result.append(":\n");
                nlohmann::json jsonObject = abilityInfo;
                result.append(jsonObject.dump(Constants::DUMP_INDENT));
                result.append("\n");
                return ERR_OK;
            }
        }
    }

    return ERR_APPEXECFWK_HIDUMP_ERROR;
}

ErrCode HidumpHelper::GetAllBundleInfo(std::string &result)
{
    auto shareDataMgr = dataMgr_.lock();
    if (!shareDataMgr) {
        return ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR;
    }

    std::vector<BundleInfo> bundleInfos;
    if (!shareDataMgr->GetBundleInfos(static_cast<int32_t>(
        BundleFlag::GET_BUNDLE_WITH_ABILITIES |
        BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO |
        BundleFlag::GET_BUNDLE_WITH_HASH_VALUE),
        bundleInfos, Constants::ANY_USERID)) {
        APP_LOGE("get bundleInfos failed.");
        return false;
    }

    for (auto &info : bundleInfos) {
        result.append(info.name);
        result.append(":\n");
        nlohmann::json jsonObject = info;
        jsonObject["hapModuleInfos"] = info.hapModuleInfos;
        result.append(jsonObject.dump(Constants::DUMP_INDENT));
        result.append("\n");
    }
    APP_LOGD("get all bundle info success");
    return ERR_OK;
}

ErrCode HidumpHelper::GetAllBundleNameList(std::string &result)
{
    auto shareDataMgr = dataMgr_.lock();
    if (!shareDataMgr) {
        return ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR;
    }

    std::vector<std::string> bundleNames;
    if (!shareDataMgr->GetBundleList(bundleNames, Constants::ANY_USERID)) {
        APP_LOGE("get bundle list failed");
        return ERR_APPEXECFWK_HIDUMP_ERROR;
    }

    for (auto &name : bundleNames) {
        result.append(name);
        result.append("\n");
    }

    return ERR_OK;
}

ErrCode HidumpHelper::GetBundleInfoByName(const std::string &name, std::string &result)
{
    APP_LOGD("hidump bundle info begin");
    auto shareDataMgr = dataMgr_.lock();
    if (!shareDataMgr) {
        return ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR;
    }

    BundleInfo bundleInfo;
    if (!shareDataMgr->GetBundleInfo(name,
        BundleFlag::GET_BUNDLE_WITH_ABILITIES |
        BundleFlag::GET_BUNDLE_WITH_REQUESTED_PERMISSION |
        BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO |
        BundleFlag::GET_BUNDLE_WITH_HASH_VALUE, bundleInfo, Constants::ANY_USERID)) {
        APP_LOGE("get bundleInfo(%{public}s) failed", name.c_str());
        return ERR_APPEXECFWK_HIDUMP_ERROR;
    }

    result.append(name);
    result.append(":\n");
    nlohmann::json jsonObject = bundleInfo;
    jsonObject["hapModuleInfos"] = bundleInfo.hapModuleInfos;
    result.append(jsonObject.dump(Constants::DUMP_INDENT));
    result.append("\n");
    APP_LOGD("get %{public}s bundle info success", name.c_str());
    return ERR_OK;
}

ErrCode HidumpHelper::GetAllDeviced(std::string &result)
{
    auto shareDataMgr = dataMgr_.lock();
    if (!shareDataMgr) {
        return ERR_APPEXECFWK_HIDUMP_SERVICE_ERROR;
    }

    std::vector<std::string> deviceIds;
    if (!shareDataMgr->QueryAllDeviceIds(deviceIds)) {
        APP_LOGE("get all deviceId failed");
        return ERR_APPEXECFWK_HIDUMP_ERROR;
    }

    for (auto deviceId : deviceIds) {
        result.append(deviceId);
        result.append("\n");
    }

    return ERR_OK;
}

void HidumpHelper::ShowHelp(std::string &result)
{
    result.append("Usage:dump  <command> [options]\n")
          .append("Description:\n")
          .append("-ability          ")
          .append("dump all ability infomation in the system\n")
          .append("-ability    [abilityName]\n")
          .append("                  dump ability list information of the specified name in the system\n")
          .append("-ability-list     ")
          .append("dump list of all ability names in the system\n")
          .append("-bundle           ")
          .append("dump all bundle infomation in the system\n")
          .append("-bundle     [bundleName]\n")
          .append("                  dump bundle list information of the specified name in the system\n")
          .append("-bundle-list      ")
          .append("dump list of all bundle names in the system\n")
          .append("-device           ")
          .append("dump the list of devices involved in the ability infomation in the system\n");
}

void HidumpHelper::ShowIllealInfomation(std::string &result)
{
    result.append(ILLEGAL_INFOMATION);
}
}  // namespace AppExecFwk
}  // namespace OHOS
