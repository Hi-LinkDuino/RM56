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

from core.constants import ToolCommandType
from core.utils import get_file_list
from core.utils import get_file_list_by_postfix
from core.utils import get_build_output_path
from core.utils import scan_support_product
from core.config.config_manager import UserConfigManager
from core.config.config_manager import FrameworkConfigManager
from core.config.parse_parts_config import ParsePartsConfig

CMD_KEY_PRODUCTLIST = "productlist"
CMD_KEY_TYPELIST = "typelist"
CMD_KEY_SUBSYSTEMLIST = "subsystemlist"
CMD_KEY_PARTLIST = "partlist"
CMD_KEY_MODULELIST = "modulelist"


TOOL_VERSION_INFO = """Welcome to DeveloperTest V1.0.0.
"""

HLEP_COMMAND_INFOMATION = """use help [follow command] for more information:
    """ + \
    "show: " + """Display a list of supported show command.
    """ + \
    "run:  " + """Display a list of supported run command.
    """ + \
    "list: " + """Display a list of supported device.
    """ + \
    "quit: " + """Exit the test framework application.
"""

SUPPORT_COMMAND_SHOW = """use show [follow command] for more information:
    """ + \
    "productlist" + """
    """ + \
    "typelist" + """
    """ + \
    "subsystemlist" + """
    """ + \
    "partlist" + """
    """ + \
    "modulelist" + """
"""

RUNCASES_INFOMATION = """run:
    This command is used to execute the selected testcases.
    It includes a series of processes such as use case compilation, \
execution, and result collection.

usage: run [-p PRODUCTFORM]
           [-t [TESTTYPE [TESTTYPE ...]]]
           [-ss [SUBSYSTEM [SUBSYSTEM ...]]]
           [-tp [TESTPART [TESTPART ...]]]
           [-tm TESTMODULE]
           [-ts TESTSUIT]
           [-tc TESTCASE]
           [-tl TESTLEVEL]

optional arguments:
  -p PRODUCTFORM, --productform PRODUCTFORM
                        Specified product form
  -t [TESTTYPE [TESTTYPE ...]], --testtype [TESTTYPE [TESTTYPE ...]]
                        Specify test type(UT,MST,ST,PERF,ALL)
  -ss [SUBSYSTEM [SUBSYSTEM ...]], --subsystem [SUBSYSTEM [SUBSYSTEM ...]]
                        Specify test subsystem
  -tp [TESTPART [TESTPART ...]], --testpart [TESTPART [TESTPART ...]]
                        Specify test testpart
  -tm TESTMODULE, --testmodule TESTMODULE
                        Specified test module
  -ts TESTSUIT, --testsuit TESTSUIT
                        Specify test suit
  -tc TESTCASE, --testcase TESTCASE
                        Specify test case
  -tl TESTLEVEL, --testlevel TESTLEVEL

Examples:
    run -t UT
    run -t UT -ss aafwk
    run -t UT -ss aafwk -tm base_test
    run -t UT -ss aafwk -tm base_test -ts base_test
    run -t UT -ss aafwk -tm base_test -ts base_test -tl 2
    run -t UT -ss aafwk -tm base_test -ts base_test -tc \
AAFwkBaseTest.*
    run -t UT -ss aafwk -tm base_test -ts base_test -tc \
AAFwkBaseTest.object_test_001
    run -t MST
    ...
    run -t ALL
    ...
"""

LIST_INFOMATION = "list\n" + """
    This command is used to display device list.
"""

QUIT_INFOMATION = "quit\n" + """
    This command is used to exit the test framework application.
"""


#############################################################################
#############################################################################

