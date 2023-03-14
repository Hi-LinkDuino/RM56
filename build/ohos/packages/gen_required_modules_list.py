#!/usr/bin/env python
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

import sys
import os
import argparse

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402


def read_install_info(system_installed_info_file, depfiles):
    install_module_dict = {}
    parts_list = _read_file_content(system_installed_info_file)
    if parts_list is None:
        raise Exception(
            "read file '{}' failed.".format(system_installed_info_file))
    for _part_info in parts_list:
        is_source = _part_info.get('is_source')
        if is_source is False:
            continue
        part_name = _part_info.get('part_name')
        part_info_file = _part_info.get('part_info_file')
        depfiles.append(part_info_file)
        module_info_file_list = _read_file_and_get_content(
            part_info_file, 'module_info_file')
        install_module_list = []
        for module_info_file in module_info_file_list:
            module_info = _read_file_content(module_info_file)
            depfiles.append(module_info_file)
            install_enable = module_info.get('install_enable')
            if not install_enable:
                continue
            install_module_list.append(module_info)
        install_module_dict[part_name] = install_module_list
    return install_module_dict


def _read_file_content(input_file):
    if not os.path.exists(input_file):
        raise Exception("file '{}' does not exist.".format(input_file))
    data = read_json_file(input_file)
    if data is None:
        raise Exception("read file '{}' failed.".format(input_file))
    return data


def _read_file_and_get_content(input_file, get_arrt_name=None):
    data = _read_file_content(input_file)
    result = []
    for info in data:
        result.append(info.get(get_arrt_name))
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--system-installed-info-file', required=True)
    parser.add_argument('--required-install-modules-file', required=True)
    parser.add_argument('--depfile', required=True)
    args = parser.parse_args()

    depfiles = [args.system_installed_info_file]
    install_moudle_dict = read_install_info(args.system_installed_info_file,
                                            depfiles)
    write_json_file(args.required_install_modules_file, install_moudle_dict)
    build_utils.write_depfile(args.depfile, args.required_install_modules_file,
                              depfiles)
    return 0


if __name__ == '__main__':
    sys.exit(main())
