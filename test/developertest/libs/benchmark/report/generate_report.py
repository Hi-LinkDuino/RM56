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

import json
import os
import shutil
import sys

SETTING_RED_STYLE = """\033[33;31m%s\033[0m"""

def load_json_data(json_file_path):
    json_data = {}
    if os.path.isfile(json_file_path):
        try:
            with open(json_file_path, 'r') as file_read:
                json_data = json.load(file_read)
            if not json_data:
                print("Loading file \"%s\" error" % json_file_path)
                return {}
        except(IOError, ValueError) as err_msg:
            print("Error for load_json_data: \"%s\"" %
                     json_file_path, err_msg)
    else:
        print("Info: \"%s\" not exist." % json_file_path)
    return json_data


def get_file_list(find_path, postfix):
    file_names = os.listdir(find_path)
    file_list = []
    if len(file_names) > 0:
        for name in file_names:
            if name.find(postfix) != -1 and name[-len(postfix):] == postfix:
                file_list.append(name)
    return file_list


def get_file_list_by_postfix(path, postfix, filter_jar = ""):
    file_list = []
    for dirs in os.walk(path):
        files = get_file_list(find_path=dirs[0], postfix=postfix)
        for file_path in files:
            if "" != file_path and -1 == file_path.find(__file__):
                pos = file_path.rfind(os.sep)
                file_name = file_path[pos + 1:]
                file_path = os.path.join(dirs[0], file_path)
                if filter_jar != "" and file_name == filter_jar:
                    print(SETTING_RED_STYLE % ("Skipped %s" % file_path))
                    continue
                file_list.append(file_path)
    return file_list

