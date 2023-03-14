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

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402


def merge_subsystem(src_install_info_file, binary_install_info_file):
    src_install_info = read_json_file(src_install_info_file)
    if src_install_info is None:
        raise Exception("read src install info file '{}' failed.".format(
            src_install_info_file))

    binary_install_info = None
    if os.path.exists(binary_install_info_file):
        binary_install_info = read_json_file(binary_install_info_file)
        if binary_install_info is None:
            raise Exception("read binary install file '{}' failed.".format(
                binary_install_info_file))
    else:
        print("binary install info file '{}' no exist.".format(
            binary_install_info_file))

    merge_result = {}
    for _subsystem_info in src_install_info:
        part_name = _subsystem_info.get('part_name')
        _subsystem_info['is_source'] = True
        merge_result[part_name] = _subsystem_info
    if binary_install_info:
        for _subsystem_info in binary_install_info:
            part_name = _subsystem_info.get('part_name')
            _subsystem_info['is_source'] = False
            merge_result[part_name] = _subsystem_info
    return merge_result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--src-install-info-file', required=True)
    parser.add_argument('--binary-install-info-file', required=True)
    parser.add_argument('--all-subsystem-info-file', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args()

    all_subsystem_info = merge_subsystem(args.src_install_info_file,
                                         args.binary_install_info_file)
    write_json_file(args.all_subsystem_info_file, all_subsystem_info)

    if args.depfile:
        _dep_files = []
        _dep_files.append(args.src_install_info_file)
        _dep_files.append(args.binary_install_info_file)
        build_utils.write_depfile(args.depfile,
                                  args.all_subsystem_info_file,
                                  _dep_files,
                                  add_pydeps=False)
    return 0


if __name__ == '__main__':
    sys.exit(main())
