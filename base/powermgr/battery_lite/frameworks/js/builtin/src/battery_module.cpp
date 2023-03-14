/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "battery_module.h"
#include "battery_impl.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace ACELite {
namespace {
void SuccessCallBack(const JSIValue thisVal, const JSIValue args, JSIValue jsiValue)
{
    if (JSI::ValueIsUndefined(args)) {
        return;
    }
    JSIValue success = JSI::GetNamedProperty(args, CB_SUCCESS);
    JSIValue complete = JSI::GetNamedProperty(args, CB_COMPLETE);
    if (!JSI::ValueIsUndefined(success)) {
        if (JSI::ValueIsUndefined(jsiValue)) {
            JSI::CallFunction(success, thisVal, nullptr, 0);
        } else {
            JSI::CallFunction(success, thisVal, &jsiValue, ARGC_ONE);
        }
    }
    if (!JSI::ValueIsUndefined(complete)) {
        JSI::CallFunction(complete, thisVal, nullptr, 0);
    }
    JSI::ReleaseValueList(success, complete, ARGS_END);
}
}

JSIValue BatteryModule::GetBatterySOC(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    int32_t batterySoc = 0;
    if ((args == nullptr) || (argsNum == 0) || JSI::ValueIsUndefined(args[0])) {
        return undefValue;
    }

    batterySoc = GetBatSocImpl();
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "batterySoc", batterySoc);
    SuccessCallBack(thisVal, args[0], result);
    JSI::ReleaseValue(result);
    return undefValue;
}

JSIValue BatteryModule::GetChargingStatus(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    int32_t chargingStatus = 0;
    if ((args == nullptr) || (argsNum == 0) || JSI::ValueIsUndefined(args[0])) {
        return undefValue;
    }

    chargingStatus = GetChargingStatusImpl();
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "chargingStatus", chargingStatus);
    SuccessCallBack(thisVal, args[0], result);
    JSI::ReleaseValue(result);
    return undefValue;
}

JSIValue BatteryModule::GetHealthStatus(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    int32_t healthStatus = 0;
    if ((args == nullptr) || (argsNum == 0) || JSI::ValueIsUndefined(args[0])) {
        return undefValue;
    }

    healthStatus = GetHealthStatusImpl();
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "healthStatus", healthStatus);
    SuccessCallBack(thisVal, args[0], result);
    JSI::ReleaseValue(result);
    return undefValue;
}

JSIValue BatteryModule::GetPluggedType(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    int32_t pluggedType = 0;
    if ((args == nullptr) || (argsNum == 0) || JSI::ValueIsUndefined(args[0])) {
        return undefValue;
    }

    pluggedType = GetPluggedTypeImpl();
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "pluggedType", pluggedType);
    SuccessCallBack(thisVal, args[0], result);
    JSI::ReleaseValue(result);
    return undefValue;
}


JSIValue BatteryModule::GetVoltage(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    int32_t voltage = 0;
    if ((args == nullptr) || (argsNum == 0) || JSI::ValueIsUndefined(args[0])) {
        return undefValue;
    }

    voltage = GetBatVoltageImpl();
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "voltage", voltage);
    SuccessCallBack(thisVal, args[0], result);
    JSI::ReleaseValue(result);
    return undefValue;
}

JSIValue BatteryModule::GetTechnology(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    char *technology = NULL;
    if ((args == nullptr) || (argsNum == 0) || JSI::ValueIsUndefined(args[0])) {
        return undefValue;
    }

    technology = GetBatTechnologyImpl();
    JSIValue result = JSI::CreateObject();
    JSI::SetStringProperty(result, "technology", technology);
    SuccessCallBack(thisVal, args[0], result);
    JSI::ReleaseValue(result);
    return undefValue;
}

JSIValue BatteryModule::GetTemperature(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum)
{
    JSIValue undefValue = JSI::CreateUndefined();
    int32_t temperature = 0;
    if ((args == nullptr) || (argsNum == 0) || JSI::ValueIsUndefined(args[0])) {
        return undefValue;
    }

    temperature = GetBatTemperature();
    JSIValue result = JSI::CreateObject();
    JSI::SetNumberProperty(result, "temperature", temperature);
    SuccessCallBack(thisVal, args[0], result);
    JSI::ReleaseValue(result);
    return undefValue;
}

void InitBatteryModule(JSIValue exports)
{
    POWER_HILOGE("InitBatteryModule start");
    JSI::SetModuleAPI(exports, "BatterySOC", BatteryModule::GetBatterySOC);
    JSI::SetModuleAPI(exports, "ChargingStatus", BatteryModule::GetChargingStatus);
    JSI::SetModuleAPI(exports, "HealthStatus", BatteryModule::GetHealthStatus);
    JSI::SetModuleAPI(exports, "PluggedType", BatteryModule::GetPluggedType);
    JSI::SetModuleAPI(exports, "Voltage", BatteryModule::GetVoltage);
    JSI::SetModuleAPI(exports, "Technology", BatteryModule::GetTechnology);
    JSI::SetModuleAPI(exports, "Temperature", BatteryModule::GetTemperature);
    POWER_HILOGE("InitBatteryModule end");
}
} // ACELite
} // OHOS

