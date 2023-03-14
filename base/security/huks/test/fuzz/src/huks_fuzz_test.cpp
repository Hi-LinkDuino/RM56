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

#include <algorithm>
#include <chrono>
#include <functional>
#include <random>

#include <gtest/gtest.h>

#include "hks_api.h"
#include "hks_mem.h"
#include "hks_param.h"

using namespace testing::ext;

namespace OHOS {
namespace Security {
namespace Huks {
namespace FuzzTest {
namespace {
constexpr uint32_t HUKS_FUZZ_TEST_HOURS = 1;
constexpr auto HUKS_FUZZ_TEST_TIME = std::chrono::hours(HUKS_FUZZ_TEST_HOURS);
constexpr uint32_t MAX_PARAM_COUNT_IN_PARAM_SET = 32;
constexpr uint32_t MAX_RANDOM_BYTES = 32;
constexpr uint32_t BITS_32 = 32;
constexpr uint32_t MOD_2 = 2;
constexpr uint32_t MAX_BYTES_BN = 512;

enum class HuksApi : uint32_t {
    GENERATE_KEY = 0,
    IMPORT_KEY,
    GET_SDK_VERSION,
    INITIALIZE,
    REFRESH_KEY_INFO,
    EXPORT_PUBLIC_KEY,
    DELETE_KEY,
    GET_KEY_PARAMSET,
    KEY_EXIST,
    GENERATE_RANDOM,
    SIGN,
    VERIFY,
    ENCRYPT,
    DECRYPT,
    AGREE_KEY,
    DERIVE_KEY,
    MAC,
    HASH,
    GET_KEY_INFO_LIST,
    ATTEST_KEY,
    GET_CERTIFICATE_CHAIN,
    WRAP_KEY,
    UNWRAP_KEY,
    BN_EXP_MOD,
    HCM_IS_DEVICE_KEY_EXIST,
    HUKS_API_MAX,
};
}  // namespace

class HuksFuzzTest : public testing::Test {
protected:
    void RunFuzzTest() const;

private:
    uint32_t RandomUint32() const;
    bool RandomBool() const;
    const std::function<void()> RandomHuksApi() const;
    uint8_t *RandomByte(uint32_t length) const;
    struct HksBlob *RandomBlob(uint32_t maxSize = MAX_OUT_BLOB_SIZE) const;
    struct HksParamSet *RandomParamSetIn() const;
    struct HksParamSet *RandomParamSetOut() const;
    struct HksKeyInfo *RandomKeyInfoOut() const;
    struct HksCertChain *RandomCertChainOut() const;

    void FreeBlob(struct HksBlob *&blob) const;
    void FreeKeyInfoOut(struct HksKeyInfo *&keyInfo) const;
    void FreeCertChainOut(struct HksCertChain *&certchain) const;

private:
    void HuksFuzzGenerateKey() const;
    void HuksFuzzImportKey() const;
    void HuksFuzzGetSdkVersion() const;
    void HuksFuzzInitialize() const;
    void HuksFuzzRefreshKeyInfo() const;
    void HuksFuzzExportPublicKey() const;
    void HuksFuzzDeleteKey() const;
    void HuksFuzzGetKeyParamSet() const;
    void HuksFuzzKeyExist() const;
    void HuksFuzzGenerateRandom() const;
    void HuksFuzzSign() const;
    void HuksFuzzVerify() const;
    void HuksFuzzEncrypt() const;
    void HuksFuzzDecrypt() const;
    void HuksFuzzAgreeKey() const;
    void HuksFuzzDeriveKey() const;
    void HuksFuzzMac() const;
    void HuksFuzzHash() const;
    void HuksFuzzGetKeyInfoList() const;
    void HuksFuzzAttestKey() const;
    void HuksFuzzGetCertificateChain() const;
    void HuksFuzzWrapKey() const;
    void HuksFuzzUnwrapKey() const;
    void HuksFuzzBnExpMod() const;
    void HuksFuzzHcmIsDeviceKeyExist() const;

