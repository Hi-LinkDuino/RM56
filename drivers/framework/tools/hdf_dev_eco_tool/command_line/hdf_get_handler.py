#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import os
import json

import hdf_utils
import hdf_tool_version
from command_line.hdf_command_error_code import CommandErrorCode
from hdf_tool_settings import HdfToolSettings
from hdf_tool_exception import HdfToolException
from .hdf_command_handler_base import HdfCommandHandlerBase
from .hdf_linux_scann import HdfLinuxScan
from .hdf_lite_mk_file import HdfLiteMkFile
from .hdf_liteos_scann import HdfLiteScan
from .hdf_vendor_kconfig_file import HdfVendorKconfigFile
from .hdf_module_kconfig_file import HdfModuleKconfigFile
from .hdf_driver_config_file import HdfDriverConfigFile


class HdfGetHandler(HdfCommandHandlerBase):
    HDF_VERSION_MAJOR = 0
    HDF_VERSION_MINOR = 1

    def __init__(self, args):
        super(HdfGetHandler, self).__init__()
        self.cmd = 'get'
        self.handlers = {
            'vendor_list': self._get_vendor_list_handler,
            'current_vendor': self._get_current_vendor_handler,
            'vendor_parent_path': self._get_vendor_parent_path_handler,
            'individual_vendor_path': self._get_individual_vendor_path_handler,
            'board_list': self._get_board_list_handler,
            'driver_list': self._get_driver_list_handler,
            'driver_file': self._get_driver_file_handler,
            'drv_config_file': self._get_drv_config_file_handler,
            'hdf_tool_core_version': self._get_version_handler,
            'model_list': self._get_model_dict,
            'model_scan': self._mode_scan,
            'version': self.__get_version,
        }
        self.parser.add_argument("--action_type",
                                 help=' '.join(self.handlers.keys()),
                                 required=True)
        self.parser.add_argument("--root_dir")
        self.parser.add_argument("--vendor_name")
        self.parser.add_argument("--module_name")
        self.parser.add_argument("--driver_name")
        self.parser.add_argument("--board_name")
        self.args = self.parser.parse_args(args)

    def _get_vendor_list_handler(self):
        self.check_arg_raise_if_not_exist("root_dir")
        root = self.args.root_dir
        vendor_root_dir = hdf_utils.get_vendor_root_dir(root)
        vendors = []
        if os.path.exists(vendor_root_dir):
            for vendor in os.listdir(vendor_root_dir):
                hdf = hdf_utils.get_vendor_hdf_dir(root, vendor)
                if os.path.exists(hdf):
                    vendors.append(vendor)
        return ','.join(vendors)

    def _get_current_vendor_handler(self):
        self.check_arg_raise_if_not_exist("root_dir")
        return HdfLiteMkFile(self.args.root_dir).get_current_vendor()

    @staticmethod
    def _get_board_list_handler():
        settings = HdfToolSettings()
        return settings.get_supported_boards()

    def _get_vendor_parent_path_handler(self):
        self.check_arg_raise_if_not_exist("root_dir")
        target = hdf_utils.get_vendor_root_dir(self.args.root_dir)
        return os.path.realpath(target)

    def _get_individual_vendor_path_handler(self):
        self.check_arg_raise_if_not_exist("root_dir")
        self.check_arg_raise_if_not_exist("vendor_name")
        root, vendor, _, _, _ = self.get_args()
        target = hdf_utils.get_vendor_dir(root, vendor)
        return os.path.realpath(target)

    @staticmethod
    def _get_version_handler():
        return hdf_tool_version.get_version()

    def _get_driver_list_handler(self):
        self.check_arg_raise_if_not_exist("root_dir")
        self.check_arg_raise_if_not_exist("vendor_name")
        root, vendor, _, _, _ = self.get_args()
        hdf_dir = hdf_utils.get_vendor_hdf_dir(root, vendor)
        if not os.path.exists(hdf_dir):
            raise HdfToolException('vendor "%s" not exist' % vendor,
                                   CommandErrorCode.TARGET_NOT_EXIST)
        modules = os.listdir(hdf_dir)
        vendor_k = HdfVendorKconfigFile(root, vendor, kernel=None, path='')
        module_items = vendor_k.get_module_and_config_path()
        drivers = {}
        for item in module_items:
            module, k_path = item
            if module in modules:
                models = \
                    HdfModuleKconfigFile(root, module,
                                         k_path).get_models()
                drivers[module] = models
        return json.dumps(drivers)

    def _get_driver_file_handler(self):
        self.check_arg_raise_if_not_exist("root_dir")
        self.check_arg_raise_if_not_exist("vendor_name")
        self.check_arg_raise_if_not_exist("module_name")
        self.check_arg_raise_if_not_exist("driver_name")
        root = os.path.realpath(self.args.root_dir)
        _, vendor, module, driver, _ = self.get_args()
        drv_dir = hdf_utils.get_drv_dir(root, vendor, module, driver)
        if not os.path.exists(drv_dir):
            raise HdfToolException(
                'driver directory: %s not exist' %
                drv_dir, CommandErrorCode.TARGET_NOT_EXIST)
        for root_path, dirs, files in os.walk(drv_dir):
            for file_name in files:
                if file_name.endswith('.c'):
                    return os.path.realpath(os.path.join(root_path, file_name))
        return ''

    def _get_drv_config_file_handler(self):
        self.check_arg_raise_if_not_exist("root_dir")
        self.check_arg_raise_if_not_exist("module_name")
        self.check_arg_raise_if_not_exist("driver_name")
        self.check_arg_raise_if_not_exist("board_name")
        root, _, module, driver, board = self.get_args()
        drv_config = HdfDriverConfigFile(root, board, module, driver, True)
        return drv_config.get_drv_config_path()

    def _get_model_dict(self):
        self.check_arg_raise_if_not_exist("root_dir")
        root, _, _, _, _, _ = self.get_args()
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
        out_model_list = []
        data = hdf_utils.read_file(create_file_save_path)
        json_type = json.loads(data)
        if not json_type:
            return out_model_list
        file_key_list = list(list(json_type.
                                  items())[0][-1].keys())[:-1]
        for k, _ in json_type.items():
            model_file_path = {}
            for key in file_key_list:
                if key.split("_")[-1] == "path":
                    path_dict = json_type[k][key]
                    model_file_path = self._model_info(
                        path_dict, root, model_file_path, key)
            out_model_list.append({k: model_file_path})
        return out_model_list

    def __get_version(self):
        version_end = "\nCopyright (c) 2020-2021 Huawei Device Co., Ltd."
        version_head = "hdf_dev_eco_tool version : "
        return version_head + str(self.HDF_VERSION_MAJOR) \
               + "." + str(self.HDF_VERSION_MINOR) + version_end

    def _model_info(self, path_dict, root, model_file_path, key):
        if isinstance(path_dict, dict):
            for k_filename, file_path in path_dict.items():
                if not os.path.exists(os.path.join(root, file_path)):
                    model_file_path[k_filename] = " "
                else:
                    model_file_path[k_filename] = path_dict[k_filename]
        else:
            hcs_file_path = os.path.join(root, path_dict)
            if not os.path.exists(hcs_file_path):
                model_file_path[key] = " "
            else:
                model_file_path[key] = path_dict
        return model_file_path

    def _mode_scan(self):
        self.check_arg_raise_if_not_exist("root_dir")
        self.check_arg_raise_if_not_exist("vendor_name")
        self.check_arg_raise_if_not_exist("board_name")
        root, vendor, _, _, board, _ = self.get_args()
        if board.split("_")[-1] != "linux":
            return HdfLiteScan(
                root=root, vendor=vendor, board=board).get_model_scan()
        else:
            return HdfLinuxScan(
                root=root, vendor=vendor, board=board).get_model_scan()