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
import load_bundle_file

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util.file_utils import read_json_file, write_json_file, write_file  # noqa: E402, E501  pylint: disable=C0413, E0611

import_list = """
# import("//build/ohos.gni")
# import("//build/ohos_var.gni")
import("//build/ohos/ohos_part.gni")
import("//build/ohos/ohos_kits.gni")
import("//build/ohos/ohos_test.gni")
"""

part_template = """
ohos_part("{}") {{
  subsystem_name = "{}"
  module_list = [
    {}
  ]
  origin_name = "{}"
  variant = "{}"
}}"""

inner_kits_template = """
ohos_inner_kits("{0}_inner_kits") {{
  sdk_libs = [
{1}
  ]
  part_name = "{0}"
  origin_name = "{2}"
  variant = "{3}"
}}"""

system_kits_template = """
ohos_system_kits("{0}_system_kits") {{
  sdk_libs = [
{1}
  ]
  part_name = "{0}"
  origin_name = "{2}"
  variant = "{3}"
}}"""

test_template = """
ohos_part_test("{0}_test") {{
  testonly = true
  test_packages = [
    {1}
  ]
  deps = [":{0}"]
  part_name = "{0}"
  subsystem_name = "{2}"
}}"""


def _normalize(label, path):
    if not label.startswith('//'):
        label = '//{}/{}'.format(path, label)
    return label

def get_syscap_from_bundle(bundle_file):
    if not os.path.exists(bundle_file):
        raise Exception(
            "config file '{}' doesn't exist.".format(bundle_file))
    bundle_config = read_json_file(bundle_file)
    if bundle_config is None:
        raise Exception("read file '{}' failed.".format(bundle_file))
    part_name = bundle_config.get('component').get('name')
    part_syscap = bundle_config.get('component').get('syscap')
    return part_name, part_syscap

def read_build_file(ohos_build_file):
    if not os.path.exists(ohos_build_file):
        raise Exception(
            "config file '{}' doesn't exist.".format(ohos_build_file))
    subsystem_config = read_json_file(ohos_build_file)
    if subsystem_config is None:
        raise Exception("read file '{}' failed.".format(ohos_build_file))
    return subsystem_config


