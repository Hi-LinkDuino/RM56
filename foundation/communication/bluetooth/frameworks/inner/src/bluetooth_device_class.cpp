/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "bluetooth_device_class.h"

#include "bluetooth_device.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
BluetoothDeviceClass::BluetoothDeviceClass() : class_(0)
{}

BluetoothDeviceClass::BluetoothDeviceClass(int deviceClass) : class_(deviceClass)
{}

BluetoothDeviceClass::~BluetoothDeviceClass()
{}

int BluetoothDeviceClass::GetMajorClass() const
{
    return (class_ & BluetoothDevice::MAJOR_BITMASK);
}

int BluetoothDeviceClass::GetMajorMinorClass() const
{
    int res = (class_ & BluetoothDevice::DEVICE_BITMASK);
    HILOGI("MajorMinorClass: 0x%{public}X", res);
    return res;
}

int BluetoothDeviceClass::GetClassOfDevice() const
{
    return class_;
}

bool BluetoothDeviceClass::IsProfileSupported(int profileId) const
{
    if (profileId == BluetoothDevice::PROFILE_A2DP) {
        return IsA2dpSupported();
    } else if (profileId == BluetoothDevice::PROFILE_A2DP_SINK) {
        return IsA2dpSinkSupported();
    } else if (profileId == BluetoothDevice::PROFILE_HEADSET) {
        return IsHeadSetSupported();
    } else if (profileId == BluetoothDevice::PROFILE_OPP) {
        return IsOppSupported();
    } else if (profileId == BluetoothDevice::PROFILE_HID) {
        return (GetMajorMinorClass() & BluetoothDevice::MAJOR_PERIPHERAL) ==
               BluetoothDevice::MAJOR_PERIPHERAL;
    } else if (profileId == BluetoothDevice::PROFILE_PANU ||
               profileId == BluetoothDevice::PROFILE_NAP) {
        if (IsServiceSupported(BluetoothDevice::SERVICE_NETWORKING)) {
            return true;
        }
        return (GetMajorMinorClass() & BluetoothDevice::MAJOR_NETWORKING) ==
               BluetoothDevice::MAJOR_NETWORKING;
    } else {
        return false;
    }
}

bool BluetoothDeviceClass::IsA2dpSupported() const
{
    if (IsServiceSupported(BluetoothDevice::SERVICE_RENDER)) {
        HILOGI("service supported.");
        return true;
    }
    switch (GetMajorMinorClass()) {
        case BluetoothDevice::AUDIO_VIDEO_HIFI_AUDIO:
        case BluetoothDevice::AUDIO_VIDEO_HEADPHONES:
        case BluetoothDevice::AUDIO_VIDEO_LOUDSPEAKER:
        case BluetoothDevice::AUDIO_VIDEO_CAR_AUDIO:
            return true;
        default:
            return false;
    }
}

bool BluetoothDeviceClass::IsA2dpSinkSupported() const
{
    if (IsServiceSupported(BluetoothDevice::SERVICE_CAPTURE)) {
        HILOGI("service supported.");
        return true;
    }
    switch (GetMajorMinorClass()) {
        case BluetoothDevice::AUDIO_VIDEO_HIFI_AUDIO:
        case BluetoothDevice::AUDIO_VIDEO_SET_TOP_BOX:
        case BluetoothDevice::AUDIO_VIDEO_VCR:
            return true;
        default:
            return false;
    }
}

bool BluetoothDeviceClass::IsHeadSetSupported() const
{
    if (IsServiceSupported(BluetoothDevice::SERVICE_RENDER)) {
        HILOGI("service supported.");
        return true;
    }
    switch (GetMajorMinorClass()) {
        case BluetoothDevice::AUDIO_VIDEO_HANDSFREE:
        case BluetoothDevice::AUDIO_VIDEO_WEARABLE_HEADSET:
        case BluetoothDevice::AUDIO_VIDEO_CAR_AUDIO:
        case BluetoothDevice::AUDIO_VIDEO_HANDSFREE_HEADSET:
            return true;
        default:
            return false;
    }
}

bool BluetoothDeviceClass::IsOppSupported() const
{
    if (IsServiceSupported(BluetoothDevice::SERVICE_OBJECT_TRANSFER)) {
        return true;
    }

    switch (GetMajorMinorClass()) {
        case BluetoothDevice::COMPUTER_UNCATEGORIZED:
        case BluetoothDevice::COMPUTER_DESKTOP:
        case BluetoothDevice::COMPUTER_SERVER:
        case BluetoothDevice::COMPUTER_LAPTOP:
        case BluetoothDevice::COMPUTER_HANDHELD_PC_PDA:
        case BluetoothDevice::COMPUTER_PALM_SIZE_PC_PDA:
        case BluetoothDevice::COMPUTER_WEARABLE:
        case BluetoothDevice::PHONE_UNCATEGORIZED:
        case BluetoothDevice::PHONE_CELLULAR:
        case BluetoothDevice::PHONE_CORDLESS:
        case BluetoothDevice::PHONE_SMART:
        case BluetoothDevice::PHONE_MODEM_OR_GATEWAY:
        case BluetoothDevice::PHONE_ISDN:
            return true;
        default:
            return false;
    }
}

bool BluetoothDeviceClass::IsServiceSupported(int service) const
{
    return ((class_ & BluetoothDevice::SERVICE_BITMASK & service) != 0);
}
}  // namespace Bluetooth
}  // namespace OHOS