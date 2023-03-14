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
import argparse

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from loader import subsystem_info  # noqa: E402
from loader import platforms_loader  # noqa: E402
from loader import generate_targets_gn  # noqa: E402
from loader import load_ohos_build  # noqa: E402
from scripts.util.file_utils import read_json_file, write_json_file, write_file  # noqa: E402, E501


def _load_component_dist(source_root_dir, target_os, target_cpu):
    _parts_variants_info = {}
    _dir = "component_dist/{}-{}/packages_to_install".format(
        target_os, target_cpu)
    _file_name = "dist_parts_info.json"
    _dist_parts_info_file = os.path.join(source_root_dir, _dir, _file_name)
    if not os.path.exists(_dist_parts_info_file):
        # If the file does not exist, do nothing and return
        return _parts_variants_info
    _parts_info = read_json_file(_dist_parts_info_file)
    if _parts_info is None:
        raise Exception("read file '{}' failed.".format(_dist_parts_info_file))
    for _part_info in _parts_info:
        origin_part_name = _part_info.get('origin_part_name')
        if origin_part_name in _parts_variants_info:
            variants = _parts_variants_info.get(origin_part_name)
        else:
            variants = []
        _variant_name = _part_info.get('variant_name')
        variants.append(_variant_name)
        _parts_variants_info[origin_part_name] = variants
    return _parts_variants_info


def _get_real_part_name(original_part_name, current_platform, parts_variants):
    part_info = parts_variants.get(original_part_name)
    if part_info is None:
        return None, None
    if current_platform in part_info and current_platform != 'phone':
        real_name = '{}_{}'.format(original_part_name, current_platform)
    else:
        real_name = original_part_name
    return real_name, original_part_name


def _get_platforms_all_parts(source_root_dir, target_os, target_cpu, all_parts,
                             build_platforms, parts_variants):
    _dist_parts_variants = _load_component_dist(source_root_dir, target_os,
                                                target_cpu)
    target_platform_parts = {}
    for _platform, _parts in all_parts.items():
        if _platform not in build_platforms:
            continue
        part_name_info = {}
        for part_def in _parts:
            real_name, original_name = _get_real_part_name(
                part_def, _platform, parts_variants)
            if real_name is None:
                # find this from component_dist
                real_name, original_name = _get_real_part_name(
                    part_def, _platform, _dist_parts_variants)
            if real_name is None:
                continue
            part_name_info[real_name] = original_name
        target_platform_parts[_platform] = part_name_info
    return target_platform_parts


def _get_platforms_all_stubs(source_root_dir, target_os, target_cpu, all_stubs,
                             build_platforms, parts_variants):
    _dist_parts_variants = _load_component_dist(source_root_dir, target_os,
                                                target_cpu)
    platform_stubs = {}
    for _platform, _part_names in all_stubs.items():
        if _platform not in build_platforms:
            continue
        stub_parts_from_src = []
        stub_parts_from_dist = []
        for part_name in _part_names:
            real_name, original_name = _get_real_part_name(
                part_name, _platform, parts_variants)
            # real_name=None means part_name doesn't exist in source tree,
            # use binary in component_dist then.
            if real_name is None:
                # find this from component_dist
                real_name, original_name = _get_real_part_name(
                    part_name, _platform, _dist_parts_variants)
                if real_name is None:
                    continue
                else:
                    stub_sources = os.path.join(
                        source_root_dir,
                        "component_dist/{}-{}/api_stubs/{}/stubs_sources_list.txt"  # noqa: E501
                        .format(target_os, target_cpu, real_name))
                    stub_parts_from_dist.append('"{}"'.format(stub_sources))
            else:
                stub_parts_from_src.append(real_name)
        platform_stubs[_platform] = {
            "src": stub_parts_from_src,
            "dist": stub_parts_from_dist,
        }
    return platform_stubs


