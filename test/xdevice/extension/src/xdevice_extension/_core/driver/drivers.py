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

import os
import time
import json
import shutil
import zipfile
import tempfile
import stat
import re
from dataclasses import dataclass

from xdevice import ParamError
from xdevice import ExecuteTerminate
from xdevice import IDriver
from xdevice import platform_logger
from xdevice import Plugin
from xdevice import get_plugin
from xdevice import JsonParser
from xdevice import ShellHandler
from xdevice import TestDescription
from xdevice import ResourceManager
from xdevice import get_device_log_file
from xdevice import check_result_report
from xdevice import get_kit_instances
from xdevice import get_config_value
from xdevice import do_module_kit_setup
from xdevice import do_module_kit_teardown

from xdevice_extension._core.constants import DeviceTestType
from xdevice_extension._core.constants import DeviceConnectorType
from xdevice_extension._core.constants import CommonParserType
from xdevice_extension._core.constants import FilePermission
from xdevice_extension._core.environment.dmlib import DisplayOutputReceiver
from xdevice_extension._core.exception import ShellCommandUnresponsiveException
from xdevice_extension._core.exception import HapNotSupportTest
from xdevice_extension._core.exception import HdcCommandRejectedException
from xdevice_extension._core.exception import HdcError
from xdevice_extension._core.testkit.kit import junit_para_parse
from xdevice_extension._core.testkit.kit import reset_junit_para
from xdevice_extension._core.utils import get_filename_extension
from xdevice_extension._core.utils import start_standing_subprocess
from xdevice_extension._core.executor.listener import CollectingTestListener
from xdevice_extension._core.testkit.kit import gtest_para_parse
from xdevice_extension._core.environment.dmlib import process_command_ret


__all__ = ["CppTestDriver", "HapTestDriver", "OHKernelTestDriver",
           "JSUnitTestDriver", "JUnitTestDriver", "RemoteTestRunner",
           "RemoteDexRunner", "disable_keyguard"]
LOG = platform_logger("Drivers")
DEFAULT_TEST_PATH = "/%s/%s/" % ("data", "test")
ON_DEVICE_TEST_DIR_LOCATION = "/%s/%s/%s/" % ("data", "local", "tmp")

FAILED_RUN_TEST_ATTEMPTS = 3
TIME_OUT = 900 * 1000


@dataclass
class ZunitConst(object):
    z_unit_app = "ohos.unittest.App"
    output_dir = "OUTPUT_DIR="
    output_file = "OUTPUT_FILE="
    test_class = "TEST_CLASS="
    exec_class = "EXEC_CLASS="
    exec_method = "EXEC_METHOD="
    exec_level = "EXEC_LEVEL="
    jacoco_exec_file = "JACOCO_EXEC_FILE="
    jtest_status_filename = "jtest_status.txt"
    remote_command_dir = "commandtmp"


def get_level_para_string(level_string):
    level_list = list(set(level_string.split(",")))
    level_para_string = ""
    for item in level_list:
        if not item.isdigit():
            continue
        item = item.strip(" ")
        level_para_string = "%sLevel%s," % (level_para_string, item)
    level_para_string = level_para_string.strip(",")
    return level_para_string


def get_execute_java_test_files(suite_file):
    java_test_file = ""
    test_info_file = "%s.info" % suite_file[:suite_file.rfind(".")]
    if not os.path.exists(test_info_file):
        return java_test_file
    try:
        test_info_file_open = os.open(test_info_file, os.O_RDWR,
                                      stat.S_IWUSR | stat.S_IRUSR)
        with os.fdopen(test_info_file_open, "r") as file_desc:
            lines = file_desc.readlines()
            for line in lines:
                class_name, _ = line.split(',', 1)
                class_name = class_name.strip()
                if not class_name.endswith("Test"):
                    continue
                java_test_file = "%s%s," % (java_test_file, class_name)
    except(IOError, ValueError) as err_msg:
        LOG.exception("Error to read info file: ", err_msg, exc_info=False)
    if java_test_file != "":
        java_test_file = java_test_file[:-1]
    return java_test_file


def get_java_test_para(testcase, testlevel):
    exec_class = "*"
    exec_method = "*"
    exec_level = ""

    if "" != testcase and "" == testlevel:
        pos = testcase.rfind(".")
        if pos != -1:
            exec_class = testcase[0:pos]
            exec_method = testcase[pos + 1:]
            exec_level = ""
        else:
            exec_class = "*"
            exec_method = testcase
            exec_level = ""
    elif "" == testcase and "" != testlevel:
        exec_class = "*"
        exec_method = "*"
        exec_level = get_level_para_string(testlevel)

    return exec_class, exec_method, exec_level


def get_xml_output(config, json_config):
    xml_output = config.testargs.get("xml-output")
    if not xml_output:
        if get_config_value('xml-output', json_config.get_driver(), False):
            xml_output = get_config_value('xml-output',
                                          json_config.get_driver(), False)
        else:
            xml_output = "false"
    else:
        xml_output = xml_output[0]
    xml_output = str(xml_output).lower()
    return xml_output


def get_result_savepath(testsuit_path, result_rootpath):
    findkey = "%stests%s" % (os.sep, os.sep)
    filedir, _ = os.path.split(testsuit_path)
    pos = filedir.find(findkey)
    if -1 != pos:
        subpath = filedir[pos + len(findkey):]
        pos1 = subpath.find(os.sep)
        if -1 != pos1:
            subpath = subpath[pos1 + len(os.sep):]
            result_path = os.path.join(result_rootpath, "result", subpath)
        else:
            result_path = os.path.join(result_rootpath, "result")
    else:
        result_path = os.path.join(result_rootpath, "result")

    if not os.path.exists(result_path):
        os.makedirs(result_path)

    LOG.info("result_savepath = %s" % result_path)
    return result_path


# all testsuit common Unavailable test result xml
def _create_empty_result_file(filepath, filename, error_message):
    error_message = str(error_message)
    error_message = error_message.replace("\"", "&quot;")
    error_message = error_message.replace("<", "&lt;")
    error_message = error_message.replace(">", "&gt;")
    error_message = error_message.replace("&", "&amp;")
    if filename.endswith(".hap"):
        filename = filename.split(".")[0]
    if not os.path.exists(filepath):
        file_open = os.open(filepath, os.O_WRONLY | os.O_CREAT | os.O_APPEND,
                            0o755)
        with os.fdopen(file_open, "w") as file_desc:
            time_stamp = time.strftime("%Y-%m-%d %H:%M:%S",
                                       time.localtime())
            file_desc.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            file_desc.write('<testsuites tests="0" failures="0" '
                            'disabled="0" errors="0" timestamp="%s" '
                            'time="0" name="AllTests">\n' % time_stamp)
            file_desc.write(
                '  <testsuite name="%s" tests="0" failures="0" '
                'disabled="0" errors="0" time="0.0" '
                'unavailable="1" message="%s">\n' %
                (filename, error_message))
            file_desc.write('  </testsuite>\n')
            file_desc.write('</testsuites>\n')
            file_desc.flush()
    return


