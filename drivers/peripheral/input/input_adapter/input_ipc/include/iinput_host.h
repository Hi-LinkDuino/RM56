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
#ifndef IINPUT_HOST_H
#define IINPUT_HOST_H

#include <list>
#include <map>
#include <vector>
#include <hdf_log.h>
#include <iservmgr_hdi.h>
#include "iinput_host_callbck.h"

namespace OHOS {
namespace Input {
enum {
    CMD_INPUT_SCAN_DEVICE = 0,
    CMD_INPUT_OPNE_DEVICE,
    CMD_INPUT_CLOSE_DEVICE,
    CMD_INPUT_GET_DEVICE,
    CMD_INPUT_GET_DEVICE_LIST,
    CMD_INPUT_SET_POWER_STATUS,
    CMD_INPUT_GET_POWER_STATUS,
    CMD_INPUT_GET_DEVICE_TYPE,
    CMD_INPUT_GET_CHIP_INFO,
    CMD_INPUT_GET_VENDOR_NAME,
    CMD_INPUT_GET_CHIP_NAME,
    CMD_INPUT_SET_GESTURE_MODE,
    CMD_INPUT_RUN_CAPACITANCE_TEST,
    CMD_INPUT_RUN_EXTRA_COMMAND,
    CMD_INPUT_REGISTER_DEVICE,
    CMD_INPUT_UNREGISTER_DEVICE,
    CMD_INPUT_REGISTER_HOT_PLAUS_DEVICE,
    CMD_INPUT_UNREGISTER_HOT_PLAUS_DEVICE,
};

class IInputHost : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.IInputHost.V1_0");

    virtual ~IInputHost() {}

