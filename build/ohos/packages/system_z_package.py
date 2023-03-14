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

import argparse
import sys
import os
import shutil
sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util import build_utils  # noqa: E402
from scripts.util import file_utils  # noqa: E402


def archive_files(system_image_zipfile, additional_files, output_dir,
                  output_file):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir, exist_ok=True)

    build_utils.extract_all(system_image_zipfile, output_dir + "/system", no_clobber=False)
    for _file in additional_files:
        _dest = os.path.join(output_dir, os.path.basename(_file))
        if os.path.isdir(_file):
            if os.path.exists(_dest):
                shutil.rmtree(_dest)
            shutil.copytree(_file, _dest)
        else:
            shutil.copy2(_file, _dest)

    files_list = []
    for root, _, files in os.walk(output_dir):
        for _file in files:
            files_list.append(os.path.join(root, _file))
    file_utils.write_file(output_file, '\n'.join(files_list))


def main(argv):
    argv = build_utils.expand_file_args(argv)
    parser = argparse.ArgumentParser()
    build_utils.add_depfile_option(parser)
    parser.add_argument("--system-image-zipfile", required=True)
    parser.add_argument('--output-dir', required=True)
    parser.add_argument('--output-file', required=True)
    parser.add_argument('--additional-files', action='append')
    args = parser.parse_args(argv[1:])

    depfiles = [args.system_image_zipfile] + args.additional_files
    build_utils.call_and_write_depfile_if_stale(
        lambda: archive_files(args.system_image_zipfile, args.additional_files,
                              args.output_dir, args.output_file),
        args,
        depfile_deps=depfiles,
        input_paths=depfiles,
        output_paths=([args.output_file, args.output_dir]),
        force=False,
        add_pydeps=False)


if __name__ == "__main__":
    main(sys.argv)
