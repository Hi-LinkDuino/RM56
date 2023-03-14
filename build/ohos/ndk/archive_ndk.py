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

import optparse
import os
import sys
import zipfile

sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir))
from scripts.util import build_utils  # noqa: E402


def parse_args(args):
    args = build_utils.expand_file_args(args)

    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)
    parser.add_option('--output', help='generated ndk stub file')
    parser.add_option('--os-irrelevant-dir',
                      help='base directory of ndk common files')
    parser.add_option('--os-specific-dir',
                      help='base directory of os specific stuff')
    parser.add_option('--prefix',
                      help='prefix string of directory in archive zipfile')
    parser.add_option('--notice-file', help='path to notice file')
    parser.add_option('--record-path', help='path to md5.stamp file')

    options, _ = parser.parse_args(args)
    return options


def do_archive(output, directory, prefix, compress_fn):
    files = []
    for root, _, filenames in os.walk(directory):
        for f in filenames:
            files.extend([os.path.join(root, f)])
    with zipfile.ZipFile(output, 'a') as outfile:
        for f in files:
            compress = compress_fn(f) if compress_fn else None
            if prefix:
                zip_path = os.path.join(prefix, os.path.relpath(f, directory))
            else:
                zip_path = os.path.relpath(f, directory)
            build_utils.add_to_zip_hermetic(outfile,
                                         zip_path,
                                         src_path=f,
                                         compress=compress)


def archive_ndk(output, os_irrelevant_dir, os_specific_dir, prefix,
                compress_fn, notice):
    # Create an empty zipfile first, then add stuff to it.
    with zipfile.ZipFile(output, 'w') as outfile:
        pass
    for directory in [os_irrelevant_dir, os_specific_dir]:
        do_archive(output, directory, prefix, compress_fn)

    with zipfile.ZipFile(output, 'a') as zip_file:
        compress = compress_fn(notice) if compress_fn else None
        if prefix:
            zip_path = os.path.join(prefix, os.path.basename(notice))
        else:
            zip_path = os.path.basename(notice)
        build_utils.add_to_zip_hermetic(zip_file,
                                     zip_path,
                                     src_path=notice,
                                     compress=compress)


def main(args):
    options = parse_args(args)

    os_irrelevant_dir = options.os_irrelevant_dir
    os_specific_dir = options.os_specific_dir
    depfile_deps = set(
        build_utils.get_all_files(os_irrelevant_dir) +
        build_utils.get_all_files(os_specific_dir))
    depfile_deps.add(options.notice_file)

    build_utils.call_and_write_depfile_if_stale(lambda: archive_ndk(
        options.output, os_irrelevant_dir, os_specific_dir, options.prefix,
        lambda _: True, options.notice_file),
                                           options,
                                           depfile_deps=depfile_deps,
                                           input_paths=depfile_deps,
                                           output_paths=([options.output]),
                                           record_path=options.record_path,
                                           force=False,
                                           add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
