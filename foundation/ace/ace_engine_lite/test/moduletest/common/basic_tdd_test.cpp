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
#include "basic_tdd_test.h"
#include <cstring>
#include <sys/prctl.h>
#include "async_task_manager.h"
#include "common/graphic_startup.h"
#include "common/task_manager.h"
#include "component_utils.h"
#include "core/render_manager.h"
#include "descriptor_utils.h"
#include "dock/screen_device_proxy.h"
#include "event_injector.h"
#include "js_app_context.h"
#include "js_app_environment.h"
#include "module_manager.h"
#include "root_view.h"

namespace OHOS {
namespace ACELite {
BasicTddTest::BasicTddTest() : rootComponent_(nullptr) {}
BasicTddTest::~BasicTddTest() {}

ScreenDevice *BasicTddTest::display_ = nullptr;
Window *BasicTddTest::window_ = nullptr;
pthread_t BasicTddTest::tickThread_ = -1;
bool BasicTddTest::isTickTaskRunning_ = false;

// 10ms a tick
constexpr uint16_t TICK = 10000;
constexpr uint32_t SWIPE_TIME = 100;

constexpr uint8_t NUMBER_TWO = 2;

void BasicTddTest::SetUpTestCase(void)
{
    GraphicStartUp::Init();
    isTickTaskRunning_ = true;

    display_ = new ScreenDevice();
    ScreenDeviceProxy::GetInstance()->SetDevice(display_);
    ScreenDeviceProxy::GetInstance()->SetScreenSize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);

    WindowConfig config = {};
    uint16_t width = GetHorizontalResolution();
    uint16_t height = GetVerticalResolution();
    config.rect.SetRect(0, 0, width - 1, height - 1);
    window_ = Window::CreateWindow(config);
    RootView *rootView = RootView::GetInstance();
    rootView->SetPosition(0, 0);
    rootView->SetWidth(width);
    rootView->SetHeight(height);
    window_->BindRootView(rootView);
    TaskManager::GetInstance()->Remove(&RenderManager::GetInstance());
    if (pthread_create(&tickThread_, nullptr, TickHandler, nullptr) == 0) {
        pthread_detach(tickThread_);
        HILOG_DEBUG(HILOG_MODULE_ACE, "[BasicTddTest::SetUpTestCase]: Success to fork tick thread.");
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE, "[BasicTddTest::SetUpTestCase]: Failed to fork tick thread.");
    }
}

void BasicTddTest::TearDownTestCase(void)
{
    isTickTaskRunning_ = false;
    if (window_ != nullptr) {
        Window::DestoryWindow(window_);
        window_ = nullptr;
    }
    if (display_ != nullptr) {
        delete display_;
        display_ = nullptr;
    }
}

void BasicTddTest::SetUp()
{
    JsAppContext *appContext = JsAppContext::GetInstance();
    const int32_t apiVersion = 6;
    appContext->SetCompatibleApi(apiVersion);
    appContext->SetTargetApi(apiVersion);
    JsAppEnvironment::GetInstance()->InitJsFramework();
    EventInjector *injector = EventInjector::GetInstance();
    if (!injector->IsEventInjectorRegistered(EventDataType::POINT_TYPE)) {
        injector->RegisterEventInjector(EventDataType::POINT_TYPE);
    }
    injector->SetWindowId(window_->GetWindowId());
    AsyncTaskManager::GetInstance().SetFront(true);
}

void BasicTddTest::TearDown()
{
    ModuleManager::GetInstance()->CleanUpModule();
    JsAppEnvironment::GetInstance()->Cleanup();

    EventInjector *injector = EventInjector::GetInstance();
    if (injector->IsEventInjectorRegistered(EventDataType::POINT_TYPE)) {
        injector->UnregisterEventInjector(EventDataType::POINT_TYPE);
    }
    AsyncTaskManager::GetInstance().SetFront(false);
}

