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

import time
import os
import sys

from core.constants import SchedulerType
from xdevice import Plugin
from xdevice import get_plugin
from xdevice import platform_logger
from xdevice import Scheduler
from xdevice import DeviceTestType
from core.utils import get_build_output_path
from core.utils import scan_support_product
from core.utils import is_lite_product
from core.common import is_open_source_product
from core.command.parameter import Parameter
from core.testcase.testcase_manager import TestCaseManager
from core.config.config_manager import UserConfigManager
from core.config.parse_parts_config import ParsePartsConfig

LOG = platform_logger("Run")


class Run(object):
    def process_command_run(self, command, options):
        para = Parameter()
        test_type_list = para.get_testtype_list(options.testtype)
        if len(test_type_list) == 0:
            LOG.error("The testtype parameter is incorrect.")
            return
        options.testtype = test_type_list

        parser = ParsePartsConfig(options.productform)
        partname_list = parser.get_part_list(
            options.subsystem,
            options.testpart)
        options.partname_list = partname_list
        options.coverage_outpath = self.get_coverage_outpath(options)

        LOG.info("")
        LOG.info("------------------------------------")
        LOG.info("Input parameter:")
        LOG.info("productform   = %s" % options.productform)
        LOG.info("testtype      = %s" % str(options.testtype))
        LOG.info("subsystem     = %s" % str(options.subsystem))
        LOG.info("testpart      = %s" % str(options.testpart))
        LOG.info("testmodule    = %s" % options.testmodule)
        LOG.info("testsuit      = %s" % options.testsuit)
        LOG.info("testcase      = %s" % options.testcase)
        LOG.info("testlevel     = %s" % options.testlevel)
        LOG.info("partname_list = %s" % str(options.partname_list))
        LOG.info("------------------------------------")
        LOG.info("")

        if not para.check_run_parameter(options):
            LOG.error("Input parameter is incorrect.")
            return

        if not self._build_test_cases(options):
            LOG.error("Build test cases failed.")
            return

        test_case_path = self.get_tests_out_path(options.productform)
        if not os.path.exists(test_case_path):
            LOG.error("%s is not exist." % test_case_path)
            return

        test_dict = TestCaseManager().get_test_files(test_case_path, options)
        if not self._check_test_dictionary(test_dict):
            LOG.error("The test file list is empty.")
            return

        if ("distributedtest" in options.testtype and
                len(options.testtype) == 1):
            from core.command.distribute_utils \
                import check_ditributetest_environment
            from core.command.distribute_utils import make_device_info_file
            from core.command.distribute_utils \
                import execute_distribute_test_file
            from core.command.distribute_utils import make_reports

            local_time = time.localtime()
            create_time = time.strftime('%Y-%m-%d-%H-%M-%S', local_time)
            start_time = time.strftime('%Y-%m-%d %H:%M:%S', local_time)

            if not check_ditributetest_environment():
                return

            result_rootpath = os.path.join(sys.framework_root_dir,
                "reports",
                create_time)
            print(result_rootpath)

            log_path = os.path.join(result_rootpath, "log")
            tmp_path = os.path.join(result_rootpath, "temp")
            os.makedirs(log_path, exist_ok=True)
            os.makedirs(tmp_path, exist_ok=True)

            Scheduler.start_task_log(log_path)
            make_device_info_file(tmp_path)

            pyfile_list = test_dict["PYT"]
            for index, element in enumerate(pyfile_list):
                LOG.info("[%s / %s] Executing: %s" % (index + 1,
                    len(pyfile_list), element))
                execute_distribute_test_file(element, result_rootpath)

            make_reports(result_rootpath, start_time)
            Scheduler.stop_task_log()
        else:
            options.testdict = test_dict
            options.target_outpath = self.get_target_out_path(
                options.productform)

            scheduler = get_plugin(plugin_type=Plugin.SCHEDULER,
                                   plugin_id=SchedulerType.SCHEDULER)[0]
            if scheduler is None:
                LOG.error("Can not find the scheduler plugin.")
            else:
                if is_lite_product(options.productform,
                                   sys.source_code_root_path):
                    options.testcases_path = options.target_outpath
                    options.resource_path = os.path.abspath(os.path.join(
                        sys.framework_root_dir, "..", "resource"))
                    print(options.testcases_path)
                    print(options.resource_path)
                    if options.productform.find("wifiiot") != -1:
                        scheduler.update_test_type_in_source(".bin",
                            DeviceTestType.ctest_lite)
                        scheduler.update_ext_type_in_source("BIN",
                            DeviceTestType.ctest_lite)
                    else:
                        print("productform is not wifiiot")
                scheduler.exec_command(command, options)
        return

    ##############################################################
    ##############################################################

    @classmethod
    def get_target_out_path(cls, product_form):
        target_out_path = UserConfigManager().get_test_cases_dir()
        if target_out_path == "":
            target_out_path = os.path.join(
                get_build_output_path(product_form),
                "packages",
                product_form)
        target_out_path = os.path.abspath(target_out_path)
        return target_out_path

    @classmethod
    def _build_test_cases(cls, options):
        if options.coverage:
            LOG.info("Coverage testing, no need to compile testcases")
            return True

        is_build_testcase = UserConfigManager().get_user_config_flag(
            "build", "testcase")
        project_root_path = sys.source_code_root_path
        if is_build_testcase and project_root_path != "":
            from core.build.build_manager import BuildManager
            build_manager = BuildManager()
            return build_manager.build_testcases(project_root_path, options)
        else:
            return True

    @classmethod
    def _check_test_dictionary(cls, test_dictionary):
        is_valid_status = False
        key_list = sorted(test_dictionary.keys())
        for key in key_list:
            file_list = test_dictionary[key]
            if len(file_list) > 0:
                is_valid_status = True
                break
        return is_valid_status

    @classmethod
    def get_tests_out_path(cls, product_form):
        testcase_path = UserConfigManager().get_test_cases_dir()
        if testcase_path == "":
            all_product_list = scan_support_product()
            if product_form in all_product_list:
                if is_open_source_product(product_form):
                    testcase_path = os.path.abspath(os.path.join(
                        get_build_output_path(product_form),
                        "packages",
                        "phone",
                        "tests"))
                else:
                    testcase_path = os.path.abspath(os.path.join(
                        get_build_output_path(product_form),
                        "packages",
                        product_form,
                        "tests"))
            else:
                testcase_path = os.path.join(
                    get_build_output_path(product_form), "test")
        LOG.info("testcase_path=%s" % testcase_path)
        return testcase_path

    @classmethod
    def get_coverage_outpath(cls, options):
        coverage_out_path = ""
        if options.coverage:
            coverage_out_path = get_build_output_path(options.productform)
            if coverage_out_path == "":
                coverage_out_path = UserConfigManager().get_user_config(
                    "coverage").get("outpath", "")
            if coverage_out_path == "":
                LOG.error("Coverage test: coverage_outpath is empty.")
        return coverage_out_path


