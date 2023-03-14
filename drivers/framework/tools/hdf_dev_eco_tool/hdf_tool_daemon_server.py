#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import sys


from hdf_tool_exception import HdfToolException
from command_line.hdf_command_line_server import HdfCommandLineServer


class HdfToolDaemonServer(object):
    def __init__(self, server_type):
        self.server_type = server_type
        self.servers = {
            'command_line': HdfCommandLineServer
        }
        self.read_obj = sys.stdin.buffer
        self.write_obj = sys.stdout.buffer

    def run(self):
        if self.server_type in self.servers:
            self.servers.get(self.server_type)(self.read_obj, self.write_obj).run()
        else:
            raise HdfToolException('unknown type: %s' % self.server_type)
