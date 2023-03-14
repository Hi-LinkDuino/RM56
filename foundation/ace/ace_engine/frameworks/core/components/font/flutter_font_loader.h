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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FONT_FLUTTER_FONT_LOADER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FONT_FLUTTER_FONT_LOADER_H

#include "core/common/font_loader.h"

namespace OHOS::Ace {

extern const char FONT_SRC_NETWORK[];

class FlutterFontLoader : public FontLoader {
    DECLARE_ACE_TYPE(FlutterFontLoader, FontLoader);

public:
    FlutterFontLoader(const std::string& familyName, const std::string& familySrc);
    ~FlutterFontLoader() override = default;

    void AddFont(const RefPtr<PipelineContext>& context) override;

private:
    void LoadFromNetwork(const RefPtr<PipelineContext>& context);
    void LoadFromAsset(const RefPtr<PipelineContext>& context);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FONT_FLUTTER_FONT_LOADER_H
