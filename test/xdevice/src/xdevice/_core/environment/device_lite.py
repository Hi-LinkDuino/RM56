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

import re
import telnetlib
import time
import os
import threading

from _core.constants import DeviceOsType
from _core.constants import ConfigConst
from _core.constants import ComType
from _core.constants import DeviceLabelType
from _core.constants import ModeType
from _core.environment.dmlib_lite import LiteHelper
from _core.exception import LiteDeviceConnectError
from _core.exception import LiteDeviceTimeout
from _core.exception import LiteParamError
from _core.interface import IDevice
from _core.logger import platform_logger
from _core.environment.manager_env import DeviceAllocationState
from _core.plugin import Plugin
from _core.utils import exec_cmd
from _core.utils import convert_serial
from _core.utils import convert_ip
from _core.utils import check_mode

LOG = platform_logger("DeviceLite")
TIMEOUT = 90
RETRY_ATTEMPTS = 0
HDC = "hdc_std.exe"
DEFAULT_BAUD_RATE = 115200


def get_hdc_path():
    from xdevice import Variables
    user_path = os.path.join(Variables.exec_dir, "resource/tools")
    top_user_path = os.path.join(Variables.top_dir, "config")
    config_path = os.path.join(Variables.res_dir, "config")
    paths = [user_path, top_user_path, config_path]

    file_path = ""
    for path in paths:
        if os.path.exists(os.path.abspath(os.path.join(
                path, HDC))):
            file_path = os.path.abspath(os.path.join(
                path, HDC))
            break

    if os.path.exists(file_path):
        return file_path
    else:
        raise LiteParamError("litehdc.exe not found", error_no="00108")


def parse_available_com(com_str):
    com_str = com_str.replace("\r", " ")
    com_list = com_str.split("\n")
    for index, item in enumerate(com_list):
        com_list[index] = item.strip().strip(b'\x00'.decode())
    return com_list


def perform_device_action(func):
    def device_action(self, *args, **kwargs):
        if not self.get_recover_state():
            LOG.debug("device %s %s is false" % (self.device_sn,
                                                 ConfigConst.recover_state))
            return "", "", ""

        tmp = int(kwargs.get("retry", RETRY_ATTEMPTS))
        retry = tmp + 1 if tmp > 0 else 1
        exception = None
        for num in range(retry):
            try:
                result = func(self, *args, **kwargs)
                return result
            except LiteDeviceTimeout as error:
                LOG.error(error)
                exception = error
                if num:
                    self.recover_device()
            except Exception as error:
                LOG.error(error)
                exception = error
        raise exception

    return device_action


