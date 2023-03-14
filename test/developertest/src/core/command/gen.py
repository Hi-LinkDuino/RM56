#!/usr/bin/env python3
# coding=utf-8

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

import os
import sys
import subprocess

from xdevice import platform_logger

LOG = platform_logger("Gen")

class Gen(object):
    def process_command_gen(self, options):
        if (len(options.testtype) != 1) or (options.dirpath == "") or \
                (options.fuzzername == ""):
            LOG.error(
                "GEN need para -t(testtype) -fz(fuzzername) -dp(dirpath)")
            return
        if "FUZZ" in options.testtype:
            self.fuzz_dir_generation(options)
        else:
            LOG.error("GEN is not support %s." % options.testtype)

    def gen_fuzzer_list_file(self, fuzzer_list):
        filepath = os.path.join(sys.source_code_root_path, "test",
            "developertest", "libs", "fuzzlib", "fuzzer_list.txt")
        LOG.info("The fuzzer list file path: %s" % filepath)
        with open(filepath, "w") as gn_file:
            gn_file.truncate(0)
            if fuzzer_list:
                for target in fuzzer_list:
                    if target:
                        gn_file.write("\"%s\",\n" % target)

    @classmethod
    def fuzz_dir_generation(cls, options):
        helper_path = os.path.join("..", "libs", "fuzzlib", "fuzzer_helper.py")
        fuzz_path = os.path.join(sys.source_code_root_path, options.dirpath)
        LOG.info("fuzz_path = %s" % fuzz_path)
        if not os.path.exists(fuzz_path):
            os.makedirs(fuzz_path)
            LOG.info("make folder %s" % fuzz_path)

        command = [sys.executable, helper_path, 'generate',
                   options.fuzzername, fuzz_path]
        LOG.info("command %s" % command)
        subprocess.call(command, shell=False)