JSValue BasicTddTest::RunJS(const char *bundle, const uint32_t size) const
{
    return jerry_eval(reinterpret_cast<const jerry_char_t *>(bundle), size, JERRY_PARSE_NO_OPTS);
}

JSValue BasicTddTest::CreatePage(const char *bundle, const uint32_t size)
{
    JSValue page = RunJS(bundle, size);
    if (JSError::Is(page)) {
        return JSUndefined::Create();
    }
    JSGlobal::Set("$root", page);
    CallLifecycle(page, "onInit");
    JSValue rootEl = JSObject::Call(page, "$render");
    if (JSUndefined::Is(rootEl) || JSError::Is(rootEl)) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[BasicTddTest::CreatePage]: Failed to render page.");
        JSRelease(rootEl);
        JSRelease(page);
        return JSUndefined::Create();
    }
    CallLifecycle(page, "onReady");
    rootComponent_ = ComponentUtils::GetComponentFromBindingObject(rootEl);
    if (rootComponent_ == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[BasicTddTest::CreatePage]: Failed to get root component.");
        JSRelease(rootEl);
        JSRelease(page);
        return JSUndefined::Create();
    }
    uint16_t horizontalResolution = GetHorizontalResolution();
    uint16_t verticalResolution = GetVerticalResolution();
    ConstrainedParameter rootViewParam(horizontalResolution, verticalResolution);
    Component::BuildViewTree(rootComponent_, nullptr, rootViewParam);
    rootComponent_->OnViewAttached();
    UIView *view = rootComponent_->GetComponentRootView();
    if (view == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[BasicTddTest::CreatePage]: Failed to get page root view.");
        JSRelease(rootEl);
        JSRelease(page);
        return JSUndefined::Create();
    }
    RootView *rootView = RootView::GetInstance();
    rootView->Add(view);
    rootView->Invalidate();
    CallLifecycle(page, "onShow");
    return page;
}

void BasicTddTest::DestroyPage(JSValue page)
{
    RootView *rootView = RootView::GetInstance();
    rootView->RemoveAll();
    rootView->Invalidate();
    CallLifecycle(page, "onHide");
    CallLifecycle(page, "onDestroy");
    ComponentUtils::ReleaseComponents(rootComponent_);
    rootComponent_ = nullptr;
    JsAppContext::GetInstance()->ReleaseStyles();
    JSGlobal::Del("$root");
    JSRelease(page);
}

UIView *BasicTddTest::GetViewByRef(JSValue page, const char *ref) const
{
    JSValue refs = JSObject::Get(page, "$refs");
    if (JSUndefined::Is(refs)) {
        JSRelease(refs);
        return nullptr;
    }
    JSValue dom = JSObject::Get(refs, ref);
    if (JSUndefined::Is(dom)) {
        JSRelease(refs);
        JSRelease(dom);
        return nullptr;
    }
    UIView *view = ComponentUtils::GetViewFromBindingObject(dom);
    JSRelease(refs);
    JSRelease(dom);
    return view;
}

void BasicTddTest::ClickByRef(JSValue page, const char *ref, uint8_t sleepTicks) const
{
    return Click(GetViewByRef(page, ref), sleepTicks);
}

void BasicTddTest::LongPressByRef(JSValue page, const char *ref, uint8_t sleepTicks) const
{
    return LongPress(GetViewByRef(page, ref), sleepTicks);
}

void BasicTddTest::Click(const UIView *view, uint8_t sleepTicks) const
{
    if (view == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[BasicTddTest::Click]: Failed to click because view is nullptr");
        return;
    }
    Rect rect = view->GetOrigRect();
    int16_t x = rect.GetLeft() + rect.GetWidth() / NUMBER_TWO;
    int16_t y = rect.GetTop() + rect.GetHeight() / NUMBER_TWO;
    return Click(x, y, sleepTicks);
}

