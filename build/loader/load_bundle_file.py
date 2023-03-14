#!/usr/bin/env python3
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
import os

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util import file_utils  # noqa: E402


class BundlePartObj(object):
    def __init__(self, bundle_config_file):
        self._build_config_file = bundle_config_file
        self._loading_config()

    def _loading_config(self):
        if not os.path.exists(self._build_config_file):
            raise Exception("file '{}' doesn't exist.".format(
                self._build_config_file))
        self.bundle_info = file_utils.read_json_file(self._build_config_file)
        if self.bundle_info is None:
            raise Exception("read file '{}' failed.".format(
                self._build_config_file))
        self._check_format()

    def _check_format(self):
        _tip_info = "bundle.json info is incorrect in '{}'".format(
            self._build_config_file)
        if 'component' not in self.bundle_info:
            raise Exception("{}, 'component' is required.".format(_tip_info))
        _component_info = self.bundle_info.get('component')
        if 'name' not in _component_info:
            raise Exception(
                "{}, 'component.name' is required.".format(_tip_info))
        if 'subsystem' not in _component_info:
            raise Exception(
                "{}, 'component.subsystem' is required.".format(_tip_info))
        if 'build' not in _component_info:
            raise Exception(
                "{}, 'component.build' is required.".format(_tip_info))
        _bundle_build = _component_info.get('build')
        if 'sub_component' not in _bundle_build:
            raise Exception(
                "{}, 'component.build.sub_component' is required.".format(
                    _tip_info))

    def to_ohos_build(self):
        _component_info = self.bundle_info.get('component')
        _subsystem_name = _component_info.get('subsystem')
        _part_name = _component_info.get('name')
        _bundle_build = _component_info.get('build')
        _ohos_build_info = {}
        _ohos_build_info['subsystem'] = _subsystem_name
        _part_info = {}
        _part_info['module_list'] = _component_info.get('build').get(
            'sub_component')
        if 'inner_kits' in _bundle_build:
            _part_info['inner_kits'] = _bundle_build.get('inner_kits')
        if 'test' in _bundle_build:
            _part_info['test_list'] = _bundle_build.get('test')
        if 'features' in _component_info:
            _part_info['feature_list'] = _component_info.get('features')
        if 'syscap' in _component_info:
            _part_info['system_capabilities'] = _component_info.get('syscap')
        if 'hisysevent_config' in _component_info:
            _part_info['hisysevent_config'] = _component_info.get('hisysevent_config')
        _ohos_build_info['parts'] = {_part_name: _part_info}
        return _ohos_build_info
