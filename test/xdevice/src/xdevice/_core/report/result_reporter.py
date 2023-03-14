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
import platform
import shutil
import time
import zipfile
from ast import literal_eval

from _core.interface import IReporter
from _core.plugin import Plugin
from _core.constants import ModeType
from _core.constants import TestType
from _core.logger import platform_logger
from _core.exception import ParamError
from _core.utils import get_filename_extension
from _core.report.encrypt import check_pub_key_exist
from _core.report.encrypt import do_rsa_encrypt
from _core.report.encrypt import get_file_summary
from _core.report.reporter_helper import DataHelper
from _core.report.reporter_helper import ExecInfo
from _core.report.reporter_helper import VisionHelper
from _core.report.reporter_helper import ReportConstant

LOG = platform_logger("ResultReporter")


@Plugin(type=Plugin.REPORTER, id=TestType.all)
class ResultReporter(IReporter):
    summary_report_result = []

    def __init__(self):
        self.report_path = None
        self.task_info = None
        self.summary_data_path = None
        self.summary_data_str = ""
        self.exec_info = None
        self.parsed_data = None
        self.data_helper = None
        self.vision_helper = None

    def __generate_reports__(self, report_path, **kwargs):
        LOG.info("")
        LOG.info("**************************************************")
        LOG.info("************** Start generate reports ************")
        LOG.info("**************************************************")
        LOG.info("")

        if self._check_params(report_path, **kwargs):
            # generate data report
            self._generate_data_report()

            # generate vision reports
            self._generate_vision_reports()

            # generate task info record
            self._generate_task_info_record()

            # generate summary ini
            self._generate_summary()

            # copy reports to reports/latest folder
            self._copy_report()

            # compress report folder
            self._compress_report_folder()

        LOG.info("")
        LOG.info("**************************************************")
        LOG.info("************** Ended generate reports ************")
        LOG.info("**************************************************")
        LOG.info("")

    def _check_params(self, report_path, **kwargs):
        task_info = kwargs.get("task_info", "")
        if not report_path:
            LOG.error("report path is wrong", error_no="00440",
                      ReportPath=report_path)
            return False
        if not task_info or not isinstance(task_info, ExecInfo):
            LOG.error("task info is wrong", error_no="00441",
                      TaskInfo=task_info)
            return False

        os.makedirs(report_path, exist_ok=True)
        self.report_path = report_path
        self.task_info = task_info
        self.summary_data_path = os.path.join(
            self.report_path, ReportConstant.summary_data_report)
        self.exec_info = task_info
        self.data_helper = DataHelper()
        self.vision_helper = VisionHelper()
        return True

    def _generate_data_report(self):
        # initial element
        test_suites_element = self.data_helper.initial_suites_element()

        # update test suites element
        update_flag = self._update_test_suites(test_suites_element)
        if not update_flag:
            return

        # generate report
        if not self._check_mode(ModeType.decc):
            self.data_helper.generate_report(test_suites_element,
                                             self.summary_data_path)

        # set SuiteReporter.suite_report_result
        if not check_pub_key_exist() and not self._check_mode(
                ModeType.decc):
            return
        self.set_summary_report_result(
            self.summary_data_path, DataHelper.to_string(test_suites_element))

        if self._check_mode(ModeType.decc):
            try:
                from agent.decc import Handler
                from xdevice import Scheduler
                LOG.info("upload task summary result to decc")
                Handler.upload_task_summary_results(
                    self.get_result_of_summary_report())
            except ModuleNotFoundError as error:
                LOG.error("module not found %s", error.args)

    def _update_test_suites(self, test_suites_element):
        # initial attributes for test suites element
        test_suites_attributes, need_update_attributes = \
            self._init_attributes()

        # get test suite elements that are children of test suites element
        modules = dict()
        test_suite_elements = []
        for data_report, module_name in self.data_reports:
            if data_report.endswith(ReportConstant.summary_data_report):
                continue
            root = self.data_helper.parse_data_report(data_report)
            if module_name == ReportConstant.empty_name:
                module_name = self._get_module_name(data_report, root)
            total = int(root.get(ReportConstant.tests, 0))
            modules[module_name] = modules.get(module_name, 0) + total

            self._append_product_info(test_suites_attributes, root)
            for child in root:
                child.tail = self.data_helper.LINE_BREAK_INDENT
                if not child.get(ReportConstant.module_name) or child.get(
                        ReportConstant.module_name) == \
                        ReportConstant.empty_name:
                    child.set(ReportConstant.module_name, module_name)
                self._check_tests_and_unavailable(child)
                # covert "notrun" to "ignored" for the test case status
                for element in child:
                    if element.get(ReportConstant.status, "") == \
                            ReportConstant.not_run:
                        ignored = int(child.get(ReportConstant.ignored, 0)) + 1
                        child.set(ReportConstant.ignored, "%s" % ignored)
                test_suite_elements.append(child)
                for update_attribute in need_update_attributes:
                    update_value = child.get(update_attribute, 0)
                    if not update_value:
                        update_value = 0
                    test_suites_attributes[update_attribute] += int(
                        update_value)

        if test_suite_elements:
            child = test_suite_elements[-1]
            child.tail = self.data_helper.LINE_BREAK
        else:
            LOG.error("execute result not exists")
            return False

        # set test suites element attributes and children
        modules_zero = [module_name for module_name, total in modules.items()
                        if total == 0]
        if modules_zero:
            LOG.info("the total tests of %s module is 0", ",".join(
                modules_zero))
        test_suites_attributes[ReportConstant.run_modules] = \
            len(modules) - len(modules_zero)
        test_suites_attributes[ReportConstant.modules] = len(modules)
        self.data_helper.set_element_attributes(test_suites_element,
                                                test_suites_attributes)
        test_suites_element.extend(test_suite_elements)
        return True

    @classmethod
    def _check_tests_and_unavailable(cls, child):
        total = child.get(ReportConstant.tests, "0")
        unavailable = child.get(ReportConstant.unavailable, "0")
        if total and total != "0" and unavailable and \
                unavailable != "0":
            child.set(ReportConstant.unavailable, "0")
            LOG.warning("%s total: %s, unavailable: %s", child.get(
                ReportConstant.name), total, unavailable)

    @classmethod
    def _append_product_info(cls, test_suites_attributes, root):
        product_info = root.get(ReportConstant.product_info, "")
        if not product_info:
            return
        try:
            product_info = literal_eval(str(product_info))
        except SyntaxError as error:
            LOG.error("%s %s", root.get(ReportConstant.name, ""), error.args)
            product_info = {}

        if not test_suites_attributes[ReportConstant.product_info]:
            test_suites_attributes[ReportConstant.product_info] = \
                product_info
            return
        for key, value in product_info.items():
            exist_value = test_suites_attributes[
                ReportConstant.product_info].get(key, "")

            if not exist_value:
                test_suites_attributes[
                    ReportConstant.product_info][key] = value
                continue
            if value in exist_value:
                continue
            test_suites_attributes[ReportConstant.product_info][key] = \
                "%s,%s" % (exist_value, value)

    @classmethod
    def _get_module_name(cls, data_report, root):
        # get module name from data report
        module_name = get_filename_extension(data_report)[0]
        if "report" in module_name or "summary" in module_name or \
                "<" in data_report or ">" in data_report:
            module_name = root.get(ReportConstant.name,
                                   ReportConstant.empty_name)
            if "report" in module_name or "summary" in module_name:
                module_name = ReportConstant.empty_name
        return module_name

    def _init_attributes(self):
        test_suites_attributes = {
            ReportConstant.name:
                ReportConstant.summary_data_report.split(".")[0],
            ReportConstant.start_time: self.task_info.test_time,
            ReportConstant.end_time: time.strftime(ReportConstant.time_format,
                                                   time.localtime()),
            ReportConstant.errors: 0, ReportConstant.disabled: 0,
            ReportConstant.failures: 0, ReportConstant.tests: 0,
            ReportConstant.ignored: 0, ReportConstant.unavailable: 0,
            ReportConstant.product_info: self.task_info.product_info,
            ReportConstant.modules: 0, ReportConstant.run_modules: 0}
        need_update_attributes = [ReportConstant.tests, ReportConstant.ignored,
                                  ReportConstant.failures,
                                  ReportConstant.disabled,
                                  ReportConstant.errors,
                                  ReportConstant.unavailable]
        return test_suites_attributes, need_update_attributes

    def _generate_vision_reports(self):
        if not self._check_mode(ModeType.decc) and not \
                self.summary_data_report_exist:
            LOG.error("summary data report not exists")
            return

        if check_pub_key_exist() or self._check_mode(ModeType.decc):
            if not self.summary_report_result_exists():
                LOG.error("summary data report not exists")
                return
            self.summary_data_str = \
                self.get_result_of_summary_report()
            if check_pub_key_exist():
                from xdevice import SuiteReporter
                SuiteReporter.clear_report_result()

        # parse data
        if self.summary_data_str:
            # only in decc mode and pub key, self.summary_data_str is not empty
            summary_element_tree = self.data_helper.parse_data_report(
                self.summary_data_str)
        else:
            summary_element_tree = self.data_helper.parse_data_report(
                self.summary_data_path)
        parsed_data = self.vision_helper.parse_element_data(
            summary_element_tree, self.report_path, self.task_info)
        self.parsed_data = parsed_data
        self.exec_info, summary, _ = parsed_data

        if self._check_mode(ModeType.decc):
            return

        LOG.info("Summary result: modules: %s, run modules: %s, total: "
                 "%s, passed: %s, failed: %s, blocked: %s, ignored: %s, "
                 "unavailable: %s", summary.modules, summary.run_modules,
                 summary.result.total, summary.result.passed,
                 summary.result.failed, summary.result.blocked,
                 summary.result.ignored, summary.result.unavailable)
        LOG.info("Log path: %s", self.exec_info.log_path)

        # generate summary vision report
        report_generate_flag = self._generate_vision_report(
            parsed_data, ReportConstant.summary_title,
            ReportConstant.summary_vision_report)

        # generate details vision report
        if report_generate_flag and summary.result.total > 0:
            self._generate_vision_report(
                parsed_data, ReportConstant.details_title,
                ReportConstant.details_vision_report)

        # generate failures vision report
        if summary.result.total != (
                summary.result.passed + summary.result.ignored) or \
                summary.result.unavailable > 0:
            self._generate_vision_report(
                parsed_data, ReportConstant.failures_title,
                ReportConstant.failures_vision_report)

    def _generate_vision_report(self, parsed_data, title, render_target):

        # render data
        report_context = self.vision_helper.render_data(
            title, parsed_data, render_target=render_target)

        # generate report
        if report_context:
            report_path = os.path.join(self.report_path, render_target)
            self.vision_helper.generate_report(report_path, report_context)
            return True
        else:
            LOG.error("Failed to generate %s", render_target)
            return False

    @property
    def summary_data_report_exist(self):
        return "<" in self.summary_data_str or \
               os.path.exists(self.summary_data_path)

    @property
    def data_reports(self):
        if check_pub_key_exist() or self._check_mode(ModeType.decc):
            from xdevice import SuiteReporter
            suite_reports = SuiteReporter.get_report_result()
            if self._check_mode(ModeType.decc):
                LOG.debug("handle history result, data_reports length:{}".
                          format(len(suite_reports)))
                SuiteReporter.clear_history_result()
                SuiteReporter.append_history_result(suite_reports)
            data_reports = []
            for report_path, report_result in suite_reports:
                module_name = get_filename_extension(report_path)[0]
                data_reports.append((report_result, module_name))
            SuiteReporter.clear_report_result()
            return data_reports

        if not os.path.isdir(self.report_path):
            return []
        data_reports = []
        result_path = os.path.join(self.report_path, "result")
        for root, _, files in os.walk(self.report_path):
            for file_name in files:
                if not file_name.endswith(self.data_helper.DATA_REPORT_SUFFIX):
                    continue
                module_name = self._find_module_name(result_path, root)
                data_reports.append((os.path.join(root, file_name),
                                     module_name))
        return data_reports

    @classmethod
    def _find_module_name(cls, result_path, root):
        # find module name from directory tree
        common_path = os.path.commonpath([result_path, root])
        if os.path.normcase(result_path) != os.path.normcase(common_path) or \
                os.path.normcase(result_path) == os.path.normcase(root):
            return ReportConstant.empty_name

        root_dir, module_name = os.path.split(root)
        if os.path.normcase(result_path) == os.path.normcase(root_dir):
            return ReportConstant.empty_name
        root_dir, subsystem_name = os.path.split(root_dir)
        while os.path.normcase(result_path) != os.path.normcase(root_dir):
            module_name = subsystem_name
            root_dir, subsystem_name = os.path.split(root_dir)
        return module_name

    def _generate_summary(self):
        if not self.summary_data_report_exist or \
                self._check_mode(ModeType.decc):
            return
        summary_ini_content = \
            "[default]\n" \
            "Platform=%s\n" \
            "Test Type=%s\n" \
            "Device Name=%s\n" \
            "Host Info=%s\n" \
            "Test Start/ End Time=%s\n" \
            "Execution Time=%s\n" % (
                self.exec_info.platform, self.exec_info.test_type,
                self.exec_info.device_name, self.exec_info.host_info,
                self.exec_info.test_time, self.exec_info.execute_time)
        if self.exec_info.product_info:
            for key, value in self.exec_info.product_info.items():
                summary_ini_content = "{}{}".format(
                    summary_ini_content, "%s=%s\n" % (key, value))

        if not self._check_mode(ModeType.factory):
            summary_ini_content = "{}{}".format(
                summary_ini_content, "Log Path=%s\n" % self.exec_info.log_path)

        # write summary_ini_content
        summary_filepath = os.path.join(self.report_path,
                                        ReportConstant.summary_ini)

        if platform.system() == "Windows":
            flags = os.O_WRONLY | os.O_CREAT | os.O_APPEND | os.O_BINARY
        else:
            flags = os.O_WRONLY | os.O_CREAT | os.O_APPEND
        summary_filepath_open = os.open(summary_filepath, flags, 0o755)

        with os.fdopen(summary_filepath_open, "wb") as file_handler:
            if check_pub_key_exist():
                try:
                    cipher_text = do_rsa_encrypt(summary_ini_content)
                except ParamError as error:
                    LOG.error(error, error_no=error.error_no)
                    cipher_text = b""
                file_handler.write(cipher_text)
            else:
                file_handler.write(bytes(summary_ini_content, 'utf-8'))
            file_handler.flush()
            LOG.info("generate summary ini: %s", summary_filepath)

    def _copy_report(self):
        from xdevice import Scheduler
        if Scheduler.upload_address or self._check_mode(ModeType.decc):
            return

        from xdevice import Variables
        dst_path = os.path.join(Variables.exec_dir,
                                Variables.report_vars.report_dir, "latest")
        try:
            shutil.rmtree(dst_path, ignore_errors=True)
            os.makedirs(dst_path, exist_ok=True)
            LOG.info("copy summary files to %s", dst_path)

            # copy reports to reports/latest folder
            for report_file in os.listdir(self.report_path):
                src_file = os.path.join(self.report_path, report_file)
                dst_file = os.path.join(dst_path, report_file)
                if os.path.isfile(src_file):
                    shutil.copyfile(src_file, dst_file)
        except OSError:
            return

    def _compress_report_folder(self):
        if self._check_mode(ModeType.decc):
            return

        if not os.path.isdir(self.report_path):
            LOG.error("'%s' is not folder!" % self.report_path)
            return

        # get file path list
        file_path_list = []
        for dir_path, _, file_names in os.walk(self.report_path):
            f_path = dir_path.replace(self.report_path, '')
            f_path = f_path and f_path + os.sep or ''
            for filename in file_names:
                file_path_list.append(
                    (os.path.join(dir_path, filename), f_path + filename))

        # compress file
        zipped_file = "%s.zip" % os.path.join(
            self.report_path, os.path.basename(self.report_path))
        zip_object = zipfile.ZipFile(zipped_file, 'w', zipfile.ZIP_DEFLATED,
                                     allowZip64=True)
        try:
            LOG.info("executing compress process, please wait...")
            for src_path, target_path in file_path_list:
                zip_object.write(src_path, target_path)
            LOG.info("generate zip file: %s", zipped_file)
        except zipfile.BadZipFile as bad_error:
            LOG.error("zip report folder error: %s" % bad_error.args)
        finally:
            zip_object.close()

        # generate hex digest, then save it to summary_report.hash
        hash_file = os.path.abspath(os.path.join(
            self.report_path, ReportConstant.summary_report_hash))
        hash_file_open = os.open(hash_file,
                                 os.O_WRONLY | os.O_CREAT | os.O_APPEND, 0o755)
        with os.fdopen(hash_file_open, "w") as hash_file_handler:
            hash_file_handler.write(get_file_summary(zipped_file))
            LOG.info("generate hash file: %s", hash_file)
            hash_file_handler.flush()
        return zipped_file

    @classmethod
    def _check_mode(cls, mode):
        from xdevice import Scheduler
        return Scheduler.mode == mode

    def _generate_task_info_record(self):
        # under encryption status, don't handle anything directly
        if check_pub_key_exist() and not self._check_mode(ModeType.decc):
            return

        # get info from command_queue
        from xdevice import Scheduler
        if not Scheduler.command_queue:
            return
        _, command, report_path = Scheduler.command_queue[-1]

        # handle parsed data
        record = self._parse_record_from_data(command, report_path)

        def encode(content):
            # inner function to encode
            return ' '.join([bin(ord(c)).replace('0b', '') for c in content])

        # write into file
        import json
        record_file = os.path.join(self.report_path,
                                   ReportConstant.task_info_record)
        _record_json = json.dumps(record, indent=2)

        with open(file=record_file, mode="wb") as file:
            if Scheduler.mode == ModeType.decc:
                # under decc, write in encoded text
                file.write(bytes(encode(_record_json), encoding="utf-8"))
            else:
                # others, write in plain text
                file.write(bytes(_record_json, encoding="utf-8"))

        LOG.info("generate record file: %s", record_file)

    def _parse_record_from_data(self, command, report_path):
        record = dict()
        if self.parsed_data:
            _, _, suites = self.parsed_data
            unsuccessful = dict()
            module_set = set()
            for suite in suites:
                module_set.add(suite.module_name)

                failed = unsuccessful.get(suite.module_name, [])
                # because suite not contains case's some attribute,
                # for example, 'module', 'classname', 'name' . so
                # if unavailable, only add module's name into list.
                if int(suite.result.unavailable) > 0:
                    failed.append(suite.module_name)
                else:
                    # others, get key attributes join string
                    for case in suite.get_cases():
                        if not case.is_passed():
                            failed.append(
                                "{}#{}".format(case.classname, case.name))
                unsuccessful.update({suite.module_name: failed})
            data_reports = self._get_data_reports(module_set)
            record = {"command": command,
                      "session_id": os.path.split(report_path)[-1],
                      "report_path": report_path,
                      "unsuccessful_params": unsuccessful,
                      "data_reports": data_reports
                      }
        return record

    def _get_data_reports(self, module_set):
        data_reports = dict()
        if self._check_mode(ModeType.decc):
            from xdevice import SuiteReporter
            for module_name, report_path, report_result in \
                    SuiteReporter.get_history_result_list():
                if module_name in module_set:
                    data_reports.update({module_name: report_path})
        else:
            for report_path, module_name in self.data_reports:
                if module_name == ReportConstant.empty_name:
                    root = self.data_helper.parse_data_report(report_path)
                    module_name = self._get_module_name(report_path, root)
                if module_name in module_set:
                    data_reports.update({module_name: report_path})

        return data_reports

    @classmethod
    def get_task_info_params(cls, history_path):
        # under encryption status, don't handle anything directly
        if check_pub_key_exist() and not cls._check_mode(ModeType.decc):
            return ()

        def decode(content):
            return ''.join([chr(i) for i in [int(b, 2) for b in
                                             content.split(' ')]])

        record_path = os.path.join(history_path,
                                   ReportConstant.task_info_record)
        if not os.path.exists(record_path):
            LOG.error("%s not exists!", ReportConstant.task_info_record)
            return ()

        import json
        from xdevice import Scheduler
        with open(record_path, mode="rb") as file:
            if Scheduler.mode == ModeType.decc:
                # under decc, read from encoded text
                result = json.loads(decode(file.read().decode("utf-8")))
            else:
                # others, read from plain text
                result = json.loads(file.read())
        if not len(result.keys()) == 5:
            LOG.error("%s error!", ReportConstant.task_info_record)
            return ()

        return result["session_id"], result["command"], result["report_path"],\
               result["unsuccessful_params"], result["data_reports"]

    @classmethod
    def set_summary_report_result(cls, summary_data_path, result_xml):
        cls.summary_report_result.clear()
        cls.summary_report_result.append((summary_data_path, result_xml))

    @classmethod
    def get_result_of_summary_report(cls):
        if cls.summary_report_result:
            return cls.summary_report_result[0][1]

    @classmethod
    def summary_report_result_exists(cls):
        return True if cls.summary_report_result else False

    @classmethod
    def get_path_of_summary_report(cls):
        if cls.summary_report_result:
            return cls.summary_report_result[0][0]
