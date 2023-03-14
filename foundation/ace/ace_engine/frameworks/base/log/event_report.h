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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_EVENT_REPORT_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_EVENT_REPORT_H

#include <string>

#include "base/utils/macros.h"

namespace OHOS::Ace {

const std::string EXCEPTION_FRAMEWORK_APP_START = "FRAMEWORK_APP_START_EXCEPTION";
const std::string EXCEPTION_FRAMEWORK_PAGE_ROUTER = "FRAMEWORK_PAGE_ROUTER_EXCEPTION";
const std::string EXCEPTION_COMPONENT = "COMPONENT_EXCEPTION";
const std::string EXCEPTION_API_CHANNEL = "API_CHANNEL_EXCEPTION";
const std::string EXCEPTION_RENDER = "RENDER_EXCEPTION";
const std::string EXCEPTION_JS = "JS_EXCEPTION";
const std::string EXCEPTION_ANIMATION = "ANIMATION_EXCEPTION";
const std::string EXCEPTION_EVENT = "EVENT_EXCEPTION";
const std::string EXCEPTION_INTERNATIONALIZATION = "INTERNATIONALIZATION_EXCEPTION";
const std::string EXCEPTION_ACCESSIBILITY = "ACCESSIBILITY_EXCEPTION";
const std::string EXCEPTION_FORM = "FORM_EXCEPTION";

// EXCEPTION_FRAMEWORK_APP_START
enum class AppStartExcepType {
    CREATE_CONTAINER_ERR = 1,
    SET_VIEW_ERR,
    VIEW_TYPE_ERR,
    GET_PACKAGE_PATH_ERR,
    JNI_CLASS_ERR,
    JNI_INIT_ERR = 10,
    FRONTEND_TYPE_ERR,
    PIPELINE_CONTEXT_ERR,
    VIEW_STATE_ERR,
    RESOURCE_REGISTER_INIT_ERR,
    JS_ENGINE_CREATE_ERR,
    JAVA_EVENT_CALLBACK_INIT_ERR,
};

// EXCEPTION_FRAMEWORK_PAGE_ROUTER
enum class PageRouterExcepType {
    ROUTE_PARSE_ERR = 0,
    PAGE_STACK_OVERFLOW_ERR,
    RUN_PAGE_ERR,
    UPDATE_PAGE_ERR,
    LOAD_PAGE_ERR,
    REPLACE_PAGE_ERR,
};

// EXCEPTION_COMPONENT
enum class ComponentExcepType {
    TEXT_INPUT_CONNECTION_CLOSE_ERR = 0,
    GET_THEME_ERR,
    BUTTON_COMPONENT_ERR,
    DIALOG_EVENT_ERR,
    DOM_NODE_NOT_FOUND,
    SET_ROOT_DOM_NODE_ERR,
    IMAGE_ANIMATOR_ERR,
    LIST_COMPONENT_ERR,
    LIST_ITEM_ERR,
    MARQUEE_ERR,
    NAVIGATION_BAR_ERR,
};

// EXCEPTION_API_CHANNEL
enum class APIChannelExcepType {
    JS_BRIDGE_INIT_ERR = 0,
    SET_FUNCTION_ERR,
};

// EXCEPTION_RENDER
enum class RenderExcepType {
    VIEW_SCALE_ERR = 0,
    RENDER_ANIMATION_ERR,
    RENDER_COMPONENT_ERR,
    CLIP_ERR,
    UI_THREAD_STUCK,
};

// EXCEPTION_JS
enum class JsExcepType {
    GET_NODE_ERR = 0,
    CREATE_NODE_ERR,
    CREATE_DOM_BODY_ERR,
    REMOVE_DOM_ELEMENT_ERR,
    UPDATE_DOM_ELEMENT_ERR,
    JS_ENGINE_INIT_ERR,
    JS_RUNTIME_OBJ_ERR,
    JS_CONTEXT_INIT_ERR,
    JS_THREAD_STUCK,
};

// EXCEPTION_ANIMATION
enum class AnimationExcepType {
    ANIMATION_BRIDGE_ERR = 0,
    ANIMATION_PAGE_ERR,
};

// EXCEPTION_EVENT
enum class EventExcepType {
    FIRE_EVENT_ERR = 1,
};

// EXCEPTION_INTERNATIONALIZATION
enum class InternalExcepType {
    CHANGE_LOCALE_ERR = 0,
};

// EXCEPTION_ACCESSIBILITY
enum class AccessibilityExcepType {
    CREATE_ACCESSIBILITY_NODE_ERR = 0,
    GET_NODE_ERR,
};

// EXCEPTION_FORM
enum class FormExcepType {
    RUN_PAGE_ERR = 0,
    LOAD_PAGE_ERR,
    CREATE_NODE_ERR,
    UPDATE_PAGE_ERR,
    FIRE_FORM_EVENT_ERR,
    ACTION_EVENT_CALLBACK_ERR,
};

enum class RawEventType { WARNING, FREEZE, RECOVER };

struct EventInfo {
    std::string eventType;
    int32_t errorType = 0;
    std::string pageUrl;
};

class ACE_EXPORT EventReport {
public:
    static void SendEvent(const EventInfo& eventInfo);
    static void SendJsCardRenderTimeEvent(
        const std::string& sessionID,
        const std::string& timeType,
        uint64_t timeDelay);

    static void SendAppStartException(AppStartExcepType type);
    static void SendPageRouterException(PageRouterExcepType type, const std::string& pageUrl = "");
    static void SendComponentException(ComponentExcepType type);
    static void SendAPIChannelException(APIChannelExcepType type);
    static void SendRenderException(RenderExcepType type);
    static void SendJsException(JsExcepType type);
    static void SendAnimationException(AnimationExcepType type);
    static void SendEventException(EventExcepType type);
    static void SendInternalException(InternalExcepType type);
    static void SendAccessibilityException(AccessibilityExcepType type);
    static void SendFormException(FormExcepType type);

    static void JsEventReport(int32_t eventType, const std::string& jsonStr);
    static void JsErrReport(
        const std::string& packageName, const std::string& reason, const std::string& summary);
    static void ANRRawReport(RawEventType type, int32_t uid, const std::string& packageName,
        const std::string& processName, const std::string& msg = " ");
    static void ANRShowDialog(int32_t uid, const std::string& packageName,
        const std::string& processName, const std::string& msg = "");

private:
    static void SendEventInner(const EventInfo& eventInfo);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_EVENT_REPORT_H
