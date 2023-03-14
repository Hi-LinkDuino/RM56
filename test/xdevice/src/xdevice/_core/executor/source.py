#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
import json
import copy
from collections import namedtuple

from _core.constants import DeviceTestType
from _core.constants import ModeType
from _core.constants import HostDrivenTestType
from _core.exception import ParamError
from _core.logger import platform_logger
from _core.utils import get_filename_extension
from _core.utils import is_config_str
from _core.utils import unique_id


__all__ = ["TestSetSource", "TestSource", "find_test_descriptors",
           "find_testdict_descriptors", "TestDictSource"]

TestSetSource = namedtuple('TestSetSource', 'set')
TestSource = namedtuple('TestSource', 'source_file source_string config_file '
                                      'test_name test_type module_name')

TEST_TYPE_DICT = {"HAP": DeviceTestType.hap_test,
                  "PYT": HostDrivenTestType.device_test,
                  "JST": DeviceTestType.jsunit_test,
                  "CXX": DeviceTestType.cpp_test,
                  "BIN": DeviceTestType.lite_cpp_test}
EXT_TYPE_DICT = {".hap": DeviceTestType.hap_test,
                 ".py": HostDrivenTestType.device_test,
                 ".js": DeviceTestType.jsunit_test,
                 ".bin": DeviceTestType.lite_cpp_test,
                 "default": DeviceTestType.cpp_test}
PY_SUFFIX = ".py"
PYD_SUFFIX = ".pyd"
MODULE_CONFIG_SUFFIX = ".json"
MAX_DIR_DEPTH = 6
LOG = platform_logger("TestSource")


def find_test_descriptors(config):
    if not config.testfile and not config.testlist and not config.task and \
            not config.testcase:
        return None

    # get test sources
    testcases_dirs = _get_testcases_dirs(config)
    test_sources = _get_test_sources(config, testcases_dirs)
    LOG.debug("test sources: %s", test_sources)

    # normalize test sources
    test_sources = _normalize_test_sources(testcases_dirs, test_sources,
                                           config)

    # make test descriptors
    test_descriptors = _make_test_descriptors_from_testsources(test_sources,
                                                               config)
    return test_descriptors


def _get_testcases_dirs(config):
    from xdevice import Variables
    # add config.testcases_path and its subfolders
    testcases_dirs = []
    if getattr(config, "testcases_path", ""):
        testcases_dirs = [config.testcases_path]
        _append_subfolders(config.testcases_path, testcases_dirs)

    # add inner testcases dir and its subfolders
    inner_testcases_dir = os.path.abspath(os.path.join(
        Variables.top_dir, "testcases"))
    if getattr(config, "testcases_path", "") and os.path.normcase(
            config.testcases_path) != os.path.normcase(inner_testcases_dir):
        testcases_dirs.append(inner_testcases_dir)
        _append_subfolders(inner_testcases_dir, testcases_dirs)

    # add execution dir and top dir
    testcases_dirs.append(Variables.exec_dir)
    if os.path.normcase(Variables.exec_dir) != os.path.normcase(
            Variables.top_dir):
        testcases_dirs.append(Variables.top_dir)

    LOG.debug("testcases directories: %s", testcases_dirs)
    return testcases_dirs


def _append_subfolders(testcases_path, testcases_dirs):
    for root, dirs, _ in os.walk(testcases_path):
        for sub_dir in dirs:
            testcases_dirs.append(os.path.abspath(os.path.join(root, sub_dir)))


def find_testdict_descriptors(config):
    from xdevice import Variables
    if getattr(config, "testdict", "") == "":
        return None
    testdict = config.testdict
    test_descriptors = []
    for test_type_key, files in testdict.items():
        for file_name in files:
            if not os.path.isabs(file_name):
                file_name = os.path.join(Variables.exec_dir, file_name)
            if os.path.isfile(file_name) and test_type_key in \
                    TestDictSource.test_type.keys():
                desc = _make_test_descriptor(os.path.abspath(file_name),
                                             test_type_key)
                if desc is not None:
                    test_descriptors.append(desc)
    if not test_descriptors:
        raise ParamError("test source is none", error_no="00110")
    return test_descriptors


