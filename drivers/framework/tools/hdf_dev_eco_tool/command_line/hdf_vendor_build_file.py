#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import os
import re

import hdf_utils
from hdf_tool_exception import HdfToolException
from .hdf_command_error_code import CommandErrorCode


class HdfVendorBuildFile(object):
    def __init__(self, root, vendor):
        self.vendor = vendor
        self.file_path = hdf_utils.get_vendor_gn_path(root)
        if not os.path.exists(self.file_path):
            raise HdfToolException('file: %s not exist' % self.file_path,
                                   CommandErrorCode.TARGET_NOT_EXIST)
        self.contents = hdf_utils.read_file(self.file_path)

    def add_module(self, module):
        with open(self.file_path, 'r') as file_read:
            data = file_read.readlines()

        line_template = r'input'
        new_line = {}
        for index, line in enumerate(data):
            result = re.search(line_template, line)
            if result:
                new_line["index"] = index + 1
                new_line["value"] = line
        new_line["value"] = new_line.get("value").replace("input", module)
        data.insert(new_line.get("index"), new_line.get("value"))
        with open(self.file_path, 'w') as write_file:
            for i in data:
                write_file.write(i)
        return self.file_path

    def delete_module(self, file_path, model):
        lines = hdf_utils.read_file_lines(file_path)
        for index, line in enumerate(lines):
            if line.find(model) > 0:
                del lines[index]
        hdf_utils.write_file_lines(file_path, lines)

    def rename_vendor(self):
        pattern = r'vendor/([a-zA-Z0-9_\-]+)/'
        replacement = 'vendor/%s/' % self.vendor
        new_content = re.sub(pattern, replacement, self.contents)
        hdf_utils.write_file(self.file_path, new_content)
