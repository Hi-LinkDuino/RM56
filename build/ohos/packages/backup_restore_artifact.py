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


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--source-dir', nargs='+', help='', required=True)
    parser.add_argument('--output-dir', help='', required=True)
    args = parser.parse_args()

    str_out_basename = os.path.basename(args.output_dir)
    if (str_out_basename == "backup" and not os.path.exists(args.output_dir)):
        os.mkdir(args.output_dir)

    # move source dir
    for source in args.source_dir:
        print(source)
        if os.path.exists(source):
            output_path = None
            if str_out_basename == "backup":
                output_path = os.path.join(
                    args.output_dir, os.path.basename(source))
            if str_out_basename == "restore":
                output_path = args.output_dir.replace(
                    str_out_basename, os.path.basename(source))
            if str_out_basename == "restore_symbols":
                asan_symbols_path = args.output_dir.replace(
                    str_out_basename, os.path.basename(source))
                asan_symbols_backup_path = source.replace(os.path.basename(
                    source), os.path.join(os.path.basename(source), "asan"))
                shutil.move(asan_symbols_path, asan_symbols_backup_path)
                output_path = asan_symbols_path
            if output_path is not None:
                print("Move:{},To:{}".format(source, output_path))
                shutil.move(source, output_path)

    return 0


if __name__ == '__main__':
    sys.exit(main())
