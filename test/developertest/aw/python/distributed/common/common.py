#!/usr/bin/env python3
# coding=utf-8

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


##############################################################################
##############################################################################

def get_result_dir(testsuit_path):
    result_rootpath = os.environ.get('PYTEST_RESULTPATH')
    findkey = os.sep + "tests" + os.sep
    filedir, _ = os.path.split(testsuit_path)
    pos = filedir.find(findkey)
    if -1 != pos:
        subpath = filedir[pos + len(findkey):]
        pos1 = subpath.find(os.sep)
        if -1 != pos1:
            subpath = subpath[pos1 + len(os.sep):]
            result_path = os.path.join(result_rootpath, "result", subpath)
        else:
            result_path = os.path.join(result_rootpath, "result")
    else:
        result_path = os.path.join(result_rootpath, "result")

    if not os.path.exists(result_path):
        os.makedirs(result_path)

    return result_path


def get_resource_dir(phone_res_dir, device_type_name):
    if device_type_name.startswith("PHONE"):
        product_form_name = "phone"
    elif device_type_name.startswith("IVI"):
        product_form_name = "ivi"
    elif device_type_name.startswith("TV"):
        product_form_name = "intellitv"
    elif device_type_name.startswith("WATCH"):
        product_form_name = "wearable"
    else:
        product_form_name = "phone"

    pos = phone_res_dir.find(os.sep + "tests" + os.sep)
    if pos != -1:
        prefix_path = phone_res_dir[:pos]
        suffix_path = phone_res_dir[pos+1:]
        prefix_path = os.path.abspath(os.path.dirname(prefix_path))
        current_dir = os.path.join(prefix_path, product_form_name,
            suffix_path)
        if not os.path.exists(current_dir):
            current_dir = phone_res_dir
    else:
        current_dir = phone_res_dir
    return current_dir


def create_empty_result_file(savepath, filename, message=""):
    message = str(message)
    message = message.replace("\"", "")
    message = message.replace("<", "")
    message = message.replace(">", "")
    message = message.replace("&", "")
    if filename.endswith(".hap"):
        filename = filename.split(".")[0]
    if not os.path.exists(savepath):
        with open(savepath, "w", encoding='utf-8') as file_desc:
            time_stamp = time.strftime("%Y-%m-%d %H:%M:%S",
                                       time.localtime())
            file_desc.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            file_desc.write(
                '<testsuites tests="0" failures="0" '
                'disabled="0" errors="0" timestamp="%s" '
                'time="0" name="AllTests">\n' % time_stamp)
            file_desc.write(
                '  <testsuite name="%s" tests="0" failures="0" '
                'disabled="0" errors="0" time="0.0" '
                'unavailable="1" message="%s">\n' %
                (filename, message))
            file_desc.write('  </testsuite>\n')
            file_desc.write('</testsuites>\n')
    return


##############################################################################
##############################################################################

