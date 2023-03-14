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
import shutil


def main(argv):

    #Check if build is release (flag provided from BUILD.gn)
    is_release = len(argv) > 3 and argv[3] == "--release"

    os.chdir(os.path.abspath(argv[1]))
    if not os.path.exists(os.path.abspath(argv[2])):
        subprocess.call(["npm", "install"])

    script = "build"
    if is_release:
        script = "build_release"

    subprocess.call(["npm", "run", script])

if __name__ == '__main__':
    main(sys.argv)
