#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
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
#

import os
import sys
import subprocess


def main(argv):
    if len(argv) < 3:
        print("argv count error, return!")
        return
    input_file = argv[1] + "/all_plugin_static.h.in"
    output_file = argv[2] + "/all_plugin_static.h"
    static_declare = []
    static_register = []
    static_unregister = []
    if not os.path.isfile(input_file):
        print("input file is not exist, return!")
        return
    if not os.path.exists(argv[2]):
        os.makedirs(argv[2])
    for i in range(3, len(argv)):
        static_declare += "PLUGIN_REGISTER_STATIC_DECLARE(%s);" % argv[i]
        static_register += "REGISTER_STATIC(%s, reg);" % argv[i]
        static_unregister += "UNREGISTER_STATIC(%s);" % argv[i]
    infile = open(input_file, "r")
    outfile = open(output_file, "w")
    for line in infile:
        if '@HISTREAMER_PLUGIN_STATIC_DECLARE@' in line:
            outfile.write("".join(static_declare))
        elif '@HISTREAMER_PLUGIN_REGISTER_STATIC@' in line:
            outfile.write("(void)(reg);")
            outfile.write("".join(static_register))
        elif '@HISTREAMER_PLUGIN_UNREGISTER_STATIC@' in line:
            outfile.write("".join(static_unregister))
        else:
            outfile.write(line)
    infile.close()
    outfile.close()


if __name__ == '__main__':
    sys.exit(main(sys.argv))

