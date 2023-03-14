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

#ifndef HAPVERIFY_LOG_H
#define HAPVERIFY_LOG_H

#include "hilog/log.h"

namespace OHOS {
namespace Security {
namespace Verify {
static constexpr unsigned int SECURITY_DOMAIN = 0xD002F00;
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN, "HapVerify"};

#define HAPVERIFY_LOG_DEBUG(label, fmt, ...) \
    OHOS::HiviewDFX::HiLog::Debug(label, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define HAPVERIFY_LOG_INFO(label, fmt, ...) \
    OHOS::HiviewDFX::HiLog::Info(label, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define HAPVERIFY_LOG_WARN(label, fmt, ...) \
    OHOS::HiviewDFX::HiLog::Warn(label, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define HAPVERIFY_LOG_ERROR(label, fmt, ...) \
    OHOS::HiviewDFX::HiLog::Error(label, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define HAPVERIFY_LOG_FATAL(label, fmt, ...) \
    OHOS::HiviewDFX::HiLog::Fatal(label, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
} // namespace Verify
} // namespace Security
} // namespace OHOS
#endif // HAPVERIFY_LOG_H
