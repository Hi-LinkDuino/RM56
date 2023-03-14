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

#ifndef UTILS_BASE_SINGLETON_H
#define UTILS_BASE_SINGLETON_H

#include "nocopyable.h"
#include <mutex>
#include <memory>

namespace OHOS {
#define DECLARE_DELAYED_SINGLETON(MyClass) \
public: \
    ~MyClass(); \
private: \
    friend DelayedSingleton<MyClass>; \
    MyClass();

#define DECLARE_DELAYED_REF_SINGLETON(MyClass) \
private: \
    friend DelayedRefSingleton<MyClass>; \
    ~MyClass(); \
    MyClass();


#define DECLARE_SINGLETON(MyClass) \
private: \
    friend Singleton<MyClass>; \
    MyClass& operator=(const MyClass&) = delete; \
    MyClass(const MyClass&) = delete; \
    MyClass(); \
    ~MyClass();


template<typename T>
class DelayedSingleton : public NoCopyable {
public:
    static std::shared_ptr<T> GetInstance();
    static void DestroyInstance();

private:
    static std::shared_ptr<T> instance_;
    static std::mutex mutex_;
};

template<typename T>
std::shared_ptr<T> DelayedSingleton<T>::instance_ = nullptr;

template<typename T>
std::mutex DelayedSingleton<T>::mutex_;

template<typename T>
std::shared_ptr<T> DelayedSingleton<T>::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<T>();
        }
    }

    return instance_;
}

template<typename T>
void DelayedSingleton<T>::DestroyInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ != nullptr) {
        instance_.reset();
        instance_ = nullptr;
    }
}

template<typename T>
class DelayedRefSingleton : public NoCopyable {
public:
    static T& GetInstance();

private:
    static T* instance_;
    static std::mutex mutex_;
};

template<typename T>
T* DelayedRefSingleton<T>::instance_ = nullptr;

template<typename T>
std::mutex DelayedRefSingleton<T>::mutex_;

template<typename T>
T& DelayedRefSingleton<T>::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (instance_ == nullptr) {
            instance_ = new T();
        }
    }

    return *instance_;
}

template<typename T>
class Singleton : public NoCopyable {
public:
    static T& GetInstance()
    {
        return instance_;
    }

private:
    static T instance_;
};

template<typename T>
T Singleton<T>::instance_;
} // namespace OHOS

#endif
