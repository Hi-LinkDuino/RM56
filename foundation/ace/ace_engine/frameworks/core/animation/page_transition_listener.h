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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PAGE_TRANSITION_LISTENER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PAGE_TRANSITION_LISTENER_H

#include <unordered_map>

#include "base/log/log.h"
#include "base/memory/referenced.h"
#include "base/utils/event_callback.h"

namespace OHOS::Ace {

// Notify Event when stage handle page event in PostFlush at the same vsync when stage's performBuild happens.
// Because in PushPage scene, the page transition of the Stage needs to know whether this jump has shared transition.
// Further, shared transition needs to know dest page's size after performLayout, So Notify this Event in
// PostFlush.
enum class TransitionEvent {
    POP_START,
    POP_END,
    PUSH_START,
    PUSH_END,
};

class PageElement;

class PageTransitionListenable {
public:
    using Callback = EventCallback<void(
        const TransitionEvent& event, const WeakPtr<PageElement>& pageIn, const WeakPtr<PageElement>& pageOut)>;
    using CallbackFuncType = Callback::FunctionType;

    PageTransitionListenable() = default;
    virtual ~PageTransitionListenable() = default;

    void AddPageTransitionListener(const Callback& callback)
    {
        callbacks_.emplace(callback.GetId(), callback);
    }

    typename Callback::IdType AddPageTransitionListener(const CallbackFuncType& funcObject)
    {
        Callback callback(funcObject);
        AddPageTransitionListener(callback);
        return callback.GetId();
    }

    void RemovePageTransitionListener(const Callback& callback)
    {
        callbacks_.erase(callback.GetId());
    }

    void RemovePageTransitionListener(typename Callback::IdType id)
    {
        callbacks_.erase(id);
    }

    void ClearPageTransitionListeners()
    {
        callbacks_.clear();
    }

    void NotifyPageTransitionListeners(
        const TransitionEvent event, const WeakPtr<PageElement>& pageIn, const WeakPtr<PageElement>& pageOut) const
    {
        for (auto&& [id, callback] : callbacks_) {
            if (callback) {
                callback(event, pageIn, pageOut);
            }
        }
    }

private:
    std::unordered_map<typename Callback::IdType, Callback> callbacks_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_PAGE_TRANSITION_LISTENER_H
