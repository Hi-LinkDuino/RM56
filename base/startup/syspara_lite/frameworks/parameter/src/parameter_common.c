/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include <fcntl.h>
#include <securec.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "hal_sys_param.h"
#ifdef USE_MBEDTLS
#include "mbedtls/sha256.h"
#endif
#include "ohos_errno.h"
#include "param_adaptor.h"
#include "parameter.h"

#define FILE_RO "ro."
#define OS_FULL_NAME_LEN 128
#define VERSION_ID_LEN 256
#define HASH_LENGTH 32
#define DEV_BUF_LENGTH 3
#define DEV_BUF_MAX_LENGTH 1024
#define DEV_UUID_LENGTH 65
#define OHOS_DISPLAY_VERSION_LEN 128
#define OHOS_PATCH_VERSION_LEN 64
#define OHOS_PATCH_VERSION_FILE "/patch/pversion"

static const char OHOS_OS_NAME[] = { "OpenHarmony" };
static const int OHOS_SDK_API_VERSION = 6;
static const char OHOS_SECURITY_PATCH_TAG[] = {"2021-09-01"};
static const char OHOS_RELEASE_TYPE[] = { "Beta" };

static const int MAJOR_VERSION = 1;
static const int SENIOR_VERSION = 0;
static const int FEATURE_VERSION = 1;
static const int BUILD_VERSION = 0;

static const char EMPTY_STR[] = { "" };

static boolean IsValidValue(const char *value, unsigned int len)
{
    if ((value == NULL) || !strlen(value) || (strlen(value) + 1 > len)) {
        return FALSE;
    }
    return TRUE;
}

int GetParameter(const char *key, const char *def, char *value, unsigned int len)
{
    if ((key == NULL) || (value == NULL)) {
        return EC_INVALID;
    }
    if (!CheckPermission()) {
        return EC_FAILURE;
    }
    int ret = GetSysParam(key, value, len);
    if (ret == EC_INVALID) {
        return EC_INVALID;
    }
    if ((ret < 0) && IsValidValue(def, len)) {
        if (strncpy_s(value, len, def, len - 1) != 0) {
            return EC_FAILURE;
        }
        ret = (int)strlen(def);
    }
    return ret;
}

int SetParameter(const char *key, const char *value)
{
    if ((key == NULL) || (value == NULL)) {
        return EC_INVALID;
    }
    if (!CheckPermission()) {
        return EC_FAILURE;
    }
    if (strncmp(key, FILE_RO, strlen(FILE_RO)) == 0) {
        return EC_INVALID;
    }

    return SetSysParam(key, value);
}

