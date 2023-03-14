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
import tempfile
from abc import ABCMeta
from abc import abstractmethod

from core.config.resource_manager import ResourceManager


##############################################################################
##############################################################################

DEVICE_TEST_PATH = "/%s/%s/" % ("data", "test")


def get_level_para_string(level_string):
    level_list = list(set(level_string.split(",")))
    level_para_string = ""
    for item in level_list:
        if not item.isdigit():
            continue
        item = item.strip(" ")
        level_para_string += ("Level%s," % item)
    level_para_string = level_para_string.strip(",")
    return level_para_string


def make_long_command_file(command, longcommand_path, filename):
    sh_file_name = '%s.sh' % filename
    file_path = os.path.join(longcommand_path, sh_file_name)
    try:
        with open(file_path, "a") as file_desc:
            file_desc.write(command)
    except(IOError, ValueError) as err_msg:
        print("Error for make long command file: ", err_msg)
    return sh_file_name, file_path


##############################################################################
##############################################################################


class ITestDriver:
    __metaclass__ = ABCMeta

    @abstractmethod
    def execute(self, suite_file, push_flag=False):
        pass


##############################################################################
##############################################################################


class CppTestDriver(ITestDriver):
    def __init__(self, device):
        self.device = device

    def execute(self, suite_file, background=False):
        file_name = os.path.basename(suite_file)

        long_command_path = tempfile.mkdtemp(prefix="long_command_",
            dir=os.path.join(os.environ.get('PYTEST_RESULTPATH'), "temp"))
        command = "cd %s; rm -rf %s.xml; chmod +x *; ./%s" % (
            DEVICE_TEST_PATH,
            file_name,
            file_name)

        print("command: %s" % command)
        sh_file_name, file_path = make_long_command_file(command,
            long_command_path,
            file_name)
        self.device.push_file(file_path, DEVICE_TEST_PATH)

        # push resource files
        resource_manager = ResourceManager()
        resource_data_dic, resource_dir = \
            resource_manager.get_resource_data_dic(suite_file)
        resource_manager.process_preparer_data(resource_data_dic, resource_dir,
                                               self.device)
        if background:
            sh_command = "nohup sh %s >%s 2>&1 &" % (
                os.path.join(DEVICE_TEST_PATH, sh_file_name),
                os.path.join(DEVICE_TEST_PATH, "agent.log"))
        else:
            sh_command = "sh %s" % (
                os.path.join(DEVICE_TEST_PATH, sh_file_name))

        return self.device.shell(sh_command)


##############################################################################
##############################################################################

