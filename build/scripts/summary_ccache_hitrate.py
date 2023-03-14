#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
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

import os
import sys
import subprocess


def summary_ccache_new(ccache_log):
    hit_dir_num = 0
    hit_pre_num = 0
    mis_num = 0
    hit_rate = 0
    mis_rate = 0
    hit_dir_str = "Result: cache hit (direct)"
    hit_pre_str = "Result: cache hit (preprocessed)"
    mis_str = "Result: cache miss"
    if os.path.exists(ccache_log):
        cmd = "grep -c \'{}\'  {}".format(hit_dir_str, ccache_log)
        hit_dir_num = int(
            subprocess.Popen(cmd, shell=True,
                             stdout=subprocess.PIPE).communicate()[0])
        cmd = "grep -c \'{}\'  {}".format(hit_pre_str, ccache_log)
        hit_pre_num = int(
            subprocess.Popen(cmd, shell=True,
                             stdout=subprocess.PIPE).communicate()[0])
        cmd = "grep -c \'{}\'  {}".format(mis_str, ccache_log)
        mis_num = int(
            subprocess.Popen(cmd, shell=True,
                             stdout=subprocess.PIPE).communicate()[0])
    sum_ccache = hit_dir_num + hit_pre_num + mis_num
    if sum_ccache != 0:
        hit_rate = (float(hit_dir_num) +
                    float(hit_pre_num)) / float(sum_ccache)
        mis_rate = float(mis_num) / float(sum_ccache)
    return hit_rate, mis_rate, hit_dir_num, hit_pre_num, mis_num


def main():
    if len(sys.argv) < 2:
        print("Error, please input the ccache log file path.")
        exit(-1)

    ccache_log = sys.argv[1]
    hit_rate = 0
    mis_rate = 0
    hit_dir_num = 0
    hit_pre_num = 0
    mis_num = 0
    if os.path.exists(ccache_log):
        hit_rate, mis_rate, hit_dir_num, hit_pre_num, mis_num = summary_ccache_new(
            ccache_log)

    print("---------------------------------------------")
    print("ccache summary:")
    print("cache hit (direct)  : " + str(hit_dir_num))
    print("cache hit (preprocessed)  : " + str(hit_pre_num))
    print("cache miss  : " + str(mis_num))
    print("hit rate:  %.2f%% " % (hit_rate * 100))
    print("mis rate: %.2f%% " % (mis_rate * 100))
    print("---------------------------------------------")


if __name__ == "__main__":
    main()
