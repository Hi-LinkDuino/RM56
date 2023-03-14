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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines device some major class.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file bluetooth_device.h
 *
 * @brief Bluetooth device major class.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef BLUETOOTH_DEVICE_H_
#define BLUETOOTH_DEVICE_H_

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Represents device major class.
 *
 * @since 1.0
 * @version 1.0
 */
class BluetoothDevice {
public:
    // Major Service Classes
    static const int SERVICE_LIMITED_DISCOVERABILITY = 0x002000;
    static const int SERVICE_POSITIONING = 0x010000;
    static const int SERVICE_NETWORKING = 0x020000;
    static const int SERVICE_RENDER = 0x040000;
    static const int SERVICE_CAPTURE = 0x080000;
    static const int SERVICE_OBJECT_TRANSFER = 0x100000;
    static const int SERVICE_AUDIO = 0x200000;
    static const int SERVICE_TELEPHONY = 0x400000;
    static const int SERVICE_INFORMATION = 0x800000;

    // Major Device Classes
    static const int MAJOR_MISC = 0x0000;
    static const int MAJOR_COMPUTER = 0x0100;
    static const int MAJOR_PHONE = 0x0200;
    static const int MAJOR_NETWORKING = 0x0300;
    static const int MAJOR_AUDIO_VIDEO = 0x0400;
    static const int MAJOR_PERIPHERAL = 0x0500;
    static const int MAJOR_IMAGING = 0x0600;
    static const int MAJOR_WEARABLE = 0x0700;
    static const int MAJOR_TOY = 0x0800;
    static const int MAJOR_HEALTH = 0x0900;
    static const int MAJOR_UNCATEGORIZED = 0x1F00;

    // The Minor Device Class field
    // Computer Major Class
    static const int COMPUTER_UNCATEGORIZED = 0x0100;
    static const int COMPUTER_DESKTOP = 0x0104;
    static const int COMPUTER_SERVER = 0x0108;
    static const int COMPUTER_LAPTOP = 0x010C;
    static const int COMPUTER_HANDHELD_PC_PDA = 0x0110;
    static const int COMPUTER_PALM_SIZE_PC_PDA = 0x0114;
    static const int COMPUTER_WEARABLE = 0x0118;
    static const int COMPUTER_TABLET = 0x011C;

    // Phone Major Class
    static const int PHONE_UNCATEGORIZED = 0x0200;
    static const int PHONE_CELLULAR = 0x0204;
    static const int PHONE_CORDLESS = 0x0208;
    static const int PHONE_SMART = 0x020C;
    static const int PHONE_MODEM_OR_GATEWAY = 0x0210;
    static const int PHONE_ISDN = 0x0214;

    // LAN/Network Access Point Major Class
    static const int NETWORK_FULLY_AVAILABLE = 0x0300;
    static const int NETWORK_1_TO_17_UTILIZED = 0x0320;
    static const int NETWORK_17_TO_33_UTILIZED = 0x0340;
    static const int NETWORK_33_TO_50_UTILIZED = 0x0360;
    static const int NETWORK_60_TO_67_UTILIZED = 0x0380;
    static const int NETWORK_67_TO_83_UTILIZED = 0x03A0;
    static const int NETWORK_83_TO_99_UTILIZED = 0x03C0;
    static const int NETWORK_NO_SERVICE = 0x03E0;

    // Audio/Video Major Class
    static const int AUDIO_VIDEO_UNCATEGORIZED = 0x0400;
    static const int AUDIO_VIDEO_WEARABLE_HEADSET = 0x0404;
    static const int AUDIO_VIDEO_HANDSFREE = 0x0408;
    static const int AUDIO_VIDEO_HANDSFREE_HEADSET = 0X0104;
    // static const int AUDIO_VIDEO_RESERVED  = 0x040C;
    static const int AUDIO_VIDEO_MICROPHONE = 0x0410;
    static const int AUDIO_VIDEO_LOUDSPEAKER = 0x0414;
    static const int AUDIO_VIDEO_HEADPHONES = 0x0418;
    static const int AUDIO_VIDEO_PORTABLE_AUDIO = 0x041C;
    static const int AUDIO_VIDEO_CAR_AUDIO = 0x0420;
    static const int AUDIO_VIDEO_SET_TOP_BOX = 0x0424;
    static const int AUDIO_VIDEO_HIFI_AUDIO = 0x0428;
    static const int AUDIO_VIDEO_VCR = 0x042C;
    static const int AUDIO_VIDEO_VIDEO_CAMERA = 0x0430;
    static const int AUDIO_VIDEO_CAMCORDER = 0x0434;
    static const int AUDIO_VIDEO_VIDEO_MONITOR = 0x0438;
    static const int AUDIO_VIDEO_VIDEO_DISPLAY_AND_LOUDSPEAKER = 0x043C;
    static const int AUDIO_VIDEO_VIDEO_CONFERENCING = 0x0440;
    // static const int AUDIO_VIDEO_RESERVED  = 0x0444;
    static const int AUDIO_VIDEO_VIDEO_GAMING_TOY = 0x0448;

