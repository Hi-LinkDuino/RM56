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
import xml.etree.ElementTree as ET

from xdevice import platform_logger
from core.constants import ConfigFileConst

LOG = platform_logger("config_manager")
CONFIG_PATH = os.path.join(sys.framework_res_dir, "config")


class FrameworkConfigManager(object):
    def __init__(self, filepath=""):
        if filepath == "":
            self.filepath = os.path.abspath(os.path.join(
                CONFIG_PATH, ConfigFileConst.FRAMECONFIG_FILEPATH))
        else:
            self.filepath = filepath

    def get_framework_config(self, target_name):
        data_list = []
        try:
            if os.path.exists(self.filepath):
                tree = ET.parse(self.filepath)
                root = tree.getroot()
                node = root.find(target_name)
                for sub in node:
                    value = sub.attrib.get("name")
                    if value and value != "":
                        data_list.append(value)
        except ET.ParseError as xml_exception:
            LOG.error(("Parse %s fail!" % self.filepath) + xml_exception.args)
        return data_list

    def get_test_category_info(self, target_name="test_category"):
        test_type_timeout_dic = {}
        try:
            if os.path.exists(self.filepath):
                tree = ET.parse(self.filepath)
                root = tree.getroot()
                node = root.find(target_name)
                for sub in node:
                    name = sub.attrib.get("name")
                    desc = sub.attrib.get("desc")
                    timeout = sub.attrib.get("timeout")
                    if name and desc and timeout:
                        test_type_timeout_dic[name] = (desc, timeout)
            else:
                LOG.error("The %s file does not exist." % self.filepath)
        except ET.ParseError as xml_exception:
            LOG.error(("Parse %s fail!" % self.filepath) + xml_exception.args)
        return test_type_timeout_dic

    def get_all_category_info(self, target_name="all_category"):
        return self.get_framework_config(target_name)


class FilterConfigManager(object):
    def __init__(self, filepath=""):
        if filepath == "":
            self.filepath = os.path.abspath(
                os.path.join(CONFIG_PATH,
                             ConfigFileConst.FILTERCONFIG_FILEPATH))
        else:
            self.filepath = filepath

    def get_filtering_list(self, target_name, product_form):
        filter_data_list = []
        try:
            if os.path.exists(self.filepath):
                tree = ET.parse(self.filepath)
                root = tree.getroot()
                for child in root:
                    if child.tag != target_name:
                        continue
                    for child2 in child:
                        if child2.tag.lower() != product_form.lower():
                            continue
                        for child3 in child2:
                            if child3.text != "" and child3.text is not None:
                                filter_data_list.append(child3.text)
            else:
                LOG.error("The %s file does not exist." % self.filepath)
        except ET.ParseError as xml_exception:
            LOG.error(("Parse %s fail!" % self.filepath) + xml_exception.args)
        return filter_data_list

    def get_filter_config_path(self):
        return self.filepath


class ResourceConfigManager(object):
    def __init__(self, filepath=""):
        if filepath == "":
            self.filepath = os.path.abspath(os.path.join(
                CONFIG_PATH, ConfigFileConst.RESOURCECONFIG_FILEPATH))
            if not os.path.exists(self.filepath):
                self.filepath = os.path.abspath(os.path.join(
                    CONFIG_PATH, ConfigFileConst.CASE_RESOURCE_FILEPATH))
        else:
            self.filepath = filepath

    def get_resource_config(self):
        data_list = []
        try:
            if os.path.exists(self.filepath):
                tree = ET.parse(self.filepath)
                root = tree.getroot()
                for child in root:
                    temp_list = [child.attrib]
                    for sub in child:
                        temp_list.append(sub.attrib)
                    data_list.append(temp_list)
            else:
                LOG.error("The %s is not exist." % self.filepath)
        except ET.ParseError as xml_exception:
            LOG.error(("Parse %s fail!" % self.filepath) + xml_exception.args)
        return data_list

    def get_resource_config_path(self):
        return self.filepath


