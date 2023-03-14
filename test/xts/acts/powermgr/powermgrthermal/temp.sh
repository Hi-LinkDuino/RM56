#!/bin/bash
# Copyright (c) 2022 Huawei Device Co., Ltd.
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

count=0
coldTemp=-22000
highTemp=40100
output=/data/sensor/battery/temp

while ((count < 100))
do
    if [ $(($count%2)) == 0 ]; then
        let "coldTemp = coldTemp + 1000"
        echo $coldTemp
        echo $coldTemp > $output
        sleep 5
    else
        let "highTemp = highTemp + 1000"
        echo $highTemp
        echo $highTemp > $output
        sleep 5
    fi
    let "count = count + 1"
    cat $output
done