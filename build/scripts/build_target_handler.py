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
import ninja_rules_parser

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util.file_utils import read_json_file  # noqa E402


def _read_parts_variants(build_configs_dir):
    parts_variants_file = os.path.join(build_configs_dir, 'parts_info',
                                       'parts_variants.json')
    if not os.path.exists(parts_variants_file):
        raise Exception("file '{}' doesn't exist.".format(parts_variants_file))
    parts_variants = read_json_file(parts_variants_file)
    if parts_variants is None:
        raise Exception("read file '{}' failed.".format(parts_variants_file))
    return parts_variants


def get_build_target(build_dir, build_targets, target_platform):
    if not target_platform or target_platform == 'all':
        target_platform = 'phone'
    real_targets = []
    build_configs_dir = os.path.join(build_dir, 'build_configs')
    parts_variants = _read_parts_variants(build_configs_dir)
    for _target_name in build_targets:
        if _target_name in parts_variants:
            if target_platform == 'phone':
                real_targets.append(_target_name)
            else:
                variants = parts_variants.get(_target_name)
                if target_platform in variants:
                    real_targets.append('{0}/{1}_{0}'.format(
                        target_platform, _target_name))
                else:
                    real_targets.append(_target_name)
        else:
            if target_platform == 'phone':
                real_targets.append(_target_name)
            else:
                real_targets.append('{0}/{1}'.format(target_platform,
                                                     _target_name))
    return real_targets


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--source-root-dir', required=True)
    parser.add_argument('--root-build-dir', required=True)
    parser.add_argument('--build-target-name', nargs='*')
    parser.add_argument('--target-platform', required=False)
    args = parser.parse_args()

    source_root_dir = args.source_root_dir
    if not os.path.exists(os.path.join(source_root_dir, '.gn')):
        print('source root dir incorrect.')
        return 1

    build_dir = os.path.join(source_root_dir, args.root_build_dir)
    # rewrite build.log
    ninja_rules_parser.update(
        build_dir, 'build_configs/platforms_info/toolchain_to_variant.json')

    if args.target_platform is None or args.target_platform == 'all':
        target_platform = None
    else:
        target_platform = args.target_platform
    # process build targets
    build_targets = get_build_target(build_dir, args.build_target_name,
                                     target_platform)
    # output build_targets string, be used on the ninja command line
    print(' '.join(build_targets))
    return 0


if __name__ == '__main__':
    sys.exit(main())
