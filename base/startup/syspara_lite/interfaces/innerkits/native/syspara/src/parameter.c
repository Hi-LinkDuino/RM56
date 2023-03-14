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

#include "parameter.h"

#include <securec.h>
#include <string.h>

#include "parameter_hal.h"
#include "sysparam_errno.h"
#include "sysversion.h"

#define OS_FULL_NAME_LEN 128
#define VERSION_ID_MAX_LEN 256

static const int DEV_UUID_LENGTH = 65;

static const char EMPTY_STR[] = { "" };

int GetParameter(const char *key, const char *def, char *value, unsigned int len)
{
    if ((key == NULL) || (value == NULL)) {
        return EC_INVALID;
    }
    int ret = HalGetParameter(key, def, value, len);
    return (ret < 0) ? ret : strlen(value);
}

int SetParameter(const char *key, const char *value)
{
    if ((key == NULL) || (value == NULL)) {
        return EC_INVALID;
    }
    return HalSetParameter(key, value);
}

int WaitParameter(const char *key, const char *value, int timeout)
{
    if ((key == NULL) || (value == NULL)) {
        return EC_INVALID;
    }
    return HalWaitParameter(key, value, timeout);
}

const char *GetDeviceType(void)
{
    return HalGetDeviceType();
}

const char *GetProductModel(void)
{
    return HalGetProductModel();
}

const char *GetManufacture(void)
{
    return HalGetManufacture();
}

const char *GetBrand(void)
{
    return HalGetBrand();
}

const char *GetMarketName(void)
{
    return HalGetMarketName();
}

const char *GetProductSeries(void)
{
    return HalGetProductSeries();
}

const char *GetSoftwareModel(void)
{
    return HalGetSoftwareModel();
}

const char *GetHardwareModel(void)
{
    return HalGetHardwareModel();
}

const char *GetHardwareProfile(void)
{
    return HalGetHardwareProfile();
}

const char *GetSerial(void)
{
    return HalGetSerial();
}

const char *GetSecurityPatchTag(void)
{
    return HalGetSecurityPatchTag();
}

const char *GetAbiList(void)
{
    return HalGetAbiList();
}

const char *GetBootloaderVersion(void)
{
    return HalGetBootloaderVersion();
}

static const char *GetOSName(void)
{
    return HalGetOSName();
}

static const char *BuildOSFullName(void)
{
    const char release[] = "Release";
    char value[OS_FULL_NAME_LEN];
    const char *releaseType = GetOsReleaseType();
    int length;
    if (strncmp(releaseType, release, sizeof(release) - 1) == 0) {
        length = sprintf_s(value, OS_FULL_NAME_LEN, "%s-%d.%d.%d.%d",
            GetOSName(), GetMajorVersion(), GetSeniorVersion(), GetFeatureVersion(), GetBuildVersion());
    } else {
        length = sprintf_s(value, OS_FULL_NAME_LEN, "%s-%d.%d.%d.%d(%s)",
            GetOSName(), GetMajorVersion(), GetSeniorVersion(), GetFeatureVersion(), GetBuildVersion(), releaseType);
    }
    if (length < 0) {
        return EMPTY_STR;
    }
    const char *osFullName = strdup(value);
    return osFullName;
}

const char *GetOSFullName(void)
{
    static const char *osFullName = NULL;
    if (osFullName != NULL) {
        return osFullName;
    }
    osFullName = BuildOSFullName();
    if (osFullName == NULL) {
        return EMPTY_STR;
    }
    return osFullName;
}

int GetSdkApiVersion(void)
{
    return atoi(HalGetSdkApiVersion());
}

int GetFirstApiVersion(void)
{
    return HalGetFirstApiVersion();
}

const char *GetDisplayVersion(void)
{
    return HalGetDisplayVersion();
}

const char *GetIncrementalVersion(void)
{
    return HalGetIncrementalVersion();
}
static int GetSdkApiLevel(void)
{
    return atoi(HalGetSdkApiLevel());
}
static const char *BuildVersionId(void)
{
    char value[VERSION_ID_MAX_LEN];
    int len = sprintf_s(value, VERSION_ID_MAX_LEN, "%s/%s/%s/%s/%s/%s/%s/%d/%s/%s",
        GetDeviceType(), GetManufacture(), GetBrand(), GetProductSeries(),
        GetOSFullName(), GetProductModel(), GetSoftwareModel(),
        GetSdkApiLevel(), GetIncrementalVersion(), GetBuildType());
    if (len < 0) {
        return EMPTY_STR;
    }
    const char *versionId = strdup(value);
    return versionId;
}

const char *GetVersionId(void)
{
    static const char *ohosVersionId = NULL;
    if (ohosVersionId != NULL) {
        return ohosVersionId;
    }
    ohosVersionId = BuildVersionId();
    if (ohosVersionId == NULL) {
        return EMPTY_STR;
    }
    return ohosVersionId;
}

const char *GetBuildType(void)
{
    return HalGetBuildType();
}

const char *GetBuildUser(void)
{
    return HalGetBuildUser();
}

const char *GetBuildHost(void)
{
    return HalGetBuildHost();
}

const char *GetBuildTime(void)
{
    return HalGetBuildTime();
}

const char *GetBuildRootHash(void)
{
    return HalGetBuildRootHash();
}

const char *GetOsReleaseType(void)
{
    return HalGetOsReleaseType();
}

int GetDevUdid(char *udid, int size)
{
    if (size < DEV_UUID_LENGTH) {
        return EC_INVALID;
    }
    return HalGetDevUdid(udid, size);
}

unsigned int FindParameter(const char *name)
{
    if (name == NULL) {
        return EC_INVALID;
    }
    return HalFindParameter(name);
}

unsigned int GetParameterCommitId(unsigned int handle)
{
    return HalGetParameterCommitId(handle);
}

int GetParameterName(unsigned int handle, char *name, unsigned int len)
{
    if (name == NULL) {
        return EC_INVALID;
    }
    int ret = HalGetParameterName(handle, name, len);
    return (ret != 0) ? EC_FAILURE : strlen(name);
}

int GetParameterValue(unsigned int handle, char *value, unsigned int len)
{
    if (value == NULL) {
        return EC_INVALID;
    }
    int ret = HalGetParameterValue(handle, value, len);
    return (ret != 0) ? EC_FAILURE : strlen(value);
}
