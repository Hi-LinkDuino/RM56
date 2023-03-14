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

__all__ = [
    "SchedulerType",
    "ToolCommandType",
    "ConfigFileConst"
]


class SchedulerType(object):
    """
    SchedulerType enumeration
    """
    # default scheduler
    SCHEDULER = "Scheduler"
    COMBINATION = "Combination"

    @property
    def default_type(self):
        return SchedulerType.SCHEDULER

    @property
    def combination_type(self):
        return SchedulerType.COMBINATION


class ToolCommandType(object):
    TOOLCMD_KEY_HELP = "help"
    TOOLCMD_KEY_SHOW = "show"
    TOOLCMD_KEY_RUN = "run"
    TOOLCMD_KEY_QUIT = "quit"
    TOOLCMD_KEY_LIST = "list"
    TOOLCMD_KEY_GEN = "gen"

    @property
    def run_command(self):
        return ToolCommandType.TOOLCMD_KEY_RUN

    @property
    def help_command(self):
        return ToolCommandType.TOOLCMD_KEY_HELP


class ConfigFileConst(object):
    FRAMECONFIG_FILEPATH = "framework_config.xml"
    BUILDCONFIG_FILEPATH = "build_config.xml"
    USERCONFIG_FILEPATH = "user_config.xml"
    FILTERCONFIG_FILEPATH = "filter_config.xml"
    RESOURCECONFIG_FILEPATH = "harmony_test.xml"
    CASE_RESOURCE_FILEPATH = "ohos_test.xml"
    SCENECONFIG_FILEPATH = "scene_config.xml"
    FUZZCONFIG_FILEPATH = "fuzz_config.xml"

    @property
    def framework_config_file(self):
        return ConfigFileConst.FRAMECONFIG_FILEPATH

    @property
    def user_config_file(self):
        return ConfigFileConst.USERCONFIG_FILEPATH


class JsTestConst(object):
    BUILD_GN_FILE_TEMPLATE = """\
# Copyright (C) 2021 Huawei Device Co., Ltd.
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

import("//build/test.gni")

module_output_path = "%(output_path)s"

ohos_js_unittest("%(suite_name)s") {
  module_out_path = module_output_path
  hap_profile = "./src/main/config.json"
  deps = [
    ":%(suite_name)s_js_assets",
    ":%(suite_name)s_resources",
  ]

  certificate_profile = "//test/developertest/signature/openharmony_sx.p7b"
  hap_name = "%(suite_name)s"
}
ohos_js_assets("%(suite_name)s_js_assets") {
  source_dir = "./src/main/js/default"
}
ohos_resources("%(suite_name)s_resources") {
  sources = [ "./src/main/resources" ]
  hap_profile = "./src/main/config.json"
}

group("unittest") {
  testonly = true
  deps = [ ":%(suite_name)s" ]
}
"""

    @property
    def build_gn_template(self):
        return JsTestConst.BUILD_GN_FILE_TEMPLATE
