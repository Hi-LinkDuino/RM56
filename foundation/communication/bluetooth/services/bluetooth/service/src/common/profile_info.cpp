/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "profile_info.h"

#include <algorithm>

#include "adapter_config.h"
#include "interface_profile.h"

namespace OHOS {
namespace bluetooth {
const std::vector<ProfileInfo> SupportProfilesInfo::SUPPORT_FILES = {
    ProfileInfo(PROFILE_NAME_GATT_CLIENT, PROFILE_ID_GATT_CLIENT, BLUETOOTH_UUID_GATT),
    ProfileInfo(PROFILE_NAME_GATT_SERVER, PROFILE_ID_GATT_SERVER, BLUETOOTH_UUID_GATT),
    ProfileInfo(PROFILE_NAME_A2DP_SRC, PROFILE_ID_A2DP_SRC, BLUETOOTH_UUID_A2DP_SRC),
    ProfileInfo(PROFILE_NAME_A2DP_SINK, PROFILE_ID_A2DP_SINK, BLUETOOTH_UUID_A2DP_SINK),
    ProfileInfo(PROFILE_NAME_AVRCP_CT, PROFILE_ID_AVRCP_CT, BLUETOOTH_UUID_AVRCP_CT),
    ProfileInfo(PROFILE_NAME_AVRCP_TG, PROFILE_ID_AVRCP_TG, BLUETOOTH_UUID_AVRCP_TG),
    ProfileInfo(PROFILE_NAME_HFP_AG, PROFILE_ID_HFP_AG, BLUETOOTH_UUID_HFP_AG),
    ProfileInfo(PROFILE_NAME_HFP_HF, PROFILE_ID_HFP_HF, BLUETOOTH_UUID_HFP_HF),
    ProfileInfo(PROFILE_NAME_MAP_MCE, PROFILE_ID_MAP_MCE, ""),
    ProfileInfo(PROFILE_NAME_MAP_MSE, PROFILE_ID_MAP_MSE, ""),
    ProfileInfo(PROFILE_NAME_PBAP_PCE, PROFILE_ID_PBAP_PCE, BLUETOOTH_UUID_PBAP_PCE),
    ProfileInfo(PROFILE_NAME_PBAP_PSE, PROFILE_ID_PBAP_PSE, BLUETOOTH_UUID_PBAP_PSE),
    ProfileInfo(PROFILE_NAME_SPP, PROFILE_ID_SPP, BLUETOOTH_UUID_SPP),
    ProfileInfo(PROFILE_NAME_DI, PROFILE_ID_DI, BLUETOOTH_UUID_PNP),
    ProfileInfo(PROFILE_NAME_HID_HOST, PROFILE_ID_HID_HOST, BLUETOOTH_UUID_HID_HOST),
    ProfileInfo(PROFILE_NAME_PAN, PROFILE_ID_PAN, BLUETOOTH_UUID_PAN),
    ProfileInfo(PROFILE_NAME_OPP, PROFILE_ID_OPP, BLUETOOTH_UUID_OPP),
};

const std::vector<ProfileInfo> &SupportProfilesInfo::GetSupportProfiles()
{
    return SUPPORT_FILES;
}

std::string SupportProfilesInfo::IdToName(uint32_t id)
{
    auto it = std::find_if(
        SUPPORT_FILES.begin(), SUPPORT_FILES.end(), [id](ProfileInfo pInfo) -> bool { return id == pInfo.id_; });
    if (it != SUPPORT_FILES.end()) {
        return it->name_;
    } else {
        return "";
    }
}

const std::vector<ProfileInfo> SupportProfilesInfo::GetConfigSupportProfiles(BTTransport transport)
{
    std::vector<ProfileInfo> retProfiles;
    retProfiles.clear();
    std::string section = (transport == BTTransport::ADAPTER_BREDR) ? SECTION_CLASSIC_ADAPTER : SECTION_BLE_ADAPTER;

    for (auto &sp : SUPPORT_FILES) {
        bool value = false;
        AdapterConfig::GetInstance()->GetValue(section, sp.name_, value);
        if (value) {
            retProfiles.push_back(sp);
        }
    }
    return retProfiles;
}
}  // namespace bluetooth
}  // namespace OHOS