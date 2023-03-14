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
import zipfile

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util import build_utils  # noqa: E402


def main():
    parser = argparse.ArgumentParser()
    build_utils.add_depfile_option(parser)

    parser.add_argument('--sdk-notice-file', required=True)
    parser.add_argument('--sdk-archive-dir', default=None)
    parser.add_argument('--output', required=True)
    parser.add_argument('--skip-pattern', required=True)

    options = parser.parse_args()

    sdk_archives = []
    for root, _, files in os.walk(options.sdk_archive_dir):
        sdk_archives.extend(
            [os.path.join(root, f) for f in files if f.endswith('.zip')])
    for archive in sdk_archives:
        if options.skip_pattern in archive:
            continue
        with zipfile.ZipFile(archive, 'a') as zip_file, open(
            options.sdk_notice_file) as notice:
            if not zip_file.namelist():
                continue
            dirname = zip_file.namelist()[0].split('/')[0]
            arcname = os.path.join(dirname,
                                   os.path.basename(options.sdk_notice_file))
            if arcname in zip_file.namelist():
                print("Warning: {} is already in {}".format(arcname, archive))
                continue
            zip_info = zipfile.ZipInfo(filename=arcname,
                                       date_time=build_utils.HERMETIC_TIMESTAMP)
            zip_file.writestr(zip_info,
                              notice.read(),
                              compress_type=zipfile.ZIP_STORED)
    build_utils.touch(options.output)

    build_utils.write_depfile(options.depfile,
                             options.output,
                             sdk_archives,
                             add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main())
