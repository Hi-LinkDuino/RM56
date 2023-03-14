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

import copy
import re
import time
import datetime
from enum import Enum

from xdevice import LifeCycle
from xdevice import IParser
from xdevice import platform_logger
from xdevice import Plugin
from xdevice import check_pub_key_exist
from xdevice import StateRecorder
from xdevice import TestDescription
from xdevice import ResultCode

from xdevice_extension._core.constants import CommonParserType

__all__ = ["CppTestParser", "CppTestListParser",
           "JunitParser", "JSUnitParser", "OHKernelTestParser"]

_INFORMATIONAL_MARKER = "[----------]"
_START_TEST_RUN_MARKER = "[==========] Running"
_TEST_RUN_MARKER = "[==========]"
_GTEST_DRYRUN_MARKER = "Running main() "
_START_TEST_MARKER = "[ RUN      ]"
_OK_TEST_MARKER = "[       OK ]"
_SKIPPED_TEST_MARKER = "[  SKIPPED ]"
_FAILED_TEST_MARKER = "[  FAILED  ]"
_ALT_OK_MARKER = "[    OK    ]"
_TIMEOUT_MARKER = "[ TIMEOUT  ]"

_START_JSUNIT_RUN_MARKER = "[start] start run suites"
_START_JSUNIT_SUITE_RUN_MARKER = "[suite start]"
_START_JSUNIT_SUITE_END_MARKER = "[suite end]"
_END_JSUNIT_RUN_MARKER = "[end] run suites end"
_PASS_JSUNIT_MARKER = "[pass]"
_FAIL_JSUNIT_MARKER = "[fail]"
_ACE_LOG_MARKER = "jsapp"

"""
OpenHarmony Kernel Test
"""
RUNTEST_TEST = "runtest test"
START_TO_TEST = "Start to test"
FINISHED_TO_TEST = "Finished to test"
TIMEOUT_TESTCASES = "Timeout testcases"
FAIL_DOT = "FAIL."
PASS_DOT = "PASS."

LOG = platform_logger("Parser")