    /**
     * @brief Scans all online input devices.
     *
     * @param staArr Indicates the pointer to the array storing information about the scanned input devices,
     * including the device index and device type.
     * @param arrLen Indicates the length of the array.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t ScanInputDevice(DevDesc *staArr, uint32_t arrLen) = 0;

    /**
     * @brief Opens a specified input device file.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t OpenInputDevice(uint32_t devIndex) = 0;

    /**
     * @brief Closes a specified input device file.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t CloseInputDevice(uint32_t devIndex) = 0;

    /**
     * @brief Gets information about a specified input device.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param devInfo Indicates the double pointer to information about the specified device.
     * For details, see {@link DeviceInfo}.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetInputDevice(uint32_t devIndex, DeviceInfo **devInfo) = 0;

    /**
     * @brief Gets information about all input devices in the device list.
     *
     * @param devNum Indicates the pointer to the total number of input devices which have been registered.
     * @param devList Indicates the double pointer to information about all devices in the device list.
     * For details, see {@link DeviceInfo}.
     * @param size Indicates the number of elements in thce <b>devList</b> array.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetInputDeviceList(uint32_t *devNum, DeviceInfo **devList, uint32_t size) = 0;

    /**
     * @brief Sets the power status.
     *
     * This function is called only when the power status of the OS is changed. \n
     * The input service or the power management module can call this function to set the power status
     * for the input device when the OS is in the Resume or Suspend status, so that the driver integrated circuit (IC)
     * of the device can normally enter the specified status.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param status Indicates the power status to set. The input service will notify the input device of entering the
     * Resume or Suspend state specified by {@link PowerStatus}.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetPowerStatus(uint32_t devIndex, uint32_t status) = 0;

    /**
     * @brief Gets the power status.
     *
     * The input service or the power management module can get the power status for the input device when the OS
     * is in the Resume or Suspend status, so that the driver IC of the device can normally enter the specified status.
     * You can call this function to obtain the power status.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param status Indicates the pointer to the power status of the device. For details, see {@link PowerStatus}.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetPowerStatus(uint32_t devIndex, uint32_t *status) = 0;

    /**
     * @brief Gets the type of the input device based on the specified device index.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param deviceType Indicates the pointer to the device type. For details, see {@link InputDevType}.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetDeviceType(uint32_t devIndex, uint32_t *deviceType) = 0;

    /**
     * @brief Gets the chip information of the specified device.
     *
     * A product is usually equipped with modules and driver ICs provided by multiple vendors. An input service
     * can call this function to get the specific information if needed.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param chipInfo Indicates the pointer to the chip information.
     * @param length Indicates the length of the chip information. The minimum value of length is 10.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetChipInfo(uint32_t devIndex, char *chipInfo, uint32_t length) = 0;

    /**
     * @brief Gets the module vendor name of the specified device.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param vendorName Indicates the pointer to the module vendor name.
     * @param length Indicates the length of the module vendor name. The minimum value of length is 10.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetVendorName(uint32_t devIndex, char *vendorName, uint32_t length) = 0;

    /**
     * @brief Gets the driver chip name of the specified device.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param chipName Indicates the pointer to the driver chip name.
     * @param length Indicates the length of the driver chip name. The minimum value of length is 10.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t GetChipName(uint32_t devIndex, char *chipName, uint32_t length) = 0;

    /**
     * @brief Sets the gesture mode.
     *
     * The input service can use this function to enable or disable the gesture mode by setting <b>EnableBit</b>
     * of the gesture mode.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param gestureMode Indicates the gesture mode to set.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t SetGestureMode(uint32_t devIndex, uint32_t gestureMode) = 0;

    /**
     * @brief Conducts a capacitance self-test.
     *
     * The capacitance self-test items are defined by the component vendor, such as the tests on the raw data,
     * short circuit, open circuit, interference, and row/column difference.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param testType Indicates the capacitance test type. For details, see {@link CapacitanceTest}.
     * @param result Indicates the pointer to the capacitance test result. The value is <b>SUCC</b> for a successful
     * operation and is an error code for a failed operation.
     * @param length Indicates the length of the test result. The minimum value of length is 20.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t RunCapacitanceTest(uint32_t devIndex, uint32_t testType, char *result, uint32_t length) = 0;

    /**
     * @brief Executes the extra command.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param cmd Indicates the pointer to the extra command data packet, including the command codes and parameters.
     * For details, see {@link InputExtraCmd}.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined
     * in {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t RunExtraCommand(uint32_t devIndex, InputExtraCmd *cmd) = 0;

    /**
     * @brief Registers a callback for reporting subscribed data of specified input devices.
     *
     * After this callback is successfully registered, the driver can report the input event data to the input service
     * through this callback.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @param callback Indicates the pointer to the callback to register.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t RegisterReportCallback(uint32_t devIndex, OHOS::sptr<InputReportEventCb> callback) = 0;

    /**
     * @brief Unregisters the callback for reporting subscribed data of specified input devices.
     *
     * @param devIndex Indicates the index of an input device. A maximum of 32 input devices are supported.
     * The value ranges from 0 to 31, and value <b>0</b> represents the first input device.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t UnregisterReportCallback(uint32_t devIndex) = 0;

    /**
     * @brief Registers a hot plug callback to the HDIs for input devices.
     *
     * All input devices can use this callback to report hot plug events.
     *
     * @param callback Indicates the pointer to the callback to register.
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t RegisterHotPlugCallback(OHOS::sptr<InputReportHostCb> callback) = 0;

    /**
     * @brief Unregisters the hot plug callback of input devices.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in
     * {@link RetStatus} otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t UnregisterHotPlugCallback(void) = 0;
    static OHOS::sptr<IInputHost> Get(const std::string& serviceName);
};
    sptr<IInputHost> IInputHost::Get(const std::string& serviceName)
    {
        do {
            using namespace OHOS::HDI::ServiceManager::V1_0;
            auto servMgr = IServiceManager::Get();
            if (servMgr == nullptr) {
                break;
            }
            sptr<IRemoteObject> remote = servMgr->GetService(serviceName.c_str());
            if (remote != nullptr) {
                HDF_LOGE("%{public}s: get %{public}s successful", __func__, serviceName.c_str());
                return iface_cast<IInputHost>(remote);
            }
        } while (false);
        return nullptr;
    }
}  // namespace Input
}  // namespace OHOS
#endif // IINPUT_HOST_H
