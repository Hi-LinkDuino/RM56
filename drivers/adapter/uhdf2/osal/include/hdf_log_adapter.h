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

#ifndef HDF_LOG_ADAPTER_H
#define HDF_LOG_ADAPTER_H

#include "hilog/log.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD002500

#ifndef LOG_TAG
#define LOG_TAG HDF
#endif

#define HDF_LOGV_WRAPPER(fmt, arg...) HILOG_DEBUG(LOG_CORE, fmt, ##arg)

#define HDF_LOGD_WRAPPER(fmt, arg...) HILOG_DEBUG(LOG_CORE, fmt, ##arg)

#define HDF_LOGI_WRAPPER(fmt, arg...) HILOG_INFO(LOG_CORE, fmt, ##arg)

#define HDF_LOGW_WRAPPER(fmt, arg...) HILOG_WARN(LOG_CORE, fmt, ##arg)

#define HDF_LOGE_WRAPPER(fmt, arg...) HILOG_ERROR(LOG_CORE, fmt, ##arg)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_LOG_ADAPTER_H */

