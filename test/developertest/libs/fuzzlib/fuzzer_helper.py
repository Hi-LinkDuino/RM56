#!/usr/bin/env python
# -*- coding:utf-8 -*-
#
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
#

from __future__ import print_function
import argparse
import os
import re
import sys
import subprocess
import zipfile
import errno
import pipes
import traceback
import time
import copy

from tools.colored import Colored
from tools.templates import GN_ENTRY_TEMPLATE
from tools.templates import PROJECT_GN_TEMPLATE
from tools.templates import PROJECT_DEMO_TEMPLATE
from tools.templates import PROJECT_HEADER_TEMPLATE
from tools.templates import PROJECT_XML_TEMPLATE
import shutil
from pprint import pprint
from tools.run_result import RunResult

CURRENT_DIR = os.path.dirname(os.path.realpath(__file__))
SOURCE_ROOT_DIR = os.path.dirname(
    os.path.dirname(
        os.path.dirname(os.path.dirname(CURRENT_DIR))
        )
    )
SOURCE_OUT_DIR = os.path.join(SOURCE_ROOT_DIR, "out")

TDD_BUILD_GN_PATH = os.path.join(
    SOURCE_ROOT_DIR,
    "test/developertest/BUILD.gn"
    )
###project name must end with _fuzzer. eg. my_fuzzer,extrator_fuzzer.
VALID_PROJECT_NAME_REGEX = re.compile(r'^[a-zA-Z0-9_-]+(_fuzzer)+$')


def _add_environment_args(parser):
    """Add common environment args."""
    parser.add_argument(
        '-t',
        '--target_platform',
        default='phone',
        choices=["phone", "ivi", "plato"],
        help="set target_platform value, default:phone")

    parser.add_argument(
        '-f',
        '--filter',
        default=None,
        help="subsystem filter")


def _get_command_string(command):
    """Returns a shell escaped command string."""
    return ' '.join(pipes.quote(part) for part in command)


def parse_projects_path():
    path_list = []
    with open(TDD_BUILD_GN_PATH, 'r') as gn_file:
        for line in gn_file.readlines()[4:]:
            striped_str = line.strip()
            if striped_str.endswith("]"):
                break
            path_list.append(striped_str.split(":")[0][3:])
    return path_list


def _get_fuzzer_yaml_config(fuzzer_name):
    project_yaml_path = os.path.join(
        CURRENT_DIR,
        "projects",
        fuzzer_name,
        "project.yaml")
    if not os.path.exists(project_yaml_path):
        return None
    #log run stdout to fuzzlog dir
    with open(project_yaml_path) as filehandle:
        yaml_config = yaml.safe_load(filehandle)
    return yaml_config


#generate template fuzzer project
def generate(args):
    print("project name %s." % args.project_name)
    print("project path %s." % args.project_path)
    color_logger = Colored.get_project_logger()

    if not VALID_PROJECT_NAME_REGEX.match(args.project_name):
        print('Invalid project name.', file=sys.stderr)
        return 1

    template_args = {
        'project_name': args.project_name,
        'author': "",
        'email': ""
    }

    project_dir_path = os.path.join(args.project_path, args.project_name)
    print("project_dir_path %s." % project_dir_path)
    try:
        os.mkdir(project_dir_path)
    except OSError as os_exception:
        if os_exception.errno != errno.EEXIST:
            raise
        print(color_logger.red('%s already exists.' % project_dir_path),
            file=sys.stderr)
        return 1
    color_logger.green('Writing new files to %s' % project_dir_path)

    file_path = os.path.join(project_dir_path, 'project.xml')
    with open(file_path, 'w') as filehandle:
        filehandle.write(PROJECT_XML_TEMPLATE % template_args)

    file_path = os.path.join(project_dir_path, "%s.cpp" % args.project_name)
    with open(file_path, 'w') as filehandle:
        filehandle.write(PROJECT_DEMO_TEMPLATE % template_args)

    file_path = os.path.join(project_dir_path, "%s.h" % args.project_name)
    with open(file_path, 'w') as filehandle:
        filehandle.write(PROJECT_HEADER_TEMPLATE % template_args)
    file_path = os.path.join(project_dir_path, "BUILD.gn")
    with open(file_path, 'w') as filehandle:
        filehandle.write(PROJECT_GN_TEMPLATE % template_args)

    corpus_dir = os.path.join(project_dir_path, 'corpus')
    if not os.path.exists(corpus_dir):
        os.mkdir(corpus_dir)
        with open(os.path.join(corpus_dir, 'init'), 'w') as filehandle:
            filehandle.write("FUZZ")


