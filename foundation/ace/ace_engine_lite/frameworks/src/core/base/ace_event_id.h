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

#ifndef OHOS_ACELITE_ACE_EVENT_ID_H
#define OHOS_ACELITE_ACE_EVENT_ID_H

// ACE framework event codes
#define MT_ACE_FWK_LAUNCHING 0x00
#define MT_ACE_RELEASE_HISTORY_PAGE  0x01
#define MT_ACE_FWK_ACTIVING 0x02
#define MT_ACE_FWK_HIDING 0x03
#define MT_ACE_FWK_DESTROYING 0x04

#define MT_ACE_FEATUREABILITY  0x08
#define MT_ACE_FEATUREABILITY_SUBSCRIBEMSG  0x00
#define MT_ACE_FEATUREABILITY_UNSUBSCRIBEMSG  0x01

// ACE framework error codes
#define EXCE_ACE_FWK_LAUNCH_FAILED  0x01
#define EXCE_ACE_ROUTER_REPLACE_FAILED  0x02
#define EXCE_ACE_ASYNC_WORK_FAILED  0x03
#define EXCE_ACE_LOCALIZATION_FAILED  0x04
#define EXCE_ACE_ENGINE_RUNTIME_ERROR  0x05

// ACE framework error code : engine runtime error
#define EXCE_ACE_JS_HEAP_OOM  0x0001
#define EXCE_ACE_JS_REF_COUNT_LIMIT  0x0002
#define EXCE_ACE_JS_DISABLED_BYTE_CODE  0x0003
#define EXCE_ACE_JS_FAILED_INTERNAL_ASSERTION  0x0004

// ACE framework error code : fwk launch error
#define EXCE_ACE_INVALID_APP_PATH  0x0001
#define EXCE_ACE_INVALID_BUNDLE_NAME  0x0002
#define EXCE_ACE_INIT_FWK_FAILED  0x0003
#define EXCE_ACE_APP_ENTRY_MISSING  0x0004
#define EXCE_ACE_APP_ENTRY_FILE_TOO_HUGE  0x0005
#define EXCE_ACE_APP_ALREADY_LAUNCHED  0x0006
#define EXCE_ACE_APP_INDEX_MISSING  0x0007
#define EXCE_ACE_APP_FILE_TOO_HUGE  0x0008
#define EXCE_ACE_APP_FILE_READ_FAILED  0x0009
#define EXCE_ACE_APP_JS_EVAL_FAILED  0x000A
#define EXCE_ACE_APP_NO_RENDER_FUNC  0x000B
#define EXCE_ACE_APP_RENDER_FAILED  0x000C

// ACE framework error code : router replace
#define EXCE_ACE_ROUTER_REPLACE_BIND_URI_FAILED  0x0001
#define EXCE_ACE_PAGE_INDEX_MISSING  0x0002
#define EXCE_ACE_PAGE_FILE_TOO_HUGE  0x0003
#define EXCE_ACE_PAGE_FILE_READ_FAILED  0x0004
#define EXCE_ACE_PAGE_JS_EVAL_FAILED  0x0005
#define EXCE_ACE_PAGE_NO_RENDER_FUNC  0x0006
#define EXCE_ACE_PAGE_RENDER_FAILED  0x0007

// ACE framework error code : async work
#define EXCE_ACE_ASYNC_WORK_DISPATCH_FAILED  0x0001

// ACE framework error code : localization
#define EXCE_ACE_LOCALIZATION_SYSTEM_LANGUAGE_NOT_INITIALIZED  0x0001
#define EXCE_ACE_LOCALIZATION_CJSON_CREATE_FAILED  0x0002
#define EXCE_ACE_LOCALIZATION_VALUE_FAILED  0x0003

#endif // OHOS_ACELITE_ACE_EVENT_ID_H
