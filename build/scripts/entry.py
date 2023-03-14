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

import os
import sys
import optparse
import subprocess


def check_output(cmd, **kwargs):
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT,
                               universal_newlines=True,
                               **kwargs)
    for line in iter(process.stdout.readline, ''):
        sys.stdout.write(line)
        sys.stdout.flush()
    process.wait()
    return process.returncode


def warning(msg):
    print(f'\033[93m{msg}\033[0m')


def do_build(args):
    build_py = os.path.join(args.source_root_dir, 'build.py')
    cmd = [
        'python3',
        build_py,
        '-p',
        args.product_name,
    ]
    build_targets = args.build_target
    gn_args = args.gn_args

    if args.product_name == 'ohos-sdk':
        gn_args.append('build_ohos_ndk=true')
        gn_args.append('build_ohos_sdk=true')
        build_targets.append('build_ohos_sdk')

    if args.build_only_gn:
        cmd.append('--build-only-gn')

    if args.sparse_image:
        gn_args.append('sparse_image=true')

    for item in args.export_para:
        key, value = item.split(':')
        if key == 'PYCACHE_ENABLE' and value == 'true':
            gn_args.append('pycache_enable=true')

    if build_targets:
        cmd.append('-T')
        for target in build_targets:
            cmd.append(target)

    if gn_args:
        cmd.extend(['--gn-args', ' '.join(gn_args)])
    if args.ninja_args:
        warning('--ninja-args is obsoleted, '
                'please use --verbose or --keep-ninja-going instead')
    if args.verbose:
        cmd.append('-v')
    if args.keep_ninja_going:
        cmd.append('--keep-ninja-going')
    try:
        return check_output(cmd)
    except KeyboardInterrupt:
        print('User abort')
        return -1


def main():
    parser = optparse.OptionParser()
    parser.add_option('--source-root-dir')
    parser.add_option('--product-name')
    parser.add_option('--device-name')
    parser.add_option('--target-cpu')
    parser.add_option('--target-os')
    parser.add_option('-T', '--build-target', action='append', default=[])
    parser.add_option('--gn-args', action='append', default=[])
    parser.add_option('--ninja-args', action='append', default=[])
    parser.add_option('-v', '--verbose', action='store_true')
    parser.add_option('--keep-ninja-going', action='store_true')
    parser.add_option('--sparse-image', action='store_true')
    parser.add_option('--jobs')
    parser.add_option('--export-para', action='append', default=[])
    parser.add_option('--build-only-gn', action='store_true')
    parser.add_option('--ccache', action='store_true')
    args, _ = parser.parse_args()

    if args.source_root_dir is None:
        print('Error: source_root_dir must be provided to entry.py')
        return -1
    if args.product_name is None:
        print('Error: product_name must be provided to entry.py')
        return -1

    return do_build(args)


if __name__ == '__main__':
    sys.exit(main())
