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

#include "frameworks/bridge/declarative_frontend/jsview/js_interactable_view.h"

namespace OHOS::Ace::Framework {

template<typename T>
void JSInteractableView::JSBindCommonEvent()
{
    JSClass<T>::CustomMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<T>::CustomMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<T>::CustomMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<T>::CustomMethod("onKey", &JSInteractableView::JsOnKey);
}

} // namespace OHOS::Ace::Framework
