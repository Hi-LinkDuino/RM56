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

#ifndef UTILS_BASE_OBSERVER_H
#define UTILS_BASE_OBSERVER_H

#include <memory>
#include <vector>
#include <set>
#include <mutex>

namespace OHOS {


struct ObserverArg {
public:
    virtual ~ObserverArg() = default;
};

class Observer;
class Observable {
public:
    virtual ~Observable() = default;
    void AddObserver(const std::shared_ptr<Observer>& o);
    void RemoveObserver(const std::shared_ptr<Observer>& o);
    void RemoveAllObservers();
    void NotifyObservers();
    void NotifyObservers(const ObserverArg* arg);
    int GetObserversCount();

protected:
    bool HasChanged();
    void SetChanged();
    void ClearChanged();

protected:
    std::set<std::shared_ptr<Observer>> obs;
    std::mutex mutex_;

private:
    bool changed_ = false;
};

class Observer {
public:
    virtual ~Observer() = default;
    virtual void Update(const Observable* o, const ObserverArg* arg) = 0;
};


}

#endif

