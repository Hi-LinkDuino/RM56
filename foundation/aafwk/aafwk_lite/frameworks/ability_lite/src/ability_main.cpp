/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "ability_main.h"

#include <cerrno>
#include <climits>

#include "ability_thread.h"
#include "liteipc_pri.h"
#include "log.h"

namespace {
    constexpr int HEX = 10;
}

int AbilityMain(const char *token)
{
    if (token == nullptr) {
        return -1;
    }

    ResetLiteIpc();
    char *endPtr = nullptr;
    errno = 0;
    uint64_t tokenId = std::strtoull(token, &endPtr, HEX);
    if ((errno == ERANGE && tokenId == ULLONG_MAX) || (errno != 0 && tokenId == 0) ||
        endPtr == nullptr || *endPtr != '\0') {
        HILOG_ERROR(HILOG_MODULE_APP, "token is invalid");
        return -1;
    }

    OHOS::AbilityThread::ThreadMain(tokenId);
    return 0;
}
