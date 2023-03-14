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

#ifndef NATIVE_RDB_LOG_PRINT_H
#define NATIVE_RDB_LOG_PRINT_H

#ifndef LOG_TAG
#define LOG_TAG
#endif

#include "hilog/log.h"

namespace OHOS::NativeRdb {
static const OHOS::HiviewDFX::HiLogLabel RDB_LABEL = { LOG_CORE, 0xD001650, "NativeRDB" };

#define LOG_DEBUG(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Debug(RDB_LABEL, LOG_TAG "::%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Info(RDB_LABEL, LOG_TAG "::%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Warn(RDB_LABEL, LOG_TAG "::%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Error(RDB_LABEL, LOG_TAG "::%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) \
    (void)OHOS::HiviewDFX::HiLog::Fatal(RDB_LABEL, LOG_TAG "::%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)
} // namespace OHOS::NativeRdb
#endif
