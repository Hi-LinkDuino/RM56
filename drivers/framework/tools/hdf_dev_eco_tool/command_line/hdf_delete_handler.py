#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import json
import os
import shutil
from string import Template

import hdf_utils
from hdf_tool_exception import HdfToolException
from .hdf_command_error_code import CommandErrorCode
from .hdf_command_handler_base import HdfCommandHandlerBase
from .hdf_defconfig_patch import HdfDefconfigAndPatch
from .hdf_device_info_hcs import HdfDeviceInfoHcsFile
from .hdf_vendor_build_file import HdfVendorBuildFile
from .hdf_vendor_kconfig_file import HdfVendorKconfigFile
from .hdf_vendor_makefile import HdfVendorMakeFile
from .hdf_vendor_mk_file import HdfVendorMkFile
from .hdf_module_kconfig_file import HdfModuleKconfigFile
from .hdf_module_mk_file import HdfModuleMkFile
from .hdf_driver_config_file import HdfDriverConfigFile


class HdfDeleteHandler(HdfCommandHandlerBase):
    def __init__(self, args):
        super(HdfDeleteHandler, self).__init__()
        self.cmd = 'delete'
        self.handlers = {
            'vendor': self._delete_vendor_handler,
            'module': self._delete_module_handler,
            'driver': self._delete_driver_handler,
        }
        self.parser.add_argument("--action_type",
                                 help=' '.join(self.handlers.keys()),
                                 required=True)
        self.parser.add_argument("--root_dir", required=True)
        self.parser.add_argument("--vendor_name")
        self.parser.add_argument("--module_name")
        self.parser.add_argument("--driver_name")
        self.parser.add_argument("--board_name")
        self.parser.add_argument("--kernel_name")
        self.args = self.parser.parse_args(args)

    def _delete_vendor_handler(self):
        self.check_arg_raise_if_not_exist("vendor_name")
        self.check_arg_raise_if_not_exist("board_name")
        root, vendor, _, _, _ = self.get_args()
        vendor_hdf_dir = hdf_utils.get_vendor_hdf_dir(root, vendor)
        if not os.path.exists(vendor_hdf_dir):
            return
        for module in os.listdir(vendor_hdf_dir):
            mod_dir = os.path.join(vendor_hdf_dir, module)
            if os.path.isdir(mod_dir):
                pass
        shutil.rmtree(vendor_hdf_dir)

    def _delete_module(self, root, model, model_info):
        for key, path_value in model_info.items():
            if key.split("_")[-1] == "name":
                pass
            elif key == "driver_file_path":
                driver_file = os.path.join(
                    root, path_value.split(model)[0], model)
                if os.path.exists(driver_file):
                    shutil.rmtree(driver_file)
            else:
                self._delete_file_func(root, key, model_info, model)
        create_model_data = self._delete_model_info()
        delete_model_info = hdf_utils.get_create_model_info(
            root=root, create_data=json.dumps(create_model_data))
        return delete_model_info

    def _delete_model_info(self):
        self.check_arg_raise_if_not_exist("root_dir")
        self.check_arg_raise_if_not_exist("module_name")
        root, _, module, _, _, _ = self.get_args()
        adapter_framework = hdf_utils.get_vendor_hdf_dir_framework(root=root)
        if not os.path.exists(adapter_framework):
            raise HdfToolException(
                ' adapter model path  "%s" not exist' %
                adapter_framework, CommandErrorCode.TARGET_NOT_EXIST)
        create_file_save_path = os.path.join(
            adapter_framework, "tools", "hdf_dev_eco_tool",
            "resources", "create_model.config")
        if not os.path.exists(create_file_save_path):
            raise HdfToolException(
                'create file config "%s" not exist' %
                create_file_save_path, CommandErrorCode.TARGET_NOT_EXIST)
        data = hdf_utils.read_file(create_file_save_path)
        write_data = json.loads(data)
        write_data.pop(module)
        hdf_utils.write_file(create_file_save_path,
                             json.dumps(write_data, indent=4))
        return write_data

    def _delete_module_handler(self):
        self.check_arg_raise_if_not_exist("root_dir")
        self.check_arg_raise_if_not_exist("module_name")
        root, _, module, _, _, _ = self.get_args()
        adapter_framework = hdf_utils.get_vendor_hdf_dir_framework(root=root)
        if not os.path.exists(adapter_framework):
            raise HdfToolException(
                ' adapter model path  "%s" not exist' %
                adapter_framework, CommandErrorCode.TARGET_NOT_EXIST)
        create_file_save_path = os.path.join(
            adapter_framework, "tools", "hdf_dev_eco_tool",
            "resources", "create_model.config")
        if not os.path.exists(create_file_save_path):
            raise HdfToolException(
                'create file config "%s" not exist' %
                create_file_save_path, CommandErrorCode.TARGET_NOT_EXIST)
        data = hdf_utils.read_file(create_file_save_path)
        file_info = json.loads(data)
        model_info = file_info.get(module, None)
        if model_info is None:
            raise HdfToolException(
                ' delete model "%s" not exist' %
                module, CommandErrorCode.TARGET_NOT_EXIST)
        else:
            return self._delete_module(root, module, model_info)

    def _delete_driver(self, module, driver):
        root, vendor, _, _, board, kernel = self.get_args()
        drv_dir = hdf_utils.get_drv_dir(root, vendor, module, driver)
        if os.path.exists(drv_dir):
            shutil.rmtree(drv_dir)
        k_path = hdf_utils.get_module_kconfig_path(root, vendor, module)
        HdfModuleKconfigFile(root, module, k_path).delete_driver(driver)
        HdfModuleMkFile(root, vendor, module).delete_driver(driver)
        HdfDriverConfigFile(root, board, module, driver, kernel).delete_driver()

    def _delete_driver_handler(self):
        self.check_arg_raise_if_not_exist("vendor_name")
        self.check_arg_raise_if_not_exist("module_name")
        self.check_arg_raise_if_not_exist("driver_name")
        self.check_arg_raise_if_not_exist("board_name")
        _, _, module, driver, _ = self.get_args()
        self._delete_driver(module, driver)

    def _delete_file_func(self, root, key, model_info, model):
        if key == "module_level_config_path":
            for key1, value1 in model_info[key].items():
                if key1 == "%s_Makefile" % model:
                    HdfVendorMakeFile(
                        root, vendor="", kernel="",
                        path=os.path.join(root, value1)).delete_module(model)
                elif key1 == "%s_Kconfig" % model:
                    HdfVendorKconfigFile(
                        root, vendor="", kernel="",
                        path=os.path.join(root, value1)).delete_module(model)
                elif key1 == "%sBuild" % model:
                    HdfVendorBuildFile(
                        root, vendor="").delete_module(
                        file_path=os.path.join(root, value1), model=model)
                elif key1 == "%s_hdf_lite" % model:
                    HdfVendorMkFile(
                        root, vendor="").delete_module(
                        file_path=os.path.join(root, value1), module=model)
                elif key1 == "%s_dot_configs" % model:
                    for dot_path in value1:
                        if dot_path.split(".")[-1] == "config":
                            template_string = \
                                "LOSCFG_DRIVERS_HDF_${module_upper_case}=y\n"
                        else:
                            template_string = \
                                "CONFIG_DRIVERS_HDF_${module_upper_case}=y\n"
                        new_demo_config = Template(template_string).\
                            substitute({"module_upper_case": model.upper()})
                        defconfig_patch = HdfDefconfigAndPatch(
                            root=root, vendor="", kernel="", board="",
                            data_model="", new_demo_config=new_demo_config)
                        defconfig_patch.delete_module(
                            path=os.path.join(root, dot_path))
        elif key == "module_path":
            for file_name, value2 in model_info[key].items():
                if file_name.startswith("adapter"):
                    file_path = os.path.join(root, value2)
                    file_info = hdf_utils.read_file_lines(file_path)
                    delete_info = "libhdf_%s_hotplug" % model

                    for index, line_info in enumerate(file_info):
                        if line_info.find(delete_info) > 0:
                            file_info.pop(index)
                            hdf_utils.write_file_lines(file_path, file_info)
                            break
                else:
                    if value2.endswith("hcs"):
                        hcs_path = os.path.join(root, value2)
                        HdfDeviceInfoHcsFile(
                            root=root, vendor="", module="",
                            board="", driver="", path=hcs_path). \
                            delete_driver(module=model)
                    else:
                        if value2:
                            file_path = os.path.join(root, value2)
                            if os.path.exists(file_path):
                                os.remove(file_path)
                            model_dir_path = "/".join(file_path.split("/")[:-1])
                            if os.path.exists(model_dir_path):
                                file_list = os.listdir(model_dir_path)
                                if not file_list:
                                    shutil.rmtree(model_dir_path)
