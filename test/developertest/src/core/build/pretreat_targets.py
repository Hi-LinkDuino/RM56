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
import json
import shutil

from core.constants import JsTestConst
from xdevice import platform_logger

LOG = platform_logger("PretreatTargets")


##############################################################################
##############################################################################

class PretreatTargets(object):
    def __init__(self, target_list):
        self.path_list = []
        self.name_list = []
        self.target_list = target_list

    def pretreat_targets_from_list(self):
        path_list, name_list = self._parse_target_info()
        self._pretreat_by_target_name(path_list, name_list)

    def disassemble_targets_from_list(self):
        self._disassemble_by_target_name(self.path_list, self.name_list)

    def _parse_target_info(self):
        path_list = []
        name_list = []

        for line in self.target_list:
            path = line.split(':')[0][2:]
            name = line.split(':')[1].split('(')[0]
            path_list.append(path)
            name_list.append(name)

        return path_list, name_list

    def _pretreat_by_target_name(self, path_list, name_list):
        for name, path in zip(name_list, path_list):
            if name.endswith("JsTest"):
                if self._pretreat_js_target(path, name):
                    self.path_list.append(path)
                    self.name_list.append(name)
                    LOG.info("js test %s pretreat success" % name)

    def _pretreat_js_target(self, path, name):
        template_path = os.path.join(sys.framework_root_dir, "libs",
                                     "js_template", "src")
        target_path = os.path.join(sys.source_code_root_path, path)
        config_path = os.path.join(target_path, "config.json")
        gn_path = os.path.join(target_path, "BUILD.gn")
        gn_bak_path = os.path.join(target_path, "BuildBak")
        test_path = os.path.join(target_path, "src", "main", "js",
                                 "default", "test")
        if not os.path.exists(config_path):
            LOG.error("js test needs config.json file")
            return False
        if not os.path.exists(gn_path):
            LOG.error("js test needs BUILD.gn file")
            return False
        LOG.info("target_path: %s" % target_path)

        #modify BUILD.gn file to compile hap
        output_path = self._parse_output_path_in_gn(gn_path)
        if output_path == "":
            LOG.error(" BUILD.gn needs 'module_output_path'")
            return
        os.rename(gn_path, gn_bak_path)
        template_args = {'output_path': output_path, 'suite_name': name}
        with open(gn_path, 'w') as filehandle:
            filehandle.write(JsTestConst.BUILD_GN_FILE_TEMPLATE %
                             template_args)

        #copy js hap template to target path
        shutil.copytree(template_path, os.path.join(target_path, "src"))
        shutil.copy(config_path, os.path.join(target_path, "src", "main"))
        file_name = os.listdir(target_path)
        for file in file_name:
            if file.endswith(".js"):
                LOG.info("file: %s" % file)
                shutil.copy(os.path.join(target_path, file), test_path)
                with open(os.path.join(test_path, "List.test.js"), 'a') \
                        as list_data:
                    list_data.write("require('./%s')\n" % file)

        #modify i18n json file
        i18n_path = os.path.join(target_path, "src", "main", "js",
                                 "default", "i18n", "en-US.json")
        json_data = ""
        with open(i18n_path, 'r') as i18n_file:
            lines = i18n_file.readlines()
            for line in lines:
                if "TargetName" in line:
                    line = line.replace("TargetName", name)
                json_data += line
        with open(i18n_path, 'w') as i18n_file:
            i18n_file.write(json_data)
        return True

    def _parse_output_path_in_gn(self, gn_path):
        output_path = ""
        with open(gn_path, 'r') as gn_file:
            for line in gn_file.readlines():
                if line.startswith("module_output_path"):
                    output_path = line.split()[2].strip('"')
                    break
        return output_path

    def _disassemble_by_target_name(self, path_list, name_list):
        for name, path in zip(name_list, path_list):
            LOG.info("name: %s path: %s" % (name, path))
            if name.endswith("JsTest"):
                self._disassemble_js_target(path, name)
                LOG.info("js test %s disassemble success" % name)

    def _disassemble_js_target(self, path, name):
        target_path = os.path.join(sys.source_code_root_path, path)
        src_path = os.path.join(target_path, "src")
        gn_path = os.path.join(target_path, "BUILD.gn")
        gn_bak_path = os.path.join(target_path, "BuildBak")

        if os.path.exists(src_path):
            shutil.rmtree(src_path)
        if os.path.exists(gn_path) and os.path.exists(gn_bak_path):
            os.remove(gn_path)
            os.rename(gn_bak_path, gn_path)

##############################################################################
##############################################################################
