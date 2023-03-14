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

#include "bridge/declarative_frontend/jsview/js_tabs_controller.h"

#include "bridge/declarative_frontend/engine/bindings.h"

namespace OHOS::Ace::Framework {
namespace {

int32_t g_tabControllerId = 0;

} // namespace

JSTabsController::JSTabsController()
{
    controller_ = CreateController();
}

void JSTabsController::JSBind(BindingTarget globalObj)
{
    JSClass<JSTabsController>::Declare("TabsController");
    JSClass<JSTabsController>::Method("changeIndex", &JSTabsController::ChangeIndex);
    JSClass<JSTabsController>::Bind(globalObj, JSTabsController::Constructor, JSTabsController::Destructor);
}

void JSTabsController::Constructor(const JSCallbackInfo& args)
{
    auto jsCalendarController = Referenced::MakeRefPtr<JSTabsController>();
    jsCalendarController->IncRefCount();
    args.SetReturnValue(Referenced::RawPtr(jsCalendarController));
}

void JSTabsController::Destructor(JSTabsController* controller)
{
    if (controller != nullptr) {
        controller->DecRefCount();
    }
}

RefPtr<TabController> JSTabsController::CreateController()
{
    return TabController::GetController(++g_tabControllerId);
}

void JSTabsController::ChangeIndex(int32_t index)
{
    if (controller_) {
        controller_->SetIndexByController(index, false);
    }
}

} // namespace OHOS::Ace::Framework