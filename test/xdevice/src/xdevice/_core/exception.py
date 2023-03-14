#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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


class ParamError(Exception):
    def __init__(self, error_msg, error_no=""):
        super(ParamError, self).__init__(error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class LiteDeviceError(Exception):
    def __init__(self, error_msg, error_no=""):
        super(LiteDeviceError, self).__init__(error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class DeviceError(Exception):
    def __init__(self, error_msg, error_no=""):
        super(DeviceError, self).__init__(error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class ExecuteTerminate(Exception):
    def __init__(self, error_msg="ExecuteTerminate", error_no=""):
        super(ExecuteTerminate, self).__init__(error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class ReportException(Exception):
    """
    Exception thrown when a shell command executed on a device takes too long
    to send its output.
    """
    def __init__(self, error_msg="ReportException", error_no=""):
        super(ReportException, self).__init__(error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class LiteParamError(LiteDeviceError):
    def __init__(self, error_msg, error_no=""):
        super(LiteParamError, self).__init__(error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class LiteDeviceConnectError(LiteDeviceError):
    def __init__(self, error_msg, error_no=""):
        super(LiteDeviceConnectError, self).__init__(error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class LiteDeviceMountError(LiteDeviceError):
    def __init__(self, error_msg, error_no=""):
        super(LiteDeviceMountError, self).__init__(error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class LiteDeviceReadOutputError(LiteDeviceError):
    def __init__(self, error_msg, error_no=""):
        super(LiteDeviceReadOutputError, self).__init__(error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class LiteDeviceExecuteCommandError(LiteDeviceError):
    def __init__(self, error_msg, error_no=""):
        super(LiteDeviceExecuteCommandError, self).__init__(
            error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)


class LiteDeviceTimeout(LiteDeviceError):
    def __init__(self, error_msg, error_no=""):
        super(LiteDeviceTimeout, self).__init__(
            error_msg, error_no)
        self.error_msg = error_msg
        self.error_no = error_no

    def __str__(self):
        return str(self.error_msg)