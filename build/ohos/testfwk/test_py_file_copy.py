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
sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import write_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402


def copy_file(target_base_dir, source_files, output_dir):
    output_files = []
    for src_file in source_files:
        if not os.path.exists(src_file):
            raise Exception("src file '{}' doesn't exist.".format(src_file))
        relative_path = os.path.relpath(src_file, target_base_dir)
        output_file = os.path.join(output_dir, relative_path)
        dest_dir = os.path.dirname(output_file)
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir, exist_ok=True)
        shutil.copy2(src_file, output_file)
        output_files.append(output_file)
    return output_files


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--target-base-dir', required=True)
    parser.add_argument('--source-files', nargs='+', required=True)
    parser.add_argument('--copy-output-dir', required=True)
    parser.add_argument('--outfile', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args()

    copy_out_list = copy_file(args.target_base_dir, args.source_files,
                              args.copy_output_dir)
    write_file(args.outfile, '\n'.join(copy_out_list))

    if args.depfile:
        _dep_files = (args.source_files + copy_out_list).sort()
        build_utils.write_depfile(args.depfile,
                                 args.outfile,
                                 _dep_files,
                                 add_pydeps=False)
    return 0


if __name__ == '__main__':
    sys.exit(main())
