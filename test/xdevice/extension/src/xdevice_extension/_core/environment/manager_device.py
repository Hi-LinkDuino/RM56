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

import threading

from xdevice import UserConfigManager
from xdevice import ManagerType
from xdevice import Plugin
from xdevice import get_plugin
from xdevice import IDeviceManager
from xdevice import platform_logger
from xdevice import ParamError
from xdevice import ConfigConst

from xdevice_extension._core.environment.device_monitor import \
    DeviceStateMonitor
from xdevice_extension._core.environment.device_state import DeviceEvent
from xdevice_extension._core.environment.device_state import TestDeviceState
from xdevice_extension._core.environment.device_state import DeviceState
from xdevice_extension._core.environment.device_state import \
    handle_allocation_event
from xdevice_extension._core.environment.device_state import \
    DeviceAllocationState
from xdevice_extension._core.environment.dmlib import DeviceConnector
from xdevice_extension._core.utils import convert_serial
from xdevice_extension._core.utils import convert_ip
from xdevice_extension._core.utils import convert_port
from xdevice_extension._core.constants import DeviceConnectorType
from xdevice_extension._core.exception import HdcCommandRejectedException

__all__ = ["ManagerDevice"]

LOG = platform_logger("ManagerDevice")


@Plugin(type=Plugin.MANAGER, id=ManagerType.device)
class ManagerDevice(IDeviceManager):
    """
    Class representing device manager
    managing the set of available devices for testing
    """

    def __init__(self):
        self.devices_list = []
        self.global_device_filter = None
        self.lock_con = threading.Condition()
        self.list_con = threading.Condition()
        self.device_connector = None
        self.managed_device_listener = None
        self.support_labels = ["phone", "watch", "car", "tv", "tablet", "ivi"]
        self.support_types = ["device"]

    def init_environment(self, environment="", user_config_file=""):
        self._start_device_monitor(environment, user_config_file)

    def env_stop(self):
        self._stop_device_monitor()

    def _start_device_monitor(self, environment="", user_config_file=""):
        self.managed_device_listener = ManagedDeviceListener(self)
        device = UserConfigManager(
            config_file=user_config_file, env=environment).get_device(
            "environment/device")
        if device:
            try:
                self.device_connector = DeviceConnector(device.get("ip"),
                                                        device.get("port"),
                                                        device.get("usb_type"))
                self.global_device_filter = UserConfigManager(
                    config_file=user_config_file, env=environment).get_sn_list(
                    device.get("sn"))
                self.device_connector.add_device_change_listener(
                    self.managed_device_listener)
                self.device_connector.start()
            except (ParamError, FileNotFoundError) as error:
                self.env_stop()
                LOG.debug("start %s error: %s" % (
                    device.get("usb_type"), error))
                if device.get("usb_type") == DeviceConnectorType.hdc:
                    self.device_connector = DeviceConnector(
                        device.get("ip"), device.get("port"),
                        "usb-hdc")
                    self.device_connector.add_device_change_listener(
                        self.managed_device_listener)
                    self.device_connector.start()
        else:
            raise ParamError("Manager device is not supported, please "
                             "check config user_config.xml", error_no="00108")

    def _stop_device_monitor(self):
        self.device_connector.remove_device_change_listener(
            self.managed_device_listener)
        self.device_connector.terminate()

    def find(self, idevice):
        LOG.debug("find: apply list con lock")
        self.list_con.acquire()
        try:
            for device in self.devices_list:
                if device.device_sn == idevice.device_sn and \
                        device.device_os_type == idevice.device_os_type:
                    return device
        finally:
            LOG.debug("find: release list con lock")
            self.list_con.release()

    def apply_device(self, device_option, timeout=10):

        LOG.debug("apply_device: apply lock con lock")
        self.lock_con.acquire()
        try:
            device = self.allocate_device_option(device_option)
            if device:
                return device
            LOG.debug("wait for available device founded")
            self.lock_con.wait(timeout)
            return self.allocate_device_option(device_option)
        finally:
            LOG.debug("apply_device: release lock con lock")
            self.lock_con.release()

    def allocate_device_option(self, device_option):
        """
        Request a device for testing that meets certain criteria.
        """

        LOG.debug("allocate_device_option: apply list con lock")
        if not self.list_con.acquire(timeout=5):
            LOG.debug("allocate_device_option: list con wait timeout")
            return None
        try:
            allocated_device = None

            for device in self.devices_list:
                if device_option.matches(device):
                    self.handle_device_event(device,
                                             DeviceEvent.ALLOCATE_REQUEST)
                    LOG.debug("allocate device sn: %s, type: %s" % (
                        device.__get_serial__(), device.__class__))
                    return device
            return allocated_device

        finally:
            LOG.debug("allocate_device_option: release list con lock")
            self.list_con.release()

    def release_device(self, device):
        LOG.debug("release_device: apply list con lock")
        self.list_con.acquire()
        try:
            if device.test_device_state == TestDeviceState.ONLINE:
                self.handle_device_event(device, DeviceEvent.FREE_AVAILABLE)
            else:
                self.handle_device_event(device, DeviceEvent.FREE_UNAVAILABLE)

            device.device_id = None

            LOG.debug("free device sn: %s, type: %s" % (
                device.__get_serial__(), device.__class__.__name__))

        finally:
            LOG.debug("release_device: release list con lock")
            self.list_con.release()

    def find_device(self, device_sn, device_os_type):
        for device in self.devices_list:
            if device.device_sn == device_sn and \
                    device.device_os_type == device_os_type:
                return device

    def append_device_by_sort(self, device_instance):
        if (not self.global_device_filter or
                not self.devices_list or
                device_instance.device_sn not in self.global_device_filter):
            self.devices_list.append(device_instance)
        else:
            device_dict = dict(zip(
                self.global_device_filter,
                list(range(1, len(self.global_device_filter)+1))))
            for index in range(len(self.devices_list)):
                if self.devices_list[index].device_sn not in \
                        self.global_device_filter:
                    self.devices_list.insert(index, device_instance)
                    break
                if device_dict[device_instance.device_sn] < \
                        device_dict[self.devices_list[index].device_sn]:
                    self.devices_list.insert(index, device_instance)
                    break
            else:
                self.devices_list.append(device_instance)

    def find_or_create(self, idevice):
        LOG.debug("find_or_create: apply list con lock")
        self.list_con.acquire()
        try:
            device = self.find_device(idevice.device_sn,
                                      idevice.device_os_type)
            if device is None:
                device = get_plugin(
                    plugin_type=Plugin.DEVICE,
                    plugin_id=idevice.device_os_type)[0]
                device_instance = device.__class__()
                device_instance.__set_serial__(idevice.device_sn)
                device_instance.host = idevice.host
                device_instance.port = idevice.port
                device_instance.usb_type = self.device_connector.usb_type
                LOG.debug("create device(%s) host is %s, "
                          "port is %s, device sn is %s, usb type is %s" %
                          (device_instance, device_instance.host,
                           device_instance.port, device_instance.device_sn,
                           device_instance.usb_type))
                device_instance.device_state = DeviceState.get_state(
                    idevice.device_state)
                device_instance.test_device_state = \
                    TestDeviceState.get_test_device_state(
                        device_instance.device_state)
                device_instance.device_state_monitor = \
                    DeviceStateMonitor(device_instance)
                if idevice.device_state == DeviceState.ONLINE:
                    device_instance.get_device_type()
                self.append_device_by_sort(device_instance)
                device = device_instance
            else:
                LOG.debug("find device(%s), host is %s, "
                          "port is %s, device sn is %s, usb type is %s" %
                          (device, device.host, device.port, device.device_sn,
                           device.usb_type))
            return device
        except HdcCommandRejectedException as hcr_error:
            LOG.debug("%s occurs error. Reason:%s" %
                      (idevice.device_sn, hcr_error))
        finally:
            LOG.debug("find_or_create: release list con lock")
            self.list_con.release()

    def remove(self, idevice):
        LOG.debug("remove: apply list con lock")
        self.list_con.acquire()
        try:
            self.devices_list.remove(idevice)
        finally:
            LOG.debug("remove: release list con lock")
            self.list_con.release()

    def handle_device_event(self, device, event):
        state_changed = None
        old_state = device.device_allocation_state
        new_state = handle_allocation_event(old_state, event)

        if new_state == DeviceAllocationState.checking_availability:
            if self.global_device_filter and \
                    device.device_sn not in self.global_device_filter:
                event = DeviceEvent.AVAILABLE_CHECK_IGNORED
            else:
                event = DeviceEvent.AVAILABLE_CHECK_PASSED
            new_state = handle_allocation_event(new_state, event)

        if old_state != new_state:
            state_changed = True
            device.device_allocation_state = new_state

        if state_changed is True and \
                new_state == DeviceAllocationState.available:
            # notify_device_state_change
            LOG.debug("handle_device_event apply lock_con")
            self.lock_con.acquire()
            LOG.debug("find available device")
            self.lock_con.notify_all()
            LOG.debug("handle_device_event release lock_con")
            self.lock_con.release()

        if device.device_allocation_state == \
                DeviceAllocationState.unknown:
            self.remove(device)
        return

    def launch_emulator(self):
        pass

    def kill_emulator(self):
        pass

    def list_devices(self):
        print("devices:")
        print("{0:<20}{1:<16}{2:<16}{3:<16}{4:<16}{5:<16}{6:<16}".format(
            "Serial", "OsType", "State", "Allocation", "Product", "host",
            "port"))
        for device in self.devices_list:
            print("{0:<20}{1:<16}{2:<16}{3:<16}{4:<16}{5:<16}{6:<16}".format(
                convert_serial(device.device_sn), device.device_os_type,
                device.test_device_state.value,
                device.device_allocation_state,
                device.label if device.label else 'None',
                convert_ip(device.host), convert_port(device.port)))


