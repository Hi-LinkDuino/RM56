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

import time
from threading import Condition

from xdevice_extension._core.environment.device_state import TestDeviceState
from xdevice_extension._core.utils import convert_serial

CHECK_POLL_TIME = 3
MAX_CHECK_POLL_TIME = 30


class DeviceStateListener(object):
    def __init__(self, expected_state):
        self.expected_state = expected_state
        self.condition = Condition()

    def state_changed(self, new_state):
        if self.expected_state == new_state:
            with self.condition:
                self.condition.notify_all()

    def get_expected_state(self):
        return self.expected_state


class DeviceStateMonitor(object):
    """
    Provides facilities for monitoring the state of a Device.
    """

    def __init__(self, device):
        self.state_listener = []
        self.device_state = device.test_device_state
        self.device = device
        self.default_online_timeout = 1 * 60 * 1000
        self.default_available_timeout = 6 * 60 * 1000

    def wait_for_device_state(self, state, wait_time):
        listener = DeviceStateListener(state)
        if self.device_state == state:
            return True
        self.device.log.debug(
            "wait device %s for %s" % (convert_serial(self.device.device_sn),
                                       state))

        self.add_device_state_listener(listener)
        with listener.condition:
            try:
                listener.condition.wait(wait_time / 1000)
            finally:
                self.remove_device_state_listener(listener)

        return self.device_state == state

    def wait_for_device_not_available(self, wait_time):
        return self.wait_for_device_state(TestDeviceState.NOT_AVAILABLE,
                                          wait_time)

    def wait_for_device_online(self, wait_time=None):
        if not wait_time:
            wait_time = self.default_online_timeout
        return self.wait_for_device_state(TestDeviceState.ONLINE, wait_time)

    def wait_for_boot_complete(self, wait_time):
        counter = 1
        start_time = int(time.time()*1000)
        self.device.log.debug("wait for boot complete, and wait time: %s ms" %
                              wait_time)
        while int(time.time()*1000) - start_time < wait_time:
            try:
                result = self.device.get_recover_result(retry=0)
                if result == "ohos":
                    return True
            except Exception as exception:
                self.device.log.error("wait for boot complete exception: %s"
                                      % exception)
            time.sleep(min(CHECK_POLL_TIME * counter, MAX_CHECK_POLL_TIME))
            counter = counter + 1
        return False

    def wait_for_device_available(self, wait_time=None):
        if not wait_time:
            wait_time = self.default_available_timeout
        start_time = int(time.time()*1000)
        if not self.wait_for_device_online(wait_time):
            return False
        elapsed_time = int(time.time()*1000) - start_time
        if not self.wait_for_boot_complete(wait_time - elapsed_time):
            return False
        return True

    def remove_device_state_listener(self, listener):
        self.state_listener.remove(listener)

    def add_device_state_listener(self, listener):
        self.state_listener.append(listener)

    def set_state(self, new_state):
        if not new_state:
            return
        self.device_state = new_state
        for listener in self.state_listener:
            listener.state_changed(new_state)
