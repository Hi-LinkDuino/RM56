#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
import distutils.dir_util as dir_util
import distutils.file_util as file_util
import json
import subprocess

# all sub system list, must be lowercase.
_SUB_SYSTEM_LIST = [
    "kernel",
    "hiviewdfx",
    "communication",
    "security",
    "updater",
    "sstsutils",
    "utils",
    "uikit",
    "multimedia",
    "hdf",
    "appexecfwk",
    "distributedschedule",
    "startup",
    "sensors",
    "sample",
    "iothardware",
    "open_posix_testsuite",
]
_GRADLE_PARAMS = ["SIGN_USER_NAME", "SIGN_PWD", "SIGN_MATERIAL_PATH",
                 "SIGN_PROFILE", "SIGN_CERT_FILE"]

_DEFAULT_USER_NAME = "user"
_DEFAULT_PWD = "decrypt_pwd"


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
            raise ValueError("Wrong format : '%s' % argument")
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
    if _sources:
        _copy_files(_sources.split(","), _output)

    if _source_dirs:
        _copy_dir(_source_dirs.split(","), _output)

    return 0


def _copy_files(sources, output):
    for source_file in sources:
        source_file = source_file.strip()
        if os.path.isfile(source_file):
            file_util.copy_file(source_file, output)


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
        subsystem_path = "/%s/" % (subsystem)
        _path = path.lower()
        if subsystem_path in _path:
            subsystem_name = subsystem
            break
        subsystem_path = "/%s/_lite" % (subsystem)
        if subsystem_path in _path:
            subsystem_name = subsystem
            break
    sys.stdout.write(subsystem_name)
    return subsystem_name


def get_modulename_by_buildtarget(module_list_file, build_target):
    if not os.path.exists(module_list_file):
        return ""
    module_info_data = {}
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


def cmd_popen(cmd):
    proc = subprocess.Popen(cmd)
    proc.wait()
    ret_code = proc.returncode
    if ret_code != 0:
        raise Exception("{} failed, return code is {}".format(cmd, ret_code))


def build_js_hap(**kwargs):
    if not check_env():
        return
    if "project_path" not in kwargs or "out_put_dir" not in kwargs:
        return
    project_path = kwargs.get("project_path")
    gradle_dir = os.path.join(project_path, "gradle")
    os.chdir(gradle_dir)
    build_clean = ["gradle", "clean"]
    cmd_popen(build_clean)
    if "SIGN_USER_NAME" not in kwargs:
        pass

    gradle_parm_cmd = get_gradle_cmd(**kwargs)
    # build sign debug hap
    build_hap_cmd = ["gradle"]
    build_hap_cmd.extend(gradle_parm_cmd)
    build_hap_cmd.append("entry:aD")
    cmd_popen(build_hap_cmd)

    # build sign ohos test hap
    build_test_hap_cmd = ["gradle"]
    build_test_hap_cmd.extend(gradle_parm_cmd)
    build_test_hap_cmd.append("entry:aDOT")
    cmd_popen(build_test_hap_cmd)

    gradle_output_dir = os.path.join(gradle_dir, "entry", "build", "outputs")
    if not os.path.exists(gradle_output_dir):
        return
    out_put_dir = kwargs.get("out_put_dir")
    if not os.path.exists(out_put_dir):
        os.makedirs(out_put_dir)
    hap_name = kwargs.get("hap_name")
    for root, _, files in os.walk(gradle_output_dir):
        for file in files:
            if file.endswith(".hap") and "unsigned" not in file:
                if "debugOhosTest" in root:
                    target_file = "{}_OhosTest.hap".format(hap_name)
                else:
                    target_file = "{}.hap".format(hap_name)
                file_util.copy_file(os.path.join(root, file),
                                    os.path.join(out_put_dir.rstrip(','),
                                                 target_file))


def get_gradle_cmd(**kwargs):
    cmd = []
    if kwargs:
        kwargs.setdefault("SIGN_USER_NAME", _DEFAULT_USER_NAME)
        kwargs.setdefault("SIGN_PWD", _DEFAULT_PWD)
    for param in _GRADLE_PARAMS:
        if param in kwargs:
            cmd.append("-P{}={}".format(param, kwargs.get(param)))
    return cmd


def check_env():
    """
    check all the env for js hap build
    return: return true if all env ready, otherwise return false
    """
    env_list = ['OHOS_SDK_HOME']
    for env in env_list:
        if not os.environ.get(env):
            print("the env {} not set, skip build!".format(env))
            return False
    else:
        return True


if __name__ == '__main__':
    sys.exit(main())
