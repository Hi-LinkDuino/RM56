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

var battery = require("@system.battery");
battery.BatterySOC({
    success: function(data) {
        console.log('success get battery Soc:' + data.batterySoc);
    },
    fail: function(data, code) {
        console.log('fail to get battery Soc code:' + code + ', data: ' + data);
    },
});

battery.ChargingStatus({
    success: function(data) {
        console.log('success get ChargingStatus:' + data.chargingStatus);
    },
    fail: function(data, code) {
        console.log('fail to get battery ChargingStatus code:' + code + ', data: ' + data);
    },
});

battery.HealthStatus({
    success: function(data) {
        console.log('success get battery HealthStatus:' + data.healthStatus);
    },
    fail: function(data, code) {
        console.log('fail to get battery HealthStatus code:' + code + ', data: ' + data);
    },
});

battery.PluggedType({
    success: function(data) {
        console.log('success get battery PluggedType:' + data.pluggedType);
    },
    fail: function(data, code) {
        console.log('fail to get battery PluggedType code:' + code + ', data: ' + data);
    },
});

battery.Voltage({
    success: function(data) {
        console.log('success get battery Voltage:' + data.voltage);
    },
    fail: function(data, code) {
        console.log('fail to get battery Voltage code:' + code + ', data: ' + data);
    },
});

battery.Technology({
    success: function(data) {
        console.log('success get battery Technology:' + data.technology);
    },
    fail: function(data, code) {
        console.log('fail to get battery Technology code:' + code + ', data: ' + data);
    },
});

battery.Temperature({
    success: function(data) {
        console.log('success get battery Temperature:' + data.temperature);
    },
    fail: function(data, code) {
        console.log('fail to get battery Temperature code:' + code + ', data: ' + data);
    },
});
