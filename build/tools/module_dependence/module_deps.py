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

import sys
import os
import argparse

import file_utils
import dependence_analysis


def gen_module_deps(deps_data):
    part_allowlist = ['unittest', 'moduletest', 'systemtest']
    label_to_alias = {}
    for _module_alias, _info in deps_data.items():
        _module_label = _info.get('module_label').split('(')[0]
        label_to_alias[_module_label] = _module_alias

    modeule_deps_data = {}
    for _module_alias, _info in deps_data.items():
        deps_module_list = []
        _part_name = _info.get('part_name')
        if _part_name in part_allowlist:
            continue
        _deps_label_list = _info.get('deps')
        for _deps_label in _deps_label_list:
            _alias = label_to_alias.get(_deps_label)
            if _alias is None:
                continue
            deps_module_list.append(_alias)
        _external_deps_list = _info.get('external_deps')
        for _ext_deps in _external_deps_list:
            deps_module_list.append(_ext_deps)

        deps_value = modeule_deps_data.get(_module_alias, [])
        deps_value.extend(deps_module_list)
        modeule_deps_data[_module_alias] = list(set(deps_value))
    return modeule_deps_data


def run(deps_files_path, output_path):
    all_deps_data = dependence_analysis.get_all_deps_data(deps_files_path)
    all_deps_data_file = os.path.join(output_path, 'all_deps_data.json')
    file_utils.write_json_file(all_deps_data_file, all_deps_data)

    module_deps_data = gen_module_deps(all_deps_data)
    module_deps_data_file = os.path.join(output_path, 'module_deps_info.json')
    file_utils.write_json_file(module_deps_data_file, module_deps_data)


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--deps-files-path', required=True)
    args = parser.parse_args(argv)

    if not os.path.exists(args.deps_files_path):
        raise Exception("'{}' doesn't exist.".format(args.deps_files_path))
    output_path = os.path.join(os.path.dirname(args.deps_files_path),
                               'module_deps_info')
    print("------Generate module dependency info------")
    run(args.deps_files_path, output_path)
    print('module deps data output to {}'.format(output_path))


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
