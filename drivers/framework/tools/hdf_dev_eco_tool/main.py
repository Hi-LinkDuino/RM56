#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import sys
import argparse

from command_line.hdf_tool_commands import HdfToolCommands
from hdf_tool_daemon_server import HdfToolDaemonServer
from hdf_tool_exception import HdfToolException
from command_line.hdf_command_error_code import CommandErrorCode


def check_python_version():
    if sys.version_info < (3, 0):
        print('Please run with python version >= 3.0')
        sys.exit(-1)


def main():
    check_python_version()
    commands = HdfToolCommands()
    help_info = 'Tools backend for hdf driver development.'
    arg_parser = argparse.ArgumentParser(description=help_info)
    arg_parser.add_argument('--run_as_daemon', action='store_true')
    arg_parser.add_argument('--server_type', help='command_line or ls_hcs,'
                            'default command_line', default='command_line')
    arg_parser.add_argument('command', help=commands.help())
    arg_parser.add_argument('remainder_args', nargs=argparse.REMAINDER)
    args = arg_parser.parse_args()
    if args.run_as_daemon:
        HdfToolDaemonServer(args.server_type).run()
        return
    try:
        ret = commands.run(args.command, args.remainder_args)
        if ret or isinstance(ret, list):
            print(ret)
    except HdfToolException as exc:
        print('error: {}, {}'.format(exc.error_code, exc.exc_msg))
    except BaseException as exc:
        print('error: {}, {}'.format(CommandErrorCode.UNKNOWN_ERROR, str(exc)))


if __name__ == "__main__":
    main()
