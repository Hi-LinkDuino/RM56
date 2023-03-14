/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef BLUETOOTH_OBSERVER_MAP_H
#define BLUETOOTH_OBSERVER_MAP_H

#include <functional>
#include <map>
#include <memory>
#include <mutex>

template <typename T>
class BluetoothObserverMap final {
public:
    BluetoothObserverMap() = default;
    ~BluetoothObserverMap();

    bool Register(int handle, T *observer);
    bool Deregister(T *observer);

    void ForEach(const std::function<void(uint8_t, T *)> &observer, int handle);

    uint8_t GetAdvertiserHandle(T *observer);
    T *PopAdvertiserObserver(uint8_t advHandle);
    T *GetAdvertiserObserver(uint8_t advHandle);
    bool IsExistAdvertiserCallback(T *observer, int &handle);

private:
    std::mutex lock_;
    std::map<int, T *> observers_;

    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothObserverMap);
};

template<typename T>
BluetoothObserverMap<T>::~BluetoothObserverMap()
{
    std::lock_guard<std::mutex> lock(lock_);
    observers_.clear();
}

template<typename T>
bool BluetoothObserverMap<T>::Register(int handle, T *observer)
{
    std::lock_guard<std::mutex> lock(lock_);

    auto it = observers_.begin();
    for (; it != observers_.end();) {
        if (it->first == handle) {
            observers_.erase(it++);
            observers_.insert(std::make_pair(handle, observer));
            return true;
        } else {
            ++it;
        }
    }
    if (it == observers_.end()) {
        observers_.insert(std::make_pair(handle, observer));
    }
    return true;
}

template<typename T>
bool BluetoothObserverMap<T>::Deregister(T *observer)
{
    std::lock_guard<std::mutex> lock(lock_);
    auto it = observers_.begin();
    for (; it != observers_.end();) {
        if (it->second == observer) {
            observers_.erase(it++);
            return true;
        } else {
            ++it;
        }
    }

    return false;
}

template<typename T>
void BluetoothObserverMap<T>::ForEach(const std::function<void(uint8_t, T *)> &observer, int handle)
{
    std::lock_guard<std::mutex> lock(lock_);
    for (const auto &it : observers_) {
        if (handle == it.first) {
            observer(it.first, it.second);
        }
    }
}

template<typename T>
uint8_t BluetoothObserverMap<T>::GetAdvertiserHandle(T *observer)
{
    std::lock_guard<std::mutex> lock(lock_);
    uint8_t advHandle = OHOS::bluetooth::BLE_INVALID_ADVERTISING_HANDLE;
    if (observer == nullptr) {
        return advHandle;
    }

    auto it = observers_.begin();
    for (; it != observers_.end(); it++) {
        if (it->second == observer) {
            advHandle = it->first;
            break;
        }
    }

    return advHandle;
}

template<typename T>
T *BluetoothObserverMap<T>::PopAdvertiserObserver(uint8_t advHandle)
{
    std::lock_guard<std::mutex> lock(lock_);
    T *t = nullptr;
    auto it = observers_.begin();
    for (; it != observers_.end(); it++) {
        if (it->first == advHandle) {
            t = it->second;
            observers_.erase(it++);
            break;
        }
    }
    return t;
}

template<typename T>
T *BluetoothObserverMap<T>::GetAdvertiserObserver(uint8_t advHandle)
{
    std::lock_guard<std::mutex> lock(lock_);
    auto it = observers_.begin();
    for (; it != observers_.end(); it++) {
        if (it->first == advHandle) {
            return it->second;
        }
    }

    return nullptr;
}

template<typename T>
bool BluetoothObserverMap<T>::IsExistAdvertiserCallback(T *observer, int &handle)
{
    bool isExtist = false;
    if (observer == nullptr) {
        return isExtist;
    }

    auto it = observers_.begin();
    for (; it != observers_.end(); it++) {
        if (it->second == observer) {
            handle = it->first;
            isExtist = true;
            break;
        }
    }

    return isExtist;
}

#endif  // BLUETOOTH_OBSERVER_LIST_H