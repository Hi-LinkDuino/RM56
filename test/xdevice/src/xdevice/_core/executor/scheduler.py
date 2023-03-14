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
import datetime
import os
import queue
import time
import uuid
from xml.etree import ElementTree

from _core.utils import unique_id
from _core.utils import check_mode
from _core.utils import get_sub_path
from _core.utils import get_filename_extension
from _core.utils import convert_serial
from _core.utils import get_instance_name
from _core.utils import is_config_str
from _core.utils import check_result_report
from _core.environment.manager_env import EnvironmentManager
from _core.environment.manager_env import DeviceSelectionOption
from _core.exception import ParamError
from _core.exception import ExecuteTerminate
from _core.exception import LiteDeviceError
from _core.exception import DeviceError
from _core.interface import LifeCycle
from _core.executor.request import Request
from _core.executor.request import Task
from _core.executor.request import Descriptor
from _core.plugin import get_plugin
from _core.plugin import Plugin
from _core.plugin import Config
from _core.report.reporter_helper import ExecInfo
from _core.report.reporter_helper import ReportConstant
from _core.report.reporter_helper import Case
from _core.report.reporter_helper import DataHelper
from _core.constants import TestExecType
from _core.constants import CKit
from _core.constants import ModeType
from _core.constants import DeviceLabelType
from _core.constants import SchedulerType
from _core.constants import ListenerType
from _core.constants import ConfigConst
from _core.executor.concurrent import DriversThread
from _core.executor.concurrent import QueueMonitorThread
from _core.executor.source import TestSetSource
from _core.executor.source import find_test_descriptors
from _core.executor.source import find_testdict_descriptors
from _core.executor.source import TestDictSource
from _core.logger import platform_logger
from _core.logger import add_task_file_handler
from _core.logger import remove_task_file_handler
from _core.logger import add_encrypt_file_handler
from _core.logger import remove_encrypt_file_handler

__all__ = ["Scheduler"]
LOG = platform_logger("Scheduler")


