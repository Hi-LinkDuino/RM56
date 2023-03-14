#!/usr/bin/env python3
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


def run(args):
    exist_targets = []
    for ndk_lib in args.ndk_lib_target_list:
        if not ndk_lib.startswith("//"):
            raise Exception(
                "ndk target '{}' configuration error.".format(ndk_lib))
        _build_gn_file = ndk_lib[len('//'):].split(':')[0]
        ndk_lib_def_file = os.path.realpath(
            os.path.join(args.source_root_dir, _build_gn_file))
        if not os.path.exists(ndk_lib_def_file):
            continue
        exist_targets.append(ndk_lib)
    return exist_targets


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--ndk-lib-target-list', nargs='+', required=True)
    parser.add_argument('--source-root-dir', required=True)
    args = parser.parse_args(argv)
    exist_targets = run(args)
    for _target in exist_targets:
        print(_target)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