class PartObject(object):
    """"part object info, description part variant."""

    def __init__(self, part_name, variant_name, part_config, toolchain,
                 subsystem_name, target_arch):
        self._origin_name = part_name
        if variant_name != 'phone':
            _real_name = '{}_{}'.format(part_name, variant_name)
        else:
            _real_name = part_name
        self._part_name = _real_name
        self._variant_name = variant_name
        self._subsystem_name = subsystem_name
        self._feature_list = []
        self._toolchain = toolchain
        self._inner_kits_info = {}
        self._kits = []
        self._target_arch = target_arch
        self._system_capabilities = []
        self._parsing_config(self._part_name, part_config, subsystem_name)

    @classmethod
    def _parsing_kits_lib(cls, kit_lib, is_inner_kits=False):
        lib_config = []
        lib_type = kit_lib.get('type')
        if lib_type is None:
            lib_type = 'so' if is_inner_kits else 'jar'
        label = kit_lib.get('name')
        if label is None:
            raise Exception("kits lib config incorrect, required for name.")
        lib_config.append('      type = "{}"'.format(lib_type))
        lib_config.append('      name = "{}"'.format(label))
        if lib_type == 'so' and 'header' in kit_lib:
            header = kit_lib.get('header')
            header_files = header.get('header_files')
            lib_config.append('      header = {')
            lib_config.append('        header_files = [')
            for h_file in header_files:
                lib_config.append('          "{}",'.format(h_file))
            lib_config.append('        ]')
            header_base = header.get('header_base')
            lib_config.append('        header_base = "{}"'.format(header_base))
            lib_config.append('      }')
        if is_inner_kits is False and 'javadoc' in kit_lib:
            javadoc_val = kit_lib.get('javadoc')
            lib_config.append('      javadoc = {')
            resource_dir = javadoc_val.get('resource_dir')
            lib_config.append(
                '        resource_dir = "{}"'.format(resource_dir))
            lib_config.append('      }')
        return lib_config

    def _parsing_inner_kits(self, part_name, inner_kits_info, build_gn_content,
                            target_arch):
        inner_kits_libs_gn = []
        for inner_kits_lib in inner_kits_info:
            inner_kits_libs_gn.append('    {')
            inner_kits_libs_gn.extend(
                self._parsing_kits_lib(inner_kits_lib, True))
            inner_kits_libs_gn.append('    },')

        inner_kits_libs_gn_line = '\n'.join(inner_kits_libs_gn)
        inner_kits_def = inner_kits_template.format(part_name,
                                                    inner_kits_libs_gn_line,
                                                    self._origin_name,
                                                    self._variant_name)
        build_gn_content.append(inner_kits_def)
        # output inner kits info to resolve external deps
        _libs_info = {}
        for inner_kits_lib in inner_kits_info:
            info = {'part_name': part_name}
            label = inner_kits_lib.get('name')
            lib_name = label.split(':')[1]
            info['label'] = label
            info['name'] = lib_name
            lib_type = inner_kits_lib.get('type')
            if lib_type is None:
                lib_type = 'so'
            info['type'] = lib_type
            prebuilt = inner_kits_lib.get('prebuilt_enable')
            if prebuilt:
                info['prebuilt_enable'] = prebuilt
                prebuilt_source_libs = inner_kits_lib.get('prebuilt_source')
                prebuilt_source = prebuilt_source_libs.get(target_arch)
                info['prebuilt_source'] = prebuilt_source
            else:
                info['prebuilt_enable'] = False
            # header files
            if lib_type == 'so':
                header = inner_kits_lib.get('header')
                if header is None:
                    raise Exception(
                        "header not configuration, part_name = '{}'".format(
                            part_name))
                header_base = header.get('header_base')
                if header_base is None:
                    raise Exception(
                        "header base not configuration, part_name = '{}'".
                        format(part_name))
                info['header_base'] = header_base
                info['header_files'] = header.get('header_files')
            _libs_info[lib_name] = info
        self._inner_kits_info = _libs_info

    def _parsing_system_kits(self, part_name, system_kits_info,
                             build_gn_content):
        system_kits_libs_gn = []
        kits = []
        for _kits_lib in system_kits_info:
            system_kits_libs_gn.append('    {')
            system_kits_libs_gn.extend(self._parsing_kits_lib(
                _kits_lib, False))
            kits.append('"{}"'.format(_kits_lib.get('name')))
            system_kits_libs_gn.append('    },')
        _kits_libs_gn_line = '\n'.join(system_kits_libs_gn)
        system_kits_def = system_kits_template.format(part_name,
                                                      _kits_libs_gn_line,
                                                      self._origin_name,
                                                      self._variant_name)
        build_gn_content.append(system_kits_def)
        self._kits = kits

    def _parsing_config(self, part_name, part_config, subsystem_name):
        self._part_target_list = []
        build_gn_content = []
        build_gn_content.append(import_list)

        # ohos part
        if 'module_list' not in part_config:
            raise Exception(
                "ohos.build incorrect, part name: '{}'".format(part_name))
        module_list = part_config.get('module_list')
        if len(module_list) == 0:
            module_list_line = ''
        else:
            module_list_line = '"{}",'.format('",\n    "'.join(module_list))
        parts_definition = part_template.format(part_name, subsystem_name,
                                                module_list_line,
                                                self._origin_name,
                                                self._variant_name)
        build_gn_content.append(parts_definition)

        # part inner kits
        if part_config.get('inner_kits'):
            self._part_target_list.append('inner_kits')
            inner_kits_info = part_config.get('inner_kits')
            self._parsing_inner_kits(part_name, inner_kits_info,
                                     build_gn_content, self._target_arch)
        # part system kits
        if part_config.get('system_kits'):
            self._part_target_list.append('system_kits')
            system_kits_info = part_config.get('system_kits')
            self._parsing_system_kits(part_name, system_kits_info,
                                      build_gn_content)
        # part test list
        if part_config.get('test_list'):
            self._part_target_list.append('test')
            test_list = part_config.get('test_list')
            test_list_line = '"{}",'.format('",\n    "'.join(test_list))
            test_def = test_template.format(part_name, test_list_line,
                                            subsystem_name)
            build_gn_content.append(test_def)
        self._build_gn_content = build_gn_content
        # feature
        if part_config.get('feature_list'):
            self._feature_list = part_config.get('feature_list')
            # check feature
            for _feature_name in self._feature_list:
                if not _feature_name.startswith('{}_'.format(
                        self._origin_name)):
                    raise Exception(
                        "part feature list config incorrect,"
                        " part_name='{}', feature_name='{}'".format(
                            self._origin_name, _feature_name))

        # system_capabilities is a list attribute of a part in ohos.build
        if part_config.get('system_capabilities'):
            self._system_capabilities = part_config.get('system_capabilities')

    def part_name(self):
        """part name."""
        return self._part_name

    def part_variant(self):
        """part variant."""
        return self._variant_name

    def toolchain(self):
        """current part variant toolchain."""
        return self._toolchain

    def part_inner_kits(self):
        """part inner kits."""
        return self._inner_kits_info

    def part_kits(self):
        """part kits."""
        return self._kits

    def write_build_gn(self, config_output_dir):
        """output build gn."""
        part_gn_file = os.path.join(config_output_dir, self._part_name,
                                    'BUILD.gn')
        write_file(part_gn_file, '\n'.join(self._build_gn_content))

    def get_target_label(self, config_output_relpath):
        """target label."""
        if config_output_relpath.startswith('/'):
            raise Exception("args config output relative path is incorrect.")
        if self._toolchain == '':
            return "//{0}/{1}:{1}".format(config_output_relpath,
                                          self._part_name)
        else:
            return "//{0}/{1}:{1}({2})".format(config_output_relpath,
                                               self._part_name,
                                               self._toolchain)

    def part_group_targets(self, config_output_relpath):
        """part group target."""
        if config_output_relpath.startswith('/'):
            raise Exception("args config output relative path is incorrect.")
        _labels = {}
        _labels['part'] = self.get_target_label(config_output_relpath)
        for group_label in self._part_target_list:
            if group_label == 'phony':
                _labels[group_label] = "//{0}/{1}:{1}_{2}".format(
                    config_output_relpath, self._part_name, group_label)
                continue
            if self._toolchain == '':
                _labels[group_label] = "//{0}/{1}:{1}_{2}".format(
                    config_output_relpath, self._part_name, group_label)
            else:
                _labels[group_label] = "//{0}/{1}:{1}_{2}({3})".format(
                    config_output_relpath, self._part_name, group_label,
                    self._toolchain)
        return _labels

    def part_info(self):
        """part info."""
        _info = {}
        _info['part_name'] = self._part_name
        _info['origin_part_name'] = self._origin_name
        _info['toolchain_label'] = self._toolchain
        _info['variant_name'] = self._variant_name
        _info['subsystem_name'] = self._subsystem_name
        _info['system_capabilities'] = self._system_capabilities

        if self._feature_list:
            _info['feature_list'] = self._feature_list
        if self._variant_name != 'phone':
            toolchain_name = self._toolchain.split(':')[1]
            _build_out_dir = toolchain_name
        else:
            _build_out_dir = '.'
        _info['build_out_dir'] = _build_out_dir
        return _info


