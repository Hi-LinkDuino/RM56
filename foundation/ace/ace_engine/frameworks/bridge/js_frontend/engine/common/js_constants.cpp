/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"

namespace OHOS::Ace::Framework {

const int32_t JS_CALL_FAIL = -1;
const int32_t JS_CALL_SUCCESS = 0;
// To solve the problem of stack overflow when qjs runs JS_Eval() after the context is generated.
const int32_t MAX_STACK_SIZE = -1;

// for pc preview
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
const char PC_PREVIEW[] = "enable";
#else
const char PC_PREVIEW[] = "disable";
#endif

// for common
const char COMMON_SUCCESS[] = "success";
const char COMMON_FAIL[] = "fail";
const char COMMON_COMPLETE[] = "complete";
const char COMMON_CANCEL[] = "cancel";

// for page route
const char ROUTE_PAGE_PUSH[] = "push";
const char ROUTE_PAGE_REPLACE[] = "replace";
const char ROUTE_PAGE_BACK[] = "back";
const char ROUTE_PAGE_CLEAR[] = "clear";
const char ROUTE_PAGE_GET_LENGTH[] = "getLength";
const char ROUTE_PAGE_GET_STATE[] = "getState";
const char ROUTE_PAGE_GET_PARAMS[] = "getParams";
const char ROUTE_KEY_URI[] = "uri";
const char ROUTE_KEY_PATH[] = "path";
const char ROUTE_KEY_PARAMS[] = "params";
const char ROUTE_KEY_DONT_OVERWRITE[] = "dontOverwrite";
const char ROUTE_ENABLE_ALERT_BEFORE_BACK_PAGE[] = "enableAlertBeforeBackPage";
const char ROUTE_DISABLE_ALERT_BEFORE_BACK_PAGE[] = "disableAlertBeforeBackPage";
const char ROUTE_POSTPONE[] = "postpone";
const char ROUTE_LAUNCH[] = "launch";

// for prompt
const char PROMPT_SHOW_TOAST[] = "showToast";
const char PROMPT_KEY_MESSAGE[] = "message";
const char PROMPT_KEY_DURATION[] = "duration";
const char PROMPT_KEY_BOTTOM[] = "bottom";
const char PROMPT_SHOW_DIALOG[] = "showDialog";
const char PROMPT_KEY_TITLE[] = "title";
const char PROMPT_KEY_BUTTONS[] = "buttons";
const char PROMPT_DIALOG_AUTO_CANCEL[] = "autocancel";
const char PROMPT_SHOW_ACTION_MENU[] = "showActionMenu";

// for callback
const char APP_DESTROY_FINISH[] = "appDestroyFinish";
const char CALLBACK_NATIVE[] = "callbackNative";
const char KEY_STEPPER_PENDING_INDEX[] = "pendingIndex";

// for configuration
const char CONFIGURATION_GET_LOCALE[] = "getLocale";
const char CONFIGURATION_SET_LOCALE[] = "setLocale";
const char LOCALE_LANGUAGE[] = "language";
const char LOCALE_COUNTRY_OR_REGION[] = "countryOrRegion";
const char LOCALE_UNICODE_SETTING[] = "unicodeSetting";
const char LOCALE_TEXT_DIR[] = "dir";
const char LOCALE_TEXT_DIR_LTR[] = "ltr";
const char LOCALE_TEXT_DIR_RTL[] = "rtl";

// for mediaquery
const char ADD_LISTENER[] = "addListener";
const char GET_DEVICE_TYPE[] = "getDeviceType";

// for timer
const char SET_TIMEOUT[] = "setTimeout";
const char CLEAR_TIMEOUT[] = "clearTimeout";
const char CLEAR_INTERVAL[] = "clearInterval";
const char SET_INTERVAL[] = "setInterval";
const char CAN_IUSE[] = "canIUse";

// for app
const char APP_GET_INFO[] = "getInfo";
const char APP_TERMINATE[] = "terminate";
const char APP_GET_PACKAGE_INFO[] = "getPackageInfo";
const char APP_PACKAGE_NAME[] = "packageName";
const char APP_REQUEST_FULL_WINDOW[] = "requestFullWindow";
const char APP_SCREEN_ON_VISIBLE[] = "screenOnVisible";
const char APP_SET_SWIPE_TO_DISMISS[] = "setSwipeToDismiss";
const char APP_REQUEST_FULL_WINDOW_DURATION[] = "duration";
const char APP_SCREEN_ON_VISIBLE_FLAG[] = "visible";
const char APP_SET_IMAGE_CACHE_COUNT[] = "setImageCacheCount";
const char APP_SET_IMAGE_RAWDATA_CACHE_SIZE[] = "setImageRawDataCacheSize";
const char APP_SET_IMAGE_FILE_CACHE_SIZE[] = "setImageFileCacheSize";

// for animation
const char ANIMATION_REQUEST_ANIMATION_FRAME[] = "requestAnimationFrame";
const char ANIMATION_CANCEL_ANIMATION_FRAME[] = "cancelAnimationFrame";

// for animator
const char ANIMATOR_CREATE_ANIMATOR[] = "createAnimator";

// for curves
const char CURVES_INIT[] = "init";
const char CURVES_CUBIC_BEZIER[] = "cubicBezier";
const char CURVES_SPRING[] = "spring";
const char CURVES_STEPS[] = "steps";
const char CURVE_INTERPOLATE[] = "interpolate";

// for grid
const char GRID_GET_SYSTEM_LAYOUT_INFO[] = "getSystemLayoutInfo";
// for plugin
const int32_t PLUGIN_REQUEST_SUCCESS = 0;
const int32_t PLUGIN_REQUEST_FAIL = 200;
const int32_t PLUGIN_CALLBACK_DESTROY = 3;

// for offscreenCanvas
const char OFFSCREEN_CANVAS_CREATE[] = "create";

// for readResource
const char READ_TEXT[] = "readText";
const char READ_ARRAY_BUFFER[] = "readArrayBuffer";
const char READ_KEY_URI[] = "uri";
const char READ_KEY_POSITION[] = "position";
const char READ_KEY_LENGTH[] = "length";

// for matrix
const char MATRIX_INIT[] = "init";
const char MATRIX_IDENTITY[] = "identity";
const char MATRIX_COPY[] = "copy";
const char MATRIX_COMBINE[] = "combine";
const char MATRIX_INVERT[] = "invert";
const char MATRIX_TRANSLATE[] = "translate";
const char MATRIX_SCALE[] = "scale";
const char MATRIX_ROTATE[] = "rotate";
const char MATRIX_TRANSFORM_POINT[] = "transformPoint";
const char MATRIX_4X4[] = "matrix4x4";
} // namespace OHOS::Ace::Framework