def _get_platforms_parts(src_parts_targets, target_platform_parts):
    platforms_parts = {}
    src_all_parts = src_parts_targets.keys()
    for _platform, _all_parts in target_platform_parts.items():
        src_parts_list = []
        no_src_parts_list = []
        for _part in _all_parts.keys():
            if _part in src_all_parts:
                src_parts_list.append(_part)
            else:
                no_src_parts_list.append(_part)
        _data = {
            'src_parts': src_parts_list,
            'no_src_parts': no_src_parts_list
        }
        platforms_parts[_platform] = _data
    return platforms_parts


def _get_parts_by_platform(target_platform_parts):
    parts_info = {}
    if 'phone' in target_platform_parts:
        phone_parts_list = target_platform_parts.get('phone').keys()
    else:
        phone_parts_list = []
    for _platform, _parts_info in target_platform_parts.items():
        base_parts_list = []
        curr_parts_list = []
        for _real_name, _original_name in _parts_info.items():
            if _real_name in phone_parts_list:
                base_parts_list.append(_real_name)
            elif _original_name in phone_parts_list:
                base_parts_list.append(_real_name)
            else:
                curr_parts_list.append(_real_name)
        result_data = {
            "base_parts_list": base_parts_list,
            "curr_parts_list": curr_parts_list
        }
        parts_info[_platform] = result_data
    return parts_info


def _check_parts_config_info(parts_config_info):
    if not ('parts_info' in parts_config_info and 'subsystem_parts'
            in parts_config_info and 'parts_variants' in parts_config_info
            and 'parts_kits_info' in parts_config_info
            and 'parts_inner_kits_info' in parts_config_info
            and 'parts_targets' in parts_config_info):
        raise Exception("Loading ohos.build information is incorrect.")


def _get_required_build_parts_list(target_platform_parts):
    parts_set = set()
    for _parts_list in target_platform_parts.values():
        parts_set.update(_parts_list)
    return list(parts_set)


def _get_required_build_targets(parts_targets, target_platform_parts):
    required_build_targets = {}
    _parts_list = _get_required_build_parts_list(target_platform_parts)
    for _p_name, _info in parts_targets.items():
        if _p_name not in _parts_list:
            continue
        required_build_targets[_p_name] = _info
    return required_build_targets


def _get_parts_src_list(required_parts_targets, parts_info):
    parts_name_map = {}
    for _list in parts_info.values():
        for _info in _list:
            parts_name_map[_info.get('part_name')] = _info.get(
                'origin_part_name')
    _src_set = set()
    for _name in required_parts_targets.keys():
        _origin_name = parts_name_map.get(_name)
        if _origin_name is None:
            continue
        _src_set.add(_origin_name)
    return list(_src_set)


def _check_product_part_feature(parts_info, product_preloader_dir):
    _preloader_feature_file = os.path.join(product_preloader_dir,
                                           'features.json')
    _preloader_feature_info = read_json_file(_preloader_feature_file)
    part_to_feature = _preloader_feature_info.get('part_to_feature')
    for key, vals in part_to_feature.items():
        part = parts_info.get(key)
        if part is None:
            continue
        _p_info = part[0]
        def_feature_list = _p_info.get('feature_list')
        if not def_feature_list:
            continue
        for _f_name in vals:
            if _f_name not in def_feature_list:
                raise Exception(
                    "The product use a feature that is not supported"
                    " by this part, part_name='{}', feature='{}'".format(
                        key, _f_name))


def _check_args(args, source_root_dir):
    print('args:', args)
    if 'gn_root_out_dir' not in args:
        raise Exception("args gn_root_out_dir is required.")
    if 'platforms_config_file' not in args:
        raise Exception("args platforms_config_file is required.")
    if 'subsystem_config_file' not in args:
        raise Exception("args subsystem_config_file is required.")
    gn_root_out_dir = args.gn_root_out_dir
    if gn_root_out_dir.startswith('/'):
        args.gn_root_out_dir = os.path.relpath(args.gn_root_out_dir,
                                               source_root_dir)
    else:
        _real_out_dir = os.path.realpath(gn_root_out_dir)
        if not _real_out_dir.startswith(source_root_dir):
            raise Exception("args gn_root_out_dir is incorrect.")