def _get_test_sources(config, testcases_dirs):
    test_sources = []

    # get test sources from testcases_dirs
    if not config.testfile and not config.testlist and not config.testcase \
            and config.task:
        for testcases_dir in testcases_dirs:
            _append_module_test_source(testcases_dir, test_sources)
        return test_sources

    # get test sources from config.testlist
    if getattr(config, "testlist", ""):
        for test_source in config.testlist.split(";"):
            if test_source.strip():
                test_sources.append(test_source.strip())
        return test_sources

    # get test sources from config.testfile
    if getattr(config, "testfile", ""):
        test_file = _get_test_file(config, testcases_dirs)
        import stat
        flags = os.O_RDONLY
        modes = stat.S_IWUSR | stat.S_IRUSR
        with os.fdopen(os.open(test_file, flags, modes), "r") as file_content:
            for line in file_content:
                if line.strip():
                    test_sources.append(line.strip())

    # get test sources from config.testcase
    if getattr(config, "testcase", ""):
        for test_source in config.testcase.split(";"):
            if test_source.strip():
                test_sources.append(test_source.strip())
        return test_sources
    return test_sources


def _append_module_test_source(testcases_path, test_sources):
    if not os.path.isdir(testcases_path):
        return
    for item in os.listdir(testcases_path):
        item_path = os.path.join(testcases_path, item)
        if os.path.isfile(item_path) and item_path.endswith(
                MODULE_CONFIG_SUFFIX):
            test_sources.append(item_path)


def _get_test_file(config, testcases_dirs):
    if os.path.isabs(config.testfile):
        if os.path.exists(config.testfile):
            return config.testfile
        else:
            raise ParamError("test file '%s' not exists" % config.testfile,
                             error_no="00110")

    for testcases_dir in testcases_dirs:
        test_file = os.path.join(testcases_dir, config.testfile)
        if os.path.exists(test_file):
            return test_file

    raise ParamError("test file '%s' not exists" % config.testfile)


def _normalize_test_sources(testcases_dirs, test_sources, config):
    norm_test_sources = []
    for test_source in test_sources:
        append_result = False
        for testcases_dir in testcases_dirs:
            # append test source absolute path
            append_result = _append_norm_test_source(
                norm_test_sources, test_source, testcases_dir, config)
            if append_result:
                break

        # append test source if no corresponding file founded
        if not append_result:
            norm_test_sources.append(test_source)
    if not norm_test_sources:
        raise ParamError("test source not found")
    return norm_test_sources


def _append_norm_test_source(norm_test_sources, test_source, testcases_dir,
                             config):
    # get norm_test_source
    norm_test_source = test_source
    if not os.path.isabs(test_source):
        norm_test_source = os.path.abspath(
            os.path.join(testcases_dir, test_source))

    # find py or pyd for test case input
    if config.testcase and not config.testlist:
        if os.path.isfile("%s%s" % (norm_test_source, PY_SUFFIX)):
            norm_test_sources.append(
                "%s%s" % (norm_test_source, PY_SUFFIX))
            return True
        elif os.path.isfile("%s%s" % (norm_test_source, PYD_SUFFIX)):
            norm_test_sources.append(
                "%s%s" % (norm_test_source, PYD_SUFFIX))
            return True
        return False

    # append to norm_test_sources
    if os.path.isfile(norm_test_source):
        norm_test_sources.append(norm_test_source)
        return True
    elif os.path.isfile("%s%s" % (norm_test_source, MODULE_CONFIG_SUFFIX)):
        norm_test_sources.append("%s%s" % (norm_test_source,
                                           MODULE_CONFIG_SUFFIX))
        return True
    return False


def _make_test_descriptor(file_path, test_type_key):
    from _core.executor.request import Descriptor
    if test_type_key is None:
        return None

    # get params
    filename, _ = get_filename_extension(file_path)
    uid = unique_id("TestSource", filename)
    test_type = TestDictSource.test_type[test_type_key]
    config_file = _get_config_file(
        os.path.join(os.path.dirname(file_path), filename))

    module_name = _parse_module_name(config_file, filename)
    # make test descriptor
    desc = Descriptor(uuid=uid, name=filename,
                      source=TestSource(file_path, "", config_file, filename,
                                        test_type, module_name))
    return desc


def _get_test_driver(test_source):
    try:
        from _core.testkit.json_parser import JsonParser
        json_config = JsonParser(test_source)
        return json_config.get_driver_type()
    except ParamError as error:
        LOG.error(error, error_no=error.error_no)
        return ""


