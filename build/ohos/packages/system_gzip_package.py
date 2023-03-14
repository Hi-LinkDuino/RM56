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
import shutil
import tarfile

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util import build_utils  # noqa: E402


def merge_image_files(src_image_path, dest_image_path):
    if not os.path.exists(src_image_path):
        raise Exception(
            "source image path {} does not exist.".format(src_image_path))

    for root, dirs, files in os.walk(src_image_path):
        for dir_name in dirs:
            src_dir = os.path.join(root, dir_name)
            dir_relpath = os.path.relpath(src_dir, src_image_path)
            dest_dir_path = os.path.join(dest_image_path, dir_relpath)
            if not os.path.exists(dest_dir_path):
                os.makedirs(dest_dir_path)

        for file_name in files:
            src_file_path = os.path.join(root, file_name)
            file_relpath = os.path.relpath(src_file_path, src_image_path)
            dest_file_path = os.path.join(dest_image_path, file_relpath)
            if not os.path.exists(dest_file_path):
                if not os.path.exists(os.path.dirname(dest_file_path)):
                    os.makedirs(os.path.dirname(dest_file_path))
                shutil.copy2(src_file_path, dest_file_path)


def compress_image_files(package_dir, output_file, additional_files):
    # Compress the image folder
    files = [package_dir] + additional_files
    with tarfile.open(output_file, "w:gz") as tar:
        for f in files:
            if os.path.exists(f):
                try:
                    # additional files will be packed outside of system dir.
                    if f in additional_files:
                        tar.add(f, arcname=os.path.basename(f))
                    else:
                        tar.add(f, arcname='system')
                except OSError as ioerr:
                    print("Compress file failed. Error code: {}".format(
                        ioerr.errno))
                except:
                    print("Unexpected error")
                    raise


def main(argv):
    argv = build_utils.expand_file_args(argv)
    parser = argparse.ArgumentParser()
    build_utils.add_depfile_option(parser)
    parser.add_argument('--image-dir', help='', required=True)
    parser.add_argument("--system-image-zipfile", required=True)
    parser.add_argument('--output-file', help='', required=True)
    parser.add_argument('--additional-files', help='', action='append')
    args = parser.parse_args(argv[1:])

    depfiles = [args.system_image_zipfile] + args.additional_files
    with build_utils.temp_dir() as img_dir:
        build_utils.extract_all(args.system_image_zipfile,
                                img_dir,
                                no_clobber=True)
        build_utils.call_and_write_depfile_if_stale(
            lambda: compress_image_files(img_dir, args.output_file, args.
                                         additional_files),
            args,
            depfile_deps=depfiles,
            input_paths=depfiles,
            output_paths=([args.output_file]),
            force=False,
            add_pydeps=False)


if __name__ == "__main__":
    main(sys.argv)
