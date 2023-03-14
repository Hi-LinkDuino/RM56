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

import os
import sys
import re
import json
import time

# insert src path for loading xdevice modules
sys.framework_src_dir = os.path.abspath(os.path.dirname(
    os.path.dirname(__file__)))
sys.path.insert(1, sys.framework_src_dir)
sys.framework_root_dir = os.path.abspath(os.path.dirname(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.dirname(__file__))))))
sys.xdevice_dir = os.path.abspath(os.path.join(
    sys.framework_root_dir,
    "src"))
sys.path.insert(2, sys.xdevice_dir)
sys.xdevice_dir = os.path.abspath(os.path.join(
    sys.framework_root_dir,
    "..",
    "xdevice",
    "src"))
sys.path.insert(3, sys.xdevice_dir)
sys.adapter_dir = os.path.abspath(os.path.join(
    sys.framework_root_dir,
    "adapter",
    "aw",
    "python"))
sys.path.insert(4, sys.adapter_dir)

from distributed.common.common import create_empty_result_file
from distributed.common.common import get_resource_dir
from distributed.common.drivers import CppTestDriver
from distributed.common.drivers import DexTestDriver
from distributed.common.drivers import HapTestDriver

DEVICE_INFO_TEMPLATE = "agentlist:%s\nagentport:%s,\ndevicesuuid:%s"


##############################################################################
##############################################################################


def get_current_driver(device, target_name):
    driver = None
    _, suffix_name = os.path.splitext(target_name)
    if suffix_name == "":
        driver = CppTestDriver(device)
    elif suffix_name == ".bin":
        driver = CppTestDriver(device)
    elif suffix_name == ".dex":
        driver = DexTestDriver(device)
    elif suffix_name == ".hap":
        driver = HapTestDriver(device)
    return driver


##############################################################################
##############################################################################


class Distribute:
    def __init__(self, suite_dir, major, agent_list):
        self.suite_dir = suite_dir
        self.major = major
        self.agent_list = agent_list

    def exec_agent(self, device, target_name):
        driver = get_current_driver(device, target_name)
        if driver is None:
            print("Error: driver is None.")
            return False

        resource_dir = get_resource_dir(self.suite_dir, device.name)
        self._make_agent_desc_file(device)
        device.push_file(os.path.join(self.suite_dir, "agent.desc"),
                         device.test_path)
        device.push_file(os.path.join(resource_dir, target_name),
                         device.test_path)

        suite_path = os.path.join(self.suite_dir, target_name)
        driver.execute(suite_path, background=True)
        return self._check_thread(device, target_name)

    def exec_major(self, device, target_name):
        driver = get_current_driver(device, target_name)
        if driver is None:
            print("Error: driver is None.")
            return False

        resource_dir = get_resource_dir(self.suite_dir, device.name)
        self._make_major_desc_file()
        device.push_file(os.path.join(self.suite_dir, "major.desc"),
                         device.test_path)
        device.push_file(os.path.join(resource_dir, target_name),
                         device.test_path)

        suite_path = os.path.join(self.suite_dir, target_name)
        return driver.execute(suite_path, background=False)

    def pull_result(self, device, source_path, result_save_path):
        _, file_name = os.path.split(source_path)
        device.pull_file(source_path, result_save_path)
        if not os.path.exists(os.path.join(result_save_path, file_name)):
            create_empty_result_file(result_save_path, file_name)
        return

    def _check_thread(self, device, thread_name):
        check_command = "ps -A | grep %s" % thread_name
        checksum = 0
        while checksum < 100:  # check 100 times
            checksum += 1
            print("check thread:%s %s times" % (thread_name, checksum))
            output = device.shell_with_output(check_command)
            if output == "":
                time.sleep(0.1)
            else:
                print("thread info: %s" % output)
                break
        return True if checksum < 100 else False

    def _make_agent_desc_file(self, device):
        agent_ip_list = ""
        device_uuid_list = ""
        device_uuid_list += self._query_device_uuid(self.major) + ","
        agent_ip_list += self._query_device_ip(device) + ","
        for agent in self.agent_list:
            device_uuid_list += self._query_device_uuid(agent) + ","
        config_info = DEVICE_INFO_TEMPLATE % (agent_ip_list, "8888",
            device_uuid_list)

        config_agent_file = os.path.join(self.suite_dir, "agent.desc")
        self._write_device_config(config_info, config_agent_file)

    def _make_major_desc_file(self):
        agent_ip_list = ""
        device_uuid_list = ""

        device_uuid_list += self._query_device_uuid(self.major) + ","
        for agent in self.agent_list:
            agent_ip_list += self._query_device_ip(agent) + ","
            device_uuid_list += self._query_device_uuid(agent) + ","
        config_info = DEVICE_INFO_TEMPLATE % (agent_ip_list, "8888",
            device_uuid_list)

        config_major_file = os.path.join(self.suite_dir, "major.desc")
        self._write_device_config(config_info, config_major_file)

    def _query_device_ip(self, device):
        output = device.shell_with_output("getprop ro.hardware")
        if output == "":
            return ""

        isemulator = re.findall(r"ranchu", output)
        output = device.shell_with_output("ifconfig")
        if output == "":
            return ""

        if len(isemulator) != 0:
            ipaddress = re.findall(r"\b10\.0\.2\.[0-9]{1,3}\b", output)
        else:
            ip_template = r"\b192\.168\.(?:[0-9]{1,3}\.)[0-9]{1,3}\b"
            ipaddress = re.findall(ip_template, output)

        if len(ipaddress) == 0:
            return ""

        return ipaddress[0]

    def _query_device_uuid(self, device):
        """
        1. Run the dumpsys DdmpDeviceMonitorService command to query the value
           of dev_nodeid.
        2. The dump information reported by the soft bus. Local device info,
           should be placed first.
        Note: The dump information may not comply with the JSON format.
        """
        dumpsys_command = "dumpsys DdmpDeviceMonitorService"
        device_info = device.shell_with_output(dumpsys_command)
        if device_info == "":
            return ""

        begin = device_info.find("dev_nodeid")
        if (begin == -1):
            return ""

        end = device_info.find(",", begin)
        if (end == -1):
            return ""

        dev_nodeid_info = device_info[begin:end].replace('"', "")
        return dev_nodeid_info.split(":")[1]

    def _write_device_config(self, device_info, file_path):
        file_dir, file_name = os.path.split(file_path)
        final_file = os.path.join(file_dir, file_name.split('.')[0] + ".desc")
        if os.path.exists(final_file):
            os.remove(final_file)
        with open(file_path, 'w') as file_desc:
            file_desc.write(device_info)
        os.rename(file_path, final_file)


##############################################################################
##############################################################################

