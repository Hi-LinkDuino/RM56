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

import optparse
import os
import sys
import json
sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util import build_utils  # noqa: E402


def _parse_args(args):
    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)
    parser.add_option('--output_dir', help='output directory')
    parser.add_option('--source_dir', help='source directory')
    parser.add_option('--target', help='name of target')
    parser.add_option('--target_label')
    parser.add_option('--test_type')
    parser.add_option('--module_list_file', help='file name of module list')
    options, _ = parser.parse_args(args)
    build_utils.check_options(options,
                             parser,
                             required=('output_dir', 'target', 'source_dir',
                                       'target_label', 'module_list_file'))
    return options, parser


def main(args):
    options, _ = _parse_args(args)
    if not os.path.exists(os.path.dirname(options.module_list_file)):
        os.makedirs(os.path.dirname(options.module_list_file), exist_ok=True)
    with open(options.module_list_file, 'w') as f:
        contents = json.dumps([{
            'target': options.target,
            'label': options.target_label,
            'source_directory': options.source_dir,
            'output_directory': options.output_dir,
            'test_type': options.test_type
        }])
        f.write(contents)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
