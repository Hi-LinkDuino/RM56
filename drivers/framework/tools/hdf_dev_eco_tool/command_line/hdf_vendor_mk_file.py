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
from hdf_tool_settings import HdfToolSettings
from .hdf_command_error_code import CommandErrorCode


class HdfVendorMkFile(object):
    def __init__(self, root, vendor):
        self.vendor = vendor
        self.file_path = hdf_utils.get_vendor_lite_mk_path(root)
        if not os.path.exists(self.file_path):
            raise HdfToolException('file: %s not exist' % self.file_path,
                                   CommandErrorCode.TARGET_NOT_EXIST)
        self.contents = hdf_utils.read_file(self.file_path)

    def _begin_end(self, module):
        module_id = hdf_utils.get_id(module)
        begin = '\n# <begin %s\n' % module_id
        end = '\n# %s end>\n' % module_id
        return begin, end

    def _create_module(self, module, need_content=True):
        begin, end = self._begin_end(module)
        if not need_content:
            return hdf_utils.SectionContent(begin, '', end)
        vendor_converter = hdf_utils.WordsConverter(self.vendor)
        module_converter = hdf_utils.WordsConverter(module)
        data_model = {
            'module_upper_case': module_converter.upper_case(),
            'module_lower_case': module_converter.lower_case(),
            'vendor_lower_case': vendor_converter.lower_case(),
            'drivers_path': HdfToolSettings().get_drivers_path_adapter()
        }
        template_str = hdf_utils.get_template('hdf_vendor_mk_module.template')
        module_item = Template(template_str).safe_substitute(data_model)
        return hdf_utils.SectionContent(begin, module_item, end)

    def add_module(self, module):
        module_ = self._create_module(module)
        old_range = hdf_utils.find_section(self.contents, module_)
        if old_range:
            hdf_utils.replace_and_save(self.contents, self.file_path,
                                       old_range, module_)
            return
        hdf_utils.append_and_save(self.contents, self.file_path, module_)
        return self.file_path

    def delete_module(self, file_path, module):
        module_content = self._create_module(module, False)
        file_contents = hdf_utils.read_file(file_path)
        old_range = hdf_utils.find_section(file_contents, module_content)
        if not old_range:
            return
        hdf_utils.delete_and_save(file_contents, self.file_path, old_range)

    def rename_vendor(self):
        pattern = r'vendor/([a-zA-Z0-9_\-]+)/'
        replacement = 'vendor/%s/' % self.vendor
        new_content = re.sub(pattern, replacement, self.contents)
        hdf_utils.write_file(self.file_path, new_content)
