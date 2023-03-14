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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_TEST_UNITTEST_CARDFRONTEND_CARD_TEST_FACTORY_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_TEST_UNITTEST_CARDFRONTEND_CARD_TEST_FACTORY_H

#include <cstdint>

#include "base/memory/referenced.h"
#include "base/utils/noncopyable.h"
#include "base/utils/singleton.h"
#include "frameworks/bridge/card_frontend/js_card_parser.h"

namespace OHOS::Ace::Framework {

class CardTestFactory : public Singleton<CardTestFactory> {
    DECLARE_SINGLETON(CardTestFactory);
    ACE_DISALLOW_MOVE(CardTestFactory);

public:
    void ParseCardDsl(const RefPtr<JsAcePage>& page, const std::string& content);
    void ParseCardDslWithParams(const RefPtr<JsAcePage>& page, const std::string& content, const std::string& params);

    int32_t GetNodeNumber()
    {
        int32_t cnt = nodeCnt_;
        nodeCnt_ = 0;
        return cnt;
    }

private:
    void FlushCommands(const RefPtr<Framework::JsAcePage>& page);
    int32_t nodeCnt_ = 0;
    RefPtr<PipelineContext> context_;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_TEST_UNITTEST_CARDFRONTEND_CARD_TEST_FACTORY_H
