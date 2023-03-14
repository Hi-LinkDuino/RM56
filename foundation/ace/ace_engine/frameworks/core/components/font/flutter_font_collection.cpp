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

#include "core/components/font/flutter_font_collection.h"

#include "flutter/lib/ui/ui_dart_state.h"
#include "flutter/third_party/txt/src/minikin/FontFamily.h"
#include "flutter/third_party/txt/src/minikin/FontLanguageListCache.h"

#include "base/i18n/localization.h"
#include "base/log/ace_trace.h"
#include "base/log/log.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"
#include "core/common/ace_engine.h"

namespace OHOS::Ace {

FlutterFontCollection FlutterFontCollection::instance;

std::shared_ptr<txt::FontCollection> FlutterFontCollection::GetFontCollection()
{
    if (!isUseFlutterEngine) {
        if (!isCompleted_) {
            isCompleted_ = future_.get();
        }
        return fontCollection_->GetFontCollection();
    }

    auto* windowClient = GetFlutterEngineWindowClient();
    if (windowClient == nullptr) {
        return nullptr;
    }
    auto& fontCollection = windowClient->GetFontCollection();
    return fontCollection.GetFontCollection();
}

flutter::WindowClient* FlutterFontCollection::GetFlutterEngineWindowClient()
{
    if (!flutter::UIDartState::Current()) {
        LOGE("uiDartState is null");
        return nullptr;
    }

    auto* window = flutter::UIDartState::Current()->window();
    if (window == nullptr) {
        LOGW("UpdateParagraph: window or client is null");
        return nullptr;
    }
    return window->client();
}

void FlutterFontCollection::LoadFontFromList(const uint8_t* fontData, size_t length, std::string familyName)
{
    auto it = std::find(families_.begin(), families_.end(), familyName);
    if (it != families_.end()) {
        return;
    }

    families_.emplace_back(familyName);

    if (!isUseFlutterEngine) {
        if (!isCompleted_) {
            isCompleted_ = future_.get();
        }
        fontCollection_->LoadFontFromList(fontData, length, familyName);
        return;
    }

    auto* windowClient = GetFlutterEngineWindowClient();
    if (windowClient == nullptr) {
        return;
    }
    auto& fontCollection = windowClient->GetFontCollection();
    fontCollection.LoadFontFromList(fontData, length, familyName);
}

void FlutterFontCollection::CreateFontCollection(const fml::RefPtr<fml::TaskRunner>& ioTaskRunner)
{
    Localization::GetInstance()->SetOnMymrChange([ioTaskRunner](bool isZawgyiMyanmar) {
        if (ioTaskRunner) {
            ioTaskRunner->PostTask(
                [isZawgyiMyanmar]() { FlutterFontCollection::GetInstance().SetIsZawgyiMyanmar(isZawgyiMyanmar); });
        }
    });

    if (isInit_ || !ioTaskRunner) {
        return;
    }
    isInit_ = true;
    isUseFlutterEngine = false;

    ioTaskRunner->PostTask([&fontCollection = fontCollection_, &promise = promise_]() mutable {
        fontCollection = std::make_unique<flutter::FontCollection>();
        if (fontCollection->GetFontCollection()) {
            // Initialize weight scale
            float fontWeightScale = SystemProperties::GetFontWeightScale();
            if (GreatNotEqual(fontWeightScale, 0.0)) {
                fontCollection->GetFontCollection()->VaryFontCollectionWithFontWeightScale(fontWeightScale);
            }

            auto locale = Localization::GetInstance()->GetFontLocale();
            uint32_t langListId = locale.empty() ? minikin::FontLanguageListCache::kEmptyListId
                                                 : minikin::FontStyle::registerLanguageList(locale);
            const minikin::FontLanguages& langs = minikin::FontLanguageListCache::getById(langListId);
            locale = langs.size() ? langs[0].getString() : "";
            // 0x4e2d is unicode for '中'.
            fontCollection->GetFontCollection()->MatchFallbackFont(0x4e2d, locale);
            fontCollection->GetFontCollection()->GetMinikinFontCollectionForFamilies({ "sans-serif" }, locale);
        }
        promise.set_value(true);
    });
}

FlutterFontCollection& FlutterFontCollection::GetInstance()
{
    return instance;
}

void FlutterFontCollection::VaryFontCollectionWithFontWeightScale(float fontWeightScale)
{
    if (LessOrEqual(fontWeightScale, 0.0)) {
        return;
    }

    if (!isUseFlutterEngine) {
        if (!isCompleted_) {
            return;
        }
        if (fontCollection_ && fontCollection_->GetFontCollection()) {
            fontCollection_->GetFontCollection()->VaryFontCollectionWithFontWeightScale(fontWeightScale);
        }
        return;
    }

    if (!flutter::UIDartState::Current()) {
        LOGE("uiDartState is null");
        return;
    }

    auto* window = flutter::UIDartState::Current()->window();
    if (window == nullptr || window->client() == nullptr) {
        LOGW("UpdateParagraph: window or client is null");
        return;
    }

    auto& fontCollection = window->client()->GetFontCollection();
    fontCollection.GetFontCollection()->VaryFontCollectionWithFontWeightScale(fontWeightScale);
}

void FlutterFontCollection::LoadSystemFont()
{
    ACE_FUNCTION_TRACE();
    if (!isUseFlutterEngine) {
        if (!isCompleted_) {
            return;
        }
        if (fontCollection_ && fontCollection_->GetFontCollection()) {
            fontCollection_->GetFontCollection()->LoadSystemFont();
        }
        return;
    }

    if (!flutter::UIDartState::Current()) {
        LOGE("uiDartState is null");
        return;
    }

    auto* window = flutter::UIDartState::Current()->window();
    if (window == nullptr || window->client() == nullptr) {
        LOGW("UpdateParagraph: window or client is null");
        return;
    }

    auto& fontCollection = window->client()->GetFontCollection();
    fontCollection.GetFontCollection()->LoadSystemFont();
}

void FlutterFontCollection::SetIsZawgyiMyanmar(bool isZawgyiMyanmar)
{
    ACE_FUNCTION_TRACE();

    if (isZawgyiMyanmar_ == isZawgyiMyanmar) {
        return;
    }
    isZawgyiMyanmar_ = isZawgyiMyanmar;

    if (!isUseFlutterEngine) {
        if (!isCompleted_) {
            isCompleted_ = future_.get();
        }
        if (fontCollection_ && fontCollection_->GetFontCollection()) {
            fontCollection_->GetFontCollection()->SetIsZawgyiMyanmar(isZawgyiMyanmar);
        }
        return;
    }

    if (!flutter::UIDartState::Current()) {
        LOGE("uiDartState is null");
        return;
    }

    auto* window = flutter::UIDartState::Current()->window();
    if (window == nullptr || window->client() == nullptr) {
        LOGW("UpdateParagraph: window or client is null");
        return;
    }

    auto& fontCollection = window->client()->GetFontCollection();
    fontCollection.GetFontCollection()->SetIsZawgyiMyanmar(isZawgyiMyanmar);

    AceEngine::Get().NotifyContainers([](const RefPtr<Container>& container) {
        if (container) {
            container->NotifyFontNodes();
        }
    });
}

} // namespace OHOS::Ace
