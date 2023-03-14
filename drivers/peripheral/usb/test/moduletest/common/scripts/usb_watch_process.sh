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

pid=$(pidof $1)
interval=1
log_file="./usb_proclog.txt"
cat /dev/null > $log_file
while true
do
cat /proc/${pid}/status | grep -e VmRSS >> $log_file
cpu=$(top -n 1 -p ${pid} | head -6 | tail -1 | cut -F 10)
echo "Cpu:" $cpu >> $log_file
cat /proc/${pid}/status | grep -e Threads >> $log_file
sleep $interval
done