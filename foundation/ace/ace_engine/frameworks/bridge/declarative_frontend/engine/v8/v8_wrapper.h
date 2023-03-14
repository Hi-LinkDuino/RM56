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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_WRAPPER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_ENGINE_V8_V8_WRAPPER_H

namespace OHOS::Ace::Framework {

class V8Wrapper {
public:
    template<typename C, typename... Args>
    static V8Wrapper* New(v8::Local<v8::Object> obj, Args&&... args);

    template<typename C>
    static V8Wrapper* New(v8::Local<v8::Object> obj, C* instance);

    v8::Local<v8::Object> Get() const
    {
        return handle_.Get(v8::Isolate::GetCurrent());
    }

    template<typename C>
    C* GetInstance() const;

    V8Wrapper& operator=(const V8Wrapper&) = delete;

    ~V8Wrapper()
    {
        handle_.Reset();
    }

private:
    template<typename C>
    V8Wrapper(v8::Local<v8::Object> obj, C* instance);

    v8::Persistent<v8::Object> handle_;
};

} // namespace OHOS::Ace::Framework

#include "v8_wrapper.inl"

#endif
