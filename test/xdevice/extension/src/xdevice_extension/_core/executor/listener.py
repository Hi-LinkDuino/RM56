#!/usr/bin/env python3
# coding=utf-8

#
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
#

from xdevice import Plugin
from xdevice import LifeCycle
from xdevice import IListener
from xdevice import platform_logger
from xdevice import TestDescription

from xdevice_extension._core.constants import ListenerType

__all__ = ["CollectingTestListener", "CollectingLiteGTestListener"]

LOG = platform_logger("Listener")


@Plugin(type=Plugin.LISTENER, id=ListenerType.collect)
class CollectingTestListener(IListener):
    """
    listener test status information to the console
    """

    def __init__(self):
        self.tests = []

    def __started__(self, lifecycle, test_result):
        if lifecycle == LifeCycle.TestCase:
            if not test_result.test_class or not test_result.test_name:
                return
            test = TestDescription(test_result.test_class,
                                   test_result.test_name)
            if test not in self.tests:
                self.tests.append(test)

    def __ended__(self, lifecycle, test_result=None, **kwargs):
        pass

    def __skipped__(self, lifecycle, test_result):
        pass

    def __failed__(self, lifecycle, test_result):
        pass

    def get_current_run_results(self):
        return self.tests


@Plugin(type=Plugin.LISTENER, id=ListenerType.collect_lite)
class CollectingLiteGTestListener(IListener):
    """
    listener test status information to the console
    """

    def __init__(self):
        self.tests = []

    def __started__(self, lifecycle, test_result):
        pass

    def __ended__(self, lifecycle, test_result=None, **kwargs):
        del kwargs
        if lifecycle == LifeCycle.TestCase:
            if not test_result.test_class or not test_result.test_name:
                return
            test = TestDescription(test_result.test_class,
                                   test_result.test_name)
            if test not in self.tests:
                self.tests.append(test)

    def __skipped__(self, lifecycle, test_result):
        pass

    def __failed__(self, lifecycle, test_result):
        if lifecycle == LifeCycle.TestCase:
            if not test_result.test_class or not test_result.test_name:
                return
            test = TestDescription(test_result.test_class,
                                   test_result.test_name)
            if test not in self.tests:
                self.tests.append(test)

    def get_current_run_results(self):
        return self.tests
