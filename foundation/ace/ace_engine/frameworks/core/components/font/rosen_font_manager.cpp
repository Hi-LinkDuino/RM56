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

#include "core/components/font/rosen_font_manager.h"

#include "core/components/font/rosen_font_collection.h"

namespace OHOS::Ace {

void RosenFontManager::VaryFontCollectionWithFontWeightScale()
{
    if (GreatNotEqual(fontWeightScale_, 0.0)) {
        RosenFontCollection::GetInstance().VaryFontCollectionWithFontWeightScale(fontWeightScale_);
        NotifyVariationNodes();
    }
}

void RosenFontManager::LoadSystemFont()
{
    RosenFontCollection::GetInstance().LoadSystemFont();
}

} // namespace OHOS::Ace
