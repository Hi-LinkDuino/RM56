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

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util.file_utils import write_file  # noqa E402


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--base-dir', required=True)
    parser.add_argument('--sub-dir-list', nargs='+', required=False)
    parser.add_argument('--result-file', required=False)
    args = parser.parse_args()

    file_list = []
    dir_list = []
    if args.sub_dir_list:
        for sub_dir in args.sub_dir_list:
            dir_list.append(os.path.join(args.base_dir, sub_dir))
    else:
        dir_list.append(args.base_dir)
    for _dir in dir_list:
        if not os.path.exists(_dir):
            continue
        for root, _, files in os.walk(_dir):
            for _file in files:
                file_list.append(os.path.realpath(os.path.join(root, _file)))

    if args.result_file:
        write_file(args.result_file, "\n".join(file_list))
    else:
        for _file in file_list:
            print(_file)
    return 0


if __name__ == '__main__':
    sys.exit(main())