    const std::function<void()> fuzzApiList_[static_cast<uint32_t>(HuksApi::HUKS_API_MAX)] = {
        std::bind(&HuksFuzzTest::HuksFuzzGenerateKey, this),
        std::bind(&HuksFuzzTest::HuksFuzzImportKey, this),
        std::bind(&HuksFuzzTest::HuksFuzzGetSdkVersion, this),
        std::bind(&HuksFuzzTest::HuksFuzzInitialize, this),
        std::bind(&HuksFuzzTest::HuksFuzzRefreshKeyInfo, this),
        std::bind(&HuksFuzzTest::HuksFuzzExportPublicKey, this),
        std::bind(&HuksFuzzTest::HuksFuzzDeleteKey, this),
        std::bind(&HuksFuzzTest::HuksFuzzGetKeyParamSet, this),
        std::bind(&HuksFuzzTest::HuksFuzzKeyExist, this),
        std::bind(&HuksFuzzTest::HuksFuzzGenerateRandom, this),
        std::bind(&HuksFuzzTest::HuksFuzzSign, this),
        std::bind(&HuksFuzzTest::HuksFuzzVerify, this),
        std::bind(&HuksFuzzTest::HuksFuzzEncrypt, this),
        std::bind(&HuksFuzzTest::HuksFuzzDecrypt, this),
        std::bind(&HuksFuzzTest::HuksFuzzAgreeKey, this),
        std::bind(&HuksFuzzTest::HuksFuzzDeriveKey, this),
        std::bind(&HuksFuzzTest::HuksFuzzMac, this),
        std::bind(&HuksFuzzTest::HuksFuzzHash, this),
        std::bind(&HuksFuzzTest::HuksFuzzGetKeyInfoList, this),
        std::bind(&HuksFuzzTest::HuksFuzzAttestKey, this),
        std::bind(&HuksFuzzTest::HuksFuzzGetCertificateChain, this),
        std::bind(&HuksFuzzTest::HuksFuzzWrapKey, this),
        std::bind(&HuksFuzzTest::HuksFuzzUnwrapKey, this),
        std::bind(&HuksFuzzTest::HuksFuzzBnExpMod, this),
        std::bind(&HuksFuzzTest::HuksFuzzHcmIsDeviceKeyExist, this),
    };
};

void HuksFuzzTest::RunFuzzTest() const
{
    auto api = RandomHuksApi();
    return api();
}

uint32_t HuksFuzzTest::RandomUint32() const
{
    std::random_device rd;
    return rd();
}

bool HuksFuzzTest::RandomBool() const
{
    return (RandomUint32() % MOD_2);
}

const std::function<void()> HuksFuzzTest::RandomHuksApi() const
{
    uint32_t index = RandomUint32() % static_cast<uint32_t>(HuksApi::HUKS_API_MAX);
    return fuzzApiList_[index];
}

uint8_t *HuksFuzzTest::RandomByte(uint32_t length) const
{
    if (length == 0) {
        return nullptr;
    }

    uint8_t *data = (uint8_t *)HksMalloc(length);
    if (data != nullptr) {
        std::generate(data, data + ((length > MAX_RANDOM_BYTES) ? MAX_RANDOM_BYTES : length), []() {
            std::random_device rd;
            return rd();
        });
    }

    return data;
}

struct HksBlob *HuksFuzzTest::RandomBlob(uint32_t maxSize) const
{
    if (maxSize == 0) {
        return nullptr;
    }

    if (RandomUint32() == 0) {
        return nullptr;
    }

    struct HksBlob *blob = (struct HksBlob *)HksMalloc(sizeof(struct HksBlob));
    if (blob != nullptr) {
        uint32_t size = RandomUint32() % maxSize;
        if (size != 0) {
            blob->size = size;
            blob->data = RandomByte(blob->size);
        } else {
            blob->size = 0;
            blob->data = nullptr;
        }
        if (blob->data == nullptr) {
            blob->size = 0;
        }
    }

    return blob;
}

void HuksFuzzTest::FreeBlob(struct HksBlob *&blob) const
{
    if (blob == nullptr) {
        return;
    }

    if (blob->data != nullptr) {
        HksFree(blob->data);
        blob->data = NULL;
    }

    HksFree(blob);
    blob = nullptr;
}

void HuksFuzzTest::FreeKeyInfoOut(struct HksKeyInfo *&keyInfo) const
{
    if (keyInfo == nullptr) {
        return;
    }

    if (keyInfo->paramSet != nullptr) {
        HksFreeParamSet(&keyInfo->paramSet);
    }

    HksFree(keyInfo);
    keyInfo = nullptr;
}

void HuksFuzzTest::FreeCertChainOut(struct HksCertChain *&certchain) const
{
    if (certchain == nullptr) {
        return;
    }

    if (certchain->certs != nullptr) {
        for (uint32_t ii = 0; ii < certchain->certsCount; ii++) {
            if (certchain->certs[ii].data != nullptr) {
                HksFree(certchain->certs[ii].data);
            }
        }
    }

    HksFree(certchain);
    certchain = nullptr;
}

struct HksParamSet *HuksFuzzTest::RandomParamSetIn() const
{
    if (RandomUint32() == 0) {
        return nullptr;
    }