class UserConfigManager(object):
    def __init__(self, config_file=""):
        if config_file == "":
            self.filepath = os.path.abspath(os.path.join(
                CONFIG_PATH, ConfigFileConst.USERCONFIG_FILEPATH))
        else:
            if os.path.isabs(config_file):
                self.filepath = config_file
            else:
                self.filepath = os.path.abspath(
                    os.path.join(CONFIG_PATH, config_file))

    def get_user_config_list(self, tag_name):
        data_dic = {}
        try:
            if os.path.exists(self.filepath):
                tree = ET.parse(self.filepath)
                root = tree.getroot()
                for child in root:
                    if tag_name == child.tag:
                        for sub in child:
                            data_dic[sub.tag] = sub.text
        except ET.ParseError as xml_exception:
            LOG.error(("Parse %s fail!" % self.filepath) + xml_exception.args)
        return data_dic

    @classmethod
    def content_strip(cls, content):
        return content.strip()

    @classmethod
    def _verify_duplicate(cls, items):
        if len(set(items)) != len(items):
            LOG.warning("find duplicate sn config, configuration incorrect")
            return False
        return True

    def _handle_str(self, content):
        config_list = map(self.content_strip, content.split(';'))
        config_list = [item for item in config_list if item]
        if config_list:
            if not self._verify_duplicate(config_list):
                return []
        return config_list

    def get_sn_list(self):
        sn_select_list = []
        try:
            data_dic = {}
            if os.path.exists(self.filepath):
                tree = ET.parse(self.filepath)
                root = tree.getroot()

                for node in root.findall("environment/device"):
                    if node.attrib["type"] != "usb-hdc":
                        continue
                    for sub in node:
                        data_dic[sub.tag] = sub.text if sub.text else ""
                    sn_config = data_dic.get("sn", "")
                    if sn_config:
                        sn_select_list = self._handle_str(sn_config)
                    break
        except ET.ParseError as xml_exception:
            LOG.warning("occurs exception:{}".format(xml_exception.args))
            sn_select_list = []
        return sn_select_list

    def get_user_config(self, target_name, sub_target=""):
        data_dic = {}
        try:
            if os.path.exists(self.filepath):
                tree = ET.parse(self.filepath)
                root = tree.getroot()

                node = root.find(target_name)
                if not node:
                    return None

                if sub_target != "":
                    node = node.find(sub_target)
                if not node:
                    return None

                for sub in node:
                    if sub.text is None:
                        data_dic[sub.tag] = ""
                    else:
                        data_dic[sub.tag] = sub.text
        except ET.ParseError as xml_exception:
            LOG.error(("Parse %s fail!" % self.filepath) + xml_exception.args)
        return data_dic

    def get_user_config_flag(self, target_name, sub_target):
        config_flag = self.get_user_config(target_name).get(sub_target, "")
        if config_flag == "":
            return False
        return True if config_flag.lower() == "true" else False

    def get_device(self, target_name):
        data_dic = {}
        if os.path.exists(self.filepath):
            tree = ET.parse(self.filepath)
            config_content = tree.getroot()
            for node in config_content.findall(target_name):
                for sub in node:
                    if sub.text is None:
                        data_dic[sub.tag] = ""
                    else:
                        data_dic[sub.tag] = sub.text
                break
        return data_dic

    def get_test_cases_dir(self):
        testcase_path = self.get_user_config("test_cases").get("dir", "")
        if testcase_path != "":
            testcase_path = os.path.abspath(testcase_path)
        return testcase_path


class BuildConfigManager(object):
    def __init__(self, filepath=""):
        if filepath == "":
            self.filepath = os.path.abspath(os.path.join(
                CONFIG_PATH, ConfigFileConst.BUILDCONFIG_FILEPATH))
        else:
            self.filepath = filepath

    def get_build_config(self, target_name):
        data_list = []
        try:
            if os.path.exists(self.filepath):
                tree = ET.parse(self.filepath)
                root = tree.getroot()
                node = root.find(target_name)
                for sub in node:
                    value = sub.attrib.get("name")
                    if value and value != "":
                        data_list.append(value)
        except ET.ParseError as xml_exception:
            LOG.error(("Parse %s fail!" % self.filepath) + xml_exception.args)
        return data_list

    def get_build_path(self):
        return self.filepath

class FuzzerConfigManager(object):
    def __init__(self, config_path=""):
        if config_path == "":
            self.filepath = self.filepath = os.path.abspath(os.path.join(
                CONFIG_PATH, ConfigFileConst.FUZZCONFIG_FILEPATH))
        else:
            self.filepath = config_path

    def get_fuzzer_config(self, target_name):
        config_list = []
        try:
            LOG.info("fuzzer config file :%s" % self.filepath)
            if os.path.exists(self.filepath):
                tree = ET.parse(self.filepath)
                root = tree.getroot()
                node = root.find(target_name)
                LOG.info("before for")
                for sub in node:
                    if sub.text is not None:
                        config_list.append(sub.text)
        except ET.ParseError as xml_exception:
            LOG.error(("Parse %s fail!" % self.filepath) + xml_exception.args)
        return config_list
