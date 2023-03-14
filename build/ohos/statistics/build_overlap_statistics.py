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
import fnmatch
import argparse
import json


def is_combine_jars(contents):
    if contents.get('deps_info').get('combine_target'):
        return True
    else:
        return False


def get_sources_file(build_config):
    if not fnmatch.fnmatch(build_config, "*java*.build_config"):
        return None
    with open(build_config, 'r') as config_f:
        contents = json.load(config_f)
        if is_combine_jars(contents):
            return None
        return contents.get('deps_info').get('java_sources_file')


def read_file(sources_file):
    contents = []
    if not os.path.exists(sources_file):
        return []
    with open(sources_file, 'r') as this:
        contents = [line.strip() for line in this.readlines()]
    return contents


def get_subsystem_paths(file, root_dir):
    paths = {
        'common': 'build',
        'third_party': 'third_party',
        'test': 'test',
        'mcl': 'mcl'
    }
    with open(file, 'r') as jfile:
        data = json.load(jfile)
        for key in data.keys():
            path = data.get(key).get('path')
            if os.path.exists(os.path.join(root_dir, path)):
                paths[key] = path

    return paths


def overlap_rate_key(element):
    return element['overlap_rate']


def compute_overlap_rate_by_subsystem(options, paths, program_language):
    objs = []
    if program_language == 'c':
        pattern = '*.o'
    if program_language == 'java':
        pattern = '*.build_config'
    for root, _, files in os.walk(options.build_out_dir):
        for file in fnmatch.filter(files, pattern):
            if program_language == 'c':
                splits = os.path.join(root, file).split('/obj/')
                obj = ''.join(splits[1:])
                if obj == '':
                    continue
                if obj.find('/gen/') != -1:
                    splits = obj.split('/gen/')
                    obj = ''.join(splits[1:])
                objs.append(obj)
            if program_language == 'java':
                sources_file = get_sources_file(os.path.join(root, file))
                if not sources_file:
                    continue
                for java_file in read_file(
                        os.path.join(options.build_out_dir, sources_file)):
                    if fnmatch.fnmatch(java_file, "*/generated_java/*"):
                        continue
                    objs.append(java_file)

    total_builds = len(objs)
    total_files = len(set(objs))
    if total_builds == 0 or total_files == 0:
        return

    statistics = []
    for subsystem in sorted(paths.keys()):
        path = paths.get(subsystem)
        if program_language == 'c':
            pattern = '{}*'.format(path)
        if program_language == 'java':
            pattern = '../../{}*'.format(path)

        sub_objs = []
        for obj in fnmatch.filter(objs, pattern):
            sub_objs.append(obj)
        builds = len(sub_objs)
        files = len(set(sub_objs))
        if files == 0:
            continue
        overlap_rate = float(builds) / float(files)
        sub_stat = {
            "builds": builds,
            "builds_percentage": 100 * float(builds) / float(total_builds),
            "files": files,
            "files_percentage": 100 * float(files) / float(total_files),
            "overlap_rate": overlap_rate,
            "subsystem": subsystem,
        }
        statistics.append(sub_stat)
    print('{} targets overlap rate statistics'.format(program_language))
    print('{:16}\t{:8}\t{:5}\t{:8}\t{:5}\t{:4}'.format(
        'subsystem', 'files NO.', 'percentage', 'builds NO.', 'percentage',
        'overlap rate'))

    for item in sorted(statistics, key=overlap_rate_key, reverse=True):
        print('{:16}\t{:8}\t{:2.1f}%\t{:8}\t{:2.1f}%\t{:.2f}'.format(
            item.get('subsystem'), item.get('files'),
            item.get('files_percentage'), item.get('builds'),
            item.get('builds_percentage'), item.get('overlap_rate')))
    print('\n{} overall build overlap rate: {:.2f}\n\n'.format(
        program_language,
        float(total_builds) / float(total_files)))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build-out-dir', help='base directory to analyze.')
    parser.add_argument('--root-source-dir', help='source root directory.')
    parser.add_argument(
        '--subsystem-config-file', help='path to subsystem java targets.')

    options = parser.parse_args()

    paths = get_subsystem_paths(options.subsystem_config_file,
                                os.path.realpath(options.root_source_dir))
    compute_overlap_rate_by_subsystem(options, paths, 'c')
    compute_overlap_rate_by_subsystem(options, paths, 'java')


if __name__ == '__main__':
    sys.exit(main())
