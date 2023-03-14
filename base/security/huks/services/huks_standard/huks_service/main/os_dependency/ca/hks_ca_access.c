/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hks_access.h"
#include "hks_teec.h"

int32_t HksAccessInitialize(void)
{
    return HKS_ERROR_NOT_SUPPORTED;
}

int32_t HksAccessRefresh(void)
{
    return HKS_ERROR_NOT_SUPPORTED;
}

int32_t HksAccessGenerateKey(const struct HksBlob *keyBlob, const struct HksParamSet *paramSetIn,
    const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    (void)keyIn;
    return HksTeeGenerateKey(keyBlob, paramSetIn, keyOut);
}

int32_t HksAccessSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature)
{
    return HksTeeSign(key, paramSet, srcData, signature);
}

int32_t HksAccessVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
    return HksTeeVerify(key, paramSet, srcData, signature);
}

int32_t HksAccessEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText)
{
    return HksTeeEncrypt(key, paramSet, plainText, cipherText);
}

int32_t HksAccessDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText)
{
    return HksTeeDecrypt(key, paramSet, cipherText, plainText);
}

int32_t HksAccessGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random)
{
    return HksTeeGenerateRandom(paramSet, random);
}

int32_t HksAccessImportKey(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *keyOut)
{
    (void)keyAlias;
    return HksTeeImportKey(key, paramSet, keyOut);
}

int32_t HksAccessExportPublicKey(const struct HksBlob *key, const struct HksParamSet *paramSet,
    struct HksBlob *keyOut)
{
    return HksTeeExportPublicKey(key, paramSet, keyOut);
}

int32_t HksAccessAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    return HksTeeAgreeKey(paramSet, privateKey, peerPublicKey, agreedKey);
}

int32_t HksAccessDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *kdfKey,
    struct HksBlob *derivedKey)
{
    return HksTeeDeriveKey(paramSet, kdfKey, derivedKey);
}

int32_t HksAccessMac(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac)
{
    return HksTeeMac(key, paramSet, srcData, mac);
}

int32_t HksAccessCheckKeyValidity(const struct HksParamSet *paramSet, const struct HksBlob *key)
{
    return HksTeeCheckKeyLegality(paramSet, key);
}

int32_t HksAccessExportTrustCerts(struct HksBlob *certChain)
{
    return HksTeeExportTrustCerts(certChain);
}

int32_t HksAccessImportTrustCerts(const struct HksBlob *certChain)
{
    return HksTeeImportTrustCerts(certChain);
}

int32_t HcmAccessIsDeviceKeyExist(const struct HksParamSet *paramSet)
{
    (void)paramSet;
    return HcmTeeIsDeviceKeyExist();
}