/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_EVENT_LISTENER_H
#define OHOS_EVENT_LISTENER_H

#include <functional>
#include <utility>

#include "components/ui_view.h"

namespace OHOS {
using OnEventFunc = std::function<bool(UIView &view, const Event &event)>;

class EventListener :
        public UIView::OnClickListener,
        public UIView::OnLongPressListener {
public:
    EventListener() = delete;
    ~EventListener() override = default;

    EventListener(OnEventFunc onClick, OnEventFunc onLongPress)
    {
        onClick_ = std::move(onClick);
        onLongPress_ = std::move(onLongPress);
    }

    bool OnClick(UIView &view, const ClickEvent &event) override
    {
        if (!onClick_) {
            return false;
        }
        return onClick_(view, event);
    }

    bool OnLongPress(UIView &view, const LongPressEvent &event) override
    {
        if (!onLongPress_) {
            return false;
        }
        return onLongPress_(view, event);
    }

private:
    OnEventFunc onClick_ {};
    OnEventFunc onLongPress_ {};
};
}
#endif // OHOS_EVENT_LISTENER_H
