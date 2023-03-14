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
from xdevice import platform_logger
from core.utils import scan_support_product
from core.config.config_manager import UserConfigManager
from core.build.select_targets import SelectTargets
from core.build.pretreat_targets import PretreatTargets
from core.build.build_testcases import BuildTestcases
from core.command.gen import Gen

LOG = platform_logger("BuildManager")


##############################################################################
##############################################################################

class BuildManager(object):

    @classmethod
    def _make_gn_file(cls, filepath, target_list):
        LOG.info("The gn file path: %s" % filepath)
        with open(filepath, "w") as gn_file:
            gn_file.write("group(\"make_temp_test\") {\n")
            gn_file.write("  testonly = true\n")
            gn_file.write("  deps = []\n")
            if target_list:
                gn_file.write("  deps += [\n")
                for target in target_list:
                    if target:
                        gn_file.write("    \"%s\",\n" % target)
                gn_file.write("  ]\n")
            gn_file.write("}\n")

    @classmethod
    def _compile_test_cases_by_target(cls, project_root_path, product_form,
                                      build_target):
        if BuildTestcases(project_root_path).build_testcases(product_form,
                                                             build_target):
            LOG.info("Test case compilation successed.")
            build_result = True
        else:
            LOG.info("Test case compilation failed, please modify.")
            build_result = False
        return build_result

    @classmethod
    def _compile_fuzz_test_case(cls, project_root_path, para):
        build_result = BuildTestcases(
            project_root_path).build_fuzz_testcases(para)
        if build_result:
            LOG.info("Test case compilation successed.")
        else:
            LOG.info("Test case compilation failed, please modify.")
        return build_result

    def _compile_testcases(self, project_root_path, para):
        all_product_list = scan_support_product()
        if para.productform not in all_product_list:
            from core.build.build_lite_manager import BuildLiteManager
            build_lite_manager = BuildLiteManager(project_root_path)
            return build_lite_manager.build_testcases(para)

        if para.testsuit != "":
            return self._compile_test_cases_by_target(
                project_root_path,
                para.productform,
                para.testsuit)

        if (len(para.partname_list) == 0 and para.testmodule == "" and
                "ALL" in para.testtype):
            return self._compile_test_cases_by_target(
                project_root_path,
                para.productform,
                "make_test")

        target_list = SelectTargets(
            project_root_path).filter_build_targets(para)
        if len(target_list) == 0:
            LOG.warning("No build target found.")
            return False

        build_cfg_filepath = os.path.join(project_root_path,
            "test",
            "developertest",
            "BUILD.gn")

        self._make_gn_file(build_cfg_filepath, target_list)
        if "fuzztest" in para.testtype:
            Gen().gen_fuzzer_list_file(target_list)
            build_result = self._compile_fuzz_test_case(
                project_root_path, para)
        else:
            build_result = self._compile_test_cases_by_target(
                project_root_path,
                para.productform,
                "make_temp_test")
        self._make_gn_file(build_cfg_filepath, [])

        return build_result

    @classmethod
    def build_version(cls, project_root_path, product_form):
        if BuildTestcases(project_root_path).build_version(product_form):
            LOG.info("The version compiled successfully.")
            build_result = True
        else:
            LOG.info("The version compilation failed, please modify.")
            build_result = False
        return build_result

    @classmethod
    def build_gn_file(cls, project_root_path, product_form):
        if BuildTestcases(project_root_path).build_gn_file(product_form):
            LOG.info("The gn compiled successfully.")
            build_result = True
        else:
            LOG.info("The gn compilation failed, please modify.")
            build_result = False
        return build_result

    def build_testcases(self, project_root_path, param):
        if not os.path.exists(project_root_path):
            LOG.error("%s is not exists." % project_root_path)
            return False

        LOG.info("--------------------------------------------------")
        LOG.info("Building parameter:")
        LOG.info("productform   = %s" % param.productform)
        LOG.info("testtype      = %s" % str(param.testtype))
        LOG.info("partname_list = %s" % str(param.partname_list))
        LOG.info("testmodule    = %s" % param.testmodule)
        LOG.info("testsuit      = %s" % param.testsuit)
        LOG.info("--------------------------------------------------")

        LOG.info("")
        LOG.info("**************************************************")
        LOG.info("*************** Start build testcases ************")
        LOG.info("**************************************************")
        LOG.info("")

        build_result = self._compile_testcases(project_root_path, param)

        LOG.info("")
        LOG.info("**************************************************")
        LOG.info("*************** Ended build testcases ************")
        LOG.info("**************************************************")
        LOG.info("")

        return build_result


##############################################################################
##############################################################################
