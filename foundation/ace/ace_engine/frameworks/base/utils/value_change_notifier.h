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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_VALUE_CHANGE_NOTIFIER_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_VALUE_CHANGE_NOTIFIER_H

#include <list>

#include "base/memory/ace_type.h"
#include "base/utils/noncopyable.h"

namespace OHOS::Ace {

class ValueChangeObserver : public virtual AceType {
    DECLARE_ACE_TYPE(ValueChangeObserver, AceType);

public:
    virtual void OnValueChanged(bool needFireChangeEvent = true, bool needFireSelectChangeEvent = true) = 0;

protected:
    ValueChangeObserver() = default;
    ~ValueChangeObserver() override = default;

    ACE_DISALLOW_COPY_AND_MOVE(ValueChangeObserver);
};

/**
 * @brief Used for observe a value change.
 * @see example as TextEditController.
 *
 * Note: Make sure all methods calling on same thread.
 *
 * @tparam U is the value type.
 */
template<class U>
class ValueChangeNotifier : public virtual AceType {
    DECLARE_ACE_TYPE(ValueChangeNotifier<U>, AceType);

public:
    void AddObserver(const WeakPtr<ValueChangeObserver>& observer)
    {
        observers_.emplace_back(observer);
    }

    void RemoveObserver(const WeakPtr<ValueChangeObserver>& observer)
    {
        observers_.remove(observer);
    }

    void SetValue(U&& newValue, bool needFireChangeEvent = true)
    {
        if (newValue == value_) {
            return;
        }

        preValue_ = value_;
        value_ = std::move(newValue);
        Notify(needFireChangeEvent);
    }

    void SetValue(const U& newValue, bool needFireChangeEvent = true)
    {
        if (newValue == value_) {
            return;
        }

        preValue_ = value_;
        value_ = newValue;
        Notify(needFireChangeEvent);
    }

    const U& GetValue() const
    {
        return value_;
    }

    const U& GetPreValue() const
    {
        return preValue_;
    }

private:
    void Notify(bool needFireChangeEvent = true)
    {
        auto iter = observers_.begin();
        while (iter != observers_.end()) {
            const auto& observer = iter->Upgrade();
            if (observer) {
                observer->OnValueChanged(needFireChangeEvent);
                iter++;
            } else {
                iter = observers_.erase(iter);
            }
        }
    }

    std::list<WeakPtr<ValueChangeObserver>> observers_;

    U value_;
    U preValue_;
};

}  // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_VALUE_CHANGE_NOTIFIER_H
