/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "components/ui_view.h"
#include "components/ui_view_group.h"

namespace OHOS {
void DeleteViewChildren(UIView *view)
{
    if (view == nullptr) {
        return;
    }
    while (view != nullptr) {
        UIView *tempView = view;
        view = view->GetNextSibling();
        if (tempView->IsViewGroup()) {
            DeleteViewChildren(dynamic_cast<UIViewGroup *>(tempView)->GetChildrenHead());
        }
        if (tempView->GetParent()) {
            dynamic_cast<UIViewGroup *>(tempView->GetParent())->Remove(tempView);
        }
        delete tempView;
    }
}
}
