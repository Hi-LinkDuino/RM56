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
#ifndef INPUT_PROXY_H
#define INPUT_PROXY_H

#include <iremote_proxy.h>
#include <thread>
#include "iinput_host.h"
#include "iinput_host_callbck.h"

namespace OHOS {
namespace Input {
class InputProxy : public IRemoteProxy<IInputHost> {
public:
    explicit InputProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IInputHost>(impl) {}
    virtual ~InputProxy() override {}
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
    virtual int32_t RegisterHotPlugCallback(OHOS::sptr<InputReportHostCb> callback) override;
    virtual int32_t UnregisterHotPlugCallback(void) override;
private:
    static constexpr int32_t CMD_INPUT_SCAN_DEVICE_PROXY = 0;
    static constexpr int32_t CMD_INPUT_OPNE_DEVICE_PROXY = 1;
    static constexpr int32_t CMD_INPUT_CLOSE_DEVICE_PROXY = 2;
    static constexpr int32_t CMD_INPUT_GET_DEVICE_PROXY = 3;
    static constexpr int32_t CMD_INPUT_GET_DEVICE_LIST_PROXY = 4;
    static constexpr int32_t CMD_INPUT_SET_POWER_STATUS_PROXY = 5;
    static constexpr int32_t CMD_INPUT_GET_POWER_STATUS_PROXY = 6;
    static constexpr int32_t CMD_INPUT_GET_DEVICE_TYPE_PROXY = 7;
    static constexpr int32_t CMD_INPUT_GET_CHIP_INFO_PROXY = 8;
    static constexpr int32_t CMD_INPUT_GET_VENDOR_NAME_PROXY = 9;
    static constexpr int32_t CMD_INPUT_GET_CHIP_NAME_PROXY = 10;
    static constexpr int32_t CMD_INPUT_SET_GESTURE_MODE_PROXY = 11;
    static constexpr int32_t CMD_INPUT_RUN_CAPACITANCE_TEST_PROXY = 12;
    static constexpr int32_t CMD_INPUT_RUN_EXTRA_COMMAND_PROXY = 13;
    static constexpr int32_t CMD_INPUT_REGISTER_DEVICE_PROXY = 14;
    static constexpr int32_t CMD_INPUT_UNREGISTER_DEVICE_PROXY = 15;
    static constexpr int32_t CMD_INPUT_REGISTER_HOT_PLAUS_DEVICE_PROXY = 16;
    static constexpr int32_t CMD_INPUT_UNREGISTER_HOT_PLAUS_DEVICE_PROXY = 17;
    static inline BrokerDelegator<InputProxy> delegator_;
    std::thread thread_;
};
}  // namespace Input
}  // namespace OHOS

#endif // INPUT_PROXY_H