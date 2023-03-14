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

#include "product_adapter.h"

#include "acelite_config.h"
#include "graphic_config.h"
#include "js_async_work.h"
#include "message_queue_utils.h"
#include "module_manager.h"
#include "securec.h"

namespace OHOS {
namespace ACELite {
/**
 * Used for holding all the related dfx interfaces assigned from specific implementation.
 */
struct DFXWrapper {
    DFXWrapper()
        : eventTag_(0),
          eventSubTag_(0),
          errCodeTag_(0),
          errCodeSubTag_(0),
          eventPrintHandler_(nullptr),
          errCodePrintHandler_(nullptr),
          jsLogOutputHandler_(nullptr),
          nativeMemInfoGetter_(nullptr)
    {
    }
    uint8_t eventTag_;
    uint8_t eventSubTag_;
    uint8_t errCodeTag_;
    uint8_t errCodeSubTag_;
    EventPrintHandler eventPrintHandler_;
    ErrCodePrintHandler errCodePrintHandler_;
    JSLogOutputHandler jsLogOutputHandler_;
    NativeMemInfoGetter nativeMemInfoGetter_;
};

static DFXWrapper g_dfxWrapper;
static TEHandlingHooks g_teHandlingHooks = {nullptr, nullptr};
static TerminateAbilityHandler g_termiantingHandler = nullptr;
static SetScreenOnVisibleHandler g_setScreenOnHandler = nullptr;
static ExtraPresetModulesHook g_extraPresetModulesHooks = {nullptr, nullptr};
// default font styles
static char *g_defaultFontFamilyName = nullptr;
static uint8_t g_defaultFontSize = 30;
static uint16_t g_screenWidth = 454;
static uint16_t g_screenHeight = 454;

// default app private data root path
const static char *DEFAULT_APP_DATA_PATH = "user/ace/data/";
static const char *g_defaultDataRootPath = DEFAULT_APP_DATA_PATH;

// default device info
const static uint8_t DEVICE_TYPE_STR_LEN = 24;
const static char *DEFAULT_DEVICE_TYPE_NAME = "smartVision";
// smartVision as default
static const char *g_deviceType = DEFAULT_DEVICE_TYPE_NAME;

// indicating if the ace application is on forground
static bool g_isRenderTickAcceptable = false;

void ProductAdapter::InitAceTags(uint8_t *aceTags, uint8_t tagCount)
{
    const uint8_t minCount = 4;
    if (aceTags == nullptr || tagCount < minCount) {
        return;
    }
    uint8_t index = 0;
    g_dfxWrapper.eventTag_ = aceTags[index++];
    g_dfxWrapper.eventSubTag_ = aceTags[index++];
    g_dfxWrapper.errCodeTag_ = aceTags[index++];
    g_dfxWrapper.errCodeSubTag_ = aceTags[index++];
}

void ProductAdapter::InitTraceHandlers(EventPrintHandler eventHandler, ErrCodePrintHandler errCodeHandler)
{
    g_dfxWrapper.eventPrintHandler_ = eventHandler;
    g_dfxWrapper.errCodePrintHandler_ = errCodeHandler;
}

void ProductAdapter::InitConsoleNativeHandler(JSLogOutputHandler handler)
{
    g_dfxWrapper.jsLogOutputHandler_ = handler;
}

void ProductAdapter::InitNativeMemPoolHook(NativeMemInfoGetter getter)
{
    g_dfxWrapper.nativeMemInfoGetter_ = getter;
}

void ProductAdapter::InitExtraModulesGetter(ProductModulesGetter productModuleGetter,
                                            PrivateModulesGetter privateModuleGetter)
{
    ModuleManager::GetInstance()->SetProductModulesGetter(productModuleGetter);
    ModuleManager::GetInstance()->SetPrivateModulesGetter(privateModuleGetter);
}

void ProductAdapter::PrintEventTrace(uint8_t info2, uint8_t info3, uint8_t info4)
{
    if (g_dfxWrapper.eventPrintHandler_ == nullptr || g_dfxWrapper.eventTag_ == 0 || g_dfxWrapper.eventSubTag_ == 0) {
        return;
    }

    uint8_t subTag = (info2 == 0) ? g_dfxWrapper.eventSubTag_ : info2;
    g_dfxWrapper.eventPrintHandler_(g_dfxWrapper.eventTag_, subTag, info3, info4);
}

void ProductAdapter::PrintErrCode(uint8_t info2, uint16_t rfu)
{
    if (g_dfxWrapper.errCodePrintHandler_ == nullptr || g_dfxWrapper.errCodeTag_ == 0 ||
        g_dfxWrapper.errCodeSubTag_ == 0) {
        return;
    }
    g_dfxWrapper.errCodePrintHandler_(g_dfxWrapper.errCodeTag_, g_dfxWrapper.errCodeSubTag_, info2, rfu);
}

void ProductAdapter::OutputJSConsoleLog(uint8_t level, const char *content)
{
    if (g_dfxWrapper.jsLogOutputHandler_ == nullptr) {
        return;
    }
    g_dfxWrapper.jsLogOutputHandler_(level, content);
}

void ProductAdapter::GetNativeMemInfo(NativeMemInfo *memInfo)
{
    if (g_dfxWrapper.nativeMemInfoGetter_ == nullptr) {
        return;
    }

    g_dfxWrapper.nativeMemInfoGetter_(memInfo);
}

void ProductAdapter::RegTerminatingHandler(TerminateAbilityHandler handler)
{
    g_termiantingHandler = handler;
}

void ProductAdapter::RegTEHandlers(const TEHandlingHooks &teHandlingHooks)
{
    g_teHandlingHooks.renderTEHandler = teHandlingHooks.renderTEHandler;
    g_teHandlingHooks.renderEndHandler = teHandlingHooks.renderEndHandler;
}

// NOTE: This TE function will be called in VSYNC interrupt, and
// as no any task can be switched to during an interrupt, so it's safe to
// read the global value directly here.
TEDispatchingResult ProductAdapter::DispatchTEMessage()
{
#if (OHOS_ACELITE_PRODUCT_WATCH == 1) // only some specific products support TE dispatching
    if (!g_isRenderTickAcceptable) {
        return TEDispatchingResult::REFUSED;
    }

    if (JsAsyncWork::DispatchToLoop(TE_EVENT, nullptr)) {
        return TEDispatchingResult::ACCEPTED;
    }
    // if the ACE application is on forground and the dispatching failed, should retry sending to ACE again
    return TEDispatchingResult::ACCEPT_FAILED;
#else
    return TEDispatchingResult::REFUSED;
#endif // OHOS_ACELITE_PRODUCT_WATCH
}

void ProductAdapter::SendTerminatingRequest(uint32_t token, bool forceStop)
{
    if (g_termiantingHandler != nullptr) {
        g_termiantingHandler(token, forceStop);
    }
}

bool ProductAdapter::IsTEHandlersRegisted()
{
    return (g_teHandlingHooks.renderTEHandler != nullptr);
}

void ProductAdapter::ProcessOneTE()
{
    if (g_teHandlingHooks.renderTEHandler != nullptr) {
        (void)(g_teHandlingHooks.renderTEHandler());
    }
}

void ProductAdapter::NotifyRenderEnd()
{
    if (g_teHandlingHooks.renderEndHandler != nullptr) {
        g_teHandlingHooks.renderEndHandler();
    }
}

void ProductAdapter::SetDefaultFontStyle(const char *defaultFontFamily, uint8_t defaultFontSize)
{
    g_defaultFontFamilyName = const_cast<char *>(defaultFontFamily);
    g_defaultFontSize = defaultFontSize;
}

const char *ProductAdapter::GetDefaultFontFamilyName()
{
    return (g_defaultFontFamilyName != nullptr) ? g_defaultFontFamilyName : DEFAULT_VECTOR_FONT_FILENAME;
}

uint8_t ProductAdapter::GetDefaultFontSize()
{
    return g_defaultFontSize;
}

void ProductAdapter::UpdateRenderTickAcceptable(bool acceptable)
{
    g_isRenderTickAcceptable = acceptable;
}

void ProductAdapter::SetScreenSize(uint16_t width, uint16_t height)
{
    g_screenWidth = width;
    g_screenHeight = height;
}

void ProductAdapter::GetScreenSize(uint16_t &width, uint16_t &height)
{
    width = g_screenWidth;
    height = g_screenHeight;
}

void ProductAdapter::RegSetScreenOnVisibleHandler(SetScreenOnVisibleHandler handler)
{
    g_setScreenOnHandler = handler;
}

bool ProductAdapter::SetScreenOnVisible(bool visible)
{
    return (g_setScreenOnHandler != nullptr) ? g_setScreenOnHandler(visible) : false;
}

void ProductAdapter::RegExtraPresetModulesHook(ExtraPresetModulesHook hook)
{
    g_extraPresetModulesHooks.loadingHandler = hook.loadingHandler;
    g_extraPresetModulesHooks.unloadingHandler = hook.unloadingHandler;
}

void ProductAdapter::LoadExtraPresetModules()
{
    if (g_extraPresetModulesHooks.loadingHandler != nullptr) {
        g_extraPresetModulesHooks.loadingHandler();
    }
}

void ProductAdapter::UnloadExtraPresetModules()
{
    if (g_extraPresetModulesHooks.unloadingHandler != nullptr) {
        g_extraPresetModulesHooks.unloadingHandler();
    }
}

void ProductAdapter::ConfigPrivateDataRootPath(const char *appDataRoot)
{
    if (appDataRoot == nullptr) {
        return;
    }
    size_t pathLen = strlen(appDataRoot);
    if (pathLen == 0 || pathLen >= UINT8_MAX) {
        return;
    }
    g_defaultDataRootPath = appDataRoot;
}

const char *ProductAdapter::GetPrivateDataRootPath()
{
    return g_defaultDataRootPath;
}

void ProductAdapter::InitDeviceInfo(const char *deviceType)
{
    if (deviceType == nullptr || (strlen(deviceType) == 0) || strlen(deviceType) >= DEVICE_TYPE_STR_LEN) {
        return;
    }
    g_deviceType = deviceType;
}

const char *ProductAdapter::GetDeviceType()
{
    return g_deviceType;
}
} // namespace ACELite
} // namespace OHOS
