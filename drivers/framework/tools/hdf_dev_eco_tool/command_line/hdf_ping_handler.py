#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


from .hdf_command_handler_base import HdfCommandHandlerBase


class HdfPingHandler(HdfCommandHandlerBase):
    def __init__(self, args):
        super(HdfPingHandler, self).__init__()
        self.args = args

    def run(self):
        return "I'm here!"
