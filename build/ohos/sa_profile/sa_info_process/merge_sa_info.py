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

import xml.etree.ElementTree as ET
import ntpath
import shutil
import tempfile
import os
import logging
import sys

sys.path.append(os.path.dirname(os.path.abspath(__file__)))
from sort_sa_by_bootphase import SARearrangement  # noqa E402
import sa_info_config_errors as sa_err  # noqa E402


class SAInfoMerger(object):
    INDENT_SPACES = ' ' * 4

    class SAInfoCollector(object):
        """
        Class for collecting sa info pieces shared with same process name
        """
        def __init__(self, process_name, wdir):
            self.process_name = process_name
            self.loadlibs = []
            self.systemabilities = []
            self.wdir = wdir

        @property
        def output_filename(self):
            basename = self.process_name + '.xml'
            return os.path.join(self.wdir, basename)

        def add_libpath_info(self, libpath):
            """
            A libpath shared by multiple SAs in a process should be added once
            """
            if libpath not in self.loadlibs:
                self.loadlibs.append(libpath)

        def add_systemability_info(self, systemability):
            self.systemabilities += systemability

        def merge_sa_info(self):
            """
            Write all pieces of sa info shared with same process to a new file
            """
            DECLARATION = '<?xml version="1.0" encoding="utf-8"?>\n'
            ROOT_OPEN_TAG = '<info>\n'
            ROOT_ClOSE_TAG = '</info>'
            # add declaration and root open tag
            xml_lines = [DECLARATION, ROOT_OPEN_TAG]
            # add process
            process_line = '{}<process>{}</process>\n'.format(
                SAInfoMerger.INDENT_SPACES, self.process_name)
            xml_lines.append(process_line)
            # add libpath
            xml_lines.append(SAInfoMerger.INDENT_SPACES + '<loadlibs>\n')
            xml_lines += list(self.loadlibs)
            xml_lines.append(SAInfoMerger.INDENT_SPACES + '</loadlibs>\n')
            # add systemability
            xml_lines += self.systemabilities
            # add root close tag
            xml_lines.append(ROOT_ClOSE_TAG)

            # write file to temporary directory
            with open(self.output_filename, 'w',
                      encoding='utf-8') as xml_files:
                for line in xml_lines:
                    xml_files.write(line)

    def __init__(self, is_64bit_arch):
        self.process_sas_dict = {}
        self.output_filelist = []
        self.is_64bit_arch = is_64bit_arch
        self.output_dir = None
        self.temp_dir = None
        self.sa_nodes_count = None

    def __add_to_output_filelist(self, infile):
        self.output_filelist.append(os.path.join(self.output_dir, infile))

    def __parse_xml_file(self, source_file):
        parser = ET.XMLParser()
        tree = ET.parse(source_file, parser)
        root = tree.getroot()

        # check root tag
        if root.tag == 'profile':
            _format = 'bad root <{}> tag, new format <info> is expected'
            # the <profile> is old format, and should not be used anymore
            raise sa_err.BadFormatXMLError(_format.format(root.tag),
                                           source_file)
        elif root.tag != 'info':
            # other profile files whose tag name don't equal to 'info' should
            # just left intact, e.g. <schedStrategies></schedStrategies>
            basename = ntpath.basename(source_file)
            dest_file = os.path.join(self.output_dir, basename)
            shutil.copyfile(source_file, dest_file)
            self.__add_to_output_filelist(basename)

            # emit a warning to let user know it if there exists a typo
            _logstr = '"{}" is not merged, for it\'s root tag is "{}"'.format(
                source_file, root.tag)
            logging.warning(_logstr)
            return

        _format = 'one and only one {} tag is expected, actually {} is found'
        # check process tag
        process_nodes = root.findall('process')
        process_nodes_count = len(process_nodes)
        if process_nodes_count != 1:
            raise sa_err.BadFormatXMLError(
                _format.format('<process>', process_nodes_count), source_file)
        else:
            # ensure that the value of <process> is valid
            process_name = process_nodes[0].text
            if process_name is None or process_name.strip() == '':
                raise sa_err.BadFormatXMLError(
                    'provide a valid value for <process>', source_file)
            process_name = process_name.strip()
            if self.process_sas_dict.get(process_name) is None:
                # create a new collector if a new process tag is found
                sa_info_collector = self.SAInfoCollector(
                    process_name, self.temp_dir)
                self.process_sas_dict[process_name] = sa_info_collector
                self.__add_to_output_filelist(process_name + '.xml')
            else:
                sa_info_collector = self.process_sas_dict[process_name]

        # check libpath tag
        libpath_nodes = root.findall('systemability/libpath')
        libpath_nodes_count = len(libpath_nodes)
        if libpath_nodes_count != 1:
            raise sa_err.BadFormatXMLError(
                _format.format('<libpath>', libpath_nodes_count), source_file)
        else:
            libpath = libpath_nodes[0].text.strip()
            libname = ntpath.basename(libpath)
            # [Temporary scheme] no additional process for 64-bit arch and
            # a libpath without prefixed directory
            if not self.is_64bit_arch and libname != libpath:
                libpath = os.path.join("/system/lib", libname)
                libpath_nodes[0].text = libpath
            reconstructed_str = '<libpath>{}</libpath>\n'.format(libpath)
            # fix weird indent problem after converting the node to string
            string_repr = self.INDENT_SPACES * 2 + reconstructed_str
            sa_info_collector.add_libpath_info(string_repr)

        # check systemability tag
        systemability_nodes = root.findall('systemability')
        sa_nodes_count = len(systemability_nodes)
        self.sa_nodes_count = sa_nodes_count
        if sa_nodes_count != 1:
            raise sa_err.BadFormatXMLError(
                _format.format('<systemability>', sa_nodes_count), source_file)
        else:
            byte_repr = ET.tostring(systemability_nodes[0], encoding='utf-8')
            # fix weird indent problem after converting the node to string
            string_repr = self.INDENT_SPACES + byte_repr.decode('utf-8')
            # fix newline problem if the </systemability> has tailling comment
            fixed_string_repr = string_repr.rstrip() + '\n'
            sa_info_collector.add_systemability_info([fixed_string_repr])

    def __merge(self, sa_info_filelist, output_dir):
        """
        Iterate process_sas_dict and call it's merge_sa_info method to
        merge systemability info by process name
        """
        self.output_dir = output_dir

        # collect systemability info by process
        for source_file in sa_info_filelist:
            self.__parse_xml_file(source_file)

        global_ordered_systemability_names = []
        global_systemability_deps_dict = {}
        # merge systemability info for each process
        for process, collector in self.process_sas_dict.items():
            rearragement = SARearrangement()
            # do the merge
            collector.merge_sa_info()
            # sort sa by bootphase and denpendency
            merged_file = collector.output_filename
            dest_file = os.path.join(output_dir, ntpath.basename(merged_file))
            rearragement.sort(merged_file, dest_file)
            # get deps info for later detecting globally circular
            # dependency use
            deps_info = rearragement.get_deps_info()
            global_ordered_systemability_names += deps_info[0]
            global_systemability_deps_dict.update(deps_info[1])

        # detect possible cross-process circular dependency
        try:
            SARearrangement.detect_invalid_dependency_globally(
                global_ordered_systemability_names,
                global_systemability_deps_dict)
        except sa_err.CircularDependencyError as e:
            for _file in self.output_filelist:
                try:
                    os.remove(_file)
                except OSError:
                    pass
            raise sa_err.CrossProcessCircularDependencyError(e)

        # finally return an output filelist
        return self.output_filelist

    def merge(self, sa_info_filelist, output_dir):
        with tempfile.TemporaryDirectory(dir='./') as temp_dir:
            self.temp_dir = temp_dir
            return self.__merge(sa_info_filelist, output_dir)
