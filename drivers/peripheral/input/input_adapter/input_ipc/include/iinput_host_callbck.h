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

#ifndef IINPUT_HOST_CALLBACK_H
#define IINPUT_HOST_CALLBACK_H

#include <iremote_broker.h>
#include "input_type.h"

namespace OHOS {
namespace Input {
enum {
    CMD_INPUT_CALLBACK_REMOTE_REPROT_EVENT = 100,
    CMD_INPUT_CALLBACK_REMOTE_REPROT_HOT_PLUG_EVENT
};
class InputReportEventCb : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.INPUT.V1_0.InputReportEventCb");
    virtual ~InputReportEventCb() {}
public:
   /**
     * @brief Reports input event data by the registered callback.
     *
     * @param pkgs describes the input event data package.
     * @param count Indicates the number of input event data packets.
     * @param devIndex Indicates the index of an input device.
     * @since 1.0
     * @version 1.0
     */
    virtual void ReportEventPkgCallback(const EventPackage *pkgs, uint32_t count, uint32_t devIndex) = 0;
};
class InputReportHostCb : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.INPUT.V1_0.InputReportHostCb");
    virtual ~InputReportHostCb() {}
public:
    /**
     * @brief Reports hot plug event data by the registered callback.
     *
     * @param event Indicates the pointer to the hot plug event data reported by the input driver.
     * @since 1.0
     * @version 1.0
     */
    virtual void ReportHotPlugEventCallback(const HotPlugEvent *event) = 0;
};
}  // namespace Input
}  // namespace OHOS
#endif // IINPUT_HOST_CALLBACK_H
