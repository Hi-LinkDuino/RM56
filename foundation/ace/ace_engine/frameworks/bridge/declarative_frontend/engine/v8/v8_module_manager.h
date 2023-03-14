/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_MODULE_MANAGER_H
#define FOUNDATION_MODULE_MANAGER_H

#include <string>
#include <unordered_map>

#include "third_party/v8/include/v8.h"

#include "base/utils/noncopyable.h"

template<typename T>
using CopyablePersistent = v8::Persistent<T, v8::CopyablePersistentTraits<T>>;

namespace OHOS::Ace::Framework {
class ModuleManager {
public:
    ModuleManager() = default;
    ~ModuleManager() = default;

    static ModuleManager* GetInstance();
    bool InitModule(v8::Local<v8::Object> moduleObj, const std::string& moduleName, v8::Isolate* isolate);

    static void InitTimerModule(v8::Local<v8::Context>& localContext);
    void SetWaitTimer(const v8::FunctionCallbackInfo<v8::Value>& args, bool isInterval);
    void ClearWaitTimer(const v8::FunctionCallbackInfo<v8::Value>& args, bool isInterval);
    uint32_t AddCallback(CopyablePersistent<v8::Function> callbackFunc,
        std::vector<CopyablePersistent<v8::Value>> callbackArray, v8::Isolate* callbackIsolateMap, bool isInterval);
    void RemoveCallbackFunc(uint32_t callbackId, bool isInterval);
    CopyablePersistent<v8::Function> GetCallbackFunc(uint32_t callbackId, bool isInterval);
    std::vector<CopyablePersistent<v8::Value>> GetCallbackArray(uint32_t callbackId, bool isInterval);
    v8::Isolate* GetCallbackIsolate(uint32_t callbackId, bool isInterval);
    void ClearTimerIsolate(v8::Isolate* isolate);

private:
    uint32_t AddCallback(std::unordered_map<uint32_t, CopyablePersistent<v8::Function>>& callbackFuncMap,
        std::unordered_map<uint32_t, std::vector<CopyablePersistent<v8::Value>>>& callbackArrayMap,
        std::unordered_map<uint32_t, v8::Isolate*>& callbackIsolateMap, CopyablePersistent<v8::Function> callbackFunc,
        std::vector<CopyablePersistent<v8::Value>> callbackArray, v8::Isolate* isolate);

    void RemoveCallbackFunc(std::unordered_map<uint32_t, CopyablePersistent<v8::Function>>& callbackFuncMap,
        std::unordered_map<uint32_t, std::vector<CopyablePersistent<v8::Value>>>& callbackArrayMap,
        std::unordered_map<uint32_t, v8::Isolate*>& callbackIsolateMap, uint32_t callbackId);

    std::vector<CopyablePersistent<v8::Value>> GetCallbackArray(
        std::unordered_map<uint32_t, std::vector<CopyablePersistent<v8::Value>>>& callbackArrayMap,
        uint32_t callbackId);

    std::unordered_map<uint32_t, CopyablePersistent<v8::Function>> callbackFuncMap_;
    std::unordered_map<uint32_t, CopyablePersistent<v8::Function>> intervalCallbackFuncMap_;
    std::unordered_map<uint32_t, std::vector<CopyablePersistent<v8::Value>>> callbackArrayMap_;
    std::unordered_map<uint32_t, std::vector<CopyablePersistent<v8::Value>>> intervalCallbackArrayMap_;
    std::unordered_map<uint32_t, v8::Isolate*> callbackIsolateMap_;
    std::unordered_map<uint32_t, v8::Isolate*> intervalCallbackIsolateMap_;
    uint32_t callbackId_ = 0;

    ACE_DISALLOW_COPY_AND_MOVE(ModuleManager);
};
} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_MODULE_MANAGER_H
