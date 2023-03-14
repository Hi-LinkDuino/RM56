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

#ifndef HKS_TEST_CURVE25519
#define HKS_TEST_CURVE25519
#include "hks_type.h"

#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t TestGenerateEd25519Key(struct HksBlob alias);

int32_t TestImportEd25519(struct HksBlob alias, struct HksBlob *pubKeyInfo);

int32_t TestCurve25519All();

int32_t TestEd25519SignTeeVerifyLocal();

int32_t TestCurve25519SignWrong();

int32_t TestCurve25519verifyWrong();

#ifdef __cplusplus
}
#endif

#endif
