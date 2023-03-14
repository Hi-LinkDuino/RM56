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

import json
import os
import re
import shutil
import time
import platform
import zipfile
import stat
from dataclasses import dataclass

from xdevice import DeviceTestType
from xdevice import DeviceLabelType
from xdevice import ExecuteTerminate
from xdevice import DeviceError
from xdevice import ShellHandler

from xdevice import IDriver
from xdevice import platform_logger
from xdevice import Plugin
from xdevice import get_plugin
from xdevice_extension._core.constants import CommonParserType
from xdevice_extension._core.environment.dmlib import process_command_ret
from core.utils import get_decode
from core.utils import get_fuzzer_path
from core.config.resource_manager import ResourceManager
from core.config.config_manager import FuzzerConfigManager


__all__ = [
    "CppTestDriver",
    "JSUnitTestDriver",
    "disable_keyguard",
    "GTestConst"]

LOG = platform_logger("Drivers")
DEFAULT_TEST_PATH = "/%s/%s/" % ("data", "test")

TIME_OUT = 900 * 1000


##############################################################################
##############################################################################


class DisplayOutputReceiver:
    def __init__(self):
        self.output = ""
        self.unfinished_line = ""

    def _process_output(self, output, end_mark="\n"):
        content = output
        if self.unfinished_line:
            content = "".join((self.unfinished_line, content))
            self.unfinished_line = ""
        lines = content.split(end_mark)
        if content.endswith(end_mark):
            return lines[:-1]
        else:
            self.unfinished_line = lines[-1]
            return lines[:-1]

    def __read__(self, output):
        self.output = "%s%s" % (self.output, output)
        lines = self._process_output(output)
        for line in lines:
            line = line.strip()
            if line:
                LOG.info(get_decode(line))

    def __error__(self, message):
        pass

    def __done__(self, result_code="", message=""):
        pass


@dataclass
class GTestConst(object):
    exec_para_filter = "--gtest_filter"
    exec_para_level = "--gtest_testsize"


def get_device_log_file(report_path, serial=None, log_name="device_log"):
    from xdevice import Variables
    log_path = os.path.join(report_path, Variables.report_vars.log_dir)
    os.makedirs(log_path, exist_ok=True)

    serial = serial or time.time_ns()
    device_file_name = "{}_{}.log".format(log_name, serial)
    device_log_file = os.path.join(log_path, device_file_name)
    return device_log_file


def get_level_para_string(level_string):
    level_list = list(set(level_string.split(",")))
    level_para_string = ""
    for item in level_list:
        if not item.isdigit():
            continue
        item = item.strip(" ")
        level_para_string += ("Level%s," % item)
    level_para_string = level_para_string.strip(",")
    return level_para_string


def get_result_savepath(testsuit_path, result_rootpath):
    findkey = os.sep + "tests" + os.sep
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

    LOG.info("result_savepath = " + result_path)
    return result_path


# all testsuit common Unavailable test result xml
def _create_empty_result_file(filepath, filename, error_message):
    error_message = str(error_message)
    error_message = error_message.replace("\"", "")
    error_message = error_message.replace("<", "")
    error_message = error_message.replace(">", "")
    error_message = error_message.replace("&", "")
    if filename.endswith(".hap"):
        filename = filename.split(".")[0]
    if not os.path.exists(filepath):
        with open(filepath, "w", encoding='utf-8') as file_desc:
            time_stamp = time.strftime("%Y-%m-%d %H:%M:%S",
                                       time.localtime())
            file_desc.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            file_desc.write(
                '<testsuites tests="0" failures="0" '
                'disabled="0" errors="0" timestamp="%s" '
                'time="0" name="AllTests">\n' % time_stamp)
            file_desc.write(
                '  <testsuite name="%s" tests="0" failures="0" '
                'disabled="0" errors="0" time="0.0" '
                'unavailable="1" message="%s">\n' %
                (filename, error_message))
            file_desc.write('  </testsuite>\n')
            file_desc.write('</testsuites>\n')
    return


def _unlock_screen(device):
    device.execute_shell_command("svc power stayon true")
    time.sleep(1)