@Plugin(type=Plugin.PARSER, id=CommonParserType.cpptest)
class CppTestParser(IParser):
    def __init__(self):
        self.state_machine = StateRecorder()
        self.suite_name = ""
        self.listeners = []
        self.product_info = {}
        self.is_params = False

    def get_suite_name(self):
        return self.suite_name

    def get_listeners(self):
        return self.listeners

    def __process__(self, lines):
        if not self.state_machine.suites_is_started():
            self.state_machine.trace_logs.extend(lines)
        for line in lines:
            if not check_pub_key_exist():
                LOG.debug(line)
            self.parse(line)

    def __done__(self):
        suite_result = self.state_machine.get_suites()
        if not suite_result.suites_name:
            return
        for listener in self.get_listeners():
            suites = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuites, test_result=suites,
                               suites_name=suites.suites_name,
                               product_info=suites.product_info)
        self.state_machine.current_suites = None

    def parse(self, line):

        if self.state_machine.suites_is_started() or line.startswith(
                _TEST_RUN_MARKER):
            if line.startswith(_START_TEST_RUN_MARKER):
                message = line[len(_TEST_RUN_MARKER):].strip()
                self.handle_suites_started_tag(message)
            elif line.startswith(_INFORMATIONAL_MARKER):
                pattern = r"(.*) (\(\d+ ms total\))"
                message = line[len(_INFORMATIONAL_MARKER):].strip()
                if re.match(pattern, line.strip()):
                    self.handle_suite_ended_tag(message)
                elif re.match(r'(\d+) test[s]? from (.*)', message):
                    self.handle_suite_started_tag(message)
            elif line.startswith(_TEST_RUN_MARKER):
                if not self.state_machine.suites_is_running():
                    return
                message = line[len(_TEST_RUN_MARKER):].strip()
                self.handle_suites_ended_tag(message)
            elif line.startswith(_START_TEST_MARKER):
                # Individual test started
                message = line[len(_START_TEST_MARKER):].strip()
                self.handle_test_started_tag(message)
            else:
                self.process_test(line)

    def process_test(self, line):
        if _SKIPPED_TEST_MARKER in line:
            message = line[line.index(_SKIPPED_TEST_MARKER) + len(
                _SKIPPED_TEST_MARKER):].strip()
            if not self.state_machine.test_is_running():
                LOG.error(
                    "Found {} without {} before, wrong GTest log format".
                    format(line, _START_TEST_MARKER))
                return
            self.handle_test_ended_tag(message, ResultCode.SKIPPED)
        elif _OK_TEST_MARKER in line:
            message = line[line.index(_OK_TEST_MARKER) + len(
                _OK_TEST_MARKER):].strip()
            if not self.state_machine.test_is_running():
                LOG.error(
                    "Found {} without {} before, wrong GTest log format".
                    format(line, _START_TEST_MARKER))
                return
            self.handle_test_ended_tag(message, ResultCode.PASSED)
        elif _ALT_OK_MARKER in line:
            message = line[line.index(_ALT_OK_MARKER) + len(
                _ALT_OK_MARKER):].strip()
            self.fake_run_marker(message)
            self.handle_test_ended_tag(message, ResultCode.PASSED)
        elif _FAILED_TEST_MARKER in line:
            message = line[line.index(_FAILED_TEST_MARKER) + len(
                _FAILED_TEST_MARKER):].strip()
            if not self.state_machine.suite_is_running():
                return
            if not self.state_machine.test_is_running():
                self.fake_run_marker(message)
            self.handle_test_ended_tag(message, ResultCode.FAILED)
        elif _TIMEOUT_MARKER in line:
            message = line[line.index(_TIMEOUT_MARKER) + len(
                _TIMEOUT_MARKER):].strip()
            self.fake_run_marker(message)
            self.handle_test_ended_tag(message, ResultCode.FAILED)
        elif self.state_machine.test_is_running():
            self.append_test_output(line)

    def handle_test_suite_failed(self, error_msg):
        error_msg = "Unknown error" if error_msg is None else error_msg
        LOG.info("Test run failed: {}".format(error_msg))
        if self.state_machine.test_is_running():
            self.state_machine.test().is_completed = True
            for listener in self.get_listeners():
                test_result = copy.copy(self.currentTestResult)
                listener.__failed__(LifeCycle.TestCase, test_result)
                listener.__ended__(LifeCycle.TestCase, test_result)
        self.state_machine.suite().stacktrace = error_msg
        self.state_machine.suite().is_completed = True
        for listener in self.get_listeners():
            suite_result = copy.copy(self.currentSuiteResult)
            listener.__failed__(LifeCycle.TestSuite, suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite_result)

    def handle_test_started_tag(self, message):
        test_class, test_name, _ = self.parse_test_description(
            message)
        test_result = self.state_machine.test(reset=True)
        test_result.test_class = test_class
        test_result.test_name = test_name
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__started__(LifeCycle.TestCase, test_result)

    @classmethod
    def parse_test_description(cls, message):
        run_time = 0
        matcher = re.match(r'(.*) \((\d+) ms\)', message)
        if matcher:
            test_class, test_name = matcher.group(1).rsplit(".", 1)
            run_time = int(matcher.group(2))
        else:
            test_class, test_name = message.rsplit(".", 1)
        return test_class, test_name, run_time

    def handle_test_ended_tag(self, message, test_status):
        test_class, test_name, run_time = self.parse_test_description(
            message)
        test_result = self.state_machine.test()
        test_result.run_time = int(run_time)
        test_result.code = test_status.value
        test_result.current = self.state_machine.running_test_index + 1
        if not test_result.is_running():
            LOG.error(
                "Test has no start tag when trying to end test: %s", message)
            return
        found_unexpected_test = False
        if test_result.test_class != test_class:
            LOG.error(
                "Expected class: {} but got:{} ".format(test_result.test_class,
                                                        test_class))
            found_unexpected_test = True
        if test_result.test_name != test_name:
            LOG.error(
                "Expected test: {} but got: {}".format(test_result.test_name,
                                                       test_name))
            found_unexpected_test = True

        if found_unexpected_test or ResultCode.FAILED == test_status:
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__failed__(LifeCycle.TestCase, result)
        elif ResultCode.SKIPPED == test_status:
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__skipped__(LifeCycle.TestCase, result)

        self.state_machine.test().is_completed = True
        for listener in self.get_listeners():
            result = copy.copy(test_result)
            listener.__ended__(LifeCycle.TestCase, result)
        self.state_machine.running_test_index += 1

    def fake_run_marker(self, message):
        fake_marker = re.compile(" +").split(message)
        self.handle_test_started_tag(fake_marker)

    def handle_suites_started_tag(self, message):
        self.state_machine.get_suites(reset=True)
        matcher = re.match(r'Running (\d+) test[s]? from .*', message)
        expected_test_num = int(matcher.group(1)) if matcher else -1
        if expected_test_num >= 0:
            test_suites = self.state_machine.get_suites()
            test_suites.suites_name = self.get_suite_name()
            test_suites.test_num = expected_test_num
            test_suites.product_info = self.product_info
            for listener in self.get_listeners():
                suite_report = copy.copy(test_suites)
                listener.__started__(LifeCycle.TestSuites, suite_report)

    def handle_suite_started_tag(self, message):
        self.state_machine.suite(reset=True)
        matcher = re.match(r'(\d+) test[s]? from (.*)', message)
        expected_test_num = int(matcher.group(1)) if matcher else -1
        if expected_test_num >= 0:
            test_suite = self.state_machine.suite()
            test_suite.suite_name = matcher.group(2)
            test_suite.test_num = expected_test_num
            for listener in self.get_listeners():
                suite_report = copy.copy(test_suite)
                listener.__started__(LifeCycle.TestSuite, suite_report)

    def handle_suite_ended_tag(self, message):
        self.state_machine.running_test_index = 0
        suite_result = self.state_machine.suite()
        matcher = re.match(r'.*\((\d+) ms total\)', message)
        if matcher:
            suite_result.run_time = int(matcher.group(1))
        suite_result.is_completed = True
        for listener in self.get_listeners():
            suite = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite, is_clear=True)

    def handle_suites_ended_tag(self, message):
        suites = self.state_machine.get_suites()
        matcher = re.match(r'.*\((\d+) ms total\)', message)
        if matcher:
            suites.run_time = int(matcher.group(1))
        suites.is_completed = True
        for listener in self.get_listeners():
            copy_suites = copy.copy(suites)
            listener.__ended__(LifeCycle.TestSuites, test_result=copy_suites,
                               suites_name=suites.suites_name,
                               product_info=suites.product_info,
                               suite_report=True)

    def append_test_output(self, message):
        if self.state_machine.test().stacktrace:
            self.state_machine.test().stacktrace += "\r\n"
        self.state_machine.test().stacktrace += message

    @staticmethod
    def handle_test_run_failed(error_msg):
        if not error_msg:
            error_msg = "Unknown error"
        if not check_pub_key_exist():
            LOG.debug("error_msg:%s" % error_msg)

    def mark_test_as_blocked(self, test):
        if not self.state_machine.current_suite and not test.class_name:
            return
        suites_result = self.state_machine.get_suites(reset=True)
        suites_result.suites_name = self.get_suite_name()
        suite_name = self.state_machine.current_suite.suite_name if \
            self.state_machine.current_suite else None
        suite_result = self.state_machine.suite(reset=True)
        test_result = self.state_machine.test(reset=True)
        suite_result.suite_name = suite_name or test.class_name
        suite_result.suite_num = 1
        test_result.test_class = test.class_name
        test_result.test_name = test.test_name
        test_result.stacktrace = "error_msg: run crashed"
        test_result.num_tests = 1
        test_result.run_time = 0
        test_result.code = ResultCode.BLOCKED.value
        for listener in self.get_listeners():
            suite_report = copy.copy(suites_result)
            listener.__started__(LifeCycle.TestSuites, suite_report)
        for listener in self.get_listeners():
            suite_report = copy.copy(suite_result)
            listener.__started__(LifeCycle.TestSuite, suite_report)
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__started__(LifeCycle.TestCase, test_result)
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__ended__(LifeCycle.TestCase, test_result)
        for listener in self.get_listeners():
            suite_report = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite_report,
                               is_clear=True)
        self.__done__()


