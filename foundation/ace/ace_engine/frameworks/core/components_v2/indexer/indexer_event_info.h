/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_V2_INDEXER_EVENT_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_V2_INDEXER_EVENT_INFO_H

#include "core/event/ace_events.h"

namespace OHOS::Ace::V2 {
class IndexerEventInfo : public BaseEventInfo, public EventToJSONStringAdapter {
    DECLARE_RELATIONSHIP_OF_CLASSES(IndexerEventInfo, BaseEventInfo, EventToJSONStringAdapter);
public:
    explicit IndexerEventInfo(int32_t selectedIndex) : BaseEventInfo("indexer"), selectedIndex_(selectedIndex)
    {}

    ~IndexerEventInfo() = default;

    std::string ToJSONString() const override;

    int32_t GetSelectedIndex() const
    {
        return selectedIndex_;
    }

private:
    int32_t selectedIndex_ = 0;
};
} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_INDEXER_V2_INDEXER_EVENT_INFO_H