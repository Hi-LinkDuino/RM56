#!/bin/bash
# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

device="/dev/ttyACM0"
stty -F "$device" raw speed 115200 min 0 time 5
port=$(lsusb -t| grep cdc_acm | tail -1 | awk -F'[ :]+' '{print $4}')
usb_dir="/sys/bus/usb/devices/1-${port}"

while true
do
    if [ ! -e "$device" ];then
        echo "$device not exists"
        sleep 1
        continue
    fi
    data=$(cat $device)
    if [ "$data" == "GET_DESCRIPTOR" ];then
        id_vendor=$(cat ${usb_dir}/idVendor)
        id_product=$(cat ${usb_dir}/idProduct)
        bcd_device=$(cat ${usb_dir}/bcdDevice)
        b_configuration_value=$(cat ${usb_dir}/bConfigurationValue)
        echo "$id_vendor" "$id_product" "$bcd_device" "$b_configuration_value"
        echo -n "$id_vendor" "$id_product" "$bcd_device" "$b_configuration_value"> $device
    fi
    if [ ! "${data}x" == "x" ];then
        echo "$data"
        echo -n "$data" > $device
    fi
done