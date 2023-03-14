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

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from loader import subsystem_scan  # noqa: E402
from scripts.util.file_utils import write_json_file  # noqa: E402


def _output_subsystem_configs(output_dir, subsystem_configs):
    build_config_file_name = "subsystem_build_config.json"
    build_config_file = os.path.join(output_dir, 'subsystem_info',
                                     build_config_file_name)
    write_json_file(build_config_file, subsystem_configs)

    src_output_file_name = "src_subsystem_info.json"
    no_src_output_file_name = "no_src_subsystem_info.json"

    src_subsystem = {}
    for key, val in subsystem_configs.get('subsystem').items():
        src_subsystem[key] = val.get('path')
    src_output_file = os.path.join(output_dir, 'subsystem_info',
                                   src_output_file_name)
    write_json_file(src_output_file, src_subsystem)

    no_src_output_file = os.path.join(output_dir, 'subsystem_info',
                                      no_src_output_file_name)
    write_json_file(no_src_output_file,
                    subsystem_configs.get('no_src_subsystem'))


def get_subsystem_info(subsystem_config_file, example_subsystem_file,
                       source_root_dir, config_output_path, os_level):
    if not subsystem_config_file:
        subsystem_config_file = 'build/subsystem_config.json'

    subsystem_configs = {}
    output_dir_realpath = os.path.join(source_root_dir, config_output_path)
    subsystem_configs = subsystem_scan.scan(subsystem_config_file,
                                            example_subsystem_file,
                                            source_root_dir)

    _output_subsystem_configs(output_dir_realpath, subsystem_configs)
    return subsystem_configs.get('subsystem')
