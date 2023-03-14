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

#ifndef HKS_RSA_COMMON_MT_H
#define HKS_RSA_COMMON_MT_H

#include <string>
#include <vector>

#include "hks_type.h"

namespace OHOS {
namespace Security {
namespace Huks {
namespace MT {
#define BIT_NUM_OF_UINT8 8

struct GenerateKeyCaseParams {
    std::vector<HksParam> params;
    std::string hexData;
    int padding = 0;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode encryptResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode decryptResult = HksErrorCode::HKS_SUCCESS;
};

struct EncryptLocalCaseParams {
    std::vector<HksParam> params;
    std::string hexData;
    int padding = 0;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode encryptResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode decryptResult = HksErrorCode::HKS_SUCCESS;
};

struct EncryptServiceCaseParams {
    std::string alias;
    std::vector<HksParam> params;
    std::string hexData;
    int padding = 0;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;
    uint32_t keySize;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode encryptResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode decryptResult = HksErrorCode::HKS_SUCCESS;
};

struct DecryptLocalCaseParams {
    std::vector<HksParam> params;
    std::string hexData;
    int padding = 0;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode encryptResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode decryptResult = HksErrorCode::HKS_SUCCESS;
};

struct DecryptServiceCaseParams {
    std::string alias;
    std::vector<HksParam> params;
    std::string hexData;
    int padding = 0;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode encryptResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode decryptResult = HksErrorCode::HKS_SUCCESS;
};

struct SignLocalCaseParams {
    std::vector<HksParam> params;
    int padding = 0;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode signResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode verifyResult = HksErrorCode::HKS_SUCCESS;
};

struct SignServiceCaseParams {
    std::string alias;
    std::vector<HksParam> params;
    int padding = 0;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode signResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode verifyResult = HksErrorCode::HKS_SUCCESS;
};

struct VerifyLocalCaseParams {
    std::vector<HksParam> params;
    int padding = 0;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode signResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode verifyResult = HksErrorCode::HKS_SUCCESS;
};

struct VerifyServiceCaseParams {
    std::string alias;
    std::vector<HksParam> params;
    int padding = 0;
    HksKeyDigest keyDigest = HksKeyDigest::HKS_DIGEST_NONE;
    uint32_t keySize;

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode signResult = HksErrorCode::HKS_SUCCESS;
    HksErrorCode verifyResult = HksErrorCode::HKS_SUCCESS;
};

class HksRsaCommonMt {
protected:
    void GenerateKeyTestCase(const GenerateKeyCaseParams &testCaseParams);

    void EncryptLocalTestCase(const EncryptLocalCaseParams &testCaseParams);

    void EncryptServiceTestCase(const EncryptServiceCaseParams &testCaseParams);

    void DecryptLocalTestCase(const DecryptLocalCaseParams &testCaseParams);

    void DecryptServiceTestCase(const DecryptServiceCaseParams &testCaseParams);

    void SignLocalTestCase(const SignLocalCaseParams &testCaseParams);

    void SignServiceTestCase(const SignServiceCaseParams &testCaseParams);

    void VerifyLocalTestCase(const VerifyLocalCaseParams &testCaseParams);

    void VerifyServiceTestCase(const VerifyServiceCaseParams &testCaseParams);
};
}  // namespace MT
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS
#endif