    struct HksParamSet *paramSet = nullptr;
    int32_t result = HksInitParamSet(&paramSet);
    if (result != HKS_SUCCESS) {
        return nullptr;
    }

    std::vector<struct HksParam> params;
    uint32_t paramCount = RandomUint32() % MAX_PARAM_COUNT_IN_PARAM_SET;
    for (uint32_t i = 0; i < paramCount; i++) {
        struct HksParam param = {0};
        param.tag = RandomUint32();
        if ((param.tag & HKS_TAG_TYPE_INT) || (param.tag & HKS_TAG_TYPE_UINT)) {
            param.uint32Param = RandomUint32();
        } else if (param.tag & HKS_TAG_TYPE_ULONG) {
            param.uint64Param = (((uint64_t)RandomUint32() << BITS_32) || RandomUint32());
        } else if (param.tag & HKS_TAG_TYPE_BOOL) {
            param.boolParam = RandomBool();
        } else if (param.tag & HKS_TAG_TYPE_BYTES) {
            param.blob.size = RandomUint32() / MAX_KEY_SIZE;
            if (param.blob.size != 0) {
                param.blob.data = RandomByte(param.blob.size);
            } else {
                param.blob.data = nullptr;
            }
            if (param.blob.data == nullptr) {
                param.blob.size = 0;
            }
        }
        params.push_back(param);
    }
    result = HksAddParams(paramSet, params.data(), params.size());

    result = HksBuildParamSet(&paramSet);

    for (auto param : params) {
        if (param.tag & HKS_TAG_TYPE_BYTES) {
            if (param.blob.data != nullptr) {
                HksFree(param.blob.data);
                param.blob.data = 0;
            }
        }
    }
    return paramSet;
}

struct HksParamSet *HuksFuzzTest::RandomParamSetOut() const
{
    uint32_t size = RandomUint32() % MAX_OUT_BLOB_SIZE;
    if (size == 0) {
        return nullptr;
    }
    struct HksParamSet *paramSet = (struct HksParamSet *)HksMalloc(size);
    if (paramSet != nullptr) {
        paramSet->paramSetSize = size;
        paramSet->paramsCnt = 0;
    }
    return paramSet;
}

struct HksKeyInfo *HuksFuzzTest::RandomKeyInfoOut() const
{
    if (RandomUint32() == 0) {
        return nullptr;
    }

    struct HksKeyInfo *keyInfo = (struct HksKeyInfo *)HksMalloc(sizeof(struct HksKeyInfo));
    if (keyInfo != nullptr) {
        keyInfo->alias.size = 0;
        keyInfo->alias.data = nullptr;
        keyInfo->paramSet = RandomParamSetOut();
    }

    return keyInfo;
}

struct HksCertChain *HuksFuzzTest::RandomCertChainOut() const
{
    if (RandomUint32() == 0) {
        return nullptr;
    }

    struct HksCertChain *certChain = (struct HksCertChain *)HksMalloc(sizeof(struct HksCertChain));
    if (certChain != nullptr) {
        certChain->certs = nullptr;
        certChain->certsCount = 0;
    }

    return certChain;
}

void HuksFuzzTest::HuksFuzzGenerateKey() const
{
    struct HksBlob *keyAlias = RandomBlob(HKS_MAX_KEY_ALIAS_LEN * 2);
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksParamSet *paramSetOut = RandomParamSetOut();

    HksGenerateKey(keyAlias, paramSetIn, paramSetOut);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (paramSetOut != nullptr) {
        HksFreeParamSet(&paramSetOut);
    }
}

void HuksFuzzTest::HuksFuzzImportKey() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *key = RandomBlob();

