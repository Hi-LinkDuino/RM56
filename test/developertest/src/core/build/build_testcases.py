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
import sys
import json
import shutil
import subprocess
import platform
from xdevice import platform_logger
from core.utils import get_build_output_path
from core.utils import scan_support_product
from core.common import is_open_source_product
from core.config.config_manager import UserConfigManager

BUILD_FILEPATH = "./build.sh"
BUILD_LITE = "build/lite/build.py"
BUILD_TARGET_PLATFORM = "build_platform=\"%s\""
LOG = platform_logger("BuildTestcases")


##############################################################################
##############################################################################

class BuildTestcases(object):
    def __init__(self, project_rootpath):
        self.project_rootpath = project_rootpath

        user_manager = UserConfigManager()
        self.is_build_example = user_manager.get_user_config_flag(
            "build", "example")
        self.build_parameter_dic = user_manager.get_user_config(
            "build", "parameter")

    @classmethod
    def _copy_folder(cls, source_dir, target_dir):
        if not os.path.exists(target_dir):
            os.makedirs(target_dir)

        if platform.system() == 'Windows':
            command = ["xcopy", "/f", "/s", "/e", "/y",
                       "/b", "/q", source_dir, target_dir]
        else:
            command = ["cp", "-rf", source_dir, target_dir]

        LOG.info("command: %s" % str(command))
        return subprocess.call(command) == 0

    @classmethod
    def _get_testcase_outname_by_productform(cls, productform):
        if productform == "phone" or is_open_source_product(productform):
            return ""

        testcase_outpath = ""
        toolchain_filepath = os.path.join(
            get_build_output_path(productform),
            "build_configs",
            "platforms_info",
            "toolchain_to_variant.json")
        if os.path.exists(toolchain_filepath):
            data_dic = []
            with open(toolchain_filepath, 'r') as toolchain_file:
                data_dic = json.load(toolchain_file)
                if not data_dic:
                    LOG.warning("The %s file load error." %
                                toolchain_filepath)
                    data_dic = []
            paltform_toolchain_dic = data_dic.get("platform_toolchain")
            testcase_outpath = paltform_toolchain_dic.get(productform, "")
            pos = testcase_outpath.rfind(':') + 1
            testcase_outpath = testcase_outpath[pos:len(testcase_outpath)]
        return testcase_outpath

    def _delete_testcase_dir(self, productform):
        if is_open_source_product(productform):
            package_out_dir = os.path.join(
                get_build_output_path(productform),
                "packages",
                "phone",
                "tests")
        else:
            package_out_dir = os.path.join(
                get_build_output_path(productform),
                "packages",
                productform,
                "tests")

        LOG.info("package_out_dir=%s" % package_out_dir)
        if os.path.exists(package_out_dir):
            shutil.rmtree(package_out_dir)

        phone_out_dir = os.path.join(
            self.project_rootpath, "out", "release", "tests")
        LOG.info("phone_out_dir=%s" % phone_out_dir)
        if os.path.exists(phone_out_dir):
            shutil.rmtree(phone_out_dir)

        curr_productform_outname = self._get_testcase_outname_by_productform(
            productform)
        if curr_productform_outname == "":
            return

        curr_productform_outdir = os.path.join(
            get_build_output_path(productform),
            curr_productform_outname,
            "tests")
        LOG.info("curr_productform_outdir=%s" % curr_productform_outdir)
        if os.path.exists(curr_productform_outdir):
            shutil.rmtree(curr_productform_outdir)

    def _merge_testcase_dir(self, productform):
        if is_open_source_product(productform):
            package_out_dir = os.path.join(
                get_build_output_path(productform),
                "packages",
                "phone")
        else:
            package_out_dir = os.path.join(
                get_build_output_path(productform),
                "packages",
                productform)
        if platform.system() == 'Windows':
            package_out_dir = os.path.join(package_out_dir, "tests")

        phone_out_dir = os.path.join(get_build_output_path(productform),
                                     "tests")
        if os.path.exists(phone_out_dir):
            self._copy_folder(phone_out_dir, package_out_dir)

        curr_productform_outname = self._get_testcase_outname_by_productform(
            productform)
        if curr_productform_outname == "":
            return

        curr_productform_outdir = os.path.join(
            get_build_output_path(productform),
            curr_productform_outname,
            "tests")
        LOG.info("curr_productform_outdir=%s" % curr_productform_outdir)
        if os.path.exists(curr_productform_outdir):
            self._copy_folder(curr_productform_outdir, package_out_dir)

    def _execute_build_command(self, productform, command):
        build_result = False
        current_path = os.getcwd()
        os.chdir(self.project_rootpath)

        command.append("--product-name")
        command.append(productform)

        if os.path.exists(BUILD_FILEPATH):
            build_command = [BUILD_FILEPATH]
            build_command.extend(command)
            LOG.info("build_command: %s" % str(build_command))
            if subprocess.call(build_command) == 0:
                build_result = True
            else:
                build_result = False
        else:
            LOG.warning("Error: The %s is not exist" % BUILD_FILEPATH)

        os.chdir(current_path)
        return build_result

    def build_fuzz_testcases(self, para):
        self._delete_testcase_dir(para.productform)
        helper_path = os.path.join("..", "libs", "fuzzlib", "fuzzer_helper.py")
        command = [sys.executable, helper_path, 'make',
                   'make_temp_test', para.productform]
        if subprocess.call(command, shell=False) == 0:
            build_result = True
        else:
            build_result = False
        self._merge_testcase_dir(para.productform)
        return build_result

    def build_testcases(self, productform, target):
        command = []
        if self.is_build_example:
            command.append("--gn-args")
            command.append("build_example=true")
        command.append("--build-target")
        command.append(target)
        command.append("--gn-args")
        command.append(BUILD_TARGET_PLATFORM % productform)
        self._delete_testcase_dir(productform)
        build_result = self._execute_build_command(productform, command)
        self._merge_testcase_dir(productform)
        return build_result

    def build_gn_file(self, productform):
        command = []
        if self.is_build_example:
            command.append("--gn-args")
            command.append("build_example=true")
        command.append("--build-only-gn")
        command.append("--gn-args")
        command.append(BUILD_TARGET_PLATFORM % productform)
        return self._execute_build_command(productform, command)

    def build_version(self, productform):
        command = []
        command.append("--build-target")
        command.append("make_all")
        command.append("--gn-args")
        command.append(BUILD_TARGET_PLATFORM % productform)
        return self._execute_build_command(productform, command)


##############################################################################
##############################################################################
