#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import os
import re
from string import Template

import hdf_utils
from hdf_tool_exception import HdfToolException
from .hdf_liteos_scann import HdfLiteScan
from .hdf_command_error_code import CommandErrorCode


class EnableOperation(object):
    def __init__(self, root, vendor, board, model):
        self.root = root
        self.vendor = vendor
        self.board = board
        self.model = model
        if self.board.endswith("linux") or self.board.endswith("Hi3516DV300"):
            pass
        else:
            temp_liteos_model_name = HdfLiteScan(
                self.root, self.vendor, self.board).scan_build()
            self.liteos_model_name = list(map(lambda x: x.strip(",").strip('"'),
                                              temp_liteos_model_name))

        self.makefile_path = hdf_utils.get_vendor_makefile_path(
            root, kernel="linux")
        if not os.path.exists(self.makefile_path):
            raise HdfToolException(
                'Makefile: %s not exist' % self.makefile_path,
                CommandErrorCode.TARGET_NOT_EXIST)

        self.contents_makefile = hdf_utils.read_file_lines(self.makefile_path)
        self.re_temp2 = r'model/[a-z 0-9 _ ]+'
        self.re_temp = r"^group"

    def scan_makefile(self):
        model_list = []
        for i in self.contents_makefile:
            result = re.search(self.re_temp2, i)
            if result:
                model_name = result.group().split('/')[-1]
                if model_name not in model_list:
                    model_list.append(model_name)
        return list(set(model_list))

    def scan_build(self):
        start_index = 0
        end_index = 0
        state = 0
        for index, line in enumerate(self.contents_build):
            if re.compile(self.re_temp).match(line):
                start_index = index
                state += 1
            elif line.strip() == "{" and start_index > 0:
                state += 1
            elif line.strip() == "}" and start_index > 0:
                state -= 1
                if state == 0:
                    end_index = index + 1

        model_list = []
        for i in self.contents[start_index: end_index]:
            model_name = re.compile(self.re_temp2).match(i.strip())
            if model_name:
                model_list.append(model_name.group().split('/')[-1])
        return list(set(model_list))

    def disable_model_liteos(self):
        dot_file_list = hdf_utils.get_dot_configs_path(
            self.root, self.vendor, self.board)
        old_template_string = \
            "LOSCFG_DRIVERS_HDF_${module_upper_case}=y"
        new_template_string = \
            "LOSCFG_DRIVERS_HDF_${module_upper_case} is not set\n"
        new_demo_config = Template(new_template_string).substitute(
            {"module_upper_case": self.model.upper()})
        old_demo_config = Template(old_template_string).substitute(
            {"module_upper_case": self.model.upper()})

        if self.model not in self.liteos_model_name:
            return False
        for dot_file in dot_file_list:
            file_lines = hdf_utils.read_file_lines(dot_file)
            for index, line in enumerate(file_lines):
                if old_demo_config == line.strip():
                    file_lines[index] = new_demo_config
            hdf_utils.write_file_lines(dot_file, file_lines)
        return True

    def enable_model_liteos(self):
        dot_file_list = hdf_utils.get_dot_configs_path(
            self.root, self.vendor, self.board)
        new_template_string = \
            "LOSCFG_DRIVERS_HDF_${module_upper_case}=y\n"
        old_template_string = \
            "LOSCFG_DRIVERS_HDF_${module_upper_case} is not set"
        new_demo_config = Template(new_template_string).substitute(
            {"module_upper_case": self.model.upper()})
        old_demo_config = Template(old_template_string).substitute(
            {"module_upper_case": self.model.upper()})

        if self.model not in self.liteos_model_name:
            return False
        for dot_file in dot_file_list:
            file_lines = hdf_utils.read_file_lines(dot_file)
            for index, line in enumerate(file_lines):
                if old_demo_config == line.strip():
                    file_lines[index] = new_demo_config
            hdf_utils.write_file_lines(dot_file, file_lines)
        return True

    def operation_enable(self):
        if self.board.endswith("Hi3516DV300"):
            try:
                if self.enable_model_linux():
                    return "success(linux) enable %s" % self.model
                else:
                    return "%s model_name is not linux_l2 type" % self.model
            except Exception:
                raise "failure(linux) enable %s" % self.model

        elif self.board.endswith("hispark_taurus_linux"):
            try:
                if self.enable_model_linux():
                    return "success(linux) enable %s" % self.model
                else:
                    return "%s model_name is not linux type" % self.model
            except Exception:
                raise "failure(linux) enable %s" % self.model

        elif self.board.endswith("hispark_taurus"):
            try:
                if self.enable_model_liteos():
                    return "success(liteos) enable %s" % self.model
                else:
                    return "%s model_name is not liteos type" % self.model
            except Exception:
                raise "failure(liteos) enable %s" % self.model

        else:
            return "this board name : (%s) is not support" % self.board

    def operation_disable(self):
        if self.board.endswith("Hi3516DV300"):
            try:
                if self.disable_model_linux():
                    return "success(linux) disable %s" % self.model
                else:
                    return "%s model_name is not linux_l2 type" % self.model
            except Exception:
                raise "failure(linux) disable %s" % self.model

        elif self.board.endswith("hispark_taurus_linux"):
            try:
                if self.disable_model_linux():
                    return "success(linux) disable %s" % self.model
                else:
                    return "%s model_name is not linux type" % self.model
            except Exception:
                raise "failure(linux) disable %s" % self.model

        elif self.board.endswith("hispark_taurus"):
            try:
                if self.disable_model_liteos():
                    return "success(liteos) disable %s" % self.model
                else:
                    return "%s model_name is not liteos type" % self.model
            except Exception:
                raise "failure(liteos) disable %s" % self.model

        else:
            return "this board name : (%s) is not support " % self.board

    def get_config_config(self, kernel):
        return os.path.join(self.root, "kernel", kernel, "config")

    def get_config_patch(self, kernel):
        return os.path.join(self.root, "kernel", kernel, "patches")

    def _get_file_patch(self, patch, endswitch, split_sign):
        file_path = []
        for roots, dirs, files in os.walk(patch):
            if endswitch == "defconfig":
                files_list = list(filter(
                    lambda x: x.split(split_sign)[-1] == endswitch, files))
            else:
                files_list = list(filter(lambda x: x == endswitch, files))
            for file_name in files_list:
                file_path.append(os.path.join(roots, file_name))
        return file_path

    def _get_config_linux(self):
        config_path = self.get_config_config(kernel="linux")
        config_path_list = self._get_file_patch(
            patch=config_path, endswitch="defconfig", split_sign="_")
        
        patch_path = self.get_config_patch(kernel="linux")
        patch_path_list = self._get_file_patch(
            patch=patch_path, endswitch="hi3516dv300.patch", split_sign=".")
        config_path_list.extend(patch_path_list)

        return config_path_list

    def _replace_operation(self, new_string, old_string, file_path):
        new_demo_config = Template(new_string).substitute(
            {"module_upper_case": self.model.upper()})
        old_demo_config = Template(old_string).substitute(
            {"module_upper_case": self.model.upper()})

        with open(file_path, 'rb') as f_read:
            file_lines = f_read.readlines()
        for index, line in enumerate(file_lines):
            if old_demo_config.encode('utf-8') == line.strip():
                file_lines[index] = new_demo_config.encode('utf-8')
        
        with open(file_path, 'wb') as f_write:
            f_write.writelines(file_lines)
        return True

    def disable_model_linux(self):
        if self.model not in self.scan_makefile():
            return False
        file_path_list = self._get_config_linux()
        for file_path in file_path_list:
            if file_path.split("_")[-1] == "defconfig":
                old_template_string = \
                    "CONFIG_DRIVERS_HDF_${module_upper_case}=y"
                new_template_string = \
                    "CONFIG_DRIVERS_HDF_${module_upper_case} is not set\n"
            else:
                old_template_string = \
                    "+CONFIG_DRIVERS_HDF_${module_upper_case}=y"
                new_template_string = \
                    "+CONFIG_DRIVERS_HDF_${module_upper_case} is not set\n"
            self._replace_operation(new_string=new_template_string,
                                    old_string=old_template_string,
                                    file_path=file_path)
        return True

    def enable_model_linux(self):
        if self.model not in self.scan_makefile():
            return False
        file_path_list = self._get_config_linux()
        for file_path in file_path_list:
            if file_path.split("_")[-1] == "defconfig":
                new_template_string \
                    = "CONFIG_DRIVERS_HDF_${module_upper_case}=y\n"
                old_template_string \
                    = "CONFIG_DRIVERS_HDF_${module_upper_case} is not set"
            else:
                new_template_string \
                    = "+CONFIG_DRIVERS_HDF_${module_upper_case}=y\n"
                old_template_string = \
                    "+CONFIG_DRIVERS_HDF_${module_upper_case} is not set"
            self._replace_operation(new_string=new_template_string,
                                    old_string=old_template_string,
                                    file_path=file_path)
        return True
