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

import copy
import os
import sys

from _core.constants import HostDrivenTestType
from _core.constants import TestExecType
from _core.constants import ModeType
from _core.constants import DeviceLabelType
from _core.driver.drivers_lite import init_remote_server
from _core.exception import DeviceError
from _core.exception import LiteDeviceError
from _core.exception import ParamError
from _core.exception import ReportException
from _core.exception import ExecuteTerminate
from _core.interface import IDriver
from _core.logger import platform_logger
from _core.plugin import Plugin
from _core.testkit.json_parser import JsonParser
from _core.utils import get_config_value
from _core.utils import do_module_kit_setup
from _core.utils import do_module_kit_teardown
from _core.utils import get_filename_extension
from _core.utils import get_file_absolute_path
from _core.utils import get_kit_instances
from _core.utils import check_result_report
from _core.utils import check_mode
from _core.report.suite_reporter import SuiteReporter

LOG = platform_logger("DeviceTest")
PY_SUFFIX = ".py"
PYD_SUFFIX = ".pyd"


@Plugin(type=Plugin.DRIVER, id=HostDrivenTestType.device_test)
class DeviceTestDriver(IDriver):
    """
    DeviceTest is a Test that runs a host-driven test on given devices.
    """
    # test driver config
    config = None
    result = ""
    error_message = ""
    py_file = ""

    def __init__(self):
        self.linux_host = ""
        self.linux_directory = ""

    def __check_environment__(self, device_options):
        pass

    def __check_config__(self, config=None):
        pass

    def __init_nfs_server__(self, request=None):
        return init_remote_server(self, request)

    def __execute__(self, request):
        try:
            # set self.config
            self.config = request.config
            self.config.tmp_id = str(request.uuid)
            self.config.tmp_folder = os.path.join(self.config.report_path,
                                                  "temp")
            self.config.devices = request.get_devices()
            if request.get("exectype") == TestExecType.device_test and \
                    not self.config.devices:
                LOG.error("no device", error_no="00104")
                raise ParamError("Load Error[00104]", error_no="00104")

            # get source, json config and kits
            if request.get_config_file():
                source = request.get_config_file()
                LOG.debug("Test config file path: %s" % source)
            else:
                source = request.get_source_string()
                LOG.debug("Test String: %s" % source)

            if not source:
                LOG.error("no config file found for '%s'" %
                          request.get_source_file(), error_no="00102")
                raise ParamError("Load Error(00102)", error_no="00102")

            json_config = JsonParser(source)
            kits = get_kit_instances(json_config, request.config.resource_path,
                                     request.config.testcases_path)

            # create tmp folder
            test_name = request.get_module_name()
            tmp_sub_folder = self._create_tmp_folder(request)
            self.result = "%s.xml" % os.path.join(tmp_sub_folder, test_name)

            # set configs keys
            configs = self._set_configs(json_config, kits, request,
                                        tmp_sub_folder)

            # get test list
            test_list = self._get_test_list(json_config, request, source)
            if not test_list:
                raise ParamError("no test list to run")

            self._run_devicetest(configs, test_list)
        except (ReportException, ModuleNotFoundError, ExecuteTerminate,
                SyntaxError, ValueError, AttributeError, TypeError,
                KeyboardInterrupt, ParamError, DeviceError, LiteDeviceError) \
                as exception:
            error_no = getattr(exception, "error_no", "00000")
            LOG.exception(exception, exc_info=False, error_no=error_no)
            self.error_message = exception

        finally:
            self._handle_finally(request)

    def _get_test_list(self, json_config, request, source):
        test_list = get_config_value('py_file', json_config.get_driver(),
                                     is_list=True)
        if str(request.root.source.source_file).endswith(PYD_SUFFIX) or \
                str(request.root.source.source_file).endswith(PY_SUFFIX):
            test_list = [request.root.source.source_file]

        if not test_list and os.path.exists(source):
            test_list = _get_dict_test_list(os.path.dirname(source))

        # check test list
        testcase = request.get("testcase")
        testcase_list = []
        if testcase:
            testcase_list = str(testcase).split(";")

        checked_test_list = []
        for index, test in enumerate(test_list):
            if not os.path.exists(test):
                try:
                    absolute_file = get_file_absolute_path(test, [
                        self.config.resource_path, self.config.testcases_path])
                except ParamError as error:
                    LOG.error(error, error_no=error.error_no)
                    continue
            else:
                absolute_file = test

            file_name = get_filename_extension(absolute_file)[0]
            if not testcase_list or file_name in testcase_list:
                checked_test_list.append(absolute_file)
            else:
                LOG.info("test '%s' is ignored", absolute_file)
        if checked_test_list:
            LOG.info("test list: {}".format(checked_test_list))
        else:
            LOG.error("no test list found", error_no="00109")
            raise ParamError("Load Error(00109)", error_no="00109")
        return checked_test_list

    def _set_configs(self, json_config, kits, request, tmp_sub_folder):
        configs = dict()
        configs["testargs"] = self.config.testargs or {}
        configs["testcases_path"] = self.config.testcases_path or ""
        configs["request"] = request
        configs["test_name"] = request.get_module_name()
        configs["report_path"] = tmp_sub_folder
        configs["execute"] = get_config_value(
            'execute', json_config.get_driver(), False)

        for device in self.config.devices:
            do_module_kit_setup(request, kits)
            if device.label == DeviceLabelType.ipcamera:
                # add extra keys to configs for ipcamera device
                self.__init_nfs_server__(request=request)
                configs["linux_host"] = self.linux_host
                configs["linux_directory"] = self.linux_directory
                configs["kits"] = kits

        return configs

    def _handle_finally(self, request):
        from xdevice import Scheduler

        # do kit teardown
        do_module_kit_teardown(request)

        # close device connect
        for device in self.config.devices:
            if device.label == DeviceLabelType.ipcamera or device.label == \
                    DeviceLabelType.watch_gt:
                device.close()
            if device.label == DeviceLabelType.phone:
                device.close()

        # check result report
        report_name = request.root.source.test_name if \
            not request.root.source.test_name.startswith("{") \
            else "report"
        module_name = request.get_module_name()
        if Scheduler.mode != ModeType.decc:
            self.result = check_result_report(
                request.config.report_path, self.result, self.error_message,
                report_name, module_name)
        else:
            tmp_list = copy.copy(SuiteReporter.get_report_result())
            if self.result not in [report_path for report_path, _ in tmp_list]:
                if not self.error_message:
                    self.error_message = "Case not execute[01205]"
                self.result = check_result_report(
                    request.config.report_path, self.result,
                    self.error_message, report_name, module_name)

    def _create_tmp_folder(self, request):
        if request.root.source.source_file.strip():
            folder_name = "task_%s_%s" % (self.config.tmp_id,
                                          request.root.source.test_name)
        else:
            folder_name = "task_%s_report" % self.config.tmp_id

        tmp_sub_folder = os.path.join(self.config.tmp_folder, folder_name)
        os.makedirs(tmp_sub_folder, exist_ok=True)
        return tmp_sub_folder

    def _run_devicetest(self, configs, test_list):
        from xdevice import Variables

        # insert paths for loading _devicetest module and testcases
        devicetest_module = os.path.join(Variables.modules_dir, "_devicetest")
        if os.path.exists(devicetest_module):
            sys.path.insert(1, devicetest_module)
        if configs["testcases_path"]:
            sys.path.insert(1, configs["testcases_path"])

        # apply data to devicetest module about resource path
        request = configs.get('request', None)
        if request:
            sys.ecotest_resource_path = request.config.resource_path
        # run devicetest
        from _devicetest.devicetest.main import DeviceTest
        device_test = DeviceTest(test_list=test_list, configs=configs,
                                 devices=self.config.devices, log=LOG)
        device_test.run()

    def __result__(self):
        if check_mode(ModeType.decc):
            return self.result
        return self.result if os.path.exists(self.result) else ""


def _get_dict_test_list(module_path):
    test_list = []
    for root, _, files in os.walk(module_path):
        for _file in files:
            if _file.endswith(".py") or _file.endswith(".pyd"):
                test_list.append(os.path.join(root, _file))
    return test_list