def syscap_sort(syscap):
    return syscap['component']

def generate_syscap_files(parts_config_info, target_platform_parts, pre_syscap_info_path, system_path):
    target_parts_list = _get_required_build_parts_list(target_platform_parts)
    syscap_info_list = parts_config_info.get('syscap_info')
    target_syscap_with_part_name_list = []
    target_syscap_list = []
    target_syscap_for_init_list = []
    for syscap in syscap_info_list:
        if syscap['component'] not in target_parts_list:
            continue
        if 'syscap' not in syscap or syscap['syscap'] == None or len(syscap['syscap']) == 0 or syscap['syscap'] == [""]:
            continue
        for syscap_string in syscap['syscap']:
            if syscap_string.startswith("SystemCapability.") == True:
                target_syscap_for_init_list.append("const." + syscap_string + "=true\n")
            else:
                raise Exception("""In bundle.json of part [{}], The syscap string [{}] is incorrect,
                 need start with \"SystemCapability.\"""".format(syscap['component'], syscap_string))
        syscap['syscap'].sort()
        target_syscap_with_part_name_list.append(syscap)
        target_syscap_list.extend(syscap['syscap'])

    # Generate SystemCapability.json & syscap.json & syscap.para
    target_syscap_list.sort()
    syscap_info_dict = read_json_file(pre_syscap_info_path)
    syscap_info_dict.update({'syscap':{'os':target_syscap_list}})
    system_etc_path = os.path.join(system_path, "etc/")
    if not os.path.exists(system_path):
        os.mkdir(system_path)
    if not os.path.exists(system_etc_path):
        os.mkdir(system_etc_path)
    syscap_info_json = os.path.join(system_etc_path, "SystemCapability.json")
    write_json_file(syscap_info_json, syscap_info_dict)
    target_syscap_with_part_name_list.sort(key = syscap_sort)
    syscap_info_with_part_name_file = os.path.join(system_etc_path, "syscap.json")
    write_json_file(syscap_info_with_part_name_file, {'components': target_syscap_with_part_name_list})
    if not os.path.exists(os.path.join(system_etc_path, "param/")):
        os.mkdir(os.path.join(system_etc_path, "param/"))
    target_syscap_for_init_file = os.path.join(system_etc_path, "param/syscap.para")
    f = open(target_syscap_for_init_file, "w")
    f.writelines(target_syscap_for_init_list)
    f.close()