def _unlock_device(device):
    device.execute_shell_command("input keyevent 82")
    time.sleep(1)
    device.execute_shell_command("wm dismiss-keyguard")
    time.sleep(1)


def _lock_screen(device):
    device.execute_shell_command("svc power stayon false")
    time.sleep(1)


def disable_keyguard(device):
    _unlock_screen(device)
    _unlock_device(device)


def _sleep_according_to_result(result):
    if result:
        time.sleep(1)

def _create_fuzz_crash_file(filepath, filename):
    if not os.path.exists(filepath):
        with open(filepath, "w", encoding='utf-8') as file_desc:
            time_stamp = time.strftime("%Y-%m-%d %H:%M:%S",
                                       time.localtime())
            file_desc.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            file_desc.write(
                '<testsuites disabled="0" name="AllTests" '
                'time="300" timestamp="%s" errors="0" '
                'failures="1" tests="1">\n' % time_stamp)
            file_desc.write(
                '  <testsuite disabled="0" name="%s" time="300" '
                'errors="0" failures="1" tests="1">\n' % filename)
            file_desc.write(
                '    <testcase name="%s" time="300" classname="%s" '
                'status="run">\n' % (filename, filename))
            file_desc.write(
                '      <failure type="" '
                'message="Fuzzer crash. See ERROR in log file">\n')
            file_desc.write('      </failure>\n')
            file_desc.write('    </testcase>\n')
            file_desc.write('  </testsuite>\n')
            file_desc.write('</testsuites>\n')
    return

def _create_fuzz_pass_file(filepath, filename):
    if not os.path.exists(filepath):
        with open(filepath, "w", encoding='utf-8') as file_desc:
            time_stamp = time.strftime("%Y-%m-%d %H:%M:%S",
                                       time.localtime())
            file_desc.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            file_desc.write(
                '<testsuites disabled="0" name="AllTests" '
                'time="300" timestamp="%s" errors="0" '
                'failures="0" tests="1">\n' % time_stamp)
            file_desc.write(
                '  <testsuite disabled="0" name="%s" time="300" '
                'errors="0" failures="0" tests="1">\n' % filename)
            file_desc.write(
                '    <testcase name="%s" time="300" classname="%s" '
                'status="run"/>\n' % (filename, filename))
            file_desc.write('  </testsuite>\n')
            file_desc.write('</testsuites>\n')
    return

def _create_fuzz_result_file(filepath, filename, error_message):
    error_message = str(error_message)
    error_message = error_message.replace("\"", "")
    error_message = error_message.replace("<", "")
    error_message = error_message.replace(">", "")
    error_message = error_message.replace("&", "")
    if "AddressSanitizer" in error_message:
        LOG.error("FUZZ TEST CRASH")
        _create_fuzz_crash_file(filepath, filename)
    elif re.search(r'Done (\b\d+\b) runs in (\b\d+\b) second',
                   error_message, re.M) is not None:
        LOG.info("FUZZ TEST PASS")
        _create_fuzz_pass_file(filepath, filename)
    else:
        LOG.error("FUZZ TEST UNAVAILABLE")
        _create_empty_result_file(filepath, filename, error_message)
    return
##############################################################################
##############################################################################

