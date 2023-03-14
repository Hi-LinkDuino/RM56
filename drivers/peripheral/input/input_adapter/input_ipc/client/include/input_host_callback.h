/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef INPUT_HOST_CALLBACK_H
#define INPUT_HOST_CALLBACK_H

#include "input_host_callback_stub.h"

namespace OHOS {
namespace Input {
class InputReportEventCallback : public InputReportEventCallbackStub {
public:
    InputReportEventCallback() = default;
    virtual ~InputReportEventCallback() = default;
public:
    void ReportEventPkgCallback(const EventPackage *pkgs, uint32_t count, uint32_t devIndex) override;
};

class InputHostReportEventCallback : public InputReportHostEventCallbackStub {
public:
    InputHostReportEventCallback() = default;
    virtual ~InputHostReportEventCallback() = default;
public:
    void ReportHotPlugEventCallback(const HotPlugEvent *event) override;
};
}  // namespace Input
}  // namespace OHOS
#endif // INPUT_HOST_CALLBACK_H