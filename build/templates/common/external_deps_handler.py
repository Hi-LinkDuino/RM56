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

import argparse
import os
import sys

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402


def get_toolchain(current_variant, external_part_variants, platform_toolchain):
    if current_variant == 'phone':
        toolchain = platform_toolchain.get(current_variant)
        required_include_dir = False
    else:
        if current_variant in external_part_variants:
            toolchain = platform_toolchain.get(current_variant)
            required_include_dir = False
        else:
            toolchain = platform_toolchain.get('phone')
            required_include_dir = True
    return toolchain, required_include_dir


def _get_external_module_info(parts_inner_kits_info, external_part_name,
                              external_module_name, adapted_part_name):
    _inner_kits_info_dict = parts_inner_kits_info.get(external_part_name)
    if _inner_kits_info_dict is None:
        raise Exception(
            "external dep part '{}' doesn't exist.".format(external_part_name))
    if external_module_name in _inner_kits_info_dict:
        external_module_desc_info = _inner_kits_info_dict.get(
            external_module_name)
    elif adapted_part_name:
        _new_kits_info_dict = parts_inner_kits_info.get(adapted_part_name)
        if _new_kits_info_dict is None:
            raise Exception(
                "part '{}' doesn't exist.".format(adapted_part_name))
        external_module_desc_info = _new_kits_info_dict.get(
            external_module_name)
        if external_module_desc_info is None:
            raise Exception(
                "external dep module '{}' doesn't exist in part '{}'.".format(
                    external_module_name, adapted_part_name))
    else:
        raise Exception(
            "external dep module '{}' doesn't exist in part '{}'.".format(
                external_module_name, external_part_name))
    return external_module_desc_info


def _get_external_module_from_sdk(sdk_base_dir, external_part_name,
                                  external_module_name, adapted_part_name):
    _sdk_info_file = os.path.join(sdk_base_dir, external_part_name,
                                  "sdk_info.json")
    subsystem_sdk_info = read_json_file(_sdk_info_file)
    if subsystem_sdk_info is None:
        raise Exception("part '{}' doesn't exist in sdk modules.".format(
            external_part_name))

    _adapted = False
    if external_module_name in subsystem_sdk_info:
        sdk_module_info = subsystem_sdk_info.get(external_module_name)
    elif adapted_part_name:
        _new_sdk_info_file = os.path.join(sdk_base_dir, adapted_part_name,
                                          "sdk_info.json")
        _new_subsystem_sdk_info = read_json_file(_new_sdk_info_file)
        if _new_subsystem_sdk_info is None:
            raise Exception("part '{}' doesn't exist sdk modules.".format(
                adapted_part_name))
        sdk_module_info = _new_subsystem_sdk_info.get(external_module_name)
        if sdk_module_info is None:
            raise Exception(
                "external dep module '{}' doesn't exist in part '{}'.".format(
                    external_module_name, adapted_part_name))
        _adapted = True
    else:
        raise Exception(
            "external dep module '{}' doesn't exist in part '{}'.".format(
                external_module_name, external_part_name))
    return sdk_module_info, _adapted


