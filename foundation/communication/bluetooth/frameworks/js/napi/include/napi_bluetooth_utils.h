/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef NAPI_BLUETOOTH_UTILS_H
#define NAPI_BLUETOOTH_UTILS_H

#include "bluetooth_gatt_characteristic.h"
#include "bluetooth_gatt_client.h"
#include "bluetooth_gatt_descriptor.h"
#include "bluetooth_gatt_server.h"
#include "bluetooth_gatt_service.h"
#include "bluetooth_log.h"
#include "bluetooth_opp.h"
#include "bluetooth_remote_device.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stdint.h>
#include <string>
#include <vector>

#include "uv.h"

#include "bluetooth_socket.h"

namespace OHOS {
namespace Bluetooth {
constexpr size_t CALLBACK_SIZE = 1;
constexpr size_t ARGS_SIZE_ZERO = 0;
constexpr size_t ARGS_SIZE_ONE = 1;
constexpr size_t ARGS_SIZE_TWO = 2;
constexpr size_t ARGS_SIZE_THREE = 3;
constexpr size_t ARGS_SIZE_FOUR = 4;
constexpr int32_t DEFAULT_INT32 = 0;
constexpr int32_t PARAM0 = 0;
constexpr int32_t PARAM1 = 1;
constexpr int32_t PARAM2 = 2;
constexpr int32_t CODE_SUCCESS = 0;
constexpr int32_t CODE_FAILED = -1;
constexpr int ASYNC_IDLE = 0;
constexpr int ASYNC_START = 1;
constexpr int ASYNC_DONE = 2;
constexpr int32_t THREAD_WAIT_TIMEOUT = 5;
constexpr int32_t BLUETOOTH_DEVICE_FIND_TYPE = 1;
constexpr int32_t STATE_CHANGE_TYPE = 2;
constexpr int32_t PIN_REQUEST_TYPE = 3;
constexpr int32_t BOND_STATE_CHANGE_TYPE = 4;
constexpr int32_t BLE_DEVICE_FIND_TYPE = 5;

constexpr uint32_t INVALID_REF_COUNT = 0xFF;

struct ServerResponse {
    std::string deviceId = "";
    int transId = 0;
    int status = 0;
    int offset = 0;
    uint8_t *value = nullptr;
    int length = 0;
    void SetValue(uint8_t *values, size_t len)
    {
        HILOGI("GattCharacteristic::SetValue starts");
        if (value != nullptr) {
            value = nullptr;
        }

        length = len;
        value = values;
    }
};

struct SppOption {
    std::string uuid_ = "";
    bool secure_ = false;
    SppSocketType type_;
};

const std::string REGISTER_DEVICE_FIND_TYPE = "bluetoothDeviceFind";
const std::string REGISTER_STATE_CHANGE_TYPE = "stateChange";
const std::string REGISTER_PIN_REQUEST_TYPE = "pinRequired";
const std::string REGISTER_BONE_STATE_TYPE = "bondStateChange";
const std::string REGISTER_BLE_FIND_DEVICE_TYPE = "BLEDeviceFind";
const std::string REGISTER_SYS_BLE_SCAN_TYPE = "sysBLEScan";
const std::string REGISTER_SYS_BLE_FIND_DEVICE_TYPE = "sysBLEDeviceFonud";

const std::string INVALID_DEVICE_ID = "00:00:00:00:00:00";
const std::string INVALID_PIN_CODE = "000000";

bool ParseString(napi_env env, std::string &param, napi_value args);
bool ParseInt32(napi_env env, int32_t &param, napi_value args);
bool ParseBool(napi_env env, bool &param, napi_value args);
bool ParseArrayBuffer(napi_env env, uint8_t **data, size_t &size, napi_value args);
napi_value GetCallbackErrorValue(napi_env env, int errCode);

void ConvertStringVectorToJS(napi_env env, napi_value result, std::vector<std::string> &stringVector);

void ConvertGattServiceToJS(napi_env env, napi_value result, GattService &service);
void ConvertGattServiceVectorToJS(napi_env env, napi_value result, std::vector<GattService> &services);

void ConvertBLECharacteristicToJS(napi_env env, napi_value result, GattCharacteristic &characteristic);
void ConvertBLECharacteristicVectorToJS(
    napi_env env, napi_value result, std::vector<GattCharacteristic> &characteristics);

void ConvertBLEDescriptorToJS(napi_env env, napi_value result, GattDescriptor &descriptor);
void ConvertBLEDescriptorVectorToJS(napi_env env, napi_value result, std::vector<GattDescriptor> &descriptors);

void ConvertCharacteristicReadReqToJS(napi_env env, napi_value result, const std::string &device,
    GattCharacteristic &characteristic, int requestId);

void ConvertCharacteristicWriteReqToJS(napi_env env, napi_value result, const std::string &device,
    GattCharacteristic &characteristic, int requestId);

void ConvertDescriptorReadReqToJS(
    napi_env env, napi_value result, const std::string &device, GattDescriptor &descriptor, int requestId);

void ConvertDescriptorWriteReqToJS(
    napi_env env, napi_value result, const std::string &device, GattDescriptor &descriptor, int requestId);
void ConvertStateChangeParamToJS(napi_env env, napi_value result, const std::string &device, int state);
void ConvertScoStateChangeParamToJS(napi_env env, napi_value result, const std::string &device, int state);

void ConvertOppTransferInformationToJS(napi_env env,
    napi_value result, BluetoothOppTransferInformation& transferInformation);

bool GetServiceVectorFromJS(napi_env env, napi_value object, std::vector<GattService> &services,
    std::shared_ptr<GattServer> server, std::shared_ptr<GattClient> client);
GattService *GetServiceFromJS(
    napi_env env, napi_value object, std::shared_ptr<GattServer> server, std::shared_ptr<GattClient> client);

bool GetCharacteristicVectorFromJS(napi_env env, napi_value object, std::vector<GattCharacteristic> &characteristics,
    std::shared_ptr<GattServer> server, std::shared_ptr<GattClient> client);
GattCharacteristic *GetCharacteristicFromJS(
    napi_env env, napi_value object, std::shared_ptr<GattServer> server, std::shared_ptr<GattClient> client);

bool GetDescriptorVectorFromJS(napi_env env, napi_value object, std::vector<GattDescriptor> &descriptors);
GattDescriptor *GetDescriptorFromJS(
    napi_env env, napi_value object, std::shared_ptr<GattServer> server, std::shared_ptr<GattClient> client);

ServerResponse GetServerResponseFromJS(napi_env env, napi_value object);
std::shared_ptr<SppOption> GetSppOptionFromJS(napi_env env, napi_value object);

void SetNamedPropertyByInteger(napi_env env, napi_value dstObj, int32_t objName, const char *propName);
napi_value NapiGetNull(napi_env env);
napi_value NapiGetBooleanFalse(napi_env env);
napi_value NapiGetBooleanTrue(napi_env env);
napi_value RegisterObserver(napi_env env, napi_callback_info info);
napi_value DeregisterObserver(napi_env env, napi_callback_info info);

int GetProfileConnectionState(int state);
int GetScoConnectionState(int state);
uint32_t GetProfileId(int profile);

struct AsyncCallbackInfo {
    napi_env env_;
    napi_async_work asyncWork_;
    napi_deferred deferred_;
    napi_ref callback_ = 0;
    int errorCode_ = 0;
};

struct ReadCharacteristicValueCallbackInfo : public AsyncCallbackInfo {
    GattCharacteristic *inputCharacteristic_ = nullptr;
    const GattCharacteristic *outputCharacteristic_ = nullptr;
    int ret = -1;
    std::shared_ptr<GattClient> client_ = nullptr;
    int asyncState_ = ASYNC_IDLE;
};

struct ReadDescriptorValueCallbackInfo : public AsyncCallbackInfo {
    GattDescriptor *inputDescriptor_ = nullptr;
    const GattDescriptor *outputDescriptor_ = nullptr;
    int ret = -1;
    std::shared_ptr<GattClient> client_ = nullptr;
    int asyncState_ = ASYNC_IDLE;
};

struct GetServiceCallbackInfo : public AsyncCallbackInfo {
    std::vector<GattService> services_;
    std::shared_ptr<GattClient> client_ = nullptr;
};

struct SppListenCallbackInfo : public AsyncCallbackInfo {
    std::shared_ptr<SppServerSocket> server_ = nullptr;
    std::string name_ = "";
    std::shared_ptr<SppOption> sppOption_;
};

struct SppAcceptCallbackInfo : public AsyncCallbackInfo {
    std::shared_ptr<SppServerSocket> server_ = nullptr;
    std::shared_ptr<SppClientSocket> client_ = nullptr;
};

struct SppConnectCallbackInfo : public AsyncCallbackInfo {
    std::shared_ptr<SppClientSocket> client_ = nullptr;
    std::string deviceId_ = "";
    std::shared_ptr<BluetoothRemoteDevice> device_ = nullptr;
    std::shared_ptr<SppOption> sppOption_ = nullptr;
};

struct CallbackPromiseInfo {
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    bool isCallback = false;
    int errorCode = CODE_FAILED;
};

struct GattGetDeviceNameCallbackInfo {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    std::string deviceId = "";
    CallbackPromiseInfo promise;
};

struct GattGetRssiValueCallbackInfo {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    int32_t rssi = 0;
    std::mutex mutexRssi {};
    std::condition_variable cvfull {};
    CallbackPromiseInfo promise;
};

struct BluetoothCallbackInfo {
    napi_env env_;
    napi_ref callback_ = 0;
    int state_;
    std::string deviceId_;
    int info_;
};

struct GattCharacteristicCallbackInfo : public BluetoothCallbackInfo {
    GattCharacteristic characteristic_ = {UUID::FromString("0"), 0, 0};
};

struct GattDescriptorCallbackInfo : public BluetoothCallbackInfo {
    GattDescriptor descriptor_ = {UUID::FromString("0"), 0};
};

struct BufferCallbackInfo : public BluetoothCallbackInfo {
    char buffer_[1024];
};

struct TransforInformationCallbackInfo : public BluetoothCallbackInfo {
    BluetoothOppTransferInformation information_;
};

namespace {
using sysBLEMap = std::map<std::string, std::array<std::shared_ptr<BluetoothCallbackInfo>, ARGS_SIZE_THREE>>;
sysBLEMap g_sysBLEObserver;
std::mutex g_sysBLEObserverMutex;
std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> g_Observer;
std::mutex g_observerMutex;
std::shared_ptr<GattGetRssiValueCallbackInfo> callbackInfo = nullptr;
std::string deviceAddr;
std::atomic<bool> isCurrentAppOperate(false);
}  // namespace
std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> GetObserver();
const sysBLEMap &GetSysBLEObserver();
void SetGattClientDeviceId(const std::string &deviceId);
std::string GetGattClientDeviceId();
std::shared_ptr<BluetoothCallbackInfo> GetCallbackInfoByType(const std::string type);

void SetRssiValueCallbackInfo(std::shared_ptr<GattGetRssiValueCallbackInfo> &callback);
std::shared_ptr<GattGetRssiValueCallbackInfo> GetRssiValueCallbackInfo();

void SetCurrentAppOperate(const bool &isCurrentApp);
bool GetCurrentAppOperate();
void RegisterSysBLEObserver(const std::shared_ptr<BluetoothCallbackInfo> &, int32_t, const std::string &);
void UnregisterSysBLEObserver(const std::string &);

struct ScanFilter {
    std::string deviceId;     // The name of a BLE peripheral device
    std::string name;         // The name of a BLE peripheral device
    UUID serviceUuid;  // The service UUID of a BLE peripheral device
    UUID serviceUuidMask;
    UUID serviceSolicitationUuid;
    UUID serviceSolicitationUuidMask;

