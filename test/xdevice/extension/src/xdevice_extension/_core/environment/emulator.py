#!/usr/bin/env python3
# coding=utf-8

#
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
#

from xdevice import IDevice
from xdevice import platform_logger

from xdevice_extension._core.environment.device_state import \
    DeviceAllocationState
from xdevice_extension._core.environment.device_state import TestDeviceState

LOG = platform_logger("Emulator")


class Emulator(IDevice):
    """
    Class representing an emulator.

    Each object of this class represents one emulator in xDevice.

    Attributes:
        device_sn: A string that's the serial number of the emulator.
    """

    def __get_serial__(self):
        pass

    def __set_serial__(self, device_sn=""):
        pass

    def __init__(self, device_sn=""):
        self.device_sn = device_sn
        self.is_timeout = False
        self.device_log_proc = None
        self.test_device_state = TestDeviceState.ONLINE
        self.device_allocation_state = DeviceAllocationState.available

    def __serial__(self):
        return self.device_sn

    def get_device_sn(self):
        """
        Returns the serial number of the device.
        """
        return self.device_sn
