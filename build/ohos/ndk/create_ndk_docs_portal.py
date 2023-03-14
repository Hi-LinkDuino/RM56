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

PORTAL_CONTENTS = """
<meta http-equiv="REFRESH" content="0;URL=%s">
"""


def parse_args(args):
    args = build_utils.expand_file_args(args)

    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)
    parser.add_option('--portal-path', help='path to portal html')
    parser.add_option('--doxygen-output', help='ndk doxygen outputs')
    parser.add_option('--record-path', help='path to md5.stamp file')
    parser.add_option('--docs-archive', help='path of docs archive zipfile')
    parser.add_option(
        '--archive-doc',
        default=False,
        action='store_true',
        help='whether to archive doc or not')

    options, _ = parser.parse_args(args)
    return options


def write_portal_and_archive(options):
    # If user doesn't have doxygen installed, then no document is generated,
    # no need to generate portal html
    if not os.path.exists(options.doxygen_output):
        return

    contents = [
        PORTAL_CONTENTS % os.path.relpath(options.doxygen_output,
                                          os.path.dirname(options.portal_path))
    ]
    with open(options.portal_path, 'w') as f:
        f.write('\n'.join(contents))

    if options.archive_doc:
        os.makedirs(os.path.dirname(options.docs_archive), exist_ok=True)
        build_utils.zip_dir(
            options.docs_archive,
            os.path.dirname(options.portal_path),
            compress_fn=lambda _: True)


def main(args):
    options = parse_args(args)

    depfile_deps = set()
    if os.path.exists(options.doxygen_output):
        depfile_deps.add(options.doxygen_output)
    outputs = [options.portal_path]
    if options.docs_archive:
        outputs.append(options.docs_archive)

    build_utils.call_and_write_depfile_if_stale(
        lambda: write_portal_and_archive(options),
        options,
        depfile_deps=depfile_deps,
        input_paths=depfile_deps,
        input_strings=[options.archive_doc],
        output_paths=(outputs),
        record_path=options.record_path,
        force=False,
        add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
