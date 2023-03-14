#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Copyright (c) 2021 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import os
import fnmatch
import sys
import argparse
import json
import platform
import subprocess

import distutils.dir_util as dir_util
import distutils.file_util as file_util
from distutils.errors import DistutilsError


# all sub system list, must be lowercase.
_SUB_SYSTEM_LIST = [
    "kernel",
    "hiviewdfx",
    "communication",
    "security",
    "update",
    "sstsutils",
    "utils",
    "multimedia",
    "hdf",
    "aafwk",
    "appexecfwk", 
    "distributed_schedule",
    "startup",
    "sensors",
    "sample",
    "iot_hardware",
    "open_posix_testsuite",
    "graphic",
    "ace",
    "applications",
    "ai",
    "global",
    "telephony",
    "dcts"
]

_NO_FILTE_SUB_SYSTEM_LIST = [
    "applications",
    "kernel",
    "open_posix_testsuite",
    "sample",
    "telephony",
    "dcts",
    "hiviewdfx",
    "security",
    "update",
    "sstsutils",
    "hdf",
    "distributed_schedule"
]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--method_name', help='', required=True)
    parser.add_argument('--arguments', help='',
                        required=True)  # format key=value#key=value
    args = parser.parse_args()
    this_module = sys.modules[__name__]
    method = getattr(this_module, args.method_name)
    arguments = {}
    for argument in args.arguments.split("#"):
        key_value = argument.strip().split("=")
        if len(key_value) != 2:
            raise ValueError(
                "The arguments' format is 'key=value#key=value'. Wrong format:"
                " " + argument)
        arguments.setdefault(key_value[0].strip(), key_value[1].strip())
    method(**arguments)
    return 0


def read_file(input_file):
    if not os.path.exists(input_file):
        return ""

    with open(input_file, 'r') as input_f:
        content = input_f.read().strip()
        return content


def write_file(output_file, content, append):
    file_dir = os.path.dirname(os.path.abspath(output_file))
    if not os.path.exists(file_dir):
        os.makedirs(file_dir)
    mode = 'a+' if append else 'w'
    with open(output_file, mode) as output_f:
        output_f.write("%s\n" % content)


def copy_file(output, sources="", source_dirs="", to_dir=True):
    """
    copy source files or source dir to output.
    if sources is not empty, the output can be file(will be created
    automatically)
    or directory(must be exist).
    :param output: If source_dirs is not empty, output must be directory.
    :param sources: source files is separated by dot
    :param source_dirs: source directory is separated by dot
    :param to_dir: output is directory or not
    :return:
    """
    if not sources and not source_dirs:
        raise Exception(
            "sources or source_dirs parameter must be specified one")
    _output = output.strip()
    _sources = sources.strip()
    _source_dirs = source_dirs.strip()
    _parent_output = os.path.dirname(_output)
    try:
        if to_dir and not os.path.exists(_output):
            os.makedirs(_output)
        if not to_dir and not os.path.exists(_parent_output):
            os.makedirs(_parent_output)
    except OSError:
        if not os.path.isdir(_output):
            raise
    try:
        if _sources:
            _copy_files(_sources.split(","), _output)

        if _source_dirs:
            _copy_dir(_source_dirs.split(","), _output)
    except DistutilsError:
        print("ignore file exist error")
    return 0


def _copy_files(sources, output):
    copy_set = set()
    for source_file in sources:
        source_file = source_file.strip()
        if os.path.isfile(source_file) and os.path.exists(source_file):
            # if same file name exist, add dir path
            if os.path.basename(source_file) in copy_set:
                new_output = os.path.join(output, os.path.dirname(source_file).
                                          split(os.sep)[-1])
                if not os.path.exists(new_output):
                    os.makedirs(new_output)
                file_util.copy_file(source_file, new_output)
            else:
                file_util.copy_file(source_file, output)
            copy_set.add(os.path.basename(source_file))


def _copy_dir(sources, output):
    for source_file in sources:
        source_file = source_file.strip()
        if os.path.isdir(source_file):
            dir_util.copy_tree(source_file, output)


def gen_suite_out(suite_output_prefix, suite_names, out_suffix):
    outputs = []
    _suite_output_prefix = suite_output_prefix.strip()
    _dirname_suffix = out_suffix.strip().rstrip(os.sep)
    for suite in suite_names.split(","):
        path = "%s%s/%s" % (
            _suite_output_prefix, suite.strip(), _dirname_suffix)
        outputs.append(path)
        print(path)
    return outputs


def get_subsystem_name(path):
    subsystem_name = ""
    for subsystem in _SUB_SYSTEM_LIST:
        subsystem_path = "/" + subsystem + "/"
        _path = path.lower()
        if subsystem_path in _path:
            subsystem_name = subsystem
            break
        subsystem_path = "/" + subsystem + "_lite/"
        if subsystem_path in _path:
            subsystem_name = subsystem
            break
    sys.stdout.write(subsystem_name)
    return subsystem_name


def get_modulename_by_buildtarget(module_list_file, build_target):
    if not os.path.exists(module_list_file):
        return ""
    with open(module_list_file, "r") as module_file:
        module_info_data = json.load(module_file)
    for module in module_info_data:
        if module_info_data[module]["build_target_name"] == build_target:
            sys.stdout.write(module)
            return module
    return ""