@Plugin(type=Plugin.DEVICE, id=DeviceOsType.lite)
class DeviceLite(IDevice):
    """
    Class representing an device lite device.

    Each object of this class represents one device lite device in xDevice.

    Attributes:
        device_connect_type: A string that's the type of lite device
    """
    device_os_type = DeviceOsType.lite
    device_allocation_state = DeviceAllocationState.available

    def __init__(self):
        self.device_sn = ""
        self.label = ""
        self.device_connect_type = ""
        self.device_kernel = ""
        self.device = None
        self.ifconfig = None
        self.extend_value = {}
        self.device_lock = threading.RLock()

    def __set_serial__(self, device=None):
        for item in device:
            if "ip" in item.keys() and "port" in item.keys():
                self.device_sn = "remote_%s_%s" % \
                                 (item.get("ip"), item.get("port"))
                break
            elif "type" in item.keys() and "com" in item.keys():
                self.device_sn = "local_%s" % item.get("com")
                break

    def __get_serial__(self):
        return self.device_sn

    def get(self, key=None, default=None):
        if not key:
            return default
        value = getattr(self, key, None)
        if value:
            return value
        else:
            return self.extend_value.get(key, default)

    def __set_device_kernel__(self, kernel_type=""):
        self.device_kernel = kernel_type

    def __get_device_kernel__(self):
        return self.device_kernel

    @staticmethod
    def _check_watchgt(device):
        for item in device:
            if "label" not in item.keys():
                if "com" not in item.keys() or ("com" in item.keys() and
                                                not item.get("com")):
                    error_message = "watchGT local com cannot be " \
                                    "empty, please check"
                    raise LiteParamError(error_message, error_no="00108")
                else:
                    hdc = get_hdc_path()
                    result = exec_cmd([hdc])
                    com_list = parse_available_com(result)
                    if item.get("com").upper() in com_list:
                        return True
                    else:
                        error_message = "watchGT local com does not exist"
                        raise LiteParamError(error_message, error_no="00108")

    @staticmethod
    def _check_wifiiot_config(device):
        com_type_set = set()
        for item in device:
            if "label" not in item.keys():
                if "com" not in item.keys() or ("com" in item.keys() and
                                                not item.get("com")):
                    error_message = "wifiiot local com cannot be " \
                                    "empty, please check"
                    raise LiteParamError(error_message, error_no="00108")

                if "type" not in item.keys() or ("type" not in item.keys() and
                                                 not item.get("type")):
                    error_message = "wifiiot com type cannot be " \
                                    "empty, please check"
                    raise LiteParamError(error_message, error_no="00108")
                else:
                    com_type_set.add(item.get("type"))
        if len(com_type_set) < 2:
            error_message = "wifiiot need cmd com and deploy com" \
                            " at the same time, please check"
            raise LiteParamError(error_message, error_no="00108")

    @staticmethod
    def _check_ipcamera_local(device):
        for item in device:
            if "label" not in item.keys():
                if "com" not in item.keys() or ("com" in item.keys() and
                                                not item.get("com")):
                    error_message = "ipcamera local com cannot be " \
                                    "empty, please check"
                    raise LiteParamError(error_message, error_no="00108")

    @staticmethod
    def _check_ipcamera_remote(device=None):
        for item in device:
            if "label" not in item.keys():
                if "port" in item.keys() and item.get("port") and not item.get(
                        "port").isnumeric():
                    error_message = "ipcamera remote port should be " \
                                    "a number, please check"
                    raise LiteParamError(error_message, error_no="00108")
                elif "port" not in item.keys():
                    error_message = "ipcamera remote port cannot be" \
                                    " empty, please check"
                    raise LiteParamError(error_message, error_no="00108")

    @staticmethod
    def _check_agent(device=None):
        for item in device:
            if "label" not in item.keys():
                if "port" in item.keys() and item.get("port") and not item.get(
                        "port").isnumeric():
                    error_message = "agent port should be " \
                                    "a number, please check"
                    raise LiteParamError(error_message, error_no="00108")
                elif "port" not in item.keys():
                    error_message = "ipcamera agent port cannot be" \
                                    " empty, please check"
                    raise LiteParamError(error_message, error_no="00108")

    def __check_config__(self, device=None):
        if "agent" == device[0].get("type"):
            self.device_connect_type = "agent"
            self.label = device[0].get("label")
        else:
            self.set_connect_type(device)
        if self.label == DeviceLabelType.wifiiot:
            self._check_wifiiot_config(device)
        elif self.label == DeviceLabelType.ipcamera and \
                self.device_connect_type == "local":
            self._check_ipcamera_local(device)
        elif self.label == DeviceLabelType.ipcamera and \
                self.device_connect_type == "remote":
            self._check_ipcamera_remote(device)
        elif self.label == DeviceLabelType.watch_gt:
            self._check_watchgt(device)
        elif self.label == DeviceLabelType.ipcamera and \
                self.device_connect_type == "agent":
            self._check_agent(device)

    def set_connect_type(self, device):
        pattern = r'^((25[0-5]|2[0-4]\d|[01]?\d\d?)\.){3}(25[0-5]|2[0-4]\d|[' \
                  r'01]?\d\d?)$'
        for item in device:
            if "label" in item.keys():
                self.label = item.get("label")
            if "com" in item.keys():
                self.device_connect_type = "local"
            if "ip" in item.keys():
                if re.match(pattern, item.get("ip")):
                    self.device_connect_type = "remote"
                else:
                    error_message = "Remote device ip not in right" \
                                         "format, please check user_config.xml"
                    raise LiteParamError(error_message, error_no="00108")
        if not self.label:
            error_message = "device label cannot be empty, " \
                            "please check"
            raise LiteParamError(error_message, error_no="00108")
        else:
            if self.label != DeviceLabelType.wifiiot and \
                    self.label != DeviceLabelType.ipcamera and \
                    self.label != DeviceLabelType.watch_gt:
                error_message = "device label should be ipcamera or" \
                                     " wifiiot, please check"
                raise LiteParamError(error_message, error_no="00108")
        if not self.device_connect_type:
            error_message = "device com or ip cannot be empty, " \
                                 "please check"
            raise LiteParamError(error_message, error_no="00108")

    def __init_device__(self, device):
        self.__check_config__(device)
        self.__set_serial__(device)
        if self.device_connect_type == "remote":
            self.device = CONTROLLER_DICT.get("remote")(device)
        elif self.device_connect_type == "agent":
            self.device = CONTROLLER_DICT.get("agent")(device)
        else:
            self.device = CONTROLLER_DICT.get("local")(device)

        self.ifconfig = device[1].get("ifconfig")

    def connect(self):
        """
        connect the device

        """
        try:
            self.device.connect()
        except LiteDeviceConnectError:
            if check_mode(ModeType.decc):
                LOG.debug("set device %s recover state to false" %
                          self.device_sn)
                self.device_allocation_state = DeviceAllocationState.unusable
                self.set_recover_state(False)
            raise

    @perform_device_action
    def execute_command_with_timeout(self, command="", case_type="",
                                     timeout=TIMEOUT, **kwargs):
        """Executes command on the device.

        Args:
            command: the command to execute
            case_type: CTest or CppTest
            timeout: timeout for read result
            **kwargs: receiver - parser handler input

        Returns:
            (filter_result, status, error_message)

            filter_result: command execution result
            status: true or false
            error_message: command execution error message
        """
        receiver = kwargs.get("receiver", None)
        if self.device_connect_type == "remote":
            LOG.info("%s execute command shell %s with timeout %ss" %
                     (convert_serial(self.__get_serial__()), command,
                      str(timeout)))
            filter_result, status, error_message = \
                self.device.execute_command_with_timeout(
                    command=command,
                    timeout=timeout,
                    receiver=receiver)
        elif self.device_connect_type == "agent":
            filter_result, status, error_message = \
                self.device.execute_command_with_timeout(
                    command=command,
                    case_type=case_type,
                    timeout=timeout,
                    receiver=receiver, type="cmd")
        else:
            filter_result, status, error_message = \
                self.device.execute_command_with_timeout(
                    command=command,
                    case_type=case_type,
                    timeout=timeout,
                    receiver=receiver)
        if not receiver:
            LOG.debug("%s execute result:%s" % (
                convert_serial(self.__get_serial__()), filter_result))
            if not status:
                LOG.debug("%s error_message:%s" % (
                    convert_serial(self.__get_serial__()), error_message))
        return filter_result, status, error_message

    def recover_device(self):
        self.reboot()

    def reboot(self):
        self.connect()
        filter_result, status, error_message = self. \
            execute_command_with_timeout(command="reset", timeout=30)
        if not filter_result:
            if check_mode(ModeType.decc):
                LOG.debug("Set device %s recover state to false" %
                          self.device_sn)
                self.device_allocation_state = DeviceAllocationState.unusable
                self.set_recover_state(False)

        if self.ifconfig:
            self.execute_command_with_timeout(command=self.ifconfig,
                                              timeout=60)

    def close(self):
        """
        Close the telnet connection with device server or close the local
        serial
        """
        self.device.close()

    def set_recover_state(self, state):
        with self.device_lock:
            setattr(self, ConfigConst.recover_state, state)

    def get_recover_state(self, default_state=True):
        with self.device_lock:
            state = getattr(self, ConfigConst.recover_state, default_state)
            return state


