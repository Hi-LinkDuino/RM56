#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


from hdf_tool_exception import HdfToolException
from .hdf_add_handler import HdfAddHandler
from .hdf_delete_handler import HdfDeleteHandler
from .hdf_get_handler import HdfGetHandler
from .hdf_set_handler import HdfSetHandler
from .hdf_ping_handler import HdfPingHandler
from .hdf_command_error_code import CommandErrorCode


class HdfToolCommands(object):
    def __init__(self):
        self.commands = {
            'add': HdfAddHandler,
            'delete': HdfDeleteHandler,
            'get': HdfGetHandler,
            'set': HdfSetHandler,
            'ping': HdfPingHandler,
        }

    def run(self, cmd, args):
        if cmd in self.commands:
            result = self.commands.get(cmd)(args).run()
            return result
        else:
            raise HdfToolException('unknown cmd: "%s"' % cmd,
                                   CommandErrorCode.INTERFACE_ERROR)

    def help(self):
        helps = ['command list:']
        for cmd in self.commands.keys():
            helps.append(cmd)
        return ' '.join(helps)
