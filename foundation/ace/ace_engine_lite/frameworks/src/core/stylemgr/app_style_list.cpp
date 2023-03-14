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

#include "stylemgr/app_style_list.h"

namespace OHOS {
namespace ACELite {
void AppStyleList::Reset()
{
    AppStyle* next = nullptr;
    while (firstStyle_ != nullptr) {
        next = const_cast<AppStyle *>(firstStyle_->GetNext());
        firstStyle_->Reset();
        delete firstStyle_;
        firstStyle_ = next;
    }
    lastStyle_ = nullptr;
}

void AppStyleList::AddStyle(AppStyle *newStyle)
{
    if (newStyle == nullptr) {
        return;
    }

    // the first one
    if (firstStyle_ == nullptr) {
        firstStyle_ = newStyle;
        lastStyle_ = newStyle;
        return;
    }

    // fresh new one
    newStyle->SetPre(lastStyle_);
    lastStyle_->SetNext(newStyle);
    lastStyle_ = newStyle;
}

AppStyle *AppStyleList::GetExistStyle(const char *name) const
{
    if (firstStyle_ == nullptr) {
        return nullptr;
    }
    if (name == nullptr || strlen(name) == 0) {
        return nullptr;
    }
    const AppStyle *current = firstStyle_;
    while (current != nullptr) {
        // point to next immediately
        const AppStyle *existCurrentStyle = current;
        current = existCurrentStyle->GetNext();
        const char *styleName = existCurrentStyle->GetStyleName();

        if (styleName == nullptr || strlen(styleName) == 0) {
            continue;
        }
        if ((strlen(styleName) == strlen(name)) && strcmp(styleName, name) == 0) {
            // exist
            return const_cast<AppStyle *>(existCurrentStyle);
        }
    }

    return nullptr;
}
} // namespace ACELite
} // namespace OHOS
