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

#include "util/digest_parameter.h"

namespace OHOS::Security::Verify {
DigestParameter::DigestParameter() : digestOutputSizeBytes(0), md(nullptr), ptrCtx(nullptr)
{
}

DigestParameter::~DigestParameter()
{
    if (ptrCtx != nullptr) {
        EVP_MD_CTX_destroy(ptrCtx);
        ptrCtx = nullptr;
    }
    /* md points to the OpenSSL global static struct constant, no need to free. */
    md = nullptr;
}

DigestParameter::DigestParameter(const DigestParameter& other)
{
    digestOutputSizeBytes = other.digestOutputSizeBytes;
    md = other.md;
    ptrCtx = EVP_MD_CTX_create();
    EVP_MD_CTX_copy(ptrCtx, other.ptrCtx);
}

DigestParameter& DigestParameter::operator = (const DigestParameter& other)
{
    if (ptrCtx != nullptr) {
        EVP_MD_CTX_destroy(ptrCtx);
        ptrCtx = nullptr;
    }

    digestOutputSizeBytes = other.digestOutputSizeBytes;
    md = other.md;
    ptrCtx = EVP_MD_CTX_create();
    EVP_MD_CTX_copy(ptrCtx, other.ptrCtx);
    return *this;
}
}  // namespace OHOS::Security::Verify
