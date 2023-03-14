/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HIVIEWDFX_NAPI_ARG
#define HIVIEWDFX_NAPI_ARG

#pragma once

#include <functional>
#include <map>
#include "n_val.h"
#include "uni_header.h"

namespace OHOS {
namespace HiviewDFX {
enum NARG_CNT {
    ZERO = 0,
    ONE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    ELEVEN = 11,
    TWELVE = 12,
};

enum NARG_POS {
    FIRST = 0,
    SECOND = 1,
    THIRD = 2,
    FOURTH = 3,
    FIFTH = 4,
    SIXTH = 5,
    SEVENTH = 6,
    EIGHTH = 7,
    NINTH = 8,
    TENTH = 9,
    ELEVENTH = 10,
    TWELVETH = 11,
};

class NFuncArg final {
public:
    NFuncArg(napi_env env, napi_callback_info info);

    virtual ~NFuncArg();

    bool InitArgs(size_t argc);

    bool InitArgs(size_t minArgc, size_t maxArgc);

    size_t GetArgc() const;

    napi_value GetThisVar() const;

    napi_value operator[](size_t idx) const;

    napi_value GetArg(size_t argPos) const;

private:
    napi_env env_ = nullptr;
    napi_callback_info info_ = nullptr;

    size_t argc_ = 0;
    std::unique_ptr<napi_value[]> argv_ = {nullptr};
    napi_value thisVar_ = nullptr;

    bool InitArgs(std::function<bool()> argcChecker);

    void SetArgc(size_t argc);

    void SetThisVar(napi_value thisVar);
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HIVIEWDFX_NAPI_ARG
