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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_RUNTIME_CONSTANTS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_RUNTIME_CONSTANTS_H

#include <cstdint>

namespace OHOS::Ace::Framework {

constexpr int32_t DEFAULT_APP_ID = 10002;
constexpr int32_t FRAGMENT_SIZE = 100;

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
constexpr char JS_LOG_TAG[] = "Console";
#else
constexpr char JS_LOG_TAG[] = "JSApp";
#endif

enum class JsLogLevel : int32_t {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
};

// JsHandleCallback index
constexpr uint32_t PROPERTIES_ARGS_LEN = 2;
constexpr uint32_t PROPERTIES_ARGS_CBID_IDX = 0;
constexpr uint32_t PROPERTIES_ARGS_RST_IDX = 1;

// JsDomCreateBody index
constexpr uint32_t CREATE_BODY_ARGS_LEN = 5;
constexpr uint32_t CREATE_BODY_TAG_IDX = 1;
constexpr uint32_t CREATE_BODY_ATTR_IDX = 2;
constexpr uint32_t CREATE_BODY_STYLE_IDX = 3;
constexpr uint32_t CREATE_BODY_EVENT_IDX = 4;

// JsDomAddElement index
constexpr uint32_t ADD_ELEMENT_ARGS_LEN = 9;
constexpr uint32_t ADD_ELEMENT_PARID_IDX = 0;
constexpr uint32_t ADD_ELEMENT_NODEID_IDX = 1;
constexpr uint32_t ADD_ELEMENT_TAG_IDX = 2;
constexpr uint32_t ADD_ELEMENT_ATTR_IDX = 3;
constexpr uint32_t ADD_ELEMENT_STYLE_IDX = 4;
constexpr uint32_t ADD_ELEMENT_EVENT_INDEX = 5;
constexpr uint32_t ADD_ELEMENT_CUSTOM_INDEX = 6;
constexpr uint32_t ADD_ELEMENT_ITEM_INDEX = 7;
constexpr uint32_t ADD_ELEMENT_INSTANCE_ID = 8;

// JsUpdateElement index
constexpr uint32_t UPLOAD_ELEMENT_ARGS_LEN = 3;
constexpr uint32_t UPLOAD_ELEMENT_NID_IDX = 0;
constexpr uint32_t UPLOAD_ELEMENT_DOM_IDX = 1;
constexpr uint32_t UPLOAD_ELEMENT_INSTANCE_ID = 2;

// JsRemoveElement index
constexpr uint32_t REMOVE_ELEMENT_ARGS_LEN = 2;
constexpr uint32_t REMOVE_ELEMENT_ID_IDX = 0;
constexpr uint32_t REMOVE_ELEMENT_INSTANCE_ID = 1;

// JsCallNative index
constexpr uint32_t NATIVE_ARGS_LEN = 2;
constexpr uint32_t NATIVE_ARGS_METHOD_IDX = 0;
constexpr uint32_t NATIVE_ARGS_IDX = 1;

// JsCallComponent index
constexpr uint32_t COMPONENT_ARGS_LEN = 3;
constexpr uint32_t COMPONENT_ARGS_ID_IDX = 0;
constexpr uint32_t COMPONENT_ARGS_METHOD_IDX = 1;
constexpr uint32_t COMPONENT_ARGS_IDX = 2;

// JsPerf index
constexpr uint32_t PERF_ARGS_LEN = 1;
constexpr uint32_t PERF_ARGS_METHOD_IDX = 0;

// JsHiViewReport index
constexpr uint32_t HIVIEW_ARGS_LEN = 2;
constexpr uint32_t HIVIEW_ARGS_ID_IDX = 0;
constexpr uint32_t HIVIEW_ARGS_JSON_IDX = 1;

// GetPackageInfo index
constexpr uint32_t PAG_INFO_ARGS_LEN = 2;
constexpr uint32_t PAG_INFO_ARGS_MSG_IDX = 0;
constexpr uint32_t PAG_INFO_ARGS_ID_IDX = 1;

constexpr uint32_t URL_SOURCE_START_IDX = 5;
constexpr uint32_t URL_SOURCE_SUFFIX_LEN = 7;
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_COMMON_RUNTIME_CONSTANTS_H
