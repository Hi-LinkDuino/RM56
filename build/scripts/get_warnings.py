#!/usr/bin/env python3
#-*- coding: UTF-8 -*-
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

"""

Usage: get_warnings.py --build-log-file out/phone-release/build.log \
               --warning-out-file out/phone-release/warning_list.txt

Generate the project notice files, including both text and xml files.

"""

import argparse
import os
import sys
import re
import subprocess


def _do_uniq(infile, outfile):
    subprocess.call(['sort', '-u', infile, '-o', outfile], shell=False)


def _pick_line(line_info):
    result = False
    parser_keys = [
        ": Warning", ": warning", "warning:", "Warning:", "WARNING:"
    ]
    for _key in parser_keys:
        if len(re.findall(_key, line_info, re.S)) >= 1:
            result = True
            break
    return result


def _parse(in_name, out_name, prj_dir):
    if not os.path.exists(in_name):
        print("warning: build log file {} is not exists.".format(in_name))
        return False

    with open(in_name, "r", encoding='utf-8', errors='ignore') as in_fd:
        os.makedirs(os.path.dirname(out_name), exist_ok=True)
        with open(out_name, "w") as out_fd:
            while True:
                line_info = in_fd.readline()
                line_info = line_info.replace("\r", "")
                if line_info == "":
                    break
                # Solve the non-standard printing in kernel compilation --begin
                if (line_info.find("kernel/linux-") >
                        1) and (": warning:" in line_info):
                    line_info = line_info[line_info.find("kernel/linux-"):]
                # Solve the non-standard printing in kernel compilation --end
                if _pick_line(line_info):
                    while True:
                        if line_info.startswith("../"):
                            line_info = line_info[3:]
                        elif line_info.startswith("./"):
                            line_info = line_info[2:]
                        elif line_info.startswith("\""):
                            line_info = line_info[1:]
                        elif line_info.startswith(":"):
                            line_info = line_info[1:]
                        elif line_info.startswith(" "):
                            line_info = line_info[1:]
                        else:
                            break
                    # solving relative path
                    templist = line_info.split(":")
                    templist[0] = os.path.abspath(templist[0])
                    templist[0] = templist[0].replace(prj_dir + "/",
                                                      "").strip()
                    temp = ":"
                    line_info = temp.join(templist)

                    out_fd.write(line_info)
                    out_fd.write("\r\n")
    return True


def _get_warn(log_file, warn_file, prj_dir):
    if not os.path.exists(os.path.dirname(warn_file)):
        os.makedirs(os.path.dirname(warn_file))
    if os.path.exists(warn_file):
        os.remove(warn_file)
    temp_out_file = os.path.join(os.path.dirname(warn_file),
                                 'temp_warning.txt')
    result = _parse(log_file, temp_out_file, prj_dir)
    if result:
        _do_uniq(temp_out_file, warn_file)
    # delete temp file
    if os.path.exists(temp_out_file):
        os.remove(temp_out_file)


def main(argv):
    """parse warning info from build log."""
    parser = argparse.ArgumentParser()
    parser.add_argument('--build-log-file', help='log file', required=True)
    parser.add_argument('--warning-out-file',
                        help='result file',
                        required=True)
    args = parser.parse_args(argv)

    log_file = args.build_log_file
    warn_file = args.warning_out_file
    prj_dir = os.getcwd()
    _get_warn(log_file, warn_file, prj_dir)
    return 0


if __name__ == "__main__":
    main(sys.argv[1:])