class ResultManager(object):
    def __init__(self, testsuit_path, config):
        self.testsuite_path = testsuit_path
        self.config = config
        self.result_rootpath = self.config.report_path
        self.device = self.config.device
        if testsuit_path.endswith(".hap"):
            self.device_testpath = self.config.test_hap_out_path
        else:
            self.device_testpath = self.config.target_test_path
        self.testsuite_name = os.path.basename(self.testsuite_path)
        self.is_coverage = False

    def set_is_coverage(self, is_coverage):
        self.is_coverage = is_coverage

    def get_test_results(self, error_message=""):
        # Get test result files
        filepath = self.obtain_test_result_file()
        if "fuzztest" == self.config.testtype[0]:
            LOG.info("create fuzz test report")
            _create_fuzz_result_file(filepath, self.testsuite_name,
                                     error_message)
            return filepath
        if not os.path.exists(filepath):
            _create_empty_result_file(filepath, self.testsuite_name,
                                      error_message)
        if "benchmark" == self.config.testtype[0]:
            self._obtain_benchmark_result()
        # Get coverage data files
        if self.is_coverage:
            self.obtain_coverage_data()

        return filepath

    def _obtain_benchmark_result(self):
        benchmark_root_dir = os.path.abspath(
            os.path.join(self.result_rootpath, "benchmark"))
        benchmark_dir = os.path.abspath(
            os.path.join(benchmark_root_dir,
                         self.get_result_sub_save_path(),
                         self.testsuite_name))

        if not os.path.exists(benchmark_dir):
            os.makedirs(benchmark_dir)

        LOG.info("benchmark_dir = %s" % benchmark_dir)
        self.device.pull_file(os.path.join(self.device_testpath,
            "%s.json" % self.testsuite_name), benchmark_dir)
        if not os.path.exists(os.path.join(benchmark_dir,
            "%s.json" % self.testsuite_name)):
            os.rmdir(benchmark_dir)
        return benchmark_dir

    def get_result_sub_save_path(self):
        find_key = os.sep + "benchmark" + os.sep
        file_dir, _ = os.path.split(self.testsuite_path)
        pos = file_dir.find(find_key)
        subpath = ""
        if -1 != pos:
            subpath = file_dir[pos + len(find_key):]
        LOG.info("subpath = " + subpath)
        return subpath

    def obtain_test_result_file(self):
        result_save_path = get_result_savepath(self.testsuite_path,
            self.result_rootpath)
        result_file_path = os.path.join(result_save_path,
            "%s.xml" % self.testsuite_name)

        result_josn_file_path = os.path.join(result_save_path,
            "%s.json" % self.testsuite_name)

        if self.testsuite_path.endswith('.hap'):
            remote_result_file = os.path.join(self.device_testpath,
                "testcase_result.xml")
            remote_json_result_file = os.path.join(self.device_testpath,
                "%s.json" % self.testsuite_name)
        else:
            remote_result_file = os.path.join(self.device_testpath,
                "%s.xml" % self.testsuite_name)
            remote_json_result_file = os.path.join(self.device_testpath,
                "%s.json" % self.testsuite_name)

        if self.device.is_file_exist(remote_result_file):
            self.device.pull_file(remote_result_file, result_file_path)
        elif self.device.is_file_exist(remote_json_result_file):
            self.device.pull_file(remote_json_result_file,
                                  result_josn_file_path)
            result_file_path = result_josn_file_path
        else:
            LOG.error("%s not exist", remote_result_file)

        return result_file_path

    def make_empty_result_file(self, error_message=""):
        result_savepath = get_result_savepath(self.testsuite_path,
            self.result_rootpath)
        result_filepath = os.path.join(result_savepath, "%s.xml" %
            self.testsuite_name)
        if not os.path.exists(result_filepath):
            _create_empty_result_file(result_filepath,
                self.testsuite_name, error_message)

    def is_exist_target_in_device(self, path, target):
        if platform.system() == "Windows":
            command = '\"ls -l %s | grep %s\"' % (path, target)
        else:
            command = "ls -l %s | grep %s" % (path, target)

        check_result = False
        stdout_info = self.device.execute_shell_command(command)
        if stdout_info != "" and stdout_info.find(target) != -1:
            check_result = True
        return check_result

    def obtain_coverage_data(self):
        cov_root_dir = os.path.abspath(os.path.join(
            self.result_rootpath,
            "..",
            "coverage",
            "data",
            "exec"))

        target_name = "obj"
        cxx_cov_path = os.path.abspath(os.path.join(
            self.result_rootpath,
            "..",
            "coverage",
            "data",
            "cxx",
            self.testsuite_name))

        if self.is_exist_target_in_device(DEFAULT_TEST_PATH, target_name):
            if not os.path.exists(cxx_cov_path):
                os.makedirs(cxx_cov_path)
            src_file = os.path.join(DEFAULT_TEST_PATH, target_name)
            self.device.pull_file(src_file, cxx_cov_path, is_create=True)


