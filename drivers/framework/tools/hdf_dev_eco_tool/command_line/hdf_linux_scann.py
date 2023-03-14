#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import json
import os
import platform
import re
from string import Template

import hdf_utils
from hdf_tool_exception import HdfToolException
from .hdf_command_error_code import CommandErrorCode


class HdfLinuxScan(object):
    def __init__(self, root, vendor, board):
        self.root = root
        self.vendor = vendor
        self.board = board
        self.kernel = "linux"

        self.makefile_path = hdf_utils.get_vendor_makefile_path(
            root, kernel="linux")
        if not os.path.exists(self.makefile_path):
            raise HdfToolException(
                'Makefile: %s not exist' % self.makefile_path,
                CommandErrorCode.TARGET_NOT_EXIST)

        self.framework_dir = hdf_utils.get_module_dir(self.root)
        if not os.path.exists(self.framework_dir):
            raise HdfToolException(
                'file: %s not exist' % self.framework_dir,
                CommandErrorCode.TARGET_NOT_EXIST)

        self.hcs_path = hdf_utils.get_hcs_file_path(
            self.root, self.vendor, self.board)
        if not os.path.exists(self.hcs_path):
            raise HdfToolException('file: %s not exist' % self.hcs_path,
                                   CommandErrorCode.TARGET_NOT_EXIST)
        self.contents = hdf_utils.read_file_lines(self.makefile_path)
        self.driver_configs_list = ['Kconfig', 'Makefile']
        self.re_temp1_model = r'model/[a-z _ 0-9]+'
        self.re_temp2_obj = r"^obj-"
        self.re_temp3_enable = r"CONFIG_DRIVERS_[A-Z _ 0-9]+"
        self.re_temp4_include = r"^include"
        self.re_temp_prefix0 = r"^../[. /]+"
        self.re_temp_prefix1 = r"\$\(HDF_DIR_PREFIX\)"
        self.re_temp_prefix2 = r"\$\(KHDF_AUDIO_BASE_ROOT_DIR\)"
        self.re_temp_current = r"^\.\/"
        self.te_temp5 = r"^[a-z]"
        if platform.system().lower() == 'windows':
            self.platform = "windows"
        elif platform.system().lower() == 'linux':
            self.platform = "linux"

    def scan_makefile(self):
        model_enable_dict = {}
        for index, lines in enumerate(self.contents):
            temp_list = []
            obj_result = re.search(self.re_temp2_obj, lines.strip())
            if obj_result:
                enable_name_result = re.search(
                    self.re_temp3_enable, lines.strip())
                model_path = lines.split("+=")[-1].strip()
                model_name_result = re.search(
                    self.re_temp1_model, model_path)
                if model_name_result:
                    model_name = enable_name_result.group()
                    if self.platform == "windows":
                        model_path =\
                            os.path.join('\\'.join(self.makefile_path.
                                        split('\\')[:-1]), model_path)
                    elif self.platform == "linux":
                        model_path = \
                            os.path.join('/'.join(
                            self.makefile_path.split('/')[:-1]), model_path)
                    temp_list.append(model_path)
                    if os.path.exists(model_path):
                        if model_enable_dict.get(model_name, None):
                            model_enable_dict[model_name] =\
                                temp_list + model_enable_dict.get(model_name)
                        else:
                            model_enable_dict[model_name] = temp_list
        return model_enable_dict

    def get_config_path(self):
        model_defconfig_list = []
        config_path = hdf_utils.\
            get_config_config_path(root=self.root, kernel=self.kernel)
        for roots, dirs, files in os.walk(config_path):
            for file_name in files:
                if file_name.strip() == "hi3516dv300_small_defconfig":
                    model_defconfig_list.append(os.path.join(roots, file_name))
        return model_defconfig_list

    def enable_scan(self):
        enable_list = []
        judge_enable_dict = self.scan_makefile()
        defconfig_path = self.get_config_path()[0]
        config_enable_lines = hdf_utils.read_file_lines(defconfig_path)
        for enable_name, model_path in judge_enable_dict.items():
            # 先去判断使能文件中是否使能(获取使能配置文件路径)
            if ('%s=y\n' % enable_name) in config_enable_lines:
                if "Makefile" in os.listdir(model_path[0]):
                    if os.path.join(model_path[0],
                                    'Makefile') not in enable_list:
                        enable_list.append(
                            os.path.join(model_path[0], 'Makefile'))
        return enable_list

    def _get_model_file_dict(self):
        model_file_dict = {}
        for model_name in self.scan_makefile():
            model_file_dict[model_name] = []
            path = os.path.join(self.framework_dir, model_name)
            for root_path, dirs, files in os.walk(path):
                for file_name in files:
                    model_file_dict.get(model_name).append(
                        os.path.join(root_path, file_name))
        return model_file_dict

    def _get_model_name(self, model_makefile_path):
        parent_path = "/".join(model_makefile_path.split("/")[:-1])
        config_file_path = []
        for i in os.listdir(parent_path):
            if i in self.driver_configs_list:
                config_file_path.append(os.path.join(parent_path, i))
        model_name = re.search(
            self.re_temp1_model, model_makefile_path).group().split("/")[-1]
        return model_name

    def scan_enable_dict(self, lines, test_dict, names):
        re_temp3 = "CONFIG_[A-Z _ 0-9]+"

        list1_12 = []
        for index, line in enumerate(lines):
            if re.search("^ifeq", line):
                list1_12.append(index)
            elif re.search("^endif", line):
                list1_12.append(index + 1)
        obj_list = self.get_obj(lines)

        if list1_12:
            obj_list.extend(list1_12)
            obj_list.sort()
            obj_list = list(filter(
                lambda x: x >= list1_12[1] or x <= list1_12[0], obj_list))
            if obj_list == list1_12:
                obj_list.clear()

        obj_list = self.split_list(obj_list)
        if list1_12 in obj_list:
            obj_list.remove(list1_12)

        if list1_12:
            names = re.search(re_temp3, lines[list1_12[0]].strip()).group()
            test2_dict = {}
            test_dict[names] = self.scan_enable_dict\
                (lines[list1_12[0] + 1:list1_12[1] - 1], test2_dict, names)
            temp_lines = lines
        else:
            temp_lines = lines
        temp_list00 = []
        for i in obj_list:
            if i[0] == i[1]:
                continue
            temp_result = re.search(re_temp3, temp_lines[i[0]].strip())
            temp_list2 = []
            if temp_result is not None:
                key_name = temp_result.group()
                temp_list00.append(key_name)
            else:
                key_name = names
            for j in temp_lines[i[0]: i[1]]:
                if j.strip().strip("\\").strip().split(".")[-1] == "o":
                    temp_list2.append(j.strip().strip("\\").strip())
            if key_name is None:
                key_name = temp_list00[-1]

            if test_dict.get(key_name, None):
                if isinstance((test_dict[key_name]), list):
                    test_dict[key_name] = test_dict[key_name] + temp_list2
                else:
                    test_dict[key_name] = test_dict[key_name][key_name] + temp_list2
            else:
                test_dict[key_name] = temp_list2

        return test_dict

    def get_obj(self, lines):
        # 查找所有 obj 的开头和结尾
        re_temp2 = "^obj-"
        re_temp1 = "^ccflags"
        status = 0
        list1 = []
        for index, line in enumerate(lines):
            temp_result_obj = re.search(re_temp2, line.strip())
            if temp_result_obj:
                status = 1
                list1.append(index)
            temp_result_flag = re.search(re_temp1, line.strip())
            if temp_result_flag and status == 1:
                status = 2
                list1.append(index)
                break
        #  如果status 为1时说明没有找到结尾,将整个长度的结尾
        if status == 1:
            list1.append(len(lines))
        return list1

    def split_list(self, src_list):
        test_list1 = []
        for i in range(len(src_list) - 1):
            test_list1.append([src_list[i], src_list[i + 1]])
        return test_list1

    def get_model_scan(self):
        enable_model_path_list = self.enable_scan()
        result_dict00 = {}
        for model_makefile_path in enable_model_path_list:
            model_name = self._get_model_name(model_makefile_path)
            model_makefile_lines = hdf_utils.\
                read_file_lines(model_makefile_path)
            enable_dict = {}
            config_enable_lines = hdf_utils.\
                read_file_lines(self.get_config_path()[0])
            result = self.scan_enable_dict(
                model_makefile_lines, enable_dict, names=None)
            name_split_dict, enable_list = \
                self.name_split_func(result, config_enable_lines)
            enable_dict = self.path_replace(
                lines=model_makefile_lines,
                enable_list=enable_list,
                makefile_path=model_makefile_path)
            result = self._prefix_template_replace(
                name_split_dict, enable_dict,
                makefile_path=model_makefile_path)
            if result_dict00.get(model_name, None):
                result_dict00.get(model_name).update(result)
            else:
                result_dict00[model_name] = result
        result_dict00["deconfig"] = self.get_config_path()[0]
        result_dict00["makefile_path"] = self.makefile_path
        result_dict00["hcs_path"] = self.hcs_path
        return json.dumps(result_dict00, indent=4)

    def scann_driver_configs(self, makefile_path):
        driver_configs_list = ['Kconfig', 'Makefile']
        parent_path = "/".join(makefile_path.split("/")[:-1])
        config_file_path = []
        for i in os.listdir(parent_path):
            if i in driver_configs_list:
                if self.platform == "windows":
                    config_file_path.append(
                        os.path.join(parent_path, i).replace("/", "\\"))
                elif self.platform == "linux":
                    config_file_path.append(os.path.join(parent_path, i))
        return config_file_path

    def _prefix_template_replace(self,
                                 name_split_dict, enable_dict,
                                 makefile_path):
        config_file_path = self.scann_driver_configs(makefile_path)
        return_dict = {}
        temp_template = Template(json.dumps(name_split_dict))
        dict000 = json.loads(temp_template.substitute(enable_dict))
        for k0, v0 in dict000.items():
            if isinstance(v0, dict):
                return_dict[k0] = {}
                for k01, v01 in v0.items():
                    drivers_sources = []
                    for i in v01:
                        i = i.replace(".o", ".c")
                        prefix4_field = re.search(self.re_temp_current, i)
                        prefix5_field = re.search(self.te_temp5, i)

                        if prefix4_field or prefix5_field:
                            replace_field = "/".join(
                                makefile_path.replace("\\", '/').
                                    split("/")[:-1]
                            ) + "/"
                            if prefix4_field:
                                i = i.replace(prefix4_field.group(),
                                              replace_field).strip()
                            elif prefix5_field:
                                i = os.path.join(replace_field, i)
                        if os.path.exists(i):
                            drivers_sources.append(i)
                    return_dict.get(k0)[k01] = {
                        "driver_configs": config_file_path,
                        "drivers_sources": drivers_sources,
                    }
            else:
                return_dict[k0] = {}
                drivers_sources = []
                for i in v0:
                    i = i.replace(".o", ".c")
                    prefix5_field = re.search(self.te_temp5, i)
                    prefix4_field = re.search(self.re_temp_current, i)
                    if prefix4_field or prefix5_field:
                        replace_field = "/".join(
                            makefile_path.replace("\\", '/').
                                split("/")[:-1]
                        ) + "/"
                        if prefix4_field:
                            i = i.replace(prefix4_field.group(),
                                          replace_field).strip()
                        elif prefix5_field:
                            i = os.path.join(replace_field, i)
                    if i.find("=") != -1:
                        i = i.split("=")[-1].strip()
                    if os.path.exists(i):
                        drivers_sources.append(i)
                return_dict[k0] = {
                    "driver_configs": config_file_path,
                    "drivers_sources": drivers_sources,
                }
        return return_dict

    def path_replace(self, lines, enable_list, makefile_path):
        enable_dict = {}
        include_list = []
        for line in lines:
            if re.search(self.re_temp4_include, line):
                include_list.append(
                    os.path.join(makefile_path.strip("Makefile"),
                                 line.strip().split("/")[-1]))
        include_lines = []
        for include_file in include_list:
            if os.path.exists(include_file):
                with open(include_file, "r") as f:
                    include_lines = f.readlines()
        if include_lines:
            lines.extend(include_lines)

        for key_enable in list(set(enable_list)):
            re_enable = '^%s' % key_enable
            for line in lines:
                result = re.search(re_enable, line)
                if result:
                    line = line.strip().split("=")[-1].strip()
                    result_replace_field = re.search(
                        self.re_temp_prefix0, line)
                    prefix1_field = re.search(self.re_temp_prefix1, line)
                    prefix2_field = re.search(self.re_temp_prefix2, line)
                    prefix3_field = re.search(self.re_temp_current, line)
                    if result_replace_field or \
                            prefix1_field or \
                            prefix2_field or \
                            prefix3_field:
                        if result_replace_field:
                            enable_dict[key_enable] = \
                                "/".join(
                                    [self.root,
                                        line.replace(result_replace_field.group(),
                                                     "drivers/").strip()])
                        if prefix1_field:
                            enable_dict[key_enable] = \
                                "/".join(
                                    [self.root,
                                     line.replace(prefix1_field.group(),
                                                  "drivers").strip()]
                                )
                        if prefix2_field:
                            enable_dict[key_enable] = \
                                "/".join(
                                    [self.root,
                                    line.strip(prefix2_field.group() + "/").
                                        strip()]
                                )
                        if prefix3_field:
                            replace_field = \
                                "/".join(
                                    makefile_path.replace("\\", '/').
                                        split("/")[:-1]
                                )
                            enable_dict[key_enable] = \
                                line.replace(prefix3_field.group(),
                                             replace_field).strip()
                    else:
                        enable_dict[key_enable] = line.split("=")[-1].strip()
        return enable_dict

    def name_split_func(self, result, config_enable_lines):
        enable_list = []
        name_split_dict = {}
        for enable_key, enable_value in result.items():
            key = "%s=y\n" % enable_key
            if key in config_enable_lines:
                if isinstance(enable_value, dict):
                    # 先判断第一级的使能(没有使能的话直接下一个文件)
                    if enable_key.find("HDF") != -1:
                        child_enable_key = ''.join(
                            ["HDF", enable_key.split("HDF")[-1]]
                        ).lower()
                    else:
                        child_enable_key = enable_key.lower()
                    name_split_dict[child_enable_key] = {}
                    for k1, v1 in enable_value.items():
                        if k1.find("HDF") != -1:
                            grand_enable_key = ''.join(
                                ["HDF", k1.split("HDF")[-1]]
                            ).lower()
                        else:
                            grand_enable_key = k1.lower()
                        # 先判断第二级的使能(没有使能的话直接下一个文件)
                        key1 = "%s=y\n" % k1
                        if key1 in config_enable_lines:
                            name_split_dict.get(child_enable_key)[grand_enable_key] = []
                            for name in v1:
                                # 分为 几种情况
                                if name.find("+=") != -1:
                                    child_enable_list, str1 = \
                                        self.get_name(
                                            str1=name.split("+=")[-1].strip()
                                        )
                                else:
                                    child_enable_list, str1 = \
                                        self.get_name(str1=name.strip())
                                enable_list.extend(child_enable_list)
                                name_split_dict.get(child_enable_key).get(grand_enable_key).append(str1)
                        else:
                            continue
                elif isinstance(enable_value, list):
                    if enable_key.find("HDF") != -1:
                        k2 = ''.join(
                            ["HDF", enable_key.split("HDF")[-1]]
                        ).lower()
                    else:
                        k2 = enable_key.lower()
                    name_split_dict[k2] = []
                    for name in enable_value:
                        if name.find("+=") != -1:
                            child_enable_list, str1 = \
                                self.get_name(
                                    str1=name.split("+=")[-1].strip())
                        else:
                            child_enable_list, str1 = \
                                self.get_name(str1=name.strip())
                        enable_list.extend(child_enable_list)
                        name_split_dict.get(k2).append(str1)
            else:
                continue
        return name_split_dict, enable_list

    def get_name(self, str1):
        temp_re = "[A-Z _ 0-9]+"
        list1 = str1.split("/")
        list2 = []
        for i in list1:
            if i.find('$(') != -1:
                temp_name = re.search(temp_re, i)
                if temp_name:
                    list2.append(temp_name.group())
        dict1 = {"$(": "${", ")": "}"}
        for k, v in dict1.items():
            str1 = str1.replace(k, v)
        return list2, str1