def select_user_input(data_list):
    data_item_count = len(data_list)
    select_item_value = ""
    select_item_index = -1

    if len(data_list) != 0:
        count = 0
        while True:
            input_data = input("")
            if "" != input_data and input_data.isdigit():
                input_num = int(input_data)
                if input_num > 0 and (input_num <= data_item_count):
                    select_item_index = input_num - 1
                    select_item_value = data_list[input_num - 1]
                    break
                else:
                    print("The data you entered is out of range, \
                        please re-enter:")
                    count += 1
            else:
                if "" == input_data:
                    select_item_index = 0
                    select_item_value = data_list[0]
                    break
                else:
                    print("You entered a non-numeric character, \
                        please re-enter:")
                    count += 1

            if count >= 3:
                print("You entered the error three times in a row, \
                    exit the frame.")
                quit()
                sys.exit(0)
        return select_item_value, select_item_index


def select_productform():
    select_value = "phone"
    scan_product_list = scan_support_product()
    config_product_list = \
        FrameworkConfigManager().get_framework_config("productform")
    productform_list = scan_product_list + config_product_list
    if len(productform_list) != 0:
        print("Please select the current tested product form:")
        for index, element in enumerate(productform_list):
            print("%d. %s" % (index + 1, element))
        print("default is [1] %s" % productform_list[0])
        select_value, _ = select_user_input(productform_list)
    print(select_value)
    return select_value


def show_wizard_mode():
    wizard_data_dic = {}
    print("+++++++++++++++++++++++++++++++++++++++++++++")

    productform = select_productform()
    if productform == "":
        productform = "phone"
    wizard_data_dic["productform"] = productform

    print("+++++++++++++++++++++++++++++++++++++++++++++")
    print("The environment is ready, please use the run command to test.")
    return wizard_data_dic


#############################################################################
#############################################################################

def display_help_info(para_list):
    if len(para_list) == 0 or para_list[0] != ToolCommandType.TOOLCMD_KEY_HELP:
        print("This command is not support.")
        return

    if len(para_list) > 1:
        display_help_command_info(para_list[1])
    else:
        print(TOOL_VERSION_INFO)
        print(HLEP_COMMAND_INFOMATION)


def display_show_info(para_list, productform):
    if len(para_list) == 0 or para_list[0] != ToolCommandType.TOOLCMD_KEY_SHOW:
        print("This command is not support.")
        return

    if len(para_list) > 1:
        display_show_command_info(para_list[1], productform)
    else:
        print(SUPPORT_COMMAND_SHOW)


#############################################################################
#############################################################################

def get_module_list_from_output_dir(product_form):
    module_path_list = []
    all_product_list = scan_support_product()
    if product_form in all_product_list:
        module_list_file_path = os.path.join(
            get_build_output_path(product_form),
            "module_list_files")
    else:
        module_list_file_path = os.path.join(
            get_build_output_path(product_form),
            "test_info",
            "module_list_files")
    print(module_list_file_path)
    if os.path.exists(module_list_file_path):
        file_list = get_file_list_by_postfix(module_list_file_path, ".mlf")
        for file in file_list:
            module_path = \
                file[len(module_list_file_path) + 1: file.rfind(os.sep)]
            if module_path != "" and module_path not in module_path_list:
                module_path_list.append(module_path)
    else:
        print("%s does not exist." % module_list_file_path)
    module_path_list.sort()
    return module_path_list


def get_module_list_from_case_dir(test_case_dir):
    file_list = []
    test_case_tests_path = test_case_dir
    if not os.path.exists(test_case_tests_path):
        return file_list

    for test_type in os.listdir(test_case_tests_path):
        file_path = os.path.join(test_case_tests_path, test_type)
        for dirs in os.walk(file_path):
            files = get_file_list(find_path=dirs[0])
            for file_name in files:
                if "" != file_name and -1 == file_name.find(__file__):
                    file_name = os.path.join(dirs[0], file_name)
                    if os.path.isfile(file_name):
                        file_name = file_name[len(file_path) + 1: \
                            file_name.rfind(os.sep)]
                        file_list.append(file_name)
    return file_list