class LoadBuildConfig(object):
    """load build config file and parse configuration info."""

    def __init__(self, source_root_dir, subsystem_build_info,
                 config_output_dir, variant_toolchains, subsystem_name,
                 target_arch, ignored_subsystems):
        self._source_root_dir = source_root_dir
        self._build_info = subsystem_build_info
        self._config_output_relpath = config_output_dir
        self._is_load = False
        self._parts_variants = {}
        self._part_list = {}
        self._part_targets_label = {}
        self._variant_toolchains = variant_toolchains
        self._subsystem_name = subsystem_name
        self._target_arch = target_arch
        self._ignored_subsystems = ignored_subsystems
        self._parts_info_dict = {}
        self._phony_targets = {}
        self._parts_path_dict = {}
        self._part_hisysevent_config = {}
        self._parts_module_list = {}

    def _parsing_config(self, parts_config):
        _parts_info_dict = {}
        _variant_phony_targets = {}
        for part_name, value in parts_config.items():
            if 'variants' in value:
                variants = value.get('variants')
                if len(variants) == 0:
                    variants = ['phone']
            else:
                variants = ['phone']
            _build_target = {}
            _targets_label = {}
            _parts_info = []
            for variant in variants:
                toolchain = self._variant_toolchains.get(variant)
                if toolchain is None:
                    continue
                part_obj = PartObject(part_name, variant, value, toolchain,
                                      self._subsystem_name, self._target_arch)
                real_part_name = part_obj.part_name()
                self._part_list[real_part_name] = part_obj

                subsystem_config_dir = os.path.join(
                    self._config_output_relpath, self._subsystem_name)
                part_obj.write_build_gn(
                    os.path.join(self._source_root_dir, subsystem_config_dir))

                _target_label = part_obj.get_target_label(subsystem_config_dir)
                _build_target[variant] = _target_label
                _targets_label[real_part_name] = part_obj.part_group_targets(
                    subsystem_config_dir)
                _parts_info.append(part_obj.part_info())
                if variant != 'phone':
                    _variant_phony_targets[real_part_name] = _target_label
            self._part_targets_label.update(_targets_label)
            self._parts_variants[part_name] = _build_target
            if 'hisysevent_config' in value:
                _config_files = value.get('hisysevent_config')
                for _config_file in _config_files:
                    if not _config_file.startswith('//'):
                        raise Exception(
                            "part '{}' hisysevent config incorrest.".format(
                                part_name))
                self._part_hisysevent_config[part_name] = _config_files
            _parts_info_dict[part_name] = _parts_info
        self._parts_info_dict = _parts_info_dict
        self._phony_targets = _variant_phony_targets

    def _merge_build_config(self):
        _build_files = self._build_info.get('build_files')
        subsystem_name = None
        parts_info = {}
        parts_path_dict = {}
        for _build_file in _build_files:
            if _build_file.endswith('bundle.json'):
                bundle_part_obj = load_bundle_file.BundlePartObj(_build_file)
                _parts_config = bundle_part_obj.to_ohos_build()
            else:
                _parts_config = read_build_file(_build_file)
            _subsystem_name = _parts_config.get('subsystem')
            if subsystem_name and _subsystem_name != subsystem_name:
                raise Exception(
                    "subsystem name config incorrect in '{}'.".format(
                        _build_file))
            subsystem_name = _subsystem_name
            _curr_parts_info = _parts_config.get('parts')
            for _pname in _curr_parts_info.keys():
                parts_path_dict[_pname] = os.path.relpath(
                    os.path.dirname(_build_file), self._source_root_dir)
            parts_info.update(_curr_parts_info)
        subsystem_config = {}
        subsystem_config['subsystem'] = subsystem_name
        subsystem_config['parts'] = parts_info
        return subsystem_config, parts_path_dict

    def parse_syscap_info(self):
        _build_files = self._build_info.get('build_files')
        subsystem_syscap = []
        for _build_file in _build_files:
            if _build_file.endswith('bundle.json'):
                part_name, part_syscap = get_syscap_from_bundle(_build_file)
                subsystem_syscap.append({'component': part_name, 'syscap': part_syscap})
        return subsystem_syscap

    def parse(self):
        """parse part info from build config file."""
        if self._is_load:
            return
        subsystem_config, parts_path_dict = self._merge_build_config()
        parts_config = subsystem_config.get('parts')
        self._parts_module_list.update(parts_config)
        self._parsing_config(parts_config)
        self._parts_path_dict = parts_path_dict
        self._is_load = True

    def parts_variants(self):
        """parts varinats info."""
        self.parse()
        return self._parts_variants

    def parts_inner_kits_info(self):
        """parts inner kits info."""
        self.parse()
        _parts_inner_kits = {}
        for part_obj in self._part_list.values():
            _parts_inner_kits[
                part_obj.part_name()] = part_obj.part_inner_kits()
        return _parts_inner_kits

    def parts_build_targets(self):
        """parts build target label."""
        self.parse()
        return self._part_targets_label

    def parts_name_list(self):
        """parts name list."""
        self.parse()
        return list(self._part_list.keys())

    def parts_info(self):
        """parts info."""
        self.parse()
        return self._parts_info_dict

    def parts_phony_target(self):
        """parts phony target info"""
        self.parse()
        return self._phony_targets

    def parts_kits_info(self):
        """parts kits info."""
        self.parse()
        _parts_kits = {}
        for part_obj in self._part_list.values():
            _parts_kits[part_obj.part_name()] = part_obj.part_kits()
        return _parts_kits

    def parts_path_info(self):
        """parts to path info."""
        self.parse()
        return self._parts_path_dict

    def parts_hisysevent_config(self):
        self.parse()
        return self._part_hisysevent_config

    def parts_modules_info(self):
        self.parse()
        return self._parts_module_list