    std::vector<uint8_t> serviceData;
    std::vector<uint8_t> serviceDataMask;

    uint16_t manufacturerId = 0;
    std::vector<uint8_t> manufactureData;
    std::vector<uint8_t> manufactureDataMask;
};

enum MatchMode {
    MATCH_MODE_AGGRESSIVE = 1,  //  aggressive mode
    MATCH_MODE_STICKY = 2       // sticky mode
};

enum class ScanDuty {
    SCAN_MODE_LOW_POWER = 0,   // low power mode */
    SCAN_MODE_BALANCED = 1,    // balanced power mode
    SCAN_MODE_LOW_LATENCY = 2  // Scan using highest duty cycle
};

struct ScanOptions {
    int32_t interval;                                        // Time of delay for reporting the scan result
    ScanDuty dutyMode = ScanDuty::SCAN_MODE_LOW_POWER;       // Bluetooth LE scan mode
    MatchMode MatchMode = MatchMode::MATCH_MODE_AGGRESSIVE;  // Match mode for Bluetooth LE scan filters hardware match
};

struct ScanResult {
    std::string deviceId;       // Address of the scanned device
    int32_t rssi;               // RSSI of the remote device
    std::vector<uint8_t> data;  // The raw data of broadcast packet
};
enum ProfileConnectionState {
    STATE_DISCONNECTED = 0,  // the current profile is disconnected
    STATE_CONNECTING = 1,    // the current profile is being connected
    STATE_CONNECTED = 2,     // the current profile is connected
    STATE_DISCONNECTING = 3  // the current profile is being disconnected
};

enum ScoState {
    SCO_DISCONNECTED,
    SCO_CONNECTING,
    SCO_DISCONNECTING,
    SCO_CONNECTED
};

enum class BluetoothState {
    /** Indicates the local Bluetooth is off */
    STATE_OFF = 0,
    /** Indicates the local Bluetooth is turning on */
    STATE_TURNING_ON = 1,
    /** Indicates the local Bluetooth is on, and ready for use */
    STATE_ON = 2,
    /** Indicates the local Bluetooth is turning off */
    STATE_TURNING_OFF = 3,
    /** Indicates the local Bluetooth is turning LE mode on */
    STATE_BLE_TURNING_ON = 4,
    /** Indicates the local Bluetooth is in LE only mode */
    STATE_BLE_ON = 5,
    /** Indicates the local Bluetooth is turning off LE only mode */
    STATE_BLE_TURNING_OFF = 6
};

enum BondState {
    BOND_STATE_INVALID = 0,
    BOND_STATE_BONDING = 1,
    BOND_STATE_BONDED = 2
};

enum class ScanMode {
    SCAN_MODE_NONE = 0,
    SCAN_MODE_CONNECTABLE = 1,
    SCAN_MODE_GENERAL_DISCOVERABLE = 2,
    SCAN_MODE_LIMITED_DISCOVERABLE = 3,
    SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE = 4,
    SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE = 5
};

enum MajorClass {
    MAJOR_MISC = 0x0000,
    MAJOR_COMPUTER = 0x0100,
    MAJOR_PHONE = 0x0200,
    MAJOR_NETWORKING = 0x0300,
    MAJOR_AUDIO_VIDEO = 0x0400,
    MAJOR_PERIPHERAL = 0x0500,
    MAJOR_IMAGING = 0x0600,
    MAJOR_WEARABLE = 0x0700,
    MAJOR_TOY = 0x0800,
    MAJOR_HEALTH = 0x0900,
    MAJOR_UNCATEGORIZED = 0x1F00
};

enum MajorMinorClass {
    // The Minor Device Class field
    // Computer Major Class
    COMPUTER_UNCATEGORIZED = 0x0100,
    COMPUTER_DESKTOP = 0x0104,
    COMPUTER_SERVER = 0x0108,
    COMPUTER_LAPTOP = 0x010C,
    COMPUTER_HANDHELD_PC_PDA = 0x0110,
    COMPUTER_PALM_SIZE_PC_PDA = 0x0114,
    COMPUTER_WEARABLE = 0x0118,
    COMPUTER_TABLET = 0x011C,

