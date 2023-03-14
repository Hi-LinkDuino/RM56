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
import argparse
import os
import shutil

sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir))
from scripts.util.file_utils import read_json_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402

README_FILE_NAME = 'README.OpenSource'
LICENSE_CANDIDATES = [
    'LICENSE',
    'License',
    'NOTICE',
    'Notice',
    'COPYRIGHT',
    'Copyright'
]


def is_top_dir(current_dir):
    return os.path.exists(os.path.join(current_dir, '.gn'))


def find_license_recursively(current_dir, default_license):
    if is_top_dir(current_dir):
        return default_license
    for file in ['LICENSE', 'NOTICE', 'License', 'Copyright']:
        candidate = os.path.join(current_dir, file)
        if os.path.isfile(os.path.join(current_dir, file)):
            return os.path.join(candidate)
    return find_license_recursively(os.path.dirname(current_dir),
                                    default_license)


def get_license_from_readme(readme_path):
    contents = read_json_file(readme_path)
    if contents is None:
        raise Exception("Error: failed to read {}.".format(readme_path))

    notice_file = contents[0].get('License File').strip()
    if notice_file is None:
        raise Exception("Error: value of notice file is empty in {}.".format(
            readme_path))

    return os.path.join(os.path.dirname(readme_path), notice_file)


def do_collect_notice_files(options, depfiles):
    notice_file = options.license_file
    if notice_file is None:
        readme_path = os.path.join(options.module_source_dir,
                                   README_FILE_NAME)
        if os.path.exists(readme_path):
            depfiles.append(readme_path)
            notice_file = get_license_from_readme(readme_path)

    if notice_file is None:
        notice_file = find_license_recursively(options.module_source_dir,
                                               options.default_license)

    if notice_file:
        for output in options.output:
            os.makedirs(os.path.dirname(output), exist_ok=True)
            if os.path.exists(notice_file):
                shutil.copy(notice_file, output)
            else:
                build_utils.touch(output)
        depfiles.append(notice_file)


def main(args):
    args = build_utils.expand_file_args(args)

    parser = argparse.ArgumentParser()
    build_utils.add_depfile_option(parser)

    parser.add_argument('--license-file', required=False)
    parser.add_argument('--default-license', required=True)
    parser.add_argument('--output', action='append', required=False)
    parser.add_argument('--module-source-dir',
                        help='source directory of this module',
                        required=True)

    options = parser.parse_args()
    depfiles = []

    do_collect_notice_files(options, depfiles)
    if options.license_file:
        depfiles.append(options.license_file)
    build_utils.write_depfile(options.depfile, options.output[0], depfiles)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
