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
import re
sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402


def _get_label_name(label):
    return re.split('[:|()]', label)[1]


def _read_notice_info_file(subsystem_notice_info_dir, platform_base_dir):
    subsystem_notice_info_list = []
    subsystem_notice_files = []
    _file_list = os.listdir(subsystem_notice_info_dir)
    for _info_file in _file_list:
        _info_file_path = os.path.join(subsystem_notice_info_dir, _info_file)
        if not os.path.isfile(_info_file_path):
            continue
        if not _info_file.endswith('_notice_file'):
            continue
        subsystem_notice_info_list.append(_info_file_path)
        _nf_src_list = read_json_file(_info_file_path)
        for _nf_src in _nf_src_list:
            _dest = os.path.join(platform_base_dir, os.path.dirname(_nf_src))
            if not os.path.exists(_dest):
                os.makedirs(_dest, exist_ok=True)
            shutil.copy2(_nf_src, _dest)
            _dest_file = os.path.relpath(
                os.path.join(_dest, os.path.basename(_nf_src)))
            subsystem_notice_files.append(_dest_file)
    return subsystem_notice_info_list, subsystem_notice_files


def get_notice_info(system_install_info_file, notice_info_dir,
                    platform_base_dir):
    system_install_info = read_json_file(system_install_info_file)
    if system_install_info is None:
        raise Exception(
            "read file '{}' failed.".format(system_install_info_file))

    src_subsystem_list = []
    for subsystem_info in system_install_info:
        if subsystem_info.get('is_source') is False:
            continue
        part_label = subsystem_info.get('part_label')
        part_name = _get_label_name(part_label)
        src_subsystem_list.append(part_name)

    notice_info_file_list = []
    system_notice_files = []
    for subsystem_name in src_subsystem_list:
        subsystem_notice_info_dir = os.path.join(notice_info_dir,
                                                 subsystem_name)
        if not os.path.exists(subsystem_notice_info_dir):
            continue
        subsystem_notice_info_list, subsystem_notice_files = _read_notice_info_file(
            subsystem_notice_info_dir, platform_base_dir)
        notice_info_file_list.extend(subsystem_notice_info_list)
        system_notice_files.extend(subsystem_notice_files)
    return notice_info_file_list, system_notice_files


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--system-install-info-file', required=True)
    parser.add_argument('--notice-file-info-dir', required=True)
    parser.add_argument('--platform-base-dir', required=True)
    parser.add_argument('--output-file', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args()

    _dep_files = [args.system_install_info_file]
    notice_info_file_list, system_notice_files = get_notice_info(
        args.system_install_info_file, args.notice_file_info_dir,
        args.platform_base_dir)

    _dep_files.extend(notice_info_file_list)
    write_json_file(args.output_file, system_notice_files)

    if args.depfile:
        list.sort(_dep_files)
        build_utils.write_depfile(args.depfile,
                                 args.output_file,
                                 _dep_files,
                                 add_pydeps=False)
    return 0


if __name__ == '__main__':
    sys.exit(main())
