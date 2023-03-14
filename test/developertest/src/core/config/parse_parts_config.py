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

import sys
import os
import json

from core.utils import get_build_output_path
from core.common import is_open_source_product
from core.config.config_manager import UserConfigManager


##############################################################################
##############################################################################

class ParsePartsConfig(object):
    def __init__(self, productform):
        self.productform = productform
        self.subsystem_infos, self.part_infos = self.get_infos_data()

    def get_config_file_path(self):
        manager = UserConfigManager()
        testcase_dir = manager.get_test_cases_dir()

        if testcase_dir == "":
            if sys.source_code_root_path != "":
                config_filepath = os.path.join(
                    get_build_output_path(self.productform),
                    "build_configs",
                    "infos_for_testfwk.json")
            else:
                config_filepath = ""
        else:
            config_filepath = os.path.join(
                testcase_dir,
                "infos_for_testfwk.json")
        return config_filepath

    def get_infos_data(self):
        config_filepath = self.get_config_file_path()
        if not os.path.exists(config_filepath):
            print("Error: %s is not exist." % config_filepath)
            return None, None

        data_dic = None
        with open(config_filepath, 'r') as file_handle:
            data_dic = json.load(file_handle)
            if not data_dic:
                print("Error: json file load error.")
                return None, None

        # open source branch, the part form of all product is "phone"
        if is_open_source_product(self.productform):
            product_data_dic = data_dic.get("phone", None)
        else:
            product_data_dic = data_dic.get(self.productform, None)
        if product_data_dic is None:
            print("Error: product_data_dic is None.")
            return None, None

        subsystem_infos = product_data_dic.get("subsystem_infos", None)
        part_infos = product_data_dic.get("part_infos", None)
        return subsystem_infos, part_infos

    def get_subsystem_infos(self):
        return self.subsystem_infos

    def get_part_infos(self):
        return self.part_infos

    def get_subsystem_name_list(self):
        subsystem_name_list = []
        if self.subsystem_infos:
            for item in self.subsystem_infos:
                subsystem_name_list.append(item)
        return subsystem_name_list

    def get_part_list(self, subsystemlist, partlist):
        if len(partlist) != 0:
            return partlist

        if self.subsystem_infos is None:
            return subsystemlist

        part_name_list = []
        if len(subsystemlist) != 0:
            for item in subsystemlist:
                parts = self.subsystem_infos.get(item, [])
                part_name_list.extend(parts)
        return part_name_list


##############################################################################
##############################################################################
