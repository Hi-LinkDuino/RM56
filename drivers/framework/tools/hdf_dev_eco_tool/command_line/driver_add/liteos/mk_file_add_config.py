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


def makefile_operation(mk_path, driver_file_path):
    lines = hdf_utils.read_file_lines(mk_path)
    makefile_info = {}

    status = False
    for index, line in enumerate(lines):
        if line.find('LOCAL_SRCS') != -1 and status == False:
            status = True
        if line.find("endif") != -1:
            status = False
        elif status:
            info_value = ('/'.join(line.split("=")[-1].split('/')[:-1])).strip()
            if info_value not in list(makefile_info.values()):
                makefile_info[index] = info_value

    file_path = '/'.join(driver_file_path.split('\\'))
    for key, info in makefile_info.items():
        if re.search(r"[A-Z_]+", info) is not None:
            model_name = re.search(r"[A-Z_]+", info).group()
            for line in lines:
                if re.search(model_name, line):
                    temp = {model_name: line.strip().split('=')[-1].strip()}
                    temp_path = Template(replace_func(info)).safe_substitute(temp)
                    lines_insert(file_path, temp_path, lines, key)

    hdf_utils.write_file_lines(mk_path, lines)


def lines_insert(file_path, temp_path, lines, key):
    insert_temp = ""
    if file_path.find(temp_path.split('model')[-1]) != -1 and insert_temp == "":
        insert_temp = lines[key]
        if insert_temp.strip().endswith("\\"):
            replace_temp = re.sub(r"[a-zA-Z0-9_]+\.c", file_path.split('/')[-1], insert_temp)
            if replace_temp.startswith("LOCAL_SRCS"):
                insert_temp = ((len(replace_temp.split("=")[0]) + 1) * " ") + replace_temp.split("=")[-1]
                lines.insert(key + 1, insert_temp)
            else:
                lines.insert(key + 1, replace_temp)
        else:
            test = (file_path.split('/')[-1] + r" \\")
            lines.insert(key, re.sub(r"[a-zA-Z0-9_]+.c$", test, insert_temp))