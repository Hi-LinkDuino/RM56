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

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import write_json_file, read_json_file  # noqa: E402 E501
from scripts.util import build_utils  # noqa: E402


def _get_src_sa_info(src_sa_install_info_file, depfiles):
    src_sa_install_info = read_json_file(src_sa_install_info_file)
    if src_sa_install_info is None:
        raise Exception("read src_sa_install_info_file failed.")
    install_info_file_dict = {}
    for _install_info in src_sa_install_info:
        if _install_info.get('type') == 'sa':
            part_name = _install_info.get('part_name')
            _install_infos = install_info_file_dict.get(part_name)
            if _install_infos is None:
                _install_infos = []
            _install_infos.append(_install_info.get('install_info_file'))
            install_info_file_dict[part_name] = _install_infos

    all_sa_info_files_dict = {}
    for part_name, _install_info_files in install_info_file_dict.items():
        for _install_info_file in _install_info_files:
            _install_info = read_json_file(_install_info_file)
            if _install_info is None:
                raise Exception("read install_info_file '{}' failed.".format(
                    _install_info_file))
            depfiles.append(_install_info_file)
            sa_info_files = _install_info.get('sa_info_files')
            _file_list = all_sa_info_files_dict.get(part_name)
            if _file_list is None:
                all_sa_info_files_dict[part_name] = sa_info_files
            else:
                _file_list.extend(sa_info_files)
                all_sa_info_files_dict[part_name] = _file_list
    return all_sa_info_files_dict


def _file_archive(all_sa_info_files_dict, archive_output_dir,
                  archive_info_file, depfiles):
    info_dict = {}
    _info_file_dir = os.path.dirname(archive_info_file)
    _relative_path = os.path.relpath(_info_file_dir, archive_output_dir)
    for key, value in all_sa_info_files_dict.items():
        subsystem_out_dir = os.path.join(archive_output_dir, key)
        if not os.path.exists(subsystem_out_dir):
            os.makedirs(subsystem_out_dir, exist_ok=True)
        _file_list = []
        for _file in value:
            depfiles.append(_file)
            shutil.copy2(_file, subsystem_out_dir)
            _file_list.append(
                os.path.join(_relative_path, key, os.path.basename(_file)))
        info_dict[key] = _file_list
    write_json_file(archive_info_file, info_dict)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--src-sa-install-info-file', required=True)
    parser.add_argument('--sa-archive-output-dir', required=True)
    parser.add_argument('--sa-archive-info-file', required=True)
    parser.add_argument('--depfile', required=True)
    args = parser.parse_args()

    depfiles = []
    all_sa_info_files_dict = _get_src_sa_info(args.src_sa_install_info_file,
                                              depfiles)

    _file_archive(all_sa_info_files_dict, args.sa_archive_output_dir,
                  args.sa_archive_info_file, depfiles)
    build_utils.write_depfile(args.depfile, args.sa_archive_info_file,
                              depfiles)
    return 0


if __name__ == '__main__':
    sys.exit(main())
