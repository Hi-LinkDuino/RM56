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

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)
#include <dlfcn.h>
#endif

#include "base/image/pixel_map.h"
#include "base/log/ace_trace.h"
#include "frameworks/bridge/common/utils/engine_helper.h"
#include "frameworks/bridge/declarative_frontend/engine/js_ref_ptr.h"
#include "frameworks/bridge/declarative_frontend/jsview/js_utils.h"
#include "frameworks/bridge/declarative_frontend/view_stack_processor.h"
#include "frameworks/bridge/js_frontend/engine/common/js_engine.h"

namespace OHOS::Ace::Framework {
#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM)

RefPtr<PixelMap> CreatePixelMapFromNapiValue(JSRef<JSVal> obj)
{
    if (!obj->IsObject()) {
        LOGE("info[0] is not an object when try CreatePixelMapFromNapiValue");
        return nullptr;
    }
    auto engine = EngineHelper::GetCurrentEngine();
    if (!engine) {
        LOGE("CreatePixelMapFromNapiValue engine is null");
        return nullptr;
    }
    auto nativeEngine = engine->GetNativeEngine();
    if (nativeEngine == nullptr) {
        LOGE("nativeEngine is nullptr.");
        return nullptr;
    }
#ifdef USE_V8_ENGINE
    v8::Local<v8::Value> value = obj->operator v8::Local<v8::Value>();
#elif USE_QUICKJS_ENGINE
    JSValue value = obj.Get().GetHandle();
#elif USE_ARK_ENGINE
    panda::Local<JsiValue> value = obj.Get().GetHandle();
#endif
    JSValueWrapper valueWrapper = value;
    NativeValue* nativeValue = nativeEngine->ValueToNativeValue(valueWrapper);

    PixelMapNapiEntry pixelMapNapiEntry = JsEngine::GetPixelMapNapiEntry();
    if (!pixelMapNapiEntry) {
        LOGE("pixelMapNapiEntry is null");
        return nullptr;
    }

    void* pixmapPtrAddr = pixelMapNapiEntry(
        reinterpret_cast<napi_env>(nativeEngine), reinterpret_cast<napi_value>(nativeValue));
    if (pixmapPtrAddr == nullptr) {
        LOGE("Failed to get pixmap pointer");
        return nullptr;
    }
    return PixelMap::CreatePixelMap(pixmapPtrAddr);
}
#endif
} // namespace OHOS::Ace::Framework