@Plugin(type=Plugin.PARSER, id=CommonParserType.cpptest_list)
class CppTestListParser(IParser):
    def __init__(self):
        self.last_test_class_name = None
        self.tests = []

    def __process__(self, lines):
        for line in lines:
            if not check_pub_key_exist():
                LOG.debug(line)
            self.parse(line)

    def __done__(self):
        pass

    def parse(self, line):
        class_matcher = re.match('^([a-zA-Z]+.*)\\.$', line)
        method_matcher = re.match('\\s+([a-zA-Z_]+[\\S]*)(.*)?(\\s+.*)?$',
                                  line)
        if class_matcher:
            self.last_test_class_name = class_matcher.group(1)
        elif method_matcher:
            if not self.last_test_class_name:
                LOG.error("parsed new test case name %s but no test class name"
                          " has been set" % line)
            else:
                test = TestDescription(self.last_test_class_name,
                                       method_matcher.group(1))
                self.tests.append(test)
        else:
            if not check_pub_key_exist():
                LOG.debug("line ignored: %s" % line)


class StatusCodes(Enum):
    FAILURE = -2
    START = 1
    ERROR = -1
    SUCCESS = 0
    IN_PROGRESS = 2
    IGNORE = -3
    BLOCKED = 3


class Prefixes(Enum):
    STATUS = "INSTRUMENTATION_STATUS: "
    STATUS_CODE = "INSTRUMENTATION_STATUS_CODE: "
    STATUS_FAILED = "INSTRUMENTATION_FAILED: "
    CODE = "INSTRUMENTATION_CODE: "
    RESULT = "INSTRUMENTATION_RESULT: "
    TIME_REPORT = "Time: "


