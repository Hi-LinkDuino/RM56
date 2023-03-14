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


def subsystem_list(all_subsystem_info_file, curr_subsystem_name_list_file):
    curr_subsystem_list = []
    all_subsystem_info = read_json_file(all_subsystem_info_file)
    if all_subsystem_info is None:
        raise Exception(
            "read file '{}' failed.".format(all_subsystem_info_file))
    curr_subsystem_name_list = read_json_file(curr_subsystem_name_list_file)
    if curr_subsystem_name_list is None:
        raise Exception(
            "read file '{}' failed.".format(curr_subsystem_name_list_file))
    for _subsystem_name in curr_subsystem_name_list:
        if _subsystem_name not in all_subsystem_info:
            continue
        else:
            curr_subsystem_list.append(all_subsystem_info.get(_subsystem_name))
    return curr_subsystem_list


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--target-platform-parts', required=True)
    parser.add_argument('--parts-install-info-file', required=True)
    parser.add_argument('--current-platform', required=True)
    args = parser.parse_args()

    curr_subsystem_list = subsystem_list(args.all_subsystem_info_file,
                                         args.target_platform_subsystem)
    platform_out_dir = os.path.dirname(args.system_install_info_file)
    if not os.path.exists(platform_out_dir):
        os.makedirs(platform_out_dir)
    write_json_file(args.system_install_info_file, curr_subsystem_list)
    return 0


if __name__ == '__main__':
    sys.exit(main())