class ResultManager(object):
    def __init__(self, testsuit_path, result_rootpath, device,
                 device_testpath):
        self.testsuite_path = testsuit_path
        self.result_rootpath = result_rootpath
        self.device = device
        self.device_testpath = device_testpath
        self.testsuite_name = os.path.basename(self.testsuite_path)
        self.is_coverage = False

    def set_is_coverage(self, is_coverage):
        self.is_coverage = is_coverage

    def get_test_results(self, error_message=""):
        # Get test result files
        filepath = self.obtain_test_result_file()
        if not os.path.exists(filepath):
            _create_empty_result_file(filepath, self.testsuite_name,
                                      error_message)

        # Get coverage data files
        if self.is_coverage:
            self.obtain_coverage_data()

        return filepath

    def obtain_test_result_file(self):
        result_savepath = get_result_savepath(self.testsuite_path,
                                              self.result_rootpath)
        if self.testsuite_path.endswith('.hap'):
            filepath = os.path.join(result_savepath, "%s.xml" % str(
                self.testsuite_name).split(".")[0])

            remote_result_name = ""
            if self.device.is_file_exist(os.path.join(self.device_testpath,
                                                      "testcase_result.xml")):
                remote_result_name = "testcase_result.xml"
            elif self.device.is_file_exist(os.path.join(self.device_testpath,
                                                        "report.xml")):
                remote_result_name = "report.xml"

            if remote_result_name:
                self.device.pull_file(
                    os.path.join(self.device_testpath, remote_result_name),
                    filepath)
            else:
                LOG.error("%s no report file", self.device_testpath)

        else:
            filepath = os.path.join(result_savepath, "%s.xml" %
                                    self.testsuite_name)
            remote_result_file = os.path.join(self.device_testpath,
                                              "%s.xml" % self.testsuite_name)

            if self.device.is_file_exist(remote_result_file):
                self.device.pull_file(remote_result_file, result_savepath)
            else:
                LOG.error("%s not exists", remote_result_file)
        return filepath

    def is_exist_target_in_device(self, path, target):
        command = "ls -l %s | grep %s" % (path, target)

        check_result = False
        stdout_info = self.device.execute_shell_command(command)
        if stdout_info != "" and stdout_info.find(target) != -1:
            check_result = True
        return check_result

    def obtain_coverage_data(self):
        java_cov_path = os.path.abspath(
            os.path.join(self.result_rootpath, "..", "coverage/data/exec"))
        dst_target_name = "%s.exec" % self.testsuite_name
        src_target_name = "jacoco.exec"
        if self.is_exist_target_in_device(self.device_testpath,
                                          src_target_name):
            if not os.path.exists(java_cov_path):
                os.makedirs(java_cov_path)
            self.device.pull_file(
                os.path.join(self.device_testpath, src_target_name),
                os.path.join(java_cov_path, dst_target_name))

        cxx_cov_path = os.path.abspath(
            os.path.join(self.result_rootpath, "..", "coverage/data/cxx",
                         self.testsuite_name))
        target_name = "obj"
        if self.is_exist_target_in_device(self.device_testpath, target_name):
            if not os.path.exists(cxx_cov_path):
                os.makedirs(cxx_cov_path)
            src_file = os.path.join(self.device_testpath, target_name)
            self.device.pull_file(src_file, cxx_cov_path)


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.cpp_test)
class CppTestDriver(IDriver):
    """
    CppTestDriver is a Test that runs a native test package on given device.
    """

    def __init__(self):
        self.result = ""
        self.error_message = ""
        self.config = None
        self.rerun = True
        self.rerun_all = True
        self.runner = None

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        pass

    def __execute__(self, request):
        try:
            LOG.debug("Start execute xdevice extension CppTest")

            self.config = request.config
            self.config.device = request.config.environment.devices[0]

            config_file = request.root.source.config_file
            self.result = "%s.xml" % \
                          os.path.join(request.config.report_path,
                                       "result", request.root.source.test_name)

            hilog = get_device_log_file(
                request.config.report_path,
                request.config.device.__get_serial__(),
                "device_hilog")

            hilog_open = os.open(hilog, os.O_WRONLY | os.O_CREAT | os.O_APPEND,
                                 0o755)
            self.config.device.hdc_command("shell hilog -r")
            self._run_cpp_test(config_file, listeners=request.listeners,
                               request=request)
            with os.fdopen(hilog_open, "a") as hilog_file_pipe:
                self.config.device.start_catch_device_log(hilog_file_pipe)
                time.sleep(30)
                hilog_file_pipe.flush()

        except Exception as exception:
            self.error_message = exception
            if not getattr(exception, "error_no", ""):
                setattr(exception, "error_no", "03404")
            LOG.exception(self.error_message, exc_info=False, error_no="03404")
            raise exception

        finally:
            self.config.device.stop_catch_device_log()
            self.result = check_result_report(
                request.config.report_path, self.result, self.error_message)

    def _run_cpp_test(self, config_file, listeners=None, request=None):
        try:
            if not os.path.exists(config_file):
                LOG.error("Error: Test cases don't exit %s." % config_file,
                          error_no="00102")
                raise ParamError(
                    "Error: Test cases don't exit %s." % config_file,
                    error_no="00102")

            json_config = JsonParser(config_file)
            kits = get_kit_instances(json_config, self.config.resource_path,
                                     self.config.testcases_path)

            for listener in listeners:
                listener.device_sn = self.config.device.device_sn

            self._get_driver_config(json_config)
            do_module_kit_setup(request, kits)
            self.runner = RemoteCppTestRunner(self.config)
            self.runner.suite_name = request.root.source.test_name

            if hasattr(self.config, "history_report_path") and \
                    self.config.testargs.get("test"):
                self._do_test_retry(listeners, self.config.testargs)
            else:
                gtest_para_parse(self.config.testargs, self.runner)
                self._do_test_run(listeners)

        finally:
            do_module_kit_teardown(request)

    def _do_test_retry(self, listener, testargs):
        for test in testargs.get("test"):
            test_item = test.split("#")
            if len(test_item) != 2:
                continue
            self.runner.add_instrumentation_arg(
                "gtest_filter", "%s.%s" % (test_item[0], test_item[1]))
            self.runner.run(listener)

    def _do_test_run(self, listener):
        test_to_run = self._collect_test_to_run()
        LOG.debug("collected test count is: %s" % len(test_to_run))
        if not test_to_run:
            self.runner.run(listener)
        else:
            self._run_with_rerun(listener, test_to_run)

    def _collect_test_to_run(self):
        if self.rerun:
            self.runner.add_instrumentation_arg("gtest_list_tests", True)
            run_results = self.runner.dry_run()
            self.runner.remove_instrumentation_arg("gtest_list_tests")
            return run_results
        return None

    def _run_tests(self, listener):
        test_tracker = CollectingTestListener()
        listener_copy = listener.copy()
        listener_copy.append(test_tracker)
        self.runner.run(listener_copy)
        test_run = test_tracker.get_current_run_results()
        return test_run

    def _run_with_rerun(self, listener, expected_tests):
        LOG.debug("ready to run with rerun, expect run: %s"
                  % len(expected_tests))
        test_run = self._run_tests(listener)
        LOG.debug("run with rerun, has run: %s" % len(expected_tests))
        if len(test_run) < len(expected_tests):
            expected_tests = TestDescription.remove_test(expected_tests,
                                                         test_run)
            if not expected_tests:
                LOG.debug("No tests to re-run, all tests executed at least "
                          "once.")
            if self.rerun_all:
                self._rerun_all(expected_tests, listener)
            else:
                self._rerun_serially(expected_tests, listener)

    def _rerun_all(self, expected_tests, listener):
        tests = []
        for test in expected_tests:
            tests.append("%s.%s" % (test.class_name, test.test_name))
        self.runner.add_instrumentation_arg("gtest_filter", ":".join(tests))
        LOG.debug("ready to rerun file, expect run: %s" % len(expected_tests))
        test_run = self._run_tests(listener)
        LOG.debug("rerun file, has run: %s" % len(test_run))
        if len(test_run) < len(expected_tests):
            expected_tests = TestDescription.remove_test(expected_tests,
                                                         test_run)
            if not expected_tests:
                LOG.debug("Rerun textFile success")
            self._rerun_serially(expected_tests, listener)

    def _rerun_serially(self, expected_tests, listener):
        LOG.debug("rerun serially, expected run: %s" % len(expected_tests))
        for test in expected_tests:
            self.runner.add_instrumentation_arg(
                "gtest_filter", "%s.%s" % (test.class_name, test.test_name))
            self.runner.rerun(listener, test)
            self.runner.remove_instrumentation_arg("gtest_filter")

    def _get_driver_config(self, json_config):
        target_test_path = get_config_value('native-test-device-path',
                                            json_config.get_driver(), False)
        if target_test_path:
            self.config.target_test_path = target_test_path
        else:
            self.config.target_test_path = DEFAULT_TEST_PATH

        self.config.module_name = get_config_value(
            'module-name', json_config.get_driver(), False)

        timeout_config = get_config_value('native-test-timeout',
                                          json_config.get_driver(), False)
        if timeout_config:
            self.config.timeout = int(timeout_config)
        else:
            self.config.timeout = TIME_OUT

        rerun = get_config_value('rerun', json_config.get_driver(), False)
        if isinstance(rerun, bool):
            self.rerun = rerun
        elif str(rerun).lower() == "false":
            self.rerun = False
        else:
            self.rerun = True

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


