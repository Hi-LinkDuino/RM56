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
from .hdf_command_error_code import CommandErrorCode


class HdfVendorMakeFile(object):
    def __init__(self, root, vendor, kernel, path):
        if path:
            self.file_path = path
        else:
            self.vendor = vendor
            self.kernel = kernel
            self.file_path = hdf_utils.\
                get_vendor_makefile_path(root, self.kernel)
        if not os.path.exists(self.file_path):
            raise HdfToolException('file: %s not exist' % self.file_path,
                                   CommandErrorCode.TARGET_NOT_EXIST)
        self.contents = hdf_utils.read_file_lines(self.file_path)
        self.template_str = \
            "obj-$(CONFIG_DRIVERS_HDF_${module_upper_case}) " \
            "+= model/${module_lower_case}/\n"

    def _begin_end(self, module):
        module_id = hdf_utils.get_id(self.vendor, module)
        begin = '\n# <begin %s\n' % module_id
        end = '\n# %s end>\n' % module_id
        return begin, end

    def add_module(self, data_model):
        self.contents.append(Template(self.template_str)
                             .safe_substitute(data_model))
        hdf_utils.write_file_lines(self.file_path, self.contents)
        return self.file_path

    def delete_module(self, module):
        module_converter = hdf_utils.WordsConverter(module)
        data_model = {
            'module_upper_case': module_converter.upper_case(),
            'module_lower_case': module_converter.lower_case(),
        }
        delete_template = Template(
            self.template_str).safe_substitute(data_model)
        if delete_template in self.contents:
            self.contents.remove(delete_template)
            hdf_utils.write_file_lines(
                content=self.contents, file_path=self.file_path)

    def rename_vendor(self):
        pattern = r'vendor/([a-zA-Z0-9_\-]+)/'
        replacement = 'vendor/%s/' % self.vendor
        new_content = re.sub(pattern, replacement, self.contents)
        hdf_utils.write_file(self.file_path, new_content)
