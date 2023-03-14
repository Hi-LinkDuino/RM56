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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FONT_ROSEN_FONT_COLLECTION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FONT_ROSEN_FONT_COLLECTION_H

#include <future>

#include "flutter/common/task_runners.h"
#include "flutter/lib/ui/text/font_collection.h"
#include "flutter/lib/ui/window/window.h"

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {

class ACE_EXPORT RosenFontCollection : public NonCopyable {
public:
    static RosenFontCollection& GetInstance();

    std::shared_ptr<txt::FontCollection> GetFontCollection();

    void LoadFontFromList(const uint8_t* fontData, size_t length, std::string familyName);

    void CreateFontCollection(const fml::RefPtr<fml::TaskRunner>& ioTaskRunner);

    void VaryFontCollectionWithFontWeightScale(float fontWeightScale);

    void LoadSystemFont();

    void SetIsZawgyiMyanmar(bool isZawgyiMyanmar);

private:
    flutter::WindowClient* GetRosenEngineWindowClient();

    std::unique_ptr<flutter::FontCollection> fontCollection_;
    std::vector<std::string> families_;

    std::promise<bool> promise_;
    std::shared_future<bool> future_ = promise_.get_future();
    bool isInit_ = false;
    bool isCompleted_ = false;
    bool isUseRosenEngine = true;
    bool isZawgyiMyanmar_ = false;

    static RosenFontCollection instance;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FONT_ROSEN_FONT_COLLECTION_H
