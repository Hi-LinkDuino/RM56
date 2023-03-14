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

import json
import os

from hb_internal.common.utils import read_json_file


def _read_lite_component_configs(file):
    subsytem_name = os.path.basename(file)[:-5]
    configs = {}
    configs['subsystem_name'] = subsytem_name
    with open(file, 'rb') as fin:
        data = json.load(fin)
        components = data.get('components')
        parts = {}
        for com in components:
            part = {}
            targets = com.get('targets')
            test_targets = []
            non_test_targets = []
            for item in targets:
                target_names = item.strip('"').split(':')
                if len(target_names) > 1 and 'test' in target_names[1]:
                    test_targets.append(item)
                else:
                    non_test_targets.append(item)
            part['module_list'] = non_test_targets
            if test_targets != []:
                part['test_list'] = test_targets
            part_name = com.get('component')
            parts[part_name] = part
        configs['parts'] = parts
    return configs


def _save_as_ohos_build(config, ohos_build):
    new_config = json.dumps(config, indent=2, sort_keys=True)
    with open(ohos_build, 'w') as fout:
        fout.write(new_config)


def parse_lite_subsystem_config(lite_components_dir, output_dir,
                                source_root_dir, subsystem_config_file):
    subsystem_infos = read_json_file(subsystem_config_file)
    for root, _, files in os.walk(lite_components_dir):
        for file in files:
            if file[-5:] == '.json':
                configs = _read_lite_component_configs(os.path.join(
                    root, file))
                subsystem_name = configs.get('subsystem_name')
                ohos_build = os.path.join(
                    output_dir, '{}/ohos.build'.format(subsystem_name))
                os.makedirs(os.path.dirname(ohos_build), exist_ok=True)
                _save_as_ohos_build(configs, ohos_build)
                subsystem_infos[subsystem_name] = {
                    'name':
                    subsystem_name,
                    "path":
                    os.path.relpath(os.path.dirname(ohos_build),
                                    source_root_dir),
                }
    return subsystem_infos
