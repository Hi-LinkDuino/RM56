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
import sys
import argparse
import re
import subprocess
import xml.dom.minidom
from xml.parsers.expat import ExpatError
from string import Template
import utils
import json
import shutil
import glob
import stat


XTS_RESOURCE_ROOT = 'ivbxfj`qspqsjfubsz0sftpvsdf'


def _get_xts_rootpath(project_path):
    if project_path is not None:
        return project_path[0:project_path.find('/xts/') + len('/xts/')]
    raise ValueError('Illegal xts project path ' + project_path)


def _get_subsystem_name(project_path):
    if '/hits/' in  project_path:
        index0 = project_path.find('/hits/') + len('/hits/')
    elif '/acts/' in  project_path:
        index0 = project_path.find('/acts/') + len('/acts/')
    else:
        raise ValueError('Illegal xts project path ' + project_path)
    index1 = project_path.find('/', index0)
    return project_path[index0:index1]


def _get_resource_rootpath(project_path):
    xts_root = _get_xts_rootpath(project_path)
    resource_reldir = ''.join(chr(ord(ch) - 1) for ch in XTS_RESOURCE_ROOT)
    return os.path.join(xts_root, resource_reldir)


class XDeviceBuilder:
    """
    To build XTS as an egg package
    @arguments(project_dir, output_dirs)
    """

    def __init__(self, arguments):
        parser = argparse.ArgumentParser()
        parser.add_argument('--source_dir', help='', required=True)
        parser.add_argument('--configs_dir', help='', required=False)
        parser.add_argument('--resources_dir', help='', required=False)
        parser.add_argument('--suite_out_dir', help='', required=True)
        self.args = parser.parse_args(arguments)

    def build_xdevice(self):
        """
        build xdevice package
        :return:
        """
        extension_dir = os.path.join(self.args.source_dir, 'extension')
        gen_dir0 = os.path.join(self.args.source_dir, 'dist')
        gen_dir1 = os.path.join(extension_dir, 'dist')
        shutil.rmtree(gen_dir0, ignore_errors=True)
        shutil.rmtree(gen_dir1, ignore_errors=True)
        command0 = ["python", "setup.py", "sdist"]
        command1 = ["python", "setup.py", "sdist"]
        try:
            subprocess.check_call(command0, cwd=self.args.source_dir)
            subprocess.check_call(command1, cwd=extension_dir)
        except subprocess.CalledProcessError as exc:
            print('returncode: {} cmd: {} output: {}'.format(
                  exc.returncode, exc.cmd, exc.output))

        run_scripts = ",".join(
            [os.path.join(self.args.source_dir, "run.bat"),
             os.path.join(self.args.source_dir, "run.sh")])

        dist_tools_dir = os.path.join(self.args.suite_out_dir, 'tools')
        utils.copy_file(output=dist_tools_dir, source_dirs=gen_dir0,
                        to_dir=True)
        utils.copy_file(output=dist_tools_dir, source_dirs=gen_dir1,
                        to_dir=True)
        utils.copy_file(output=self.args.suite_out_dir, sources=run_scripts,
                        to_dir=True)

        if self.args.configs_dir:
            dist_configs_dir = os.path.join(self.args.suite_out_dir, 'config')
            utils.copy_file(output=dist_configs_dir, 
                            source_dirs=self.args.configs_dir, to_dir=True)
        if self.args.resources_dir:
            dist_resources_dir = os.path.join(self.args.suite_out_dir,
                                              'resource')
            utils.copy_file(output=dist_resources_dir, 
                            source_dirs=self.args.resources_dir, to_dir=True)


class SuiteArchiveBuilder:
    def __init__(self, arguments):
        self.module_info_dir = None
        self.arguments = arguments

    def archive_suite(self):
        parser = argparse.ArgumentParser()
        parser.add_argument('--suite_path', help='', required=True)
        parser.add_argument('--prebuilts_resource', help='', required=True)
        parser.add_argument('--suite_archive_dir', help='', required=True)
        parser.add_argument('--make_archive', help='', required=True)
        args = parser.parse_args(self.arguments)
    
        if not args.make_archive.lower() == 'true':
            print('make archive disabled')
            return 0
    
        suite_path = args.suite_path
        if not os.path.isdir(suite_path):
            raise Exception("[%s] does not exist" % suite_path)
    
        copyfiles = args.prebuilts_resource.split(",")
        for file_path in copyfiles:
            subprocess.call(["cp", "-rf", file_path, suite_path])
    
        archive_name = os.path.basename(suite_path)
        archive_root_path = args.suite_archive_dir
        if not os.path.isdir(archive_root_path):
            os.mkdir(archive_root_path)
        # remove the extra output of target "java_prebuilt"
        # such as ztest-tradefed-common.interface.jar
        subprocess.call(["find", suite_path, "-name", "*.interface.jar", 
                        "-exec", "rm", "{}", "+"])
        shutil.make_archive(os.path.join(archive_root_path, archive_name),
                            "zip", suite_path)
        return 0