def load(args):
    source_root_dir = args.source_root_dir
    _check_args(args, source_root_dir)
    config_output_relpath = os.path.join(args.gn_root_out_dir, 'build_configs')

    # loading subsystem info, scan src dir and get subsystem ohos.build
    _subsystem_info = subsystem_info.get_subsystem_info(
        args.subsystem_config_file, args.example_subsystem_file,
        source_root_dir, config_output_relpath, args.os_level)

    target_arch = '{}_{}'.format(args.target_os, args.target_cpu)
    # loading platforms config
    _platforms_info = platforms_loader.get_platforms_info(
        args.platforms_config_file, source_root_dir, args.gn_root_out_dir,
        target_arch, config_output_relpath, args.scalable_build)

    # get build platforms list
    toolchain_to_variant_dict = _platforms_info.get('variant_toolchain_info')
    variant_toolchains = toolchain_to_variant_dict.get('platform_toolchain')
    _all_platforms = variant_toolchains.keys()

    if args.build_platform_name == 'all':
        build_platforms = _all_platforms
    elif args.build_platform_name in _all_platforms:
        build_platforms = [args.build_platform_name]
    else:
        raise Exception(
            "The target_platform is incorrect, only allows [{}].".format(
                ', '.join(_all_platforms)))

    # loading ohos.build and gen part variant info
    parts_config_info = load_ohos_build.get_parts_info(
        source_root_dir, config_output_relpath, _subsystem_info,
        variant_toolchains, target_arch, args.ignore_api_check, args.build_xts)
    # check parts_config_info
    _check_parts_config_info(parts_config_info)
    parts_variants = parts_config_info.get('parts_variants')
    parts_targets = parts_config_info.get('parts_targets')
    parts_info = parts_config_info.get('parts_info')

    config_output_dir = os.path.join(source_root_dir, config_output_relpath)

    # target_platforms_parts.json
    target_platform_parts = _get_platforms_all_parts(
        source_root_dir, args.target_os, args.target_cpu,
        _platforms_info.get('all_parts'), build_platforms, parts_variants)
    target_platform_parts_file = os.path.join(config_output_dir,
                                              "target_platforms_parts.json")
    write_json_file(target_platform_parts_file,
                    target_platform_parts,
                    check_changes=True)

    # {platform}_system_capabilities.json
    # we assume that platform and devicetype are the same.
    for platform in build_platforms:
        platform_parts = target_platform_parts.get(platform)
        platform_capabilities = []
        for _, origin in platform_parts.items():
            # parts_info.get() might be None if the part is a binary package
            all_parts_variants = parts_info.get(origin)
            if all_parts_variants is None:
                continue
            part = all_parts_variants[0]
            if part.get('system_capabilities'):
                entry = part.get('system_capabilities')
                if len(entry) > 0:
                    platform_capabilities.extend(entry)
        platform_part_json_file = os.path.join(
            config_output_dir, "{0}_system_capabilities.json".format(platform))
        write_json_file(platform_part_json_file,
                        sorted(platform_capabilities),
                        check_changes=True)

    target_platform_stubs = _get_platforms_all_stubs(
        source_root_dir, args.target_os, args.target_cpu,
        _platforms_info.get('all_stubs'), build_platforms, parts_variants)
    generate_targets_gn.gen_stub_targets(
        parts_config_info.get('parts_kits_info'), target_platform_stubs,
        config_output_dir)

    # platforms_parts_by_src.json
    platforms_parts_by_src = _get_platforms_parts(parts_targets,
                                                  target_platform_parts)
    platforms_parts_by_src_file = os.path.join(source_root_dir,
                                               config_output_relpath,
                                               "platforms_parts_by_src.json")
    write_json_file(platforms_parts_by_src_file,
                    platforms_parts_by_src,
                    check_changes=True)

    required_parts_targets = _get_required_build_targets(
        parts_targets, target_platform_parts)
    generate_targets_gn.gen_targets_gn(required_parts_targets,
                                       config_output_dir)
    _phony_target = parts_config_info.get('phony_target')
    required_phony_targets = _get_required_build_targets(
        _phony_target, target_platform_parts)
    generate_targets_gn.gen_phony_targets(required_phony_targets,
                                          config_output_dir)

    # required_parts_targets.json
    build_targets_info_file = os.path.join(config_output_dir,
                                           "required_parts_targets.json")
    write_json_file(build_targets_info_file, required_parts_targets)
    # required_parts_targets_list.json
    build_targets_list_file = os.path.join(config_output_dir,
                                           "required_parts_targets_list.json")
    write_json_file(build_targets_list_file,
                    list(required_parts_targets.values()))

    # parts src flag file
    parts_src_flag_file = os.path.join(config_output_dir,
                                       "parts_src_flag.json")
    write_json_file(parts_src_flag_file,
                    _get_parts_src_list(required_parts_targets, parts_info),
                    check_changes=True)

    # write platforms_list.gni
    platforms_list_gni_file = os.path.join(config_output_dir,
                                           "platforms_list.gni")
    _platforms = set(build_platforms)
    _gni_file_content = []
    _gni_file_content.append('target_platform_list = [')
    _gni_file_content.append('  "{}"'.format('",\n  "'.join(_platforms)))
    _gni_file_content.append(']')
    _gni_file_content.append('kits_platform_list = [')
    _gni_file_content.append('  "{}",'.format('",\n  "'.join(_platforms)))
    if 'phone' not in build_platforms:
        _gni_file_content.append('  "phone"')
    _gni_file_content.append(']')
    write_file(platforms_list_gni_file, '\n'.join(_gni_file_content))

    # parts_different_info.json
    # Generate parts differences in different platforms, using phone as base.
    parts_different_info = _get_parts_by_platform(target_platform_parts)
    parts_different_info_file = os.path.join(config_output_dir,
                                             "parts_different_info.json")
    write_json_file(parts_different_info_file,
                    parts_different_info,
                    check_changes=True)
    # for testfwk
    infos_for_testfwk_file = os.path.join(config_output_dir,
                                          "infos_for_testfwk.json")
    _output_infos_for_testfwk(parts_config_info, target_platform_parts,
                              infos_for_testfwk_file)

    # check part feature
    _check_product_part_feature(parts_info,
                                os.path.dirname(args.platforms_config_file))
    pre_syscap_info_path = os.path.join(os.path.dirname(args.platforms_config_file), "SystemCapability.json")
    system_path = os.path.join(source_root_dir, os.path.join(os.path.dirname(args.platforms_config_file), "system/"))
    generate_syscap_files(parts_config_info, target_platform_parts, pre_syscap_info_path, system_path)