def glob(path, filename_pattern):
    files = []
    for dir_path, _, files in os.walk(path):
        for filename in fnmatch.filter(files, filename_pattern):
            files.append(os.path.join(dir_path, filename))
    return files


def filter_by_subsystem(testsuites, product_json):
    product_info = {}
    filtered_features = []
    subs_comps = {}
    # parses product json to obtain all the subsystem name
    if os.path.exists(product_json):
        try:
            with open(product_json, 'r') as product_info:
                product_info = json.load(product_info)
        except ValueError:
            print("NO json object could be decoded.")
        subsystem_info = product_info.get("subsystems")
        for subsystem in subsystem_info:
            subs_comps[subsystem.get("subsystem")] = \
                subsystem.get("components", [])

    feature_list = testsuites.split(",")
    for feature in feature_list:
        # if subsytem name match
        subsystem = get_subsystem_name_no_output(feature)
        if subsystem in _NO_FILTE_SUB_SYSTEM_LIST:
            filtered_features.append(feature)
            print(feature)
        elif subsystem in subs_comps:
            components = subs_comps.get(subsystem, [])
            if check_component(feature, components):
                filtered_features.append(feature)
                print(feature)
    return filtered_features


def get_subsystem_name_no_output(path):
    subsystem_name = ""
    for subsystem in _SUB_SYSTEM_LIST:
        subsystem_path = "/" + subsystem
        _path = path.lower()
        if subsystem_path in _path:
            subsystem_name = subsystem
            break
        subsystem_path = "/" + subsystem + "_lite"
        if subsystem_path in _path:
            subsystem_name = subsystem
            break
    return subsystem_name


def check_component(path, components):
    for component in components:
        component_name = component.get("component", "")
        component_name = component_name.replace("_lite", "")
        if component_name in path or "{}_hal".format(component_name) in path \
                 or "{}_posix".format(component_name) in path:
            return True
    return False


def get_python_cmd():
    major, _, _ = platform.python_version_tuple()
    if major == "3":
        return "python"
    else:
        return "python3"


def record_testmodule_info(build_target_name, module_name,
                           subsystem_name, suite_out_dir, same_file=False):
    if not build_target_name or not subsystem_name:
        print(
            'build_target_name or subsystem_name of testmodule "%s" '
            'is invalid!' % module_name)
        return
    if same_file:
        module_info_list_file = os.path.join(suite_out_dir, 'module_info.json')
    else:
        module_info_list_file = os.path.join(suite_out_dir,
                                             '{}_module_info.json'.format
                                             (build_target_name))
    module_info_data = {}
    if os.path.exists(module_info_list_file):
        try:
            with open(module_info_list_file, 'r') as module_file:
                module_info_data = json.load(module_file)
        except ValueError:
            print("NO json object could be decoded but continue")
    module_info = {'subsystem': subsystem_name,
                   'build_target_name': build_target_name}
    module_info_data[module_name] = module_info
    with open(module_info_list_file, 'w') as out_file:
        json.dump(module_info_data, out_file)


def record_test_component_info(out_dir, version):
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    all_module_file = os.path.join(out_dir, 'module_info.json')
    all_module_data = {}
    for root, dirs, files in os.walk(out_dir):
        for file in files:
            if file.endswith("module_info.json"):
                with open(os.path.join(root, file), 'r') as json_data:
                    module_data = json.load(json_data)
                    all_module_data.update(module_data)
                os.remove(os.path.join(root, file))
    with open(all_module_file, 'w') as out_file:
        json.dump(all_module_data, out_file)

    test_component_file = os.path.join(out_dir, 'test_component.json')
    test_component_data = {'version': version, }
    with open(test_component_file, 'w') as out_file:
        json.dump(test_component_data, out_file)


def get_target_modules(all_features):
    feature_list = []
    if all_features:
        for feature in all_features.split(","):
            if feature:
                feature_list.append(feature)
                print(feature)
    return feature_list


def cmd_popen(cmd):
    proc = subprocess.Popen(cmd)
    proc.wait()
    ret_code = proc.returncode
    if ret_code != 0:
        raise Exception("{} failed, return code is {}".format(cmd, ret_code))


def build_js_hap(project_path, out_put_dir, hap_name):
    if not check_env():
        return
    gradle_dir = os.path.join(project_path, "gradle")
    os.chdir(gradle_dir)
    build_clean = ["gradle", "clean"]
    cmd_popen(build_clean)
    build_cmd = ["gradle", "entry:packageDebugHap"]
    cmd_popen(build_cmd)
    gralde_output_dir = os.path.join(gradle_dir, "entry", "build", "outputs")
    if os.path.exists(gralde_output_dir):
        for root, _, files in os.walk(gralde_output_dir):
            for file in files:
                if file.endswith(".hap"):
                    file_util.copy_file(os.path.join(root, file),
                                        os.path.join(out_put_dir.rstrip(','),
                                                     hap_name))
                    return



def check_env():
    """
    check all the env for js hap build
    return: return true if all env ready, otherwise return false
    """
    env_list = ['OHOS_SDK_HOME', 'NODE_HOME', 'GRADLE_HOME']
    for env in env_list:
        if not os.environ.get(env):
            print("the env {} not set, skip build!".format(env))
            return False
    else:
        return True
if __name__ == '__main__':
    sys.exit(main())
