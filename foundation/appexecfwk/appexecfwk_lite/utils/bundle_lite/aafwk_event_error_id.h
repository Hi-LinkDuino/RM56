/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef UTILS_BUNDLE_LITE_AAFWK_EVENT_ERROR_ID_H
#define UTILS_BUNDLE_LITE_AAFWK_EVENT_ERROR_ID_H

namespace OHOS {
//  event codes
enum {
    MT_ACE_APP_START = 0x01,
    MT_ACE_APP_ACTIVE,
    MT_ACE_APP_BACKGROUND,
    MT_ACE_APP_STOP
} AceAppType;

//  start error codes
const int EXCE_ACE_APP_START = 0x01;

enum {
    EXCE_ACE_APP_START_UNKNOWN_BUNDLE_INFO = 0x0,
    EXCE_ACE_APP_START_CREATE_TSAK_FAILED = 0x3,
    EXCE_ACE_APP_START_LAUNCHER_EXIT_FAILED,
    EXCE_ACE_APP_STOP_NO_ABILITY_RUNNING,
    EXCE_ACE_APP_STOP_UNKNOWN_ABILITY_TOKEN
} AceAppStartError;

//  scan error codes
const int EXCE_ACE_APP_SCAN = 0x07;

enum {
    EXCE_ACE_APP_SCAN_INVALID_SYSTEM_APP = 0x0,
    EXCE_ACE_APP_SCAN_PARSE_JSON_FALIED,
    EXCE_ACE_APP_SCAN_PARSE_PROFILE_FALIED,
    EXCE_ACE_APP_SCAN_UNKNOWN_BUNDLE_INFO
} AceAppScanError;
}
#endif