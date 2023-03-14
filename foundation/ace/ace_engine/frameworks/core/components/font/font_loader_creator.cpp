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

#include "core/components/font/flutter_font_loader.h"
#include "core/components/font/rosen_font_loader.h"

namespace OHOS::Ace {

const char FONT_SRC_NETWORK[] = "http";

RefPtr<FontLoader> FontLoader::Create(const std::string& familyName, const std::string& familySrc)
{
    if (SystemProperties::GetRosenBackendEnabled()) {
#ifdef ENABLE_ROSEN_BACKEND
        return AceType::MakeRefPtr<RosenFontLoader>(familyName, familySrc);
#else
        return nullptr;
#endif
    } else {
        return AceType::MakeRefPtr<FlutterFontLoader>(familyName, familySrc);
    }
}

} // namespace OHOS::Ace
