/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "das_version_util.h"
#include "common_defs.h"
#include "hc_log.h"
#include "hc_types.h"
#include "json_utils.h"
#include "string_util.h"

#define BIND_PRIORITY_LEN 5
#define AUTH_PRIORITY_LEN 5

typedef struct PriorityMapT {
    uint32_t alg;
    ProtocolType type;
} PriorityMap;

VersionStruct g_defaultVersion = { 1, 0, 0 };
PriorityMap g_bindPriorityList[BIND_PRIORITY_LEN] = {
    { EC_PAKE_V2, PAKE_V2 },
    { DL_PAKE_V2, PAKE_V2 },
    { EC_PAKE_V1, PAKE_V1 },
    { DL_PAKE_V1, PAKE_V1 },
    { ISO_ALG, ISO }
};
PriorityMap g_authPriorityList[AUTH_PRIORITY_LEN] = {
    { PSK_SPEKE | EC_PAKE_V2, PAKE_V2 },
    { PSK_SPEKE | EC_PAKE_V1, PAKE_V1 },
    { ISO_ALG, ISO }
};

int32_t GetVersionFromJson(const CJson* jsonObj, VersionStruct *minVer, VersionStruct *maxVer)
{
    CHECK_PTR_RETURN_ERROR_CODE(jsonObj, "jsonObj");
    CHECK_PTR_RETURN_ERROR_CODE(minVer, "minVer");
    CHECK_PTR_RETURN_ERROR_CODE(maxVer, "maxVer");

    const char *minStr = GetStringFromJson(jsonObj, FIELD_MIN_VERSION);
    CHECK_PTR_RETURN_ERROR_CODE(minStr, "minStr");
    const char *maxStr = GetStringFromJson(jsonObj, FIELD_CURRENT_VERSION);
    CHECK_PTR_RETURN_ERROR_CODE(maxStr, "maxStr");

    int32_t minRet = StringToVersion(minStr, minVer);
    int32_t maxRet = StringToVersion(maxStr, maxVer);
    if (minRet != HC_SUCCESS || maxRet != HC_SUCCESS) {
        LOGE("Convert version string to struct failed.");
        return HC_ERROR;
    }
    return HC_SUCCESS;
}

int32_t AddVersionToJson(CJson *jsonObj, const VersionStruct *minVer, const VersionStruct *maxVer)
{
    CHECK_PTR_RETURN_ERROR_CODE(jsonObj, "jsonObj");
    CHECK_PTR_RETURN_ERROR_CODE(minVer, "minVer");
    CHECK_PTR_RETURN_ERROR_CODE(maxVer, "maxVer");

    char minStr[TMP_VERSION_STR_LEN] = { 0 };
    int32_t minRet = VersionToString(minVer, minStr, TMP_VERSION_STR_LEN);
    char maxStr[TMP_VERSION_STR_LEN] = { 0 };
    int32_t maxRet = VersionToString(maxVer, maxStr, TMP_VERSION_STR_LEN);
    if (minRet != HC_SUCCESS || maxRet != HC_SUCCESS) {
        return HC_ERROR;
    }
    CJson* version = CreateJson();
    if (version == NULL) {
        LOGE("CreateJson for version failed.");
        return HC_ERR_JSON_CREATE;
    }
    if (AddStringToJson(version, FIELD_MIN_VERSION, minStr) != HC_SUCCESS) {
        LOGE("Add min version to json failed.");
        FreeJson(version);
        return HC_ERR_JSON_ADD;
    }
    if (AddStringToJson(version, FIELD_CURRENT_VERSION, maxStr) != HC_SUCCESS) {
        LOGE("Add max version to json failed.");
        FreeJson(version);
        return HC_ERR_JSON_ADD;
    }
    if (AddObjToJson(jsonObj, FIELD_VERSION, version) != HC_SUCCESS) {
        LOGE("Add version object to json failed.");
        FreeJson(version);
        return HC_ERR_JSON_ADD;
    }
    FreeJson(version);
    return HC_SUCCESS;
}

bool IsVersionEqual(VersionStruct *src, VersionStruct *des)
{
    if ((src->first == des->first) && (src->second == des->second) && (src->third == des->third)) {
        return true;
    }
    return false;
}

int32_t NegotiateVersion(VersionStruct *minVersionPeer, VersionStruct *curVersionPeer,
    VersionStruct *curVersionSelf)
{
    (void)minVersionPeer;
    if (IsVersionEqual(curVersionPeer, &g_defaultVersion)) {
        curVersionSelf->first = g_defaultVersion.first;
        curVersionSelf->second = g_defaultVersion.second;
        curVersionSelf->third = g_defaultVersion.third;
        return HC_SUCCESS;
    }
    curVersionSelf->third = curVersionSelf->third & curVersionPeer->third;
    if (curVersionSelf->third == 0) {
        LOGE("Unsupported version!");
        return HC_ERR_UNSUPPORTED_VERSION;
    }
    return HC_SUCCESS;
}

static ProtocolType GetBindPrototolType(VersionStruct *curVersion)
{
    if (IsVersionEqual(curVersion, &g_defaultVersion)) {
        return PAKE_V1;
    }
    for (int i = 0; i < BIND_PRIORITY_LEN; i++) {
        if ((curVersion->third & g_bindPriorityList[i].alg) == g_bindPriorityList[i].alg) {
            return g_bindPriorityList[i].type;
        }
    }
    return PROTOCOL_TYPE_NONE;
}

static ProtocolType GetAuthPrototolType(VersionStruct *curVersion)
{
    if (IsVersionEqual(curVersion, &g_defaultVersion)) {
        LOGE("Not support STS.");
        return PROTOCOL_TYPE_NONE;
    }
    for (int i = 0; i < AUTH_PRIORITY_LEN; i++) {
        if ((curVersion->third & g_authPriorityList[i].alg) == g_authPriorityList[i].alg) {
            return g_authPriorityList[i].type;
        }
    }
    return PROTOCOL_TYPE_NONE;
}

ProtocolType GetPrototolType(VersionStruct *curVersion, OperationCode opCode)
{
    switch (opCode) {
        case OP_BIND:
        case AUTH_KEY_AGREEMENT:
            return GetBindPrototolType(curVersion);
        case AUTHENTICATE:
        case OP_UNBIND:
            return GetAuthPrototolType(curVersion);
        default:
            LOGE("Unsupported opCode: %d.", opCode);
    }
    return PROTOCOL_TYPE_NONE;
}

PakeAlgType GetSupportedPakeAlg(VersionStruct *curVersion, ProtocolType protocolType)
{
    PakeAlgType pakeAlgType = PAKE_ALG_NONE;
    if (protocolType == PAKE_V2) {
        pakeAlgType = ((curVersion->third & EC_PAKE_V2) >> ALG_OFFSET_FOR_PAKE_V2) |
            ((curVersion->third & DL_PAKE_V2) >> ALG_OFFSET_FOR_PAKE_V2);
    } else if (protocolType == PAKE_V1) {
        pakeAlgType = ((curVersion->third & EC_PAKE_V1) >> ALG_OFFSET_FOR_PAKE_V1) |
            ((curVersion->third & DL_PAKE_V1) >> ALG_OFFSET_FOR_PAKE_V1);
    } else {
        LOGE("Invalid protocolType: %d.", protocolType);
    }
    return pakeAlgType;
}

bool IsSupportedPsk(VersionStruct *curVersion)
{
    return ((curVersion->third & PSK_SPEKE) != 0);
}