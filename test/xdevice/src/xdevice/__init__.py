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

import pkg_resources

from .variables import Variables
from _core.plugin import Plugin
from _core.plugin import get_plugin
from _core.logger import platform_logger
from _core.interface import IDriver
from _core.interface import IDevice
from _core.interface import IDeviceManager
from _core.interface import IParser
from _core.interface import LifeCycle
from _core.interface import IShellReceiver
from _core.interface import ITestKit
from _core.interface import IListener
from _core.interface import IReporter
from _core.exception import ParamError
from _core.exception import DeviceError
from _core.exception import LiteDeviceError
from _core.exception import ExecuteTerminate
from _core.exception import ReportException
from _core.exception import LiteDeviceExecuteCommandError
from _core.exception import LiteDeviceConnectError
from _core.constants import DeviceTestType
from _core.constants import DeviceLabelType
from _core.constants import ManagerType
from _core.constants import DeviceOsType
from _core.constants import ProductForm
from _core.constants import TestType
from _core.constants import CKit
from _core.constants import ConfigConst
from _core.constants import ComType
from _core.constants import ParserType
from _core.config.config_manager import UserConfigManager
from _core.config.resource_manager import ResourceManager
from _core.executor.listener import CaseResult
from _core.executor.listener import SuiteResult
from _core.executor.listener import SuitesResult
from _core.executor.listener import StateRecorder
from _core.executor.listener import TestDescription
from _core.testkit.json_parser import JsonParser
from _core.driver.parser_lite import ShellHandler
from _core.report.encrypt import check_pub_key_exist
from _core.utils import get_file_absolute_path
from _core.utils import check_result_report
from _core.utils import get_device_log_file
from _core.utils import get_kit_instances
from _core.utils import get_config_value
from _core.utils import exec_cmd
from _core.utils import check_device_name
from _core.utils import do_module_kit_setup
from _core.utils import do_module_kit_teardown
from _core.utils import get_test_component_version
from _core.environment.manager_env import DeviceSelectionOption
from _core.environment.manager_env import EnvironmentManager
from _core.executor.scheduler import Scheduler
from _core.report.suite_reporter import SuiteReporter
from _core.report.suite_reporter import ResultCode
from _core.report.reporter_helper import ExecInfo
from _core.report.result_reporter import ResultReporter
from _core.report.__main__ import main_report
from _core.command.console import Console
from _core.testkit.kit_lite import DeployKit
from _core.testkit.kit_lite import DeployToolKit
from _core.environment.manager_env import DeviceAllocationState

__all__ = [
    "Variables",
    "Console",
    "platform_logger",
    "Plugin",
    "get_plugin",
    "IDriver",
    "IDevice",
    "IDeviceManager",
    "IParser",
    "LifeCycle",
    "IShellReceiver",
    "ITestKit",
    "IListener",
    "IReporter",
    "ParamError",
    "DeviceError",
    "LiteDeviceError",
    "ExecuteTerminate",
    "ReportException",
    "LiteDeviceExecuteCommandError",
    "LiteDeviceConnectError",
    "DeviceTestType",
    "DeviceLabelType",
    "ManagerType",
    "DeviceOsType",
    "ProductForm",
    "TestType",
    "CKit",
    "ConfigConst",
    "ComType",
    "ParserType",
    "UserConfigManager",
    "ResourceManager",
    "CaseResult",
    "SuiteResult",
    "SuitesResult",
    "StateRecorder",
    "TestDescription",
    "Scheduler",
    "SuiteReporter",
    "DeviceSelectionOption",
    "EnvironmentManager",
    "JsonParser",
    "ShellHandler",
    "ResultCode",
    "check_pub_key_exist",
    "check_result_report",
    "get_file_absolute_path",
    "get_device_log_file",
    "get_kit_instances",
    "get_config_value",
    "exec_cmd",
    "check_device_name",
    "do_module_kit_setup",
    "do_module_kit_teardown",
    "get_test_component_version",
    "ExecInfo",
    "ResultReporter",
    "main_report",
    "DeployKit",
    "DeployToolKit",
    "DeviceAllocationState"
]


def _load_external_plugins():
    plugins = [Plugin.SCHEDULER, Plugin.DRIVER, Plugin.DEVICE, Plugin.LOG,
               Plugin.PARSER, Plugin.LISTENER, Plugin.TEST_KIT, Plugin.MANAGER,
               Plugin.REPORTER]
    for plugin_group in plugins:
        for entry_point in pkg_resources.iter_entry_points(group=plugin_group):
            entry_point.load()
    return


_load_external_plugins()
del _load_external_plugins
