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

import os
import sys
import argparse

sys.path.append(
    os.path.dirname(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from scripts.util.file_utils import write_json_file  # noqa E402


def get_allowlist():
    return ['hilog_java']


def get_kits_signature_list(sign_file_root_dir, subsystem_name,
                            sign_file_name):
    kits_signature_list = []
    subsystem_sign_file_dir = os.path.join(sign_file_root_dir, subsystem_name)
    if not os.path.exists(subsystem_sign_file_dir):
        return kits_signature_list
    kits_module_list = os.listdir(subsystem_sign_file_dir)
    for module_name in kits_module_list:
        signature_file = os.path.join(subsystem_sign_file_dir, module_name,
                                      sign_file_name)
        if os.path.exists(signature_file):
            kits_signature_list.append(module_name)
    return kits_signature_list


def check_remove(sign_file_root_dir, subsystem_name, kit_list, kit_type):
    if not os.path.exists(sign_file_root_dir):
        return True

    sign_file_name = 'signature'
    if kit_type == 'so':
        sign_file_name = 'check.txt'

    kits_signature_list = get_kits_signature_list(sign_file_root_dir,
                                                  subsystem_name,
                                                  sign_file_name)

    if len(kits_signature_list) != len(kit_list):
        return False

    for kits_signature_module in kits_signature_list:
        if kits_signature_module not in kit_list:
            return False
    return True


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--subsystem-name', required=True)
    parser.add_argument('--sign-file-root-dir', required=True)
    parser.add_argument('--kit-list', nargs='+', default=[])
    parser.add_argument('--output-file', required=True)
    parser.add_argument('--kit-type', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args()

    kit_list = args.kit_list
    for _kit in get_allowlist():
        if _kit in kit_list:
            kit_list.remove(_kit)

    result = check_remove(args.sign_file_root_dir, args.subsystem_name,
                          kit_list, args.kit_type)
    if not result:
        raise Exception(
            "Error, part '{}' kit remove, please check kit config.".
            format(args.subsystem_name))
    write_json_file(args.output_file, {})
    return 0


if __name__ == '__main__':
    sys.exit(main())
