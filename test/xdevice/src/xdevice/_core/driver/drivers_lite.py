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

import os
import shutil
import glob
import time
import stat

from _core.config.config_manager import UserConfigManager
from _core.constants import ConfigConst
from _core.constants import DeviceTestType
from _core.constants import GTestConst
from _core.constants import DeviceLabelType
from _core.constants import ComType
from _core.constants import ParserType
from _core.constants import CKit
from _core.constants import DeviceLiteKernel
from _core.constants import FilePermission
from _core.driver.parser_lite import ShellHandler
from _core.environment.dmlib_lite import generate_report
from _core.exception import ExecuteTerminate
from _core.exception import LiteDeviceError
from _core.exception import LiteDeviceExecuteCommandError
from _core.exception import ParamError
from _core.executor.listener import CollectingLiteGTestListener
from _core.executor.listener import TestDescription
from _core.interface import IDriver
from _core.plugin import Plugin
from _core.plugin import get_plugin
from _core.logger import platform_logger
from _core.report.reporter_helper import DataHelper
from _core.testkit.json_parser import JsonParser
from _core.testkit.kit_lite import DeployKit
from _core.testkit.kit_lite import DeployToolKit
from _core.utils import get_config_value
from _core.utils import get_kit_instances
from _core.utils import check_result_report
from _core.utils import get_device_log_file
from _core.utils import get_filename_extension
from _core.utils import get_file_absolute_path
from _core.utils import get_test_component_version
from _core.report.suite_reporter import SuiteReporter

__all__ = ["CppTestDriver", "CTestDriver", "init_remote_server"]
LOG = platform_logger("DriversLite")
FAILED_RUN_TEST_ATTEMPTS = 2
CPP_TEST_MOUNT_STOP_SIGN = "not mount properly, Test Stop"
CPP_TEST_NFS_SIGN = "execve: I/O error"


def get_nfs_server(request):
    config_manager = UserConfigManager(
        config_file=request.get(ConfigConst.configfile, ""),
        env=request.get(ConfigConst.test_environment, ""))
    remote_info = config_manager.get_user_config("testcases/server",
                                                 filter_name="NfsServer")
    if not remote_info:
        err_msg = "The name of remote nfs server does not match"
        LOG.error(err_msg, error_no="00403")
        raise ParamError(err_msg, error_no="00403")
    return remote_info


def init_remote_server(lite_instance, request=None):
    config_manager = UserConfigManager(
        config_file=request.get(ConfigConst.configfile, ""),
        env=request.get(ConfigConst.test_environment, ""))
    linux_dict = config_manager.get_user_config("testcases/server")

    if linux_dict:
        setattr(lite_instance, "linux_host", linux_dict.get("ip"))
        setattr(lite_instance, "linux_port", linux_dict.get("port"))
        setattr(lite_instance, "linux_directory", linux_dict.get("dir"))

    else:
        error_message = "nfs server does not exist, please " \
                        "check user_config.xml"
        raise ParamError(error_message, error_no="00108")


def get_testcases(testcases_list):
    cases_list = []
    for test in testcases_list:
        test_item = test.split("#")
        if len(test_item) == 1:
            cases_list.append(test)
        elif len(test_item) == 2:
            cases_list.append(test_item[-1])
    return cases_list


