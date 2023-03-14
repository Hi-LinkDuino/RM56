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
import sys
import subprocess

from xdevice import IReporter
from xdevice import Plugin
from xdevice import TestType

from _core.logger import platform_logger

__all__ = ["BenchmarkReporter"]
LOG = platform_logger("BenchmarkReporter")

@Plugin(type=Plugin.REPORTER, id=TestType.benchmark)
class BenchmarkReporter(IReporter):

    def __generate_reports__(self, report_path, **kwargs):
        del kwargs
        LOG.info("report_path = %s" % report_path)
        self._make_benchmark_report(report_path)

    def _make_benchmark_report(self, result_path):
        result_path = os.path.join(result_path, "benchmark")
        reports_dir = os.path.join(result_path, "benchmark", "report")
        if not os.path.exists(reports_dir):
            os.makedirs(reports_dir)
        report_generate_tool = os.path.abspath(
            os.path.join(os.path.dirname(os.path.realpath(__file__)),
                         "generate_report.py"))

        command = [sys.executable, report_generate_tool, result_path,
                   reports_dir]
        LOG.info(command)
        subprocess.call(command, shell=False)
