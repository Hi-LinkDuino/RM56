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

import argparse
import json
import os

part_infos = "part_infos"
subsystem_infos = "subsystem_infos"


def create_testfwk_info_file(component_info_file, abs_fold, file_name):
    if not os.path.exists(abs_fold):
        os.makedirs(abs_fold)
    file_path = os.path.join(abs_fold, file_name)
    data = get_testfwk_info(component_info_file)
    dict_product = json.dumps(data)
    with open(file_path, 'w') as testfwk_info_file:
        json.dump(json.loads(dict_product), testfwk_info_file)
        testfwk_info_file.close()
    return file_path


def get_testfwk_info(platform_json_file):
    platform_name = platform_json_file[(platform_json_file.rfind("/") + 1):]
    platform_name = platform_name[0: platform_name.rfind(".")]
    data = {
        platform_name: {
            subsystem_infos: {

            },
            part_infos: {

            }
        }
    }
    with open(platform_json_file, 'r') as platform_file:
        dict_component_infos = json.load(platform_file)
        list_subsystems = dict_component_infos["subsystems"]
        set_components_name = []
        for subsystem in list_subsystems:
            subsystem_name = subsystem["subsystem"]
            list_components_name = []
            list_components = subsystem["components"]
            for component in list_components:
                component_name = component["component"]
                list_components_name.append(component_name)
                if component_name not in set_components_name:
                    set_components_name.append(component_name)
            data[platform_name][subsystem_infos][subsystem_name] = \
                list_components_name
        for component_name in set_components_name:
            data[platform_name][part_infos][component_name] = \
                {"part_name": component_name, "build_out_dir": "."}
    return data


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('--component-info-file', required=True)
    arg_parser.add_argument('--output-json-fold', required=True)
    arg_parser.add_argument('--output-json-file-name', required=True)
    arg_parser.add_argument('--output-module-list-files-fold', required=True)

    args = arg_parser.parse_args()

    create_testfwk_info_file(args.component_info_file, \
        args.output_json_fold, args.output_json_file_name)