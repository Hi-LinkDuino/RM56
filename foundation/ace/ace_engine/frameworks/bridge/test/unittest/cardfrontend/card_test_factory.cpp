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

#include "frameworks/bridge/test/unittest/cardfrontend/card_test_factory.h"

#include <map>

#include "frameworks/bridge/test/unittest/jsfrontend/dom_mock.h"

#define private public
#include "frameworks/bridge/card_frontend/card_frontend.h"

namespace OHOS::Ace::Framework {

CardTestFactory::CardTestFactory() = default;
CardTestFactory::~CardTestFactory() = default;

void CardTestFactory::ParseCardDsl(const RefPtr<JsAcePage>& page, const std::string& content)
{
    ParseCardDslWithParams(page, content, "");
}

void CardTestFactory::FlushCommands(const RefPtr<Framework::JsAcePage>& page)
{
    auto jsCommands = std::make_shared<std::vector<RefPtr<Framework::JsCommand>>>();
    page->PopAllCommands(*jsCommands);

    for (const auto& command : *jsCommands) {
        command->Execute(page);
    }
}

void CardTestFactory::ParseCardDslWithParams(
    const RefPtr<JsAcePage>& page, const std::string& content, const std::string& params)
{
    page->SetFlushCallback(
        [](const RefPtr<Framework::JsAcePage>& page) { CardTestFactory::GetInstance().FlushCommands(page); });
    CardFrontend cardFrontend;
    if (!context_) {
        auto frontend = AceType::MakeRefPtr<MockFrontend>();
        auto platformWindow = std::make_unique<MockPlatformWindow>();
        auto window = std::make_unique<Window>(std::move(platformWindow));
        context_ = AceType::MakeRefPtr<PipelineContext>(std::move(window), nullptr, nullptr, nullptr, frontend, 0);
        context_->SetThemeManager(AceType::MakeRefPtr<ThemeManager>());
    }
    cardFrontend.Initialize(FrontendType::JS_CARD, nullptr);
    cardFrontend.ParsePage(context_, content, params, page);
    nodeCnt_ = cardFrontend.parseJsCard_->GetNumberOfNodes();
}

} // namespace OHOS::Ace::Framework