class RemoteController:
    """
    Class representing an device lite remote device.
    Each object of this class represents one device lite remote device
    in xDevice.
    """

    def __init__(self, device):
        self.host = device[1].get("ip")
        self.port = int(device[1].get("port"))
        self.telnet = None

    def connect(self):
        """
        connect the device server

        """
        try:
            if self.telnet:
                return self.telnet
            self.telnet = telnetlib.Telnet(self.host, self.port,
                                           timeout=TIMEOUT)
        except Exception as err_msgs:
            error_message = "Connect remote lite device failed, host is %s, " \
                            "port is %s, error is %s" % \
                            (convert_ip(self.host), self.port, str(err_msgs))
            raise LiteDeviceConnectError(error_message, error_no="00401")
        time.sleep(2)
        self.telnet.set_debuglevel(0)
        return self.telnet

    def execute_command_with_timeout(self, command="", timeout=TIMEOUT,
                                     receiver=None):
        """
        Executes command on the device.

        Parameters:
            command: the command to execute
            timeout: timeout for read result
            receiver: parser handler
        """
        return LiteHelper.execute_remote_cmd_with_timeout(
            self.telnet, command, timeout, receiver)

    def close(self):
        """
        Close the telnet connection with device server
        """
        try:
            if not self.telnet:
                return
            self.telnet.close()
            self.telnet = None
        except (ConnectionError, Exception):
            error_message = "Remote device is disconnected abnormally"
            LOG.error(error_message, error_no="00401")


