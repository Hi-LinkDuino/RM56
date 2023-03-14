#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import argparse

from command_line.hdf_command_error_code import CommandErrorCode
from hdf_tool_exception import HdfToolException


class HdfToolArgumentParser(argparse.ArgumentParser):
    def exit(self, status=0, message=None):
        if status:
            raise HdfToolException(message, CommandErrorCode.INTERFACE_ERROR)
        exit(status)

    def error(self, message):
        self.exit(2, 'error: %s\n' % message)
