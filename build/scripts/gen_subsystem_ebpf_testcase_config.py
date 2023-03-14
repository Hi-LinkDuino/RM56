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

"""

Usage: gen_ebpf_testcase_config.py --subsystem-name common \
               --subsystem-testcase-config-file xx/xxx.json \
               --subsystem-testcase-list [xx/xx.py yy/yy.py ... ] \
               ----subsystem-testcase-collect-path [xxx]

Generate the ebpf testcase config files.

"""
import argparse
import os
import os.path
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util.file_utils import write_json_file   # noqa: E402, E501


def get_testcase_dest_list(src_testcase_list, testcase_collect_path):
    dest_list = []
    for testcase in src_testcase_list:
        file_name = os.path.basename(testcase)
        dest_list.append(os.path.join(testcase_collect_path, file_name))
    return dest_list


def write_testcase_config_file(subsystem_name,
                               config_file,
                               testcase_list,
                               testcase_collect_path):
    dest_list = get_testcase_dest_list(testcase_list, testcase_collect_path)

    data = {
        'subsystem_name': subsystem_name,
        'testcase_src_list': testcase_list,
        'testcase_dest_list': dest_list
    }

    # write the subsystem ebpf testcase config file.
    write_json_file(config_file, data)


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--subsystem-name', help='', required=True)
    parser.add_argument('--subsystem-ebpf-testcase-config-file',
                        help='',
                        required=True)
    parser.add_argument('--subsystem-testcase-list', nargs='+',
                        help='',
                        required=True)
    parser.add_argument('--subsystem-testcase-collect-path',
                        help='',
                        required=True)
    args = parser.parse_args()

    subsystem_name = args.subsystem_name
    subsystem_testcase_config_file = args.subsystem_ebpf_testcase_config_file
    subsystem_testcase_list = args.subsystem_testcase_list
    subsystem_testcase_collect_path = args.subsystem_testcase_collect_path

    write_testcase_config_file(subsystem_name,
                               subsystem_testcase_config_file,
                               subsystem_testcase_list,
                               subsystem_testcase_collect_path)


if __name__ == "__main__":
    main(sys.argv)
