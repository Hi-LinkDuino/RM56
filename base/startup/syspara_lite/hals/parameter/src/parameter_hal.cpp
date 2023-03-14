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

#include "parameter_hal.h"

#include <cstring>
#include <fstream>
#include <openssl/sha.h>
#include <securec.h>

#include "parameters.h"
#include "sysparam_errno.h"
#include "string_ex.h"
#include "sys_param.h"

static const char *g_emptyStr = "";

static const char DEF_OHOS_SERIAL[] = {"1234567890"};
#ifdef USE_MTK_EMMC
static const char SN_FILE[] = {"/proc/bootdevice/cid"};
#else
static const char SN_FILE[] = {"/sys/block/mmcblk0/device/cid"};
#endif
static const int DEV_BUF_LENGTH = 3;
static const int DEV_BUF_MAX_LENGTH = 1024;
static const int DEV_UUID_LENGTH = 65;

static bool IsValidValue(const char *value, unsigned int len)
{
    if (value == nullptr) {
        return false;
    }
    const std::string strValue(value);
    if (strValue.length() + 1 > len) {
        return false;
    }
    return true;
}

int HalGetParameter(const char *key, const char *def, char *value, unsigned int len)
{
    if ((key == nullptr) || (value == nullptr)) {
        return EC_INVALID;
    }
    const std::string strKey(key);
    std::string res = OHOS::system::GetParameter(strKey, "");
    if (res == "") {
        if (!IsValidValue(def, len)) {
            return EC_INVALID;
        }
        if (sprintf_s(value, len, "%s", def) < 0) {
            return EC_FAILURE;
        }
        return EC_SUCCESS;
    }

    const char *result = res.c_str();
    if (!IsValidValue(result, len)) {
        return EC_INVALID;
    }
    if (sprintf_s(value, len, "%s", result) <= 0) {
        return EC_FAILURE;
    }

    return EC_SUCCESS;
}

int HalGetIntParameter(const char *key, int def)
{
    const std::string strKey(key);
    return OHOS::system::GetIntParameter(strKey, def);
}

int HalSetParameter(const char *key, const char *value)
{
    if ((key == nullptr) || (value == nullptr)) {
        return EC_INVALID;
    }
    const std::string strKey(key);
    const std::string strVal(value);
    bool ret = OHOS::system::SetParameter(strKey, strVal);
    return ret ? EC_SUCCESS : EC_FAILURE;
}

const char *GetProperty(const std::string &key, const char **paramHolder)
{
    if (paramHolder == nullptr) {
        return nullptr;
    }
    if (*paramHolder != nullptr) {
        return *paramHolder;
    }
    std::string result = OHOS::system::GetParameter(key, "");
    const char *res = strdup(result.c_str());
    if (res == nullptr) {
        return g_emptyStr;
    }
    *paramHolder = res;
    return *paramHolder;
}

const char *HalGetDeviceType()
{
    static const char *productType = nullptr;
    return GetProperty("const.build.characteristics", &productType);
}

const char *HalGetProductModel()
{
    static const char *productModel = nullptr;
    return GetProperty("const.product.model", &productModel);
}

const char *HalGetManufacture()
{
    static const char *productManufacture = nullptr;
    return GetProperty("const.product.manufacturer", &productManufacture);
}

const char *HalGetBrand()
{
    static const char *productBrand = nullptr;
    return GetProperty("const.product.brand", &productBrand);
}

const char *HalGetMarketName()
{
    static const char *marketName = nullptr;
    return GetProperty("const.product.name", &marketName);
}

const char *HalGetProductSeries()
{
    static const char *productSeries = nullptr;
    return GetProperty("const.build.product", &productSeries);
}

const char *HalGetSoftwareModel()
{
    static const char *softwareModel = nullptr;
    return GetProperty("const.software.model", &softwareModel);
}

const char *HalGetHardwareModel()
{
    static const char *hardwareModel = nullptr;
    return GetProperty("const.product.hardwareversion", &hardwareModel);
}

const char *HalGetHardwareProfile()
{
    static const char *hardwareProfile = nullptr;
    return GetProperty("const.product.hardwareprofile", &hardwareProfile);
}

const char *HalGetSerial()
{
    static const char* ohos_serial = nullptr;
    if (ohos_serial != nullptr) {
        return ohos_serial;
    }
    std::ifstream infile;
    infile.open(SN_FILE);
    std::string value;
    infile >> value;
    if (value.empty()) {
        ohos_serial = DEF_OHOS_SERIAL;
    } else {
        std::string sn = OHOS::ReplaceStr(value, ":", "");
        const char* res = strdup(sn.c_str());
        if (res == nullptr) {
            return DEF_OHOS_SERIAL;
        }
        ohos_serial = res;
    }
    return ohos_serial;
}

const char *HalGetAbiList()
{
    static const char *productAbiList = nullptr;
    return GetProperty("const.product.cpu.abilist", &productAbiList);
}

const char *HalGetBootloaderVersion()
{
    static const char *productBootloader = nullptr;
    return GetProperty("const.product.bootloader.version", &productBootloader);
}

const char *HalGetSecurityPatchTag()
{
    static const char *securityPatchTag = nullptr;
    return GetProperty("const.ohos.version.security_patch", &securityPatchTag);
}

