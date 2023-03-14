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

#ifndef HAP_SIGNING_BLOCK_UTILS_TEST_H
#define HAP_SIGNING_BLOCK_UTILS_TEST_H

#include <string>

#include "util/signature_info.h"

#include "test_const.h"

namespace OHOS {
namespace Security {
namespace Verify {
long long CreatTestZipFile(const std::string& pathFile, SignatureInfo& signInfo);
}
}
}
int FindHapSignatureTest001(void);
int VerifyHapIntegrityTest001(void);
int FindEocdInHapTest001(void);
int GetCentralDirectoryOffsetTest001(void);
int ClassifyHapSubSigningBlock001(void);
int ComputeDigestsWithOptionalBlock001(void);
int GetSumOfChunkDigestLenTest001(void);
#endif // HAP_SIGNING_BLOCK_UTILS_TEST_H
