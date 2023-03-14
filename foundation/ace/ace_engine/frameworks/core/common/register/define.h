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

#ifndef HDC_TEST
#define HDC_TEST

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include <cinttypes>
#include <cstring>
#include <uv.h>
#include <securec.h>

using std::string;
using std::vector;
enum class RetErrCode {
    SUCCESS = 0,
    ERR_GENERIC = -1,
    ERR_BUF_ALLOC = -2,
};
const string HANDSHAKE_MESSAGE = "OHOS HDC-HELLO";

#endif // end HDC_TEST
