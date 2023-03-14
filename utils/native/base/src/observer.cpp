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

#include "observer.h"

using namespace std;

namespace OHOS {

void Observable::AddObserver(const shared_ptr<Observer>& o)
{
    if (o == nullptr) {
        return;
    }

    lock_guard<mutex> lock(mutex_);
    if (obs.count(o) > 0) {
        return;
    }

    obs.insert(o);
}

void Observable::RemoveObserver(const shared_ptr<Observer>& o)
{
    lock_guard<mutex> lock(mutex_);
    obs.erase(o);
}

void Observable::RemoveAllObservers()
{
    lock_guard<mutex> lock(mutex_);
    obs.clear();
}

bool Observable::HasChanged()
{
    return changed_;
}

int Observable::GetObserversCount()
{
    lock_guard<mutex> lock(mutex_);
    return (int)obs.size();
}

void Observable::NotifyObservers()
{
    NotifyObservers(nullptr);
}

void Observable::NotifyObservers(const ObserverArg* arg)
{
    set<shared_ptr<Observer>> arrLocal;
    {
        lock_guard<mutex> lock(mutex_);
        if (!changed_) {
            return;
        }

        arrLocal = obs;
        ClearChanged();
    }

    for (auto& o : arrLocal) {
        o->Update(this, arg);
    }
}

void Observable::SetChanged()
{
    changed_ = true;
}

void Observable::ClearChanged()
{
    changed_ = false;
}

}
