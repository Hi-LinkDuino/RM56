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
import argparse

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util.file_utils import read_json_file, read_file, write_file  # noqa: E402 E501


def _read_subninja_build(build_dir, subninja_build_file):
    subninja_build_file = os.path.join(build_dir, subninja_build_file)
    if not os.path.exists(subninja_build_file):
        raise Exception("file '{}' doesn't exist.".format(subninja_build_file))
    subninja_build = read_file(subninja_build_file)
    if subninja_build is None:
        raise Exception("read file '{}' failed.".format(subninja_build_file))
    support_lib_type = ['.a', '.so', '']
    label_name = ''
    label_target = ''
    for _line in subninja_build:
        if _line.startswith('label_name = '):
            label_name = _line[len('label_name = '):]
        elif _line.startswith('build '):
            _build_info = _line.split(':')[0]
            build_label = _build_info.split(' ')[1]
            _, extension = os.path.splitext(build_label)
            if extension in support_lib_type:
                label_target = build_label

    if label_target != '':
        if label_name == '':
            target_filename = os.path.basename(label_target)
            label_name, _ = os.path.splitext(target_filename)
        return {label_name: label_target}
    return None


def _parse_target_label(build_label_list, toolchain_name):
    phony_targets_dict = {}
    for build_label in build_label_list:
        target_filename = os.path.basename(build_label)
        label_name, _ = os.path.splitext(target_filename)
        if label_name:
            phony_targets_dict[label_name] = build_label
            start_index = len('{}/obj/'.format(toolchain_name))
        _path = os.path.dirname(build_label)[start_index:]
        if _path:
            phony_targets_dict[_path] = build_label
        if label_name and _path:
            _label_path = '{}$:{}'.format(_path, label_name)
            phony_targets_dict[_label_path] = build_label
    return phony_targets_dict


def _read_toolchain_ninja(build_dir, toolchain_ninja_file, toolchain_name):
    if not os.path.exists(toolchain_ninja_file):
        raise Exception(
            "file '{}' doesn't exist.".format(toolchain_ninja_file))
    toolchain_ninja_rules = read_file(toolchain_ninja_file)
    if toolchain_ninja_rules is None:
        raise Exception("read file '{}' failed.".format(toolchain_ninja_file))

    build_label_list = []
    subninja_targets = {}
    for _ninja_rule in toolchain_ninja_rules:
        if _ninja_rule.startswith('build '):
            _tmp = _ninja_rule.split(':')[0]
            _label = _tmp[len('build '):]
            if not _label.endswith('.stamp'):
                continue
            build_label_list.append(_label)
        if _ninja_rule.startswith('subninja '):
            sbuninja_file = _ninja_rule[len('subninja '):]
            subninja_target_info = _read_subninja_build(
                build_dir, sbuninja_file)
            if subninja_target_info:
                subninja_targets.update(subninja_target_info)
    build_phony_targets = _parse_target_label(build_label_list, toolchain_name)
    build_phony_targets.update(subninja_targets)
    return build_phony_targets


def _read_variants_toolchain_info(variants_toolchain_info_file):
    if not os.path.exists(variants_toolchain_info_file):
        raise Exception(
            "file '{}' doesn't exist.".format(variants_toolchain_info_file))
    variants_toolchain_info = read_json_file(variants_toolchain_info_file)
    if variants_toolchain_info is None:
        raise Exception(
            "read file '{}' failed.".format(variants_toolchain_info_file))
    platform_toolchain = variants_toolchain_info.get('platform_toolchain')
    return platform_toolchain


def _read_build_ninja(build_ninja_file):
    if not os.path.exists(build_ninja_file):
        raise Exception("file '{}' doesn't exist.".format(build_ninja_file))
    ninja_targets = read_file(build_ninja_file)
    if ninja_targets is None:
        raise Exception("read file '{}' failed.".format(build_ninja_file))
    return ninja_targets


def generate_phony_targets(build_dir, toolchain_ninja_file, platform,
                           toolchain_name, default_targets_name):
    build_phony_targets = _read_toolchain_ninja(build_dir,
                                                toolchain_ninja_file,
                                                toolchain_name)
    targets_list = []
    for key, build_label in build_phony_targets.items():
        targets_list.append('build {}/{}: phony {}'.format(
            platform, key, build_label))

    _diff_targets = set(default_targets_name).difference(
        set(build_phony_targets.keys()))
    for _diff_target in _diff_targets:
        targets_list.append('build {}/{}: phony {}'.format(
            platform, _diff_target, _diff_target))
    build_file = os.path.join(os.path.dirname(toolchain_ninja_file),
                              '{}_build.ninja'.format(platform))
    write_file(build_file, '{}\n\n'.format('\n'.join(targets_list)))
    return build_file


def _update_build_ninja(build_dir, include_files):
    try:
        ninja_build_file = os.path.join(build_dir, 'build.ninja')
        if not os.path.exists(ninja_build_file):
            raise Exception(
                "file '{}' doesn't exist.".format(ninja_build_file))
        with open(ninja_build_file, 'a+') as _file:
            data = []
            for line in _file.readlines():
                _line = line.rstrip('\n')
                if _line.startswith('subninja '):
                    data.append(_line)
            for include_file in include_files:
                include_info = 'subninja {}'.format(
                    os.path.relpath(include_file, build_dir))
                if include_info in data:
                    continue
                _file.write('{}\n'.format(include_info))
            _file.flush()
    except: # noqa E722
        raise


def update(build_dir, variants_toolchain_info_file):
    variants_toolchain_info_file = os.path.join(build_dir,
                                                variants_toolchain_info_file)
    platform_toolchain = _read_variants_toolchain_info(
        variants_toolchain_info_file)

    ninja_build_file = os.path.join(build_dir, 'build.ninja')
    default_ninja_targets = _read_build_ninja(ninja_build_file)
    default_targets_name = []
    for _ninja_target in default_ninja_targets:
        if not _ninja_target.startswith('build '):
            continue
        _ninja_target = _ninja_target.split(': ')[0]
        default_targets_name.append(_ninja_target[len('build '):])

    include_files = []
    for platform, toolchain_label in platform_toolchain.items():
        if platform == 'phone':
            continue
        toolchain_name = toolchain_label.split(':')[1]
        toolchain_ninja_file = os.path.join(build_dir, toolchain_name,
                                            'toolchain.ninja')
        if not os.path.exists(toolchain_ninja_file):
            continue
        _build_file = generate_phony_targets(build_dir, toolchain_ninja_file,
                                             platform, toolchain_name,
                                             default_targets_name)
        include_files.append(_build_file)
    _update_build_ninja(build_dir, include_files)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--source-root-dir', required=True)
    parser.add_argument('--root-build-dir', required=True)
    parser.add_argument('--variants-toolchain-info-file', required=True)
    args = parser.parse_args()

    source_root_dir = args.source_root_dir
    if not os.path.exists(os.path.join(source_root_dir, '.gn')):
        print('source root dir incorrect.')
        return 1
    build_dir = os.path.join(source_root_dir, args.root_build_dir)
    update(build_dir, args.variants_toolchain_info_file)
    return 0


if __name__ == '__main__':
    sys.exit(main())
