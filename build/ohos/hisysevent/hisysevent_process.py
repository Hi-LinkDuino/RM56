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
from gen_def_from_all_yaml import merge_hisysevent_config  # noqa: E402

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))

from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402 E501
from scripts.util import build_utils  # noqa: E402


def _get_src_parts(system_install_info_file):
    parts_list = read_json_file(system_install_info_file)
    if parts_list is None:
        raise Exception(
            "read file '{}' failed.".format(system_install_info_file))
    src_part_list = []
    for _part_info in parts_list:
        if 'is_source' not in _part_info:
            raise Exception("read subsystem info error, missing key is_source")
        if _part_info.get('is_source') is True:
            src_part_list.append(_part_info.get('origin_part_name'))
    return src_part_list


def _get_all_config_files(hisysevent_config_info, src_part_list,
                          source_root_dir, gn_root_build_dir):
    hisysevent_config_files = []
    for origin_part_name in src_part_list:
        _config_files = hisysevent_config_info.get(origin_part_name, [])
        for _config_file in _config_files:
            if not _config_file.startswith('//'):
                raise Exception(
                    "part '{}' hisysevent config incorrect.".format(
                        origin_part_name))
            _relpath = os.path.relpath(
                os.path.join(source_root_dir, _config_file[2:]),
                gn_root_build_dir)
            hisysevent_config_files.append(_relpath)
    return hisysevent_config_files


def _get_install_info(hisysevent_config_files, output_path,
                      config_install_dest_dir):
    install_info_list = []
    if hisysevent_config_files:
        hisysevent_merge_result_file = merge_hisysevent_config(
            hisysevent_config_files, output_path)
        _config_install_info = {}
        _config_install_info['type'] = 'hisysevent_config'
        _config_install_info['source'] = hisysevent_merge_result_file
        _config_install_info['install_enable'] = True
        _install_dest = os.path.join(
            config_install_dest_dir,
            os.path.basename(hisysevent_merge_result_file))
        _config_install_info['dest'] = [_install_dest]
        install_info_list.append(_config_install_info)
    return install_info_list


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--parts-config-file', required=True)
    parser.add_argument('--system-install-info-file', required=True)
    parser.add_argument('--config-install-dest-dir', required=True)
    parser.add_argument('--hisysevent-install-info-file', required=True)
    parser.add_argument('--depfile', required=True)
    parser.add_argument('--source-root-dir', required=True)
    parser.add_argument('--gn-root-build-dir', required=True)
    args = parser.parse_args(argv)

    depfiles = [args.parts_config_file]
    hisysevent_config_info = read_json_file(args.parts_config_file)
    if hisysevent_config_info is None:
        raise Exception("read file '{}' failed.".format(
            args.parts_config_file))
    src_part_list = _get_src_parts(args.system_install_info_file)

    hisysevent_config_files = _get_all_config_files(hisysevent_config_info,
                                                    src_part_list,
                                                    args.source_root_dir,
                                                    args.gn_root_build_dir)

    _output_path = os.path.dirname(args.hisysevent_install_info_file)
    install_info_list = _get_install_info(hisysevent_config_files,
                                          _output_path,
                                          args.config_install_dest_dir)
    write_json_file(args.hisysevent_install_info_file, install_info_list)
    build_utils.write_depfile(args.depfile, args.hisysevent_install_info_file,
                              depfiles)

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))