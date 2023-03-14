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

import argparse
import sys
import os


def is_exists(filename, type):
    result = False
    if type == 'file':
        result = os.path.exists(filename) and os.path.isfile(filename)
    elif type == 'dir':
        result = os.path.exists(filename) and os.path.isdir(filename)
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--filename', required=True)
    parser.add_argument('--type', default='file')
    args = parser.parse_args()
    result = is_exists(args.filename, args.type)
    sys.stdout.write(str(result))
    return 0


if __name__ == '__main__':
    sys.exit(main())
