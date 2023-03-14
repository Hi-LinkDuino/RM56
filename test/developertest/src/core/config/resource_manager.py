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
import shutil
import xml.etree.ElementTree as ElementTree
from xdevice import platform_logger
from xdevice import DeviceTestType
from core.constants import ConfigFileConst

LOG = platform_logger("ResourceManager")
DEFAULT_TIMEOUT = "300"


##############################################################################
##############################################################################

class ResourceManager(object):
    def __init__(self):
        pass

    def get_resource_data_dic(self, testsuit_filepath):
        resource_dir = ""
        data_dic = {}

        target_name, _ = self._get_file_name_extension(testsuit_filepath)
        xml_filepath = self.get_resource_xml_file_path(testsuit_filepath)
        if not os.path.exists(xml_filepath):
            return data_dic, resource_dir

        data_dic = self.get_resource_data(xml_filepath, target_name)
        resource_dir = os.path.abspath(os.path.dirname(xml_filepath))
        return data_dic, resource_dir

    def get_resource_data(self, xml_filepath, target_name):
        data_dic = {}
        if os.path.exists(xml_filepath):
            data_dic = self._parse_resource_test_xml_file(
                xml_filepath, target_name)
        return data_dic

    def _parse_resource_test_xml_file(self, filepath, targetname):
        data_dic = {}

        node = self.find_node_by_target(filepath, targetname)
        if node:
            target_attrib_list = []
            target_attrib_list.append(node.attrib)
            environment_data_list = []
            env_node = node.find("environment")
            if env_node:
                environment_data_list.append(env_node.attrib)
                for element in env_node.findall("device"):
                    environment_data_list.append(element.attrib)
                    for option_element in element.findall("option"):
                        environment_data_list.append(option_element.attrib)

            preparer_data_list = []
            pre_node = node.find("preparer")
            if pre_node:
                preparer_data_list.append(pre_node.attrib)
                for element in pre_node.findall("option"):
                    preparer_data_list.append(element.attrib)

            cleaner_data_list = []
            clr_node = node.find("cleaner")
            if clr_node:
                cleaner_data_list.append(clr_node.attrib)
                for element in clr_node.findall("option"):
                    cleaner_data_list.append(element.attrib)

            data_dic["nodeattrib"] = target_attrib_list
            data_dic["environment"] = environment_data_list
            data_dic["preparer"] = preparer_data_list
            data_dic["cleaner"] = cleaner_data_list

        return data_dic

    @staticmethod
    def find_node_by_target(file_path, targe_tname):
        node = None
        try:
            if os.path.exists(file_path):
                tree = ElementTree.parse(file_path)
                root = tree.getroot()
                targets = root.getiterator("target")
                for target in targets:
                    curr_dic = target.attrib
                    if curr_dic.get("name") == targe_tname or \
                            targe_tname.startswith(curr_dic.get("name")):
                        node = target
                        break
        except ElementTree.ParseError as xml_exception:
            LOG.error("resource_test.xml parsing failed." +
                      xml_exception.args)
        return node

    ##########################################################################
    ##########################################################################

    @classmethod
    def _get_file_name_extension(cls, filepath):
        _, fullname = os.path.split(filepath)
        filename, ext = os.path.splitext(fullname)
        LOG.debug("file path:{}".format(filepath))
        return filename, ext

    @classmethod
    def get_dir_name(cls, dir_path):
        dir_name = ""
        if os.path.isdir(dir_path) and dir_path[-1] != ".":
            dir_name_list = dir_path.rstrip(os.sep).split(os.sep)
            if len(dir_name_list) > 1:
                dir_name = dir_name_list[-1]
        return dir_name

    def process_resource_file(self, resource_dir, preparer_list, device):
        for item in preparer_list:
            if "name" not in item.keys():
                continue

            if item["name"] == "push":
                push_value = item["value"]
                find_key = "->"
                pos = push_value.find(find_key)
                src = os.path.join(resource_dir, push_value[0:pos].strip())
                dst = push_value[pos + len(find_key):len(push_value)].strip()
                src = src.replace("/", os.sep)
                dir_name = self.get_dir_name(src)
                if dir_name != "":
                    dst = dst.rstrip("/") + "/" + dir_name
                device.execute_shell_command("mkdir -p %s" % dst)
                device.push_file(src, dst)
            elif item["name"] == "pull":
                push_value = item["value"]
                find_key = "->"
                pos = push_value.find(find_key)
                src = os.path.join(resource_dir, push_value[0:pos].strip())
                dst = push_value[pos + len(find_key):len(push_value)].strip()
                device.pull_file(src, dst)
            elif item["name"] == "shell":
                command = item["value"].strip()
                device.execute_shell_command(command)
            else:
                command = item["name"] + " " + item["value"]
                command = command.strip()
                device.hdc_command(command)

    def lite_process_resource_file(self, resource_dir, preparer_list):
        for item in preparer_list:
            if "name" not in item.keys():
                continue

            if item["name"] == "push":
                copy_value = item["value"]
                find_key = "->"
                pos = copy_value.find(find_key)
                src = os.path.join(resource_dir, copy_value[0:pos].strip())
                dst = copy_value[pos + len(find_key):len(copy_value)].strip()
                shutil.copy(src, dst)

            elif item["name"] == "pull":
                copy_value = item["value"]
                find_key = "->"
                pos = copy_value.find(find_key)
                src = os.path.join(resource_dir, copy_value[0:pos].strip())
                dst = copy_value[pos + len(find_key):len(copy_value)].strip()
                shutil.copyfile(dst, src)
            else:
                command = item["name"] + " " + item["value"]
                command = command.strip()
                self.lite_device.execute_command_with_timeout(command, case_type=DeviceTestType.lite_cpp_test)

    @classmethod
    def get_env_data(cls, environment_list):
        env_data_dic = {}
        device_name = ""
        option_dic = {}

        for item in environment_list:
            if "type" in item.keys():
                if device_name != "":
                    temp_dic = option_dic.copy()
                    env_data_dic[device_name] = temp_dic
                    device_name = ""
                    option_dic.clear()
                device_name = item["type"]

            if "name" in item.keys():
                name = item["name"]
                value = item["value"]
                option_dic[name] = value

        if device_name != "":
            temp_dic = option_dic.copy()
            env_data_dic[device_name] = temp_dic
            device_name = ""
            option_dic.clear()
        LOG.debug("get environment data finish")
        return env_data_dic

    @staticmethod
    def get_resource_xml_file_path(test_suit_file_path):
        current_dir = os.path.dirname(test_suit_file_path)
        while True:
            if current_dir.endswith(os.sep + "tests"):
                current_dir = ""
                break
            if current_dir == "/" or current_dir.endswith(":\\"):
                current_dir = ""
                break
            if os.path.exists(os.path.join(current_dir, "resource")):
                break
            current_dir = os.path.dirname(current_dir)

        if current_dir != "":
            xml_filepath = os.path.join(
                current_dir,
                "resource",
                ConfigFileConst.RESOURCECONFIG_FILEPATH)
            if not os.path.exists(xml_filepath):
                xml_filepath = os.path.join(
                    current_dir,
                    "resource",
                    ConfigFileConst.CASE_RESOURCE_FILEPATH)
        else:
            xml_filepath = ""
        LOG.info("xml_filepath = %s" % xml_filepath)
        return xml_filepath

    @classmethod
    def get_nodeattrib_data(cls, data_dic):
        curr_timeout = DEFAULT_TIMEOUT
        if "nodeattrib" in data_dic.keys():
            LOG.info("++++++++++++++nodeattrib+++++++++++++++")
            nodeattrib_list = data_dic["nodeattrib"]
            if len(nodeattrib_list) != 0:
                node_item_dic = nodeattrib_list[0]
                if "timeout" in node_item_dic:
                    curr_timeout = node_item_dic["timeout"]
        return curr_timeout

    def get_environment_data(self, data_dic):
        env_data_dic = {}
        if "environment" in data_dic.keys():
            LOG.info("++++++++++++++environment+++++++++++++++")
            environment_list = data_dic["environment"]
            env_data_dic = self.get_env_data(environment_list)
        return env_data_dic

    def process_preparer_data(self, data_dic, resource_dir, device):
        if "preparer" in data_dic.keys():
            LOG.info("++++++++++++++preparer+++++++++++++++")
            preparer_list = data_dic["preparer"]
            self.process_resource_file(resource_dir, preparer_list, device)
        return

    def lite_process_preparer_data(self, data_dic, resource_dir):
        if "preparer" in data_dic.keys():
            LOG.info("++++++++++++++preparer+++++++++++++++")
            preparer_list = data_dic["preparer"]
            self.lite_process_resource_file(resource_dir, preparer_list)
        return

    def process_cleaner_data(self, data_dic, resource_dir, device):
        if "cleaner" in data_dic.keys():
            LOG.info("++++++++++++++cleaner+++++++++++++++")
            cleaner_list = data_dic["cleaner"]
            self.process_resource_file(resource_dir, cleaner_list, device)
        return


##############################################################################
##############################################################################
