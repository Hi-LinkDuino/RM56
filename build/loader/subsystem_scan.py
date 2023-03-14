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

import os
import sys
import argparse
import glob

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402 E501

_default_subsystem = {"common": "build/common"}


def _read_config(subsystem_config_file, example_subsystem_file):
    if not os.path.exists(subsystem_config_file):
        raise Exception(
            "config file '{}' doesn't exist.".format(subsystem_config_file))
    subsystem_config = read_json_file(subsystem_config_file)
    if subsystem_config is None:
        raise Exception("read file '{}' failed.".format(subsystem_config_file))

    # example subsystem
    if example_subsystem_file:
        example_subsystem_config = read_json_file(example_subsystem_file)
        if example_subsystem_config is not None:
            subsystem_config.update(example_subsystem_config)

    subsystem_info = {}
    for key, val in subsystem_config.items():
        if 'path' not in val:
            raise Exception("subsystem '{}' not config path.".format(key))
        subsystem_info[key] = val.get('path')
    return subsystem_info


def _scan_build_file(subsystem_path):
    build_config_file_name = "ohos.build"
    search_str = "{}/**/{}".format(subsystem_path, build_config_file_name)
    _files = glob.glob(search_str, recursive=True)
    # campatibility bundle.json and ohos.build
    bundle_file_name = 'bundle.json'
    _bundle_files = glob.glob("{}/**/{}".format(subsystem_path,
                                                bundle_file_name),
                              recursive=True)
    if _bundle_files:
        _files.extend(_bundle_files)
    return _files


def scan(subsystem_config_file, example_subsystem_file, source_root_dir):
    subsystem_infos = _read_config(subsystem_config_file,
                                   example_subsystem_file)
    # add common subsystem info
    subsystem_infos.update(_default_subsystem)

    no_src_subsystem = {}
    _build_configs = {}
    for key, val in subsystem_infos.items():
        _info = {'path': val}
        _subsystem_path = os.path.join(source_root_dir, val)

        _build_config_files = _scan_build_file(_subsystem_path)

        if _build_config_files:
            _info['build_files'] = _build_config_files
            _build_configs[key] = _info
        else:
            no_src_subsystem[key] = val

    scan_result = {
        'source_path': source_root_dir,
        'subsystem': _build_configs,
        'no_src_subsystem': no_src_subsystem
    }
    return scan_result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--subsystem-config-file', required=True)
    parser.add_argument('--example-subsystem-file', required=False)
    parser.add_argument('--source-root-dir', required=True)
    parser.add_argument('--output-dir', required=True)
    args = parser.parse_args()

    build_configs = scan(args.subsystem_config_file,
                         args.example_subsystem_file, args.source_root_dir)

    build_configs_file = os.path.join(args.output_dir,
                                      "subsystem_build_config.json")
    write_json_file(build_configs_file, build_configs)
    return 0


if __name__ == '__main__':
    sys.exit(main())
