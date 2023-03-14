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
import argparse
import os
import shutil

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402


def get_src_parts(system_install_info_file):
    parts_info_list = read_json_file(system_install_info_file)
    if parts_info_list is None:
        raise Exception(
            "read file '{}' failed.".format(system_install_info_file))
    src_parts_info = {}
    for part_info in parts_info_list:
        if 'is_source' not in part_info:
            raise Exception("read part info error, missing key is_source.")
        if part_info.get('is_source') is True:
            part_name = part_info.get('part_name')
            src_parts_info[part_name] = part_info
    return src_parts_info


def _collect_testcase(resources_dir, parts_info, output_base_dir):
    for part_name, part_info in parts_info.items():
        origin_part_name = part_info.get('origin_part_name')
        if part_info.get('variant_name') != 'phone':
            _toolchain = part_info.get('toolchain_label')
            _toolchain_name = _toolchain.split(':')[1]
            _variant_dir = os.path.join(_toolchain_name, resources_dir,
                                        origin_part_name)
        else:
            _variant_dir = os.path.join(resources_dir, part_name)
        if not os.path.exists(_variant_dir):
            continue

        _src_dir_name = os.path.basename(resources_dir)
        package_output_dir = os.path.join(output_base_dir, _src_dir_name)
        dest_dir = os.path.join(package_output_dir, origin_part_name)
        if os.path.exists(dest_dir):
            shutil.rmtree(dest_dir)
        shutil.copytree(_variant_dir, dest_dir)


def collect_testcase(resources_dir_list, parts_info, output_base_dir):
    for resources_dir in resources_dir_list:
        _collect_testcase(resources_dir, parts_info, output_base_dir)


def _collect_test_mlf(resources_dir, parts_info, output_base_dir):
    for part_name, part_info in parts_info.items():
        origin_part_name = part_info.get('origin_part_name')
        if part_info.get('variant_name') != 'phone':
            _toolchain = part_info.get('toolchain_label')
            _toolchain_name = _toolchain.split(':')[1]
            _variant_dir = os.path.join(_toolchain_name, resources_dir,
                                        origin_part_name)
        else:
            _variant_dir = os.path.join(resources_dir, part_name)
        if not os.path.exists(_variant_dir):
            continue

        _src_dir_name = os.path.basename(resources_dir)
        package_output_dir = os.path.join(output_base_dir, _src_dir_name)
        dest_dir = os.path.join(package_output_dir, origin_part_name)
        if os.path.exists(dest_dir):
            shutil.rmtree(dest_dir)
        shutil.copytree(_variant_dir, dest_dir)


def collect_test_mlf(resources_dir_list, parts_info, output_base_dir):
    for resources_dir in resources_dir_list:
        _collect_test_mlf(resources_dir, parts_info, output_base_dir)


def _collect_libs_symbols(resources_dir, parts_info, package_output_base_dir):
    for part_name, part_info in parts_info.items():
        origin_part_name = part_info.get('origin_part_name')
        if part_info.get('variant_name') != 'phone':
            _toolchain = part_info.get('toolchain_label')
            _toolchain_name = _toolchain.split(':')[1]
            _variant_dir = os.path.join(_toolchain_name, resources_dir,
                                        _toolchain_name,
                                        part_info.get('subsystem_name'),
                                        origin_part_name)
        else:
            _variant_dir = os.path.join(resources_dir,
                                        part_info.get('subsystem_name'),
                                        part_name)
        if not os.path.exists(_variant_dir):
            continue

        _src_dir_name = os.path.basename(resources_dir)
        package_output_dir = os.path.join(package_output_base_dir,
                                          _src_dir_name)
        dest_dir = os.path.join(package_output_dir, origin_part_name)
        if os.path.exists(dest_dir):
            shutil.rmtree(dest_dir)
        shutil.copytree(_variant_dir, dest_dir)


def collect_libs_symbols(resources_dir_list, parts_info, output_base_dir):
    for resources_dir in resources_dir_list:
        _collect_libs_symbols(resources_dir, parts_info, output_base_dir)


def _collect_libs_java(resources_dir, parts_info, output_base_dir):
    parts_libs_java_path = {}
    for part_name, part_info in parts_info.items():
        origin_part_name = part_info.get('origin_part_name')
        if part_info.get('variant_name') != 'phone':
            _toolchain = part_info.get('toolchain_label')
            _toolchain_name = _toolchain.split(':')[1]
            _variant_dir = os.path.join(_toolchain_name, resources_dir,
                                        part_info.get('subsystem_name'),
                                        origin_part_name)
        else:
            _variant_dir = os.path.join(resources_dir,
                                        part_info.get('subsystem_name'),
                                        part_name)
        if not os.path.exists(_variant_dir):
            continue

        parts_libs_java_path[origin_part_name] = _variant_dir
    _libs_java_path_info_file = os.path.join(output_base_dir,
                                             'libs_java_info.json')
    write_json_file(_libs_java_path_info_file, parts_libs_java_path)


def collect_libs_java(resources_dir_list, parts_info, output_base_dir):
    for resources_dir in resources_dir_list:
        _collect_libs_java(resources_dir, parts_info, output_base_dir)


def _collect_kits_modules(resources_dir, parts_info, output_base_dir):
    parts_modules_path = {}
    for part_name, part_info in parts_info.items():
        part_modules_dir = os.path.join(resources_dir, part_name)
        if not os.path.exists(part_modules_dir):
            continue
        origin_part_name = part_info.get('origin_part_name')
        parts_modules_path[origin_part_name] = part_modules_dir
    _modules_path_info_file = os.path.join(output_base_dir,
                                           'kits_modules_info.json')
    write_json_file(_modules_path_info_file, parts_modules_path)


def collect_kits_modules(resources_dir_list, parts_info, output_base_dir):
    for resources_dir in resources_dir_list:
        _collect_kits_modules(resources_dir, parts_info, output_base_dir)


def collect_innerkits_modules(resources_dir_list, parts_info, output_base_dir):
    for resources_dir in resources_dir_list:
        _collect_kits_modules(resources_dir, parts_info, output_base_dir)


def collect_resources(collect_type, resources_dir_list, parts_info,
                      output_base_dir):
    if collect_type == 'libs_symbols':
        collect_libs_symbols(resources_dir_list, parts_info, output_base_dir)
    elif collect_type == 'module_list_files':
        collect_test_mlf(resources_dir_list, parts_info, output_base_dir)
    elif collect_type == 'testcase':
        collect_testcase(resources_dir_list, parts_info, output_base_dir)
    elif collect_type == 'kits_modules':
        collect_kits_modules(resources_dir_list, parts_info, output_base_dir)
    elif collect_type == 'innerkits_modules':
        collect_innerkits_modules(resources_dir_list, parts_info,
                                  output_base_dir)
    elif collect_type == 'libs_java':
        collect_libs_java(resources_dir_list, parts_info, output_base_dir)
    else:
        raise Exception("Unsupported resource type.")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--collect-type', required=True)
    parser.add_argument('--system-install-info-file', required=True)
    parser.add_argument('--resources-dir-list', nargs='+', default=[])
    parser.add_argument('--package-output-base-dir', required=True)
    parser.add_argument('--output-file', required=True)
    args = parser.parse_args()

    parts_info = get_src_parts(args.system_install_info_file)

    collect_resources(args.collect_type, args.resources_dir_list, parts_info,
                      args.package_output_base_dir)
    write_json_file(args.output_file, '{}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
