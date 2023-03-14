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
import shutil

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util.file_utils import read_json_file  # noqa: E402


def _get_toolchain_name(toolchain_label):
    return toolchain_label.split(':')[1]


def _remove_unstripped_dir(toolchain_info_file):
    data = read_json_file(toolchain_info_file)
    if data is None:
        raise Exception("read file '{}' failed.".format(toolchain_info_file))
    platform_toolchain = data.get('platform_toolchain')
    base_dir_list = []
    for key, val in platform_toolchain.items():
        if key == 'phone':
            base_dir_list.append('.')
        else:
            toolchain_name = val.split(':')[1]
            base_dir_list.append(toolchain_name)
    dir_list = ['lib.unstripped', 'exe.unstripped']
    for _base_dir in base_dir_list:
        for _dir_name in dir_list:
            _dir = os.path.join(_base_dir, _dir_name)
            if os.path.exists(_dir):
                shutil.rmtree(_dir)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--backup-dir', required=True)
    parser.add_argument('--backup-dest-dir', required=True)
    parser.add_argument('--asan-clean', dest='asan_clean', action='store_true')
    parser.set_defaults(asan_clean=False)
    parser.add_argument('--removed-dir-list', nargs='*', default=[])
    parser.add_argument('--platforms-toolchain-info-file')
    args = parser.parse_args()

    if not os.path.exists(args.backup_dir):
        return 0

    if os.path.exists(args.backup_dest_dir):
        shutil.rmtree(args.backup_dest_dir)
    os.makedirs(args.backup_dest_dir, exist_ok=True)

    dir_name = os.path.basename(args.backup_dir)
    _dest_dir = os.path.join(args.backup_dest_dir, dir_name)
    shutil.copytree(args.backup_dir, _dest_dir)

    if args.asan_clean:
        if args.removed_dir_list:
            for _dir in args.removed_dir_list:
                if os.path.exists(_dir):
                    shutil.rmtree(_dir)
        if args.platforms_toolchain_info_file:
            _remove_unstripped_dir(args.platforms_toolchain_info_file)
    return 0


if __name__ == '__main__':
    sys.exit(main())