@Plugin(type=Plugin.PARSER, id=CommonParserType.junit)
class JunitParser(IParser):
    def __init__(self):
        self.state_machine = StateRecorder()
        self.suite_name = ""
        self.listeners = []
        self.current_key = None
        self.current_value = None
        self.start_time = datetime.datetime.now()
        self.test_time = 0
        self.test_run_finished = False

    def get_suite_name(self):
        return self.suite_name

    def get_listeners(self):
        return self.listeners

    def __process__(self, lines):
        for line in lines:
            if not check_pub_key_exist():
                LOG.debug(line)
            self.parse(line)

    def __done__(self):
        suite_result = self.state_machine.suite()
        suite_result.run_time = self.test_time
        suite_result.is_completed = True
        for listener in self.get_listeners():
            suite = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite,
                               suite_report=True)
        self.state_machine.current_suite = None

    def parse(self, line):
        if line.startswith(Prefixes.STATUS_CODE.value):
            self.submit_current_key_value()
            self.parse_status_code(line)
        elif line.startswith(Prefixes.STATUS.value):
            self.submit_current_key_value()
            self.parse_key(line, len(Prefixes.STATUS.value))
        elif line.startswith(Prefixes.RESULT.value):
            self.test_run_finished = True
        elif line.startswith(Prefixes.STATUS_FAILED.value) or \
                line.startswith(Prefixes.CODE.value):
            self.submit_current_key_value()
            self.test_run_finished = True
        elif line.startswith(Prefixes.TIME_REPORT.value):
            self.parse_time(line)
        else:
            if self.current_value:
                self.current_value = self.current_value + r"\r\n"
                self.current_value = self.current_value + line
            elif line:
                pass

    def parse_key(self, line, key_start_pos):
        key_value = line[key_start_pos:].split("=", 1)
        if len(key_value) == 2:
            self.current_key = key_value[0]
            self.current_value = key_value[1]

    def parse_time(self, line):
        message = line[len(Prefixes.TIME_REPORT.value):]
        self.test_time = float(message.replace(",", "")) * 1000

    @staticmethod
    def check_legality(name):
        if not name or name == "null":
            return False
        return True

    def parse_status_code(self, line):
        value = line[len(Prefixes.STATUS_CODE.value):]
        test_info = self.state_machine.test()
        test_info.code = int(value)
        if test_info.code != StatusCodes.IN_PROGRESS:
            if self.check_legality(test_info.test_class) and \
                    self.check_legality(test_info.test_name):
                self.report_result(test_info)
                self.clear_current_test_info()

    def clear_current_test_info(self):
        self.state_machine.current_test = None

    def submit_current_key_value(self):
        if self.current_key and self.current_value:
            status_value = self.current_value
            test_info = self.state_machine.test()
            if self.current_key == "class":
                test_info.test_class = status_value
            elif self.current_key == "test":
                test_info.test_name = status_value
            elif self.current_key == "numtests":
                test_info.num_tests = int(status_value)
            elif self.current_key == "Error":
                self.handle_test_run_failed(status_value)
            elif self.current_key == "stack":
                test_info.stacktrace = status_value
            elif self.current_key == "stream":
                pass
            self.current_key = None
            self.current_value = None

    def report_result(self, test_info):
        if not test_info.test_name or not test_info.test_class:
            LOG.info("invalid instrumentation status bundle")
            return
        test_info.is_completed = True
        self.report_test_run_started(test_info)
        if test_info.code == StatusCodes.START.value:
            self.start_time = datetime.datetime.now()
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                listener.__started__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.FAILURE.value:
            self.state_machine.running_test_index += 1
            test_info.current = self.state_machine.running_test_index
            end_time = datetime.datetime.now()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.FAILED.value
                listener.__ended__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.ERROR.value:
            self.state_machine.running_test_index += 1
            test_info.current = self.state_machine.running_test_index
            end_time = datetime.datetime.now()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.FAILED.value
                listener.__ended__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.SUCCESS.value:
            self.state_machine.running_test_index += 1
            test_info.current = self.state_machine.running_test_index
            end_time = datetime.datetime.now()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.PASSED.value
                listener.__ended__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.IGNORE.value:
            end_time = datetime.datetime.now()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.SKIPPED.value
                listener.__skipped__(LifeCycle.TestCase, result)
        elif test_info.code == StatusCodes.BLOCKED.value:
            test_info.current = self.state_machine.running_test_index
            end_time = datetime.datetime.now()
            run_time = (end_time - self.start_time).total_seconds()
            test_info.run_time = int(run_time * 1000)
            for listener in self.get_listeners():
                result = copy.copy(test_info)
                result.code = ResultCode.BLOCKED.value
                listener.__ended__(LifeCycle.TestCase, result)

        self.output_stack_trace(test_info)

    @classmethod
    def output_stack_trace(cls, test_info):
        if check_pub_key_exist():
            return
        if test_info.stacktrace:
            stack_lines = test_info.stacktrace.split(r"\r\n")
            LOG.error("stacktrace information is:")
            for line in stack_lines:
                line.strip()
                if line:
                    LOG.error(line)

    def report_test_run_started(self, test_result):
        test_suite = self.state_machine.suite()
        if not self.state_machine.suite().is_started:
            if not test_suite.test_num or not test_suite.suite_name:
                test_suite.suite_name = self.get_suite_name()
                test_suite.test_num = test_result.num_tests
                for listener in self.get_listeners():
                    suite_report = copy.copy(test_suite)
                    listener.__started__(LifeCycle.TestSuite, suite_report)

    @staticmethod
    def handle_test_run_failed(error_msg):
        if not error_msg:
            error_msg = "Unknown error"
        if not check_pub_key_exist():
            LOG.debug("error_msg:%s" % error_msg)

    def mark_test_as_failed(self, test):
        test_info = self.state_machine.test()
        if test_info:
            test_info.test_class = test.class_name
            test_info.test_name = test.test_name
            test_info.code = StatusCodes.START.value
            self.report_result(test_info)
            test_info.code = StatusCodes.FAILURE.value
            self.report_result(test_info)
            self.__done__()

    def mark_test_as_blocked(self, test):
        test_info = self.state_machine.test()
        if test_info:
            test_info.test_class = test.class_name
            test_info.test_name = test.test_name
            test_info.num_tests = 1
            test_info.run_time = 0
            test_info.code = StatusCodes.START.value
            self.report_result(test_info)
            test_info.code = StatusCodes.BLOCKED.value
            self.report_result(test_info)
            self.__done__()


