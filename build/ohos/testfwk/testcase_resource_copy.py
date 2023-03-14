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
import shutil
import xml.etree.ElementTree as ET
sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402


def copy_dir(src, dest):
    if not os.path.exists(src):
        raise Exception("src dir '{}' doesn't exist.".format(src))
    if not os.path.exists(dest):
        os.makedirs(dest, exist_ok=True)
    result_files = []
    src_files = []
    for root, _, files in os.walk(src):
        for _file in files:
            file_path = os.path.join(root, _file)
            src_files.append(file_path)
    for src_path in src_files:
        if os.path.islink(src_path):
            continue
        file_relpath = os.path.relpath(src_path, src)
        dest_path = os.path.join(dest, file_relpath)
        dest_dir = os.path.dirname(dest_path)
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir, exist_ok=True)
        shutil.copy2(src_path, dest_path)
        result_files.append(src_path)
        result_files.append(dest_path)
    return result_files


def _resources_with_xml_v1(root, testcase_target_name, test_resource_path,
                           part_build_out_path, resource_output_path):
    _out_resources_list = []
    for target in root:
        if target.attrib.get('name') != testcase_target_name:
            continue
        for _depend in target:
            _findpath = _depend.attrib.get('findpath')
            _resource_file = _depend.attrib.get('resource')
            if _findpath == 'res':
                _resource_src = os.path.join(test_resource_path,
                                             _resource_file)
                _res_dest = os.path.join(resource_output_path, _resource_file)
            elif _findpath == 'out':
                if not os.path.exists(_resource_file):
                    __dir_name = _resource_file.split('/')[0]
                    _resource_file_new = os.path.join(__dir_name,
                                                      _resource_file)
                    _resource_src_new = os.path.join(part_build_out_path,
                                                     _resource_file_new)
                    if os.path.exists(_resource_src_new):
                        _resource_src = _resource_src_new
                        _res_dest = os.path.join(resource_output_path,
                                                 _resource_file)
                    else:
                        _resource_src = ''
                        _res_dest = ''
                else:
                    _resource_src = os.path.join(part_build_out_path,
                                                 _resource_file)
                    _res_dest = os.path.join(resource_output_path,
                                             _resource_file)
            else:
                raise Exception(
                    "resource findpath type '{}' not support.".format(
                        _findpath))
            if _resource_src:
                _out_resources_list.append({
                    "src":
                    os.path.relpath(_resource_src),
                    "dest":
                    os.path.relpath(_res_dest)
                })
    return _out_resources_list


def _parse_res_value(value):
    res_file = value.split('->')[0].strip()
    return res_file


def _resources_with_xml_v2(root, testcase_target_name, test_resource_path,
                           part_build_out_path, resource_output_path):
    _out_resources_list = []
    for target in root:
        if target.attrib.get('name') != testcase_target_name:
            continue
        for child in target:
            if child.tag != 'preparer':
                continue
            for _option in child:
                if _option.attrib.get('name') != 'push':
                    continue
                _src_type = _option.attrib.get('src')
                _resource_file_val = _option.attrib.get('value')
                _resource_file = _parse_res_value(_resource_file_val)
                if _src_type == 'res':
                    _resource_src = os.path.join(test_resource_path,
                                                 _resource_file)
                    _res_dest = os.path.join(resource_output_path,
                                             _resource_file)
                elif _src_type == 'out':
                    _resource_src = os.path.join(part_build_out_path,
                                                 _resource_file)
                    _res_dest = os.path.join(resource_output_path,
                                             _resource_file)
                else:
                    raise Exception(
                        "resource src type '{}' not support.".format(
                            _src_type))
                if _resource_src:
                    _out_resources_list.append({
                        "src":
                        os.path.relpath(_resource_src),
                        "dest":
                        os.path.relpath(_res_dest)
                    })
    return _out_resources_list


def find_testcase_resources(resource_config_file, testcase_target_name,
                            test_resource_path, part_build_out_path,
                            resource_output_path):
    if not os.path.exists(resource_config_file):
        return []
    tree = ET.parse(resource_config_file)
    root = tree.getroot()
    if root.attrib.get('ver') == '2.0':
        _resources_list = _resources_with_xml_v2(root, testcase_target_name,
                                                 test_resource_path,
                                                 part_build_out_path,
                                                 resource_output_path)
    else:
        _resources_list = _resources_with_xml_v1(root, testcase_target_name,
                                                 test_resource_path,
                                                 part_build_out_path,
                                                 resource_output_path)
    # copy ohos_test.xml
    _resources_list.append({
        "src":
        resource_config_file,
        "dest":
        os.path.join(resource_output_path,
                     os.path.basename(resource_config_file))
    })
    return _resources_list


