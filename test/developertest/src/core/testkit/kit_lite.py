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

import os
import re
import random
import string
import shutil
import subprocess

from xdevice import platform_logger
from xdevice import Plugin
from xdevice import CKit
from xdevice import ComType
from xdevice import ParamError
from xdevice import LiteDeviceError
from xdevice import ITestKit
from xdevice import get_config_value
from xdevice import get_file_absolute_path
from xdevice import LiteDeviceConnectError
from xdevice import DeviceAllocationState


__all__ = ["DeployKit"]
LOG = platform_logger("KitLite")

RESET_CMD = "0xEF, 0xBE, 0xAD, 0xDE, 0x0C, 0x00, 0x87, 0x78, 0x00, 0x00, " \
            "0x61, 0x94"


@Plugin(type=Plugin.TEST_KIT, id=CKit.deploy)
class DeployKit(ITestKit):
    def __init__(self):
        self.burn_file = ""
        self.burn_command = ""
        self.timeout = ""
        self.paths = ""

    def __check_config__(self, config):
        self.timeout = str(int(get_config_value(
            'timeout', config, is_list=False, default=0)) * 1000)
        self.burn_file = get_config_value('burn_file', config, is_list=False)
        burn_command = get_config_value('burn_command', config, is_list=False,
                                        default=RESET_CMD)
        self.burn_command = burn_command.replace(" ", "").split(",")
        self.paths = get_config_value('paths', config)
        if self.timeout == "0" or not self.burn_file:
            msg = "The config for deploy kit is invalid with timeout:{}, " \
                  "burn_file:{}".format(self.timeout, self.burn_file)
            raise ParamError(msg, error_no="00108")

    def _reset(self, device):
        cmd_com = device.device.com_dict.get(ComType.cmd_com)
        try:
            cmd_com.connect()
            cmd_com.execute_command(command='AT+RST={}'.format(self.timeout))
            cmd_com.close()
        except (LiteDeviceConnectError, IOError) as error:
            device.device_allocation_state = DeviceAllocationState.unusable
            LOG.error(
                "The exception {} happened in deploy kit running".format(
                    error), error_no=getattr(error, "error_no",
                                             "00000"))
            raise LiteDeviceError("%s port set_up wifiiot failed" %
                                  cmd_com.serial_port,
                                  error_no=getattr(error, "error_no",
                                                   "00000"))
        finally:
            if cmd_com:
                cmd_com.close()

    def _send_file(self, device, source_file):
        burn_tool_name = "HiBurn.exe" if os.name == "nt" else "HiBurn"
        burn_tool_path = get_file_absolute_path(
            os.path.join("tools", burn_tool_name), self.paths)

        deploy_serial_port = device.device.com_dict.get(
            ComType.deploy_com).serial_port
        deploy_baudrate = device.device.com_dict.\
            get(ComType.deploy_com).baud_rate
        port_number = re.findall(r'\d+$', deploy_serial_port)
        if not port_number:
            raise LiteDeviceError("The config of serial port {} to deploy is "
                                  "invalid".format(deploy_serial_port),
                                  error_no="00108")
        new_temp_tool_path = self.copy_file_as_temp(burn_tool_path, 10)
        cmd = '{} -com:{} -bin:{} -signalbaud:{}' \
            .format(new_temp_tool_path, port_number[0], source_file,
                    deploy_baudrate)
        LOG.info('The running cmd is {}'.format(cmd))
        LOG.info('The burn tool is running, please wait..')
        return_code, out = subprocess.getstatusoutput(cmd)
        LOG.info(
            'Deploy kit to execute burn tool finished with return_code: {} '
            'output: {}'.format(return_code, out))
        os.remove(new_temp_tool_path)
        if 0 != return_code:
            device.device_allocation_state = DeviceAllocationState.unusable
            raise LiteDeviceError("%s port set_up wifiiot failed" %
                                  deploy_serial_port, error_no="00402")

    def __setup__(self, device, **kwargs):
        """
        Execute reset command on the device by cmd serial port and then upload
        patch file by deploy tool.
        Parameters:
            device: the instance of LocalController with one or more
                    ComController
        """
        args = kwargs
        source_file = args.get("source_file", None)
        self._reset(device)
        self._send_file(device, source_file)

    def __teardown__(self, device):
        pass

    def copy_file_as_temp(self, original_file, str_length):
        """
        To obtain a random string with specified length
        Parameters:
            original_file : the original file path
            str_length: the length of random string
        """
        if os.path.isfile(original_file):
            random_str = random.sample(string.ascii_letters + string.digits,
                                       str_length)
            new_temp_tool_path = '{}_{}{}'.format(
                os.path.splitext(original_file)[0], "".join(random_str),
                os.path.splitext(original_file)[1])
            return shutil.copyfile(original_file, new_temp_tool_path)