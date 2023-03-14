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

import json
import os
import subprocess
import hashlib


# Read json file data
def read_json_file(input_file):
    if not os.path.exists(input_file):
        print("file '{}' doesn't exist.".format(input_file))
        return None

    data = None
    try:
        with open(input_file, 'r') as input_f:
            data = json.load(input_f)
    except json.decoder.JSONDecodeError:
        print("The file '{}' format is incorrect.".format(input_file))
        raise
    except:  # noqa E722
        print("read file '{}' failed.".format(input_file))
        raise
    return data


# Read file by line
def read_file(input_file):
    if not os.path.exists(input_file):
        print("file '{}' doesn't exist.".format(input_file))
        return None

    data = []
    try:
        with open(input_file, 'r') as file_obj:
            for line in file_obj.readlines():
                data.append(line.rstrip('\n'))
    except:  # noqa E722
        print("read file '{}' failed".format(input_file))
        raise
    return data


# Write json file data
def write_json_file(output_file, content, check_changes=False):
    file_dir = os.path.dirname(os.path.abspath(output_file))
    if not os.path.exists(file_dir):
        os.makedirs(file_dir, exist_ok=True)

    if check_changes is True:
        changed = __check_changes(output_file, content)
    else:
        changed = True
    if changed is True:
        with open(output_file, 'w') as output_f:
            json.dump(content, output_f, sort_keys=True, indent=2)


def __check_changes(output_file, content):
    if os.path.exists(output_file) and os.path.isfile(output_file):
        # file content md5 val
        sha256_obj = hashlib.sha256()
        sha256_obj.update(str(read_json_file(output_file)).encode())
        hash_value = sha256_obj.hexdigest()
        # new content md5 val
        sha256_obj_new = hashlib.sha256()
        sha256_obj_new.update(str(content).encode())
        hash_value_new = sha256_obj_new.hexdigest()
        if hash_value_new == hash_value:
            return False
    return True


# Write file data
def write_file(output_file, content):
    file_dir = os.path.dirname(os.path.abspath(output_file))
    if not os.path.exists(file_dir):
        os.makedirs(file_dir, exist_ok=True)

    with open(output_file, 'w') as output_f:
        output_f.write(content)
    if output_file.endswith('.gni') or output_file.endswith('.gn'):
        # Call gn format to make the output gn file prettier.
        cmd = ['gn', 'format']
        cmd.append(output_file)
        subprocess.check_output(cmd)
