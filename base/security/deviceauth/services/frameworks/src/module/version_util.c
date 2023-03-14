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

#include "version_util.h"
#include "hc_log.h"
#include "hc_types.h"
#include "protocol_common.h"
#include "string_util.h"

static const char *GetSlice(char *str, char delim, int *nextIdx)
{
    uint32_t len = HcStrlen(str);
    for (uint32_t i = 0; i < len; i++) {
        if (str[i] == delim) {
            *nextIdx = *nextIdx + i + 1;
            str[i] = '\0';
            return str;
        }
    }
    return str;
}

int32_t StringToVersion(const char* verStr, VersionStruct* version)
{
    CHECK_PTR_RETURN_ERROR_CODE(version, "version");
    CHECK_PTR_RETURN_ERROR_CODE(verStr, "verStr");

    const char *subVer = NULL;
    int nextIdx = 0;

    uint32_t len = HcStrlen(verStr);
    char *verStrTmp = (char *)HcMalloc(len + 1, 0);
    if (verStrTmp == NULL) {
        LOGE("Malloc for verStrTmp failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (memcpy_s(verStrTmp, len + 1, verStr, len) != EOK) {
        LOGE("Memcpy for verStrTmp failed.");
        HcFree(verStrTmp);
        return HC_ERR_MEMORY_COPY;
    }

    subVer = GetSlice(verStrTmp, '.', &nextIdx);
    if (subVer == NULL) {
        goto CLEAN_UP;
    }
    version->first = (uint32_t)strtoul(subVer, NULL, DEC);

    subVer = GetSlice(verStrTmp + nextIdx, '.', &nextIdx);
    if (subVer == NULL) {
        goto CLEAN_UP;
    }
    version->second = (uint32_t)strtoul(subVer, NULL, DEC);

    subVer = GetSlice(verStrTmp + nextIdx, '.', &nextIdx);
    if (subVer == NULL) {
        goto CLEAN_UP;
    }
    version->third = (uint32_t)strtoul(subVer, NULL, DEC);

    HcFree(verStrTmp);
    return HC_SUCCESS;
CLEAN_UP:
    LOGE("GetSlice failed.");
    HcFree(verStrTmp);
    return HC_ERROR;
}

int32_t VersionToString(const VersionStruct *version, char *verStr, uint32_t len)
{
    CHECK_PTR_RETURN_ERROR_CODE(version, "version");
    CHECK_PTR_RETURN_ERROR_CODE(verStr, "verStr");

    char tmpStr[TMP_VERSION_STR_LEN] = { 0 };
    if (sprintf_s(tmpStr, TMP_VERSION_STR_LEN, "%d.%d.%d", version->first, version->second, version->third) <= 0) {
        LOGE("Convert version struct to string failed.");
        return HC_ERR_CONVERT_FAILED;
    }
    uint32_t tmpStrLen = HcStrlen(tmpStr);
    if (len < tmpStrLen + 1) {
        LOGE("The length of verStr is too short, len: %u.", len);
        return HC_ERR_INVALID_LEN;
    }

    if (memcpy_s(verStr, len, tmpStr, tmpStrLen + 1) != 0) {
        LOGE("Memcpy for verStr failed.");
        return HC_ERR_MEMORY_COPY;
    }

    return HC_SUCCESS;
}

int32_t AddSingleVersionToJson(CJson *jsonObj, const VersionStruct *version)
{
    CHECK_PTR_RETURN_ERROR_CODE(jsonObj, "jsonObj");
    CHECK_PTR_RETURN_ERROR_CODE(version, "version");

    char versionStr[TMP_VERSION_STR_LEN] = { 0 };
    int32_t ret = VersionToString(version, versionStr, TMP_VERSION_STR_LEN);
    if (ret != HC_SUCCESS) {
        LOGE("VersionToString failed, res: %x.", ret);
        return ret;
    }

    CJson *sendToPeer = GetObjFromJson(jsonObj, FIELD_SEND_TO_PEER);
    if (sendToPeer == NULL) {
        LOGD("There is not sendToPeer in json.");
        return HC_SUCCESS;
    }
    if (AddStringToJson(sendToPeer, FIELD_GROUP_AND_MODULE_VERSION, versionStr) != HC_SUCCESS) {
        LOGE("Add group and module version to sendToPeer failed.");
        return HC_ERR_JSON_ADD;
    }
    return HC_SUCCESS;
}

int32_t GetSingleVersionFromJson(const CJson* jsonObj, VersionStruct *version)
{
    CHECK_PTR_RETURN_ERROR_CODE(jsonObj, "jsonObj");
    CHECK_PTR_RETURN_ERROR_CODE(version, "version");

    const char *versionStr = GetStringFromJson(jsonObj, FIELD_GROUP_AND_MODULE_VERSION);
    if (versionStr == NULL) {
        LOGE("Get group and module version from json failed.");
        return HC_ERR_JSON_GET;
    }

    int32_t ret = StringToVersion(versionStr, version);
    if (ret != HC_SUCCESS) {
        LOGE("StringToVersion failed, res: %x.", ret);
        return ret;
    }
    return HC_SUCCESS;
}

void InitGroupAndModuleVersion(VersionStruct *version)
{
    if (version == NULL) {
        LOGE("Version is null.");
        return;
    }
    version->first = MAJOR_VERSION_NO;
    version->second = 0;
    version->third = 0;
}