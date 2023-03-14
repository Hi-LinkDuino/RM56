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
import subprocess


def main(argv):
    if len(argv) != 3: # this is called by build system, must has 3 args.
        print("argv count error, return!")
        return

    path = argv[1]
    folder = os.path.exists(path)
    if not folder:
        os.makedirs(path)
        print("make folder OK")
    else:
        print("folder already existed")

    symlink_path = argv[2]
    src_path = os.path.abspath(path)
    print("skia path is : " + src_path)
    dst_path = os.path.abspath(symlink_path)
    print("symlink path is: " + dst_path)
    subprocess.run(['ln', '-s', dst_path, src_path])

if __name__ == '__main__':
    main(sys.argv)
