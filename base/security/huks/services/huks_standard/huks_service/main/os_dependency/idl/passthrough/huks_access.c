/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include "huks_access.h"

#include "huks_core_hal.h"

#include "hks_log.h"
#include "hks_mem.h"

static struct HuksHdi *g_hksHalDevicePtr = NULL;

#ifndef _CUT_AUTHENTICATE_
int32_t HuksAccessModuleInit(void)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiModuleInit == NULL) {
        HKS_LOG_E("Module Init function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiModuleInit();
}

int32_t HuksAccessRefresh(void)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiRefresh == NULL) {
        HKS_LOG_E("Refresh function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiRefresh();
}

int32_t HuksAccessGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn,
    const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiGenerateKey == NULL) {
        HKS_LOG_E("GenerateKey function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiGenerateKey(keyAlias, paramSetIn, keyIn, keyOut);
}

int32_t HuksAccessImportKey(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiImportKey == NULL) {
        HKS_LOG_E("ImportKey function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiImportKey(keyAlias, key, paramSet, keyOut);
}

int32_t HuksAccessImportWrappedKey(const struct HksBlob *wrappingKeyAlias, const struct HksBlob *key,
    const struct HksBlob *wrappedKeyData, const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiImportWrappedKey == NULL) {
        HKS_LOG_E("ImportWrappedKey function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiImportWrappedKey(wrappingKeyAlias, key, wrappedKeyData, paramSet, keyOut);
}

int32_t HuksAccessExportPublicKey(const struct HksBlob *key,  const struct HksParamSet *paramSet,
    struct HksBlob *keyOut)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiExportPublicKey == NULL) {
        HKS_LOG_E("ExportPublicKey function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiExportPublicKey(key, paramSet, keyOut);
}

int32_t HuksAccessInit(const struct  HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *handle)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiInit == NULL) {
        HKS_LOG_E("Init function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiInit(key, paramSet, handle);
}

int32_t HuksAccessUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiUpdate == NULL) {
        HKS_LOG_E("Update function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiUpdate(handle, paramSet, inData, outData);
}

int32_t HuksAccessFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiFinish == NULL) {
        HKS_LOG_E("Finish function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiFinish(handle, paramSet, inData, outData);
}

int32_t HuksAccessAbort(const struct HksBlob *handle, const struct HksParamSet *paramSet)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiAbort == NULL) {
        HKS_LOG_E("Abort function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiAbort(handle, paramSet);
}

int32_t HuksAccessGetKeyProperties(const struct HksParamSet *paramSet, const struct HksBlob *key)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiGetKeyProperties == NULL) {
        HKS_LOG_E("GetKeyProperties function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiGetKeyProperties(paramSet, key);
}

int32_t HuksAccessGetAbility(int funcType)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiGetAbility == NULL) {
        HKS_LOG_E("GetAbility function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiGetAbility(funcType);
}

int32_t HuksAccessGetHardwareInfo(void)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiGetHardwareInfo == NULL) {
        HKS_LOG_E("GetHardwareInfo function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiGetHardwareInfo();
}

int32_t HuksAccessSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiSign == NULL) {
        HKS_LOG_E("Sign function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiSign(key, paramSet, srcData, signature);
}

int32_t HuksAccessVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiVerify == NULL) {
        HKS_LOG_E("Verify function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiVerify(key, paramSet, srcData, signature);
}

int32_t HuksAccessEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiEncrypt == NULL) {
        HKS_LOG_E("Encrypt function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiEncrypt(key, paramSet, plainText, cipherText);
}

int32_t HuksAccessDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiDecrypt == NULL) {
        HKS_LOG_E("Decrypt function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiDecrypt(key, paramSet, cipherText, plainText);
}

int32_t HuksAccessAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiAgreeKey == NULL) {
        HKS_LOG_E("AgreeKey function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiAgreeKey(paramSet, privateKey, peerPublicKey, agreedKey);
}

int32_t HuksAccessDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *kdfKey,
    struct HksBlob *derivedKey)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiDeriveKey == NULL) {
        HKS_LOG_E("DeriveKey function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiDeriveKey(paramSet, kdfKey, derivedKey);
}

int32_t HuksAccessMac(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiMac == NULL) {
        HKS_LOG_E("Mac function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiMac(key, paramSet, srcData, mac);
}

#ifdef _STORAGE_LITE_
int32_t HuksAccessCalcHeaderMac(const struct HksParamSet *paramSet, const struct HksBlob *salt,
    const struct HksBlob *srcData, struct HksBlob *mac)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiCalcMacHeader == NULL) {
        HKS_LOG_E("CalcMacHeader function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiCalcMacHeader(paramSet, salt, srcData, mac);
}
#endif

#ifdef HKS_SUPPORT_UPGRADE_STORAGE_DATA
int32_t HuksAccessUpgradeKeyInfo(const struct HksBlob *keyAlias, const struct HksBlob *keyInfo,
    struct HksBlob *keyOut)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiUpgradeKeyInfo == NULL) {
        HKS_LOG_E("UpgradeKeyInfo function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiUpgradeKeyInfo(keyAlias, keyInfo, keyOut);
}
#endif

#ifdef HKS_SUPPORT_API_ATTEST_KEY
int32_t HuksAccessAttestKey(const struct HksBlob *key, const struct HksParamSet *paramSet,
    struct HksBlob *certChain)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiAttestKey == NULL) {
        HKS_LOG_E("AttestKey function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiAttestKey(key, paramSet, certChain);
}
#endif

#endif /* _CUT_AUTHENTICATE_ */

int32_t HuksAccessGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random)
{
    if (HksCreateHuksHdiDevice(&g_hksHalDevicePtr) != HKS_SUCCESS) {
        return HKS_ERROR_NULL_POINTER;
    }
    if (g_hksHalDevicePtr->HuksHdiGenerateRandom == NULL) {
        HKS_LOG_E("GenerateRandom function is null pointer");
        return HKS_ERROR_NULL_POINTER;
    }
    return g_hksHalDevicePtr->HuksHdiGenerateRandom(paramSet, random);
}
