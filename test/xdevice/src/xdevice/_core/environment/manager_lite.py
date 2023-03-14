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

import time
import threading

from _core.config.config_manager import UserConfigManager
from _core.constants import DeviceOsType
from _core.constants import ManagerType
from _core.environment.manager_env import DeviceAllocationState
from _core.exception import LiteDeviceError
from _core.plugin import Plugin
from _core.plugin import get_plugin
from _core.interface import IDeviceManager
from _core.logger import platform_logger
from _core.utils import convert_ip
from _core.utils import convert_port

__all__ = ["ManagerLite"]

LOG = platform_logger("ManagerLite")


@Plugin(type=Plugin.MANAGER, id=ManagerType.lite_device)
class ManagerLite(IDeviceManager):
    """
    Class representing device manager
    managing the set of available devices for testing
    """

    def __init__(self):
        self.devices_list = []
        self.list_con = threading.Condition()
        self.support_labels = ["ipcamera", "wifiiot", "watchGT"]

    def init_environment(self, environment="", user_config_file=""):
        device_lite = get_plugin(plugin_type=Plugin.DEVICE,
                                 plugin_id=DeviceOsType.lite)[0]

        devices = UserConfigManager(
            config_file=user_config_file, env=environment).get_com_device(
            "environment/device")

        for device in devices:
            try:
                device_lite_instance = device_lite.__class__()
                device_lite_instance.__init_device__(device)
                device_lite_instance.device_allocation_state = \
                    DeviceAllocationState.available
            except LiteDeviceError as exception:
                LOG.warning(exception)
                continue

            self.devices_list.append(device_lite_instance)

    def env_stop(self):
        pass

    def apply_device(self, device_option, timeout=10):
        """
        Request a device for testing that meets certain criteria.
        """
        del timeout
        LOG.debug("lite apply_device: apply lock")
        self.list_con.acquire()
        try:
            allocated_device = None
            for device in self.devices_list:
                if device_option.matches(device):
                    device.device_allocation_state = \
                        DeviceAllocationState.allocated
                    LOG.debug("allocate device sn: %s, type: %s" % (
                        device.__get_serial__(), device.__class__))
                    return device
            time.sleep(10)
            return allocated_device
        finally:
            LOG.debug("lite apply_device: release lock")
            self.list_con.release()

    def release_device(self, device):
        LOG.debug("lite release_device: apply lock")
        self.list_con.acquire()
        try:
            if device.device_allocation_state == \
                    DeviceAllocationState.allocated:
                device.device_allocation_state = \
                    DeviceAllocationState.available
            LOG.debug("free device sn: %s, type: %s" % (
                device.__get_serial__(), device.__class__))
        finally:
            LOG.debug("lite release_device: release lock")
            self.list_con.release()

    def list_devices(self):
        print("lite devices:")
        print("{0:<20}{1:<16}{2:<16}{3:<16}{4:<16}{5:<16}{6:<16}".
              format("SerialPort/IP", "Baudrate/Port", "OsType", "Allocation",
                     "Product", "ConnectType", "ComType"))
        for device in self.devices_list:
            if device.device_connect_type == "remote" or \
                    device.device_connect_type == "agent":
                print("{0:<20}{1:<16}{2:<16}{3:<16}{4:<16}{5:<16}".format(
                    convert_ip(device.device.host),
                    convert_port(device.device.port),
                    device.device_os_type,
                    device.device_allocation_state,
                    device.label,
                    device.device_connect_type))
            else:
                for com_controller in device.device.com_dict:
                    print("{0:<20}{1:<16}{2:<16}{3:<16}{4:<16}{5:<16}{6:<16}".
                          format(convert_port(device.device.com_dict[
                                     com_controller].serial_port),
                                 device.device.com_dict[
                                     com_controller].baud_rate,
                                 device.device_os_type,
                                 device.device_allocation_state,
                                 device.label,
                                 device.device_connect_type,
                                 com_controller))
