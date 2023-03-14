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

#include "bridge/declarative_frontend/jsview/js_canvas_image_data.h"
#include "bridge/declarative_frontend/jsview/js_rendering_context.h"
#include "bridge/declarative_frontend/jsview/js_view_common_def.h"

namespace OHOS::Ace::Framework {

JSCanvasImageData::JSCanvasImageData()
{
}

void JSCanvasImageData::Constructor(const JSCallbackInfo& args)
{
    auto jsCanvasImageData = Referenced::MakeRefPtr<JSCanvasImageData>();
    jsCanvasImageData->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(jsCanvasImageData));
}

void JSCanvasImageData::Destructor(JSCanvasImageData* controller)
{
    if (controller != nullptr) {
        controller->DecRefCount();
    }
}

void JSCanvasImageData::JSBind(BindingTarget globalObj)
{
    JSClass<JSCanvasImageData>::Declare("ImageData");
    JSClass<JSCanvasImageData>::CustomMethod("data", &JSCanvasImageData::getData);
    JSClass<JSCanvasImageData>::Bind(globalObj, JSCanvasImageData::Constructor, JSCanvasImageData::Destructor);
}

void JSCanvasImageData::getData(const JSCallbackInfo& info)
{
    auto returnValue = JSVal(ToJSValue(data_));
    auto returnPtr = JSRef<JSVal>::Make(returnValue);
    info.SetReturnValue(returnPtr);
}

} // namespace OHOS::Ace::Framework