class ManagedDeviceListener(object):
    """
    A class to listen for and act on device presence updates from ddmlib
    """

    def __init__(self, manager):
        self.manager = manager

    def device_changed(self, idevice):
        test_device = self.manager.find_or_create(idevice)
        if test_device is None:
            return
        new_state = TestDeviceState.get_test_device_state(idevice.device_state)
        test_device.test_device_state = new_state
        if new_state == TestDeviceState.ONLINE:
            self.manager.handle_device_event(test_device,
                                             DeviceEvent.STATE_CHANGE_ONLINE)
        elif new_state == TestDeviceState.NOT_AVAILABLE:
            self.manager.handle_device_event(test_device,
                                             DeviceEvent.STATE_CHANGE_OFFLINE)
        test_device.device_state_monitor.set_state(
            test_device.test_device_state)
        LOG.debug("device changed to %s: %s %s %s %s" % (
            new_state, convert_serial(idevice.device_sn),
            idevice.device_os_type, idevice.host, idevice.port))

    def device_connected(self, idevice):
        test_device = self.manager.find_or_create(idevice)
        if test_device is None:
            return
        new_state = TestDeviceState.get_test_device_state(idevice.device_state)
        test_device.test_device_state = new_state
        if test_device.test_device_state == TestDeviceState.ONLINE:
            self.manager.handle_device_event(test_device,
                                             DeviceEvent.CONNECTED_ONLINE)
        elif new_state == TestDeviceState.NOT_AVAILABLE:
            self.manager.handle_device_event(test_device,
                                             DeviceEvent.CONNECTED_OFFLINE)
        test_device.device_state_monitor.set_state(
            test_device.test_device_state)
        LOG.debug("device connected: %s %s %s %s" % (
            convert_serial(idevice.device_sn), idevice.device_os_type,
            idevice.host, idevice.port))
        LOG.debug("set device %s %s to true" % (
            idevice.device_sn, ConfigConst.recover_state))
        test_device.set_recover_state(True)

    def device_disconnected(self, disconnected_device):
        test_device = self.manager.find(disconnected_device)
        if test_device is not None:
            test_device.test_device_state = TestDeviceState.NOT_AVAILABLE
            self.manager.handle_device_event(test_device,
                                             DeviceEvent.DISCONNECTED)
            test_device.device_state_monitor.set_state(
                TestDeviceState.NOT_AVAILABLE)
        LOG.debug("device disconnected: %s %s %s %s" % (
            convert_serial(disconnected_device.device_sn),
            disconnected_device.device_os_type,
            disconnected_device.host, disconnected_device.port))
