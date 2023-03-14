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

from xdevice import platform_logger
from core.utils import scan_support_product
from core.config.config_manager import FrameworkConfigManager

LOG = platform_logger("Console")


class Parameter(object):
    @classmethod
    def get_valid_test_level_string(cls, test_level):
        if test_level == "":
            return ""

        level_key_list = ["0", "1", "2", "3", "4"]
        level_list = test_level.strip(",").split(",")

        check_flag = True
        for item in level_list:
            item = item.strip(" ")
            if not item.isdigit():
                check_flag = False
                LOG.warning("The testlevel you entered is incorrect.")
                break

            if item not in level_key_list:
                LOG.warning("The testlevel you entered is incorrect.")
                check_flag = False
                break

        test_level_string = ""
        if check_flag:
            for item in level_list:
                item = item.strip(" ")
                test_level_string += (item + ",")
            test_level_string = test_level_string.strip(",")
        return test_level_string

    @classmethod
    def get_testtype_list(cls, type_list):
        test_type_list = []
        if "ALL" not in type_list:
            test_category_dic = \
                FrameworkConfigManager().get_test_category_info()
            for item in type_list:
                item = item.upper()
                data = test_category_dic.get(item, None)
                if data is None:
                    LOG.warning("data is None.")
                    test_type_list = []
                    break
                test_type_list.append(data[0])
        else:
            test_type_list = FrameworkConfigManager().get_all_category_info()
        return test_type_list

    def check_run_parameter(self, options):
        if options.productform is None or options.productform == "":
            LOG.warning("The productform is incorrect.")
            return False

        if "" != options.testcase and "" != options.testlevel:
            LOG.warning("The -l and -c parameters cannot exist at the \
                        same time.")
            return False

        level_string = self.get_valid_test_level_string(options.testlevel)
        if options.testlevel != "" and level_string == "":
            LOG.warning("The testlevel you entered is incorrect.")
            return False

        return True