    // Phone Major Class
    PHONE_UNCATEGORIZED = 0x0200,
    PHONE_CELLULAR = 0x0204,
    PHONE_CORDLESS = 0x0208,
    PHONE_SMART = 0x020C,
    PHONE_MODEM_OR_GATEWAY = 0x0210,
    PHONE_ISDN = 0x0214,

    // LAN/Network Access Point Major Class
    NETWORK_FULLY_AVAILABLE = 0x0300,
    NETWORK_1_TO_17_UTILIZED = 0x0320,
    NETWORK_17_TO_33_UTILIZED = 0x0340,
    NETWORK_33_TO_50_UTILIZED = 0x0360,
    NETWORK_60_TO_67_UTILIZED = 0x0380,
    NETWORK_67_TO_83_UTILIZED = 0x03A0,
    NETWORK_83_TO_99_UTILIZED = 0x03C0,
    NETWORK_NO_SERVICE = 0x03E0,

    // Audio/Video Major Class
    AUDIO_VIDEO_UNCATEGORIZED = 0x0400,
    AUDIO_VIDEO_WEARABLE_HEADSET = 0x0404,
    AUDIO_VIDEO_HANDSFREE = 0x0408,
    AUDIO_VIDEO_MICROPHONE = 0x0410,
    AUDIO_VIDEO_LOUDSPEAKER = 0x0414,
    AUDIO_VIDEO_HEADPHONES = 0x0418,
    AUDIO_VIDEO_PORTABLE_AUDIO = 0x041C,
    AUDIO_VIDEO_CAR_AUDIO = 0x0420,
    AUDIO_VIDEO_SET_TOP_BOX = 0x0424,
    AUDIO_VIDEO_HIFI_AUDIO = 0x0428,
    AUDIO_VIDEO_VCR = 0x042C,
    AUDIO_VIDEO_VIDEO_CAMERA = 0x0430,
    AUDIO_VIDEO_CAMCORDER = 0x0434,
    AUDIO_VIDEO_VIDEO_MONITOR = 0x0438,
    AUDIO_VIDEO_VIDEO_DISPLAY_AND_LOUDSPEAKER = 0x043C,
    AUDIO_VIDEO_VIDEO_CONFERENCING = 0x0440,
    AUDIO_VIDEO_VIDEO_GAMING_TOY = 0x0448,

