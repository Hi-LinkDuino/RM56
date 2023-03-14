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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_CONTROLLER_H

#include <map>

#include "base/memory/ace_type.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {
using TabBarChangeListener = std::function<void(int32_t)>;

class ACE_EXPORT TabController : public AceType {
    DECLARE_ACE_TYPE(TabController, AceType);

public:
    explicit TabController(int32_t id);
    ~TabController() override = default;

    static RefPtr<TabController> GetController(int32_t id);

    int32_t GetId() const;
    int32_t GetIndex() const;

    void SetTotalCount(int32_t totalCount)
    {
        totalCount_ = totalCount;
    }
    int32_t GetTotalCount() const
    {
        return totalCount_;
    }

    int32_t GetInitialIndex() const
    {
        return initialIndex_;
    }

    int32_t GetPendingIndex() const
    {
        return pendingIndex_;
    }

    void ValidateIndex(int32_t maxIndex);
    void SetPageReady(bool ready);
    void SetIndex(int32_t index);
    void SetInitialIndex(int32_t index);
    void SetPendingIndex(int32_t index);
    void SetIndexWithoutChangeContent(int32_t index);
    void SetIndexByController(int32_t index, bool blockEvent = true);
    void SetIndexByScrollContent(int32_t index);
    void SetContentElement(const RefPtr<Element>& contentElement);
    void SetBarElement(const RefPtr<Element>& barElement);
    void ChangeDispatch(int32_t index);
    bool IsIndexDefined() const;

    void OnTabBarChanged(int32_t index)
    {
        if (tabBarChangeListener_) {
            tabBarChangeListener_(index);
        }
    }
    void SetTabBarChangeListener(const TabBarChangeListener& listener)
    {
        tabBarChangeListener_ = listener;
    }

private:
    int32_t id_ = -1;
    int32_t index_ = 0;
    int32_t initialIndex_ = -1;
    int32_t pendingIndex_ = -1;
    int32_t totalCount_ = 0;
    bool pageReady_ = false;
    bool indexDefined_ = false;
    WeakPtr<Element> contentElement_;
    WeakPtr<Element> barElement_;
    TabBarChangeListener tabBarChangeListener_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_CONTROLLER_H
