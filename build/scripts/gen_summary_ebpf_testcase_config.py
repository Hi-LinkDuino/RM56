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

Usage: gen_summary_ebpf_testcase_config.py \
               --ebpf-testcase-path out/xxx/ebpf_testcase \
               --ebpf-summary-config-file xx/xxx.json \

Generate the summary ebpf testcase config files.

"""
import argparse
import os
import os.path
import sys
import json


def summary_subsystem_config_file(testcase_dir, summary_file):
    if testcase_dir == ' ':
        return

    subsystem_list = []
    for root, dirs, files in os.walk(testcase_dir):
        for name in files:
            if name.endswith('.json'):
                subsystem_list.append(os.path.join(root, name))

    # load the subsystem testcase info
    context = []
    for file_path in subsystem_list:
        try:
            with open(file_path, 'r') as infile:
                file_data = json.load(infile)
                context.append(file_data)
        except OSError as err:
            raise err

    # write the summary file.json
    try:
        with open(summary_file, 'w') as out_file:
            json.dump(context, out_file, sort_keys=True, indent=2)
    except OSError as err:
        raise err


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--ebpf-testcase-path', help='', required=True)
    parser.add_argument('--ebpf-summary-config-file', help='', required=True)
    args = parser.parse_args()

    testcase_dir = args.ebpf_testcase_path
    summary_file = args.ebpf_summary_config_file

    if os.path.exists(summary_file):
        os.remove(summary_file)

    summary_subsystem_config_file(testcase_dir, summary_file)


if __name__ == "__main__":
    main(sys.argv)
