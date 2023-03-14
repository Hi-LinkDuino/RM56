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


def binary_install_subsystem(dist_parts_info_file,
                             parts_src_installed_info_file):
    # These subsystem have  binary install packages
    if not os.path.exists(dist_parts_info_file):
        print("dist subsystem info file [{}] no exist.".format(
            dist_parts_info_file))
        return []

    dist_parts_info = read_json_file(dist_parts_info_file)
    if dist_parts_info is None:
        raise Exception("read install parts info file failed.")

    parts_src_list = read_json_file(parts_src_installed_info_file)
    if parts_src_list is None:
        raise Exception("read subsystem src flag file failed.")
    src_part_name_list = []
    for _part_info in parts_src_list:
        src_part_name_list.append(_part_info.get('part_name'))

    required_subsystem = []
    for info in dist_parts_info:
        part_name = info.get('part_name')
        if part_name not in src_part_name_list:
            required_subsystem += [info]
    return required_subsystem


def output_installed_info(binary_subsystem_info, required_parts):
    write_json_file(binary_subsystem_info, required_parts)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--dist-parts-info-file', required=True)
    parser.add_argument('--parts-src-installed-info-file', required=True)
    parser.add_argument('--binary-installed-info-file', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args()

    required_parts = binary_install_subsystem(
        args.dist_parts_info_file, args.parts_src_installed_info_file)

    output_installed_info(args.binary_installed_info_file, required_parts)

    if args.depfile:
        _dep_files = []
        _dep_files.append(args.dist_parts_info_file)
        _dep_files.append(args.parts_src_installed_info_file)
        build_utils.write_depfile(args.depfile,
                                 args.binary_installed_info_file,
                                 _dep_files,
                                 add_pydeps=False)
    return 0


if __name__ == '__main__':
    sys.exit(main())