##############################################################################
##############################################################################

@Plugin(type=Plugin.DRIVER, id=DeviceTestType.cpp_test)
class CppTestDriver(IDriver):
    """
    CppTest is a Test that runs a native test package on given device.
    """
    # test driver config
    config = None
    result = ""

    def __check_environment__(self, device_options):
        if len(device_options) == 1 and device_options[0].label is None:
            return True
        if len(device_options) != 1 or \
                device_options[0].label != DeviceLabelType.phone:
            return False
        return True

    def __check_config__(self, config):
        pass

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""

    def __execute__(self, request):
        try:
            self.config = request.config
            self.config.target_test_path = DEFAULT_TEST_PATH
            self.config.device = request.config.environment.devices[0]

            suite_file = request.root.source.source_file
            LOG.debug("Testsuite FilePath: %s" % suite_file)

            if not suite_file:
                LOG.error("test source '%s' not exists" %
                          request.root.source.source_string)
                return

            if not self.config.device:
                result = ResultManager(suite_file, self.config)
                result.set_is_coverage(False)
                result.make_empty_result_file(
                    "No test device is found. ")
                return

            serial = request.config.device.__get_serial__()
            device_log_file = get_device_log_file(
                request.config.report_path,
                serial)

            with open(device_log_file, "a", encoding="UTF-8") as file_pipe:
                self.config.device.start_catch_device_log(file_pipe)
                self._init_gtest()
                self._run_gtest(suite_file)
        finally:
            self.config.device.stop_catch_device_log()

    def _init_gtest(self):
        self.config.device.hdc_command("remount")
        self.config.device.execute_shell_command(
            "rm -rf %s" % self.config.target_test_path)
        self.config.device.execute_shell_command(
            "mkdir -p %s" % self.config.target_test_path)
        if "fuzztest" == self.config.testtype[0]:
            self.config.device.execute_shell_command(
                "mkdir -p %s" % os.path.join(self.config.target_test_path,
                "corpus"))

    def _run_gtest(self, suite_file):
        from xdevice import Variables
        filename = os.path.basename(suite_file)
        test_para = self._get_test_para(self.config.testcase,
                                        self.config.testlevel,
                                        self.config.testtype,
                                        self.config.target_test_path,
                                        filename)
        is_coverage_test = True if self.config.coverage else False

        # push testsuite file
        self.config.device.push_file(suite_file, self.config.target_test_path)
        self._push_corpus_if_exist(filename)

        # push resource files
        resource_manager = ResourceManager()
        resource_data_dic, resource_dir = \
            resource_manager.get_resource_data_dic(suite_file)
        resource_manager.process_preparer_data(resource_data_dic, resource_dir,
                                               self.config.device)

        # execute testcase
        if not self.config.coverage:
            command = "cd %s; rm -rf %s.xml; chmod +x *; ./%s %s" % (
                self.config.target_test_path,
                filename,
                filename,
                test_para)
        else:
            coverage_outpath = self.config.coverage_outpath
            strip_num = len(coverage_outpath.split(os.sep)) - 1
            command = "cd %s; rm -rf %s.xml; chmod +x *; GCOV_PREFIX=. " \
                "GCOV_PREFIX_STRIP=%s ./%s %s" % \
                (self.config.target_test_path,
                 filename,
                 str(strip_num),
                 filename,
                 test_para)

        result = ResultManager(suite_file, self.config)
        result.set_is_coverage(is_coverage_test)

        try:
            # get result
            display_receiver = DisplayOutputReceiver()
            self.config.device.execute_shell_command(
                command,
                receiver=display_receiver,
                timeout=TIME_OUT,
                retry=0)
            return_message = display_receiver.output
        except (ExecuteTerminate, DeviceError) as exception:
            return_message = str(exception.args)

        self.result = result.get_test_results(return_message)
        resource_manager.process_cleaner_data(resource_data_dic,
            resource_dir,
            self.config.device)

    def _push_corpus_if_exist(self, filename):
        if "fuzztest" == self.config.testtype[0]:
            corpus_path = os.path.join(get_fuzzer_path(filename), "corpus")
            self.config.device.push_file(corpus_path,
                os.path.join(self.config.target_test_path, "corpus"))

    @staticmethod
    def _get_test_para(testcase,
                       testlevel,
                       testtype,
                       target_test_path,
                       filename):
        if "benchmark" == testtype[0]:
            test_para = (" --benchmark_out_format=json"
                         " --benchmark_out=%s%s.json") % (
                            target_test_path, filename)
            return test_para

        if "" != testcase and "" == testlevel:
            test_para = "%s=%s" % (GTestConst.exec_para_filter, testcase)
        elif "" == testcase and "" != testlevel:
            level_para = get_level_para_string(testlevel)
            test_para = "%s=%s" % (GTestConst.exec_para_level, level_para)
        else:
            test_para = ""

        if "fuzztest" == testtype[0]:
            cfg_list = FuzzerConfigManager(os.path.join(get_fuzzer_path(
                filename), "project.xml")).get_fuzzer_config("fuzztest")
            LOG.info("config list :%s" % str(cfg_list))
            test_para += "corpus -max_len=" + cfg_list[0] + \
                         " -max_total_time=" + cfg_list[1] + \
                         " -rss_limit_mb=" + cfg_list[2]
        return test_para