class RemoteCppTestRunner:
    def __init__(self, config):
        self.arg_list = {}
        self.suite_name = None
        self.config = config
        self.rerun_attempt = FAILED_RUN_TEST_ATTEMPTS

    def dry_run(self):
        parsers = get_plugin(Plugin.PARSER, CommonParserType.cpptest_list)
        if parsers:
            parsers = parsers[:1]
        parser_instances = []
        for parser in parsers:
            parser_instance = parser.__class__()
            parser_instances.append(parser_instance)
        handler = ShellHandler(parser_instances)
        # apply execute right
        chmod_cmd = "shell chmod 777 {}/{}".format(
            self.config.target_test_path, self.config.module_name)
        LOG.info("The apply execute command is {}".format(chmod_cmd))
        self.config.device.hdc_command(chmod_cmd, timeout=30*1000)
        # dry run command
        dry_command = "{}/{} {}".format(self.config.target_test_path,
                                         self.config.module_name,
                                         self.get_args_command())
        pre_cmd = "hdc_std -t %s shell " % self.config.device.device_sn
        command = "%s %s" % (pre_cmd, dry_command)
        LOG.info("The dry_command execute command is {}".format(command))
        output = start_standing_subprocess(command.split(),
                                           return_result=True)
        LOG.debug("dryrun output ---")
        output = output.replace("\r", "")
        handler.__read__(output)
        handler.__done__()
        return parser_instances[0].tests

    def run(self, listener):
        handler = self._get_shell_handler(listener)
        pre_cmd = "hdc_std -t %s shell " % self.config.device.device_sn
        command = "{}/{} {}".format(self.config.target_test_path,
                                         self.config.module_name,
                                         self.get_args_command())
        command = "%s %s" % (pre_cmd, command)
        LOG.debug("run command is: %s" % command)
        output = start_standing_subprocess(command.split(), return_result=True)
        LOG.debug("run output ---")
        output = output.replace("\r", "")
        handler.__read__(output)
        handler.__done__()

    def rerun(self, listener, test):
        if self.rerun_attempt:
            test_tracker = CollectingTestListener()
            listener_copy = listener.copy()
            listener_copy.append(test_tracker)
            handler = self._get_shell_handler(listener_copy)
            try:
                pre_cmd = "hdc_std -t %s shell " % self.config.device.device_sn
                command = "{}/{} {}".format(self.config.target_test_path,
                                             self.config.module_name,
                                             self.get_args_command())
                command = "%s %s" % (pre_cmd, command)
                LOG.debug("rerun command is: %s" % command)
                output = start_standing_subprocess(command.split(),
                                                   return_result=True)
                LOG.debug("run output ---")
                output = output.replace("\r", "")
                handler.__read__(output)
                handler.__done__()

            except ShellCommandUnresponsiveException:
                LOG.debug("Exception: ShellCommandUnresponsiveException")
            finally:
                if not len(test_tracker.get_current_run_results()):
                    LOG.debug("No test case is obtained finally")
                    self.rerun_attempt -= 1
                    handler.parsers[0].mark_test_as_blocked(test)
        else:
            LOG.debug("not execute and mark as blocked finally")
            handler = self._get_shell_handler(listener)
            handler.parsers[0].mark_test_as_blocked(test)

    def add_instrumentation_arg(self, name, value):
        if not name or not value:
            return
        self.arg_list[name] = value

    def remove_instrumentation_arg(self, name):
        if not name:
            return
        if name in self.arg_list:
            del self.arg_list[name]

    def get_args_command(self):
        args_commands = ""
        for key, value in self.arg_list.items():
            if key == "gtest_list_tests":
                args_commands = "%s --%s" % (args_commands, key)
            else:
                args_commands = "%s --%s=%s" % (args_commands, key, value)
        return args_commands

    def _get_shell_handler(self, listener):
        parsers = get_plugin(Plugin.PARSER, CommonParserType.cpptest)
        if parsers:
            parsers = parsers[:1]
        parser_instances = []
        for parser in parsers:
            parser_instance = parser.__class__()
            parser_instance.suite_name = self.suite_name
            parser_instance.listeners = listener
            parser_instances.append(parser_instance)
        handler = ShellHandler(parser_instances)
        return handler


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.junit_test)
class JUnitTestDriver(IDriver):
    """
    JUnitTestDriver is a Test that runs a native test package on given device.
    """

    def __init__(self):
        self.result = ""
        self.error_message = ""
        self.kits = []
        self.config = None
        self.rerun = True
        self.runner = None
        self.rerun_using_test_file = True
        self.temp_file_list = []
        self.is_no_test = False

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        if hasattr(config, "devices") and len(config.devices) > 1:
            for device in config.devices:
                device_name = device.get("name")
                if not device_name:
                    self.error_message = "JUnitTest Load Error(03100)"
                    raise ParamError("device name not set in config file",
                                     error_no="03100")

    def __execute__(self, request):
        try:
            LOG.debug("Start execute xdevice extension JUnit Test")

            self.config = request.config
            self.config.device = request.get_devices()[0]
            self.config.devices = request.get_devices()

            config_file = request.get_config_file()
            LOG.info("config file: %s", config_file)
            self.result = os.path.join(request.get("report_path"), "result",
                                       ".".join((request.get_test_name(),
                                                 "xml")))
            self.__check_config__(self.config)

            device_log_pipes = []
            try:
                for device in self.config.devices:
                    device_name = device.get("name", "")
                    hilog = get_device_log_file(
                        request.config.report_path, device.__get_serial__(),
                        "device_hilog", device_name)
                    hilog_open = os.open(hilog, os.O_WRONLY |
                                         os.O_CREAT | os.O_APPEND, 0o755)
                    hilog_pipe = os.fdopen(hilog_open, "a")
                    device.start_catch_device_log(hilog_pipe)
                    device_log_pipes.extend([hilog_pipe])

                self._run_junit(config_file, listeners=request.listeners,
                                request=request)
            finally:
                for device_log_pipe in device_log_pipes:
                    device_log_pipe.flush()
                    device_log_pipe.close()
                for device in self.config.devices:
                    device.stop_catch_device_log()

        except Exception as exception:
            if not getattr(exception, "error_no", ""):
                setattr(exception, "error_no", "03405")
            LOG.exception(self.error_message, exc_info=False, error_no="03405")
            raise exception

        finally:
            if not self._is_ignore_report():
                self.result = check_result_report(
                    request.config.report_path, self.result,
                    self.error_message)
            else:
                LOG.debug("hide result and not generate report")

    def _run_junit(self, config_file, listeners, request):
        try:
            if not os.path.exists(config_file):
                error_msg = "Error: Test cases %s don't exist." % config_file
                LOG.error(error_msg, error_no="00102")
                raise ParamError(error_msg, error_no="00102")

            for device in self.config.devices:
                cmd = "target mount" \
                    if device.usb_type == DeviceConnectorType.hdc \
                    else "remount"
                device.hdc_command(cmd)
            json_config = JsonParser(config_file)
            self.kits = get_kit_instances(json_config,
                                          self.config.resource_path,
                                          self.config.testcases_path)

            for listener in listeners:
                listener.device_sn = self.config.device.device_sn

            self._get_driver_config(json_config)
            do_module_kit_setup(request, self.kits)
            self.runner = RemoteTestRunner(self.config)
            self.runner.suite_name = request.get_test_name()
            self.runner.suite_file = "%s.hap" % \
                                     get_filename_extension(config_file)[0]

            self._get_runner_config(json_config)
            if hasattr(self.config, "history_report_path") and \
                    self.config.testargs.get("test"):
                self._do_test_retry(listeners, self.config.testargs)
            else:
                self._do_include_tests()
                self.runner.junit_para = junit_para_parse(
                    self.config.device, self.config.testargs, "-s")
                self._do_test_run(listeners)

        finally:
            do_module_kit_teardown(request)
            if self.runner and self.runner.junit_para and (
                    self.runner.junit_para.find("testFile") != -1
                    or self.runner.junit_para.find("notTestFile") != -1):
                self._junit_clear()

    def _junit_clear(self):
        self.config.device.execute_shell_command(
            "rm -r /%s/%s/%s/%s" % ("data", "local", "tmp", "ajur"))
        for temp_file in self.temp_file_list:
            if os.path.exists(temp_file):
                os.remove(temp_file)
        self.temp_file_list.clear()

    def _get_driver_config(self, json_config):
        package = get_config_value('package', json_config.get_driver(), False)
        runner = "ohos.testkit.runner.Runner"
        include_tests = get_config_value("include-tests",
                                         json_config.get_driver(), True, [])
        if not package:
            raise ParamError("Can't find package in config file.")
        self.config.package = package
        self.config.runner = runner
        self.config.include_tests = include_tests

        self.config.xml_output = get_xml_output(self.config, json_config)

        timeout_config = get_config_value('shell-timeout',
                                          json_config.get_driver(), False)
        if timeout_config:
            self.config.timeout = int(timeout_config)
        else:
            self.config.timeout = TIME_OUT

        nohup = get_config_value('nohup', json_config.get_driver(), False)
        if nohup and (nohup == "true" or nohup == "True"):
            self.config.nohup = True
        else:
            self.config.nohup = False

    def _get_runner_config(self, json_config):
        test_timeout = get_config_value('test-timeout',
                                        json_config.get_driver(), False)
        if test_timeout:
            self.runner.add_instrumentation_arg("timeout_sec",
                                                int(test_timeout))

    def _do_test_retry(self, listener, testargs):
        for test in testargs.get("test"):
            test_item = test.split("#")
            if len(test_item) != 2:
                continue
            self.runner.class_name = test_item[0]
            self.runner.test_name = test_item[1]
            self.runner.run(listener)

    def _do_test_run(self, listener):
        if not self._check_package():
            LOG.error("%s is not supported test" % self.config.package)
            raise HapNotSupportTest("%s is not supported test" %
                                    self.config.package)
        test_to_run = self._collect_test_to_run()
        LOG.debug("collected test count is: %s" % len(test_to_run))
        if not test_to_run:
            self.is_no_test = True
            self.runner.run(listener)
        else:
            self._run_with_rerun(listener, test_to_run)

    def _check_package(self):
        command = '''systemdumper -s 401 -a "-bundle %s"''' % \
                  self.config.package
        output = self.config.device.execute_shell_command(command)
        LOG.debug("systemdumper output: %s" % output)
        if output and "ohos.testkit.runner.EntryAbility" in output:
            return True
        else:
            LOG.info("try hidumper command to check package")
            command = '''hidumper -s 401 -a "-bundle %s"''' % \
                      self.config.package
            output = self.config.device.execute_shell_command(command)
            LOG.debug("hidumper output: %s" % output)
            if output and "ohos.testkit.runner.EntryAbility" in output:
                return True
        return False

    def _run_tests(self, listener):
        test_tracker = CollectingTestListener()
        listener_copy = listener.copy()
        listener_copy.append(test_tracker)
        self.runner.run(listener_copy)
        test_run = test_tracker.get_current_run_results()
        return test_run

    def _run_with_rerun(self, listener, expected_tests):
        LOG.debug("ready to run with rerun, expect run: %s"
                  % len(expected_tests))
        test_run = self._run_tests(listener)
        LOG.debug("run with rerun, has run: %s" % len(expected_tests))
        if len(test_run) < len(expected_tests):
            expected_tests = TestDescription.remove_test(
                expected_tests, test_run)
            if not expected_tests:
                LOG.debug("No tests to re-run, all tests executed at least "
                          "once.")
            if self.rerun_using_test_file:
                self._rerun_file(expected_tests, listener)
            else:
                self._rerun_serially(expected_tests, listener)

    def _make_test_file(self, expected_tests, test_file_path):
        file_name = 'xdevice_testFile_%s.txt' % self.runner.suite_name
        file_path = os.path.join(test_file_path, file_name)
        try:
            file_path_open = os.open(file_path, os.O_WRONLY | os.O_CREAT |
                                     os.O_APPEND, 0o755)
            with os.fdopen(file_path_open, "a") as file_desc:
                for test in expected_tests:
                    file_desc.write("%s#%s" % (test.class_name,
                                               test.test_name))
                    file_desc.write("\n")
                    file_desc.flush()
        except(IOError, ValueError) as err_msg:
            LOG.exception("Error for make long command file: ", err_msg,
                          exc_info=False, error_no="03200")
        return file_name, file_path

    def _rerun_file(self, expected_tests, listener):
        test_file_path = tempfile.mkdtemp(prefix="test_file_",
                                          dir=self.config.report_path)
        file_name, file_path = self._make_test_file(
            expected_tests, test_file_path)
        self.config.device.push_file(file_path, ON_DEVICE_TEST_DIR_LOCATION)
        file_path_on_device = ''.join((ON_DEVICE_TEST_DIR_LOCATION, file_name))
        self.runner.add_instrumentation_arg("testFile", file_path_on_device)
        self.runner.junit_para = reset_junit_para(self.runner.junit_para, "-s")
        LOG.debug("ready to rerun file, expect run: %s" % len(expected_tests))
        test_run = self._run_tests(listener)
        LOG.debug("rerun file, has run: %s" % len(test_run))
        self.config.device.execute_shell_command("rm %s" % file_path_on_device)
        if len(test_run) < len(expected_tests):
            expected_tests = TestDescription.remove_test(expected_tests,
                                                         test_run)
            if not expected_tests:
                LOG.debug("Rerun textFile success")
            self._rerun_serially(expected_tests, listener)
        shutil.rmtree(test_file_path)

    def _rerun_serially(self, expected_tests, listener):
        LOG.debug("rerun serially, expected run: %s" % len(expected_tests))
        self.runner.remove_instrumentation_arg("testFile")
        for test in expected_tests:
            self.runner.class_name = test.class_name
            self.runner.test_name = test.test_name
            self.runner.rerun(listener, test)

    def _collect_test_to_run(self):
        if self.rerun and self.config.xml_output == "false" and \
                not self.config.nohup:
            self.runner.set_test_collection(True)
            tests = self._collect_test_and_retry()
            self.runner.set_test_collection(False)
            return tests
        return None

    def _collect_test_and_retry(self):
        collector = CollectingTestListener()
        listener = [collector]
        self.runner.run(listener)
        run_results = collector.get_current_run_results()
        return run_results

    def _do_include_tests(self):
        """
        handler the include-tests parameters in json file of current module.
        the main approach is to inject new dict into "testargs".
        """
        if not self.config.include_tests:
            return
        keys_list = [key.strip() for key in self.config.testargs.keys()]
        if "test-file-include-filter" in keys_list:
            return
        test_filter = self._slice_include_tests()
        if not test_filter:
            LOG.error("invalid include-tests! please check json file.")
            return
        if "test" in keys_list or "class" in keys_list:
            test_list = list()
            if "test" in keys_list:
                for element in self.config.testargs.get("test", []):
                    if self._filter_valid_test(element, test_filter):
                        test_list.append(element.strip())
                self.config.testargs.pop("test")
            if "class" in keys_list:
                for element in self.config.testargs.get("class", []):
                    if self._filter_valid_test(element, test_filter):
                        test_list.append(element.strip())
                self.config.testargs.pop("class")
        else:
            test_list = [ele.strip() for ele in self.config.include_tests]
        if test_list:
            import datetime
            prefix = datetime.datetime.now().strftime('%Y%m%d%H%M%S%f')

            save_file = \
                os.path.join(self.config.report_path, "temp_%s.txt" % prefix)
            save_file_open = os.open(save_file, os.O_WRONLY
                                     | os.O_CREAT, 0o755)
            with os.fdopen(save_file_open, "w") as save_handler:
                for test in test_list:
                    save_handler.write("{}\n".format(test.strip()))
                save_handler.flush()
            self.temp_file_list.append(save_file)
            include_tests_key = "test-file-include-filter"
            self.config.testargs.update(
                {include_tests_key: self.temp_file_list})
            LOG.debug("handle include-tests, write to %s, data length is %s" %
                      (self.temp_file_list[0], len(test_list)))
        else:
            msg = "there is any valid test after filter by 'include-tests'"
            LOG.error(msg)
            raise ParamError(msg)

    def _slice_include_tests(self):
        test_filter = dict()
        for include_test in self.config.include_tests:
            include_test = include_test.strip()
            if include_test:
                # element like 'class#method'
                if "#" in include_test:
                    test_list = test_filter.get("test_in", [])
                    test_list.append(include_test)
                    test_filter.update({"test_in": test_list})
                # element like 'class'
                else:
                    class_list = test_filter.get("class_in", [])
                    class_list.append(include_test)
                    test_filter.update({"class_in": class_list})
            else:
                LOG.warning("there is empty element in include-tests")
        if len([ele for test in test_filter.values() for ele in test]) > 0:
            return test_filter

    @classmethod
    def _filter_valid_test(cls, element, test_filter):
        element = element.strip()
        # if element in the list which element like 'class#method'
        if element in test_filter.get("test_in", []):
            return element
        # if element is the list which element like 'class'
        element_items = element.split("#")
        if element_items[0].strip() in test_filter.get("class_in", []):
            return element
        raise ParamError("{} not match 'include-tests'!".format(element))

    def _is_ignore_report(self):
        if self.config.task and not self.config.testlist and \
                not self.config.testfile:
            if self.is_no_test and self.config.testargs.get("level", None):
                return True

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


