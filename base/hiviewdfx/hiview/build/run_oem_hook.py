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
import runpy
import sys


DEBUG = False


def find_oem_script(path):
    for relpath, dirs, files in os.walk(path):
        if "oem_hook.py" in files:
            script_path = os.path.join(path, relpath, "oem_hook.py")
            return os.path.normpath(os.path.abspath(script_path))
    return ""


def main():
    if DEBUG:
        print("Run oem hook start")
    hiview_build_path = os.path.split(os.path.realpath(__file__))[0]
    hiview_path = os.path.realpath(os.path.join(hiview_build_path, ".."))
    script_path = find_oem_script(hiview_path)
    if script_path.strip() == '':
        print("Could not find oem hook")
    else:
        runpy.run_path(script_path, run_name='__main__')

    if DEBUG:
        print("Run oem hook end")
    return 0


if __name__ == '__main__':
    sys.exit(main())
