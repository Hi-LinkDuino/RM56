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

#include "hks_core_interfaces.h"

#include "hks_core_service.h"
#include "hks_log.h"
#include "hks_mem.h"

int32_t HuksHdiModuleInit(void)
{
    return HksCoreModuleInit();
}

int32_t HuksHdiRefresh(void)
{
    return HksCoreRefresh();
}

int32_t HuksHdiGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    return HksCoreGenerateKey(keyAlias, paramSet, keyIn, keyOut);
}

int32_t HuksHdiImportKey(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    return HksCoreImportKey(keyAlias, key, paramSet, keyOut);
}

int32_t HuksHdiImportWrappedKey(const struct HksBlob *wrappingKeyAlias, const struct HksBlob *key,
    const struct HksBlob *wrappedKeyData, const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    return HksCoreImportWrappedKey(wrappingKeyAlias, key, wrappedKeyData, paramSet, keyOut);
}

int32_t HuksHdiExportPublicKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    return HksCoreExportPublicKey(key, paramSet, keyOut);
}

int32_t HuksHdiInit(const struct  HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *handle)
{
    return HksCoreInit(key, paramSet, handle);
}

int32_t HuksHdiUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData,
    struct HksBlob *outData)
{
    return HksCoreUpdate(handle, paramSet, inData, outData);
}

int32_t HuksHdiFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData,
    struct HksBlob *outData)
{
    return HksCoreFinish(handle, paramSet, inData, outData);
}

int32_t HuksHdiAbort(const struct HksBlob *handle, const struct HksParamSet *paramSet)
{
    return HksCoreAbort(handle, paramSet);
}

int32_t HuksHdiGetKeyProperties(const struct HksParamSet *paramSet, const struct HksBlob *key)
{
    return HksCoreGetKeyProperties(paramSet, key);
}

int32_t HuksHdiAttestKey(const struct HksBlob *key, const  struct HksParamSet *paramSet, struct HksBlob *certChain)
{
    return HksCoreAttestKey(key, paramSet, certChain);
}

int32_t HuksHdiGetAbility(int32_t funcType)
{
    return HksCoreGetAbility(funcType);
}

int32_t HuksHdiGetHardwareInfo(void)
{
    return HksCoreGetHardwareInfo();
}

int32_t HuksHdiGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random)
{
    return HksCoreGenerateRandom(paramSet, random);
}

int32_t HuksHdiSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
    return HksCoreSign(key, paramSet, srcData, signature);
}

int32_t HuksHdiVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
    return HksCoreVerify(key, paramSet, srcData, signature);
}

int32_t HuksHdiEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText)
{
    return HksCoreEncrypt(key, paramSet, plainText, cipherText);
}

int32_t HuksHdiDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText)
{
    return HksCoreDecrypt(key, paramSet, cipherText, plainText);
}

int32_t HuksHdiAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    return HksCoreAgreeKey(paramSet, privateKey, peerPublicKey, agreedKey);
}

int32_t HuksHdiDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *kdfKey,
    struct HksBlob *derivedKey)
{
    return HksCoreDeriveKey(paramSet, kdfKey, derivedKey);
}

int32_t HuksHdiMac(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac)
{
    return HksCoreMac(key, paramSet, srcData, mac);
}

#ifdef _STORAGE_LITE_
int32_t HuksHdiCalcMacHeader(const struct HksParamSet *paramSet, const struct HksBlob *salt,
    const struct HksBlob *srcData, struct HksBlob *mac)
{
    return HksCoreCalcMacHeader(paramSet, salt, srcData, mac);
}
#endif

#ifdef HKS_SUPPORT_UPGRADE_STORAGE_DATA
int32_t HuksHdiUpgradeKeyInfo(const struct HksBlob *keyAlias, const struct HksBlob *keyInfo, struct HksBlob *keyOut)
{
    return HksCoreUpgradeKeyInfo(keyAlias, keyInfo, keyOut);
}
#endif

struct HuksHdi *HuksCreateHdiDevicePtr(void)
{
    struct HuksHdi *hdiDevicePtr = (struct HuksHdi *)HksMalloc(sizeof(struct HuksHdi));
    if (hdiDevicePtr == NULL) {
        HKS_LOG_E("malloc hdiDevicePtr failed.");
        return hdiDevicePtr;
    }
    (void)memset_s(hdiDevicePtr, sizeof(struct HuksHdi), 0, sizeof(struct HuksHdi));

#ifndef _CUT_AUTHENTICATE_
    hdiDevicePtr->HuksHdiModuleInit       = HuksHdiModuleInit;
    hdiDevicePtr->HuksHdiRefresh          = HuksHdiRefresh;
    hdiDevicePtr->HuksHdiGenerateKey      = HuksHdiGenerateKey;
    hdiDevicePtr->HuksHdiImportKey        = HuksHdiImportKey;
    hdiDevicePtr->HuksHdiImportWrappedKey = HuksHdiImportWrappedKey;
    hdiDevicePtr->HuksHdiExportPublicKey  = HuksHdiExportPublicKey;
    hdiDevicePtr->HuksHdiInit             = HuksHdiInit;
    hdiDevicePtr->HuksHdiUpdate           = HuksHdiUpdate;
    hdiDevicePtr->HuksHdiFinish           = HuksHdiFinish;
    hdiDevicePtr->HuksHdiAbort            = HuksHdiAbort;
    hdiDevicePtr->HuksHdiGetKeyProperties = HuksHdiGetKeyProperties;
    hdiDevicePtr->HuksHdiAttestKey        = HuksHdiAttestKey;
    hdiDevicePtr->HuksHdiGetAbility       = HuksHdiGetAbility;
    hdiDevicePtr->HuksHdiGetHardwareInfo  = HuksHdiGetHardwareInfo;
    hdiDevicePtr->HuksHdiSign             = HuksHdiSign;
    hdiDevicePtr->HuksHdiVerify           = HuksHdiVerify;
    hdiDevicePtr->HuksHdiEncrypt          = HuksHdiEncrypt;
    hdiDevicePtr->HuksHdiDecrypt          = HuksHdiDecrypt;
    hdiDevicePtr->HuksHdiAgreeKey         = HuksHdiAgreeKey;
    hdiDevicePtr->HuksHdiDeriveKey        = HuksHdiDeriveKey;
    hdiDevicePtr->HuksHdiMac              = HuksHdiMac;
#ifdef _STORAGE_LITE_
    hdiDevicePtr->HuksHdiCalcMacHeader    = HuksHdiCalcMacHeader;
#endif

#ifdef HKS_SUPPORT_UPGRADE_STORAGE_DATA
    hdiDevicePtr->HuksHdiUpgradeKeyInfo   = HuksHdiUpgradeKeyInfo;
#endif
#endif /* _CUT_AUTHENTICATE_ */

    hdiDevicePtr->HuksHdiGenerateRandom   = HksCoreGenerateRandom;

    return hdiDevicePtr;
}

void HuksDestoryHdiDevicePtr(struct HuksHdi *hdiDevicePtr)
{
    HKS_FREE_PTR(hdiDevicePtr);
}
