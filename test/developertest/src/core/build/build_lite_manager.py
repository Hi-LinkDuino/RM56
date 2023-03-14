#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020 Huawei Device Co., Ltd.
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
import platform
import subprocess

from xdevice import platform_logger


class BuildLiteManager(object):
    """
    build lite system version or test cases
    build param:
    device type:for examples, watch or camera
    board platform:for examples, HI3518ev300 or HI3516DV300
    kernel type:for examples, liteos_m, liteos_a, linux
    """
    log = platform_logger("BuildLiteManager")

    def __init__(self, project_root_path):
        self.project_rootpath = project_root_path

    def build_testcases(self, param):
        if platform.system() != "Linux":
            self.log.info("Windows environment, only use .bin test cases")
            return True

        current_path = os.getcwd()
        os.chdir(self.project_rootpath)

        command = []
        if param.productform.find("wifiiot") == -1:
            command.append("hb")
            command.append("build")
            command.append("-p")
            command.append("%s@hisilicon" % param.productform)
            command.append("-b")
            command.append("debug")
            if param.testsuit != "":
                command.append("target=%s" % param.testsuit)
        else:
            build_script = os.path.abspath(os.path.join(
                os.path.dirname(__file__),
                "build_lite_testcases.sh"))
            print("build_script=%s" % build_script)
            command.append(build_script)
            command.append("product=%s" % param.productform)
            command.append("kernel=liteos_m")
            if param.testsuit != "":
                command.append("target=%s" % param.testsuit)
        self.log.info("build_command: %s" % str(command))

        build_result = False
        try:
            build_result = subprocess.call(command) == 0
        except IOError as exception:
            self.log.error("build test case failed, exception=%s" % exception)

        if build_result:
            self.log.info("build test case successed.")
        else:
            self.log.info("build test case failed.")

        os.chdir(os.path.realpath(current_path))
        return build_result

    def build_version(self, productform):
        current_path = os.getcwd()
        os.chdir(self.project_rootpath)

        command = []
        command.append("hb")
        command.append("build")
        command.append("-p")
        command.append("%s@hisilicon" % productform)
        command.append("-f")
        self.log.info("build_command: %s" % str(command))

        build_result = False
        try:
            build_result = subprocess.call(command) == 0
        except IOError as exception:
            self.log.error("build version failed, exception=%s" % exception)

        if build_result:
            self.log.info("build version successed.")
        else:
            self.log.info("build version failed.")

        os.chdir(os.path.realpath(current_path))
        return build_result
