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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_HIDUMPE_HELPER_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_HIDUMPE_HELPER_H

#include <string>
#include <vector>

#include "bundle_data_mgr.h"

namespace OHOS {
namespace AppExecFwk {
enum class HidumpFlag {
    UNKNOW = 0,
    GET_HELP,
    GET_ABILITY,
    GET_ABILITY_LIST,
    GET_ABILITY_BY_NAME,
    GET_BUNDLE,
    GET_BUNDLE_LIST,
    GET_BUNDLE_BY_NAME,
    GET_DEVICEID,
};

struct HidumpParam {
    HidumpFlag hidumpFlag = HidumpFlag::UNKNOW;
    std::string args;
};

class HidumpHelper {
public:
    explicit HidumpHelper(const std::weak_ptr<BundleDataMgr> &dataMgr);
    ~HidumpHelper() = default;
    /**
     * @brief Process hidump.
     * @param args Indicates the args.
     * @param result Indicates the result.
     * @return Returns whether the interface is called successfully.
     */
    bool Dump(const std::vector<std::string>& args, std::string &result);

private:
    ErrCode ProcessOneParam(const std::string& args, std::string &result);
    ErrCode ProcessTwoParam(const std::string& firstParam,
        const std::string& secondParam, std::string &result);
    void ShowHelp(std::string &result);
    void ShowIllealInfomation(std::string &result);
    ErrCode ProcessDump(const HidumpParam& hidumpParam, std::string &result);

    ErrCode GetAllAbilityInfo(std::string &result);
    ErrCode GetAllAbilityNameList(std::string &result);
    ErrCode GetAbilityInfoByName(const std::string &name, std::string &result);
    ErrCode GetAllBundleInfo(std::string &result);
    ErrCode GetAllBundleNameList(std::string &result);
    ErrCode GetBundleInfoByName(const std::string &name, std::string &result);
    ErrCode GetAllDeviced(std::string &result);

    std::weak_ptr<BundleDataMgr> dataMgr_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_HIDUMPE_HELPER_H
