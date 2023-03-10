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

#ifndef LOGGER_H
#define LOGGER_H

#include "hilog/log.h"

namespace OHOS {
namespace AppDataFwk {
static const OHOS::HiviewDFX::HiLogLabel APP_DATA_FWK_LABEL = { LOG_CORE, 0xD001650, "APP_DATA_FWK_NATIVE" };

#define LOG_DEBUG(...) ((void)OHOS::HiviewDFX::HiLog::Debug(APP_DATA_FWK_LABEL, __VA_ARGS__))
#define LOG_INFO(...) ((void)OHOS::HiviewDFX::HiLog::Info(APP_DATA_FWK_LABEL, __VA_ARGS__))
#define LOG_WARN(...) ((void)OHOS::HiviewDFX::HiLog::Warn(APP_DATA_FWK_LABEL, __VA_ARGS__))
#define LOG_ERROR(...) ((void)OHOS::HiviewDFX::HiLog::Error(APP_DATA_FWK_LABEL, __VA_ARGS__))
#define LOG_FATAL(...) ((void)OHOS::HiviewDFX::HiLog::Fatal(APP_DATA_FWK_LABEL, __VA_ARGS__))
} // namespace AppDataFwk
} // namespace OHOS

#endif // LOGGER_H