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
#include "napi_bluetooth_profile.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
thread_local std::map<ProfileId, napi_value> NapiProfile::profiles_;

void NapiProfile::DefineProfileFunctions(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getProfile", GetProfile),
        DECLARE_NAPI_FUNCTION("getProfileInst", GetProfile),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    ProfileEnumInit(env, exports);
}

void NapiProfile::SetProfile(ProfileId code, napi_value profile)
{
    profiles_[code] = profile;
}

napi_value NapiProfile::GetProfile(napi_env env, napi_callback_info info)
{
    HILOGI("enter");
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = {0};
    napi_value thisVar = nullptr;
    bool isOK = false;
    napi_value ret = nullptr;
    napi_get_boolean(env, isOK, &ret);

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        HILOGE("Requires 1 argument.");
        return ret;
    }
    int32_t profileId;
    if (!ParseInt32(env, profileId, argv[PARAM0])) {
        HILOGE("False type! Int32 required.");
        return ret;
    }

    napi_value profile = profiles_[static_cast<ProfileId>(profileId)];
    if (!profile) {
        return ret;
    }
    return profile;
}

void NapiProfile::ProfileEnumInit(napi_env env, napi_value exports)
{
    HILOGI("enter");
    napi_value sppTypeObj = SppTypeInit(env);
    napi_value playingStateObj = PlayingStateInit(env);
    napi_value profileIdObj = ProfileIdInit(env);
    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("SppType", sppTypeObj),
        DECLARE_NAPI_PROPERTY("PlayingState", playingStateObj),
        DECLARE_NAPI_PROPERTY("ProfileId", profileIdObj),
    };
    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);
    HILOGI("end");
}

napi_value NapiProfile::SppTypeInit(napi_env env)
{
    HILOGI("enter");
    napi_value sppType = nullptr;
    napi_create_object(env, &sppType);
    SetNamedPropertyByInteger(env, sppType, SppType::SPP_RFCOMM, "SPP_RFCOMM");
    return sppType;
}

napi_value NapiProfile::PlayingStateInit(napi_env env)
{
    HILOGI("enter");
    napi_value playingState = nullptr;
    napi_create_object(env, &playingState);
    SetNamedPropertyByInteger(env, playingState, PlayingState::STATE_NOT_PLAYING, "STATE_NOT_PLAYING");
    SetNamedPropertyByInteger(env, playingState, PlayingState::STATE_PLAYING, "STATE_PLAYING");
    return playingState;
}

napi_value NapiProfile::ProfileIdInit(napi_env env)
{
    HILOGI("enter");
    napi_value profileId = nullptr;
    napi_create_object(env, &profileId);
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_A2DP_SINK, "PROFILE_A2DP_SINK");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_A2DP_SOURCE, "PROFILE_A2DP_SOURCE");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_AVRCP_CT, "PROFILE_AVRCP_CT");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_AVRCP_TG, "PROFILE_AVRCP_TG");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_HANDS_FREE_AUDIO_GATEWAY,
        "PROFILE_HANDS_FREE_AUDIO_GATEWAY");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_HANDS_FREE_UNIT, "PROFILE_HANDS_FREE_UNIT");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_HID_HOST, "PROFILE_HID_HOST");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_PAN_NETWORK, "PROFILE_PAN_NETWORK");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_PBAP_CLIENT, "PROFILE_PBAP_CLIENT");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_PBAP_SERVER, "PROFILE_PBAP_SERVER");
    SetNamedPropertyByInteger(env, profileId, ProfileId::PROFILE_OPP, "PROFILE_OPP");
    return profileId;
}
} // namespace Bluetooth
} // namespace OHOS
