#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import os
import hdf_utils

from hdf_tool_exception import HdfToolException
from .hdf_command_error_code import CommandErrorCode
from .hdf_tool_argument_parser import HdfToolArgumentParser


class HdfCommandHandlerBase(object):
    def __init__(self):
        self.cmd = 'base'
        self.action_type = 'base_action_type'
        self.handlers = {}
        self.args = {}
        self.parser = HdfToolArgumentParser()

    def run(self):
        self.action_type = self._get_action_type()
        if self.action_type in self.handlers:
            return self.handlers.get(self.action_type)()
        else:
            raise HdfToolException(
                'unknown action_type: "%s" for "%s" cmd' %
                (self.action_type, self.cmd),
                CommandErrorCode.INTERFACE_ERROR)

    def _get_action_type(self):
        try:
            return getattr(self.args, 'action_type')
        except AttributeError:
            return ''

    def check_arg_raise_if_not_exist(self, arg):
        try:
            value = getattr(self.args, arg)
            if not value:
                raise AttributeError()
        except AttributeError:
            raise HdfToolException(
                'argument "--%s" is required for "%s" cmd' %
                (arg, self.cmd), CommandErrorCode.INTERFACE_ERROR)

    def _get_arg(self, arg):
        try:
            value = getattr(self.args, arg)
            return value
        except AttributeError:
            return ''

    def get_args(self):
        args = ['vendor_name', 'module_name',
                'driver_name', 'board_name',
                'kernel_name', "runmode"]
        ret = [self._get_arg('root_dir')]
        for arg in args:
            if arg == "runmode":
                value = self._get_arg(arg)
                if value:
                    board_index = args.index('board_name')+1
                    ret[board_index] = '_'.join([ret[board_index], value])
            else:
                if arg == "board_name":
                    value = self._get_arg(arg)
                    ret.append(value)
                else:
                    value = self._get_arg(arg)
                    if value:
                        value = hdf_utils.WordsConverter(value).lower_case()
                    ret.append(value)
        return tuple(ret)

    @staticmethod
    def check_path_raise_if_not_exist(full_path):
        if not os.path.exists(full_path):
            raise HdfToolException(
                '%s not exist' %
                full_path, CommandErrorCode.TARGET_NOT_EXIST)