    // Peripheral Major Class
    PERIPHERAL_NON_KEYBOARD_NON_POINTING = 0x0500,
    PERIPHERAL_KEYBOARD = 0x0540,
    PERIPHERAL_POINTING_DEVICE = 0x0580,
    PERIPHERAL_KEYBOARD_POINTING = 0x05C0,
    PERIPHERAL_UNCATEGORIZED = 0x0500,
    PERIPHERAL_JOYSTICK = 0x0504,
    PERIPHERAL_GAMEPAD = 0x0508,
    PERIPHERAL_REMOTE_CONTROL = 0x05C0,
    PERIPHERAL_SENSING_DEVICE = 0x0510,
    PERIPHERAL_DIGITIZER_TABLET = 0x0514,
    PERIPHERAL_CARD_READER = 0x0518,
    PERIPHERAL_DIGITAL_PEN = 0x051C,
    PERIPHERAL_SCANNER_RFID = 0x0520,
    PERIPHERAL_GESTURAL_INPUT = 0x0522,

    // Imaging Major Class
    IMAGING_UNCATEGORIZED = 0x0600,
    IMAGING_DISPLAY = 0x0610,
    IMAGING_CAMERA = 0x0620,
    IMAGING_SCANNER = 0x0640,
    IMAGING_PRINTER = 0x0680,