def sort_by_length(file_name):
    return len(file_name)


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.cpp_test_lite)
class CppTestDriver(IDriver):
    """
    CppTest is a test that runs a native test package on given lite device.
    """
    config = None
    result = ""
    error_message = ""

    def __init__(self):
        self.rerun = True
        self.file_name = ""

    def __check_environment__(self, device_options):
        if len(device_options) != 1 or \
                device_options[0].label != DeviceLabelType.ipcamera:
            self.error_message = "check environment failed"
            return False
        return True

    def __check_config__(self, config=None):
        pass

    def __execute__(self, request):
        kits = []
        device_log_file = get_device_log_file(
            request.config.report_path,
            request.get_devices()[0].__get_serial__())
        try:
            self.config = request.config
            self.init_cpp_config()
            self.config.device = request.config.environment.devices[0]
            init_remote_server(self, request=request)
            config_file = request.root.source.config_file
            json_config = JsonParser(config_file)
            self._get_driver_config(json_config)

            bin_file = get_config_value('execute', json_config.get_driver(),
                                        False)
            kits = get_kit_instances(json_config,
                                     request.config.resource_path,
                                     request.config.testcases_path)
            from xdevice import Scheduler
            for kit in kits:
                if not Scheduler.is_execute:
                    raise ExecuteTerminate("ExecuteTerminate",
                                           error_no="00300")
                kit.__setup__(request.config.device, request=request)

            command = self._get_execute_command(bin_file)

            self.set_file_name(request, command)

            if self.config.xml_output:
                self.delete_device_xml(request, self.config.device_xml_path)
            if os.path.exists(self.result):
                os.remove(self.result)
            if request.config.testargs.get("dry_run"):
                self.config.dry_run = request.config.testargs.get(
                    "dry_run")[0].lower()
                self.dry_run(command, request.listeners)
            else:
                self.run_cpp_test(command, request)
                self.generate_device_xml(request, self.execute_bin)

        except (LiteDeviceError, Exception) as exception:
            LOG.exception(exception, exc_info=False)
            self.error_message = exception
        finally:
            device_log_file_open = os.open(device_log_file, os.O_WRONLY |
                                           os.O_CREAT | os.O_APPEND,
                                           FilePermission.mode_755)
            with os.fdopen(device_log_file_open, "a") as file_name:
                file_name.write(self.config.command_result)
                file_name.flush()
            LOG.info("-------------finally-----------------")
            self._after_command(kits, request)

    def _get_execute_command(self, bin_file):
        if self.config.device.get("device_kernel") == \
                DeviceLiteKernel.linux_kernel:
            execute_dir = "/storage" + "/".join(bin_file.split("/")[0:-1])
        else:
            execute_dir = "/".join(bin_file.split("/")[0:-1])
        self.execute_bin = bin_file.split("/")[-1]

        self.config.device.execute_command_with_timeout(
            command="cd {}".format(execute_dir), timeout=1)
        self.config.execute_bin_path = execute_dir

        if self.execute_bin.startswith("/"):
            command = ".%s" % self.execute_bin
        else:
            command = "./%s" % self.execute_bin

        report_path = "/%s/%s/" % ("reports", self.execute_bin.split(".")[0])
        self.config.device_xml_path = (self.linux_directory + report_path).\
            replace("//", "/")
        self.config.device_report_path = execute_dir + report_path

        return command

    def _get_driver_config(self, json_config):
        xml_output = get_config_value('xml-output',
                                      json_config.get_driver(), False)

        if isinstance(xml_output, bool):
            self.config.xml_output = xml_output
        elif str(xml_output).lower() == "false":
            self.config.xml_output = False
        else:
            self.config.xml_output = True

        rerun = get_config_value('rerun', json_config.get_driver(), False)
        if isinstance(rerun, bool):
            self.rerun = rerun
        elif str(rerun).lower() == "false":
            self.rerun = False
        else:
            self.rerun = True

        timeout_config = get_config_value('timeout',
                                              json_config.get_driver(), False)
        if timeout_config:
            self.config.timeout = int(timeout_config)
        else:
            self.config.timeout = 900

    def _after_command(self, kits, request):
        if self.config.device.get("device_kernel") == \
                DeviceLiteKernel.linux_kernel:
            self.config.device.execute_command_with_timeout(
                command="cd /storage", timeout=1)
        else:
            self.config.device.execute_command_with_timeout(
                command="cd /", timeout=1)
        for kit in kits:
            kit.__teardown__(request.config.device)
        self.config.device.close()
        self.delete_device_xml(request, self.linux_directory)

        report_name = "report" if request.root.source. \
            test_name.startswith("{") else get_filename_extension(
            request.root.source.test_name)[0]
        if not self.config.dry_run:
            self.result = check_result_report(
                request.config.report_path, self.result, self.error_message,
                report_name)

    def generate_device_xml(self, request, execute_bin):
        if self.config.xml_output:
            self.download_nfs_xml(request, self.config.device_xml_path)
            self.merge_xml(execute_bin)

    def dry_run(self, request, command, listener=None):
        if self.config.xml_output:
            collect_test_command = "%s --gtest_output=xml:%s " \
                                   "--gtest_list_tests" % \
                                   (command, self.config.device_report_path)
            result, _, _ = self.config.device.execute_command_with_timeout(
                command=collect_test_command,
                case_type=DeviceTestType.cpp_test_lite,
                timeout=15, receiver=None)
            if CPP_TEST_MOUNT_STOP_SIGN in result:
                tests = []
                return tests
            tests = self.read_nfs_xml(request, self.config.device_xml_path)
            self.delete_device_xml(request, self.config.device_xml_path)
            return tests

        else:
            parsers = get_plugin(Plugin.PARSER, ParserType.cpp_test_list_lite)
            parser_instances = []
            for parser in parsers:
                parser_instance = parser.__class__()
                parser_instance.suites_name = os.path.basename(self.result)
                if listener:
                    parser_instance.listeners = listener
                parser_instances.append(parser_instance)
            handler = ShellHandler(parser_instances)

            collect_test_command = "%s --gtest_list_tests" % command
            result, _, _ = self.config.device.execute_command_with_timeout(
                command=collect_test_command,
                case_type=DeviceTestType.cpp_test_lite,
                timeout=15, receiver=handler)
            self.config.command_result = "{}{}".format(
                self.config.command_result, result)
            if parser_instances[0].tests and \
                    len(parser_instances[0].tests) > 0:
                SuiteReporter.set_suite_list([item.test_name for item in
                                              parser_instances[0].tests])
            else:
                SuiteReporter.set_suite_list([])
            tests = parser_instances[0].tests
        if not tests:
            LOG.error("collect test failed!", error_no="00402")
        return parser_instances[0].tests

    def run_cpp_test(self, command, request):
        if request.config.testargs.get("test"):
            testcases_list = get_testcases(
                request.config.testargs.get("test"))
            for test in testcases_list:
                command_case = "{} --gtest_filter=*{}".format(
                    command, test)

                if not self.config.xml_output:
                    self.run(command_case, request.listeners, timeout=15)
                else:
                    command_case = "{} --gtest_output=xml:{}".format(
                        command_case, self.config.device_report_path)
                    self.run(command_case, None, timeout=15)
        else:
            self._do_test_run(command, request)

    def init_cpp_config(self):
        setattr(self.config, "command_result", "")
        setattr(self.config, "device_xml_path", "")
        setattr(self.config, "dry_run", False)

    def merge_xml(self, execute_bin):
        report_path = os.path.join(self.config.report_path, "result")
        summary_result = DataHelper.get_summary_result(
            report_path, self.result, key=sort_by_length,
            file_prefix=execute_bin)
        if summary_result:
            SuiteReporter.append_report_result((
                os.path.join(report_path, "%s.xml" % execute_bin),
                DataHelper.to_string(summary_result)))
        else:
            self.error_message = "The test case did not generate XML"
        for xml_file in os.listdir(os.path.split(self.result)[0]):
            if not xml_file.startswith(execute_bin):
                continue
            if xml_file != os.path.split(self.result)[1]:
                os.remove(os.path.join(os.path.split(
                    self.result)[0], xml_file))

    def set_file_name(self, request, command):
        self.file_name = command.split(" ")[0].split("/")[-1].split(".")[0]
        self.result = "%s.xml" % os.path.join(request.config.report_path,
                                              "result", self.file_name)

    def run(self, command=None, listener=None, timeout=None):
        if not timeout:
            timeout = self.config.timeout
        if listener:
            parsers = get_plugin(Plugin.PARSER, ParserType.cpp_test_lite)
            parser_instances = []
            for parser in parsers:
                parser_instance = parser.__class__()
                parser_instance.suite_name = self.file_name
                parser_instance.listeners = listener
                parser_instances.append(parser_instance)
            handler = ShellHandler(parser_instances)
        else:
            handler = None
        result, _, error = self.config.device.execute_command_with_timeout(
            command=command, case_type=DeviceTestType.cpp_test_lite,
            timeout=timeout, receiver=handler)
        self.config.command_result += result
        if result.count(CPP_TEST_NFS_SIGN) >= 1:
            _, _, error = self.config.device.execute_command_with_timeout(
                command="ping %s" % self.linux_host,
                case_type=DeviceTestType.cpp_test_lite,
                timeout=5)
        return error, result, handler

    def _do_test_run(self, command, request):
        test_to_run = self._collect_test_to_run(request, command)
        self._run_with_rerun(command, request, test_to_run)

    def _run_with_rerun(self, command, request, expected_tests):
        if self.config.xml_output:
            self.run("{} --gtest_output=xml:{}".format(
                command, self.config.device_report_path))
            time.sleep(5)
            test_rerun = True
            if self.check_xml_exist(self.execute_bin + ".xml"):
                test_rerun = False
            test_run = self.read_nfs_xml(request, self.config.device_xml_path,
                                         test_rerun)

            if len(test_run) < len(expected_tests):
                expected_tests = TestDescription.remove_test(expected_tests,
                                                             test_run)
                self._rerun_tests(command, expected_tests, None)
        else:
            test_tracker = CollectingLiteGTestListener()
            listener = request.listeners
            listener_copy = listener.copy()
            listener_copy.append(test_tracker)
            self.run(command, listener_copy)
            test_run = test_tracker.get_current_run_results()
            if len(test_run) != len(expected_tests):
                expected_tests = TestDescription.remove_test(expected_tests,
                                                             test_run)
                self._rerun_tests(command, expected_tests, listener)

    def _rerun_tests(self, command, expected_tests, listener):
        if not expected_tests:
            LOG.debug("No tests to re-run, all tests executed at least once.")
        for test in expected_tests:
            self._re_run(command, test, listener)

    def _re_run(self, command, test, listener):
        if self.config.xml_output:
            _, _, handler = self.run("{} {}=*{} --gtest_output=xml:{}".format(
                command, GTestConst.exec_para_filter, test.test_name,
                self.config.device_report_path),
                listener, timeout=15)
        else:
            handler = None
            for _ in range(FAILED_RUN_TEST_ATTEMPTS):
                try:
                    listener_copy = listener.copy()
                    test_tracker = CollectingLiteGTestListener()
                    listener_copy.append(test_tracker)
                    _, _, handler = self.run("{} {}=*{}".format(
                        command, GTestConst.exec_para_filter, test.test_name),
                        listener_copy, timeout=15)
                    if len(test_tracker.get_current_run_results()):
                        return
                except LiteDeviceError as _:
                    LOG.debug("Exception: ShellCommandUnresponsiveException")
            handler.parsers[0].mark_test_as_failed(test)

    def _collect_test_to_run(self, request, command):
        if self.rerun:
            tests = self.dry_run(request, command)
            return tests
        return []

    def download_nfs_xml(self, request, report_path):
        remote_nfs = get_nfs_server(request)
        if not remote_nfs:
            err_msg = "The name of remote device {} does not match". \
                format(self.remote)
            LOG.error(err_msg, error_no="00403")
            raise TypeError(err_msg)
        LOG.info("Trying to pull remote server: {}:{} report files to local "
                 "in dir {}".format
                 (remote_nfs.get("ip"), remote_nfs.get("port"),
                  os.path.dirname(self.result)))
        result_dir = os.path.join(request.config.report_path, "result")
        os.makedirs(result_dir, exist_ok=True)
        try:
            if remote_nfs["remote"] == "true":
                import paramiko
                client = paramiko.Transport((remote_nfs.get("ip"),
                                             int(remote_nfs.get("port"))))
                client.connect(username=remote_nfs.get("username"),
                               password=remote_nfs.get("password"))
                sftp = paramiko.SFTPClient.from_transport(client)
                files = sftp.listdir(report_path)

                for report_xml in files:
                    if report_xml.endswith(".xml"):
                        filepath = report_path + report_xml
                        try:
                            sftp.get(remotepath=filepath,
                                     localpath=os.path.join(os.path.split(
                                         self.result)[0], report_xml))
                        except IOError as error:
                            LOG.error(error, error_no="00404")
                client.close()
            else:
                if os.path.isdir(report_path):
                    for report_xml in os.listdir(report_path):
                        if report_xml.endswith(".xml"):
                            filepath = report_path + report_xml
                            shutil.copy(filepath,
                                        os.path.join(os.path.split(
                                            self.result)[0], report_xml))
        except (FileNotFoundError, IOError) as error:
            LOG.error("download xml failed %s" % error, error_no="00403")

    def check_xml_exist(self, xml_file, timeout=60):
        ls_command = "ls %s" % self.config.device_report_path
        start_time = time.time()
        while time.time() - start_time < timeout:
            result, _, _ = self.config.device.execute_command_with_timeout(
                command=ls_command, case_type=DeviceTestType.cpp_test_lite,
                timeout=5, receiver=None)
            if xml_file in result:
                return True
            time.sleep(5)
            if (self.execute_bin + "_1.xml") in result:
                return False
        return False

    def read_nfs_xml(self, request, report_path, is_true=False):
        remote_nfs = get_nfs_server(request)
        if not remote_nfs:
            err_msg = "The name of remote device {} does not match". \
                format(self.remote)
            LOG.error(err_msg, error_no="00403")
            raise TypeError(err_msg)
        tests = []
        execute_bin_xml = (self.execute_bin + "_1.xml") if is_true else (
                self.execute_bin + ".xml")
        LOG.debug("run into :{}".format(is_true))
        file_path = os.path.join(report_path, execute_bin_xml)
        if not self.check_xml_exist(execute_bin_xml):
            return tests

        from xml.etree import ElementTree
        try:
            if remote_nfs["remote"] == "true":
                import paramiko
                client = paramiko.SSHClient()
                client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
                client.connect(hostname=remote_nfs.get("ip"),
                               port=int(remote_nfs.get("port")),
                               username=remote_nfs.get("username"),
                               password=remote_nfs.get("password"))
                sftp_client = client.open_sftp()
                remote_file = sftp_client.open(file_path)
                try:
                    result = remote_file.read().decode()
                    suites_element = ElementTree.fromstring(result)
                    for suite_element in suites_element:
                        suite_name = suite_element.get("name", "")
                        for case in suite_element:
                            case_name = case.get("name")
                            test = TestDescription(suite_name, case_name)
                            if test not in tests:
                                tests.append(test)
                finally:
                    remote_file.close()
                client.close()
            else:
                if os.path.isdir(report_path):
                    flags = os.O_RDONLY
                    modes = stat.S_IWUSR | stat.S_IRUSR
                    with os.fdopen(os.open(file_path, flags, modes),
                                   "r") as test_file:
                        result = test_file.read()
                        suites_element = ElementTree.fromstring(result)
                        for suite_element in suites_element:
                            suite_name = suite_element.get("name", "")
                            for case in suite_element:
                                case_name = case.get("name")
                                test = TestDescription(suite_name, case_name)
                                if test not in tests:
                                    tests.append(test)
        except (FileNotFoundError, IOError) as error:
            LOG.error("download xml failed %s" % error, error_no="00403")
        except SyntaxError as error:
            LOG.error("parse xml failed %s" % error, error_no="00404")
        return tests

    def delete_device_xml(self, request, report_path):
        remote_nfs = get_nfs_server(request)
        if not remote_nfs:
            err_msg = "The name of remote device {} does not match". \
                format(self.remote)
            LOG.error(err_msg, error_no="00403")
            raise TypeError(err_msg)
        LOG.info("delete xml directory {} from remote server: {}"
                 "".format
                 (report_path, remote_nfs.get("ip")))
        if remote_nfs["remote"] == "true":
            import paramiko
            client = paramiko.Transport((remote_nfs.get("ip"),
                                         int(remote_nfs.get("port"))))
            client.connect(username=remote_nfs.get("username"),
                           password=remote_nfs.get("password"))
            sftp = paramiko.SFTPClient.from_transport(client)
            try:
                sftp.stat(report_path)
                files = sftp.listdir(report_path)
                for report_xml in files:
                    if report_xml.endswith(".xml"):
                        filepath = "{}{}".format(report_path, report_xml)
                        try:
                            sftp.remove(filepath)
                            time.sleep(0.5)
                        except IOError as _:
                            pass
            except FileNotFoundError as _:
                pass
            client.close()
        else:
            for report_xml in glob.glob(os.path.join(report_path, '*.xml')):
                try:
                    os.remove(report_xml)
                except Exception as exception:
                    LOG.error(
                        "remove {} Failed:{}".format(report_xml, exception))
                    pass

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.ctest_lite)
class CTestDriver(IDriver):
    """
    CTest is a test that runs a native test package on given lite device.
    """
    config = None
    result = ""
    error_message = ""
    version_cmd = "AT+CSV"

    def __init__(self):
        self.file_name = ""

    def __check_environment__(self, device_options):
        if len(device_options) != 1 or \
                device_options[0].label != DeviceLabelType.wifiiot:
            self.error_message = "check environment failed"
            return False
        return True

    def __check_config__(self, config=None):
        del config
        self.config = None

    def __execute__(self, request):
        from xdevice import Variables
        try:
            self.config = request.config
            self.config.device = request.config.environment.devices[0]
            current_dir = request.config.resource_path if \
                request.config.resource_path else Variables.exec_dir
            if request.root.source.source_file.strip():
                source = os.path.join(current_dir,
                                      request.root.source.source_file.strip())
                self.file_name = os.path.basename(
                    request.root.source.source_file.strip()).split(".")[0]
            else:
                source = request.root.source.source_string.strip()
            json_config = JsonParser(source)
            kit_instances = get_kit_instances(json_config,
                                              request.config.resource_path,
                                              request.config.testcases_path)
            for (kit_instance, kit_info) in zip(kit_instances,
                                                json_config.get_kits()):
                if isinstance(kit_instance, DeployToolKit):
                    LOG.debug("run ctest third party")
                    self._run_ctest_third_party(source=source, request=request,
                                                time_out=int(
                                                    kit_instance.time_out))
                    break
                else:
                    LOG.debug("run ctest")
                    self._run_ctest(source=source, request=request)
                    break

        except (LiteDeviceExecuteCommandError, Exception) as exception:
            LOG.error(exception, error_no=getattr(exception, "error_no",
                                                  "00000"))
            self.error_message = exception
        finally:
            report_name = "report" if request.root.source. \
                test_name.startswith("{") else get_filename_extension(
                request.root.source.test_name)[0]
            self.result = check_result_report(
                request.config.report_path, self.result, self.error_message,
                report_name)

    def _run_ctest(self, source=None, request=None):
        parser_instances = []
        parsers = get_plugin(Plugin.PARSER, ParserType.ctest_lite)
        try:
            if not source:
                LOG.error("Error: source don't exist %s." % source,
                          error_no="00101")
                return

            version = get_test_component_version(self.config)

            for parser in parsers:
                parser_instance = parser.__class__()
                parser_instance.suites_name = self.file_name
                parser_instance.product_info.setdefault("Version", version)
                parser_instance.listeners = request.listeners
                parser_instances.append(parser_instance)
            handler = ShellHandler(parser_instances)

            reset_cmd = self._reset_device(request, source)
            self.result = "%s.xml" % os.path.join(
                request.config.report_path, "result", self.file_name)
            self.config.device.device.com_dict.get(
                ComType.deploy_com).connect()
            result, _, error = self.config.device.device. \
                execute_command_with_timeout(
                    command=reset_cmd, case_type=DeviceTestType.ctest_lite,
                    key=ComType.deploy_com, timeout=90, receiver=handler)
            device_log_file = get_device_log_file(request.config.report_path,
                                                  request.config.device.
                                                  __get_serial__())
            device_log_file_open = \
                os.open(device_log_file, os.O_WRONLY | os.O_CREAT |
                        os.O_APPEND, FilePermission.mode_755)
            with os.fdopen(device_log_file_open, "a") as file_name:
                file_name.write("{}{}".format(
                    "\n".join(result.split("\n")[0:-1]), "\n"))
                file_name.flush()
        finally:
            self.config.device.device.com_dict.get(
                ComType.deploy_com).close()

    def _run_ctest_third_party(self, source=None, request=None, time_out=5):
        parser_instances = []
        parsers = get_plugin(Plugin.PARSER, ParserType.ctest_lite)
        try:
            if not source:
                LOG.error("Error: source don't exist %s." % source,
                          error_no="00101")
                return

            version = get_test_component_version(self.config)

            for parser in parsers:
                parser_instance = parser.__class__()
                parser_instance.suites_name = self.file_name
                parser_instance.product_info.setdefault("Version", version)
                parser_instance.listeners = request.listeners
                parser_instances.append(parser_instance)
            handler = ShellHandler(parser_instances)

            while True:
                input_burning = input("Please enter 'y' or 'n' after "
                                      "the burning is complete,"
                                      "enter 'quit' to exit:")
                if input_burning.lower().strip() in ["y", "yes"]:
                    LOG.info("Burning succeeded.")
                    break
                elif input_burning.lower().strip() in ["n", "no"]:
                    LOG.info("Burning failed.")
                elif input_burning.lower().strip() == "quit":
                    break
                else:
                    LOG.info("The input {} parameter is incorrect,"
                             "please enter 'y' or 'n' after the "
                             "burning is complete ,enter 'quit' "
                             "to exit.".format(input_burning))
            LOG.info("Please press the device "
                     "reset button when the send commmand [] appears ")
            time.sleep(3)
            self.result = "%s.xml" % os.path.join(
                request.config.report_path, "result", self.file_name)
            self.config.device.device.com_dict.get(
                ComType.deploy_com).connect()
            result, _, error = self.config.device.device. \
                execute_command_with_timeout(
                command=[], case_type=DeviceTestType.ctest_lite,
                key=ComType.deploy_com, timeout=time_out, receiver=handler)
            device_log_file = get_device_log_file(request.config.report_path,
                                                  request.config.device.
                                                  __get_serial__())
            device_log_file_open = \
                os.open(device_log_file, os.O_WRONLY | os.O_CREAT |
                        os.O_APPEND, 0o755)
            with os.fdopen(device_log_file_open, "a") as file_name:
                file_name.write("{}{}".format(
                    "\n".join(result.split("\n")[0:-1]), "\n"))
                file_name.flush()
        finally:
            self.config.device.device.com_dict.get(
                ComType.deploy_com).close()

    def _reset_device(self, request, source):
        json_config = JsonParser(source)
        reset_cmd = []
        kit_instances = get_kit_instances(json_config,
                                          request.config.resource_path,
                                          request.config.testcases_path)
        from xdevice import Scheduler
        for (kit_instance, kit_info) in zip(kit_instances,
                                            json_config.get_kits()):
            if not isinstance(kit_instance, DeployKit):
                continue
            if not self.file_name:
                self.file_name = get_config_value(
                    'burn_file', kit_info)[0].split("\\")[-1].split(".")[0]
            reset_cmd = kit_instance.burn_command
            if not Scheduler.is_execute:
                raise ExecuteTerminate("ExecuteTerminate",
                                       error_no="00300")
            kit_instance.__setup__(
                self.config.device)
        reset_cmd = [int(item, 16) for item in reset_cmd]
        return reset_cmd

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.open_source_test)
class OpenSourceTestDriver(IDriver):
    """
    OpenSourceTest is a test that runs a native test package on given
    device lite device.
    """
    config = None
    result = ""
    error_message = ""
    has_param = False

    def __init__(self):
        self.rerun = True
        self.file_name = ""
        self.handler = None

    def __check_environment__(self, device_options):
        if len(device_options) != 1 or \
                device_options[0].label != DeviceLabelType.ipcamera:
            self.error_message = "check environment failed"
            return False
        return True

    def __check_config__(self, config=None):
        pass

    def __execute__(self, request):
        kits = []
        try:
            self.config = request.config
            setattr(self.config, "command_result", "")
            self.config.device = request.config.environment.devices[0]
            init_remote_server(self, request)
            config_file = request.root.source.config_file
            json_config = JsonParser(config_file)
            pre_cmd = get_config_value('pre_cmd', json_config.get_driver(),
                                       False)
            execute_dir = get_config_value('execute', json_config.get_driver(),
                                           False)
            kits = get_kit_instances(json_config,
                                     request.config.resource_path,
                                     request.config.testcases_path)
            from xdevice import Scheduler
            for kit in kits:
                if not Scheduler.is_execute:
                    raise ExecuteTerminate("ExecuteTerminate",
                                           error_no="00300")
                copy_list = kit.__setup__(request.config.device,
                                          request=request)

            self.file_name = request.root.source.test_name
            self.set_file_name(request, request.root.source.test_name)
            self.config.device.execute_command_with_timeout(
                command=pre_cmd, timeout=1)
            self.config.device.execute_command_with_timeout(
                command="cd {}".format(execute_dir), timeout=1)
            device_log_file = get_device_log_file(
                request.config.report_path,
                request.config.device.__get_serial__())
            device_log_file_open = \
                os.open(device_log_file, os.O_WRONLY | os.O_CREAT |
                        os.O_APPEND, FilePermission.mode_755)
            with os.fdopen(device_log_file_open, "a") as file_name:
                for test_bin in copy_list:
                    if not test_bin.endswith(".run-test"):
                        continue
                    if test_bin.startswith("/"):
                        command = ".%s" % test_bin
                    else:
                        command = "./%s" % test_bin
                    self._do_test_run(command, request)
                    file_name.write(self.config.command_result)
                file_name.flush()

        except (LiteDeviceExecuteCommandError, Exception) as exception:
            LOG.error(exception, error_no=getattr(exception, "error_no",
                                                  "00000"))
            self.error_message = exception
        finally:
            LOG.info("-------------finally-----------------")
            # umount the dirs already mount
            for kit in kits:
                kit.__teardown__(request.config.device)
            self.config.device.close()
            report_name = "report" if request.root.source. \
                test_name.startswith("{") else get_filename_extension(
                    request.root.source.test_name)[0]
            self.result = check_result_report(
                request.config.report_path, self.result, self.error_message,
                report_name)

    def set_file_name(self, request, bin_file):
        self.result = "%s.xml" % os.path.join(
            request.config.report_path, "result", bin_file)

    def run(self, command=None, listener=None, timeout=20):
        parsers = get_plugin(Plugin.PARSER,
                             ParserType.open_source_test)
        parser_instances = []
        for parser in parsers:
            parser_instance = parser.__class__()
            parser_instance.suite_name = self.file_name
            parser_instance.test_name = command.replace("./", "")
            parser_instance.listeners = listener
            parser_instances.append(parser_instance)
        self.handler = ShellHandler(parser_instances)
        for _ in range(3):
            result, _, error = self.config.device.execute_command_with_timeout(
                command=command, case_type=DeviceTestType.open_source_test,
                timeout=timeout, receiver=self.handler)
            self.config.command_result = result
            if "pass" in result.lower():
                break
        return error, result, self.handler

    def _do_test_run(self, command, request):
        listeners = request.listeners
        for listener in listeners:
            listener.device_sn = self.config.device.device_sn
        error, _, _ = self.run(command, listeners, timeout=60)
        if error:
            LOG.error(
                "execute %s failed" % command, error_no="00402")

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.build_only_test)
class BuildOnlyTestDriver(IDriver):
    """
    BuildOnlyTest is a test that runs a native test package on given
    device lite device.
    """
    config = None
    result = ""
    error_message = ""

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        pass

    def __execute__(self, request):
        self.config = request.config
        self.config.device = request.config.environment.devices[0]
        self.file_name = request.root.source.test_name
        self.config_file = request.root.source.config_file
        self.testcases_path = request.config.testcases_path
        file_path = self._get_log_file()
        result_list = self._get_result_list(file_path)
        if len(result_list) == 0:
            LOG.error(
                "Error: source don't exist %s." % request.root.source.
                source_file, error_no="00101")
            return
        total_result = ''
        for result in result_list:
            flags = os.O_RDONLY
            modes = stat.S_IWUSR | stat.S_IRUSR
            with os.fdopen(os.open(result, flags, modes), "r",
                           encoding="utf-8") as file_content:
                result = file_content.read()
                if not result.endswith('\n'):
                    result = '%s\n' % result
            total_result = '{}{}'.format(total_result, result)
        parsers = get_plugin(Plugin.PARSER, ParserType.build_only_test)
        parser_instances = []
        for parser in parsers:
            parser_instance = parser.__class__()
            parser_instance.suite_name = self.file_name
            parser_instance.listeners = request.listeners
            parser_instances.append(parser_instance)
        handler = ShellHandler(parser_instances)
        generate_report(handler, total_result)

    @classmethod
    def _get_result_list(cls, file_path):
        result_list = list()
        for root_path, dirs_path, file_names in os.walk(file_path):
            for file_name in file_names:
                if file_name == "logfile":
                    result_list.append(os.path.join(root_path, file_name))
        return result_list

    def _get_log_file(self):
        json_config = JsonParser(self.config_file)
        log_path = get_config_value('log_path', json_config.get_driver(),
                                    False)
        log_path = str(log_path.replace("/", "", 1)) if log_path.startswith(
            "/") else str(log_path)
        LOG.debug("The log path is:%s" % log_path)
        file_path = get_file_absolute_path(log_path,
                                           paths=[self.testcases_path])
        LOG.debug("The file path is:%s" % file_path)
        return file_path

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.jsunit_test_lite)
class JSUnitTestLiteDriver(IDriver):
    """
    JSUnitTestDriver is a Test that runs a native test package on given device.
    """

    def __init__(self):
        self.result = ""
        self.error_message = ""
        self.kits = []
        self.config = None

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        pass

    def _get_driver_config(self, json_config):
        bundle_name = get_config_value('bundle-name',
                                       json_config.get_driver(), False)
        if not bundle_name:
            raise ParamError("Can't find bundle-name in config file.",
                             error_no="00108")
        else:
            self.config.bundle_name = bundle_name

        ability = get_config_value('ability',
                                   json_config.get_driver(), False)
        if not ability:
            self.config.ability = "default"
        else:
            self.config.ability = ability

    def __execute__(self, request):
        try:
            LOG.debug("Start execute xdevice extension JSUnit Test")

            self.config = request.config
            self.config.device = request.config.environment.devices[0]

            config_file = request.root.source.config_file
            suite_file = request.root.source.source_file

            if not suite_file:
                raise ParamError(
                    "test source '%s' not exists" %
                    request.root.source.source_string, error_no="00101")

            if not os.path.exists(config_file):
                LOG.error("Error: Test cases don't exist %s." % config_file,
                          error_no="00101")
                raise ParamError(
                    "Error: Test cases don't exist %s." % config_file,
                    error_no="00101")

            self.file_name = os.path.basename(
                request.root.source.source_file.strip()).split(".")[0]

            self.result = "%s.xml" % os.path.join(
                request.config.report_path, "result", self.file_name)

            json_config = JsonParser(config_file)
            self.kits = get_kit_instances(json_config,
                                          self.config.resource_path,
                                          self.config.testcases_path)

            self._get_driver_config(json_config)
            from xdevice import Scheduler
            for kit in self.kits:
                if not Scheduler.is_execute:
                    raise ExecuteTerminate("ExecuteTerminate",
                                           error_no="00300")
                if kit.__class__.__name__ == CKit.liteinstall:
                    kit.bundle_name = self.config.bundle_name
                kit.__setup__(self.config.device, request=request)

            self._run_jsunit(request)

        except Exception as exception:
            self.error_message = exception
        finally:
            report_name = "report" if request.root.source. \
                test_name.startswith("{") else get_filename_extension(
                request.root.source.test_name)[0]

            self.result = check_result_report(
                request.config.report_path, self.result, self.error_message,
                report_name)

            for kit in self.kits:
                kit.__teardown__(self.config.device)
            self.config.device.close()

    def _run_jsunit(self, request):
        parser_instances = []
        parsers = get_plugin(Plugin.PARSER, ParserType.jsuit_test_lite)
        for parser in parsers:
            parser_instance = parser.__class__()
            parser_instance.suites_name = self.file_name
            parser_instance.listeners = request.listeners
            parser_instances.append(parser_instance)
        handler = ShellHandler(parser_instances)

        command = "./bin/aa start -p %s -n %s" % \
                  (self.config.bundle_name, self.config.ability)
        result, _, error = self.config.device.execute_command_with_timeout(
            command=command, timeout=300, receiver=handler)
        device_log_file = get_device_log_file(request.config.report_path,
                                              request.config.device.
                                              __get_serial__())
        device_log_file_open =\
            os.open(device_log_file, os.O_WRONLY | os.O_CREAT | os.O_APPEND,
                    0o755)
        with os.fdopen(device_log_file_open, "a") as file_name:
            file_name.write("{}{}".format(
                "\n".join(result.split("\n")[0:-1]), "\n"))
            file_name.flush()

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""