const char *GetDeviceType(void)
{
    return HalGetDeviceType();
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

const char *GetProductModel(void)
{
    return HalGetProductModel();
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

const char *GetBootloaderVersion(void)
{
    return HalGetBootloaderVersion();
}

const char *GetSecurityPatchTag(void)
{
    return OHOS_SECURITY_PATCH_TAG;
}

const char *GetAbiList(void)
{
    return HalGetAbiList();
}

static const char *BuildOSFullName(void)
{
    const char release[] = "Release";
    char value[OS_FULL_NAME_LEN];
    const char *releaseType = GetOsReleaseType();
    int length;
    if (strncmp(releaseType, release, sizeof(release) - 1) == 0) {
        length = sprintf_s(value, OS_FULL_NAME_LEN, "%s-%d.%d.%d.%d",
            OHOS_OS_NAME, MAJOR_VERSION, SENIOR_VERSION, FEATURE_VERSION, BUILD_VERSION);
    } else {
        length = sprintf_s(value, OS_FULL_NAME_LEN, "%s-%d.%d.%d.%d(%s)",
            OHOS_OS_NAME, MAJOR_VERSION, SENIOR_VERSION, FEATURE_VERSION, BUILD_VERSION, releaseType);
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

static const char *BuildDisplayVersion(void)
{
    int len;
    char patchValue[OHOS_PATCH_VERSION_LEN] = {0};
    char displayValue[OHOS_DISPLAY_VERSION_LEN] = {0};
    int fd = open(OHOS_PATCH_VERSION_FILE, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }
    len = read(fd, patchValue, OHOS_PATCH_VERSION_LEN);
    if (len < strlen("version=")) {
        close(fd);
        return NULL;
    }
    close(fd);
    if (patchValue[len - 1] == '\n') {
        patchValue[len - 1] = '\0';
    }
    const char *versionValue = HalGetDisplayVersion();
    const int versionLen = strlen(versionValue);
    if (versionLen > 0) {
        if (versionValue[versionLen - 1] != ')') {
            len = sprintf_s(displayValue, OHOS_DISPLAY_VERSION_LEN, "%s(%s)", versionValue,
                patchValue + strlen("version="));
        } else {
            char tempValue[versionLen];
            memset_s(tempValue, versionLen, 0, versionLen);
            if (strncpy_s(tempValue, versionLen, versionValue, versionLen - 1) != 0) {
                return NULL;
            }
            tempValue[versionLen - 1] = '\0';
            len = sprintf_s(displayValue, OHOS_DISPLAY_VERSION_LEN, "%s%s)", tempValue,
                patchValue + strlen("version="));
        }
    }
    if (len < 0) {
        return NULL;
    }
    return strdup(displayValue);
}

const char *GetDisplayVersion(void)
{
    static const char *displayVersion = NULL;
    if (displayVersion != NULL) {
        return displayVersion;
    }
    displayVersion = BuildDisplayVersion();
    if (displayVersion == NULL) {
        return HalGetDisplayVersion();
    }
    return displayVersion;
}

int GetSdkApiVersion(void)
{
    return OHOS_SDK_API_VERSION;
}

int GetFirstApiVersion(void)
{
    return HalGetFirstApiVersion();
}

const char *GetIncrementalVersion(void)
{
    return HalGetIncrementalVersion();
}

static const char *BuildVersionId(void)
{
    char value[VERSION_ID_LEN];
    int len = sprintf_s(value, VERSION_ID_LEN, "%s/%s/%s/%s/%s/%s/%s/%d/%s/%s",
        GetDeviceType(), GetManufacture(), GetBrand(), GetProductSeries(),
        GetOSFullName(), GetProductModel(), GetSoftwareModel(),
        OHOS_SDK_API_VERSION, GetIncrementalVersion(), GetBuildType());
    if (len < 0) {
        return EMPTY_STR;
    }
    const char *versionId = strdup(value);
    return versionId;
}

const char *GetVersionId(void)
{
    static const char *versionId = NULL;
    if (versionId != NULL) {
        return versionId;
    }
    versionId = BuildVersionId();
    if (versionId == NULL) {
        return EMPTY_STR;
    }
    return versionId;
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
    return BUILD_ROOTHASH;
}

const char *GetOsReleaseType(void)
{
    return OHOS_RELEASE_TYPE;
}
#ifdef USE_MBEDTLS
static int GetSha256Value(const unsigned char *input, char *udid, int udidSize)
{
    if (input == NULL) {
        return EC_FAILURE;
    }
    char buf[DEV_BUF_LENGTH] = { 0 };
    unsigned char hash[HASH_LENGTH] = { 0 };

    mbedtls_sha256_context context;
    mbedtls_sha256_init(&context);
    mbedtls_sha256_starts_ret(&context, 0);
    mbedtls_sha256_update_ret(&context, input, strlen((const char *)input));
    mbedtls_sha256_finish_ret(&context, hash);

    for (size_t i = 0; i < HASH_LENGTH; i++) {
        unsigned char value = hash[i];
        memset_s(buf, DEV_BUF_LENGTH, 0, DEV_BUF_LENGTH);
        sprintf_s(buf, sizeof(buf), "%02X", value);
        if (strcat_s(udid, udidSize, buf) != 0) {
            return EC_FAILURE;
        }
    }
    return EC_SUCCESS;
}

int GetDevUdid(char *udid, int size)
{
    if (size < DEV_UUID_LENGTH) {
        return EC_FAILURE;
    }

    const char *manufacture = GetManufacture();
    const char *model = GetHardwareModel();
    const char *sn = GetSerial();
    if (manufacture == NULL || model == NULL || sn == NULL) {
        return EC_FAILURE;
    }
    int tmpSize = strlen(manufacture) + strlen(model) + strlen(sn) + 1;
    if (tmpSize <= 0 || tmpSize > DEV_BUF_MAX_LENGTH) {
        return EC_FAILURE;
    }
    char *tmp = malloc(tmpSize);
    if (tmp == NULL) {
        return EC_FAILURE;
    }

    memset_s(tmp, tmpSize, 0, tmpSize);
    if ((strcat_s(tmp, tmpSize, manufacture) != 0) ||
        (strcat_s(tmp, tmpSize, model) != 0) ||
        (strcat_s(tmp, tmpSize, sn) != 0)) {
        free(tmp);
        return EC_FAILURE;
    }

    int ret = GetSha256Value((const unsigned char *)tmp, udid, size);
    free(tmp);
    return ret;
}
#else
int GetDevUdid(char *udid, int size)
{
    return 0;
}
#endif