def _make_test_descriptors_from_testsources(test_sources, config):
    test_descriptors = []

    for test_source in test_sources:
        filename, ext = test_source.split()[0], "str"
        if os.path.isfile(test_source):
            filename, ext = get_filename_extension(test_source)

        test_driver = config.testdriver
        if is_config_str(test_source):
            test_driver = _get_test_driver(test_source)

        # get params
        config_file = _get_config_file(
            os.path.join(os.path.dirname(test_source), filename), ext, config)
        test_type = _get_test_type(config_file, test_driver, ext)

        desc = _create_descriptor(config_file, filename, test_source,
                                  test_type, config)
        if desc:
            test_descriptors.append(desc)

    return test_descriptors


def _create_descriptor(config_file, filename, test_source, test_type, config):
    from xdevice import Scheduler
    from _core.executor.request import Descriptor

    error_message = ""
    if not test_type:
        error_message = "no driver to execute '%s'" % test_source
        LOG.error(error_message, error_no="00112")
        if Scheduler.mode != ModeType.decc:
            return None

    # create Descriptor
    uid = unique_id("TestSource", filename)
    module_name = _parse_module_name(config_file, filename)
    desc = Descriptor(uuid=uid, name=filename,
                      source=TestSource(test_source, "", config_file,
                                        filename, test_type, module_name))
    if not os.path.isfile(test_source):
        if is_config_str(test_source):
            desc = Descriptor(uuid=uid, name=filename,
                              source=TestSource("", test_source, config_file,
                                                filename, test_type,
                                                module_name))
        else:
            if config.testcase and not config.testlist:
                error_message = "test case '%s' or '%s' not exists" % (
                        "%s%s" % (test_source, PY_SUFFIX), "%s%s" % (
                            test_source, PYD_SUFFIX))
                error_no = "00103"
            else:
                error_message = "test source '%s' or '%s' not exists" % (
                    test_source, "%s%s" % (test_source, MODULE_CONFIG_SUFFIX))
                error_no = "00102"
            if Scheduler.mode != ModeType.decc:
                raise ParamError(error_message, error_no=error_no)

    if Scheduler.mode == ModeType.decc and error_message:
        Scheduler.report_not_executed(config.report_path, [("", desc)],
                                      error_message)
        return None

    return desc


def _get_config_file(filename, ext=None, config=None):
    config_file = None
    if os.path.exists("%s%s" % (filename, MODULE_CONFIG_SUFFIX)):
        config_file = "%s%s" % (filename, MODULE_CONFIG_SUFFIX)
        return config_file
    if ext and os.path.exists("%s%s%s" % (filename, ext,
                                          MODULE_CONFIG_SUFFIX)):
        config_file = "%s%s%s" % (filename, ext, MODULE_CONFIG_SUFFIX)
        return config_file
    if config and getattr(config, "testcase", "") and not getattr(
            config, "testlist"):
        return _get_testcase_config_file(filename)

    return config_file


def _get_testcase_config_file(filename):
    depth = 1
    dirname = os.path.dirname(filename)
    while dirname and depth < MAX_DIR_DEPTH:
        for item in os.listdir(dirname):
            item_path = os.path.join(dirname, item)
            if os.path.isfile(item_path) and item.endswith(
                    MODULE_CONFIG_SUFFIX):
                return item_path
        depth += 1
        dirname = os.path.dirname(dirname)
    return None


def _get_test_type(config_file, test_driver, ext):
    if test_driver:
        return test_driver

    if config_file:
        if not os.path.exists(config_file):
            LOG.error("config file '%s' not exists" % config_file,
                      error_no="00110")
            return ""
        return _get_test_driver(config_file)
    if ext in [".py", ".js", ".dex", ".hap", ".bin"] \
            and ext in TestDictSource.exe_type.keys():
        test_type = TestDictSource.exe_type[ext]
    elif ext in TestDictSource.exe_type.keys():
        test_type = DeviceTestType.hap_test
    else:
        test_type = DeviceTestType.cpp_test
    return test_type


def _parse_module_name(config_file, file_name):
    if config_file:
        return get_filename_extension(config_file)[0]
    else:
        if "{" in file_name:
            return "report"
        return file_name


class TestDictSource:
    exe_type = dict()
    test_type = dict()

    @classmethod
    def reset(cls):
        cls.test_type = copy.deepcopy(TEST_TYPE_DICT)
        cls.exe_type = copy.deepcopy(EXT_TYPE_DICT)
