/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "interfaces/hap_verify_result.h"

#include "util/hap_signing_block_utils.h"
#include "util/hap_verify_openssl_utils.h"

namespace OHOS {
namespace Security {
namespace Verify {
HapVerifyResult::HapVerifyResult()
    : version(0), publicKeys(), signatures(), pkcs7SignBlock(),
      pkcs7ProfileBlock(), optionalBlocks(), provisionInfo()
{
}

HapVerifyResult::~HapVerifyResult()
{
}

int HapVerifyResult::GetVersion() const
{
    return version;
}

void HapVerifyResult::SetVersion(int signatureVersion)
{
    version = signatureVersion;
}

void HapVerifyResult::SetPkcs7SignBlock(const HapByteBuffer& pkcs7)
{
    pkcs7SignBlock = pkcs7;
}

void HapVerifyResult::SetPkcs7ProfileBlock(const HapByteBuffer& pkcs7)
{
    pkcs7ProfileBlock = pkcs7;
}

void HapVerifyResult::SetOptionalBlocks(const std::vector<OptionalBlock>& option)
{
    optionalBlocks = option;
}

std::vector<std::string> HapVerifyResult::GetPublicKey() const
{
    return publicKeys;
}

std::vector<std::string> HapVerifyResult::GetSignature() const
{
    return signatures;
}

void HapVerifyResult::SetPublicKey(const std::vector<std::string>& inputPubkeys)
{
    publicKeys = inputPubkeys;
}

void HapVerifyResult::SetSignature(const std::vector<std::string>& inputSignatures)
{
    signatures = inputSignatures;
}

int HapVerifyResult::GetProperty(std::string& property) const
{
    return GetBlockFromOptionalBlocks(PROPERTY_BLOB, property);
}

int HapVerifyResult::GetBlockFromOptionalBlocks(int blockType, std::string& block) const
{
    for (unsigned long i = 0; i < optionalBlocks.size(); i++) {
        if (optionalBlocks[i].optionalType == blockType) {
            const HapByteBuffer& option = optionalBlocks[i].optionalBlockValue;
            block += std::string(option.GetBufferPtr(), option.GetCapacity());
            return GET_SUCCESS;
        }
    }
    return NO_THIS_BLOCK_IN_PACKAGE;
}

void HapVerifyResult::SetProvisionInfo(const ProvisionInfo &info)
{
    provisionInfo = info;
}

ProvisionInfo HapVerifyResult::GetProvisionInfo() const
{
    return provisionInfo;
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
