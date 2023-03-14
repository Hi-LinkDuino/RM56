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

import sys
import re
import os
import json
import argparse

sys.path.append(
    os.path.dirname(
        os.path.dirname(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))))))

from third_party.PyYAML.lib3 import yaml  # noqa: E402


def format_validation(pattern, content):
    regex = re.compile(pattern)
    matched = regex.match(content)
    if not matched:
        print("error: %s match %s " % (content, pattern), "failed")
    return matched


def length_check(content, length_min, length_max):
    length = len(content)
    length_valid = [False, True][(length >= length_min)
                                 and (length <= length_max)]
    if not length_valid:
        print("error: length of %s is incorrect," % (content),
              "must be in range of [%d, %d]" % (length_min, length_max))
    return length_valid


def check_char(content):
    return format_validation('^[A-Z]{1}[A-Z0-9_]*$', content)


def check_char_base(content):
    return format_validation('[A-Za-z0-9_]*$', content)


def check_char_desc(content):
    return format_validation('[A-Za-z0-9_ ]*$', content)


def check_char_tag(content):
    return format_validation('[A-Za-z0-9]*$', content)


def domain_name_check(content, length_min, length_max):
    if not length_check(content, length_min, length_max):
        sys.exit(1)
    if not check_char(content):
        if check_char_base(content):
            print("warning: %s: %s" % (DOMAIN_NAME, content),
                  "is not all in A-Z, or not start with A-Z",
                  file=WARNING_TXT)
        else:
            sys.exit(1)


def desc_check(content):
    if ((not length_check(content, 3, 128)) or (not check_char_desc(content))):
        sys.exit(1)


def tag_check(tag_list):
    if not length_check(tag_list, 1, 5):
        sys.exit(1)
    for each_tag in tag_list:
        if ((not check_char_tag(each_tag))
                or (not length_check(each_tag, 1, 16))):
            sys.exit(1)


def check_domain_duplicate(domain, domain_value):
    if domain in DOMAIN_DICT:
        sys.exit(1)
    else:
        DOMAIN_DICT[domain] = domain_value


def event_check(yaml_info, base_type, base_level, param_type):
    for event_name, param_info in yaml_info.items():
        domain_name_check(event_name, 1, 32)
        if "__BASE" in param_info:
            if not param_info["__BASE"]["type"] in base_type:
                print('error: __BASE type must be in %s' % base_type)
                sys.exit(1)
            if not param_info["__BASE"]["level"] in base_level:
                print('error: __BASE level must be in %s' % base_level)
                sys.exit(1)
            if "desc" in param_info["__BASE"]:
                desc_check(param_info["__BASE"]["desc"])
            else:
                print("error: __BASE must have desc")
                sys.exit(1)
            if "tag" in param_info["__BASE"]:
                param_tag_list = param_info["__BASE"]["tag"].split()
                tag_check(param_tag_list)
        else:
            print("error: __BASE is not the key in ", param_info)
            sys.exit(1)
        if not length_check(param_info, 1, 129):
            sys.exit(1)
        for param_name, param_value in param_info.items():
            if param_name == "__BASE":
                continue
            domain_name_check(param_name, 1, 32)
            if not param_value["type"] in param_type:
                print('error:', param_name, 'type must in %s' % param_type)
                sys.exit(1)
            if "desc" in param_value:
                desc_check(param_value["desc"])
            else:
                print('error: ', param_value, 'must have desc')
                sys.exit(1)
            if "arrsize" in param_value:
                if ((param_value["arrsize"] < 1)
                        or (param_value["arrsize"] > 100)):
                    print("error: ", param_name,
                          "array size must be in range [1, 100]")
                    sys.exit(1)


def merge_hisysevent_config(yaml_list, output_path):
    if (len(output_path) == 0):
        present_path = os.path.dirname(os.path.abspath(__file__))
        output_path = present_path
    if (len(yaml_list) == 0):
        print("error: there is no yaml imput")
        sys.exit(1)
    if not os.path.exists(output_path):
        os.makedirs(output_path, exist_ok=True)
    base_type = ["FAULT", "STATISTIC", "SECURITY", "BEHAVIOR"]
    base_level = ["CRITICAL", "MINOR"]
    param_type = [
        "BOOL", "INT8", "UINT8", "INT16", "UINT16", "INT32", "UINT32", "INT64",
        "UINT64", "FLOAT", "DOUBLE", "STRING"
    ]
    global DOMAIN_DICT
    DOMAIN_DICT = {}
    dictionary_result = {}
    global WARNING_TXT
    hisysevent_warning_file = os.path.join(output_path,
                                           'hisysevent_warning.txt')
    WARNING_TXT = open(hisysevent_warning_file, 'w+')
    for each_yaml in yaml_list:
        yaml_file = open(each_yaml, 'r')
        yaml_info = yaml.safe_load(yaml_file)
        global DOMAIN_NAME
        DOMAIN_NAME = yaml_info['domain']
        domain_name_check(DOMAIN_NAME, 1, 16)
        check_domain_duplicate(DOMAIN_NAME, each_yaml)
        del yaml_info['domain']
        if not length_check(yaml_info, 1, 4096):
            print("error: events count should in the range of [1, 4096]")
            sys.exit()
        event_check(yaml_info, base_type, base_level, param_type)
        dictionary_result[DOMAIN_NAME] = yaml_info
    hisysevent_def_file = os.path.join(output_path, 'hisysevent.def')
    with open(hisysevent_def_file, 'w') as j:
        json.dump(dictionary_result, j, indent=4)
    print(
        "Verify the YAML files and pack them into {} hisysevent.def successful"
        .format(hisysevent_def_file))
    return hisysevent_def_file


def main(argv):
    parser = argparse.ArgumentParser(description='yaml list')
    parser.add_argument("--yaml-list", nargs='+', required=True)
    parser.add_argument("--def-path", required=True)
    args = parser.parse_args(argv)
    hisysevent_def_file = merge_hisysevent_config(args.yaml_list,
                                                  args.def_path)
    print(hisysevent_def_file)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
