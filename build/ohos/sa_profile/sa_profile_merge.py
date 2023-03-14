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
import os
import sys
import shutil

from sa_info_process.merge_sa_info import SAInfoMerger

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import write_json_file, \
    read_json_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402


def _get_src_sa_info(src_sa_install_info_file, depfiles):
    src_sa_install_info = read_json_file(src_sa_install_info_file)
    if src_sa_install_info is None:
        raise Exception("read src_sa_install_info_file failed.")
    install_info_file_list = []
    for _install_info in src_sa_install_info:
        if _install_info.get('type') == 'sa':
            install_info_file_list.append(
                _install_info.get('install_info_file'))

    depfiles.extend(install_info_file_list)
    all_sa_input_files = []
    for _install_info_file in install_info_file_list:
        _install_info = read_json_file(_install_info_file)
        if _install_info is None:
            raise Exception("read install_info_file '{}' failed.".format(
                _install_info_file))
        sa_info_files = _install_info.get('sa_info_files')
        all_sa_input_files.extend(sa_info_files)
    depfiles.extend(all_sa_input_files)
    return all_sa_input_files


def _sa_profile_merge(sa_input_files, no_src_subsystem_sa_zipfile,
                      merge_out_dir, merged_zipfile, target_cpu):
    with build_utils.temp_dir() as tmp:
        build_utils.extract_all(no_src_subsystem_sa_zipfile, tmp)
        for root, _, files in os.walk(tmp):
            for sa_file in files:
                sa_input_files.append(os.path.join(root, sa_file))

        if not os.path.exists(merge_out_dir):
            os.makedirs(merge_out_dir, exist_ok=True)

        is_64bit_arch = target_cpu not in ["arm", "x86"]
        # call merge tool
        merge_tool = SAInfoMerger(is_64bit_arch)
        result_file_list = merge_tool.merge(sorted(sa_input_files),
                                            merge_out_dir)
    build_utils.zip_dir(merged_zipfile, merge_out_dir)
    shutil.rmtree(merge_out_dir)
    return result_file_list


def _generate_install_info(sa_result_file_list, sa_info_install_dest_dir,
                           sa_install_info_file):
    module_install_info_list = []
    for _sa_file in sa_result_file_list:
        _install_dest = os.path.join(sa_info_install_dest_dir,
                                     os.path.basename(_sa_file))
        module_install_info = {
            'type': 'sa_info',
            'source': _sa_file,
            'install_enable': True,
            'dest': [_install_dest]
        }
        module_install_info_list.append(module_install_info)
    write_json_file(sa_install_info_file, module_install_info_list)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--src-sa-install-info-file', required=True)
    parser.add_argument('--no-src-sa-install-info-file', required=True)
    parser.add_argument('--sa-output-dir', required=True)
    parser.add_argument('--merged-sa-profile', required=True)
    parser.add_argument('--sa-install-info-file', required=True)
    parser.add_argument('--sa-info-install-dest-dir', required=True)
    parser.add_argument('--target-cpu', default='arm64')
    parser.add_argument('--depfile', required=True)
    args = parser.parse_args()

    depfiles = []
    src_subsystem_file_list = _get_src_sa_info(args.src_sa_install_info_file,
                                               depfiles)
    no_src_sa_profile_zip = args.no_src_sa_install_info_file

    result_file_list = _sa_profile_merge(src_subsystem_file_list,
                                         no_src_sa_profile_zip,
                                         args.sa_output_dir,
                                         args.merged_sa_profile,
                                         args.target_cpu)
    _generate_install_info(result_file_list, args.sa_info_install_dest_dir,
                           args.sa_install_info_file)
    build_utils.write_depfile(args.depfile, args.sa_install_info_file,
                              depfiles)
    return 0


if __name__ == '__main__':
    sys.exit(main())
