#!/usr/bin/env python
# -*- coding: utf-8 -*-
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


class CircularDependencyError(Exception):
    pass


class CrossProcessCircularDependencyError(Exception):
    pass


class InvertDependencyError(Exception):
    pass


class RunOnCreateDependencyError(Exception):
    pass


class NotSupportedBootphaseError(Exception):
    pass


class SystemAbilityNameConflictError(Exception):
    pass


class BadFormatXMLError(Exception):
    def __init__(self, message, file_in_process):
        super().__init__(message, file_in_process)
        self.message = message
        self.file_in_process = file_in_process

    def __str__(self):
        return "In xml file: {}, {}".format(self.file_in_process, self.message)
