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
import sys
import os
import time


class Colored(object):
    RED = '\033[1;31m'
    GREEN = '\033[1;32m'
    YELLOW = '\033[1;33m'
    BLUE = '\033[1;34m'
    FUCHSIA = '\033[1;35m'
    CYAN = '\033[1;36m'
    WHITE = '\033[1;37m'
    RESET = '\033[0m'

    DEBUG = True

    LOG_TO_FILE = False
    LOG_DIR = "fuzzlog"
    LOG_PROJECT = None
    LOG_DATE = None

    PROJECT_LOGGER_MAP = {}

    @staticmethod
    def get_project_logger(log_project="default"):
        if log_project in Colored.PROJECT_LOGGER_MAP:
            return Colored.PROJECT_LOGGER_MAP[log_project]
        logger = Colored(log_project)
        Colored.PROJECT_LOGGER_MAP[log_project] = logger
        return logger

    def __init__(self, log_project="default"):
        self.is_debug = True
        self.is_log_file = False
        self.log_project = log_project
        self.log_date = time.strftime("%Y%m%d%H%M%S", time.localtime())


    def start_log_file(self):
        self.is_log_file = True
        if not os.path.exists(Colored.LOG_DIR):
            os.mkdir(Colored.LOG_DIR)

        project_log_dir = self.get_fuzz_project_log_dir()
        if not os.path.exists(project_log_dir):
            os.mkdir(project_log_dir)

        current_project_log_dir = self.get_fuzz_current_project_log_dir()
        if not os.path.exists(current_project_log_dir):
            os.mkdir(current_project_log_dir)


    def get_fuzz_project_log_dir(self):
        return os.path.join(Colored.LOG_DIR, self.log_project)

    def get_fuzz_current_project_log_dir(self):
        return os.path.join(Colored.LOG_DIR, self.log_project, self.log_date)

    def get_fuzz_current_project_log_path(self):
        return os.path.join(self.get_fuzz_current_project_log_dir(), "run.log")

    def loghook(self, msg):
        if self.is_log_file:
            run_log = os.path.join(
                self.get_fuzz_current_project_log_dir(),
                "run.log"
            )
            with open(run_log, 'ab') as f:
                f.write(msg+"\n")


    def color_str(self, color, s, tag=None):
        msg = ""
        if tag:
            msg = '{}{}{}{}'.format(
                getattr(Colored, color),
                tag,
                s,
                Colored.RESET
            )
        else:
            msg =  '{}{}{}'.format(
                getattr(Colored, color),
                s,
                Colored.RESET
            )
        self.loghook(msg)
        return msg

    def red(self, s):
        print(self.color_str('RED', s, "[ERROR] "))


    def green(self, s):
        if self.is_debug:
            print(self.color_str('GREEN', s, "[INFO] "))


    def yellow(self, s):
        print(self.color_str('YELLOW', s, "[WARNING] "))


    def blue(self, s):
        return self.color_str('BLUE', s)

    def fuchsia(self, s):
        return self.color_str('FUCHSIA', s)

    def cyan(s):
        return self.color_str('CYAN', s)

    def white(self, s):
        print(self.color_str('WHITE', s))


    def simple_print(self, s):
        self.loghook(s)
        print(s)


    @staticmethod
    def get_fuzz_log_dir():
        return Colored.LOG_DIR

    @staticmethod
    def log_task_init(project):
        Colored.LOG_TO_FILE = True
        Colored.LOG_PROJECT = project
        Colored.LOG_DATE = time.strftime("%Y%m%d%H%M%S", time.localtime())

        if not os.path.exists(Colored.LOG_DIR):
            os.mkdir(Colored.LOG_DIR)

        project_log_dir = Colored.get_fuzz_project_log_dir()
        if not os.path.exists(project_log_dir):
            os.mkdir(project_log_dir)

        current_project_log_dir = Colored.get_fuzz_current_project_log_dir()
        if not os.path.exists(current_project_log_dir):
            os.mkdir(current_project_log_dir)