void BasicTddTest::LongPress(const UIView *view, uint8_t sleepTicks) const
{
    if (view == nullptr) {
        HILOG_ERROR(HILOG_MODULE_ACE, "[BasicTddTest::LongPress]: Failed to long press because view is nullptr");
        return;
    }
    Rect rect = view->GetOrigRect();
    int16_t x = rect.GetLeft() + rect.GetWidth() / NUMBER_TWO;
    int16_t y = rect.GetTop() + rect.GetHeight() / NUMBER_TWO;
    return LongPress(x, y, sleepTicks);
}

void BasicTddTest::Click(int16_t x, int16_t y, uint8_t sleepTicks) const
{
    Point point = {x, y};
    EventInjector *injector = EventInjector::GetInstance();
    if (injector->SetClickEvent(point)) {
        HILOG_DEBUG(HILOG_MODULE_ACE,
                    "[BasicTddTest::Click]: Success to inject CLICK event on (%{public}d, %{public}d)\n", x, y);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[BasicTddTest::Click]: Failed to inject CLICK event on (%{public}d, %{public}d)\n", x, y);
    }
    // Waiting for tick breath.
    usleep(sleepTicks * TICK);
}

void BasicTddTest::LongPress(int16_t x, int16_t y, uint8_t sleepTicks) const
{
    Point point = {x, y};
    EventInjector *injector = EventInjector::GetInstance();
    if (injector->SetLongPressEvent(point)) {
        HILOG_DEBUG(HILOG_MODULE_ACE,
                    "[BasicTddTest::LongPress]: Success to inject LONGPRESS event on (%{public}d, %{public}d)\n", x, y);
    } else {
        HILOG_ERROR(HILOG_MODULE_ACE,
                    "[BasicTddTest::LongPress]: Failed to inject LONGPRESS event on (%{public}d, %{public}d)\n", x, y);
    }
    // Waiting for tick breath.
    usleep(sleepTicks * TICK);
}

void BasicTddTest::Swipe(int16_t startX, int16_t startY, int16_t endX, int16_t endY, uint8_t sleepTicks) const
{
    Point startPoint = {startX, startY};
    Point endPoint = {endX, endY};
    EventInjector *injector = EventInjector::GetInstance();
    if (injector->SetDragEvent(startPoint, endPoint, SWIPE_TIME)) {
        HILOG_DEBUG(HILOG_MODULE_ACE,
                    "[BasicTddTest::Swipe]: Success to inject SWIPE event from (%{public}d, %{public}d) to "
                    "(%{public}d, %{public}d)\n",
                    startX, startY, endX, endY);
    } else {
        HILOG_ERROR(
            HILOG_MODULE_ACE,
            "[BasicTddTest::Swipe]: Failed to inject SWIPE from (%{public}d, %{public}d) to (%{public}d, %{public}d)\n",
            startX, startY, endX, endY);
    }

    // Waiting for tick breath.
    usleep(sleepTicks * TICK);
}

uint8_t BasicTddTest::GetChildrenSize(const UIViewGroup &grop) const
{
    UIView *child = grop.GetChildrenHead();
    uint8_t size = 0;
    while (child != nullptr) {
        ++size;
        child = child->GetNextSibling();
    }
    return size;
}

void *BasicTddTest::TickHandler(void *args)
{
    (void)args;
    prctl(PR_SET_NAME, "TickTask");
    while (isTickTaskRunning_) {
        // Periodically call TaskHandler(). It could be done in a timer interrupt or an OS task too.
        TaskManager::GetInstance()->TaskHandler();
        // Just to let the system breathe
        usleep(TICK);
    }
    return nullptr;
}

void BasicTddTest::CallLifecycle(JSValue page, const char *lifecycle) const
{
    JSValue func = JSObject::Get(page, lifecycle);
    if (JSFunction::Is(func)) {
        JSRelease(JSFunction::Call(func, page, nullptr, 0));
    }
    JSRelease(func);
}
} // namespace ACELite
} // namespace OHOS
