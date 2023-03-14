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
import re
import tempfile
import distutils.spawn
import shutil

sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir))
from scripts.util import build_utils  # noqa: E402



def parse_args(args):
    args = build_utils.expand_file_args(args)

    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)
    parser.add_option('--version', help='software version')
    parser.add_option('--doxygen-file', help='doxygen config file')
    parser.add_option('--output', help='output index.html')
    parser.add_option('--record-path', help='path to md5.stamp file')
    parser.add_option(
        '--working-dir',
        help='the directory where doxygen command will be executed')

    options, _ = parser.parse_args(args)
    return options


def generate_ndk_docs(options, html_output_dir):
    contents = None
    with tempfile.NamedTemporaryFile(
            suffix=os.path.basename(options.doxygen_file)) as doxygen_file:
        shutil.copyfile(options.doxygen_file, doxygen_file.name)
        with open(doxygen_file.name, 'r') as f:
            contents = f.read()
        if contents is None:
            raise Exception('Failed to read %s' % options.doxygen_file)

        keys = {
            '%VERSION%':
                options.version,
            '%OUTPUT_DIR%':
                os.path.relpath(html_output_dir, options.working_dir)
        }
        for (k, v) in list(keys.items()):
            v = v.replace('\\', '\\\\')
            contents = re.sub(k, v, contents)

        with open(doxygen_file.name, 'w') as f:
            f.write(contents)

        old_cwd = os.getcwd()
        try:
            # if no ndk headers exist, return.
            if os.path.exists(options.working_dir) is not True:
                print("no ndk headers exist, return")
                return
            os.chdir(options.working_dir)

            doxygen_path = distutils.spawn.find_executable('doxygen')
            if doxygen_path is None:
                print(
                    "Warning: Failed to find doxygen, please install doxygen with \"sudo apt-get install doxygen\" on Ubuntu"
                )
                return
            os.makedirs(
                os.path.relpath(html_output_dir, options.working_dir),
                exist_ok=True)
            cmd = [doxygen_path, doxygen_file.name]
            build_utils.check_output(cmd)
        finally:
            os.chdir(old_cwd)


def main(args):
    options = parse_args(args)

    depfile_deps = ([options.doxygen_file])
    for root, _, filenames in os.walk(options.working_dir):
        for f in filenames:
            depfile_deps += ([os.path.join(root, f)])

    html_output_dir = options.output
    build_utils.call_and_write_depfile_if_stale(
        lambda: generate_ndk_docs(options, html_output_dir),
        options,
        depfile_deps=depfile_deps,
        input_paths=depfile_deps,
        input_strings=[options.version, options.working_dir],
        output_paths=([html_output_dir]),
        record_path=options.record_path,
        force=False,
        add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
