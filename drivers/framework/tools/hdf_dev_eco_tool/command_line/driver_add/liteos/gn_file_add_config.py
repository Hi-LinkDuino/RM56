#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2022 Huawei Device Co., Ltd.
#
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.

import ast
import re
import json
from string import Template

import hdf_utils


def build_file_operation(path, driver_file_path):
    lines = hdf_utils.read_file_lines(path)
    test_dict = {}
    status = False
    line_temp = ''
    for index, line in enumerate(lines):
        if line.startswith("#"):
            pass
        elif line.strip().startswith("sources"):
            if line.split('=')[-1].find(']') != -1 and line.find("+=") == -1:
                test_dict[index] = line
        else:
            if line.strip().startswith('FRAMEWORKS'):
                test_dict[line.split('=')[0].strip()] = ast.literal_eval(line.split('=')[-1].strip())
            elif line.find('$FRAMEWORKS') != -1 and \
                    line.find('.c') != -1 and \
                    status is False:
                if line.strip()[1:-2].startswith('$FRAMEWORKS'):
                    line_temp = '/'.join((line.strip()[1:-2]).split('/')[:-1])
                    test_dict[index] = line_temp
                status = True
            elif line.find('$FRAMEWORKS') == -1:
                line_temp = ''
                status = False
            elif line.find('$FRAMEWORKS') != -1 and \
                    line.find('.c') != -1 and line_temp:
                if line.find(line_temp) == -1:
                    line_temp = '/'.join((line.strip()[1:-2]).split('/')[:-1])
                    test_dict[index] = line_temp

    file_path = '/'.join(driver_file_path.split('\\'))

    for i in test_dict.keys():
        if isinstance(i, str):
            pass
        elif isinstance(i, int):
            str1 = Template(test_dict.get(i))
            source_file_path = str1.safe_substitute(test_dict)
            if file_path.find("/".join(source_file_path.split("/")[1:])) != -1:
                lines.insert(i + 1, re.sub(r'[a-zA-Z_0-9]+\.c', file_path.split('/')[-1], lines[i]))
            elif source_file_path.strip().startswith("sources"):
                temp_append_path = re.sub(r'[a-zA-Z_0-9]+\.c', file_path.split('/')[-1],
                                          ast.literal_eval(lines[i].split('=')[-1])[0])
                source_list = ast.literal_eval(lines[i].split('=')[-1])
                source_list.append(temp_append_path)
                result_str = json.dumps(source_list, indent=4)
                temp_13 = (lines[i].split('=')[0] + "= " + result_str)
                temp_13 = temp_13.replace("]", "  ]")
                lines[i] = temp_13

    hdf_utils.write_file_lines(path, lines)
