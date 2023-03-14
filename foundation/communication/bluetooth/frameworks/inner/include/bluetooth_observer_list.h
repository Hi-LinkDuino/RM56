/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BLUETOOTH_OBSERVER_LIST_H
#define BLUETOOTH_OBSERVER_LIST_H

#include <functional>
#include <list>
#include <memory>
#include <mutex>

#include "bluetooth_log.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
template <typename T>
class BluetoothObserverList final {
public:
    BluetoothObserverList() = default;
    ~BluetoothObserverList();

    bool Register(std::shared_ptr<T> &observer);
    bool Deregister(std::shared_ptr<T> &observer);

    void ForEach(const std::function<void(std::shared_ptr<T>)> &observer);

private:
    std::mutex lock_{};
    std::list<std::shared_ptr<T>> observers_{};

    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothObserverList);
};

template <typename T>
BluetoothObserverList<T>::~BluetoothObserverList()
{
    std::lock_guard<std::mutex> lock(lock_);
    observers_.clear();
}

template <typename T>
bool BluetoothObserverList<T>::Register(std::shared_ptr<T> &observer)
{
    std::lock_guard<std::mutex> lock(lock_);
    for (auto it = observers_.begin(); it != observers_.end(); ++it) {
        if (*it == observer) {
            return true;
        }
    }
    observers_.push_back(observer);
    return true;
}

template <typename T>
bool BluetoothObserverList<T>::Deregister(std::shared_ptr<T> &observer)
{
    std::lock_guard<std::mutex> lock(lock_);
    for (auto it = observers_.begin(); it != observers_.end(); ++it) {
        if (*it == observer) {
            observers_.erase(it);
            return true;
        }
    }

    return false;
}

template <typename T>
void BluetoothObserverList<T>::ForEach(const std::function<void(std::shared_ptr<T>)> &observer)
{
    std::lock_guard<std::mutex> lock(lock_);
    for (const auto &it : observers_) {
        if (it != nullptr) {
            observer(it);
        }
    }
}
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // BLUETOOTH_OBSERVER_LIST_H