/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef SECURITY_APP_VERIFY_BASE_H
#define SECURITY_APP_VERIFY_BASE_H
#include "app_verify_hal.h"
#include "log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define LOG_INFO(fmt, args...) HILOG_INFO(LOG_CORE, "["__FILE__":%d] :"fmt, __LINE__, ##args)
#define LOG_DEBUG(fmt, args...) HILOG_DEBUG(LOG_CORE, "["__FILE__":%d] :"fmt, __LINE__, ##args)
#define LOG_WARN(fmt, args...) HILOG_WARN(LOG_CORE, "["__FILE__":%d] :"fmt, __LINE__, ##args)
#define LOG_ERROR(fmt, args...) HILOG_ERROR(LOG_CORE, "["__FILE__":%d] :"fmt, __LINE__, ##args)
#define LOG_PRINT_STR(fmt, args...) HILOG_INFO(LOG_CORE, "["__FILE__":%d] :"fmt, __LINE__, ##args)

void RegistProductFunc(ProductDiff *productFunc);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
