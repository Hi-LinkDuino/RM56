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

from dataclasses import dataclass
from enum import Enum
from enum import unique


@unique
class TestDeviceState(Enum):
    FASTBOOT = "FASTBOOT"
    ONLINE = "ONLINE"
    RECOVERY = "RECOVERY"
    NOT_AVAILABLE = "NOT_AVAILABLE"

    @staticmethod
    def get_test_device_state(device_state):
        if device_state is None:
            return TestDeviceState.NOT_AVAILABLE
        elif device_state == DeviceState.ONLINE:
            return TestDeviceState.ONLINE
        elif device_state == DeviceState.OFFLINE:
            return TestDeviceState.NOT_AVAILABLE
        elif device_state == DeviceState.RECOVERY:
            return TestDeviceState.RECOVERY
        elif device_state == DeviceState.BOOTLOADER:
            return TestDeviceState.FASTBOOT
        else:
            return TestDeviceState.NOT_AVAILABLE


@unique
class DeviceState(Enum):
    BOOTLOADER = "bootloader"
    OFFLINE = "offline"
    ONLINE = "device"
    RECOVERY = "recovery"

    @staticmethod
    def get_state(state):
        for device_state in DeviceState:
            if device_state.value == state:
                return device_state
        return None


@unique
class DeviceEvent(Enum):
    """
    Represents a test device event that can change allocation state
    """
    CONNECTED_ONLINE = "CONNECTED_ONLINE"
    CONNECTED_OFFLINE = "CONNECTED_OFFLINE"
    STATE_CHANGE_ONLINE = "STATE_CHANGE_ONLINE"
    STATE_CHANGE_OFFLINE = "STATE_CHANGE_OFFLINE"
    DISCONNECTED = "DISCONNECTED"
    FORCE_AVAILABLE = "FORCE_AVAILABLE"
    AVAILABLE_CHECK_PASSED = "AVAILABLE_CHECK_PASSED"
    AVAILABLE_CHECK_FAILED = "AVAILABLE_CHECK_FAILED"
    AVAILABLE_CHECK_IGNORED = "AVAILABLE_CHECK_IGNORED"
    ALLOCATE_REQUEST = "ALLOCATE_REQUEST"
    FORCE_ALLOCATE_REQUEST = "FORCE_ALLOCATE_REQUEST"
    FREE_AVAILABLE = "FREE_AVAILABLE"
    FREE_UNRESPONSIVE = "FREE_UNRESPONSIVE"
    FREE_UNAVAILABLE = "FREE_UNAVAILABLE"
    FREE_UNKNOWN = "FREE_UNKNOWN"


def handle_allocation_event(old_state, event):
    new_state = None
    if event == DeviceEvent.CONNECTED_ONLINE \
            or event == DeviceEvent.STATE_CHANGE_ONLINE:
        if old_state == DeviceAllocationState.allocated:
            new_state = old_state
        else:
            new_state = DeviceAllocationState.checking_availability
    elif event == DeviceEvent.CONNECTED_OFFLINE \
            or event == DeviceEvent.STATE_CHANGE_OFFLINE \
            or event == DeviceEvent.DISCONNECTED:
        if old_state == DeviceAllocationState.allocated:
            new_state = old_state
        else:
            new_state = DeviceAllocationState.unknown
    elif event == DeviceEvent.ALLOCATE_REQUEST:
        new_state = DeviceAllocationState.allocated
    elif event == DeviceEvent.FREE_AVAILABLE:
        new_state = DeviceAllocationState.available
    elif event == DeviceEvent.FREE_UNAVAILABLE:
        new_state = DeviceAllocationState.unknown
    elif event == DeviceEvent.AVAILABLE_CHECK_IGNORED:
        new_state = DeviceAllocationState.ignored
    elif event == DeviceEvent.AVAILABLE_CHECK_PASSED:
        new_state = DeviceAllocationState.available

    return new_state


@dataclass
class DeviceAllocationState:
    ignored = "Ignored"
    available = "Available"
    allocated = "Allocated"
    checking_availability = "Checking_Availability"
    unavailable = "Unavailable"
    unknown = "unknown"
