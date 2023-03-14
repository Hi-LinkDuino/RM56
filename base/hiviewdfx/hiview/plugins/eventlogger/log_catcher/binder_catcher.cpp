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
#include "binder_catcher.h"

#include "securec.h"

#include "common_utils.h"
#include "defines.h"
namespace OHOS {
namespace HiviewDFX {
using namespace OHOS::HiviewDFX::CommonUtils;
BinderCatcher::BinderCatcher() : EventLogCatcher()
{
    name_ = "BinderCatcher";
}

bool BinderCatcher::Initialize(const std::string& strParam1, int intParam1, int intParam2)
{
    // this catcher do not need parameters, just return true
    char buf[BUF_SIZE_512] = {0};
    int ret = snprintf_s(buf, BUF_SIZE_512, BUF_SIZE_512 - 1,
        "BinderCatcher --\n");
    if (ret > 0) {
        description_ = buf;
    }
    return true;
};

int BinderCatcher::Catch(int fd)
{
    logSize_ = EventLogCatcher::AppendFile(fd, "/sys/kernel/debug/binder/transactions")
        + EventLogCatcher::AppendFile(fd, "/sys/kernel/debug/binder/transaction_log");
    return logSize_;
};
} // namespace HiviewDFX
} // namespace OHOS
