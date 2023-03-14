#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
# Copyright (c) 2020 Huawei Device Co., Ltd.
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

import os
import sys
import json
import argparse
import importlib
import traceback

VERSION = "0.4.6"


def find_top():
    cur_dir = os.getcwd()
    while cur_dir != "/":
        hb_internal = os.path.join(cur_dir, 'build/lite/hb_internal')
        if os.path.exists(hb_internal):
            return cur_dir

        cur_dir = os.path.dirname(cur_dir)
    raise Exception("Please call hb utilities inside source root directory")


def get_hb_commands(config_file):
    if not os.path.exists(config_file):
        raise Exception('Error: {} not exist, couldnot get hb command set'.format(config_file))
    with open(config_file, 'r') as file:
        config = json.load(file)
        return config


def main():
    try:
        topdir = find_top()
    except Exception as ex:
        return print("hb_error: Please call hb utilities inside source root directory")
    sys.path.insert(0, os.path.join(topdir, 'build/lite'))
    parser = argparse.ArgumentParser(description='OHOS Build System '
                                     f'version {VERSION}')
    parser.add_argument('-v',
                        '--version',
                        action='version',
                        version=f'[OHOS INFO] hb version {VERSION}')

    subparsers = parser.add_subparsers()
    parser_list = []

    command_set = get_hb_commands(os.path.join(topdir, 'build/lite/hb_internal/hb_command_set.json'))
    for key, val in command_set.items():
        parser_list.append({'name': key, 'help': val})

    for each in parser_list:
        module_parser = subparsers.add_parser(name=each.get('name'),
                                              help=each.get('help'))
        module = importlib.import_module('hb_internal.{0}.{0}'.format(
            each.get('name')))
        module.add_options(module_parser)
        module_parser.set_defaults(parser=module_parser,
                                   command=module.exec_command)

    args = parser.parse_args()

    module = importlib.import_module('hb_internal.common.utils')
    hb_error = getattr(module, 'hb_error')
    hb_warning = getattr(module, 'hb_warning')
    OHOSException = getattr(module, 'OHOSException')
    try:
        if args.parser.prog == 'hb set' and 'root_path' in vars(args):
            # Root_path is topdir.
            args.root_path = topdir
        status = args.command(args)
    except KeyboardInterrupt:
        hb_warning('User Abort')
        status = -1
    except OHOSException as exception:
        hb_error(exception.args[0])
        status = -1
    except Exception as exception:
        if not hasattr(args, 'command'):
            parser.print_help()
        else:
            hb_error(traceback.format_exc())
            hb_error(f'Unhandled error: {exception}')
        status = -1

    return status


if __name__ == "__main__":
    sys.exit(main())
