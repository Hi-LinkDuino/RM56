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
import platform
import subprocess
import sys
import uuid

from xdevice import platform_logger
from xdevice import IDriver
from xdevice import Plugin

from xdevice_extension._core.utils import get_decode

KUNPENG_TEST = "KunpengTest"
LOG = platform_logger(KUNPENG_TEST)


@Plugin(type=Plugin.DRIVER, id=KUNPENG_TEST)
class KunpengTest(IDriver):
    """
    KunpengTest is a Test that runs a host-driven test on given kunpeng
    servers.
    """
    # test driver config
    config = None
    result = ""

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        pass

    def __execute__(self, request):
        self.config = request.config

        # 1.parse config file
        mainconfig_file = request.get_config_file()
        if not mainconfig_file:
            LOG.error("config file not exists")
            return
        LOG.debug("KunpengTest mainconfig_file FilePath: %s" % mainconfig_file)

        # 2.set params
        tmp_id = str(uuid.uuid4())
        tmp_folder = os.path.join(self.config.report_path, "temp")
        self.config.tmp_sub_folder = os.path.join(tmp_folder, "task_" + tmp_id)
        os.makedirs(self.config.tmp_sub_folder, exist_ok=True)

        # 3.test execution
        self._start_kunpengtest_with_cmd(mainconfig_file)
        return

    def _start_kunpengtest_with_cmd(self, mainconfig_file):
        from xdevice import Variables
        # insert& _kunpengtest path for loading kunpengtest module
        kunpengtest_module = os.path.join(Variables.modules_dir,
                                          "_kunpengtest")
        sys.path.insert(1, kunpengtest_module)

        cmd_parts = []
        if platform.system() == "Windows":
            cmd_parts.append("python")
        else:
            cmd_parts.append("python3")
        relative_path = "uniautos/src/Framework/Dev/bin/UniAutosScript.py"
        cmd_parts.append(os.path.abspath(os.path.join(kunpengtest_module,
                                         relative_path)))
        cmd_parts.append("-c")
        cmd_parts.append(mainconfig_file)
        cmd_parts.append("-rp")
        cmd_parts.append(self.config.tmp_sub_folder)
        cmd = " ".join(cmd_parts)
        LOG.info("start kunpengtest with cmd: %s" % cmd)
        try:
            proc = subprocess.Popen(cmd_parts, shell=False,
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)
            (out, _) = proc.communicate()
            out = get_decode(out).strip()
            for line in out.split("\n"):
                LOG.info(line)
        except (subprocess.CalledProcessError, FileNotFoundError) as error:
            LOG.error("kunpeng test error: %s" % error)

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""