@Plugin(type=Plugin.PARSER, id=CommonParserType.jsunit)
class JSUnitParser(IParser):
    last_line = ""
    pattern = r"(\d{1,2}-\d{1,2}\s\d{1,2}:\d{1,2}:\d{1,2}\.\d{3}) "

    def __init__(self):
        self.state_machine = StateRecorder()
        self.suites_name = ""
        self.listeners = []
        self.suite_name = ""

    def get_listeners(self):
        return self.listeners

    def __process__(self, lines):
        if not self.state_machine.suites_is_started():
            self.state_machine.trace_logs.extend(lines)
        for line in lines:
            self.parse(line)

    def __done__(self):
        suite_result = self.state_machine.suite()
        suite_result.is_completed = True
        for listener in self.get_listeners():
            suite = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuites, suite,
                               suite_report=True)
        self.state_machine.current_suite = None

    def parse(self, line):
        line = line.strip()
        if (self.state_machine.suites_is_started() or line.find(
                _START_JSUNIT_RUN_MARKER) != -1) and line.lower().find(
                _ACE_LOG_MARKER) != -1:
            if line.find(_START_JSUNIT_RUN_MARKER) != -1:
                self.handle_suites_started_tag()
            elif line.endswith(_END_JSUNIT_RUN_MARKER):
                self.handle_suites_ended_tag()
            elif line.find(_START_JSUNIT_SUITE_RUN_MARKER) != -1:
                self.handle_suite_started_tag(line.strip())
            elif line.endswith(_START_JSUNIT_SUITE_END_MARKER):
                self.handle_suite_ended_tag()
            elif _PASS_JSUNIT_MARKER in line or _FAIL_JSUNIT_MARKER \
                    in line:
                self.handle_one_test_tag(line.strip())
            self.last_line = line

    def parse_test_description(self, message):
        pattern = r"\[(pass|fail)\]"
        year = time.strftime("%Y")
        match_list = ["app Log:", "JSApp:", "JsApp:"]
        for keyword in match_list:
            if keyword in message:
                filter_message = message.split(r"{0}".format(keyword))[1].strip()
                break
        end_time = "%s-%s" % \
                   (year, re.match(self.pattern, message).group().strip())
        start_time = "%s-%s" % \
                     (year, re.match(self.pattern,
                                     self.last_line.strip()).group().strip())
        start_timestamp = int(time.mktime(
            time.strptime(start_time, "%Y-%m-%d %H:%M:%S.%f"))) * 1000 + int(
            start_time.split(".")[-1])
        end_timestamp = int(time.mktime(
            time.strptime(end_time, "%Y-%m-%d %H:%M:%S.%f"))) * 1000 + int(
            end_time.split(".")[-1])
        run_time = end_timestamp - start_timestamp
        _, status_end_index = re.match(pattern, filter_message).span()
        status = filter_message[:status_end_index]
        if " ;" in filter_message:
            test_name = filter_message[status_end_index:
                                       str(filter_message).find(" ;")]
        else:
            test_name = filter_message[status_end_index:]
        status_dict = {"pass": ResultCode.PASSED, "fail": ResultCode.FAILED,
                       "ignore": ResultCode.SKIPPED}
        status = status_dict.get(status[1:-1])
        return test_name, status, run_time

    def handle_suites_started_tag(self):
        self.state_machine.get_suites(reset=True)
        test_suites = self.state_machine.get_suites()
        test_suites.suites_name = self.suites_name
        test_suites.test_num = 0
        for listener in self.get_listeners():
            suite_report = copy.copy(test_suites)
            listener.__started__(LifeCycle.TestSuites, suite_report)

    def handle_suites_ended_tag(self):
        suites = self.state_machine.get_suites()
        suites.is_completed = True

        for listener in self.get_listeners():
            listener.__ended__(LifeCycle.TestSuites, test_result=suites,
                               suites_name=suites.suites_name)

    def handle_one_test_tag(self, message):
        test_name, status, run_time = \
            self.parse_test_description(message)
        test_result = self.state_machine.test(reset=True)
        test_suite = self.state_machine.suite()
        test_result.test_class = test_suite.suite_name
        test_result.test_name = test_name
        test_result.run_time = run_time
        test_result.code = status.value
        test_result.current = self.state_machine.running_test_index + 1
        self.state_machine.suite().run_time += run_time
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__started__(LifeCycle.TestCase, test_result)

        test_suites = self.state_machine.get_suites()
        found_unexpected_test = False

        if found_unexpected_test or ResultCode.FAILED == status:
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__failed__(LifeCycle.TestCase, result)
        elif ResultCode.SKIPPED == status:
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__skipped__(LifeCycle.TestCase, result)

        self.state_machine.test().is_completed = True
        test_suite.test_num += 1
        test_suites.test_num += 1
        for listener in self.get_listeners():
            result = copy.copy(test_result)
            listener.__ended__(LifeCycle.TestCase, result)
        self.state_machine.running_test_index += 1

    def fake_run_marker(self, message):
        fake_marker = re.compile(" +").split(message)
        self.processTestStartedTag(fake_marker)

    def handle_suite_started_tag(self, message):
        self.state_machine.suite(reset=True)
        test_suite = self.state_machine.suite()
        if re.match(r".*\[suite start\].*", message):
            _, index = re.match(r".*\[suite start\]", message).span()
        if message[index:]:
            test_suite.suite_name = message[index:]
        else:
            test_suite.suite_name = self.suite_name
        test_suite.test_num = 0
        for listener in self.get_listeners():
            suite_report = copy.copy(test_suite)
            listener.__started__(LifeCycle.TestSuite, suite_report)

    def handle_suite_ended_tag(self):
        suite_result = self.state_machine.suite()
        suites = self.state_machine.get_suites()
        suite_result.run_time = suite_result.run_time
        suites.run_time += suite_result.run_time
        suite_result.is_completed = True

        for listener in self.get_listeners():
            suite = copy.copy(suite_result)
            listener.__ended__(LifeCycle.TestSuite, suite, is_clear=True)

    def append_test_output(self, message):
        if self.state_machine.test().stacktrace:
            self.state_machine.test().stacktrace = \
                "%s\r\n" % self.state_machine.test().stacktrace
        self.state_machine.test().stacktrace = \
            ''.join((self.state_machine.test().stacktrace, message))


