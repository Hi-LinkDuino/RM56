#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


from command_line.hdf_command_error_code import CommandErrorCode


class HdfToolException(Exception):
    def __init__(self, msg, error_code=CommandErrorCode.UNKNOWN_ERROR):
        super(HdfToolException, self).__init__(msg)
        self.error_code = error_code
        self.exc_msg = msg
