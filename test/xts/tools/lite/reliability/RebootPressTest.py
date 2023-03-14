#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#
from utils import *

DeviceKey = "1501003847544634520655ef9320c795"
hdcPath = "D:\\tools\\hdc\\hdc_std.exe"


def rebootTest():
    print("------------------------TEST START---------------------------")
    rebootLoop = 100
    rebootedTime = 1
    rebootWaitTime = 60
    rebootMaxWaitTime = 300
    start = datetime.datetime.now()
    now = datetime.datetime.now()
    abilitySuccess = "start ability successfully"
    while rebootedTime < rebootLoop:
        cmdRet = exec_cmd(hdcPath + " list targets", waitOut=True)
        print("cmdRet is " + str(cmdRet))
        if str(cmdRet).find(DeviceKey) != -1:
            cmdRet = exec_cmd(hdcPath + " -t " + DeviceKey + " shell " +
                              "aa start -d 123456 -a com.ohos.photos.MainAbility -b com.ohos.photos", waitOut=True)
            print("cmdRet is " + str(cmdRet))
            if str(cmdRet).find(abilitySuccess) != -1:
                print("Current Time is:" + str(datetime.datetime.now()))
                print("Will reboot round " + str(rebootedTime) + ", total time is " + str(rebootLoop))
                cmdRet = exec_cmd(hdcPath + " -t " + DeviceKey + " shell reboot")
                rebootedTime = rebootedTime + 1
                start = datetime.datetime.now()
                time.sleep(rebootWaitTime)
            else:
                print("The device " + DeviceKey + "can not start APP after reboot")
                print("The test will stop.")
                raise "The device " + DeviceKey + "can not start APP after reboot"
        else:
            now = datetime.datetime.now()
            if (now - start).seconds > int(rebootMaxWaitTime):
                print("The device " + DeviceKey + "is not ready after reboot for 300 seconds.")
                print("The test will stop.")
                raise "The device " + DeviceKey + "is not ready after reboot for 300 seconds."


if __name__ == "__main__":
    rebootTest()
    # main()
