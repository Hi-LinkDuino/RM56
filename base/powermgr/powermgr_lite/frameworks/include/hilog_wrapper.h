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

#ifndef POWERMGR_HILOG_WRAPPER_H
#define POWERMGR_HILOG_WRAPPER_H

#define LOG_TAG                 "PowerMgr"

#include <log.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define __FILENAME__            (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define __FORMATED__(fmt, ...)  "[%s] %s# " fmt, __FILENAME__, __FUNCTION__, ##__VA_ARGS__

#ifdef POWER_HILOGD
#undef POWER_HILOGD
#endif

#ifdef POWER_HILOGI
#undef POWER_HILOGI
#endif

#ifdef POWER_HILOGW
#undef POWER_HILOGW
#endif

#ifdef POWER_HILOGE
#undef POWER_HILOGE
#endif

#ifdef POWER_HILOGF
#undef POWER_HILOGF
#endif

#ifdef __LITEOS_M__
#define POWER_HILOGD(...)   HILOG_DEBUG(HILOG_MODULE_POWERMGR, __VA_ARGS__)
#define POWER_HILOGI(...)   HILOG_INFO(HILOG_MODULE_POWERMGR, __VA_ARGS__)
#define POWER_HILOGW(...)   HILOG_WARN(HILOG_MODULE_POWERMGR, __VA_ARGS__)
#define POWER_HILOGE(...)   HILOG_ERROR(HILOG_MODULE_POWERMGR, __VA_ARGS__)
#define POWER_HILOGF(...)   HILOG_FATAL(HILOG_MODULE_POWERMGR, __VA_ARGS__)
#else // !__LITEOS_M__
#define POWER_HILOGD(...)   HILOG_DEBUG(LOG_CORE, __FORMATED__(__VA_ARGS__))
#define POWER_HILOGI(...)   HILOG_INFO(LOG_CORE, __FORMATED__(__VA_ARGS__))
#define POWER_HILOGW(...)   HILOG_WARN(LOG_CORE, __FORMATED__(__VA_ARGS__))
#define POWER_HILOGE(...)   HILOG_ERROR(LOG_CORE, __FORMATED__(__VA_ARGS__))
#define POWER_HILOGF(...)   HILOG_FATAL(LOG_CORE, __FORMATED__(__VA_ARGS__))
#endif // __LITEOS_M__

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // POWERMGR_HILOG_WRAPPER_H
