#!/usr/bin/env python3
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
import argparse
import os
import hashlib
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
import file_utils  # noqa: E402
from scripts.util import build_utils  # noqa: E402

__MAX_BUF = 1024 * 1024


def _gen_signature(input_file):
    if not os.path.isfile(input_file):
        raise Exception()
    hash_value = ''
    sha256obj = hashlib.sha256()
    try:
        with open(input_file, 'rb') as file_obj:
            while True:
                buf = file_obj.read(__MAX_BUF)
                if not buf:
                    break
                sha256obj.update(buf)
            hash_value = sha256obj.hexdigest()
    except OSError as err:
        sys.stdout.write("read file failed. {}".format(err))
    return hash_value


def _write_signature_file(signature_file, hash_value):
    if os.path.exists(signature_file):
        os.remove(signature_file)
    file_utils.write_file(signature_file, hash_value)


def _update_signature(signature_file, new_hash_value):
    if os.path.exists(signature_file):
        data = file_utils.read_file(signature_file)
        if data is None:
            raise Exception(
                "read signatrue file '{}' failed.".format(signature_file))
        old_value = data[0]
        if old_value is None or old_value == '':
            raise Exception(
                "signatrue file '{}' content error.".format(signature_file))
        if old_value == new_hash_value:
            return
    _write_signature_file(signature_file, new_hash_value)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--input-dir', required=True)
    parser.add_argument('--output-zipfile', required=True)
    parser.add_argument('--signature-file', required=True)
    args = parser.parse_args()

    if os.path.exists(args.output_zipfile):
        os.remove(args.output_zipfile)
    build_utils.zip_dir(args.output_zipfile, args.input_dir)
    if not os.path.exists(args.output_zipfile):
        raise Exception("generate zipfile '{}' failed.".format(
            args.output_zipfile))

    hash_value = _gen_signature(args.output_zipfile)
    _update_signature(args.signature_file, hash_value)
    return 0


if __name__ == '__main__':
    sys.exit(main())