def copy_testcase_resources(resource_infos):
    result_dest_list = []
    for resource_info in resource_infos:
        src_file = resource_info.get('src')
        if not os.path.exists(src_file):
            print("warning: testcase resource {} doesn't exist.".format(
                src_file))
            return
        dest_file = resource_info.get('dest')
        dest_dir = os.path.dirname(dest_file)
        if os.path.isdir(src_file):
            result_files = copy_dir(src_file, dest_file)
            result_dest_list.extend(result_files)
        else:
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir, exist_ok=True)
            shutil.copy2(src_file, dest_file)
            if src_file:
                result_dest_list.append(src_file)
                result_dest_list.append(dest_file)
    return result_dest_list


def _get_subsystem_name(part_name):
    subsystem_parts_file = 'build_configs/parts_info/subsystem_parts.json'
    subsystem_parts_info = read_json_file(subsystem_parts_file)
    if subsystem_parts_info is None:
        raise Exception("read file '{}' failed.".format(subsystem_parts_file))
    for name, p_list in subsystem_parts_info.items():
        if part_name in p_list:
            return name
    return None


def _get_subsystem_path(part_name):
    subsystem_name = _get_subsystem_name(part_name)
    if subsystem_name is None:
        return None
    subsystem_build_config_file = os.path.join('build_configs/subsystem_info',
                                               'subsystem_build_config.json')
    config_info = read_json_file(subsystem_build_config_file)
    if config_info is None:
        raise Exception(
            "read file '{}' failed.".format(subsystem_build_config_file))
    subsystem_infos = config_info.get('subsystem')
    info = subsystem_infos.get(subsystem_name)
    if info is None:
        raise Exception(
            "subsystem '{}' info doesn't exist.".format(subsystem_name))
    subsystem_path = info.get('path')
    return subsystem_path


def _parse_module_out_path(module_out_path):
    split_re = module_out_path.split('/', 1)
    part_name = split_re[0]
    module_name = split_re[1]
    return part_name, module_name


def _find_resource_config_file(config_file_name, subsystem_path, module_name):
    resource_config_file = os.path.join('../../', subsystem_path,
                                        'test/resource', module_name,
                                        config_file_name)
    # compatibility
    if not os.path.exists(resource_config_file):
        module_dirs = module_name.split('/')
        _dirs_num = len(module_dirs)
        _dir_name = os.path.dirname(resource_config_file)
        while _dirs_num > 1:
            _dir_name = os.path.dirname(_dir_name)
            resource_config_file = os.path.join(_dir_name, config_file_name)
            if os.path.exists(resource_config_file):
                break
            _dirs_num -= 1
    return resource_config_file


def _get_res_config_file(module_out_path):
    part_name, module_name = _parse_module_out_path(module_out_path)
    subsystem_path = _get_subsystem_path(part_name)
    if subsystem_path is None:
        return None
    resource_config_file = _find_resource_config_file('ohos_test.xml',
                                                      subsystem_path,
                                                      module_name)
    if not os.path.exists(resource_config_file):
        resource_config_file = _find_resource_config_file(
            'harmony_test.xml', subsystem_path, module_name)
    return resource_config_file


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--resource-config-file', required=False)
    parser.add_argument('--testcase-target-name', required=True)
    parser.add_argument('--part-build-out-path', required=True)
    parser.add_argument('--resource-output-path', required=True)
    parser.add_argument('--module-out-path', required=False)
    parser.add_argument('--output-file', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args()
    if not args.resource_config_file:
        if not args.module_out_path:
            raise Exception('Missing parameter module_out_path.')
        resource_config_file = _get_res_config_file(args.module_out_path)
        if resource_config_file is None:
            print("warning: cannot find resource config file, target: '{}'".
                  format(args.testcase_target_name))
            return 0
        if not os.path.exists(resource_config_file):
            return 0
    else:
        resource_config_file = args.resource_config_file
        if not os.path.exists(resource_config_file):
            raise Exception(
                "testcase '{}' resource_config_file config incorrect.".format(
                    args.testcase_target_name))

    test_resource_path = os.path.dirname(resource_config_file)
    resources_list = find_testcase_resources(resource_config_file,
                                             args.testcase_target_name,
                                             test_resource_path,
                                             args.part_build_out_path,
                                             args.resource_output_path)
    write_json_file(args.output_file, resources_list)
    result_dest_list = copy_testcase_resources(resources_list)
    if args.depfile and result_dest_list:
        result_dest_list.sort()
        build_utils.write_depfile(args.depfile,
                                 args.output_file,
                                 result_dest_list,
                                 add_pydeps=False)
    return 0


if __name__ == '__main__':
    sys.exit(main())
