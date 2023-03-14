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
#ifndef HIVIEW_PLATFORM_TEST_RESULT_LISTENER_H
#define HIVIEW_PLATFORM_TEST_RESULT_LISTENER_H
#include <map>
#include "event.h"
#include "plugin.h"
class PlatformTestResultListener : public OHOS::HiviewDFX::EventListener {
public:
    explicit PlatformTestResultListener(const std::string& name)
        : unorderEventCount_(0), name_(name){};
    ~PlatformTestResultListener(){};
    void OnUnorderedEvent(const OHOS::HiviewDFX::Event &msg) override;
    std::string GetListenerName() override;
    std::map<std::string, std::string> processedUnorderedEvents_;
    uint32_t unorderEventCount_;

private:
    std::string name_;
};
#endif // HIVIEW_PLATFORM_TEST_RESULT_LISTENER_H