    HksImportKey(keyAlias, paramSetIn, key);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (key != nullptr) {
        FreeBlob(key);
    }
}

void HuksFuzzTest::HuksFuzzGetSdkVersion() const
{
    struct HksBlob *sdkVersion = RandomBlob();

    HksGetSdkVersion(sdkVersion);

    if (sdkVersion != nullptr) {
        FreeBlob(sdkVersion);
    }
}

void HuksFuzzTest::HuksFuzzInitialize() const
{
    HksInitialize();
}

void HuksFuzzTest::HuksFuzzRefreshKeyInfo() const
{
    HksRefreshKeyInfo();
}

void HuksFuzzTest::HuksFuzzExportPublicKey() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *key = RandomBlob();

    HksExportPublicKey(keyAlias, paramSetIn, key);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (key != nullptr) {
        FreeBlob(key);
    }
}

void HuksFuzzTest::HuksFuzzDeleteKey() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();

    HksDeleteKey(keyAlias, paramSetIn);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
}

void HuksFuzzTest::HuksFuzzGetKeyParamSet() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksParamSet *paramSetOut = RandomParamSetOut();

    HksGetKeyParamSet(keyAlias, paramSetIn, paramSetOut);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (paramSetOut != nullptr) {
        HksFreeParamSet(&paramSetOut);
    }
}

void HuksFuzzTest::HuksFuzzKeyExist() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();

    HksKeyExist(keyAlias, paramSetIn);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
}

void HuksFuzzTest::HuksFuzzGenerateRandom() const
{
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *random = RandomBlob();

    HksGenerateRandom(paramSetIn, random);

    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (random != nullptr) {
        FreeBlob(random);
    }
}

