/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_BT_MODULE_H
#define OHOS_ACELITE_BT_MODULE_H

#include <cJSON.h>
#include "acelite_config.h"
#include "jsi.h"
#include "non_copyable.h"

namespace OHOS {
namespace ACELite {

typedef struct _bt_callback
{
    const char* m_event_owner;
    JSIValue m_callback;
    JSIValue m_this;
}bt_event_map_t;

#define BT_EVENT_MAP_NUM 11

class BtModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(BtModule);
    BtModule() = default;
    ~BtModule() = default;
    static JSIValue TestJSICallback(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothON(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothOFF(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothHungUp(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothAnswer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothScanCancel(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothConnect(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothBleWTInit(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothBleWTDeinit(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothBleWTStartScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothBleWTStopScan(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothBleWTStartCapture(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue BluetoothBleWTStopCapture(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);

    static int GetEventIdx(const JSIValue);
    static int SetEventMap(const JSIValue _owner, const JSIValue _callback, const JSIValue _this);
    static int SetEventMap(const int _idx, const JSIValue _callback, const JSIValue _this);

    static int GetEventMap(const JSIValue _owner, JSIValue* _callback, JSIValue* _this);
    static int GetEventMap(const int _idx, JSIValue* _callback, JSIValue* _this);
    static bt_event_map_t m_event_map[BT_EVENT_MAP_NUM];

    static JSIValue m_on_callback;
    static JSIValue m_on_this;
private:
    static uint8_t m_bt_scan_status;
};

void InitBlutetoothModule(JSIValue exports)
{
    JSI::SetModuleAPI(exports, "test_callback", BtModule::TestJSICallback);
    JSI::SetModuleAPI(exports, "on",            BtModule::BluetoothON);
    JSI::SetModuleAPI(exports, "off",            BtModule::BluetoothOFF);
    JSI::SetModuleAPI(exports, "hungup",        BtModule::BluetoothHungUp);
    JSI::SetModuleAPI(exports, "answer",        BtModule::BluetoothAnswer);
    JSI::SetModuleAPI(exports, "bt_scan",       BtModule::BluetoothScan);
    JSI::SetModuleAPI(exports, "bt_scan_cancel",BtModule::BluetoothScanCancel);
    JSI::SetModuleAPI(exports, "bt_connect",    BtModule::BluetoothConnect);
#ifdef BLE_WALKIE_TALKIE
    JSI::SetModuleAPI(exports, "walkie_talkie_init",    BtModule::BluetoothBleWTInit);
    JSI::SetModuleAPI(exports, "walkie_talkie_deinit",    BtModule::BluetoothBleWTDeinit);
    JSI::SetModuleAPI(exports, "start_listen",    BtModule::BluetoothBleWTStartScan);
    JSI::SetModuleAPI(exports, "stop_listen",    BtModule::BluetoothBleWTStopScan);
    JSI::SetModuleAPI(exports, "start_talk",    BtModule::BluetoothBleWTStartCapture);
    JSI::SetModuleAPI(exports, "stop_talk",    BtModule::BluetoothBleWTStopCapture);
#endif
}
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_BT_MODULE_H