def _output_parts_info(parts_config_dict, config_output_path):
    parts_info_output_path = os.path.join(config_output_path, "parts_info")
    # parts_info.json
    if 'parts_info' in parts_config_dict:
        parts_info = parts_config_dict.get('parts_info')
        parts_info_file = os.path.join(parts_info_output_path,
                                       "parts_info.json")
        write_json_file(parts_info_file, parts_info)
        _part_subsystem_dict = {}
        for key, value in parts_info.items():
            for _info in value:
                _sub_name = _info.get('subsystem_name')
                _part_subsystem_dict[key] = _sub_name
                break
        _part_subsystem_file = os.path.join(parts_info_output_path,
                                            "part_subsystem.json")
        write_json_file(_part_subsystem_file, _part_subsystem_dict)

    # subsystem_parts.json
    if 'subsystem_parts' in parts_config_dict:
        subsystem_parts = parts_config_dict.get('subsystem_parts')
        subsystem_parts_file = os.path.join(parts_info_output_path,
                                            "subsystem_parts.json")
        write_json_file(subsystem_parts_file, subsystem_parts)
        # adapter mini system
        for _sub_name, _p_list in subsystem_parts.items():
            _output_info = {}
            _output_info['parts'] = _p_list
            _sub_info_output_file = os.path.join(config_output_path,
                                                 'mini_adapter',
                                                 '{}.json'.format(_sub_name))
            write_json_file(_sub_info_output_file, _output_info)

    # parts_variants.json
    if 'parts_variants' in parts_config_dict:
        parts_variants = parts_config_dict.get('parts_variants')
        parts_variants_info_file = os.path.join(parts_info_output_path,
                                                "parts_variants.json")
        write_json_file(parts_variants_info_file, parts_variants)

    # inner_kits_info.json
    if 'parts_inner_kits_info' in parts_config_dict:
        parts_inner_kits_info = parts_config_dict.get('parts_inner_kits_info')
        parts_inner_kits_info_file = os.path.join(parts_info_output_path,
                                                  "inner_kits_info.json")
        write_json_file(parts_inner_kits_info_file, parts_inner_kits_info)

    # parts_targets.json
    if 'parts_targets' in parts_config_dict:
        parts_targets = parts_config_dict.get('parts_targets')
        parts_targets_info_file = os.path.join(parts_info_output_path,
                                               "parts_targets.json")
        write_json_file(parts_targets_info_file, parts_targets)

    # phony_targets.json
    if 'phony_target' in parts_config_dict:
        phony_target = parts_config_dict.get('phony_target')
        phony_target_info_file = os.path.join(parts_info_output_path,
                                              "phony_target.json")
        write_json_file(phony_target_info_file, phony_target)

    # paths_path_info.json
    if 'parts_path_info' in parts_config_dict:
        parts_path_info = parts_config_dict.get('parts_path_info')
        parts_path_info_file = os.path.join(parts_info_output_path,
                                            'parts_path_info.json')
        write_json_file(parts_path_info_file, parts_path_info)
        path_to_parts = {}
        for _key, _val in parts_path_info.items():
            _p_list = path_to_parts.get(_val, [])
            _p_list.append(_key)
            path_to_parts[_val] = _p_list
        path_to_parts_file = os.path.join(parts_info_output_path,
                                          'path_to_parts.json')
        write_json_file(path_to_parts_file, path_to_parts)

    # hisysevent_config
    if 'hisysevent_config' in parts_config_dict:
        hisysevent_config = parts_config_dict.get('hisysevent_config')
        hisysevent_info_file = os.path.join(parts_info_output_path,
                                            'hisysevent_configs.json')
        write_json_file(hisysevent_info_file, hisysevent_config)

    # _parts_modules_info
    if 'parts_modules_info' in parts_config_dict:
        parts_modules_info = parts_config_dict.get('parts_modules_info')
        _output_info = {}
        _all_p_info = []
        for key, value in parts_modules_info.items():
            _p_info = {}
            _p_info['part_name'] = key
            _module_list = value.get('module_list')
            _p_info['module_list'] = _module_list
            _all_p_info.append(_p_info)
        _output_info['parts'] = _all_p_info
        parts_modules_info_file = os.path.join(parts_info_output_path,
                                               'parts_modules_info.json')
        write_json_file(parts_modules_info_file, _output_info)