class RemoteTestRunner:
    def __init__(self, config):
        self.arg_list = {}
        self.suite_name = None
        self.suite_file = None
        self.class_name = None
        self.test_name = None
        self.junit_para = None
        self.config = config
        self.rerun_attempt = FAILED_RUN_TEST_ATTEMPTS

    def __check_environment__(self, device_options):
        pass

    def run(self, listener):
        handler = self._get_shell_handler(listener)
        # execute test case
        command = "aa start -p %s " \
                  "-n ohos.testkit.runner.EntryAbility" \
                  " -s unittest %s -s rawLog true %s %s" \
                  % (self.config.package, self.config.runner, self.junit_para,
                     self.get_args_command())

        try:
            if self.config.nohup:
                nohup_command = "nohup %s &" % command
                result_value = self.config.device.execute_shell_cmd_background(
                    nohup_command, timeout=self.config.timeout)
            elif self.config.xml_output == "true":
                result_value = self.config.device.execute_shell_command(
                    command, timeout=self.config.timeout,
                    retry=0)
            else:
                self.config.device.execute_shell_command(
                    command, timeout=self.config.timeout, receiver=handler,
                    retry=0)
                return
        except ShellCommandUnresponsiveException:
            LOG.debug("Exception: ShellCommandUnresponsiveException")
        else:
            self.config.target_test_path = "/%s/%s/%s/%s/%s/" % \
                                           ("data", "user", "0",
                                            self.config.package, "cache")
            result = ResultManager(self.suite_file, self.config.report_path,
                                   self.config.device,
                                   self.config.target_test_path)
            result.get_test_results(result_value)

    def rerun(self, listener, test):
        if self.rerun_attempt:
            listener_copy = listener.copy()
            test_tracker = CollectingTestListener()
            listener_copy.append(test_tracker)
            handler = self._get_shell_handler(listener_copy)
            try:
                command = "aa start -p %s " \
                          "-n ohos.testkit.runner.EntryAbility" \
                          " -s unittest %s -s rawLog true %s" \
                          % (self.config.package, self.config.runner,
                             self.get_args_command())

                self.config.device.execute_shell_command(
                    command, timeout=self.config.timeout, receiver=handler,
                    retry=0)

            except Exception as error:
                LOG.error("rerun error %s, %s" % (error, error.__class__))
            finally:
                if not len(test_tracker.get_current_run_results()):
                    LOG.debug("No test case is obtained finally")
                    self.rerun_attempt -= 1
                    handler.parsers[0].mark_test_as_blocked(test)
        else:
            LOG.debug("not execute and mark as blocked finally")
            handler = self._get_shell_handler(listener)
            handler.parsers[0].mark_test_as_blocked(test)

    def set_test_collection(self, collect):
        if collect:
            self.add_instrumentation_arg("log", "true")
        else:
            self.remove_instrumentation_arg("log")

    def add_instrumentation_arg(self, name, value):
        if not name or not value:
            return
        self.arg_list[name] = value

    def remove_instrumentation_arg(self, name):
        if not name:
            return
        if name in self.arg_list:
            del self.arg_list[name]

    def get_args_command(self):
        args_commands = ""
        for key, value in self.arg_list.items():
            args_commands = "%s -s %s %s" % (args_commands, key, value)
        if self.class_name and self.test_name:
            args_commands = "%s -s class %s#%s" % (args_commands,
                                                   self.class_name,
                                                   self.test_name)
        elif self.class_name:
            args_commands = "%s -s class %s" % (args_commands, self.class_name)
        return args_commands

    def _get_shell_handler(self, listener):
        parsers = get_plugin(Plugin.PARSER, CommonParserType.junit)
        if parsers:
            parsers = parsers[:1]
        parser_instances = []
        for parser in parsers:
            parser_instance = parser.__class__()
            parser_instance.suite_name = self.suite_name
            parser_instance.listeners = listener
            parser_instances.append(parser_instance)
        handler = ShellHandler(parser_instances)
        return handler


