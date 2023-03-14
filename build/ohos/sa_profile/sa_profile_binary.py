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
import shutil
import argparse
import zipfile
sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402


def get_no_src_parts(system_install_info_file):
    install_parts_info = read_json_file(system_install_info_file)
    if install_parts_info is None:
        raise Exception("read no src subsystem info file failed.")
    parts_name_list = []
    for info in install_parts_info:
        if 'is_source' not in info:
            raise Exception(
                "read subsystem info error, missing key is_source.")
        if info.get('is_source') is True:
            continue
        if 'part_name' not in info:
            raise Exception("read part info error, missing key part_name.")

        part_name = info.get('part_name')
        parts_name_list.append(part_name)
    return parts_name_list


def generate_binary_sa_archive(parts_list, sa_profile_archive_info_file,
                               sa_output_dir, depfiles):
    if not os.path.exists(sa_output_dir):
        os.makedirs(sa_output_dir, exist_ok=True)
    sa_tuples = []
    subsystem_sa_archive_dir = os.path.dirname(sa_profile_archive_info_file)
    if not os.path.exists(subsystem_sa_archive_dir):
        return sa_tuples
    if not os.path.exists(sa_profile_archive_info_file):
        print("warning: sa profile archive info file does not exist.")
        return sa_tuples

    sa_profile_archive_info = read_json_file(sa_profile_archive_info_file)
    if sa_profile_archive_info is None:
        raise Exception("read sa profile archive info file failed.")

    for _part_name in parts_list:
        _sa_file_list = sa_profile_archive_info.get(_part_name)
        if _sa_file_list is None:
            continue
        for _sa_file in _sa_file_list:
            _sa_file_path = os.path.join(subsystem_sa_archive_dir, _sa_file)
            sa_tuples.append((_sa_file_path,
                              os.path.relpath(_sa_file_path,
                                              subsystem_sa_archive_dir)))
            depfiles.append(_sa_file_path)
    return sa_tuples


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--system-install-info-file', required=True)
    parser.add_argument('--sa-profile-archive-info-file', required=False)
    parser.add_argument('--sa-output-dir', required=True)
    parser.add_argument('--sa-output-zipfile', required=True)
    parser.add_argument('--depfile', required=True)
    args = parser.parse_args()

    sa_files_list = []
    depfiles = []
    sa_files_tuples = []
    if args.sa_profile_archive_info_file:
        depfiles.append(args.sa_profile_archive_info_file)
        parts_list = get_no_src_parts(args.system_install_info_file)
        sa_files_tuples.extend(
            generate_binary_sa_archive(parts_list,
                                       args.sa_profile_archive_info_file,
                                       args.sa_output_dir, depfiles))
    with zipfile.ZipFile(args.sa_output_zipfile, 'w') as outfile:
        for sa_file_path, sa_file in sa_files_tuples:
            build_utils.add_to_zip_hermetic(outfile,
                                         sa_file,
                                         src_path=sa_file_path)
    build_utils.write_depfile(args.depfile, args.sa_output_zipfile, depfiles)
    return 0


if __name__ == '__main__':
    sys.exit(main())
