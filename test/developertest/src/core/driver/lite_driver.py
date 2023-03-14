#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020 Huawei Device Co., Ltd.
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
import time
from dataclasses import dataclass

from xdevice import DeviceTestType
from xdevice import IDriver
from xdevice import Plugin
from xdevice import platform_logger
from xdevice import DeviceLabelType
from xdevice import ComType
from xdevice import ParserType
from xdevice import ShellHandler
from xdevice import ExecuteTerminate
from xdevice import LiteDeviceExecuteCommandError
from xdevice import get_plugin
from xdevice import JsonParser
from xdevice import get_config_value
from xdevice import get_kit_instances
from xdevice import check_result_report
from xdevice import get_device_log_file
from xdevice import get_test_component_version
from xdevice import ParamError
from core.utils import get_filename_extension
from core.testkit.kit_lite import DeployKit
from core.config.resource_manager import ResourceManager
from core.config.config_manager import UserConfigManager

__all__ = ["LiteUnitTest", "CTestDriver", "JSUnitTestLiteDriver"]
LOG = platform_logger("LiteUnitTest")


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


@dataclass
class GTestConst(object):
    exec_para_filter = "--gtest_filter"
    exec_para_level = "--gtest_testsize"


@Plugin(type=Plugin.DRIVER, id=DeviceTestType.lite_cpp_test)
class LiteUnitTest(IDriver):
    """
    lite gtest test driver for L1
    """
    config = None
    log = platform_logger("LiteUnitTest")
    nfs_dir = ""
    mnt_cmd = ""
    lite_device = None
    result = None

    def __check_failed__(self, msg):
        self.log.error("check failed {}".format(msg))
        return None

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config):
        """
        1. check serial protocol
        2. login device
        3. NFS is available
        :param config: serial device
        :return:
        """
        self.log.error("Lite driver check config:{}".format(config))

    def __execute__(self, request):
        """

        1. select test case by subsystem, module, suite
        2. open test dir
        3、execute single test case, eg. ./test_demo
        :param request: contains test condition, sub_system
            module_name，test_suit，
        test_case，test_level，test_case_dir
        :return:
        """
        self.log.debug("Test suite FilePath: %s" %
                      request.root.source.source_file)
        self.lite_device = request.config.environment.devices[0]
        self.lite_device.connect()
        if not self._before_execute_test():
            self.log.error("open test dir failed")
            return
        self.log.debug("open test dir success")
        if self._execute_test(request) == "":
            self.log.error("execute test command failed")
            return
        self.log.info("execute test command success")
        if not self._after_execute_test(request):
            self.log.error("after execute test failed")
            return
        self.log.info("lite device execute request success")

    def _mount_nfs_server(self):
        #before execute each suits bin, mount nfs
        self.mnt_cmd = "mount {}".format(UserConfigManager().get_user_config(
            "NFS").get("mnt_cmd"))
        if self.mnt_cmd == "mount ":
            self.log.error("no configure for mount command")
            return

        filter_result, status, _ = \
            self.lite_device.execute_command_with_timeout(
            self.mnt_cmd, case_type=DeviceTestType.lite_cpp_test, timeout=3)
        if "already mounted" in filter_result:
            self.log.info("nfs has been mounted")
            return

        for i in range(0, 2):
            if status:
                self.log.info("execute mount command success")
                return
            self.log.info("try mount %d" % (i + 2))
            _, status, _ = self.lite_device.execute_command_with_timeout(
                self.mnt_cmd, case_type=DeviceTestType.lite_cpp_test,
                timeout=3)

        self.log.error("execute mount command failed")

    def _before_execute_test(self):
        """
        need copy test case to nfs dir
        :param request: nfs dir, test case path
        :return:
        """
        self.nfs_dir = \
            UserConfigManager().get_user_config("NFS").get("host_dir")
        if self.nfs_dir == "":
            self.log.error("no configure for nfs directory")
            return False
        self._mount_nfs_server()
        _, status, _ = \
            self.lite_device.execute_command_with_timeout("cd /{}".format(
                UserConfigManager().get_user_config("NFS").get("board_dir")),
            case_type=DeviceTestType.lite_cpp_test)
        if not status:
            self.log.error("pre execute command failed")
            return False
        self.log.info("pre execute command success")
        return True

    def _execute_test(self, request):
        test_case = request.root.source.source_file
        self.config = request.config
        test_para = self._get_test_para(self.config.testcase,
                                       self.config.testlevel)
        case_name = os.path.basename(test_case)
        if os.path.exists(os.path.join(self.nfs_dir, case_name)):
            os.remove(os.path.join(self.nfs_dir, case_name))
        result_name = case_name + ".xml"
        result_file = os.path.join(self.nfs_dir, result_name)
        if os.path.exists(result_file):
            os.remove(result_file)
        shutil.copyfile(test_case, os.path.join(self.nfs_dir, case_name))
        # push resource files
        resource_manager = ResourceManager()
        resource_data_dic, resource_dir = \
            resource_manager.get_resource_data_dic(test_case)
        resource_manager.lite_process_preparer_data(resource_data_dic, resource_dir)
        self.lite_device.execute_command_with_timeout(
            "chmod 777 {}".format(case_name),
            case_type=DeviceTestType.lite_cpp_test)
        test_command = "./%s %s" % (case_name, test_para)
        case_result, status, _ = \
            self.lite_device.execute_command_with_timeout(
            test_command, case_type=DeviceTestType.lite_cpp_test)
        if status:
            self.log.info("test case result:\n %s" % case_result)
            return
        self.log.error("failed case: %s" % test_case)

    def _get_test_para(self, testcase, testlevel):
        if "" != testcase and "" == testlevel:
            test_para = "%s=%s" % (GTestConst.exec_para_filter, testcase)
        elif "" == testcase and "" != testlevel:
            level_para = get_level_para_string(testlevel)
            test_para = "%s=%s" % (GTestConst.exec_para_level, level_para)
        else:
            test_para = ""
        return test_para

    def _after_execute_test(self, request):
        """
        copy test result to result dir
        :param request:
        :return:
        """
        if request.config is None:
            self.log.error("test config is null")
            return False
        report_path = request.config.report_path
        test_result = os.path.join(report_path, "result")
        test_case = request.root.source.source_file
        case_name = os.path.basename(test_case)
        if not os.path.exists(test_result):
            os.mkdir(test_result)
        sub_system_module = test_case.split(
            "unittest" + os.sep)[1].split(os.sep + "bin")[0]
        if os.sep in sub_system_module:
            sub_system = sub_system_module.split(os.sep)[0]
            module_name = sub_system_module.split(os.sep)[1]
            subsystem_dir = os.path.join(test_result, sub_system)
            if not os.path.exists(subsystem_dir):
                os.mkdir(subsystem_dir)
            module_dir = os.path.join(subsystem_dir, module_name)
            if not os.path.exists(module_dir):
                os.mkdir(module_dir)
            test_result = module_dir
        else:
            if sub_system_module != "":
                test_result = os.path.join(test_result, sub_system_module)
                if not os.path.exists(test_result):
                    os.mkdir(test_result)
        result_name = case_name + ".xml"
        result_file = os.path.join(self.nfs_dir, result_name)
        if not self._check_xml_exist(result_name):
            self.log.error("result xml file %s not exist." % result_name)
        if not os.path.exists(result_file):
            self.log.error("file %s not exist." % result_file)
            self._clear_nfs_space()
            return False
        file_name = os.path.basename(result_file)
        final_result = os.path.join(test_result, file_name)
        shutil.copyfile(result_file,
                        final_result)
        self.log.info("after execute test")
        self._clear_nfs_space()
        self.lite_device.close()
        return True

    def _clear_nfs_space(self):
        _, status, _ = \
            self.lite_device.execute_command_with_timeout(
                "cd ..",
                case_type=DeviceTestType.lite_cpp_test)
        _, status, _ = \
            self.lite_device.execute_command_with_timeout(
                "umount %s" % UserConfigManager().get_user_config("NFS").get("board_dir"),
                case_type=DeviceTestType.lite_cpp_test)
        shutil.rmtree(self.nfs_dir)
        os.mkdir(self.nfs_dir)

    def _check_xml_exist(self, xml_file, timeout=10):
        ls_command = \
            "ls /%s" % \
            UserConfigManager().get_user_config("NFS").get("board_dir")
        start_time = time.time()
        while time.time()-start_time < timeout:
            result, _, _ = self.lite_device.execute_command_with_timeout(
                command=ls_command, case_type=DeviceTestType.cpp_test_lite,
                timeout=5, receiver=None)
            if xml_file in result:
                return True
            time.sleep(5)
        return False

    def show_help_info(self):
        """
        show help info.
        """
        self.log.info("this is test driver for cpp test")
        return None

    def show_driver_info(self):
        """
        show driver info.
        """
        self.log.info("this is test driver for cpp test")
        return None

    def __result__(self):
        pass


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

            if request.config.resource_path:
                current_dir = request.config.resource_path
            else:
                current_dir = Variables.exec_dir

            config_file = request.root.source.config_file.strip()
            if config_file:
                source = os.path.join(current_dir, config_file)
                self.file_name = os.path.basename(config_file).split(".")[0]
            else:
                source = request.root.source.source_string.strip()

            self._run_ctest(source=source, request=request)

        except (LiteDeviceExecuteCommandError, Exception) as exception:
            LOG.error(exception, error_no=getattr(exception, "error_no",
                                                  "00000"))
            self.error_message = exception
        finally:
            if request.root.source.test_name.startswith("{"):
                report_name = "report"
            else:
                report_name = get_filename_extension(
                    request.root.source.test_name)[0]

            self.result = check_result_report(request.config.report_path,
                                              self.result,
                                              self.error_message,
                                              report_name)

    def _run_ctest(self, source=None, request=None):
        if not source:
            LOG.error("Error: %s don't exist." % source, error_no="00101")
            return

        try:
            parsers = get_plugin(Plugin.PARSER, ParserType.ctest_lite)
            version = get_test_component_version(self.config)
            parser_instances = []
            for parser in parsers:
                parser_instance = parser.__class__()
                parser_instance.suites_name = self.file_name
                parser_instance.product_info.setdefault("Version", version)
                parser_instance.listeners = request.listeners
                parser_instances.append(parser_instance)
            handler = ShellHandler(parser_instances)

            reset_cmd = self._reset_device(request, source)
            self.result = "%s.xml" % os.path.join(request.config.report_path,
                                                  "result", self.file_name)

            self.config.device.device.com_dict.get(
                ComType.deploy_com).connect()

            result, _, error = self.config.device.device. \
                execute_command_with_timeout(
                    command=reset_cmd,
                    case_type=DeviceTestType.ctest_lite,
                    key=ComType.deploy_com,
                    timeout=90,
                    receiver=handler)

            device_log_file = get_device_log_file(request.config.report_path,
                                                  request.config.device.
                                                  __get_serial__())

            device_log_file_open = os.open(device_log_file, os.O_WRONLY |
                os.O_CREAT | os.O_APPEND, 0o755)
            with os.fdopen(device_log_file_open, "a") as file_name:
                file_name.write("{}{}".format(
                    "\n".join(result.split("\n")[0:-1]), "\n"))
                file_name.flush()
        finally:
            self.config.device.device.com_dict.get(ComType.deploy_com).close()

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
                raise ExecuteTerminate("ExecuteTerminate", error_no="00300")

            kit_instance.__setup__(self.config.device,
                source_file=request.root.source.source_file.strip())

        reset_cmd = [int(item, 16) for item in reset_cmd]
        return reset_cmd

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
            command=command,
            timeout=300,
            receiver=handler)

        device_log_file = get_device_log_file(request.config.report_path,
            request.config.device.
            __get_serial__())

        device_log_file_open = os.open(device_log_file, os.O_WRONLY |
            os.O_CREAT | os.O_APPEND, 0o755)
        with os.fdopen(device_log_file_open, "a") as file_name:
            file_name.write("{}{}".format(
                "\n".join(result.split("\n")[0:-1]), "\n"))
            file_name.flush()

    def __result__(self):
        return self.result if os.path.exists(self.result) else ""