@Plugin(type=Plugin.SCHEDULER, id=SchedulerType.scheduler)
class Scheduler(object):
    """
    The Scheduler is the main entry point for client code that wishes to
    discover and execute tests.
    """
    # factory params
    is_execute = True
    terminate_result = queue.Queue()
    upload_address = ""
    task_type = ""
    task_name = ""
    mode = ""
    proxy = None

    # command_queue to store test commands
    command_queue = []
    max_command_num = 50
    # the number of tests in current task
    test_number = 0

    def __discover__(self, args):
        """discover task to execute"""
        config = Config()
        config.update(args)
        task = Task(drivers=[])
        task.init(config)

        TestDictSource.reset()
        root_descriptor = self._find_test_root_descriptor(task.config)
        task.set_root_descriptor(root_descriptor)
        return task

    def __execute__(self, task):
        error_message = ""
        try:
            Scheduler.is_execute = True
            if Scheduler.command_queue:
                LOG.debug("Run command: %s" % Scheduler.command_queue[-1])
                run_command = Scheduler.command_queue.pop()
                task_id = str(uuid.uuid1()).split("-")[0]
                Scheduler.command_queue.append((task_id, run_command,
                                                task.config.report_path))
                if len(Scheduler.command_queue) > self.max_command_num:
                    Scheduler.command_queue.pop(0)

            if getattr(task.config, ConfigConst.test_environment, ""):
                self._reset_environment(task.config.get(
                    ConfigConst.test_environment, ""))
            elif getattr(task.config, ConfigConst.configfile, ""):
                self._reset_environment(config_file=task.config.get(
                    ConfigConst.configfile, ""))

            # do with the count of repeat about a task
            if getattr(task.config, ConfigConst.repeat, 0) > 0:
                drivers_list = list()
                for repeat_index in range(task.config.repeat):
                    for driver_index in range(len(task.test_drivers)):
                        drivers_list.append(
                            copy.deepcopy(task.test_drivers[driver_index]))
                task.test_drivers = drivers_list

            self.test_number = len(task.test_drivers)

            if task.config.exectype == TestExecType.device_test:
                self._device_test_execute(task)
            elif task.config.exectype == TestExecType.host_test:
                self._host_test_execute(task)
            else:
                LOG.info("exec type %s is bypassed" % task.config.exectype)

        except (ParamError, ValueError, TypeError, SyntaxError, AttributeError,
                DeviceError, LiteDeviceError, ExecuteTerminate) as exception:
            error_no = getattr(exception, "error_no", "")
            error_message = "%s[%s]" % (str(exception), error_no) \
                if error_no else str(exception)
            error_no = error_no if error_no else "00000"
            LOG.exception(exception, exc_info=True, error_no=error_no)

        finally:
            Scheduler._clear_test_dict_source()
            if getattr(task.config, ConfigConst.test_environment, "") or\
                    getattr(task.config, ConfigConst.configfile, ""):
                self._restore_environment()

            if Scheduler.upload_address:
                Scheduler.upload_task_result(task, error_message)
                Scheduler.upload_report_end()

    def _device_test_execute(self, task):
        used_devices = {}
        try:
            self._dynamic_concurrent_execute(task, used_devices)
        finally:
            # generate reports
            self._generate_task_report(task, used_devices)

    def _host_test_execute(self, task):
        """execute host test"""
        try:
            # initial params
            current_driver_threads = {}
            test_drivers = task.test_drivers
            message_queue = queue.Queue()

            # execute test drivers
            queue_monitor_thread = self._start_queue_monitor(
                message_queue, test_drivers, current_driver_threads)
            while test_drivers:
                if len(current_driver_threads) > 5:
                    time.sleep(3)
                    continue

                # clear remaining test drivers when scheduler is terminated
                if not Scheduler.is_execute:
                    LOG.info("clear test drivers")
                    self._clear_not_executed(task, test_drivers)
                    break

                # get test driver and device
                test_driver = test_drivers[0]

                # display executing progress
                self._display_executing_process(None, test_driver,
                                                test_drivers)

                # start driver thread
                self._start_driver_thread(current_driver_threads, (
                    None, message_queue, task, test_driver))
                test_drivers.pop(0)

            # wait for all drivers threads finished and do kit teardown
            while True:
                if not queue_monitor_thread.is_alive():
                    break
                time.sleep(3)

        finally:
            # generate reports
            self._generate_task_report(task)

    def _generate_task_report(self, task, used_devices=None):
        task_info = ExecInfo()
        test_type = getattr(task.config, "testtype", [])
        task_name = getattr(task.config, "task", "")
        if task_name:
            task_info.test_type = str(task_name).upper()
        else:
            task_info.test_type = ",".join(test_type) if test_type else "Test"
        if used_devices:
            serials = []
            platforms = []
            for serial, device in used_devices.items():
                serials.append(convert_serial(serial))
                platform = str(device.label).capitalize()
                if platform not in platforms:
                    platforms.append(platform)
            task_info.device_name = ",".join(serials)
            task_info.platform = ",".join(platforms)
        else:
            task_info.device_name = "None"
            task_info.platform = "None"
        task_info.test_time = task.config.start_time
        task_info.product_info = getattr(task, "product_info", "")

        listeners = self._create_listeners(task)
        for listener in listeners:
            listener.__ended__(LifeCycle.TestTask, task_info,
                               test_type=task_info.test_type)

    @classmethod
    def _create_listeners(cls, task):
        listeners = []
        # append log listeners
        log_listeners = get_plugin(Plugin.LISTENER, ListenerType.log)
        for log_listener in log_listeners:
            log_listener_instance = log_listener.__class__()
            listeners.append(log_listener_instance)
        # append report listeners
        report_listeners = get_plugin(Plugin.LISTENER, ListenerType.report)
        for report_listener in report_listeners:
            report_listener_instance = report_listener.__class__()
            setattr(report_listener_instance, "report_path",
                    task.config.report_path)
            listeners.append(report_listener_instance)
        # append upload listeners
        upload_listeners = get_plugin(Plugin.LISTENER, ListenerType.upload)
        for upload_listener in upload_listeners:
            upload_listener_instance = upload_listener.__class__()
            listeners.append(upload_listener_instance)
        return listeners

    @staticmethod
    def _find_device_options(environment_config, options, test_source):
        devices_option = []
        for device_dict in environment_config:
            label = device_dict.get("label", "")
            if not label:
                continue
            device_option = DeviceSelectionOption(options, label, test_source)
            device_dict.pop("type", None)
            device_dict.pop("label", None)
            device_option.extend_value = device_dict
            device_option.source_file = test_source.config_file or \
                                        test_source.source_string
            devices_option.append(device_option)
        return devices_option

    def __allocate_environment__(self, options, test_driver):
        device_options = self.get_device_options(options,
                                                 test_driver[1].source)
        environment = None
        env_manager = EnvironmentManager()
        while True:
            if not Scheduler.is_execute:
                break
            environment = env_manager.apply_environment(device_options)
            if len(environment.devices) == len(device_options):
                return environment
            else:
                env_manager.release_environment(environment)
                LOG.debug("'%s' is waiting available device",
                          test_driver[1].source.test_name)
                if env_manager.check_device_exist(device_options):
                    continue
                else:
                    raise DeviceError("The '%s' required device does not exist"
                                      % test_driver[1].source.source_file,
                                      error_no="00104")

        return environment

    @classmethod
    def get_device_options(cls, options, test_source):
        device_options = []
        config_file = test_source.config_file

        from _core.testkit.json_parser import JsonParser
        if test_source.source_string and is_config_str(
                test_source.source_string):
            json_config = JsonParser(test_source.source_string)
            device_options = cls._find_device_options(
                json_config.get_environment(), options, test_source)
        elif config_file and os.path.exists(config_file):
            json_config = JsonParser(test_source.config_file)
            device_options = cls._find_device_options(
                json_config.get_environment(), options, test_source)

        if not device_options:
            if str(test_source.source_file).endswith(".bin"):
                device_option = DeviceSelectionOption(
                    options, DeviceLabelType.ipcamera, test_source)
            else:
                device_option = DeviceSelectionOption(
                    options, None, test_source)
            device_option.source_file = test_source.source_file or \
                                        test_source.source_string
            device_options.append(device_option)
        return device_options

    @staticmethod
    def __free_environment__(environment):
        env_manager = EnvironmentManager()
        env_manager.release_environment(environment)

    def _check_device_spt(self, kit, driver_request, device):
        kit_version = self._parse_version_id(driver_request, kit)
        kit_spt = kit.properties.get(ConfigConst.spt, None)
        if not kit_spt or not kit_version:
            setattr(device, ConfigConst.task_state, False)
            LOG.error("spt or version is empty", error_no="00108")
            return
        if getattr(driver_request, ConfigConst.product_info, ""):
            product_info = getattr(driver_request,
                                   ConfigConst.product_info)
            if not isinstance(product_info, dict):
                LOG.warning("product info should be dict, %s",
                            product_info)
                setattr(device, ConfigConst.task_state, False)
                return
            device_spt = product_info.get("Security Patch", None)
            device_version = product_info.get("VersionID", None)
            if not device_version or not device_version == kit_version:
                LOG.error("The device %s VersionID is %s, "
                          "and the test case version is %s, "
                          "which does not meet the requirements" %
                          (device.device_sn, device_version, kit_version),
                          error_no="00116")
                setattr(device, ConfigConst.task_state, False)
                return

            if not device_spt or not \
                    Scheduler.compare_spt_time(kit_spt, device_spt):
                LOG.error("The device %s spt is %s, "
                          "and the test case spt is %s, "
                          "which does not meet the requirements" %
                          (device.device_sn, device_spt, kit_spt),
                          error_no="00116")

                setattr(device, ConfigConst.task_state, False)
                return

    def _decc_task_setup(self, environment, task):
        config = Config()
        config.update(task.config.__dict__)
        config.environment = environment
        driver_request = Request(config=config)

        if environment is None:
            return False

        for device in environment.devices:
            if not getattr(device, ConfigConst.need_kit_setup, True):
                LOG.debug("device %s need kit setup is false" % device)
                continue

            # do task setup for device
            kits_copy = copy.deepcopy(task.config.kits)
            setattr(device, ConfigConst.task_kits, kits_copy)
            for kit in getattr(device, ConfigConst.task_kits, []):
                if not Scheduler.is_execute:
                    break
                try:
                    kit.__setup__(device, request=driver_request)
                except (ParamError, ExecuteTerminate, DeviceError,
                        LiteDeviceError, ValueError, TypeError,
                        SyntaxError, AttributeError) as exception:
                    error_no = getattr(exception, "error_no", "00000")
                    LOG.exception(
                        "task setup device: %s, exception: %s" % (
                            environment.__get_serial__(),
                            exception), exc_info=False, error_no=error_no)
                if kit.__class__.__name__ == CKit.query:
                    self._check_device_spt(kit, driver_request, device)
            LOG.debug("set device %s need kit setup to false" % device)
            setattr(device, ConfigConst.need_kit_setup, False)

        for device in environment.devices:
            if not getattr(device, ConfigConst.task_state, True):
                return False

        # set product_info to self.task
        if getattr(driver_request, ConfigConst.product_info, "") and \
                not getattr(task, ConfigConst.product_info, ""):
            product_info = getattr(driver_request, ConfigConst.product_info)
            if not isinstance(product_info, dict):
                LOG.warning("product info should be dict, %s",
                            product_info)
            else:
                setattr(task, ConfigConst.product_info, product_info)
        return True

    def _dynamic_concurrent_execute(self, task, used_devices):
        # initial params
        current_driver_threads = {}
        test_drivers = task.test_drivers
        message_queue = queue.Queue()
        task_unused_env = []

        # execute test drivers
        queue_monitor_thread = self._start_queue_monitor(
            message_queue, test_drivers, current_driver_threads)
        while test_drivers:
            # clear remaining test drivers when scheduler is terminated
            if not Scheduler.is_execute:
                LOG.info("clear test drivers")
                self._clear_not_executed(task, test_drivers)
                break

            # get test driver and device
            test_driver = test_drivers[0]

            if getattr(task.config, ConfigConst.history_report_path, ""):
                module_name = test_driver[1].source.module_name
                if not self.is_module_need_retry(task, module_name):
                    self._display_executing_process(None, test_driver,
                                                    test_drivers)
                    LOG.info("%s are passed, no need to retry" % module_name)
                    self._append_history_result(task, module_name)
                    LOG.info("")
                    test_drivers.pop(0)
                    continue
            # get environment
            try:
                environment = self.__allocate_environment__(
                    task.config.__dict__, test_driver)
            except DeviceError as exception:
                self._handle_device_error(exception, task, test_drivers)
                continue

            if not Scheduler.is_execute:
                if environment:
                    Scheduler.__free_environment__(environment)
                continue

            if check_mode(ModeType.decc) or getattr(
                    task.config, ConfigConst.check_device, False):
                LOG.info("start to check environment: %s" %
                         environment.__get_serial__())
                status = self._decc_task_setup(environment, task)
                if not status:
                    Scheduler.__free_environment__(environment)
                    task_unused_env.append(environment)
                    error_message = "Load Error[00116]"
                    self.report_not_executed(task.config.report_path,
                                             [test_drivers[0]],
                                             error_message, task)
                    test_drivers.pop(0)
                    continue
                else:
                    LOG.info("environment %s check success",
                             environment.__get_serial__())

            # display executing progress
            self._display_executing_process(environment, test_driver,
                                            test_drivers)

            # add to used devices and set need_kit_setup attribute
            self._append_used_devices(environment, used_devices)

            # start driver thread
            self._start_driver_thread(current_driver_threads, (
                environment, message_queue, task, test_driver))
            test_drivers.pop(0)

        # wait for all drivers threads finished and do kit teardown
        while True:
            if not queue_monitor_thread.is_alive():
                break
            time.sleep(3)

        self._do_taskkit_teardown(used_devices, task_unused_env)

    @classmethod
    def _append_history_result(cls, task, module_name):
        history_report_path = getattr(
            task.config, ConfigConst.history_report_path, "")
        from _core.report.result_reporter import ResultReporter
        params = ResultReporter.get_task_info_params(
            history_report_path)

        if not params or not params[4]:
            LOG.debug("task info record data reports is empty")
            return

        report_data_dict = dict(params[4])
        if module_name not in report_data_dict.keys():
            module_name_ = str(module_name).split(".")[0]
            if module_name_ not in report_data_dict.keys():
                LOG.error("%s not in data reports" % module_name)
                return
            module_name = module_name_

        from xdevice import SuiteReporter
        if check_mode(ModeType.decc):
            virtual_report_path, report_result = SuiteReporter. \
                get_history_result_by_module(module_name)
            LOG.debug("append history result: (%s, %s)" % (
                virtual_report_path, report_result))
            SuiteReporter.append_report_result(
                (virtual_report_path, report_result))
        else:
            import shutil
            history_execute_result = report_data_dict.get(module_name, "")
            LOG.info("start copy %s" % history_execute_result)
            file_name = get_filename_extension(history_execute_result)[0]
            if os.path.exists(history_execute_result):
                result_dir = \
                    os.path.join(task.config.report_path, "result")
                os.makedirs(result_dir, exist_ok=True)
                target_execute_result = "%s.xml" % os.path.join(
                    task.config.report_path, "result", file_name)
                shutil.copyfile(history_execute_result, target_execute_result)
                LOG.info("copy %s to %s" % (
                    history_execute_result, target_execute_result))
            else:
                error_msg = "copy failed! %s not exists!" %\
                            history_execute_result
                raise ParamError(error_msg)

    def _handle_device_error(self, exception, task, test_drivers):
        self._display_executing_process(None, test_drivers[0], test_drivers)
        error_message = "%s: %s" % \
                        (get_instance_name(exception), exception)
        LOG.exception(error_message, exc_info=False,
                      error_no=exception.error_no)
        if check_mode(ModeType.decc):
            error_message = "Load Error[00104]"
        self.report_not_executed(task.config.report_path, [test_drivers[0]],
                                 error_message, task)

        LOG.info("")
        test_drivers.pop(0)

    @classmethod
    def _clear_not_executed(cls, task, test_drivers):
        if Scheduler.mode != ModeType.decc:
            # clear all
            test_drivers.clear()
            return
        # The result is reported only in DECC mode, and also clear all.
        LOG.error("case no run: task execution terminated!", error_no="00300")
        error_message = "Execute Terminate[00300]"
        cls.report_not_executed(task.config.report_path, test_drivers,
                                error_message)
        test_drivers.clear()

    @classmethod
    def report_not_executed(cls, report_path, test_drivers, error_message,
                            task=None):
        # traversing list to get remained elements
        for test_driver in test_drivers:
            # get report file
            if task and getattr(task.config, "testdict", ""):
                report_file = os.path.join(get_sub_path(
                    test_driver[1].source.source_file),
                    "%s.xml" % test_driver[1].source.test_name)
            else:
                report_file = os.path.join(
                    report_path, "result",
                    "%s.xml" % test_driver[1].source.module_name)

            # get report name
            report_name = test_driver[1].source.test_name if \
                not test_driver[1].source.test_name.startswith("{") \
                else "report"

            # get module name
            module_name = test_driver[1].source.module_name

            # here, normally create empty report and then upload result
            check_result_report(report_path, report_file, error_message,
                                report_name, module_name)

    def _start_driver_thread(self, current_driver_threads, thread_params):
        environment, message_queue, task, test_driver = thread_params
        thread_id = self._get_thread_id(current_driver_threads)
        driver_thread = DriversThread(test_driver, task, environment,
                                      message_queue)
        driver_thread.setDaemon(True)
        driver_thread.set_thread_id(thread_id)
        driver_thread.set_listeners(self._create_listeners(task))
        driver_thread.start()
        current_driver_threads.setdefault(thread_id, driver_thread)

    @classmethod
    def _do_taskkit_teardown(cls, used_devices, task_unused_env):
        for device in used_devices.values():
            if getattr(device, ConfigConst.need_kit_setup, True):
                continue
            
            for kit in getattr(device, ConfigConst.task_kits, []):
                try:
                    kit.__teardown__(device)
                except Exception as error:
                    LOG.debug("do taskkit teardown: %s" % error)
            setattr(device, ConfigConst.task_kits, [])
            setattr(device, ConfigConst.need_kit_setup, True)

        for environment in task_unused_env:
            for device in environment.devices:
                setattr(device, ConfigConst.task_state, True)
                setattr(device, ConfigConst.need_kit_setup, True)

    def _display_executing_process(self, environment, test_driver,
                                   test_drivers):
        source_content = test_driver[1].source.source_file or \
                         test_driver[1].source.source_string
        if environment is None:
            LOG.info("[%d / %d] Executing: %s, Driver: %s" %
                     (self.test_number - len(test_drivers) + 1,
                      self.test_number, source_content,
                      test_driver[1].source.test_type))
            return

        LOG.info("[%d / %d] Executing: %s, Device: %s, Driver: %s" %
                 (self.test_number - len(test_drivers) + 1,
                  self.test_number, source_content,
                  environment.__get_serial__(),
                  test_driver[1].source.test_type))

    @classmethod
    def _get_thread_id(cls, current_driver_threads):
        thread_id = datetime.datetime.now().strftime(
            '%Y-%m-%d-%H-%M-%S-%f')
        while thread_id in current_driver_threads.keys():
            thread_id = datetime.datetime.now().strftime(
                '%Y-%m-%d-%H-%M-%S-%f')
        return thread_id

    @classmethod
    def _append_used_devices(cls, environment, used_devices):
        for device in environment.devices:
            device_serial = device.__get_serial__() if device else "None"
            if device_serial and device_serial not in used_devices.keys():
                used_devices[device_serial] = device

    @staticmethod
    def _start_queue_monitor(message_queue, test_drivers,
                             current_driver_threads):
        queue_monitor_thread = QueueMonitorThread(message_queue,
                                                  current_driver_threads,
                                                  test_drivers)
        queue_monitor_thread.setDaemon(True)
        queue_monitor_thread.start()
        return queue_monitor_thread

    def exec_command(self, command, options):
        """
        Directly executes a command without adding it to the command queue.
        """
        if command != "run":
            raise ParamError("unsupported command action: %s" % command,
                             error_no="00100")
        exec_type = options.exectype
        if exec_type in [TestExecType.device_test, TestExecType.host_test,
                         TestExecType.host_driven_test]:
            self._exec_task(options)
        else:
            LOG.error("unsupported execution type '%s'" % exec_type,
                      error_no="00100")

        return

    def _exec_task(self, options):
        """
        Directly allocates a device and execute a device test.
        """
        try:
            task = self.__discover__(options.__dict__)
            self.__execute__(task)
        except (ParamError, ValueError, TypeError, SyntaxError,
                AttributeError) as exception:
            error_no = getattr(exception, "error_no", "00000")
            LOG.exception("%s: %s" % (get_instance_name(exception), exception),
                          exc_info=False, error_no=error_no)
            if Scheduler.upload_address:
                Scheduler.upload_unavailable_result(str(exception.args))
                Scheduler.upload_report_end()
        finally:
            self.stop_task_log()
            self.stop_encrypt_log()

    @classmethod
    def _reset_environment(cls, environment="", config_file=""):
        env_manager = EnvironmentManager()
        env_manager.env_stop()
        EnvironmentManager(environment, config_file)

    @classmethod
    def _restore_environment(cls):
        env_manager = EnvironmentManager()
        env_manager.env_stop()
        EnvironmentManager()

    @classmethod
    def start_task_log(cls, log_path):
        tool_file_name = "task_log.log"
        tool_log_file = os.path.join(log_path, tool_file_name)
        add_task_file_handler(tool_log_file)

    @classmethod
    def start_encrypt_log(cls, log_path):
        from _core.report.encrypt import check_pub_key_exist
        if check_pub_key_exist():
            encrypt_file_name = "task_log.ept"
            encrypt_log_file = os.path.join(log_path, encrypt_file_name)
            add_encrypt_file_handler(encrypt_log_file)

    @classmethod
    def stop_task_log(cls):
        remove_task_file_handler()

    @classmethod
    def stop_encrypt_log(cls):
        remove_encrypt_file_handler()

    @staticmethod
    def _find_test_root_descriptor(config):
        # read test list from testdict
        if getattr(config, "testdict", "") != "" and getattr(
                config, "testfile", "") == "":
            uid = unique_id("Scheduler", "testdict")
            root = Descriptor(uuid=uid, name="testdict",
                              source=TestSetSource(config.testdict),
                              container=True)
            root.children = find_testdict_descriptors(config)
            return root

        # read test list from testfile, testlist or task
        test_set = getattr(config, "testfile", "") or getattr(
            config, "testlist", "") or getattr(config, "task", "") or getattr(
            config, "testcase")
        if test_set:
            fname, _ = get_filename_extension(test_set)
            uid = unique_id("Scheduler", fname)
            root = Descriptor(uuid=uid, name=fname,
                              source=TestSetSource(test_set), container=True)
            root.children = find_test_descriptors(config)
            return root
        else:
            raise ParamError("no test file, list, dict, case or task found",
                             error_no="00102")

    @classmethod
    def terminate_cmd_exec(cls):
        Scheduler.is_execute = False
        LOG.info("start to terminate execution")
        return Scheduler.terminate_result.get()

    @classmethod
    def upload_case_result(cls, upload_param):
        if not Scheduler.upload_address:
            return
        case_id, result, error, start_time, end_time, report_path = \
            upload_param
        if error and len(error) > 150:
            error = "%s..." % error[:150]
        LOG.info(
            "get upload params: %s, %s, %s, %s, %s, %s" % (
                case_id, result, error, start_time, end_time, report_path))
        if Scheduler.proxy:
            Scheduler.proxy.upload_result(case_id, result, error,
                start_time, end_time, report_path)
            return
        from agent.factory import upload_result
        upload_result(case_id, result, error, start_time, end_time,
                      report_path)

    @classmethod
    def upload_module_result(cls, exec_message):
        if not Scheduler.is_execute:
            return
        result_file = exec_message.get_result()
        request = exec_message.get_request()

        test_name = request.root.source.test_name
        if not result_file or not os.path.exists(result_file):
            LOG.error("%s result not exists", test_name, error_no="00200")
            return

        test_type = request.root.source.test_type
        LOG.info("need upload result: %s, test type: %s" %
                 (result_file, test_type))
        upload_params, _, _ = cls._get_upload_params(result_file, request)
        if not upload_params:
            LOG.error("%s no test case result to upload" % result_file,
                      error_no="00201")
            return
        LOG.info("need upload %s case" % len(upload_params))
        upload_suite = []
        for upload_param in upload_params:
            case_id, result, error, start_time, end_time, report_path = \
                upload_param
            case = {"caseid": case_id, "result": result, "error": error,
                    "start": start_time, "end": end_time,
                    "report": report_path}
            LOG.info("case info: %s", case)
            upload_suite.append(case)
        if Scheduler.proxy:
            Scheduler.proxy.upload_result(case_id, result, error,
                start_time, end_time, report_path)
            return
        from agent.factory import upload_batch            
        upload_batch(upload_suite)

    @classmethod
    def _get_upload_params(cls, result_file, request):
        upload_params = []

        report_path = result_file
        task_log_path = os.path.join(request.config.report_path, "log",
                                     "task_log.log")
        testsuites_element = DataHelper.parse_data_report(report_path)
        start_time, end_time = cls._get_time(testsuites_element)

        for testsuite_element in testsuites_element:
            if check_mode(ModeType.developer):
                module_name = str(get_filename_extension(
                    report_path)[0]).split(".")[0]
            else:
                module_name = testsuite_element.get(ReportConstant.name,
                                                    "none")
            for case_element in testsuite_element:
                case_id = cls._get_case_id(case_element, module_name)
                case_result, error = cls._get_case_result(case_element)
                if error and len(error) > 150:
                    error = "%s..." % error[:150]
                if case_result == "Ignored":
                    LOG.info("get upload params: %s result is ignored",
                             case_id)
                    continue
                upload_params.append(
                    (case_id, case_result, error, start_time,
                     end_time, task_log_path))
        return upload_params, start_time, end_time

    @classmethod
    def get_script_result(cls, model_element):
        disabled = int(model_element.get(ReportConstant.disabled)) if \
            model_element.get(ReportConstant.disabled, "") else 0
        failures = int(model_element.get(ReportConstant.failures)) if \
            model_element.get(ReportConstant.failures, "") else 0
        errors = int(model_element.get(ReportConstant.errors)) if \
            model_element.get(ReportConstant.errors, "") else 0
        unavailable = int(model_element.get(ReportConstant.unavailable)) if \
            model_element.get(ReportConstant.unavailable, "") else 0
        if failures > 0 or errors > 0:
            result = "Failed"
        elif disabled > 0 or unavailable > 0:
            result = "Unavailable"
        else:
            result = "Passed"

        if result == "Passed":
            return result, ""
        if Scheduler.mode == ModeType.decc:
            result = "Failed"

        error_msg = model_element.get(ReportConstant.message, "")
        if not error_msg and len(model_element) > 0:
            error_msg = model_element[0].get(ReportConstant.message, "")
            if not error_msg and len(model_element[0]) > 0:
                error_msg = model_element[0][0].get(ReportConstant.message, "")
        return result, error_msg

    @classmethod
    def _get_case_id(cls, case_element, package_name):
        class_name = case_element.get(ReportConstant.class_name, "none")
        method_name = case_element.get(ReportConstant.name, "none")
        case_id = "{}#{}#{}#{}".format(Scheduler.task_name, package_name,
                                       class_name, method_name)
        return case_id

    @classmethod
    def _get_case_result(cls, case_element):
        # get result
        case = Case()
        case.status = case_element.get(ReportConstant.status, "")
        case.result = case_element.get(ReportConstant.result, "")
        if case_element.get(ReportConstant.message, ""):
            case.message = case_element.get(ReportConstant.message)
        if len(case_element) > 0:
            if not case.result:
                case.result = ReportConstant.false
            case.message = case_element[0].get(ReportConstant.message)
        if case.is_passed():
            result = "Passed"
        elif case.is_failed():
            result = "Failed"
        elif case.is_blocked():
            result = "Blocked"
        elif case.is_ignored():
            result = "Ignored"
        else:
            result = "Unavailable"
        return result, case.message

    @classmethod
    def _get_time(cls, testsuite_element):
        start_time = testsuite_element.get(ReportConstant.start_time, "")
        end_time = testsuite_element.get(ReportConstant.end_time, "")
        try:
            if start_time and end_time:
                start_time = int(time.mktime(time.strptime(
                    start_time, ReportConstant.time_format)) * 1000)
                end_time = int(time.mktime(time.strptime(
                    end_time, ReportConstant.time_format)) * 1000)
            else:
                timestamp = str(testsuite_element.get(
                    ReportConstant.time_stamp, "")).replace("T", " ")
                cost_time = testsuite_element.get(ReportConstant.time, "")
                if timestamp and cost_time:
                    try:
                        end_time = int(time.mktime(time.strptime(
                            timestamp, ReportConstant.time_format)) * 1000)
                    except ArithmeticError as error:
                        LOG.error("get time error %s" % error)
                        end_time = int(time.time() * 1000)
                    start_time = int(end_time - float(cost_time) * 1000)
                else:
                    current_time = int(time.time() * 1000)
                    start_time, end_time = current_time, current_time
        except ArithmeticError as error:
            LOG.error("get time error %s" % error)
            current_time = int(time.time() * 1000)
            start_time, end_time = current_time, current_time
        return start_time, end_time

    @classmethod
    def upload_task_result(cls, task, error_message=""):
        if not Scheduler.task_name:
            LOG.info("no need upload summary report")
            return

        summary_data_report = os.path.join(task.config.report_path,
                                           ReportConstant.summary_data_report)
        summary_vision_report = os.path.join(
            task.config.report_path, ReportConstant.summary_vision_report)
        if not os.path.exists(summary_data_report):
            task_log_path = os.path.join(task.config.report_path, "log",
                                         "task_log.log")
            if not os.path.exists(task_log_path):
                task_log_path = ""
            Scheduler.upload_unavailable_result(str(
                error_message) or "summary report not exists", task_log_path)
            return

        task_element = ElementTree.parse(summary_data_report).getroot()
        start_time, end_time = cls._get_time(task_element)
        task_result = cls._get_task_result(task_element)
        if task_result == "Unavailable":
            summary_vision_report = os.path.join(task.config.report_path,
                                                 "log", "task_log.log")
        error_msg = ""
        for child in task_element:
            if child.get(ReportConstant.message, ""):
                error_msg = "{}{}".format(
                    error_msg, "%s;" % child.get(ReportConstant.message))
        if error_msg:
            error_msg = error_msg[:-1]
        cls.upload_case_result((Scheduler.task_name, task_result,
                                error_msg, start_time, end_time,
                                summary_vision_report))

    @classmethod
    def _get_task_result(cls, task_element):
        failures = int(task_element.get(ReportConstant.failures, 0))
        errors = int(task_element.get(ReportConstant.errors, 0))
        disabled = int(task_element.get(ReportConstant.disabled, 0))
        unavailable = int(task_element.get(ReportConstant.unavailable, 0))
        if disabled > 0:
            task_result = "Blocked"
        elif errors > 0 or failures > 0:
            task_result = "Failed"
        elif unavailable > 0:
            task_result = "Unavailable"
        else:
            task_result = "Passed"
        return task_result

    @classmethod
    def upload_unavailable_result(cls, error_msg, report_path=""):
        start_time = int(time.time() * 1000)
        Scheduler.upload_case_result((Scheduler.task_name, "Unavailable",
                                      error_msg, start_time, start_time,
                                      report_path))

    @classmethod
    def upload_report_end(cls):
        LOG.info("Upload report end")
        if Scheduler.proxy is not None:
            Scheduler.proxy.report_end()
            return
        from agent.factory import report_end
        report_end()

    @classmethod
    def is_module_need_retry(cls, task, module_name):
        failed_flag = False
        if check_mode(ModeType.decc):
            from xdevice import SuiteReporter
            for module, failed in SuiteReporter.get_failed_case_list():
                if module_name == module or str(module_name).split(
                        ".")[0] == module:
                    failed_flag = True
                    break
        else:
            from xdevice import ResultReporter
            history_report_path = \
                getattr(task.config, ConfigConst.history_report_path, "")
            params = ResultReporter.get_task_info_params(history_report_path)
            if params and params[3]:
                if dict(params[3]).get(module_name, []):
                    failed_flag = True
                elif dict(params[3]).get(str(module_name).split(".")[0], []):
                    failed_flag = True
        return failed_flag

    @classmethod
    def compare_spt_time(cls, kit_spt, device_spt):
        if not kit_spt or not device_spt:
            return False
        try:
            kit_time = str(kit_spt).split("-")[:2]
            device_time = str(device_spt).split("-")[:2]
            k_spt = datetime.datetime.strptime(
                "-".join(kit_time), "%Y-%m")
            d_spt = datetime.datetime.strptime("-".join(device_time), "%Y-%m")
        except ValueError as value_error:
            LOG.debug("date format is error, %s" % value_error.args)
            return False
        month_interval = int(k_spt.month) - int(d_spt.month)
        year_interval = int(k_spt.year) - int(d_spt.year)
        LOG.debug("kit spt (year=%s, month=%s), device spt (year=%s, month=%s)"
                  % (k_spt.year, k_spt.month, d_spt.year, d_spt.month))
        if year_interval == 0 and month_interval in (0, 1, 2):
            return True
        if year_interval == 1 and month_interval + 12 in (1, 2):
            return True

    @classmethod
    def _parse_version_id(cls, driver_request, kit):
        test_args = copy.deepcopy(
            driver_request.config.get(ConfigConst.testargs, dict()))
        version_id = ""
        if ConfigConst.pass_through in test_args.keys():
            import json
            pt_dict = json.loads(test_args.get(ConfigConst.pass_through, ""))
            version_id = pt_dict.get("VersionID", None)
        elif "VersionID" in test_args.keys():
            version_id = test_args.get("VersionID", None)
        if version_id:
            kit_version = version_id
        else:
            kit_version = kit.properties.get(ConfigConst.version, None)
        return kit_version

    @classmethod
    def update_test_type_in_source(cls, key, value):
        LOG.debug("update test type dict in source")
        TestDictSource.test_type[key] = value

    @classmethod
    def update_ext_type_in_source(cls, key, value):
        LOG.debug("update ext type dict in source")
        TestDictSource.exe_type[key] = value

    @classmethod
    def _clear_test_dict_source(cls):
        TestDictSource.exe_type.clear()
        TestDictSource.test_type.clear()