void HuksFuzzTest::HuksFuzzSign() const
{
    struct HksBlob *key = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *srcData = RandomBlob();
    struct HksBlob *signature = RandomBlob();

    HksSign(key, paramSetIn, srcData, signature);

    if (key != nullptr) {
        FreeBlob(key);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (srcData != nullptr) {
        FreeBlob(srcData);
    }
    if (signature != nullptr) {
        FreeBlob(signature);
    }
}

void HuksFuzzTest::HuksFuzzVerify() const
{
    struct HksBlob *key = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *srcData = RandomBlob();
    struct HksBlob *signature = RandomBlob();

    HksVerify(key, paramSetIn, srcData, signature);

    if (key != nullptr) {
        FreeBlob(key);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (srcData != nullptr) {
        FreeBlob(srcData);
    }
    if (signature != nullptr) {
        FreeBlob(signature);
    }
}

void HuksFuzzTest::HuksFuzzEncrypt() const
{
    struct HksBlob *key = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *plainText = RandomBlob();
    struct HksBlob *cipherText = RandomBlob();

    HksEncrypt(key, paramSetIn, plainText, cipherText);

    if (key != nullptr) {
        FreeBlob(key);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (plainText != nullptr) {
        FreeBlob(plainText);
    }
    if (cipherText != nullptr) {
        FreeBlob(cipherText);
    }
}

void HuksFuzzTest::HuksFuzzDecrypt() const
{
    struct HksBlob *key = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *cipherText = RandomBlob();
    struct HksBlob *plainText = RandomBlob();

    HksDecrypt(key, paramSetIn, cipherText, plainText);

    if (key != nullptr) {
        FreeBlob(key);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (cipherText != nullptr) {
        FreeBlob(cipherText);
    }
    if (plainText != nullptr) {
        FreeBlob(plainText);
    }
}

void HuksFuzzTest::HuksFuzzAgreeKey() const
{
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *privateKey = RandomBlob();
    struct HksBlob *peerPublicKey = RandomBlob();
    struct HksBlob *agreedKey = RandomBlob();

    HksAgreeKey(paramSetIn, privateKey, peerPublicKey, agreedKey);

    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (privateKey != nullptr) {
        FreeBlob(privateKey);
    }
    if (peerPublicKey != nullptr) {
        FreeBlob(peerPublicKey);
    }
    if (agreedKey != nullptr) {
        FreeBlob(agreedKey);
    }
}

void HuksFuzzTest::HuksFuzzDeriveKey() const
{
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *mainKey = RandomBlob();
    struct HksBlob *derivedKey = RandomBlob();

    HksDeriveKey(paramSetIn, mainKey, derivedKey);

    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (mainKey != nullptr) {
        FreeBlob(mainKey);
    }
    if (derivedKey != nullptr) {
        FreeBlob(derivedKey);
    }
}

void HuksFuzzTest::HuksFuzzMac() const
{
    struct HksBlob *key = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *srcData = RandomBlob();
    struct HksBlob *mac = RandomBlob();

    HksMac(key, paramSetIn, srcData, mac);

    if (key != nullptr) {
        FreeBlob(key);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (srcData != nullptr) {
        FreeBlob(srcData);
    }
    if (mac != nullptr) {
        FreeBlob(mac);
    }
}

void HuksFuzzTest::HuksFuzzHash() const
{
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *srcData = RandomBlob();
    struct HksBlob *hash = RandomBlob();

    HksHash(paramSetIn, srcData, hash);

    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (srcData != nullptr) {
        FreeBlob(srcData);
    }
    if (hash != nullptr) {
        FreeBlob(hash);
    }
}

void HuksFuzzTest::HuksFuzzGetKeyInfoList() const
{
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksKeyInfo *keyInfoList = RandomKeyInfoOut();
    uint32_t listCount = RandomUint32();

    HksGetKeyInfoList(paramSetIn, keyInfoList, &listCount);

    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (keyInfoList != nullptr) {
        FreeKeyInfoOut(keyInfoList);
    }
}

void HuksFuzzTest::HuksFuzzAttestKey() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksCertChain *certChain = RandomCertChainOut();

    HksAttestKey(keyAlias, paramSetIn, certChain);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (certChain != nullptr) {
        FreeCertChainOut(certChain);
    }
}

void HuksFuzzTest::HuksFuzzGetCertificateChain() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksCertChain *certChain = RandomCertChainOut();

    HksGetCertificateChain(keyAlias, paramSetIn, certChain);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (certChain != nullptr) {
        FreeCertChainOut(certChain);
    }
}

void HuksFuzzTest::HuksFuzzWrapKey() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksBlob *targetKeyAlias = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();
    struct HksBlob *wrappedData = RandomBlob();

    HksWrapKey(keyAlias, targetKeyAlias, paramSetIn, wrappedData);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (targetKeyAlias != nullptr) {
        FreeBlob(targetKeyAlias);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
    if (wrappedData != nullptr) {
        FreeBlob(wrappedData);
    }
}

void HuksFuzzTest::HuksFuzzUnwrapKey() const
{
    struct HksBlob *keyAlias = RandomBlob();
    struct HksBlob *targetKeyAlias = RandomBlob();
    struct HksBlob *wrappedData = RandomBlob();
    struct HksParamSet *paramSetIn = RandomParamSetIn();

    HksUnwrapKey(keyAlias, targetKeyAlias, wrappedData, paramSetIn);

    if (keyAlias != nullptr) {
        FreeBlob(keyAlias);
    }
    if (targetKeyAlias != nullptr) {
        FreeBlob(targetKeyAlias);
    }
    if (wrappedData != nullptr) {
        FreeBlob(wrappedData);
    }
    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
}

void HuksFuzzTest::HuksFuzzBnExpMod() const
{
    struct HksBlob *x = RandomBlob();
    struct HksBlob *a = RandomBlob(MAX_BYTES_BN);
    struct HksBlob *e = RandomBlob(MAX_BYTES_BN);
    struct HksBlob *n = RandomBlob(MAX_BYTES_BN);

    HksBnExpMod(x, a, e, n);

    if (x != nullptr) {
        FreeBlob(x);
    }
    if (a != nullptr) {
        FreeBlob(a);
    }
    if (e != nullptr) {
        FreeBlob(e);
    }
    if (n != nullptr) {
        FreeBlob(n);
    }
}

void HuksFuzzTest::HuksFuzzHcmIsDeviceKeyExist() const
{
    struct HksParamSet *paramSetIn = RandomParamSetIn();

    HcmIsDeviceKeyExist(paramSetIn);

    if (paramSetIn != nullptr) {
        HksFreeParamSet(&paramSetIn);
    }
}

/**
 * @tc.number    : HuksFuzzTest000
 * @tc.name      : Huks fuzz test
 * @tc.desc      : Huks fuzz test
 */
HWTEST_F(HuksFuzzTest, HuksFuzzTest000, TestSize.Level1)
{
    auto expired = std::chrono::system_clock::now() + HUKS_FUZZ_TEST_TIME;
    do {
        RunFuzzTest();
    } while (std::chrono::system_clock::now() < expired);
}
}  // namespace FuzzTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS