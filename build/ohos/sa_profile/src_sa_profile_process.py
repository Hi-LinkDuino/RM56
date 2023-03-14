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
from scripts.util import build_utils  # noqa: E402


def sa_info_process(sa_profile_infos_file, toolchain_platform):
    sa_profile_infos = read_json_file(sa_profile_infos_file)
    if sa_profile_infos is None:
        raise Exception("read file '{}' failed.".format(sa_profile_infos))
    processed_sa_infos = []
    for _sa_info in sa_profile_infos:
        origin_part_name = _sa_info.get('part_name')
        if origin_part_name is None or origin_part_name == '':
            raise Exception(
                "get part name failed in sa info, part_name='{}'".format(
                    origin_part_name))
        _toolchain = _sa_info.get('toolchain')
        platform_name = toolchain_platform.get(_toolchain)
        if platform_name != 'phone':
            part_name = '{}_{}'.format(origin_part_name, platform_name)
        else:
            part_name = origin_part_name
        _sa_info['part_name'] = part_name
        _sa_info['origin_part_name'] = origin_part_name
        processed_sa_infos.append(_sa_info)
    return processed_sa_infos


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--variants-toolchain-file', required=True)
    parser.add_argument('--sa-profile-infos-file', required=False)
    parser.add_argument('--output-file', required=True)
    parser.add_argument('--depfile', required=True)
    args = parser.parse_args()

    depfiles = [args.variants_toolchain_file]
    variants_toolchain_info = read_json_file(args.variants_toolchain_file)
    if variants_toolchain_info is None:
        raise Exception(
            "read file '{}' failed.".format(args.variants_toolchain_file))
    toolchain_platform = variants_toolchain_info.get('toolchain_platform')
    processed_sa_infos = sa_info_process(args.sa_profile_infos_file,
                                         toolchain_platform)
    write_json_file(args.output_file, processed_sa_infos)
    build_utils.write_depfile(args.depfile, args.output_file, depfiles)
    return 0


if __name__ == '__main__':
    sys.exit(main())
