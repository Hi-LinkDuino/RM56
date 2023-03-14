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

sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir))
from scripts.util import build_utils  # noqa: E402


def parse_args(args):
    parser = optparse.OptionParser()

    parser.add_option(
        '--generated-build-file',
        help='path to generated platform build file')
    parser.add_option(
        '--required-build-file', help='path to required platform build file')
    parser.add_option(
        '--optional-build-file', help='path to optional platform build file')
    parser.add_option(
        '--stub-build-file', help='path to stub platform build file')

    options, _ = parser.parse_args(args)
    return options


def do_merge(required_build_file, optional_build_file, stub_build_file,
             generated_build_file):
    with open(required_build_file, 'r') as required_f:
        required = json.load(required_f)
    with open(optional_build_file, 'r') as optional_f:
        optional = json.load(optional_f)
    with open(stub_build_file, 'r') as stub_f:
        stub = json.load(stub_f)

    parts = required.get('parts') + optional.get('parts')
    stub_parts = list(set(stub.get('stub_parts')) - set(parts))

    if not os.path.exists(generated_build_file):
        build_utils.touch(generated_build_file)
        build_utils.write_json({
            'parts': parts,
            'stub_parts': stub_parts,
        },
            generated_build_file,
            only_if_changed=True)


def main(args):
    options = parse_args(args)
    do_merge(options.required_build_file, options.optional_build_file,
             options.stub_build_file, options.generated_build_file)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
