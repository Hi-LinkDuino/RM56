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

#ifndef OHOS_WIFI_LOG_H
#define OHOS_WIFI_LOG_H


#include "hilog/log_c.h"

#undef LOG_TAG
#define LOG_TAG "WifiHalService"

#undef LOG_DOMAIN
#define  LOG_DOMAIN    0xD001560

#define LOGD(...) ((void)HiLogPrint(LOG_CORE, LOG_DEBUG, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define LOGI(...) ((void)HiLogPrint(LOG_CORE, LOG_INFO, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define LOGW(...) ((void)HiLogPrint(LOG_CORE, LOG_WARN, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define LOGE(...) ((void)HiLogPrint(LOG_CORE, LOG_ERROR, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#define LOGF(...) ((void)HiLogPrint(LOG_CORE, LOG_FATAL, LOG_DOMAIN, LOG_TAG, __VA_ARGS__))

#endif