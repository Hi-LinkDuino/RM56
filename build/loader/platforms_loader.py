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
import os
import merge_platform_build

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402


class PlatformsLoader:
    def __init__(self, platforms_config_file, source_root_dir, root_build_dir,
                 target_arch, scalable_build):
        self._platforms_config_file = platforms_config_file
        self._source_root_dir = source_root_dir
        self._root_build_dir = root_build_dir
        self._platforms_info = {}
        self._all_parts = {}
        self._all_stubs = {}
        self._is_load = False
        self._target_arch = target_arch
        self._scalable_build = scalable_build

    def _read_platforms_config(self):
        if not os.path.exists(self._platforms_config_file):
            raise Exception("config file '{}' doesn't exist.".format(
                self._platforms_config_file))
        config_info = read_json_file(self._platforms_config_file)
        if config_info is None:
            raise Exception("read file '{}' failed.".format(
                self._platforms_config_file))
        config_version = config_info.get('version')
        platforms_info = config_info.get('platforms')

        platforms_config = {}
        for _name, _platform_infos in platforms_info.items():
            if config_version >= 2:
                _info = _platform_infos
                target_os = _info.get('target_os')
                target_cpu = _info.get('target_cpu')
                if target_os is None or target_cpu is None:
                    error_info = "platform '{}' config incorrect,"\
                        "target_os or target_cpu is None."
                    raise Exception(error_info.format(_name))
                arch = "{}_{}".format(target_os, target_cpu)
                if arch in platforms_config:
                    _infos = platforms_config.get(arch)
                else:
                    _infos = {}
                _infos[_name] = _info
                platforms_config[arch] = _infos
            else:
                for _info in _platform_infos:
                    target_os = _info.get('target_os')
                    target_cpu = _info.get('target_cpu')
                    if target_os is None or target_cpu is None:
                        error_info = "platform '{}' config incorrect,"\
                            "target_os or target_cpu is None."
                        raise Exception(error_info.format(_name))
                    arch = "{}_{}".format(target_os, target_cpu)
                    if arch in platforms_config:
                        _infos = platforms_config.get(arch)
                    else:
                        _infos = {}
                    _infos[_name] = _info
                    platforms_config[arch] = _infos
        return platforms_config

    @staticmethod
    def _load_platform_config(platform_config_file):
        if not os.path.exists(platform_config_file):
            raise Exception(
                "config file '{}' doesn't exist.".format(platform_config_file))
        _platform_config = read_json_file(platform_config_file)
        if _platform_config is None:
            raise Exception(
                "read file '{}' failed.".format(platform_config_file))
        if 'parts' in _platform_config:
            parts = _platform_config.get('parts')
        else:
            parts = []
        if 'stub_parts' in _platform_config:
            stub_parts = _platform_config.get('stub_parts')
        else:
            stub_parts = []
        return parts, stub_parts

    def _loading(self):
        if self._is_load:
            return
        all_platforms_config = self._read_platforms_config()
        _platforms_info = all_platforms_config.get(self._target_arch)
        config_base_dir = os.path.dirname(self._platforms_config_file)
        all_parts = {}
        all_stubs = {}
        for _platform_name, _info in _platforms_info.items():
            if self._scalable_build is False:
                _file_name = _info.get('parts_config')
                if _file_name is None:
                    _file_name = os.path.join(
                        'target_platforms', "{}.build".format(_platform_name))
                _platform_config_file = os.path.join(config_base_dir,
                                                     _file_name)
            else:
                required_build_file = os.path.join(
                    config_base_dir, _info.get('base_parts_config'))
                optional_build_file = os.path.join(
                    config_base_dir, _info.get('optional_parts_config'))
                stub_build_file = os.path.join(config_base_dir,
                                               _info.get('stub_parts_config'))
                _platform_config_file = '{}.build'.format(_platform_name)

                merge_platform_build.do_merge(required_build_file,
                                              optional_build_file,
                                              stub_build_file,
                                              _platform_config_file)

            parts, stubs = self._load_platform_config(_platform_config_file)
            all_parts[_platform_name] = parts
            all_stubs[_platform_name] = stubs

        self._platforms_info = _platforms_info
        self._all_parts = all_parts
        self._all_stubs = all_stubs
        self._is_load = True

    def get_all_parts(self):
        self._loading()
        all_parts = {}
        for _platform, _parts in self._all_parts.items():
            part_list = []
            for part_def in _parts:
                _part_name = part_def.split(':')[1]
                part_list.append(_part_name)
            all_parts[_platform] = part_list
        return all_parts

    def get_all_stubs(self):
        self._loading()
        all_parts = {}
        for _platform, _parts in self._all_stubs.items():
            part_list = []
            for part_def in _parts:
                _part_name = part_def.split(':')[1]
                part_list.append(_part_name)
            all_parts[_platform] = part_list
        return all_parts

    def platforms_toolchain(self):
        self._loading()
        platform_toolchain = {}
        toolchain_platform = {}
        for key, val in self._platforms_info.items():
            _toolchain = val.get('toolchain')
            platform_toolchain[key] = _toolchain
            toolchain_platform[_toolchain] = key
        _result = {
            "platform_toolchain": platform_toolchain,
            "toolchain_platform": toolchain_platform
        }
        return _result


def get_platforms_info(platforms_config_file, source_root_dir, root_build_dir,
                       target_arch, config_output_relpath, scalable_build):
    platform_loader = PlatformsLoader(platforms_config_file, source_root_dir,
                                      root_build_dir, target_arch,
                                      scalable_build)

    platforms_info_output_dir = 'platforms_info'
    all_parts = platform_loader.get_all_parts()
    all_parts_file = os.path.join(source_root_dir, config_output_relpath,
                                  platforms_info_output_dir, "all_parts.json")
    write_json_file(all_parts_file, all_parts)

    # variant to toolchain and toolchain to variant
    toolchain_to_variant_dict = platform_loader.platforms_toolchain()
    toolchain_variant_info_file = os.path.join(source_root_dir,
                                               config_output_relpath,
                                               platforms_info_output_dir,
                                               "toolchain_to_variant.json")
    write_json_file(toolchain_variant_info_file,
                    toolchain_to_variant_dict,
                    check_changes=True)

    result = {}
    result['all_parts'] = all_parts
    result['all_stubs'] = platform_loader.get_all_stubs()
    result['variant_toolchain_info'] = toolchain_to_variant_dict
    return result
