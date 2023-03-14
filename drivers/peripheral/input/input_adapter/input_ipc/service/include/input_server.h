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
#ifndef INPUT_SERVICE_H
#define INPUT_SERVICE_H

#include "iinput_host.h"
#include "input_device_manager.h"

namespace OHOS {
namespace Input {
class InputServer : public IInputHost {
public:
    InputServer();
    virtual ~InputServer() override {}
    virtual int32_t ScanInputDevice(DevDesc *staArr, uint32_t arrLen) override;
    virtual int32_t OpenInputDevice(uint32_t devIndex) override;
    virtual int32_t CloseInputDevice(uint32_t devIndex) override;
    virtual int32_t GetInputDevice(uint32_t devIndex, DeviceInfo **devInfo) override;
    virtual int32_t GetInputDeviceList(uint32_t *devNum, DeviceInfo **devList, uint32_t size) override;
    virtual int32_t SetPowerStatus(uint32_t devIndex, uint32_t status) override;
    virtual int32_t GetPowerStatus(uint32_t devIndex, uint32_t *status) override;
    virtual int32_t GetDeviceType(uint32_t devIndex, uint32_t *status) override;
    virtual int32_t GetChipInfo(uint32_t devIndex, char *chipInfo, uint32_t length) override;
    virtual int32_t GetVendorName(uint32_t devIndex, char *vendorName, uint32_t length) override;
    virtual int32_t GetChipName(uint32_t devIndex, char *chipName, uint32_t length) override;
    virtual int32_t SetGestureMode(uint32_t devIndex, uint32_t gestureMode) override;
    virtual int32_t RunCapacitanceTest(uint32_t devIndex, uint32_t testType, char *result, uint32_t length) override;
    virtual int32_t RunExtraCommand(uint32_t devIndex, InputExtraCmd *cmd) override;
    virtual int32_t RegisterReportCallback(uint32_t devIndex, OHOS::sptr<InputReportEventCb> callback) override;
    virtual int32_t UnregisterReportCallback(uint32_t devIndex) override;
    virtual int32_t RegisterHotPlugCallback(sptr<InputReportHostCb> callback) override;
    virtual int32_t UnregisterHotPlugCallback(void) override;
    static std::map<int32_t, OHOS::sptr<InputReportEventCb>> reportEventPkgCallback_;
    OHOS::sptr<InputReportHostCb> reportHotPlugEventCallback_;
    std::shared_ptr<InputDeviceManager> inputDeviceManager_;
};
}  // namespace Input
}  // namespace OHOS
#endif // INPUT_SERVICE_H
