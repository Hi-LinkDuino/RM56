#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2022 Huawei Device Co., Ltd.
#
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.

import re
from string import Template

import hdf_utils


def replace_func(info_str):
    replace_dict = {'(': "{", ')': "}"}
    for key, values in replace_dict.items():
        info_str = info_str.replace(key, values)
    return info_str


def linux_makefile_operation(path, driver_file_path):
    lines = hdf_utils.read_file_lines(path)

    makefile_info = {}

    for index, line in enumerate(lines):
        if line.find('.o') != -1:
            info_value = ('/'.join(line.split("=")[-1].split('/')[:-1])).strip()

            if info_value not in list(makefile_info.values()):
                makefile_info[index] = info_value

    file_path = '/'.join(driver_file_path.split('\\'))

    for key, info in makefile_info.items():
        model_name = re.search(r"[A-Z_]+", info).group()
        for line in lines:
            if re.search(model_name, line):
                temp = Template(replace_func(info))
                temp_path = temp.safe_substitute({model_name: line.strip().split('=')[-1].strip()})
                lines_insert(file_path, temp_path, lines, key)
                break
    hdf_utils.write_file_lines(path, lines)


def lines_insert(file_path, temp_path, lines, key):
    insert_temp = ""
    if file_path.find(temp_path.split('model')[-1]) != -1 and insert_temp == "":
        insert_temp = lines[key]
        if insert_temp.strip().endswith("\\"):
            lines.insert(key + 1, re.sub(r"[a-zA-Z0-9_]+\.o", file_path.split('/')[-1].replace('.c', '.o'),
                                         insert_temp))
        else:
            test = (file_path.split('/')[-1] + r" \\")
            lines.insert(key, re.sub(r"[a-zA-Z0-9_]+\.o", test.replace('.c', '.o'), insert_temp))