class RemoteDexRunner:
    def __init__(self, config):
        self.arg_list = {}
        self.suite_name = None
        self.junit_para = ""
        self.config = config
        self.class_name = None
        self.test_name = None
        self.rerun_attempt = FAILED_RUN_TEST_ATTEMPTS

    def run(self, listener):
        handler = self._get_shell_handler(listener)
        command = "export BOOTCLASSPATH=$BOOTCLASSPATH:" \
                  "{remote_path}/{module_name};cd {remote_path}; " \
                  "app_process -cp {remote_path}/{module_name} / " \
                  "ohos.testkit.runner.JUnitRunner {junit_para}{arg_list}" \
                  " --rawLog true --coverage false " \
                  "--classpathToScan {remote_path}/{module_name}".format(
                   remote_path=self.config.remote_path,
                   module_name=self.config.module_name,
                   junit_para=self.junit_para,
                   arg_list=self.get_args_command())

        try:
            self.config.device.execute_shell_command(
                command, timeout=self.config.timeout,
                receiver=handler, retry=0)
        except ConnectionResetError:
            if len(listener) == 1 and isinstance(listener[0],
                                                 CollectingTestListener):
                LOG.info("try subprocess ")
                listener[0].tests.clear()
                command = ["shell", command]
                result = self.config.device.hdc_command(
                    command, timeout=self.config.timeout, retry=0,
                    join_result=True)
                handler.__read__(result)
                handler.__done__()
                LOG.info("get current testcase: %s " %
                         len(listener[0].get_current_run_results()))

    def rerun(self, listener, test):
        if self.rerun_attempt:
            listener_copy = listener.copy()
            test_tracker = CollectingTestListener()
            listener_copy.append(test_tracker)
            handler = self._get_shell_handler(listener_copy)
            try:
                command = "export BOOTCLASSPATH=$BOOTCLASSPATH:" \
                          "{remote_path}/{module_name};cd {remote_path}; " \
                          "app_process -cp {remote_path}/{module_name} / " \
                          "ohos.testkit.runner.JUnitRunner {arg_list} " \
                          "--rawLog true --coverage false " \
                          "--classpathToScan " \
                          "{remote_path}/{module_name}".format(
                           remote_path=self.config.remote_path,
                           module_name=self.config.module_name,
                           arg_list=self.get_args_command())
                self.config.device.execute_shell_command(
                    command, timeout=self.config.timeout,
                    receiver=handler, retry=0)

            except ShellCommandUnresponsiveException as _:
                LOG.debug("Exception: ShellCommandUnresponsiveException")
            finally:
                if not len(test_tracker.get_current_run_results()):
                    LOG.debug("No test case is obtained finally")
                    self.rerun_attempt -= 1
                    handler.parsers[0].mark_test_as_blocked(test)
        else:
            LOG.debug("not execute and mark as blocked finally")
            handler = self._get_shell_handler(listener)
            handler.parsers[0].mark_test_as_blocked(test)

    def set_test_collection(self, collect):
        if collect:
            self.add_instrumentation_arg("log", "true")
        else:
            self.remove_instrumentation_arg("log")

    def add_instrumentation_arg(self, name, value):
        if not name or not value:
            return
        self.arg_list[name] = value

    def remove_instrumentation_arg(self, name):
        if not name:
            return
        if name in self.arg_list:
            del self.arg_list[name]

    def get_args_command(self):
        args_commands = ""
        for key, value in self.arg_list.items():
            args_commands = "%s --%s %s" % (args_commands, key, value)
        if self.class_name and self.test_name:
            args_commands = "%s --class %s#%s" % (args_commands,
                                                  self.class_name,
                                                  self.test_name)
        elif self.class_name:
            args_commands = "%s --class %s" % (args_commands, self.class_name)
        return args_commands

    def _get_shell_handler(self, listener):
        parsers = get_plugin(Plugin.PARSER, CommonParserType.junit)
        if parsers:
            parsers = parsers[:1]
        parser_instances = []
        for parser in parsers:
            parser_instance = parser.__class__()
            parser_instance.suite_name = self.suite_name
            parser_instance.listeners = listener
            parser_instances.append(parser_instance)
        handler = ShellHandler(parser_instances)
        return handler


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.hap_test)
class HapTestDriver(IDriver):
    """
    HapTestDriver is a Test that runs a native test package on given device.
    """
    # test driver config
    config = None
    instrument_hap_file_suffix = '_ad.hap'
    result = ""

    def __init__(self):
        self.ability_name = ""
        self.package_name = ""
        self.activity_name = ""

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        pass

    def __execute__(self, request):
        try:
            LOG.debug("Start execute xdevice extension HapTest")

            self.config = request.config
            self.config.target_test_path = DEFAULT_TEST_PATH
            self.config.device = request.config.environment.devices[0]

            suite_file = request.root.source.source_file
            if not suite_file:
                LOG.error("test source '%s' not exists" %
                          request.root.source.source_string, error_no="00110")
                return

            LOG.debug("Testsuite FilePath: %s" % suite_file)
            package_name, ability_name = self._get_package_and_ability_name(
                suite_file)
            self.package_name = package_name
            self.ability_name = ability_name
            self.activity_name = "%s.MainAbilityShellActivity" % \
                                 self.package_name
            self.config.test_hap_out_path = \
                "/data/data/%s/files/test/result/" % self.package_name
            self.config.test_suite_timeout = 300 * 1000

            serial = request.config.device.__get_serial__()
            device_log_file = get_device_log_file(request.config.report_path,
                                                  serial)
            device_log_file_open = os.open(
                device_log_file, os.O_WRONLY | os.O_CREAT | os.O_APPEND, 0o755)
            with os.fdopen(device_log_file_open, "a")as file_pipe:
                self.config.device.start_catch_device_log(file_pipe)
                self._init_junit_test()
                self._run_junit_test(suite_file)
                file_pipe.flush()
        finally:
            self.config.device.stop_catch_device_log()

    def _init_junit_test(self):
        cmd = "target mount" \
            if self.config.device.usb_type == DeviceConnectorType.hdc \
            else "remount"
        self.config.device.hdc_command(cmd)
        self.config.device.execute_shell_command(
            "rm -rf %s" % self.config.target_test_path)
        self.config.device.execute_shell_command(
            "mkdir -p %s" % self.config.target_test_path)
        self.config.device.execute_shell_command(
            "mount -o rw,remount,rw /%s" % "system")

    def _run_junit_test(self, suite_file):
        filename = os.path.basename(suite_file)
        suitefile_target_test_path = self.config.test_hap_out_path
        junit_test_para = self._get_junit_test_para(filename, suite_file)
        is_coverage_test = True if self.config.coverage == "coverage" else \
            False

        # push testsuite file
        self.config.device.push_file(suite_file, self.config.target_test_path)

        resource_manager = ResourceManager()
        resource_data_dic, resource_dir = \
            resource_manager.get_resource_data_dic(suite_file)
        resource_manager.process_preparer_data(resource_data_dic, resource_dir,
                                               self.config.device)

        # execute testcase
        install_result = self._install_hap(filename)
        result = ResultManager(suite_file, self.config.report_path,
                               self.config.device,
                               self.config.test_hap_out_path)
        result.set_is_coverage(is_coverage_test)
        if install_result:
            return_message = self._execute_suitefile_junittest(
                filename, junit_test_para, suitefile_target_test_path)

            self.result = result.get_test_results(return_message)
            self._uninstall_hap(self.package_name)
        else:
            self.result = result.get_test_results("Error: install hap failed.")
            LOG.error("Error: install hap failed.", error_no="03204")

        resource_manager.process_cleaner_data(resource_data_dic, resource_dir,
                                              self.config.device)

    def _get_junit_test_para(self, filename, suite_file):
        if not filename.endswith(self.instrument_hap_file_suffix):
            exec_class, exec_method, exec_level = get_java_test_para(
                self.config.testcase, self.config.testlevel)
            java_test_file = get_execute_java_test_files(suite_file)
            junit_test_para = self._get_hap_test_para(java_test_file,
                                                      exec_class, exec_method,
                                                      exec_level)
        else:
            junit_test_para = get_execute_java_test_files(suite_file)
        return junit_test_para

    @staticmethod
    def _get_hap_test_para(java_test_file, exec_class, exec_method,
                           exec_level):
        hap_test_para = "%s%s#%s%s#%s%s#%s%s" % (
            ZunitConst.test_class, java_test_file,
            ZunitConst.exec_class, exec_class,
            ZunitConst.exec_method, exec_method,
            ZunitConst.exec_level, exec_level)
        return hap_test_para

    def _execute_suitefile_junittest(self, filename, testpara,
                                     target_test_path):
        return_message = self._execute_hapfile_junittest(filename, testpara,
                                                         target_test_path)
        return return_message

    def _execute_hapfile_junittest(self, filename, testpara, target_test_path):
        _unlock_screen(self.config.device)
        _unlock_device(self.config.device)

        try:
            if not filename.endswith(self.instrument_hap_file_suffix):
                return_message = self.start_hap_activity(testpara, filename)
                LOG.info("HAP Testcase is executing, please wait a moment...")
                if "Error" not in return_message:
                    self._check_hap_finished(target_test_path)
            else:
                return_message = self.start_instrument_hap_activity(testpara)
        except (ExecuteTerminate, HdcCommandRejectedException,
                ShellCommandUnresponsiveException, HdcError) as exception:
            return_message = str(exception.args)
            if not getattr(exception, "error_no", ""):
                setattr(exception, "error_no", "03203")

        _lock_screen(self.config.device)
        return return_message

    def _init_hap_device(self):
        self.config.device.execute_shell_command(
            "rm -rf %s" % self.config.test_hap_out_path)
        self.config.device.execute_shell_command(
            "mkdir -p %s" % self.config.test_hap_out_path)

    def _install_hap(self, filename):
        message = self.config.device.execute_shell_command(
            "bm install -p %s" % os.path.join(self.config.target_test_path,
                                              filename))
        message = str(message).rstrip()
        if message == "" or "Success" in message:
            return_code = True
            if message != "":
                LOG.info(message)
        else:
            return_code = False
            if message != "":
                LOG.warning(message)

        _sleep_according_to_result(return_code)
        return return_code

    def start_hap_activity(self, testpara, filename):
        execute_para = testpara
        if self.config.coverage == "coverage":
            execute_para = ''.join((execute_para, ' ',
                                    ZunitConst.jacoco_exec_file, filename,
                                    ".exec"))
        try:
            display_receiver = DisplayOutputReceiver()
            self.config.device.execute_shell_command(
                "am start -S -n %s/%s --es param '%s'" %
                (self.package_name, self.activity_name,
                 execute_para), receiver=display_receiver,
                timeout=self.config.test_suite_timeout)
            _sleep_according_to_result(display_receiver.output)
            return_message = display_receiver.output

        except (ExecuteTerminate, HdcCommandRejectedException,
                ShellCommandUnresponsiveException, HdcError) as exception:
            return_message = exception.args
            if not getattr(exception, "error_no", ""):
                setattr(exception, "error_no", "03203")
        return return_message

    def start_instrument_hap_activity(self, testpara):
        from xdevice import Variables
        try:
            display_receiver = DisplayOutputReceiver()
            if self.config.coverage != "coverage":
                self.config.device.execute_shell_command(
                    "aa start -p %s -n %s -s AbilityTestCase %s -w %s" %
                    (self.package_name, self.ability_name, testpara,
                     str(self.config.test_suite_timeout)),
                    receiver=display_receiver,
                    timeout=self.config.test_suite_timeout)
            else:
                build_variant_outpath = os.path.join(
                    Variables.source_code_rootpath, "out",
                    self.config.build_variant)
                strip_num = len(build_variant_outpath.split(os.sep)) - 1
                self.config.device.execute_shell_command(
                    "cd %s; export GCOV_PREFIX=%s; "
                    "export GCOV_PREFIX_STRIP=%d; "
                    "aa start -p %s -n %s -s AbilityTestCase %s -w %s" %
                    (self.config.target_test_path,
                     self.config.target_test_path,
                     strip_num, self.package_name, self.ability_name, testpara,
                     str(self.config.test_suite_timeout)),
                    receiver=display_receiver,
                    timeout=self.config.test_suite_timeout)
            _sleep_according_to_result(display_receiver.output)
            return_message = display_receiver.output
        except (ExecuteTerminate, HdcCommandRejectedException,
                ShellCommandUnresponsiveException, HdcError) as exception:
            return_message = exception.args
            if not getattr(exception, "error_no", ""):
                setattr(exception, "error_no", "03203")
        return return_message

    def _check_hap_finished(self, target_test_path):
        run_timeout = True
        sleep_duration = 3
        target_file = os.path.join(target_test_path,
                                   ZunitConst.jtest_status_filename)
        for _ in range(
                int(self.config.test_suite_timeout / (1000 * sleep_duration))):
            check_value = self.config.device.is_file_exist(target_file)
            LOG.info("%s state: %s", self.config.device.device_sn,
                     self.config.device.test_device_state.value)
            if not check_value:
                time.sleep(sleep_duration)
                continue
            run_timeout = False
            break
        if run_timeout:
            return_code = False
            LOG.error("HAP Testcase executed timeout or exception, please "
                      "check detail information from system log",
                      error_no="03205")
        else:
            return_code = True
            LOG.info("HAP Testcase executed finished")
        return return_code

    def _uninstall_hap(self, package_name):
        return_message = self.config.device.execute_shell_command(
            "pm uninstall %s" % package_name)
        _sleep_according_to_result(return_message)
        return return_message

    @staticmethod
    def _get_package_and_ability_name(hap_filepath):
        package_name = ""
        ability_name = ""

        if os.path.exists(hap_filepath):
            filename = os.path.basename(hap_filepath)

            # unzip the hap file
            hap_bak_path = os.path.abspath(
                os.path.join(os.path.dirname(hap_filepath),
                             "%s.bak" % filename))
            try:
                with zipfile.ZipFile(hap_filepath) as zf_desc:
                    zf_desc.extractall(path=hap_bak_path)
            except RuntimeError as error:
                LOG.error(error, error_no="03206")

            # verify config.json file
            app_profile_path = os.path.join(hap_bak_path,
                                            "config.json")
            if not os.path.exists(app_profile_path):
                LOG.info("file %s not exists" % app_profile_path)
                return package_name, ability_name

            if os.path.isdir(app_profile_path):
                LOG.info("%s is a folder, and not a file" % app_profile_path)
                return package_name, ability_name

            # get package_name and ability_name value.
            app_profile_path_open = os.open(app_profile_path, os.O_RDONLY,
                                            stat.S_IWUSR | stat.S_IRUSR)
            with os.fdopen(app_profile_path_open, 'r') as load_f:
                load_dict = json.load(load_f)
            profile_list = load_dict.values()
            for profile in profile_list:
                package_name = profile.get("package")
                if not package_name:
                    continue
                abilities = profile.get("abilities")
                for abilitie in abilities:
                    abilities_name = abilitie.get("name")
                    if abilities_name.startswith("."):
                        ability_name = ''.join(
                            (package_name,
                             abilities_name[abilities_name.find("."):]))
                    else:
                        ability_name = abilities_name
                    break
                break

            # delete hap_bak_path
            if os.path.exists(hap_bak_path):
                shutil.rmtree(hap_bak_path)
        else:
            LOG.info("file %s not exists" % hap_filepath)

        return package_name, ability_name

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.jsunit_test)
class JSUnitTestDriver(IDriver):
    """
    JSUnitTestDriver is a Test that runs a native test package on given device.
    """

    def __init__(self):
        self.timeout = 80 * 1000
        self.start_time = None
        self.result = ""
        self.error_message = ""
        self.kits = []
        self.config = None

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        pass

    def __execute__(self, request):
        try:
            LOG.debug("Start execute xdevice extension JSUnit Test")
            self.result = os.path.join(
                request.config.report_path, "result",
                '.'.join((request.get_module_name(), "xml")))
            self.config = request.config
            self.config.device = request.config.environment.devices[0]

            config_file = request.root.source.config_file
            suite_file = request.root.source.source_file

            if not suite_file:
                raise ParamError(
                    "test source '%s' not exists" %
                    request.root.source.source_string, error_no="00110")

            LOG.debug("Test case file path: %s" % suite_file)
            # avoid hilog service stuck issue
            self.config.device.hdc_command("shell stop_service hilogd",
                                           timeout=30 * 1000)
            self.config.device.hdc_command("shell start_service hilogd",
                                           timeout=30 * 1000)
            time.sleep(10)

            self.config.device.hdc_command("shell hilog -r", timeout=30 * 1000)
            self._run_jsunit(config_file, request)
        except Exception as exception:
            self.error_message = exception
            if not getattr(exception, "error_no", ""):
                setattr(exception, "error_no", "03409")
            LOG.exception(self.error_message, exc_info=True, error_no="03409")
            raise exception
        finally:
            self.config.device.stop_catch_device_log()
            self.result = check_result_report(
                request.config.report_path, self.result, self.error_message)

    def generate_console_output(self, device_log_file, request, timeout):
        LOG.info("prepare to read device log, may wait some time")
        message_list = list()
        _, end_count, is_suites_end = self.read_device_log_timeout(
            device_log_file, message_list, timeout)
        if end_count == 0:
            message_list.append("app Log: [suite end]\n")
            LOG.error("there is no suite end")
        if not is_suites_end:
            message_list.append("app Log: [end] run suites end\n")
            LOG.error("there is no suites end")
        result_message = "".join(message_list)
        message_list.clear()

        report_name = request.get_module_name()
        parsers = get_plugin(
            Plugin.PARSER, CommonParserType.jsunit)
        if parsers:
            parsers = parsers[:1]
        for listener in request.listeners:
            listener.device_sn = self.config.device.device_sn
        parser_instances = []

        for parser in parsers:
            parser_instance = parser.__class__()
            parser_instance.suites_name = report_name
            parser_instance.suite_name = report_name
            parser_instance.listeners = request.listeners
            parser_instances.append(parser_instance)
        handler = ShellHandler(parser_instances)
        process_command_ret(result_message, handler)

    def read_device_log(self, device_log_file, timeout=60):
        LOG.info("The timeout is {} seconds".format(timeout))

        while time.time() - self.start_time <= timeout:
            result_message = ""
            with open(device_log_file, "r", encoding='utf-8',
                     errors='ignore') as file_read_pipe:
                while True:
                    try:
                        line = file_read_pipe.readline()
                    except (UnicodeDecodeError, UnicodeError) as error:
                        LOG.warning("While read log file: %s" % error)
                    if not line :
                        break
                    if line.lower().find("jsapp:") != -1:
                        result_message += line
                    if "[end] run suites end" in line:
                        LOG.info("Find the end mark then analysis result")
                        return result_message
                # if test not finished, wait 5 seconds
                else:
                    LOG.info("did not find the end mark then wait 5 seconds")
                    time.sleep(5)
        else:
            LOG.error("Hjsunit run timeout {}s reached".format(timeout))
            raise RuntimeError("Hjsunit run timeout!")

    def read_device_log_timeout(self, device_log_file,
                                message_list, timeout):
        LOG.info("The timeout is {} seconds".format(timeout))
        suite_end_count = 0
        suite_start_count = 0
        pattern = "^\\d{2}-\\d{2}\\s+\\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\s+(\\d+)"
        while time.time() - self.start_time <= timeout: 

            with open(device_log_file, "r", encoding='utf-8',
                      errors='ignore') as file_read_pipe:
                pid = ""
                message_list.clear()
                while True:
                    try:
                        line = file_read_pipe.readline()
                    except (UnicodeDecodeError, UnicodeError) as error:
                        LOG.warning("While read log file: %s" % error)
                    if not line:
                        time.sleep(2)  # wait for log write to file
                        break
                    if line.lower().find("jsapp:") != -1:
                        if not pid and "[start] start run suites" in line:
                            matcher = re.match(pattern, line.strip())
                            if matcher and matcher.group(1):
                                pid = matcher.group(1)
                            else:
                                LOG.warning("Can't find pid in suites start")
                        if not pid or pid not in line:
                            continue
                        message_list.append(line)
                        if "[suite end]" in line:
                            suite_end_count += 1
                        if "[suite start]" in line:
                            suite_start_count += 1
                        if "[end] run suites end" in line:
                            LOG.info("Find the end mark then analysis result")
                            return suite_start_count, suite_end_count, True
        else:
            LOG.error("Hjsunit run timeout {}s reached".format(timeout))
            return suite_start_count, suite_end_count, False

    def _run_jsunit(self, config_file, request):
        try:
            if not os.path.exists(config_file):
                LOG.error("Error: Test cases don't exist %s." % config_file)
                raise ParamError(
                    "Error: Test cases don't exist %s." % config_file,
                    error_no="00102")

            json_config = JsonParser(config_file)
            self.kits = get_kit_instances(json_config,
                                          self.config.resource_path,
                                          self.config.testcases_path)

            package, ability_name = self._get_driver_config(json_config)
            self.config.device.hdc_command("target mount")
            do_module_kit_setup(request, self.kits)

            hilog = get_device_log_file(
                request.config.report_path,
                request.config.device.__get_serial__() + "_" + request.
                get_module_name(),
                "device_hilog")

            hilog_open = os.open(hilog, os.O_WRONLY | os.O_CREAT | os.O_APPEND,
                                 0o755)
            # execute test case
            command = "shell aa start -d 123 -a %s -b %s" \
                      % (ability_name, package)

            result_value = self.config.device.hdc_command(command)
            if result_value and "start ability successfully" in\
                    str(result_value).lower():
                setattr(self, "start_success", True)
                LOG.info("execute %s's testcase success. result value=%s"
                         % (package, result_value))
            else:
                LOG.info("execute %s's testcase failed. result value=%s"
                         % (package, result_value))
                raise RuntimeError("hjsunit test run error happened!")

            with os.fdopen(hilog_open, "a") as hilog_file_pipe:
                self.config.device.start_catch_device_log(hilog_file_pipe)

            self.start_time = time.time()
            timeout_config = get_config_value('test-timeout',
                                              json_config.get_driver(),
                                              False, 60000)
            timeout = int(timeout_config) / 1000
            self.generate_console_output(hilog, request, timeout)
        finally:
            do_module_kit_teardown(request)

    def _jsunit_clear(self):
        self.config.device.execute_shell_command(
            "rm -r /%s/%s/%s/%s" % ("data", "local", "tmp", "ajur"))

    def _get_driver_config(self, json_config):
        package = get_config_value('package', json_config.get_driver(), False)
        default_ability = "{}.MainAbility".format(package)
        ability_name = get_config_value('abilityName', json_config.
                                        get_driver(), False, default_ability)
        self.xml_output = get_xml_output(self.config, json_config)
        timeout_config = get_config_value('native-test-timeout',
                                          json_config.get_driver(), False)
        if timeout_config:
            self.timeout = int(timeout_config)

        if not package:
            raise ParamError("Can't find package in config file.",
                             error_no="03201")
        return package, ability_name

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.ltp_posix_test)
class LTPPosixTestDriver(IDriver):
    def __init__(self):
        self.timeout = 80 * 1000
        self.start_time = None
        self.result = ""
        self.error_message = ""
        self.kits = []
        self.config = None

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        pass

    def __execute__(self, request):
        try:
            LOG.debug("Start execute xdevice extension LTP Posix Test")
            self.result = os.path.join(
                request.config.report_path, "result",
                '.'.join((request.get_module_name(), "xml")))
            self.config = request.config
            self.config.device = request.config.environment.devices[0]

            config_file = request.root.source.config_file
            suite_file = request.root.source.source_file

            if not suite_file:
                raise ParamError(
                    "test source '%s' not exists" %
                    request.root.source.source_string, error_no="00110")

            LOG.debug("Test case file path: %s" % suite_file)
            # avoid hilog service stuck issue
            self.config.device.hdc_command("shell stop_service hilogd",
                                           timeout=30 * 1000)
            self.config.device.hdc_command("shell start_service hilogd",
                                           timeout=30 * 1000)
            time.sleep(10)

            self.config.device.hdc_command("shell hilog -r", timeout=30 * 1000)
            self._run_posix(config_file, request)
        except Exception as exception:
            self.error_message = exception
            if not getattr(exception, "error_no", ""):
                setattr(exception, "error_no", "03409")
            LOG.exception(self.error_message, exc_info=True, error_no="03409")
            raise exception
        finally:
            self.config.device.stop_catch_device_log()
            self.result = check_result_report(
                request.config.report_path, self.result, self.error_message)

    def _run_posix(self, config_file, request):
        try:
            if not os.path.exists(config_file):
                LOG.error("Error: Test cases don't exist %s." % config_file)
                raise ParamError(
                    "Error: Test cases don't exist %s." % config_file,
                    error_no="00102")

            json_config = JsonParser(config_file)
            self.kits = get_kit_instances(json_config,
                                          self.config.resource_path,
                                          self.config.testcases_path)
            self.config.device.hdc_command("target mount")
            test_list = None
            dst = None
            for kit in self.kits:
                test_list, dst = kit.__setup__(request.config.device,
                                               request=request)
            # apply execute right
            self.config.device.hdc_command("shell chmod -R 777 {}".format(dst))

            hilog = get_device_log_file(
                request.config.report_path,
                request.config.device.__get_serial__() + "_" + request.
                get_module_name(),
                "device_hilog")

            hilog_open = os.open(hilog, os.O_WRONLY | os.O_CREAT | os.O_APPEND,
                                 0o755)
            with os.fdopen(hilog_open, "a") as hilog_file_pipe:
                for test_bin in test_list:
                    if not test_bin.endswith(".run-test"):
                        continue
                    listeners = request.listeners
                    for listener in listeners:
                        listener.device_sn = self.config.device.device_sn
                    parsers = get_plugin(Plugin.PARSER,
                                         "OpenSourceTest")
                    parser_instances = []
                    for parser in parsers:
                        parser_instance = parser.__class__()
                        parser_instance.suite_name = request.root.source.\
                            test_name
                        parser_instance.test_name = test_bin.replace("./", "")
                        parser_instance.listeners = listeners
                        parser_instances.append(parser_instance)
                    self.handler = ShellHandler(parser_instances)
                    result_message = self.config.device.hdc_command(
                        "shell {}{}".format(dst, test_bin))
                    LOG.info("get result from command {}".
                             format(result_message))
                    process_command_ret(result_message, self.handler)
        finally:
            do_module_kit_teardown(request)

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.oh_kernel_test)
class OHKernelTestDriver(IDriver):
    """
        OpenHarmonyKernelTest
    """
    def __init__(self):
        self.timeout = 30 * 1000
        self.result = ""
        self.error_message = ""
        self.kits = []
        self.config = None
        self.runner = None

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        pass

    def __execute__(self, request):
        try:
            LOG.debug("Start to Execute OpenHarmony Kernel Test")

            self.config = request.config
            self.config.device = request.config.environment.devices[0]

            config_file = request.root.source.config_file

            self.result = "%s.xml" % \
                          os.path.join(request.config.report_path,
                                       "result", request.get_module_name())
            device_log = get_device_log_file(
                request.config.report_path,
                request.config.device.__get_serial__(),
                "device_log")

            hilog = get_device_log_file(
                request.config.report_path,
                request.config.device.__get_serial__(),
                "device_hilog")

            device_log_open = os.open(device_log, os.O_WRONLY | os.O_CREAT |
                                      os.O_APPEND, FilePermission.mode_755)
            hilog_open = os.open(hilog, os.O_WRONLY | os.O_CREAT | os.O_APPEND,
                                 FilePermission.mode_755)
            with os.fdopen(device_log_open, "a") as log_file_pipe, \
                    os.fdopen(hilog_open, "a") as hilog_file_pipe:
                self.config.device.start_catch_device_log(log_file_pipe,
                                                          hilog_file_pipe)
                self._run_oh_kernel(config_file, request.listeners, request)
                log_file_pipe.flush()
                hilog_file_pipe.flush()
        except Exception as exception:
            self.error_message = exception
            if not getattr(exception, "error_no", ""):
                setattr(exception, "error_no", "03409")
            LOG.exception(self.error_message, exc_info=False, error_no="03409")
            raise exception
        finally:
            do_module_kit_teardown(request)
            self.config.device.stop_catch_device_log()
            self.result = check_result_report(
                request.config.report_path, self.result, self.error_message)

    def _run_oh_kernel(self, config_file, listeners=None, request=None):
        try:
            json_config = JsonParser(config_file)
            kits = get_kit_instances(json_config, self.config.resource_path,
                                     self.config.testcases_path)
            self._get_driver_config(json_config)
            do_module_kit_setup(request, kits)
            self.runner = OHKernelTestRunner(self.config)
            self.runner.suite_name = request.get_module_name()
            self.runner.run(listeners)
        finally:
            do_module_kit_teardown(request)

    def _get_driver_config(self, json_config):
        LOG.info("_get_driver_config")
        d = dict(json_config.get_driver())
        for key in d.keys():
            LOG.info("%s:%s" % (key, d[key]))

        target_test_path = get_config_value('native-test-device-path',
                                            json_config.get_driver(), False)
        test_suite_name = get_config_value('test-suite-name',
                                           json_config.get_driver(), False)
        test_suites_list = get_config_value('test-suites-list',
                                            json_config.get_driver(), False)
        timeout_limit = get_config_value('timeout-limit',
                                         json_config.get_driver(), False)
        conf_file = get_config_value('conf-file',
                                     json_config.get_driver(), False)
        self.config.arg_list = {}
        if target_test_path:
            self.config.target_test_path = target_test_path
        if test_suite_name:
            self.config.arg_list["test-suite-name"] = test_suite_name
        if test_suites_list:
            self.config.arg_list["test-suites-list"] = test_suites_list
        if timeout_limit:
            self.config.arg_list["timeout-limit"] = timeout_limit
        if conf_file:
            self.config.arg_list["conf-file"] = conf_file
        timeout_config = get_config_value('shell-timeout',
                                          json_config.get_driver(), False)
        if timeout_config:
            self.config.timeout = int(timeout_config)
        else:
            self.config.timeout = TIME_OUT

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


