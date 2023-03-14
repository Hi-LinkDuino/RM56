/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef HC_TIME_H
#define HC_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Calculate in seconds */
#define TIME_OUT_VALUE 300

/* Return in seconds */
int64_t HcGetCurTime(void);

/* Return the interval seconds from startTime to current Time */
int64_t HcGetIntervalTime(int64_t startTime);

#ifdef __cplusplus
}
#endif
#endif