@Plugin(type=Plugin.PARSER, id=CommonParserType.oh_kernel_test)
class OHKernelTestParser(IParser):

    def __init__(self):
        self.state_machine = StateRecorder()
        self.suites_name = ""
        self.listeners = []

    def get_listeners(self):
        return self.listeners

    def __process__(self, lines):
        if not self.state_machine.suites_is_started():
            self.state_machine.trace_logs.extend(lines)
        for line in lines:
            self.parse(line)

    def __done__(self):
        pass

    def parse(self, line):
        line = re.sub('\x1b.*?m', '', line)
        if self.state_machine.suites_is_started() or RUNTEST_TEST in line:
            if RUNTEST_TEST in line:
                self.handle_suites_started_tag(line)
            elif START_TO_TEST in line:
                self.handle_suite_start_tag(line)
            elif FINISHED_TO_TEST in line:
                self.handle_suite_end_tag(line)
            elif line.endswith(PASS_DOT) or line.endswith(FAIL_DOT):
                self.handle_one_test_case_tag(line)
            elif TIMEOUT_TESTCASES in line:
                self.handle_suites_ended_tag(line)

    def handle_suites_started_tag(self, line):
        self.state_machine.get_suites(reset=True)
        test_suites = self.state_machine.get_suites()
        test_suites.suites_name = self.suites_name
        test_suites.test_num = 0
        for listener in self.get_listeners():
            suite_report = copy.copy(test_suites)
            listener.__started__(LifeCycle.TestSuites, suite_report)

    def handle_suites_ended_tag(self, line):
        suites = self.state_machine.get_suites()
        suites.is_completed = True

        for listener in self.get_listeners():
            listener.__ended__(LifeCycle.TestSuites, test_result=suites,
                               suites_name=suites.suites_name)

    def handle_suite_start_tag(self, line):
        pattern = "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}" \
                  " Start to test (.+)$"
        matcher = re.match(pattern, line)
        if matcher and matcher.group(1):
            self.state_machine.suite(reset=True)
            test_suite = self.state_machine.suite()
            test_suite.suite_name = matcher.group(1)
            for listener in self.get_listeners():
                suite_report = copy.copy(test_suite)
                listener.__started__(LifeCycle.TestSuite, suite_report)

    def handle_suite_end_tag(self, line):
        pattern = "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}" \
                  " Finished to test (.+)$"
        matcher = re.match(pattern, line)
        if matcher and matcher.group(1):
            suite_result = self.state_machine.suite()
            suites = self.state_machine.get_suites()
            suite_result.run_time = suite_result.run_time
            suites.run_time += suite_result.run_time
            suite_result.is_completed = True

            for listener in self.get_listeners():
                suite = copy.copy(suite_result)
                listener.__ended__(LifeCycle.TestSuite, suite, is_clear=True)

    def handle_one_test_case_tag(self, line):
        pattern = "^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2} (.+) " \
                  "(PASS|FAIL)\\.$"
        matcher = re.match(pattern, line)
        if not (matcher and matcher.group(1) and matcher.group(2)):
            return
        test_result = self.state_machine.test(reset=True)
        test_suite = self.state_machine.suite()
        test_result.test_class = test_suite.suite_name
        test_result.test_name = matcher.group(1)
        test_result.current = self.state_machine.running_test_index + 1
        for listener in self.get_listeners():
            test_result = copy.copy(test_result)
            listener.__started__(LifeCycle.TestCase, test_result)

        test_suites = self.state_machine.get_suites()
        if PASS_DOT in line:
            test_result.code = ResultCode.PASSED.value
        elif FAIL_DOT in line:
            test_result.code = ResultCode.FAILED.value
            for listener in self.get_listeners():
                result = copy.copy(test_result)
                listener.__failed__(LifeCycle.TestCase, result)
        self.state_machine.test().is_completed = True
        test_suite.test_num += 1
        test_suites.test_num += 1
        for listener in self.get_listeners():
            result = copy.copy(test_result)
            listener.__ended__(LifeCycle.TestCase, result)
        self.state_machine.running_test_index += 1
