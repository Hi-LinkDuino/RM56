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

#ifndef BASE_OBSERVER_LIST_H
#define BASE_OBSERVER_LIST_H

#include <list>
#include <mutex>
#include <memory>
#include "base_def.h"

template<typename T>
class BaseObserverList final {
public:
    BaseObserverList(){};
    ~BaseObserverList();

    bool Register(T &observer);
    bool Deregister(T &observer);

    void ForEach(const std::function<void(T &)> &observer);

private:
    std::mutex lock_ = {};
    std::list<T *> observers_ = {};

    BT_DISALLOW_COPY_AND_ASSIGN(BaseObserverList);
};

template<typename T>
BaseObserverList<T>::~BaseObserverList()
{
    std::lock_guard<std::mutex> lock(lock_);
    observers_.clear();
}

template<typename T>
bool BaseObserverList<T>::Register(T &observer)
{
    std::lock_guard<std::mutex> lock(lock_);
    for (auto it = observers_.begin(); it != observers_.end(); ++it) {
        if (*it == &observer) {
            return false;
        }
    }

    observers_.push_back(&observer);
    return true;
}

template<typename T>
bool BaseObserverList<T>::Deregister(T &observer)
{
    std::lock_guard<std::mutex> lock(lock_);
    for (auto it = observers_.begin(); it != observers_.end(); ++it) {
        if (*it == &observer) {
            observers_.erase(it);
            return true;
        }
    }

    return false;
}

template<typename T>
void BaseObserverList<T>::ForEach(const std::function<void(T &)> &observer)
{
    std::lock_guard<std::mutex> lock(lock_);
    for (const auto &it : observers_)
        observer(*it);
}

#endif  // BASE_OBSERVER_LIST_H