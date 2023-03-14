#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import re
import glob

from file_utils import read_json_file


def _get_external_deps_file_list(search_path):
    search_str = "{}/**/*{}".format(search_path, "_external_deps_temp.json")
    external_deps_file_list = glob.glob(search_str, recursive=True)
    return external_deps_file_list


def _read_external_deps_info(build_out_dir):
    ext_deps_file_list = _get_external_deps_file_list(
        os.path.join(build_out_dir, 'gen'))
    ext_deps_file_dict = {}
    for _external_deps_file in ext_deps_file_list:
        if not os.path.exists(_external_deps_file):
            raise Exception(
                "file '{}' doesn't exist.".format(_external_deps_file))
        module_ext_deps_info = read_json_file(_external_deps_file)
        if module_ext_deps_info is None:
            raise Exception(
                "read file '{}' failed.".format(_external_deps_file))
        _filename = os.path.basename(_external_deps_file)
        _filename_snippet = re.search(r'(.*)_external_deps_temp.json',
                                      _filename).group(1)
        part_name, module_name = _parse_module_name(_filename_snippet)
        module_alias = '{}:{}'.format(part_name, module_name)
        ext_deps_file_dict[module_alias] = module_ext_deps_info
    return ext_deps_file_dict


def _parse_module_name(name_str):
    split_result = name_str.split('__')
    part_name = split_result[0]
    module_name = split_result[1]
    return part_name, module_name


def _read_module_deps_info(module_deps_files_path):
    deps_files = os.listdir(module_deps_files_path)
    deps_data = {}
    for _filename in deps_files:
        _deps_file = os.path.join(module_deps_files_path, _filename)
        if not os.path.exists(_deps_file):
            raise Exception("file '{}' doesn't exist.".format(_deps_file))
        module_deps_info = read_json_file(_deps_file)
        if module_deps_info is None:
            raise Exception("read file '{}' failed.".format(_deps_file))
        _filename_snippet = re.search(r'(.*).json', _filename).group(1)
        part_name, module_name = _parse_module_name(_filename_snippet)
        module_alias = '{}:{}'.format(part_name, module_name)
        deps_data[module_alias] = module_deps_info
    return deps_data


def _merge_external_deps_label(deps_data, external_deps_data):
    for _module_alias, _info in deps_data.items():
        external_deps = _info.get('external_deps')
        if not external_deps:
            continue
        ext_deps_label_info = external_deps_data.get(_module_alias)
        if not ext_deps_label_info:
            raise Exception(
                "module '{}' external deps info is incorrect.".format(
                    _module_alias))
        _info['external_deps_lable'] = ext_deps_label_info.get('deps')
    return deps_data


def get_all_deps_data(module_deps_files_path):
    deps_data = _read_module_deps_info(module_deps_files_path)
    build_out_dir = os.path.dirname(module_deps_files_path)
    external_deps_data = _read_external_deps_info(build_out_dir)
    all_deps_data = _merge_external_deps_label(deps_data, external_deps_data)
    return all_deps_data
