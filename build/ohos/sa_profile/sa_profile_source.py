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
sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402


def get_src_parts(system_install_info_file):
    parts_list = read_json_file(system_install_info_file)
    if parts_list is None:
        raise Exception("read file '{}' failed.".format(system_install_info_file))
    src_parts_info = {}
    for _part_info in parts_list:
        if 'is_source' not in _part_info:
            raise Exception(
                "read subsystem info error, missing key is_source.")
        if _part_info.get('is_source') is True:
            part_name = _part_info.get('part_name')
            origin_part_name = _part_info.get('origin_part_name')
            src_parts_info[part_name] = origin_part_name
    return src_parts_info


def get_sa_install_info(sa_info_file, src_parts_info):
    sa_infos = read_json_file(sa_info_file)
    if sa_infos is None:
        raise Exception("read file '{}' failed.".format(sa_info_file))
    sa_install_infos = []
    for _sa_info in sa_infos:
        _sa_info_part = _sa_info.get('part_name')
        if _sa_info_part in src_parts_info:
            sa_install_infos.append(_sa_info)
    return sa_install_infos


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--system-install-info-file', required=True)
    parser.add_argument('--src-sa-info-file', required=False)
    parser.add_argument('--src-sa-install-info-file', required=True)
    args = parser.parse_args()

    src_parts_info = get_src_parts(args.system_install_info_file)
    sa_install_infos = get_sa_install_info(args.src_sa_info_file,
                                           src_parts_info)
    write_json_file(args.src_sa_install_info_file, sa_install_infos)
    return 0


if __name__ == '__main__':
    sys.exit(main())
