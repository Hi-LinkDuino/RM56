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

sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir))
from scripts.util import build_utils  # noqa: E402

STUB_FUNCTION_TEMPLATE = '''
void {}() {{  }}
'''

STUB_VARIABLE_TEMPLATE = '''
int {} = 0;
'''


def parse_args(args):
    args = build_utils.expand_file_args(args)

    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)
    parser.add_option('--output', help='generated ndk stub file')
    parser.add_option('--ndk-description-file', help='ndk description file')

    options, _ = parser.parse_args(args)
    return options


def generate_stub_file(options):
    contents = []
    with open(options.ndk_description_file, 'r') as f:
        interfaces = json.load(f)
        for inf in interfaces:
            name = inf.get('name')
            if inf.get('type') == 'variable':
                contents.append(STUB_VARIABLE_TEMPLATE.format(name))
            else:
                contents.append(STUB_FUNCTION_TEMPLATE.format(name))
    with open(options.output, 'w') as f:
        f.write('\n'.join(contents))


def main(args):
    options = parse_args(args)

    depfile_deps = ([options.ndk_description_file])

    build_utils.call_and_write_depfile_if_stale(
        lambda: generate_stub_file(options),
        options,
        depfile_deps=depfile_deps,
        input_paths=depfile_deps,
        output_paths=([options.output]),
        force=False,
        add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