def get_module_list(product_form):
    module_path_list = []
    testcase_dir = UserConfigManager().get_test_cases_dir()
    if testcase_dir == "":
        module_path_list = get_module_list_from_output_dir(product_form)
    else:
        module_path_list = get_module_list_from_case_dir(testcase_dir)
    return module_path_list


#############################################################################
#############################################################################


def show_product_list():
    print("List of currently supported productform:")
    scan_product_list = scan_support_product()
    config_product_list = \
        FrameworkConfigManager().get_framework_config("productform")
    productform_list = scan_product_list + config_product_list
    if 0 != len(productform_list):
        for index, element in enumerate(productform_list):
            print("    %d. %s" % (index + 1, element))
    else:
        print("No category specified.")


def show_testtype_list():
    print("List of currently supported test types:")
    testtype_list = FrameworkConfigManager().get_framework_config(
        "test_category")
    if 0 != len(testtype_list):
        for index, element in enumerate(testtype_list):
            print("    %d. %s" % (index + 1, element))
    else:
        print("No category specified.")


def show_subsystem_list(product_form):
    print("List of currently supported subsystem names:")
    parser = ParsePartsConfig(product_form)
    subsystem_name_list = parser.get_subsystem_name_list()
    if len(subsystem_name_list) == 0:
        return

    subsystem_name_list.sort()
    for index, element in enumerate(subsystem_name_list):
        print("    %d. %s" % (index + 1, element))


def show_partname_list(product_form):
    print("List of currently supported part names:")
    parser = ParsePartsConfig(product_form)
    subsystem_name_list = parser.get_subsystem_name_list()
    if len(subsystem_name_list) == 0:
        return

    subsystem_name_list.sort()
    subsystem_infos = parser.get_subsystem_infos()
    for subsystem in subsystem_name_list:
        print("%s:" % subsystem)
        part_name_list = subsystem_infos[subsystem]
        part_name_list.sort()
        for index, element in enumerate(part_name_list):
            print("    %d. %s" % (index + 1, element))


def show_module_list(product_form):
    print("List of currently supported module names:")
    subsystem_name_list = []
    subsystem_module_list = get_module_list(product_form)

    for item in subsystem_module_list:
        if item != "":
            subsystem_name = item.split(os.sep)[0]
            if subsystem_name not in subsystem_name_list:
                subsystem_name_list.append(subsystem_name)

    for subsystem_name in subsystem_name_list:
        print("%s:" % subsystem_name)
        index = 0
        module_value_list = []
        for item in subsystem_module_list:
            find_key = subsystem_name + os.sep
            pos_subsystem = item.find(find_key)
            if pos_subsystem >= 0:
                subsystem_module_dir = \
                    item[pos_subsystem + len(find_key):len(item)]
                module_value = subsystem_module_dir.split(os.sep)[0]
                if module_value not in module_value_list:
                    module_value_list.append(module_value)
                    index += 1
                    print("    %d. %s" % (index, module_value))


def display_help_command_info(command):
    if command == ToolCommandType.TOOLCMD_KEY_SHOW:
        print(SUPPORT_COMMAND_SHOW)
    elif command == ToolCommandType.TOOLCMD_KEY_RUN:
        print(RUNCASES_INFOMATION)
    elif command == ToolCommandType.TOOLCMD_KEY_LIST:
        print(LIST_INFOMATION)
    elif command == ToolCommandType.TOOLCMD_KEY_QUIT:
        print(QUIT_INFOMATION)
    else:
        print("'%s' command no help information." % command)


def display_show_command_info(command, product_form="phone"):
    if command == CMD_KEY_PRODUCTLIST:
        show_product_list()
    elif command == CMD_KEY_TYPELIST:
        show_testtype_list()
    elif command == CMD_KEY_SUBSYSTEMLIST:
        show_subsystem_list(product_form)
    elif command == CMD_KEY_PARTLIST:
        show_partname_list(product_form)
    elif command == CMD_KEY_MODULELIST:
        show_module_list(product_form)
    else:
        print("This command is not support.")


#############################################################################
#############################################################################
