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
import argparse

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import write_json_file  # noqa: E402


def _generate_info_file(part_name, target_label, sa_info_files,
                        out_info_file):
    info = {
        'part_name': part_name,
        'label': target_label,
        'sa_info_files': sa_info_files
    }
    write_json_file(out_info_file, info)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--sa-input-files', nargs='+', required=True)
    parser.add_argument('--target-label', required=True)
    parser.add_argument('--output-module-info-file', required=True)
    parser.add_argument('--part-name', required=True)
    args = parser.parse_args()
    _generate_info_file(args.part_name, args.target_label,
                        args.sa_input_files, args.output_module_info_file)
    return 0


if __name__ == '__main__':
    sys.exit(main())