class OHKernelTestRunner:
    def __init__(self, config):
        self.suite_name = None
        self.config = config
        self.arg_list = config.arg_list

    def run(self, listeners):
        handler = self._get_shell_handler(listeners)
        # hdc shell cd /data/local/tmp/OH_kernel_test;
        # sh runtest test -t OpenHarmony_RK3568_config
        # -n OpenHarmony_RK3568_skiptest -l 60
        command = "cd %s; chmod +x *; sh runtest test %s" % (
            self.config.target_test_path, self.get_args_command())
        self.config.device.execute_shell_command(
            command, timeout=self.config.timeout, receiver=handler, retry=0)

    def _get_shell_handler(self, listeners):
        parsers = get_plugin(Plugin.PARSER, CommonParserType.oh_kernel_test)
        if parsers:
            parsers = parsers[:1]
        parser_instances = []
        for parser in parsers:
            parser_instance = parser.__class__()
            parser_instance.suites_name = self.suite_name
            parser_instance.listeners = listeners
            parser_instances.append(parser_instance)
        handler = ShellHandler(parser_instances)
        return handler

    def get_args_command(self):
        args_commands = ""
        for key, value in self.arg_list.items():
            if key == "test-suite-name" or key == "test-suites-list":
                args_commands = "%s -t %s" % (args_commands, value)
            elif key == "conf-file":
                args_commands = "%s -n %s" % (args_commands, value)
            elif key == "timeout-limit":
                args_commands = "%s -l %s" % (args_commands, value)
        return args_commands


def disable_keyguard(device):
    _unlock_screen(device)
    _unlock_device(device)


def _unlock_screen(device):
    device.execute_shell_command("svc power stayon true")
    time.sleep(1)


def _unlock_device(device):
    device.execute_shell_command("input keyevent 82")
    time.sleep(1)
    device.execute_shell_command("wm dismiss-keyguard")
    time.sleep(1)


def _lock_screen(device):
    time.sleep(1)


def _sleep_according_to_result(result):
    if result:
        time.sleep(1)
