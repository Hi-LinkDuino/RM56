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
import hashlib
import argparse


class InterfaceMgr:
    __max_buf = 1024 * 1024

    def get_file_sha256(self, filename):
        hash_value = None
        if os.path.isfile(filename):
            sha256obj = hashlib.sha256()
            try:
                with open(filename, 'rb') as f:
                    while True:
                        buf = f.read(self.__max_buf)
                        if not buf:
                            break
                        sha256obj.update(buf)
                    hash_value = sha256obj.hexdigest()
            except OSError as err:
                sys.stdout.write("read file failed. {}".format(err))
                return ""
        return str(hash_value)

    def get_header_files(self, file_dir):
        h_files = []
        for path, _, files in os.walk(file_dir):
            for file in files:
                if file.endswith('.h'):
                    x_file = os.path.relpath(os.path.join(path, file),
                                             file_dir)
                    h_files.append(x_file)
        return h_files

    def gen_sig_file_by_subsystem(self, subsystem_sdk_out_dir,
                                  sig_file_gen_dir):
        if not os.path.exists(subsystem_sdk_out_dir) or not os.path.isdir(
                subsystem_sdk_out_dir):
            raise Exception(
                "subsystem sdk out dir '{}' not exist or not dir".format(
                    subsystem_sdk_out_dir))

        module_list = os.listdir(subsystem_sdk_out_dir)
        for module_name in module_list:
            module_dir = os.path.join(subsystem_sdk_out_dir, module_name)
            if not os.path.exists(module_dir) or not os.path.isdir(module_dir):
                continue
            header_files = self.get_header_files(module_dir)
            if not header_files:
                continue
            check_content = []
            for h_file in header_files:
                file_sha256 = self.get_file_sha256(
                    os.path.join(module_dir, h_file))
                check_content.append('{} {}'.format(h_file, file_sha256))

            check_file = os.path.join(sig_file_gen_dir, module_name,
                                      'check.txt')
            file_dir = os.path.dirname(os.path.abspath(check_file))
            if not os.path.exists(file_dir):
                os.makedirs(file_dir)

            # sort check file content
            check_content.sort()
            with open(check_file, 'w') as output_file:
                output_file.write('\n'.join(check_content))
                output_file.flush()

    def _gen_checkfile(self, check_file_dir, target_type_dir, target_type):
        subsystem_list = os.listdir(target_type_dir)
        for subsystem_name in subsystem_list:
            subsystem_dir = os.path.join(target_type_dir, subsystem_name)
            if not os.path.isdir(subsystem_dir) or target_type.startswith(
                    '.'):
                continue
            self.gen_sig_file_by_subsystem(subsystem_name, check_file_dir)

    def gen_interface_checkfile(self, sdk_base_dir, check_file_dir):
        if not os.path.isdir(sdk_base_dir):
            raise Exception(
                'sdk base dir [{}] does not exist.'.format(sdk_base_dir))

        target_type_allowlist = ['ohos-arm64']
        target_types = os.listdir(sdk_base_dir)
        for target_type in target_types:
            target_type_dir = os.path.join(sdk_base_dir, target_type)
            if not os.path.isdir(target_type_dir) or target_type.startswith(
                    '.'):
                continue

            if target_type == 'java':
                continue

            if target_type not in target_type_allowlist:
                raise Exception('target type is incorrect.')

            self._gen_checkfile(check_file_dir, target_type_dir, target_type)

    def check(self, check_file_dir, subsystem_sdk_dir, subsystem_name,
              module_name):
        check_file = os.path.join(check_file_dir, subsystem_name, module_name,
                                  'check.txt')
        if not os.path.isfile(check_file):
            raise Exception(
                '[{}:{}] interface check failed. file [{}] does not exist.'.
                format(subsystem_name, module_name, check_file))

        check_value = {}
        with open(check_file, 'r') as f:
            for line in f.readlines():
                values = line.rstrip('\n').split(' ')
                check_value[values[0]] = values[1]

        module_dir = os.path.join(subsystem_sdk_dir, module_name)
        header_files = self.get_header_files(module_dir)
        if len(check_value) != len(header_files):
            raise Exception(('[{}:{}] interface check failed. '
                             'the number of files is different.').format(
                                 subsystem_name, module_name))

        for h_file in header_files:
            file_sha256 = self.get_file_sha256(os.path.join(
                module_dir, h_file))
            if check_value.get(h_file) != file_sha256:
                raise Exception(
                    '[{}:{}] interface check failed. file [{}] has changed.'.
                    format(subsystem_name, module_name, h_file))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--generate', dest='generate', action='store_true')
    parser.set_defaults(generate=False)
    parser.add_argument('--sdk-base-dir', help='', required=True)
    parser.add_argument('--check_file_dir', help='', required=True)
    args = parser.parse_args()

    if args.generate:
        interface_mgr = InterfaceMgr()
        interface_mgr.gen_interface_checkfile(args.sdk_base_dir,
                                              args.check_file_dir)
    else:
        sys.stdout.write('Usage: interface_mgr.py --generate '
                         '--sdk-base-dir SDK_BASE_DIR '
                         '--check_file_dir CHECK_FILE_DIR')
        sys.stdout.flush()
    return 0


if __name__ == '__main__':
    sys.exit(main())
