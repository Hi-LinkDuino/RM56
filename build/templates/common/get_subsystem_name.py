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
from scripts.util import file_utils  # noqa: E402


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--part-name', required=True)
    parser.add_argument('--part-subsystem-info-file', required=False)
    args = parser.parse_args()

    part_subsystem_info_file = 'build_configs/parts_info/part_subsystem.json'
    if args.part_subsystem_info_file:
        part_subsystem_info_file = args.part_subsystem_info_file
    if not os.path.exists(part_subsystem_info_file):
        raise Exception(
            "file '{}' does not exist.".format(part_subsystem_info_file))

    data = file_utils.read_json_file(part_subsystem_info_file)
    if data is None:
        raise Exception(
            "read file '{}' failed.".format(part_subsystem_info_file))

    subsystem_name = data.get(args.part_name)
    if subsystem_name is None or subsystem_name == '':
        raise Exception("subsystem name error, part_name='{}'".format(
            args.part_name))
    print(subsystem_name)
    return 0


if __name__ == '__main__':
    sys.exit(main())