def get_parts_info(source_root_dir,
                   config_output_relpath,
                   subsystem_info,
                   variant_toolchains,
                   target_arch,
                   ignored_subsystems,
                   build_xts=False):
    """parts info,
    get info from build config file.
    """
    parts_variants = {}
    parts_inner_kits_info = {}
    parts_kits_info = {}
    parts_targets = {}
    parts_info = {}
    subsystem_parts = {}
    _phony_target = {}
    _parts_path_info = {}
    _parts_hisysevent_config = {}
    _parts_modules_info = {}
    system_syscap = []
    for subsystem_name, build_config_info in subsystem_info.items():
        if subsystem_name == 'xts' and build_xts is False:
            continue
        build_loader = LoadBuildConfig(source_root_dir, build_config_info,
                                       config_output_relpath,
                                       variant_toolchains, subsystem_name,
                                       target_arch, ignored_subsystems)
        _parts_variants = build_loader.parts_variants()
        parts_variants.update(_parts_variants)
        _inner_kits_info = build_loader.parts_inner_kits_info()
        parts_inner_kits_info.update(_inner_kits_info)
        parts_kits_info.update(build_loader.parts_kits_info())
        _parts_targets = build_loader.parts_build_targets()
        parts_targets.update(_parts_targets)
        subsystem_parts[subsystem_name] = build_loader.parts_name_list()
        parts_info.update(build_loader.parts_info())
        _phony_target.update(build_loader.parts_phony_target())
        _parts_path_info.update(build_loader.parts_path_info())
        _parts_hisysevent_config.update(build_loader.parts_hisysevent_config())
        _parts_modules_info.update(build_loader.parts_modules_info())
        system_syscap.extend(build_loader.parse_syscap_info())
    parts_config_dict = {}
    parts_config_dict['parts_info'] = parts_info
    parts_config_dict['subsystem_parts'] = subsystem_parts
    parts_config_dict['parts_variants'] = parts_variants
    parts_config_dict['parts_inner_kits_info'] = parts_inner_kits_info
    parts_config_dict['parts_kits_info'] = parts_kits_info
    parts_config_dict['parts_targets'] = parts_targets
    parts_config_dict['phony_target'] = _phony_target
    parts_config_dict['parts_path_info'] = _parts_path_info
    parts_config_dict['hisysevent_config'] = _parts_hisysevent_config
    parts_config_dict['parts_modules_info'] = _parts_modules_info
    _output_parts_info(parts_config_dict,
                       os.path.join(source_root_dir, config_output_relpath))
    parts_config_dict['syscap_info'] = system_syscap
    return parts_config_dict
