#!/usr/bin/env python
# -*- coding:utf-8 -*-
#
# Copyright (c) 2021 Huawei Device Co., Ltd.
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

import re


class RunResult():
    SUCCESS = 0

    RUN_ENV_ERROR = -1
    RUN_NO_DEVICE_ERROR = -2
    RUN_CONFIG_FORMAT_ERROR = -3
    RUN_CONFIG_FORMAT_NOCOLON_ERROR = -4
    RUN_FUZZER_BIN_NOT_FOUND_ERROR = -5
    RUN_CONNECT_ERROR = -6
    RUN_EXEC_ERROR = -7
    RUN_CONFIG_DICT_ERROR = -8
    RUN_LINK_ERROR = -9


    def __init__(self, code, data):
        self.code = code
        self.data = data
        self.payload = {}

        self.crash_info =  {
            "run_times": 0,
            "log": "",
            "project": "",
            "speed": 0,
            "summary": "No vulnerable",
            "command_log": "",
            "vulnerable": False,
            "backtrace": "",
            "cov": 0,
            "libscov": {},
            "report_progress": 0
        }


    def get_log(self):
        return "code :{}, msg: {}".format(self.code, self.data)

    @staticmethod
    def filter_log(log_str):
        ansi_escape = re.compile(r'''
            \x1B
            (?:
                [@-Z\\-_]
            |
                \[
                [0-?]*
                [ -/]*
                [@-~]
            )
        ''', re.VERBOSE)
        result = ansi_escape.sub('', log_str)
        return result

    def analysis(self, result, outdir):
        pass

    def write_analysis_result(self, analysis_ressult_path, html_format=True):
        with open(analysis_ressult_path, "wb") as f:
            if html_format:
                f.write(RunResult.filter_log(render_detail(self.crash_info)))
            else:
                f.write(RunResult.filter_log(self.crash_info["backtrace"]))


if __name__ == "__main__":
    cmd_log = ""
    res_obj = RunResult(0, "OK")
    res_obj.analysis(cmd_log, "../../../out/")
    print(res_obj.crash_info)

