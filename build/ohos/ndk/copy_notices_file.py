#!/usr/bin/env python3
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
import shutil


def create_dest_file(dest_dir):
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir, exist_ok=True)


def get_file_name(target_name, opensource_name):
    file_name = ''
    if not opensource_name.strip():
        file_name = '{}.txt'.format(target_name)
    else:
        file_name = '{}.txt'.format(opensource_name)
    return file_name


def merge_multi_notices(notice_root_dir,
                        module_notices,
                        target_name,
                        opensource_name):

    dest_file = os.path.join(notice_root_dir,
        get_file_name(target_name, opensource_name))

    with open(dest_file, 'a') as dest_notice:
        for notice in module_notices:
            if os.path.exists(notice):
                with open(notice, 'r', errors='ignore') as source_notice:
                    for line in source_notice.readlines():
                        dest_notice.write(line)
                dest_notice.write(u'\n\n')


def copy_notice_file(root_out_dir,
                     module_notices,
                     target_name,
                     opensource_name):
    nf_dest_dir = os.path.join(root_out_dir, 'NOTICE_FILE/system')
    create_dest_file(nf_dest_dir)

    # If the module has multi-notices, it need to merge one file.
    if len(module_notices) > 1:
        merge_multi_notices(nf_dest_dir,
                            module_notices,
                            target_name,
                            opensource_name)
    else:
        for notice in module_notices:
            if os.path.exists(notice):
                file_name = get_file_name(target_name, opensource_name)
                dest_file = os.path.join(nf_dest_dir, file_name)
                shutil.copy(notice, dest_file)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--root-out-dir', help='', required=True)
    parser.add_argument('--target-name', help='', required=True)
    parser.add_argument('--opensource-name', help='', required=True)
    parser.add_argument('--module-notices', nargs='+', help='', required=True)
    args = parser.parse_args()

    copy_notice_file(args.root_out_dir,
                     args.module_notices,
                     args.target_name,
                     args.opensource_name)

    return 0


if __name__ == '__main__':
    sys.exit(main())
