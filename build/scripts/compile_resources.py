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

import optparse
import os
import sys
import shutil
import json

from zipfile import ZipFile  # noqa: E402
from util import build_utils  # noqa: E402


def parse_args(args):
    args = build_utils.expand_file_args(args)

    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)
    parser.add_option('--output-resources-zipfile',
                      help='path to packaged resources')
    parser.add_option('--output-header-file',
                      help='path to generated ResourceTable.h')
    parser.add_option('--resources-dir', help='resources directory')
    parser.add_option('--restool-path', help='path to restool')
    parser.add_option('--hap-profile', help='path to hap profile')
    parser.add_option('--package-name', help='package name of resource file')
    parser.add_option('--app-profile', default = False, help='path to app profile')

    options, _ = parser.parse_args(args)
    options.resources_dir = build_utils.parse_gn_list(options.resources_dir)
    return options


def get_package_name_from_profile(options):
    with open(options.hap_profile) as fp:
        if not options.app_profile:
            return json.load(fp)['module']['package']
        else:
            return json.load(fp)['app']['bundleName']


def compile_resources(options):
    with build_utils.temp_dir() as build:
        res_dir = os.path.join(build, 'gen/resources')
        gen_dir = os.path.join(os.path.dirname(options.output_header_file), "gen")
        gen_dir = os.path.abspath(gen_dir)
        header_dir = os.path.join(build, 'header')
        os.makedirs(res_dir)
        if os.path.exists(gen_dir):
            shutil.rmtree(gen_dir)
        os.makedirs(gen_dir)
        os.makedirs(header_dir)

        cmd = [options.restool_path]
        for directory in options.resources_dir:
            dest_res_dir = os.path.join(res_dir, os.path.dirname(directory))
            shutil.copytree(os.path.dirname(directory), dest_res_dir)
            cmd.extend(['-i', dest_res_dir])
        cmd.extend(['-j', options.hap_profile])
        if options.package_name != "" and options.package_name is not None:
            package_name = options.package_name
        else:
            package_name = get_package_name_from_profile(options)
        generated_header_file = os.path.join(
            header_dir, os.path.basename(options.output_header_file))
        cmd.extend(
            ['-p', package_name, '-o', gen_dir, '-r', generated_header_file])
        build_utils.check_output(cmd)
        R_txt_path = os.path.join(gen_dir, 'R.txt')
        if os.path.exists(R_txt_path):
            os.unlink(R_txt_path)
        if options.output_resources_zipfile:
            build_utils.zip_dir(options.output_resources_zipfile, gen_dir)
        if options.output_header_file:
            shutil.copy(generated_header_file, options.output_header_file)


def main(args):
    options = parse_args(args)

    if options.resources_dir == []:
        with ZipFile(options.output_resources_zipfile, 'w') as file:
            return
    inputs = ([options.restool_path, options.hap_profile])
    depfiles = []
    for directory in options.resources_dir:
        depfiles += (build_utils.get_all_files(directory))

    input_strings = [options.package_name] if options.package_name else []
    outputs = []
    if options.output_resources_zipfile:
        outputs.append(options.output_resources_zipfile)
    if options.output_header_file:
        outputs.append(options.output_header_file)
    build_utils.call_and_write_depfile_if_stale(
        lambda: compile_resources(options),
        options,
        depfile_deps=depfiles,
        input_paths=inputs + depfiles,
        input_strings=input_strings,
        output_paths=(outputs),
        force=False,
        add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