class LocalController:
    def __init__(self, device):
        """
        Init Local device.
        Parameters:
            device: local device
        """
        self.com_dict = {}
        for item in device:
            if "com" in item.keys():
                if "type" in item.keys() and ComType.cmd_com == item.get(
                        "type"):
                    self.com_dict[ComType.cmd_com] = ComController(item)
                elif "type" in item.keys() and ComType.deploy_com == item.get(
                        "type"):
                    self.com_dict[ComType.deploy_com] = ComController(item)

    def connect(self, key=ComType.cmd_com):
        """
        Open serial.
        """
        self.com_dict.get(key).connect()

    def close(self, key=ComType.cmd_com):
        """
        Close serial.
        """
        if self.com_dict and self.com_dict.get(key):
            self.com_dict.get(key).close()

    def execute_command_with_timeout(self, **kwargs):
        """
        Execute command on the serial and read all the output from the serial.
        """
        args = kwargs
        key = args.get("key", ComType.cmd_com)
        command = args.get("command", None)
        case_type = args.get("case_type", "")
        receiver = args.get("receiver", None)
        timeout = args.get("timeout", TIMEOUT)
        return self.com_dict.get(key).execute_command_with_timeout(
            command=command, case_type=case_type,
            timeout=timeout, receiver=receiver)


class ComController:
    def __init__(self, device):
        """
        Init serial.
        Parameters:
            device: local com
        """
        self.is_open = False
        self.com = None
        self.serial_port = device.get("com") if device.get("com") else None
        self.baud_rate = int(device.get("baud_rate")) if device.get(
            "baud_rate") else DEFAULT_BAUD_RATE
        self.timeout = int(device.get("timeout")) if device.get(
            "timeout") else TIMEOUT

    def connect(self):
        """
        Open serial.
        """
        try:
            if not self.is_open:
                import serial
                self.com = serial.Serial(self.serial_port,
                                         baudrate=self.baud_rate,
                                         timeout=self.timeout)
                self.is_open = True
        except Exception as error_msg:
            error = "connect %s serial failed, please make sure this port is" \
                    " not occupied, error is %s[00401]" % \
                   (self.serial_port, str(error_msg))
            raise LiteDeviceConnectError(error, error_no="00401")

    def close(self):
        """
        Close serial.
        """
        try:
            if not self.com:
                return
            if self.is_open:
                self.com.close()
            self.is_open = False
        except (ConnectionError, Exception):
            error_message = "Local device is disconnected abnormally"
            LOG.error(error_message, error_no="00401")

    def execute_command_with_timeout(self, **kwargs):
        """
        Execute command on the serial and read all the output from the serial.
        """
        return LiteHelper.execute_local_cmd_with_timeout(self.com, **kwargs)

    def execute_command(self, command):
        """
        Execute command on the serial and read all the output from the serial.
        """
        LiteHelper.execute_local_command(self.com, command)


class AgentController:
    def __init__(self, device):
        """
        Init serial.
        Parameters:
            device: local com
        """
        LOG.info("AgentController begin")
        self.com_dict = {}
        self.host = device[1].get("ip")
        self.port = str(device[1].get("port"))
        self.headers = {'Content-Type': 'application/json'}
        self.local_source_dir = ""
        self.target_save_path = ""
        self.base_url = "http" + "://" + self.host + ":" + self.port

    def connect(self):
        """
        Open serial.
        """

    def close(self):
        """
         Close serial.
        """
        pass

    def execute_command_with_timeout(self, command, **kwargs):
        """
        Executes command on the device.

        Parameters:
            command: the command to execute
            timeout: timeout for read result
            receiver: parser handler
        """
        return LiteHelper.execute_agent_cmd_with_timeout(self,
                                                         command,
                                                         **kwargs)


CONTROLLER_DICT = {
    "local": LocalController,
    "remote": RemoteController,
    "agent": AgentController
}
