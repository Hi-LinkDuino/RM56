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
from scripts.util.file_utils import write_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402


def copy_files(source_files, output_dir):
    output_files = []
    for src_file in source_files:
        if not os.path.exists(src_file):
            raise Exception("src file '{}' doesn't exist.".format(src_file))
        if not os.path.isfile(src_file):
            continue
        file_name = os.path.basename(src_file)
        output_file = os.path.join(output_dir, file_name)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir, exist_ok=True)
        shutil.copy2(src_file, output_file)
        output_files.append(output_file)
    return output_files


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--source-files', nargs='+', required=True)
    parser.add_argument('--copy-output-dir', required=True)
    parser.add_argument('--outfile', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args()

    copy_out_list = copy_files(args.source_files, args.copy_output_dir)
    write_file(args.outfile, '\n'.join(copy_out_list))

    if args.depfile:
        _dep_files = []
        _dep_files.extend(args.source_files)
        _dep_files.extend(copy_out_list)
        _dep_files.sort()
        build_utils.write_depfile(args.depfile,
                                 args.outfile,
                                 _dep_files,
                                 add_pydeps=False)
    return 0


if __name__ == '__main__':
    sys.exit(main())
