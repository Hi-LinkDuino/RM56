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
import json
import sys
import argparse


def read_json_file(input_file):
    if not os.path.exists(input_file):
        print("file '{}' doesn't exist.".format(input_file))
        return None

    data = None
    try:
        with open(input_file, 'r') as input_f:
            data = json.load(input_f)
    except json.decoder.JSONDecodeError:
        print("The file '{}' format is incorrect.".format(input_file))
        raise
    return data


def write_json_file(output_file, content):
    file_dir = os.path.dirname(os.path.abspath(output_file))
    if not os.path.exists(file_dir):
        os.makedirs(file_dir, exist_ok=True)
    with open(output_file, 'w') as output_f:
        json.dump(content, output_f, indent=2)


def get_wrong_used_deps(parts_path_file, deps_files):

    print("Start check deps!")
    part_path_data = read_json_file(parts_path_file)
    wrong_used_deps = {}
    not_exist_part = []
    ignore_parts = ['unittest', 'moduletest', 'systemtest']

    for filename in os.listdir(deps_files):
        file_path = os.path.join(deps_files, filename)
        module_deps_data = read_json_file(file_path)

        part_name = module_deps_data.get("part_name")
        deps = module_deps_data.get("deps")
        module_label = module_deps_data.get("module_label").split("(")[0]
        part_path = part_path_data.get(part_name, "no_part_path")

        if part_name in ignore_parts:
            continue
        if part_path == "no_part_path":
            if part_name not in not_exist_part:
                print("Warning! Can not find part '{}' path".format(part_name))
                not_exist_part.append(part_name)
            continue

        _wrong_used_deps = []
        for dep in deps:
            dep_path = dep[2:]
            if not dep_path.startswith(part_path):
                _wrong_used_deps.append(dep)
        if len(_wrong_used_deps) > 0:
            if part_name not in wrong_used_deps:
                wrong_used_deps[part_name] = {module_label: _wrong_used_deps}
            else:
                wrong_used_deps[part_name][module_label] = _wrong_used_deps

    output_file = os.path.join(os.path.dirname(
        deps_files), "module_deps_info", "wrong_used_deps.json")
    write_json_file(output_file, wrong_used_deps)
    print("Check deps result output to '{}'.".format(output_file))


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--parts-path-file', required=True)
    parser.add_argument('--deps-path', required=True)
    args = parser.parse_args(argv)
    get_wrong_used_deps(args.parts_path_file, args.deps_path)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
