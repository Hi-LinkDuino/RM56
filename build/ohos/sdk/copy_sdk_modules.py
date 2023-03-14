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
import zipfile

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util import build_utils  # noqa: E402
from scripts.util.file_utils import read_json_file  # noqa: E402


def get_source_from_module_info_file(module_info_file):
    data = read_json_file(module_info_file)
    if data is None:
        raise Exception("read file '{}' failed.".format(module_info_file))
    source = data.get('source')
    notice = data.get('notice')
    return source, notice


def do_copy_and_stamp(copy_infos, options, depfile_deps):
    notice_tuples = []
    for cp_info in copy_infos:
        source = cp_info.get('source')
        dest = cp_info.get('dest')
        notice = cp_info.get('notice')
        install_dir = cp_info.get('install_dir')
        if os.path.isdir(source):
            if os.listdir(source):
                files = build_utils.get_all_files(source)
                if files:
                    shutil.copytree(source, dest, dirs_exist_ok=True)
                    depfile_deps.update(build_utils.get_all_files(source))
                else:
                    # Skip empty directories.
                    depfile_deps.add(source)
        else:
            dest_dir = os.path.dirname(dest)
            os.makedirs(dest_dir, exist_ok=True)
            shutil.copy2(source, dest)
            depfile_deps.add(source)
        if notice and os.path.exists(notice):
            depfile_deps.add(notice)
            if notice.endswith('.zip'):
                suffix = ".zip"
            else:
                suffix = ".txt"
            if os.path.isdir(source):
                notice_dest = os.path.join('{}{}'.format(install_dir, suffix))
            else:
                notice_dest = os.path.join(
                    install_dir, '{}{}'.format(os.path.basename(source),
                                               suffix))
            notice_tuples.append((notice_dest, notice))
    build_utils.zip_dir(options.sdk_output_archive,
                        options.archive_dir,
                        compress_fn=lambda _: zipfile.ZIP_DEFLATED,
                        zip_prefix_path=options.zip_prefix_path)
    with zipfile.ZipFile(options.notice_output_archive, 'w') as outfile:
        for zip_path, fs_path in notice_tuples:
            build_utils.add_to_zip_hermetic(outfile,
                                            zip_path,
                                            src_path=fs_path)

    build_utils.write_depfile(options.depfile,
                              options.sdk_output_archive,
                              depfile_deps,
                              add_pydeps=False)


def main():
    parser = argparse.ArgumentParser()
    build_utils.add_depfile_option(parser)

    parser.add_argument('--sdk-modules-desc-file', required=True)
    parser.add_argument('--sdk-archive-paths-file', required=True)
    parser.add_argument('--dest-dir', required=True)
    parser.add_argument('--archive-dir', required=True)
    parser.add_argument('--zip-prefix-path', default=None)
    parser.add_argument('--notice-output-archive', required=True)
    parser.add_argument('--sdk-output-archive', required=True)

    options = parser.parse_args()

    sdk_modules_desc_file = options.sdk_modules_desc_file
    sdk_out_dir = options.dest_dir
    sdk_archive_paths_file = options.sdk_archive_paths_file

    sdk_modules = read_json_file(sdk_modules_desc_file)
    if sdk_modules is None:
        sdk_modules = []

    archive_paths = read_json_file(sdk_archive_paths_file)
    if archive_paths is None:
        archive_paths = []

    depfile_deps = set(
        [options.sdk_modules_desc_file, options.sdk_archive_paths_file])
    copy_infos = []
    for module in sdk_modules:
        cp_info = {}
        sdk_label = module.get('label')
        module_info_file = module.get('module_info_file')
        source, notice = get_source_from_module_info_file(module_info_file)
        cp_info['source'] = source
        cp_info['notice'] = notice
        depfile_deps.add(module_info_file)

        for item in archive_paths:
            if sdk_label == item.get('label'):
                dest = os.path.join(sdk_out_dir, item.get('install_dir'),
                                    os.path.basename(source))
                break
        cp_info['dest'] = dest
        cp_info['install_dir'] = item.get('install_dir')
        copy_infos.append(cp_info)

    do_copy_and_stamp(copy_infos, options, depfile_deps)


if __name__ == '__main__':
    sys.exit(main())
