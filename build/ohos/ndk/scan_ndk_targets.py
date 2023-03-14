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

import argparse
import sys
import os
import subprocess
import fnmatch
import glob
import re
import errno
import codecs

sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir))
from scripts.util import build_utils  # noqa: E402,E501 pylint: disable=E0401,C0413

ALL_NDK_TARGETS_TEMPLATE = '''
group("all_ndk_targets") {{
 deps = [ {} ]
}}
'''

ALL_NDK_TEMPLATES_NAMES = [
    "ohos_ndk_headers", "ohos_ndk_library",
    "ohos_ndk_prebuilt_library", "ohos_ndk_copy"
]


def remove_comment(file):
    contents = []
    with open(file, 'r') as in_file:
        for line in in_file:
            # Strip comments in gn file.
            # If someone uses # as part of a string, ignore it.
            if re.match(r'.*?("\S*#\S*")', line):
                pass
            else:
                line = re.sub(r'(#.*?)\n', '', line)
            contents.append(line)
    return contents


def do_dos2unix(in_file, out_file):
    contents = ''
    with open(in_file, 'r+b') as fin:
        contents = fin.read()
    # Remove BOM header.
    if contents.startswith(codecs.BOM_UTF8):
        contents = contents[len(codecs.BOM_UTF8):]
    contents = re.sub(r'\r\n', '\n', contents.decode())
    with open(out_file, 'w') as fout:
        fout.write(contents)


def do_gn_format(gn_file, org_file):
    cmd = ['gn', 'format', gn_file]
    child = subprocess.Popen(cmd)
    child.communicate()
    if child.returncode:
        print(
            'Error: Something is wrong with {}, pleae check file encoding or format'
            .format(org_file))


def get_ndk_targets(file, options):
    ndk_targets = []
    with build_utils.temp_dir() as tmp:
        gn_file = os.path.join(tmp, os.path.basename(file))
        do_dos2unix(file, gn_file)
        do_gn_format(gn_file, file)
        contents = remove_comment(gn_file)
        for template_name in ALL_NDK_TEMPLATES_NAMES:
            pattern = re.escape(template_name) + r"\(\"(.*)\"\)"
            targets = re.findall(pattern, ''.join(contents))
            for target in targets:
                ndk_targets.append('\"//{}:{}\",'.format(
                    os.path.relpath(os.path.dirname(file), options.root_dir),
                    target))
    return ndk_targets


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', required=True)
    parser.add_argument('--root-dir', required=True)

    options = parser.parse_args()

    paths = glob.glob(os.path.join(options.root_dir, "*"))
    dirs = [
        d for d in paths if os.path.isdir(d)
        and not fnmatch.fnmatch(d, os.path.join(options.root_dir, ".repo"))
        and not fnmatch.fnmatch(d, os.path.join(options.root_dir, "out"))
    ]

    gn_list = []
    for d in dirs:
        gn_list += glob.glob(os.path.join(d, "**/BUILD.gn"), recursive=True)

    ndk_targets = []
    for gn_file in gn_list:
        # Skip dead link.
        try:
            os.stat(gn_file)
        except OSError as err:
            if err.errno == errno.ENOENT:
                continue
            else:
                raise Exception("Error: failed to stat {}".format(gn_file))
        ndk_targets.extend(get_ndk_targets(gn_file, options))

    ndk_contents = ALL_NDK_TARGETS_TEMPLATE.format('\n'.join(ndk_targets))

    os.makedirs(os.path.dirname(options.output), exist_ok=True)
    with open(options.output, 'w') as f:
        f.write(ndk_contents)

    # Call gn format to make the output gn file prettier.
    cmd = ['gn', 'format', options.output]
    subprocess.check_output(cmd)
    return 0


if __name__ == '__main__':
    sys.exit(main())
