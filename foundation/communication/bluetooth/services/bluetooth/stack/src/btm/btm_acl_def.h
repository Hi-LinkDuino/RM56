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

#ifndef BTM_ACL_DEF_H
#define BTM_ACL_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#define BTM_MAX_ACL 13

#define ACL_TIMEOUT (15 * 1000)          // 15s
#define ACL_PASSIVE_TIMEOUT (60 * 1000)  // 60s

#define LE_SCAN_INTERVAL_DEFAULT 96      // 96 * 0.625ms = 60ms
#define LE_SCAN_WINDOW_DEFAULT 48        // 48 * 0.625ms = 30ms
#define LE_SCAN_INTERVAL_FAST 96         // 96 * 0.625ms = 60ms
#define LE_SCAN_WINDOW_FAST 48           // 48 * 0.625ms = 30ms
#define LE_SCAN_INTERVAL_SLOW 2048       // 2048 * 0.625ms = 1.28s
#define LE_SCAN_WINDOW_SLOW 48           // 48 * 0.625ms = 30ms
#define LE_CONN_INTERVAL_MIN_DEFAULT 24  // 24 * 1.25ms = 30ms
#define LE_CONN_INTERVAL_MAX_DEFAULT 40  // 40 * 1.25ms = 50ms
#define LE_CONN_LATENCY_DEFAULT 0
#define LE_SUPERVISION_TIMEOUT_DEFAULT 500  // 50 10ms = 5s
#define LE_MINIMUM_CE_LENGTH_DEFAULT 0
#define LE_MAXIMUM_CE_LENGTH_DEFAULT 0

#ifdef __cplusplus
}
#endif

#endif