class BenchmarkReport(object):
    SUBSYSTEM_SUMMARY = "OHOS_SUBSYSTEM_SUMMARY"
    ENABLE_LINK = "OHOS_ENABLE_PASSCASE_LINK"
    REPORT_SUMMARY = "OHOS_REPORT_SUMMARY"
    LEGEND_DATA = "OHOS_LEGEND_DATA"
    XAXIS_DATA = "OHOS_XAXIS_DATA"
    SERIES_DATA = "OHOS_TITLE_DATA"
    TITLE_TEXT = "OHOS_TITLE_TEST"
    YAXIS_FORMATTER = "OHOS_YAXIS_FORMATTER"


    def __init__(self):
        self.index = 0
        self.filtered = ["detail", "id", "pm", "owner",
                         "Count", "ScoreUnit", "Variance"]
        self.default_item = []
        self.max_index = 1000
        self.sbs_mdl_summary_list = []
        self.benchmark_list = []
        self._init_default_item()

    def _init_default_item(self):
        self.default_item.append("Subsystem")
        self.default_item.append("Module")
        self.default_item.append("Testsuit")
        self.default_item.append("Benchmark")
        self.default_item.append("Mode")
        self.default_item.append("RunType")
        self.default_item.append("TestTargetName")
        self.default_item.append("TestTargetMethod")
        self.default_item.append("Repetitions")
        self.default_item.append("RepetitionIndex")
        self.default_item.append("Threads")
        self.default_item.append("Iterations")
        self.default_item.append("Score")
        self.default_item.append("CpuTime")
        self.max_index = len(self.default_item) + 1000

    def generate_benchmark(self, args):
        if args is None or len(args) <= 2:
            print(SETTING_RED_STYLE %
                      "Error: source_dir and report_dir can't be empty")
            return

        src_path = sys.argv[1]
        dest_path = os.path.abspath(sys.argv[2])

        print("source_dir: %s" % src_path)
        print("report_dir: %s" % dest_path)

        if not os.path.exists(src_path):
            print("%s not exists" % src_path)
            return

        if os.path.exists(dest_path):
            shutil.rmtree(dest_path)

        self._get_benchmark_result_data(src_path)
        self._generate_benchmark_summary_report(os.path.abspath(dest_path))
        self._generate_all_benchmark_detail(os.path.abspath(dest_path))

    def _remove_iterations(self, mdl_summary_list):
        final_mdl_summary = []
        for item_info in mdl_summary_list:
            copy_item = item_info.copy()
            copy_item.pop("Iterations")
            final_mdl_summary.append(copy_item)
        return final_mdl_summary

    def _get_benchmark_result_data(self, src_path):
        self.benchmark_list = []
        self.sbs_mdl_summary_list = []
        system_summary_dic = {}
        json_files = get_file_list_by_postfix(src_path, ".json")
        print("json_files %s" % json_files)
        for json_file in json_files:
            pos = json_file.find(src_path)
            subsystem_root = json_file[pos + len(src_path):]
            dir_list = subsystem_root.split(os.sep)
            sbs_name = dir_list[1]
            module_name = dir_list[2]
            testsuit_name = dir_list[len(dir_list) - 2]

            print(SETTING_RED_STYLE % (
                "subsystem_root: %s \n\n"
                "subsystem_name: %s \n\n"
                "module_name: %s \n\n"
                "testsuit_name: %s \n\n" %
                (subsystem_root, str(sbs_name),
                 str(module_name), str(testsuit_name))))

            mdl_summary_list = self._get_subsystem_cxx_benchmark(sbs_name,
                module_name, testsuit_name, json_file)
            self.benchmark_list = mdl_summary_list

            if sbs_name in system_summary_dic.keys() \
                and testsuit_name in system_summary_dic[sbs_name].keys():
                subsystem_summary_dic = \
                    system_summary_dic[sbs_name][testsuit_name]
                subsystem_summary_dic["children"] += \
                    self._remove_iterations(mdl_summary_list)
            else:
                self.index += 1
                subsystem_summary_dic = dict()
                subsystem_summary_dic["id"] = self.index
                subsystem_summary_dic["Subsystem"] = sbs_name
                subsystem_summary_dic["Testsuit"] = testsuit_name
                subsystem_summary_dic["Module"] = "---"
                subsystem_summary_dic["Detail"] = ""
                subsystem_summary_dic["TestTargetName"] = "---"
                subsystem_summary_dic["TestTargetMethod"] = "---"
                subsystem_summary_dic["RunType"] = "---"
                subsystem_summary_dic["Benchmark"] = "---"
                subsystem_summary_dic["Mode"] = "---"
                subsystem_summary_dic["Count"] = "---"
                subsystem_summary_dic["Score"] = "---"
                subsystem_summary_dic["ScoreUnit"] = "---"
                subsystem_summary_dic["children"] = []
                subsystem_summary_dic["children"] += \
                    self._remove_iterations(mdl_summary_list)
                self.sbs_mdl_summary_list.append(subsystem_summary_dic)
                system_summary_dic[sbs_name] = {}
            system_summary_dic[sbs_name][testsuit_name] = \
                subsystem_summary_dic
            subsystem_summary_dic["pm"] = "unknown"
            subsystem_summary_dic["owner"] = "unknown"

    def _get_subsystem_cxx_benchmark(self, sbs_name, module_name,
                                     testsuit_name, json_file):
        sbs_mdl_summary_list = list()
        json_data_dic = load_json_data(json_file)
        for json_data in json_data_dic.get("benchmarks", []):
            self.index += 1
            sbs_mdl_summary = dict()
            sbs_mdl_summary["id"] = self.index
            sbs_mdl_summary["Subsystem"] = sbs_name
            sbs_mdl_summary["Module"] = module_name
            sbs_mdl_summary["Testsuit"] = testsuit_name
            sbs_mdl_summary["pm"] = "unknown"
            sbs_mdl_summary["owner"] = "unknown"

            benchmark_name = json_data.get("name", "").replace("/", "_"). \
                replace(":", "_")
            test_target = benchmark_name.split("_")[0]
            sbs_mdl_summary["TestTargetName"] = test_target
            sbs_mdl_summary["TestTargetMethod"] = "%s()" % test_target
            sbs_mdl_summary["RunType"] = str(json_data.get("run_type", ""))
            sbs_mdl_summary["Mode"] = \
                str(json_data.get("aggregate_name", "normal"))
            sbs_mdl_summary["Benchmark"] = benchmark_name
            sbs_mdl_summary["Repetitions"] = json_data.get("repetitions", 0)
            sbs_mdl_summary["RepetitionIndex"] = \
                json_data.get("repetition_index", 0)
            sbs_mdl_summary["Threads"] = json_data.get("threads", 0)
            sbs_mdl_summary["Iterations"] = json_data.get("iterations", 0)

            score_unit = json_data.get("time_unit", "")
            sbs_mdl_summary["ScoreUnit"] = score_unit
            sbs_mdl_summary["CpuTime"] = "%.2e %s " % (
                json_data.get("cpu_time", 0),
                score_unit
            )
            sbs_mdl_summary["Score"] = "%.2e %s " % (
                json_data.get("real_time", 0),
                score_unit
            )
            sbs_mdl_summary["detail"] = "Link"
            sbs_mdl_summary_list.append(sbs_mdl_summary)
        return sbs_mdl_summary_list

    def _generate_benchmark_summary_report(self, dest_dir_path):
        tmpl_file_path = os.path.abspath(os.path.join(
            os.path.dirname(__file__),
            "..", "template", "benchmark_summary.html"))
        if not os.path.exists(os.path.dirname(tmpl_file_path)):
            print(SETTING_RED_STYLE %
                     ("Warning: %s not exists" % tmpl_file_path))
            return

        out_report_file_path = os.path.join(dest_dir_path, "index.html")
        if not os.path.exists(os.path.dirname(out_report_file_path)):
            os.makedirs(os.path.dirname(out_report_file_path))

        if os.path.exists(tmpl_file_path):
            try:
                with open(os.path.abspath(tmpl_file_path), "r+") as file_read:
                    report_content = file_read.read()
                    file_read.close()
                    content_new = report_content

                    pos = content_new.find(BenchmarkReport.SUBSYSTEM_SUMMARY)
                    if pos >= 0:
                        content_new = \
                            content_new[0:pos] + \
                            str(self.sbs_mdl_summary_list) + \
                            content_new[pos +
                                        len(BenchmarkReport.SUBSYSTEM_SUMMARY):
                                        len(content_new)]

                    try:
                        with open(os.path.abspath(out_report_file_path), "w") \
                            as output_fd:
                            content_new = str(content_new)
                            output_fd.write(content_new)
                    except IOError as err_msg:
                        print("Error5 for open %s failed, with msg %s" %
                                  (out_report_file_path, err_msg))
            except IOError as err_msg:
                print("Error6 for open %s failed, with msg %s" %
                          (tmpl_file_path, err_msg))

    def _generate_all_benchmark_detail(self, dest_dir_parh):
        for benchmark_info in self.benchmark_list:
            self._generate_benchmark_detail(benchmark_info,
                                            os.path.abspath(dest_dir_parh))

    def _is_filtered_id(self, item_key):
        if item_key in self.filtered:
            return True
        return False

    def _get_index_id(self, item_key):
        pos = self.default_item.index(item_key)
        if pos != -1:
            return pos + 1
        else:
            self.max_index -= 1
            return self.max_index

    def _generate_benchmark_detail(self, benchmark_info, dest_dir_path):
        report_tmpl_file_path = os.path.abspath(
            os.path.join(os.path.dirname(__file__),
            "..", "template", "benchmark_detail.html"))
        if not os.path.exists(os.path.dirname(report_tmpl_file_path)):
            print(SETTING_RED_STYLE %
                      ("Warning: %s not exists" % report_tmpl_file_path))
            return

        out_report_file_path = os.path.join(os.path.abspath(dest_dir_path),
                                            str(benchmark_info["Subsystem"]),
                                            str(benchmark_info["Module"]),
                                            str(benchmark_info["Testsuit"]),
                                            str(benchmark_info["Benchmark"])
                                            + "_"
                                            + str(benchmark_info["Mode"])
                                            + "_detail.html")
        if not os.path.exists(os.path.dirname(out_report_file_path)):
            os.makedirs(os.path.dirname(out_report_file_path))

        detail_info = self._get_detail_info(benchmark_info)

        if os.path.exists(report_tmpl_file_path):
            try:
                with open(os.path.abspath(report_tmpl_file_path), "r+") \
                    as file_read:
                    report_content = file_read.read()
                    file_read.close()
                    content_new = report_content
                    content_new = \
                        self._update_report_summary(content_new, detail_info)

                    try:
                        with open(os.path.abspath(out_report_file_path), "w") \
                            as output_fd:
                            output_fd.write(content_new)
                    except IOError as err_msg:
                        print("Error5 for open %s failed, with msg %s" %
                                  (out_report_file_path, err_msg))
            except IOError as err_msg:
                print("Error6 for open %s failed, with msg %s" %
                          (report_tmpl_file_path, err_msg))

    def _get_detail_info(self, benchmark_info):
        detail_info = []
        self.max_index = 1000
        for item_key, item_value in benchmark_info.items():
            if self._is_filtered_id(item_key):
                continue

            item_info = {"item": item_key,
                         "id": self._get_index_id(item_key),
                         "content": item_value.decode("UTF-8")
                         if isinstance(item_value, bytes) else item_value}
            detail_info.append(item_info)
        detail_info = sorted(detail_info, key=lambda s: s["id"])
        dest_detail_info = []
        index = 1
        for item in detail_info:
            item["id"] = index
            dest_detail_info.append(item)
            index += 1
        return dest_detail_info

    def _update_report_summary(self, content_new, detail_info):
        pos = content_new.find(BenchmarkReport.REPORT_SUMMARY)
        if pos >= 0:
            content_new = \
                content_new[0:pos] + \
                str(detail_info) + \
                content_new[pos +
                            len(BenchmarkReport.REPORT_SUMMARY):
                            len(content_new)]
        return content_new

if __name__ == '__main__':
    print("****************** Benchmark Report Starting ******************")
    BenchmarkReport().generate_benchmark(sys.argv)
    print("****************** Benchmark Report Finished ******************")