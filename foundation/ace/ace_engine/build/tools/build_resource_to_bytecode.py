#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2020 Huawei Device Co., Ltd.
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
import os
import sys


def resource_file_to_bytecode(input_dir, input_file, output_path):
    with open(os.path.join(input_dir, input_file), 'rb')\
            as resource_file_object:
        with open(output_path, 'a') as cpp_file_object:
            length = 0;
            all_the_content = resource_file_object.read();
            template0 = "#include <stdint.h>\n";
            template1 = "const uint8_t  _binary_$1_start[$2] = {$3};\n";
            template2 = \
                "const uint8_t* _binary_$1_end = _binary_$1_start + $2;";

            formats = ","
            seq = []
            for content in all_the_content:
                seq.append(str(hex(content)))
                length = length + 1
            byte_code = formats.join(seq);
            input_file = input_file.replace(".", "_")
            template1 = template1.replace("$1", str(input_file)) \
                .replace("$2", str(length)) \
                .replace("$3", str(byte_code))
            template2 = template2.replace("$1", str(input_file)) \
                .replace("$2", str(length))
            cpp_file_object.seek(0)
            cpp_file_object.truncate();
            cpp_file_object.write(template0 + template1 + template2);


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--objcopy', type=str, required=False)
    parser.add_argument('--input', type=str, required=True)
    parser.add_argument('--output', type=str, required=True)
    parser.add_argument('--arch', type=str, required=False)

    args = parser.parse_args()

    input_dir, input_file = os.path.split(args.input)
    output_path = os.path.abspath(args.output)
    resource_file_to_bytecode(input_dir, input_file, output_path)


if __name__ == '__main__':
    sys.exit(main())
