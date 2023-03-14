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
import shutil
import tarfile
import optparse
from util import build_utils

sys.path.append(
    os.path.abspath(os.path.dirname(os.path.abspath(
        os.path.dirname(__file__)))))
from scripts.util.file_utils import read_json_file  # noqa: E402

RELEASE_FILENAME = 'README.OpenSource'
scan_dir_list = ['third_party', 'kernel', 'device', 'drivers']


def _copy_opensource_file(opensource_config_file, top_dir, package_dir):
    if not os.path.exists(opensource_config_file):
        print("Warning, the opensource config file is not exists.")
        return False

    src_dir = os.path.dirname(opensource_config_file)
    dst_dir = os.path.join(package_dir, os.path.relpath(src_dir, top_dir))

    # copy opensource folder to out dir
    if os.path.exists(dst_dir):
        shutil.rmtree(dst_dir)
    shutil.copytree(src_dir,
                    dst_dir,
                    symlinks=True,
                    ignore=shutil.ignore_patterns('*.pyc', 'tmp*', '.git*'))

    # delete the README.OpenSource file
    release_file = os.path.join(dst_dir, RELEASE_FILENAME)
    os.remove(release_file)
    return True


def _parse_opensource_file(opensource_config_file):
    if not os.path.exists(opensource_config_file):
        print("Warning, the opensource config file is not exists.")
        return False

    opensource_config = read_json_file(opensource_config_file)
    if opensource_config is None:
        raise Exception("read opensource config file [{}] failed.".format(
            opensource_config_file))

    result = False
    for info in opensource_config:
        _license = info.get('License')
        if _license.count('GPL') > 0 or _license.count('LGPL') > 0:
            result = True
    return result


def _scan_and_package_code_release(scan_dir, top_dir, package_dir):
    file_dir_names = os.listdir(scan_dir)
    for file_dir_name in file_dir_names:
        file_dir_path = os.path.join(scan_dir, file_dir_name)
        if os.path.isdir(file_dir_path):
            _scan_and_package_code_release(file_dir_path, top_dir, package_dir)
        elif file_dir_path == os.path.join(scan_dir, RELEASE_FILENAME):
            if _parse_opensource_file(file_dir_path):
                _copy_opensource_file(file_dir_path, top_dir, package_dir)


def _tar_opensource_package_file(options, package_dir):
    result = -1
    if os.path.exists(package_dir):
        try:
            with tarfile.open(options.output, "w:gz") as tar:
                tar.add(package_dir, arcname=".")
                result = 0
        except IOError as err:
            raise err
    return result


def main(args):
    """generate open source packages to release."""
    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)
    parser.add_option('--output', help='output')
    parser.add_option('--root-dir', help='source root directory')
    options, _ = parser.parse_args(args)

    # get the source top directory to be scan
    top_dir = options.root_dir

    with build_utils.temp_dir() as package_dir:
        # scan the target dir and copy release code to out/opensource dir
        dir_list = [os.path.join(top_dir, _dir) for _dir in scan_dir_list]
        for scan_dir in dir_list:
            _scan_and_package_code_release(scan_dir, top_dir, package_dir)

        # package the opensource to Code_Opensource.tar.gz
        if _tar_opensource_package_file(options, package_dir) == 0:
            print('Generate the opensource package successfully.')
        else:
            print('Generate the opensource package failed.')

    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