    // Peripheral Major Class
    static const int PERIPHERAL_NON_KEYBOARD_NON_POINTING = 0x0500;
    static const int PERIPHERAL_KEYBOARD = 0x0540;
    static const int PERIPHERAL_POINTING_DEVICE = 0x0580;
    static const int PERIPHERAL_KEYBOARD_POINTING = 0x05C0;
    static const int PERIPHERAL_UNCATEGORIZED = 0x0500;
    static const int PERIPHERAL_JOYSTICK = 0x0504;
    static const int PERIPHERAL_GAMEPAD = 0x0508;
    static const int PERIPHERAL_REMOTE_CONTROL = 0x05C0;
    static const int PERIPHERAL_SENSING_DEVICE = 0x0510;
    static const int PERIPHERAL_DIGITIZER_TABLET = 0x0514;
    static const int PERIPHERAL_CARD_READER = 0x0518;
    static const int PERIPHERAL_DIGITAL_PEN = 0x051C;
    static const int PERIPHERAL_SCANNER_RFID = 0x0520;
    static const int PERIPHERAL_GESTURAL_INPUT = 0x0522;

    // Imaging Major Class
    static const int IMAGING_UNCATEGORIZED = 0x0600;
    static const int IMAGING_DISPLAY = 0x0610;
    static const int IMAGING_CAMERA = 0x0620;
    static const int IMAGING_SCANNER = 0x0640;
    static const int IMAGING_PRINTER = 0x0680;

    // Wearable Major Class
    static const int WEARABLE_UNCATEGORIZED = 0x0700;
    static const int WEARABLE_WRIST_WATCH = 0x0704;
    static const int WEARABLE_PAGER = 0x0708;
    static const int WEARABLE_JACKET = 0x070C;
    static const int WEARABLE_HELMET = 0x0710;
    static const int WEARABLE_GLASSES = 0x0714;

    // Minor Device Class field - Toy Major Class
    static const int TOY_UNCATEGORIZED = 0x0800;
    static const int TOY_ROBOT = 0x0804;
    static const int TOY_VEHICLE = 0x0808;
    static const int TOY_DOLL_ACTION_FIGURE = 0x080C;
    static const int TOY_CONTROLLER = 0x0810;
    static const int TOY_GAME = 0x0814;

    // Minor Device Class field - Health
    static const int HEALTH_UNCATEGORIZED = 0x0900;
    static const int HEALTH_BLOOD_PRESSURE = 0x0904;
    static const int HEALTH_THERMOMETER = 0x0908;
    static const int HEALTH_WEIGHING = 0x090C;
    static const int HEALTH_GLUCOSE = 0x0910;
    static const int HEALTH_PULSE_OXIMETER = 0x0914;
    static const int HEALTH_PULSE_RATE = 0x0918;
    static const int HEALTH_DATA_DISPLAY = 0x091C;
    static const int HEALTH_STEP_COUNTER = 0x0920;
    static const int HEALTH_BODY_COMPOSITION_ANALYZER = 0x0924;
    static const int HEALTH_PEAK_FLOW_MOITOR = 0x0928;
    static const int HEALTH_MEDICATION_MONITOR = 0x092C;
    static const int HEALTH_KNEE_PROSTHESIS = 0x0930;
    static const int HEALTH_ANKLE_PROSTHESIS = 0x0934;
    static const int HEALTH_GENERIC_HEALTH_MANAGER = 0x0938;
    static const int HEALTH_PERSONAL_MOBILITY_DEVICE = 0x093C;

    // Error
    static const int DEVICE_ERROR = 0xFF000000;

    static const int SERVICE_BITMASK = 0xFFE000;
    static const int MAJOR_BITMASK = 0x1F00;
    static const int DEVICE_BITMASK = 0x1FFC;

    static const int PROFILE_HEADSET = 0;
    static const int PROFILE_A2DP = 1;
    static const int PROFILE_OPP = 2;
    static const int PROFILE_HID = 3;
    static const int PROFILE_PANU = 4;
    static const int PROFILE_NAP = 5;
    static const int PROFILE_A2DP_SINK = 6;
};
} // namespace Bluetooth
} // namespace OHOS

#endif  // BLUETOOTH_DEVICE_H_