int HalGetFirstApiVersion()
{
    static const char *firstApiVersion = nullptr;
    return atoi(GetProperty("const.product.firstapiversion", &firstApiVersion));
}

const char *HalGetDisplayVersion()
{
    static const char *displayVersion = nullptr;
    return GetProperty("const.product.software.version", &displayVersion);
}

const char *HalGetIncrementalVersion()
{
    static const char *incrementalVersion = nullptr;
    return GetProperty("const.product.incremental.version", &incrementalVersion);
}

const char *HalGetOsReleaseType()
{
    static const char *osReleaseType = nullptr;
    return GetProperty("const.ohos.releasetype", &osReleaseType);
}

const char *HalGetSdkApiVersion()
{
    static const char *sdkApiVersion = nullptr;
    return GetProperty("const.ohos.apiversion", &sdkApiVersion);
}
const char *HalGetBuildRootHash()
{
    static const char *buildRootHash = nullptr;
    return GetProperty("const.ohos.buildroothash", &buildRootHash);
}

const char *HalGetSdkApiLevel()
{
    static const char *sdkApiLevel = nullptr;
    return GetProperty("const.ohos.sdkapilevel", &sdkApiLevel);
}

const char *HalGetOSName()
{
    static const char *osName = nullptr;
    return GetProperty("const.ohos.name", &osName);
}

const char *HalGetBuildType()
{
    static const char *buildType = nullptr;
    return GetProperty("const.product.build.type", &buildType);
}

const char *HalGetBuildUser()
{
    static const char *buildUser = nullptr;
    return GetProperty("const.product.build.user", &buildUser);
}

const char *HalGetBuildHost()
{
    static const char *buildHost = nullptr;
    return GetProperty("const.product.build.host", &buildHost);
}

const char *HalGetBuildTime()
{
    static const char *buildTime = nullptr;
    return GetProperty("const.product.build.date", &buildTime);
}

int HalWaitParameter(const char *key, const char *value, int timeout)
{
    if ((key == nullptr) || (value == nullptr)) {
        return EC_INVALID;
    }
    return OHOS::system::WaitParameter(key, value, timeout);
}

unsigned int HalFindParameter(const char *key)
{
    if (key == nullptr) {
        return EC_INVALID;
    }

    unsigned int handle = 0;
    int ret = SystemFindParameter(key, &handle);
    if (ret != 0) {
        return static_cast<unsigned int>(-1);
    }
    return handle;
}

unsigned int HalGetParameterCommitId(unsigned int handle)
{
    return OHOS::system::GetParameterCommitId(handle);
}

int HalGetParameterName(unsigned int handle, char *name, unsigned int len)
{
    if (name == nullptr) {
        return EC_INVALID;
    }
    std::string data = OHOS::system::GetParameterName(handle);
    if (data.empty()) {
        return EC_INVALID;
    }
    return strcpy_s(name, len, data.c_str());
}

int HalGetParameterValue(unsigned int handle, char *value, unsigned int len)
{
    if (value == nullptr) {
        return EC_INVALID;
    }
    std::string data = OHOS::system::GetParameterValue(handle);
    if (data.empty()) {
        return EC_INVALID;
    }
    return strcpy_s(value, len, data.c_str());
}

static int HalGetSha256Value(const char *input, char *udid, int udidSize)
{
    if (input == nullptr || udid == nullptr) {
        return EC_FAILURE;
    }
    char buf[DEV_BUF_LENGTH] = { 0 };
    unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };
    SHA256_CTX sha256;
    if ((SHA256_Init(&sha256) == 0) || (SHA256_Update(&sha256, input, strlen(input)) == 0) ||
        (SHA256_Final(hash, &sha256) == 0)) {
        return EC_FAILURE;
    }

    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        unsigned char value = hash[i];
        (void)memset_s(buf, DEV_BUF_LENGTH, 0, DEV_BUF_LENGTH);
        (void)sprintf_s(buf, sizeof(buf), "%02X", value);
        if (strcat_s(udid, udidSize, buf) != 0) {
            return EC_FAILURE;
        }
    }
    return EC_SUCCESS;
}

int HalGetDevUdid(char *udid, int size)
{
    if (size < DEV_UUID_LENGTH) {
        return EC_INVALID;
    }

    const char *manufacture = HalGetManufacture();
    const char *model = HalGetHardwareModel();
    const char *sn = HalGetSerial();
    if (manufacture == nullptr || model == nullptr || sn == nullptr) {
        return EC_INVALID;
    }
    int tmpSize = strlen(manufacture) + strlen(model) + strlen(sn) + 1;
    if (tmpSize <= 0 || tmpSize > DEV_BUF_MAX_LENGTH) {
        return EC_INVALID;
    }
    char *tmp = (char *)malloc(tmpSize);
    if (tmp == nullptr) {
        return EC_SYSTEM_ERR;
    }

    (void)memset_s(tmp, tmpSize, 0, tmpSize);
    if ((strcat_s(tmp, tmpSize, manufacture) != 0) || (strcat_s(tmp, tmpSize, model) != 0) ||
        (strcat_s(tmp, tmpSize, sn) != 0)) {
        free(tmp);
        return EC_SYSTEM_ERR;
    }

    int ret = HalGetSha256Value(tmp, udid, size);
    free(tmp);
    return ret;
}
