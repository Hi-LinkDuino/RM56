#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
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
#

import os
import sys
import importlib
import subprocess


def check_output(cmd, **kwargs):
    process = subprocess.Popen(cmd,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT,
                               universal_newlines=True,
                               **kwargs)
    for line in iter(process.stdout.readline, ''):
        sys.stdout.write(line)
        sys.stdout.flush()

    process.wait()
    ret_code = process.returncode

    return ret_code


def set_root_path(path):
    sys.path.insert(0, os.path.join(path, 'build/lite'))
    module = importlib.import_module('hb_internal.set.set')
    return module.set_root_path(root_path=path)


def build(path, args_list):
    cmd = ['python3', 'build/lite/hb/__main__.py', 'build'] + args_list
    return check_output(cmd, cwd=path)


def main():
    root_path = os.path.dirname(os.path.abspath(__file__))
    ret_code = set_root_path(root_path)
    if ret_code != 0:
        return ret_code
    return build(root_path, sys.argv[1:])


if __name__ == "__main__":
    sys.exit(main())