#complie fuzzer project
def make(args,  stdout=None):
    """make fuzzer module."""
    color_logger = Colored.get_project_logger()

    pre_cmd = ['cd', SOURCE_ROOT_DIR]
    build_target_platform = "build_platform=\"%s\""

    build_script = [
        './build.sh',
        '--gn-args',
        'build_example=true',
        '--build-target'
    ]
    build_script.append(args.project_name)
    build_script.append("--gn-args")
    build_script.append(build_target_platform % args.build_platform)
    build_script.append("--product-name")
    build_script.append(args.build_platform)
    build_script.append("--export-para")
    build_script.append("PYCACHE_ENABLE:true")
    build_script.append("--export-para")
    build_script.append("BUILD_AOSP:false")
    print("BUILD_SCRIPT %s" % build_script)
    final_cmd = "%s && %s" % (
        _get_command_string(pre_cmd),
        _get_command_string(build_script)
        )

    color_logger.green('Running:%s' % final_cmd)

    subsystem_src_flag_file_path = os.path.join(
        SOURCE_OUT_DIR,
        "release/current_build_fuzz_target.txt"
    )
    if not os.path.exists(os.path.dirname(subsystem_src_flag_file_path)):
        os.makedirs(os.path.dirname(subsystem_src_flag_file_path))
    with open(subsystem_src_flag_file_path, "wb") as file_handle:
        file_handle.write(args.project_name.encode())

    try:
        if stdout:
            ret = subprocess.check_call(build_script, cwd=SOURCE_ROOT_DIR,
                                        stdout=stdout)
        else:
            ret = subprocess.check_call(build_script, cwd=SOURCE_ROOT_DIR)
        return ret
    except subprocess.CalledProcessError:
        print("*"*50)
        print("*"*50)
        print(
            'fuzzers {} build failed.'.format(args.project_name),
            file=sys.stdout
        )
        return -1


def report(args):
    pass


def coverage_all(args):
    pass


def main():
    parser = argparse.ArgumentParser(
        'fuzzer_helper.py',
        description='hydra-fuzz helpers'
    )
    subparsers = parser.add_subparsers(dest='command')

    generate_parser = subparsers.add_parser(
        'generate',
        help='Generate files for new project.name must end with "_fuzzer".')
    generate_parser.add_argument('project_name')
    generate_parser.add_argument('project_path')
    _add_environment_args(generate_parser)

    make_parser = subparsers.add_parser(
        'make', help='Build a single fuzzer module project. ')
    make_parser.add_argument('project_name')
    make_parser.add_argument('build_platform')
    _add_environment_args(make_parser)

    report_parser = subparsers.add_parser(
       'report', help='Report fuzzer log'
    )
    _add_environment_args(report_parser)
    report_parser.add_argument(
        'subfunc',
        default="list",
        choices=["list", "coverage", "all"]
    )
    report_parser.add_argument(
        "-i",
        "--id",
        required=False,
        help="report ID, e.g. empty_fuzzer.20200211184850"
    )

    args = parser.parse_args()

    if args.command == 'generate':
        return generate(args)
    elif args.command == 'make':
        return make(args)

    elif args.command == 'report':
        report(args)

if __name__ == "__main__":
    main()