def _get_inner_kits_adapter_info(innerkits_adapter_info_file):
    _parts_compatibility = {}
    if os.path.exists(innerkits_adapter_info_file):
        inner_kits_adapter_info = read_json_file(innerkits_adapter_info_file)
        if inner_kits_adapter_info is None:
            raise Exception("read inner_kits_adapter info failed.")
        _parts_compatibility.update(inner_kits_adapter_info)
    return _parts_compatibility


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--external-deps', nargs='*', required=True)
    parser.add_argument('--parts-src-flag-file', required=True)
    parser.add_argument('--sdk-base-dir', required=True)
    parser.add_argument('--sdk-dir-name', required=True)
    parser.add_argument('--external-deps-temp-file', required=True)
    parser.add_argument('--use-sdk', dest='use_sdk', action='store_true')
    parser.set_defaults(use_sdk=False)
    parser.add_argument('--current-toolchain', required=False, default='')
    parser.add_argument(
        '--innerkits-adapter-info-file',
        default='../../build/ohos/inner_kits_adapter.json')
    args = parser.parse_args()

    if len(args.external_deps) == 0:
        result = {}
        write_json_file(args.external_deps_temp_file, result)
        return 0

    # parts info
    parts_src_flag = read_json_file(args.parts_src_flag_file)
    # external deps list
    external_deps = args.external_deps
    # sdk base dir
    sdk_base_dir = args.sdk_base_dir
    sdk_dir_name = args.sdk_dir_name
    use_sdk = args.use_sdk

    deps = []
    libs = []
    include_dirs = []

    # load inner kits info file
    inner_kits_info_file = 'build_configs/parts_info/inner_kits_info.json'
    all_kits_info_dict = read_json_file(inner_kits_info_file)
    if all_kits_info_dict is None:
        raise Exception("read pre_build inner_kits_info failed.")

    # load parts variants
    parts_variants_info_file = 'build_configs/parts_info/parts_variants.json'
    all_parts_variants_info = read_json_file(parts_variants_info_file)
    if all_parts_variants_info is None:
        raise Exception("read pre_build parts_variants failed.")

    # load toolchains info
    toolchain_variant_info_file = os.path.join('build_configs',
                                               'platforms_info',
                                               'toolchain_to_variant.json')
    toolchain_variant_info = read_json_file(toolchain_variant_info_file)
    if toolchain_variant_info is None:
        raise Exception("read pre_build parts_variants failed.")
    toolchain_platform = toolchain_variant_info.get('toolchain_platform')
    current_variant = toolchain_platform.get(args.current_toolchain)
    if not current_variant:
        current_variant = 'phone'
    platform_toolchain = toolchain_variant_info.get('platform_toolchain')

    # compatibility interim
    _parts_compatibility = _get_inner_kits_adapter_info(
        args.innerkits_adapter_info_file)

    for external_lib in external_deps:
        deps_desc = external_lib.split(':')
        external_part_name = deps_desc[0]
        external_module_name = deps_desc[1]

        # Usually the value is None
        _adapted_part_name = _parts_compatibility.get(external_part_name)

        # Check if the subsystem has source code
        if not use_sdk and external_part_name in parts_src_flag:
            external_module_desc_info = _get_external_module_info(
                all_kits_info_dict, external_part_name, external_module_name,
                _adapted_part_name)
            dep_label = external_module_desc_info['label']

            part_variants_info = all_parts_variants_info.get(external_part_name)
            if part_variants_info is None:
                raise Exception(
                    "external deps part '{}' variants info is None.".format(
                        external_part_name))
            toolchain, required_include_dir = get_toolchain(
                current_variant, part_variants_info.keys(), platform_toolchain)
            dep_label_with_tc = "{}({})".format(dep_label, toolchain)
            deps += [dep_label_with_tc]

            if required_include_dir is True and external_module_desc_info.get(
                    'type') == 'so':
                include_dir = external_module_desc_info.get('header_base')
                include_dirs.append(include_dir)

            # sdk prebuilt
            if external_module_desc_info['prebuilt_enable']:
                libs += [external_module_desc_info['prebuilt_source']]
        else:
            sdk_module_info, adapted_ok = _get_external_module_from_sdk(
                sdk_base_dir, external_part_name, external_module_name,
                _adapted_part_name)

            if adapted_ok is True:
                _external_part_name = _adapted_part_name
            else:
                _external_part_name = external_part_name
            deps += [
                "//{}/{}:{}".format(sdk_dir_name, _external_part_name,
                                    external_module_name)
            ]
            # java sdk module does not need to add libs
            if not (sdk_module_info.get('type')
                    and sdk_module_info.get('type') == 'jar'):
                external_lib_source = sdk_module_info.get('source')
                libs += [
                    "//{}/{}/{}".format(sdk_dir_name, _external_part_name,
                                        external_lib_source)
                ]

    result = {}
    if deps:
        result['deps'] = deps
    if libs:
        result['libs'] = libs
    if include_dirs:
        result['include_dirs'] = include_dirs

    write_json_file(args.external_deps_temp_file, result)
    return 0


if __name__ == '__main__':
    sys.exit(main())
