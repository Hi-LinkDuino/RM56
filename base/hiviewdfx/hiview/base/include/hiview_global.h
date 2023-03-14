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
#ifndef HIVIEW_UTILITY_PLATFORM_GLOBAL_H
#define HIVIEW_UTILITY_PLATFORM_GLOBAL_H

#include <memory>

#include "plugin.h"

namespace OHOS {
namespace HiviewDFX {
class DllExport HiviewGlobal {
public:
    explicit HiviewGlobal(HiviewContext& context) : context_(context) {}
    ~HiviewGlobal() {};
    static void CreateInstance(HiviewContext& context);
    static void ReleaseInstance();
    // maybe null reference, check before use
    static std::unique_ptr<HiviewGlobal>& GetInstance();
    std::string GetHiViewDirectory(HiviewContext::DirectoryType type) const;
    std::string GetHiviewProperty(const std::string& key, const std::string& defaultValue);
    bool SetHiviewProperty(const std::string& key, const std::string& value, bool forceUpdate);
    void PostAsyncEventToTarget(const std::string& targetPlugin, std::shared_ptr<Event> event);
    bool PostSyncEventToTarget(const std::string& targetPlugin, std::shared_ptr<Event> event);
    void PostUnorderedEvent(std::shared_ptr<Event> event);
    void AddListenerInfo(uint32_t type, const std::string& name,
    const std::set<std::string>& eventNames, const std::set<EventListener::EventIdRange>& listenerInfo);
    bool GetListenerInfo(uint32_t type, const std::string& name, std::set<EventListener::EventIdRange> &listenerInfo);
    bool GetListenerInfo(uint32_t type, const std::string& name, std::set<std::string> &eventNames);
private:
    HiviewContext& context_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_UTILITY_PLATFORM_GLOBAL_H