    // Wearable Major Class
    WEARABLE_UNCATEGORIZED = 0x0700,
    WEARABLE_WRIST_WATCH = 0x0704,
    WEARABLE_PAGER = 0x0708,
    WEARABLE_JACKET = 0x070C,
    WEARABLE_HELMET = 0x0710,
    WEARABLE_GLASSES = 0x0714,

    // Minor Device Class field - Toy Major Class
    TOY_UNCATEGORIZED = 0x0800,
    TOY_ROBOT = 0x0804,
    TOY_VEHICLE = 0x0808,
    TOY_DOLL_ACTION_FIGURE = 0x080C,
    TOY_CONTROLLER = 0x0810,
    TOY_GAME = 0x0814,

    // Minor Device Class field - Health
    HEALTH_UNCATEGORIZED = 0x0900,
    HEALTH_BLOOD_PRESSURE = 0x0904,
    HEALTH_THERMOMETER = 0x0908,
    HEALTH_WEIGHING = 0x090C,
    HEALTH_GLUCOSE = 0x0910,
    HEALTH_PULSE_OXIMETER = 0x0914,
    HEALTH_PULSE_RATE = 0x0918,
    HEALTH_DATA_DISPLAY = 0x091C,
    HEALTH_STEP_COUNTER = 0x0920,
    HEALTH_BODY_COMPOSITION_ANALYZER = 0x0924,
    HEALTH_PEAK_FLOW_MOITOR = 0x0928,
    HEALTH_MEDICATION_MONITOR = 0x092C,
    HEALTH_KNEE_PROSTHESIS = 0x0930,
    HEALTH_ANKLE_PROSTHESIS = 0x0934,
    HEALTH_GENERIC_HEALTH_MANAGER = 0x0938,
    HEALTH_PERSONAL_MOBILITY_DEVICE = 0x093C,
};

enum SppType {
    /** RFCOMM */
    SPP_RFCOMM = 0
};

enum PlayingState {
    STATE_NOT_PLAYING = 0,
    STATE_PLAYING = 1
};

enum ProfileId {
    PROFILE_A2DP_SINK = 0,
    PROFILE_A2DP_SOURCE = 1,
    PROFILE_AVRCP_CT = 2,
    PROFILE_AVRCP_TG = 3,
    PROFILE_HANDS_FREE_AUDIO_GATEWAY = 4,
    PROFILE_HANDS_FREE_UNIT = 5,
    PROFILE_HID_HOST = 6,
    PROFILE_PAN_NETWORK = 7,
    PROFILE_PBAP_CLIENT = 8,
    PROFILE_PBAP_SERVER = 9,
    PROFILE_OPP = 10
};

template<typename T1, typename T2, typename T3>
struct AfterWorkCallbackData {
    T1* object;
    T2 function;
    napi_env env;
    napi_ref callback;
    T3 data;
};

template<typename T>
void AfterWorkCallback(uv_work_t *work, int status)
{
    if (work == nullptr) {
        return;
    }
    T data = static_cast<T>(work->data);
    (data->object->*(data->function))(work, data->data);
    if (work->data != nullptr) {
        delete data;
        work->data = nullptr;
    }
    delete work;
    work = nullptr;
}
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // NAPI_BLUETOOTH_UTILS_H
