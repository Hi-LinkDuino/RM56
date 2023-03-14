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

sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir))
from scripts.util import build_utils  # noqa: E402
from scripts.interface_mgr import InterfaceMgr  # noqa: E402


def parse_args(args):
    args = build_utils.expand_file_args(args)

    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)
    parser.add_option('--output', help='generated ndk stub file')
    parser.add_option('--headers',
                      action='append',
                      help='base directory of ndk common files')
    parser.add_option('--generated-signature',
                      help='base directory of os specific stuff')
    parser.add_option('--saved-signature',
                      help='prefix string of directory in archive zipfile')
    parser.add_option('--check-signature',
                      action='store_true',
                      help='check ndk signature')
    parser.add_option(
        '--root-build-dir',
        help='root build directory, used to strip relative address')

    options, _ = parser.parse_args(args)

    return options


def header_signature(output, headers, root_build_dir, saved, generated, check):
    signature = []
    mgr = InterfaceMgr()

    for f in headers:
        signature.append('//{} {}'.format(os.path.relpath(f, root_build_dir),
                                          mgr.get_file_sha256(f)))

    os.makedirs(os.path.dirname(generated), exist_ok=True)
    with open(generated, 'w') as g:
        g.write('\n'.join(sorted(signature)))

    if check and mgr.get_file_sha256(generated) != mgr.get_file_sha256(saved):
        raise Exception(
            "Error: ndk header signature changed. generated signature {} has "
            "different signature with saved signature {}"
            .format(generated, saved))

    build_utils.touch(output)


def main(args):
    options = parse_args(args)

    depfile_deps = set()
    if options.check_signature:
        depfile_deps.add(options.saved_signature)

    build_utils.call_and_write_depfile_if_stale(lambda: header_signature(
        options.output, options.headers, options.root_build_dir, options.
        saved_signature, options.generated_signature, options.check_signature),
                                                options,
                                                depfile_deps=depfile_deps,
                                                input_paths=depfile_deps,
                                                output_paths=([options.output
                                                               ]),
                                                input_strings=args,
                                                force=False,
                                                add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
