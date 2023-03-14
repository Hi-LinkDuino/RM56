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


def get_platform_parts(current_platform, platforms_parts_file):
    all_platforms_parts = read_json_file(platforms_parts_file)
    if all_platforms_parts is None:
        raise Exception("read file '{}' failed.".format(all_platforms_parts))
    platform_parts = all_platforms_parts.get(current_platform)
    return platform_parts


def platform_parts(all_parts_info_file, current_platform_parts):
    _parts_list = []
    all_parts_info = read_json_file(all_parts_info_file)
    if all_parts_info is None:
        raise Exception("read file '{}' failed.".format(all_parts_info_file))

    for part_name in current_platform_parts.keys():
        if part_name not in all_parts_info:
            raise Exception(
                "required part '{}' doesn't exist.".format(part_name))
        _parts_list.append(all_parts_info.get(part_name))
    return _parts_list


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--all-parts-info-file', required=True)
    parser.add_argument('--platforms-parts-file', required=True)
    parser.add_argument('--system-install-info-file', required=True)
    parser.add_argument('--current-platform', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args()

    current_platform_parts = get_platform_parts(args.current_platform,
                                                args.platforms_parts_file)

    _parts_list = platform_parts(args.all_parts_info_file,
                                 current_platform_parts)

    platform_out_dir = os.path.dirname(args.system_install_info_file)
    if not os.path.exists(platform_out_dir):
        os.makedirs(platform_out_dir, exist_ok=True)
    write_json_file(args.system_install_info_file, _parts_list)

    if args.depfile:
        _dep_files = []
        _dep_files.append(args.all_parts_info_file)
        _dep_files.append(args.platforms_parts_file)
        build_utils.write_depfile(args.depfile,
                                 args.system_install_info_file,
                                 _dep_files,
                                 add_pydeps=False)
    return 0


if __name__ == '__main__':
    sys.exit(main())