##############################################################################
##############################################################################

@Plugin(type=Plugin.DRIVER, id=DeviceTestType.jsunit_test)
class JSUnitTestDriver(IDriver):
    """
    JSUnitTestDriver is a Test that runs a native test package on given device.
    """

    def __init__(self):
        self.config = None
        self.result = ""
        self.start_time = None
        self.ability_name = ""
        self.package_name = ""

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        pass

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""

    def __execute__(self, request):
        try:
            LOG.info("developertest driver")
            self.result = os.path.join(
                request.config.report_path, "result",
                '.'.join((request.get_module_name(), "xml")))
            self.config = request.config
            self.config.target_test_path = DEFAULT_TEST_PATH
            self.config.device = request.config.environment.devices[0]

            suite_file = request.root.source.source_file
            if not suite_file:
                LOG.error("test source '%s' not exists" %
                          request.root.source.source_string)
                return

            if not self.config.device:
                result = ResultManager(suite_file, self.config)
                result.set_is_coverage(False)
                result.make_empty_result_file(
                    "No test device is found")
                return

            package_name, ability_name = self._get_package_and_ability_name(
                suite_file)
            self.package_name = package_name
            self.ability_name = ability_name
            self.config.test_hap_out_path = \
                "/data/data/%s/files/" % self.package_name
            self.config.device.hdc_command("shell hilog -r")

            hilog = get_device_log_file(
                request.config.report_path,
                request.config.device.__get_serial__() + "_" + request.
                get_module_name(),
                "device_hilog")

            hilog_open = os.open(hilog, os.O_WRONLY | os.O_CREAT | os.O_APPEND,
                                 0o755)

            with os.fdopen(hilog_open, "a") as hilog_file_pipe:
                self.config.device.start_catch_device_log(hilog_file_pipe)
                self._init_jsunit_test()
                self._run_jsunit(suite_file)
                hilog_file_pipe.flush()
                self.generate_console_output(hilog, request)
        finally:
            self.config.device.stop_catch_device_log()

    def _init_jsunit_test(self):
        self.config.device.hdc_command("target mount")
        self.config.device.execute_shell_command(
            "rm -rf %s" % self.config.target_test_path)
        self.config.device.execute_shell_command(
            "mkdir -p %s" % self.config.target_test_path)
        self.config.device.execute_shell_command(
            "mount -o rw,remount,rw /%s" % "system")


    def _run_jsunit(self, suite_file):
        filename = os.path.basename(suite_file)

        resource_manager = ResourceManager()
        resource_data_dic, resource_dir = \
            resource_manager.get_resource_data_dic(suite_file)
        resource_manager.process_preparer_data(resource_data_dic, resource_dir,
                                               self.config.device)

        main_result = self._install_hap(suite_file)
        result = ResultManager(suite_file, self.config)

        if main_result:
            self._execute_hapfile_jsunittest()
            self._uninstall_hap(self.package_name)
        else:
            self.result = result.get_test_results("Error: install hap failed")
            LOG.error("Error: install hap failed")

        resource_manager.process_cleaner_data(resource_data_dic, resource_dir,
                                              self.config.device)

    def generate_console_output(self, device_log_file, request):
        result_message = self.read_device_log(device_log_file)

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

    def read_device_log(self, device_log_file):
        device_log_file_open = os.open(device_log_file, os.O_RDONLY,
                                       stat.S_IWUSR | stat.S_IRUSR)

        result_message = ""
        with os.fdopen(device_log_file_open, "r", encoding='utf-8') \
                as file_read_pipe:
            while True:
                data = file_read_pipe.readline()
                if not data or not data.strip():
                    break
                # only filter JSApp log
                if data.find("JSApp:") != -1:
                    result_message += data
                    if data.find("[end] run suites end") != -1:
                        break
        return result_message

    def _execute_hapfile_jsunittest(self):
        _unlock_screen(self.config.device)
        _unlock_device(self.config.device)

        try:
            return_message = self.start_hap_activity()
        except (ExecuteTerminate, DeviceError) as exception:
            return_message = str(exception.args)

        _lock_screen(self.config.device)
        return return_message

    def _install_hap(self, suite_file):
        message = self.config.device.hdc_command("install %s" % suite_file)
        message = str(message).rstrip()
        if message == "" or "success" in message:
            return_code = True
            if message != "":
                LOG.info(message)
        else:
            return_code = False
            if message != "":
                LOG.warning(message)

        _sleep_according_to_result(return_code)
        return return_code

    def start_hap_activity(self):
        try:
            command = "aa start -d 123 -a %s.MainAbility -b %s" \
                      % (self.package_name, self.package_name)
            self.start_time = time.time()
            result_value = self.config.device.execute_shell_command(
                command, timeout=TIME_OUT)

            if "success" in str(result_value).lower():
                LOG.info("execute %s's testcase success. result value=%s"
                         % (self.package_name, result_value))
                time.sleep(30)
            else:
                LOG.info("execute %s's testcase failed. result value=%s"
                         % (self.package_name, result_value))

            _sleep_according_to_result(result_value)
            return_message = result_value
        except (ExecuteTerminate, DeviceError) as exception:
            return_message = exception.args

        return return_message

    def _uninstall_hap(self, package_name):
        return_message = self.config.device.execute_shell_command(
            "bm uninstall -n %s" % package_name)
        _sleep_according_to_result(return_message)
        return return_message

    @staticmethod
    def _get_package_and_ability_name(hap_filepath):
        package_name = ""
        ability_name = ""

        if os.path.exists(hap_filepath):
            filename = os.path.basename(hap_filepath)

            #unzip the hap file
            hap_bak_path = os.path.abspath(os.path.join(
                os.path.dirname(hap_filepath),
                "%s.bak" % filename))
            zf_desc = zipfile.ZipFile(hap_filepath)
            try:
                zf_desc.extractall(path=hap_bak_path)
            except RuntimeError as error:
                print(error)
            zf_desc.close()

            #verify config.json file
            app_profile_path = os.path.join(hap_bak_path, "config.json")
            if not os.path.exists(app_profile_path):
                print("file %s not exist" % app_profile_path)
                return package_name, ability_name

            if os.path.isdir(app_profile_path):
                print("%s is a folder, and not a file" % app_profile_path)
                return package_name, ability_name

            #get package_name and ability_name value
            load_dict = {}
            with open(app_profile_path, 'r') as load_f:
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
                        ability_name = package_name + abilities_name[
                                       abilities_name.find("."):]
                    else:
                        ability_name = abilities_name
                    break
                break

            #delete hap_bak_path
            if os.path.exists(hap_bak_path):
                shutil.rmtree(hap_bak_path)
        else:
            print("file %s not exist" % hap_filepath)

        return package_name, ability_name