class SuiteModuleWithTestbundleBuilder:

    def __init__(self, arguments):
        self.arguments = arguments
        self.args = None

    def build_module_with_testbundle(self):
        parser = argparse.ArgumentParser()
        parser.add_argument('--build_gen_dir', help='', required=True)
        parser.add_argument('--build_target_name', help='', required=True)
        parser.add_argument('--subsystem_name', help='', 
                            required=False)
        parser.add_argument('--buildgen_testfile', help='', required=True)
        parser.add_argument('--project_path', help='', required=True)
        parser.add_argument('--test_xml', help='', required=False)
        parser.add_argument('--project_type', help='', required=True)
        parser.add_argument('--suite_out_dir', help='', required=True)
        parser.add_argument('--archive_testfile', help='', required=True)
        parser.add_argument('--apilibrary_deps', help='', required=False)
        parser.add_argument('--test_files', help='', required=False)

        self.args = parser.parse_args(self.arguments)

        self._create_testsuite(self.args)
        return 0

    def _create_testsuite(self, args):
        _test_xml = args.test_xml
        _testcases_dir = os.path.dirname(args.archive_testfile)
        _testsuite_name = os.path.basename(args.archive_testfile)\
            .replace('.hap', '').replace('module_', '')
        _testcase_xml = os.path.join(_testcases_dir, _testsuite_name + ".xml")
        _config_file = os.path.join(_testcases_dir,
                                    _testsuite_name + ".config")
        _json_file = os.path.join(_testcases_dir, _testsuite_name + ".json")
      
        if args.project_type == "js_test_hap":
            self._generate_json_by_template(_test_xml.replace(".xml", ".json"),
                                            _testsuite_name, _json_file)
            dest_file = os.path.join(
                _testcases_dir, "{}.hap".format(_testsuite_name))            
            self._copy_file(args.buildgen_testfile, dest_file)
            os.chmod(dest_file, stat.S_IRWXU|stat.S_IRWXG|stat.S_IRWXO)
            return
        if args.project_type == "pythontest":
            self._generate_json_by_template(_test_xml.replace(".xml", ".json"),
                                            _testsuite_name, os.path.join(
                    args.archive_testfile, _testsuite_name + ".json"))
            utils.copy_file(output=self.args.archive_testfile,
                            source_dirs=self.args.test_files)
            return
        self._check_file_exist(args.buildgen_testfile)
        self._copy_file(args.buildgen_testfile, args.archive_testfile)
        if args.project_type == "app":
            return
        self._record_testmodule_info(args.build_target_name,
                                     _testsuite_name,
                                     _testcases_dir)
        if _test_xml and os.path.exists(_test_xml):
            self._copy_file(_test_xml, _config_file)
        elif _test_xml.replace(".xml", ".json") and \
                os.path.exists(_test_xml.replace(".xml", ".json")):
            self._generate_json_by_template(_test_xml.replace(".xml", ".json"),
                                            _testsuite_name, _json_file)
        else:
            self._generate_xml_by_template(_test_xml, _testsuite_name,
                                           _config_file)
        #self._generate_testcase_xml(args)
        _resource_srcroot = _get_resource_rootpath(args.project_path)
        self._archive_test_file_to_testcase(_testcases_dir)

    @staticmethod
    def _record_testmodule_info(build_target_name, module_name, testcases_dir):
        if not build_target_name or not module_name:
            raise ValueError(
                    'Ethire build_target_name or module_name is invalid')

        module_info_list_file = os.path.join(testcases_dir, 'module_info.list')
        lines = []
        if os.path.isfile(module_info_list_file):
            with open(module_info_list_file, 'r') as file_read:
                for line in file_read:
                    lines.append(line.strip())

        new_lines = ['%s %s' % (build_target_name, module_name)]
        for line in lines:
            arr = line.strip().split(' ')
            if len(arr) == 0 or arr[0] == build_target_name:
                continue
            new_lines.append(line)
    
        # add module info
        with open(module_info_list_file, 'w') as file_write:
            file_write.write('\n'.join(new_lines) + '\n')


    @staticmethod
    def _generate_json_by_template(source_file, module_name, dest_file):
        source_content = utils.read_file(source_file)
        values = {"module": module_name}
        xml_content = Template(source_content).substitute(values)
        utils.write_file(dest_file, xml_content, False)

    @staticmethod
    def _generate_xml_by_template(test_xml, module_name,
                                  config_file):
        # find the template file
        index = test_xml.rfind(".xml")
        tmpl_file = test_xml[:index] + ".tmpl"
        if not os.path.exists(tmpl_file):
            raise Exception("Can't find the Test.xml or "
                            "Test.tmpl in the path %s " %
                            os.path.dirname(test_xml))
        tmpl_content = utils.read_file(tmpl_file)
        values = {"module": module_name}
        xml_content = Template(tmpl_content).substitute(values)
        utils.write_file(config_file, xml_content, False)

    def _copy_file(self, source, target):
        if not os.path.exists(os.path.dirname(target)):
            os.makedirs(os.path.dirname(target))
        if not os.path.exists(target) or (
                os.path.exists(target) and
                (os.stat(target).st_mtime != os.stat(source).st_mtime)):
            print('Trying to copy "%s" to "%s"' % (source, target))
            subprocess.call(["rm", "-rf", target])
            subprocess.call(["cp", "-rf", source, target])

    def _copy_file_to_target(self, source_file, dest_file):
        if not os.path.exists(source_file):
            print("[ERROR] source_file is not exist. %s" % source_file,
                  file=sys.stderr)
            return
        else:
            self._copy_file(source_file, dest_file)

    def _archive_test_resource(self, xml_file, root_dir, resource_dir):
        _pattern = re.compile("[-=]>")
        if os.path.exists(xml_file):
            try:
                dom = xml.dom.minidom.parse(xml_file)
                childroot = dom.getElementsByTagName("target_preparer")
                for child in childroot:
                    for child_atrr in child.getElementsByTagName("option"):
                        name = child_atrr.getAttribute("name")
                        attr_value = child_atrr.getAttribute("value")
                        value = _pattern.split(attr_value)[0].strip()
                        if name != "" or value != "":
                            if name == "push" and \
                                    value.startswith("resource/"):
                                self._copy_file_to_target(
                                    os.path.join(root_dir, "../", value),
                                    os.path.join(resource_dir, value))
                        else:
                            raise Exception("Test.xml node name and "
                                            "value not is null")
            except IOError as error:
                print("IO Error: %s" % error, file=sys.stderr)
            except ExpatError as error:
                print("ExpatError Error: %s" % error, file=sys.stderr)
            finally:
                pass
        else:
            with open(xml_file.replace(".config", ".json"),
                      "r", encoding="UTF-8") as json_file:
                json_str = json.load(json_file)
                kits = json_str["kits"]
                for kit in kits:
                    types = kit["type"]
                    if types == "PushKit":
                        push_resources = kit["push"]
                        for resource in push_resources:
                            value = _pattern.split(resource)[0].strip()
                            if value.startswith("resource/"):
                                self._copy_file_to_target(
                                    os.path.join(root_dir, "../", value),
                                    os.path.join(resource_dir, value))

    def _archive_test_file_to_testcase(self, cases_dir):
        if self.args.test_files is None:
            return
        arr_test_files = self.args.test_files.split(',')
        for file in arr_test_files:
            if file == "":
                continue
            self._copy_file_to_target(file,
                                      os.path.join(cases_dir,
                                                   os.path.basename(file)))

    def _check_file_exist(self, file_path):
        if not os.path.exists(file_path):
            raise Exception("File [%s] does not exist!" % file_path)


ACTION_MAP = {"build_module": "SuiteModuleBuilder",
              "build_xdevice": "XDeviceBuilder",
              "archive_suite": "SuiteArchiveBuilder",
              "build_module_with_testbundle":
                  "SuiteModuleWithTestbundleBuilder"}


def _find_action(action, arguments):
    class_name = ACTION_MAP[action]
    if not class_name:
        raise ValueError('Unsupported operation: %s' % action)

    this_module = sys.modules[__name__]
    class_def = getattr(this_module, class_name, None)
    if not class_def:
        raise ValueError(
            'Unsupported operation(No Implementation Class): %s' % action)
    class_obj = class_def(arguments)
    func = getattr(class_obj, action, None)
    if not func:
        raise ValueError(
            'Unsupported operation(No Implementation Method): %s' % action)
    return func


def main(arguments):
    action = arguments[0]
    args = arguments[1:]
    func = _find_action(action, args)
    func()
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