def _output_infos_by_platform(part_name_infos, parts_info_dict):
    required_parts = {}
    subsystem_infos = {}
    for part_name, origin_part_name in part_name_infos.items():
        part_info = parts_info_dict.get(part_name)
        if part_info is None:
            continue
        if origin_part_name != part_info.get('origin_part_name'):
            raise Exception("part configuration is incorrect.")
        required_parts[origin_part_name] = part_info
        _subsystem_name = part_info.get('subsystem_name')
        if _subsystem_name in subsystem_infos:
            p_list = subsystem_infos.get(_subsystem_name)
        else:
            p_list = []
        p_list.append(origin_part_name)
        subsystem_infos[_subsystem_name] = p_list
    result = {}
    result['subsystem_infos'] = subsystem_infos
    result['part_infos'] = required_parts
    return result

def _output_infos_for_testfwk(parts_config_info, target_platform_parts,
                              infos_for_testfwk_file):
    parts_info = parts_config_info.get('parts_info')
    parts_info_dict = {}
    for _part_name, _parts in parts_info.items():
        for _info in _parts:
            parts_info_dict[_info.get('part_name')] = _info

    _output_infos = {}
    for _platform, _parts in target_platform_parts.items():
        result = _output_infos_by_platform(_parts, parts_info_dict)
        _output_infos[_platform] = result

    write_json_file(infos_for_testfwk_file, _output_infos, check_changes=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--platforms-config-file', required=True)
    parser.add_argument('--subsystem-config-file', required=True)
    parser.add_argument('--example-subsystem-file', required=False)
    parser.add_argument('--source-root-dir', required=True)
    parser.add_argument('--gn-root-out-dir', default='.')
    parser.add_argument('--build-platform-name', default='phone')
    parser.add_argument('--build-xts', dest='build_xts', action='store_true')
    parser.set_defaults(build_xts=False)
    parser.add_argument('--target-os', default='ohos')
    parser.add_argument('--target-cpu', default='arm64')
    parser.add_argument('--os-level', default='standard')
    parser.add_argument('--ignore-api-check', nargs='*', default=[])

    parser.add_argument('--scalable-build', action='store_true')
    parser.set_defaults(scalable_build=False)
    args = parser.parse_args()

    load(args)
    return 0


if __name__ == '__main__':
    sys.exit(main())
