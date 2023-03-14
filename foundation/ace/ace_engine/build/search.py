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


def main():
    if (len(sys.argv) != 2):
        sys.stderr.write("MUST have 1 parameter for the searching path")
        sys.stderr.write(os.linesep)
        exit(os.errno.EINVAL)

    base_dir = os.path.realpath(sys.argv[1])
    if (not os.path.isdir(base_dir)):
        sys.stderr.write("Searching path MUST be a directory")
        sys.stderr.write(os.linesep)
        exit(os.errno.EINVAL)

    for item in os.listdir(base_dir):
        if not os.path.isdir(os.path.join(base_dir, item)):
            continue

        file_path = os.path.join(base_dir, item, "build", "platform.gni")
        if not os.path.isfile(file_path):
            continue

        sys.stdout.write(item)
        sys.stdout.write(os.linesep)

if __name__ == "__main__":
    main()
