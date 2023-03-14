#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
import json
import gzip
import shutil
import argparse

KFILESIGNATURE = "# ninja log v5\n"


class StoringDataLine(object):
    def __init__(self, start, end):
        self.start = int(start)
        self.end = int(end)
        self.target_obj_names = []

    def __str__(self):
        return "{} {} {} ".format(self.start, self.end, self.target_obj_names)


class NinjaToTrace(object):
    def __init__(self):
        self.datalist = list()
        self.durations = list()

    def parse_file(self, filename, ninja_start_time):
        # ensure file exist
        if not os.path.exists(filename):
            print("file: {} not exists".format(filename))
            return False
        storing_data = {}
        with open(filename, mode='r') as f:
            firstline = f.readline()
            if firstline != KFILESIGNATURE:
                print("unrecognized ninja log format, we need {}".format(
                    KFILESIGNATURE))

            for _, line in enumerate(f.readlines()):
                start, end, time_stamp, name, cmdhash = line.strip().split(
                    '\t')
                if time_stamp < ninja_start_time:
                    continue
                storing_data.setdefault(cmdhash, StoringDataLine(start, end))
                storing_data.get(cmdhash).target_obj_names.append(name)

        self.datalist = sorted(storing_data.values(),
                               key=lambda line: line.start)
        self.durations = sorted(storing_data.values(),
                                key=lambda line: line.end - line.start,
                                reverse=True)
        return True

    def save_durations(self, duration_file):
        total_time = 0
        with open(duration_file, 'w') as file:
            for item in self.durations:
                duration = item.end - item.start
                total_time += duration
                file.write('{}: {}\n'.format(item.target_obj_names[0],
                                             duration))
            file.write('total time: {} ms'.format(total_time))

    def trans_to_trace_json(self, dest_file_name):
        counter = CountingTheTid()
        tracelist = list()
        for storingdataline in self.datalist:
            tracelist.append({
                'name': '%0s' % ', '.join(storingdataline.target_obj_names),
                'cat': 'targets',
                'ph': 'X',
                'ts': str(storingdataline.start * 1000),
                'dur': str((storingdataline.end - storingdataline.start) * 1000),
                'pid': str(0),
                'tid': str(counter.counting_the_new_tid(storingdataline)),
                'args': {},
            })

        if not dest_file_name.endswith('.gz'):
            dest_file_name = dest_file_name + '.gz'

        if os.path.exists(dest_file_name):
            shutil.move(
                dest_file_name, '%s/build.trace.%d.gz' %
                                (os.path.dirname(dest_file_name),
                                 int(os.stat(dest_file_name).st_mtime)))

        with gzip.open(dest_file_name, "wt") as f:
            json.dump(tracelist, f)


class CountingTheTid(object):
    def __init__(self):
        self.tids = []  # store the tid's end time

    def counting_the_new_tid(self, storingdataline):
        for i in range(len(self.tids)):
            if self.tids[i] <= storingdataline.start:
                self.tids[i] = storingdataline.end
                return i  # renew the endtime and return the current tid

        # for the end time is newer than all tids so we need a new one
        self.tids.append(storingdataline.end)
        return len(self.tids) - 1  # the last index of the tids


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--ninja-log', help='path to ninja log')
    parser.add_argument('--trace-file', help='path to build trace file')
    parser.add_argument('--duration-file', help='path to duration file')
    parser.add_argument(
        '--ninja-start-time',
        help='epoch time of "Starting ninja ..." in nanoseconds')

    options = parser.parse_args()
    myparser = NinjaToTrace()
    if not myparser.parse_file(options.ninja_log, options.ninja_start_time):
        print("parse file fail")
        return

    myparser.trans_to_trace_json(options.trace_file)
    myparser.save_durations(options.duration_file)


if __name__ == '__main__':
    sys.exit(main())
