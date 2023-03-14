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
import re
import sys
import copy
from enum import Enum

import sa_info_config_errors as sa_err


class RearrangementPolicy(object):
    BOOT_START_PHASE = "BootStartPhase"
    CORE_START_PHASE = "CoreStartPhase"
    DEFAULT_START_PHASE = "OthersStartPhase"

    rearrange_category_order = (BOOT_START_PHASE, CORE_START_PHASE,
                                DEFAULT_START_PHASE)

    bootphase_priority_table = {
        BOOT_START_PHASE: 3,
        CORE_START_PHASE: 2,
        DEFAULT_START_PHASE: 1
    }

    def __init__(self):
        self.bootphase_categories = {
            RearrangementPolicy.BOOT_START_PHASE: [],
            RearrangementPolicy.CORE_START_PHASE: [],
            RearrangementPolicy.DEFAULT_START_PHASE: []
        }


class SARearrangement(object):
    def __init__(self):
        self.rearranged_systemabilities = []
        self.ordered_systemability_names = []
        self.name_node_dict = {}
        self.systemability_deps_dict = {}
        self.bootphase_dict = {}
        self.creation_dict = {}
        self.policy = RearrangementPolicy()
        self.sanode_start_idx = 0
        self.systemability_nodes = []
        self.xml_lines = []
        self.xml_lines_range = {}
        self.sa_nodes_count = 0
        self.first_sanode_start_lineno = 0
        self.last_sanode_end_lineno = 0
        self.file_in_process = None
        self.root = None

    def __read_xml_file_into_lines(self, source_file):
        """
        Read xml file into lines, and map systemability node into ranges with
        begin line and end line
        """
        class Status(Enum):
            expect_sa_tag_begin = 1
            expect_name_value = 2
            expect_sa_tag_end = 3

        def find_systemability_name(line):
            regexp = r"^[\t\s]*<name>(.*)</name>"
            matches = re.search(regexp, line)
            if matches is not None:
                name = matches.group(1).strip()
                return name
            return ""

        with open(source_file, "r", encoding="utf-8") as xml_file:
            self.xml_lines = xml_file.readlines()

        sa_tag_begin = r"^[\t\s]*\<systemability"
        sa_tag_end = r"^[\t\s]*\<\/systemability"
        status = Status.expect_sa_tag_begin
        ranges = []
        is_first_sa_node = True
        systemability_name = ""
        sa_nodes_count = 0
        for linenum in range(len(self.xml_lines)):
            line = self.xml_lines[linenum]
            if status == Status.expect_sa_tag_begin:
                if re.match(sa_tag_begin, line) is not None:
                    status = Status.expect_name_value
                    ranges.append(linenum)
                    if is_first_sa_node:
                        self.first_sanode_start_lineno = linenum
                        is_first_sa_node = False
            elif status == Status.expect_name_value:
                systemability_name = find_systemability_name(line)
                if systemability_name != "":
                    status = Status.expect_sa_tag_end
            elif status == Status.expect_sa_tag_end:
                if re.match(sa_tag_end, line) is not None:
                    ranges.append(linenum)
                    self.xml_lines_range[systemability_name] = ranges
                    self.last_sanode_end_lineno = linenum
                    status = Status.expect_sa_tag_begin
                    systemability_name = ""
                    ranges = []
                    sa_nodes_count += 1
            else:
                pass  # ignore other tags
        if self.sa_nodes_count != sa_nodes_count:
            _format = (
                "The <systemability> tag and it's child tags should "
                "start at a newline, And should not have stuffs before!\n")
            raise sa_err.BadFormatXMLError(_format, source_file)

    def __parse_xml_file(self, source_file):
        parser = ET.XMLParser()
        tree = ET.parse(source_file, parser)
        self.root = tree.getroot()
        self.systemability_nodes = self.root.findall("systemability")

        try:
            first_sa_node = self.systemability_nodes[0]
            self.sa_nodes_count = len(self.systemability_nodes)
            children_nodes = list(self.root.iter())
            self.sanode_start_idx = children_nodes.index(first_sa_node)
        except IndexError:
            # throw a BadFormatXMLError or just pass over if
            # no <systemability> tag found in xml file
            pass

    def __rearrange_systemability_node_nonstrict(self, dest_file):
        """
        Rearrange systemability nodes in this way will change the original
        format of source file
        """
        # remove old systemability nodes
        for systemability_node in self.systemability_nodes:
            self.root.remove(systemability_node)

        # insert the rearranged systemability nodes
        for idx in range(len(self.rearranged_systemabilities)):
            name = self.rearranged_systemabilities[idx]
            self.root.insert(self.sanode_start_idx + idx,
                             self.name_node_dict[name])
        tree = ET.ElementTree(self.root)
        tree.write(dest_file, encoding="utf-8", xml_declaration=True)

    def __rearrange_systemability_node_strict(self, dest_file):
        """
        Rearrange systemability nodes in this way preserve the original
        format of source file
        """
        if self.first_sanode_start_lineno != 0:
            rearranged_lines = self.xml_lines[:self.first_sanode_start_lineno]
            for name in self.rearranged_systemabilities:
                ranges = self.xml_lines_range[name]
                rearranged_lines += self.xml_lines[ranges[0]:ranges[1] + 1]
            rearranged_lines += self.xml_lines[self.last_sanode_end_lineno +
                                               1:]
        else:
            rearranged_lines = self.xml_lines

        with open(dest_file, "w", encoding="utf-8") as xml_files:
            for line in rearranged_lines:
                xml_files.write(line)

    def __sort_systemability_by_bootphase_priority(self):
        def inner_category_sort(systemabilities):
            """
            Sort dependencies with same bootphase category, preserve the
            original order in source file
            """
            systemabilities_ = systemabilities[:]
            for systemability in systemabilities_:
                dependencies = self.systemability_deps_dict[systemability]
                for dependency in dependencies:
                    # should update idx_self each iteration
                    idx_self = systemabilities.index(systemability)
                    try:
                        idx_dep = systemabilities.index(dependency)
                        # if the dependency is behind, then exchange the order
                        if idx_self < idx_dep:
                            tmp = systemabilities[idx_dep]
                            systemabilities[idx_dep] = systemabilities[
                                idx_self]
                            systemabilities[idx_self] = tmp
                    except ValueError:
                        pass  # ignore different category of dependencies

        # put the systemability nodes into different categories
        for systemability_name in self.ordered_systemability_names:
            bootphase = self.bootphase_dict.get(systemability_name)
            salist = self.policy.bootphase_categories.get(bootphase)
            salist.append(systemability_name)

        # sort the systemability nodes according to RearrangementPolicy
        for category in RearrangementPolicy.rearrange_category_order:
            salist = self.policy.bootphase_categories.get(category)
            inner_category_sort(salist)
            self.rearranged_systemabilities += salist

    def __detect_invert_dependency(self, systemability, depend):
        """
        Detect invert dependency: systemability with high boot priority depends
        on systemability with low ones, e.g. a systemability named 'sa1' with
        BootStartPhase priority depends on a systemability named 'sa2' with
        CoreStartPhase
        """
        _format = ("Bad dependency found: the {} with high priority "
                   "depends on a {} with low one")
        self_idx = self.bootphase_dict.get(systemability)
        # The depend may be in other process
        dep_idx = self.bootphase_dict.get(depend)
        if dep_idx is None:
            return
        self_priority = RearrangementPolicy.bootphase_priority_table.get(
            self_idx)
        depend_priority = RearrangementPolicy.bootphase_priority_table.get(
            dep_idx)
        if self_priority > depend_priority:
            raise sa_err.InvertDependencyError(
                _format.format(systemability, depend))

    def __detect_creation_dependency(self, systemability, depend):
        """
        Detect dependency related to configuration on <run-on-create>:
        if a sa with <run-on-create> set to 'true' depending on a sa
        with 'false', then a RunOnCreateDependencyError will be thrown
        """
        _format = ("Bad dependency found: the {} with run-on-create "
                   "depends on a {} with run-on-demand")
        self_creation = self.creation_dict.get(systemability)
        dep_creation = self.creation_dict.get(depend)
        if self_creation == "true" and dep_creation == "false":
            raise sa_err.RunOnCreateDependencyError(
                _format.format(systemability, depend))

    @classmethod
    def __detect_invalid_dependency(cls, dependency_checkers, ordered_sa_names,
                                    sa_deps_dict):
        """
        Iterate over the dependency tree, to detect whether there
        exists circular dependency and other kinds bad dependency
        """
        deps_visit_cnt = {}
        ordered_systemability_names = ordered_sa_names
        systemability_deps_dict = sa_deps_dict

        def init_dependencies_visit_counter():
            for name in ordered_systemability_names:
                deps_visit_cnt[name] = 0

        def do_check(systemability, dependency):
            """
            Check other kind dependency problem
            """
            for checker in dependency_checkers:
                checker(systemability, dependency)

        init_dependencies_visit_counter()
        for systemability in ordered_systemability_names:
            depend_path = []
            depend_path.append(systemability)
            while len(depend_path) != 0:
                cur_systemability = depend_path[-1]
                # the cur_systemability may be in a different process,
                # thus can't find it's dependency info
                dependencies = systemability_deps_dict.get(cur_systemability)
                if dependencies is None:
                    dependencies = []
                deps_count = len(dependencies)
                if deps_count == 0:
                    depend_path.pop()
                else:
                    if deps_visit_cnt[cur_systemability] < deps_count:
                        index = deps_visit_cnt[cur_systemability]
                        cur_dependency = dependencies[index]
                        # execute other kind dependency checkers right here
                        do_check(cur_systemability, cur_dependency)
                        try:
                            depend_path.index(cur_dependency)
                            depend_path.append(cur_dependency)
                            _format = "A circular dependency found: {}"
                            route = "->".join(map(str, depend_path))
                            raise sa_err.CircularDependencyError(
                                _format.format(route))
                        except ValueError:
                            depend_path.append(cur_dependency)
                            deps_visit_cnt[cur_systemability] += 1
                    else:
                        # pop the systemability in process if it's all
                        # dependencies have been visited
                        depend_path.pop()

    def __extract_info_from_systemability_nodes(self):
        """
        Extract info like dependencies and bootphase from a systemability node
        """
        def validate_bootphase(bootphase, nodename):
            _format = (
                "In systemability: {}, The bootphase '{}' is not supported "
                "please check yourself")
            if self.policy.bootphase_categories.get(bootphase) is None:
                raise sa_err.NotSupportedBootphaseError(
                    _format.format(nodename, bootphase))

        def validate_creation(creation):
            _format = ("In tag <{}> only a boolean value is expected, "
                       "but actually is '{}'")
            if creation not in ["true", "false"]:
                raise sa_err.BadFormatXMLError(
                    _format.format("run-on-create", creation),
                    self.file_in_process)

        def validate_systemability_name(nodename):
            if not nodename.isdigit() or nodename.startswith("0"):
                _format = ("<name>'s value should be non-zeros leading "
                           "digits, but actually is {}")
                raise sa_err.BadFormatXMLError(_format.format(nodename),
                                               self.file_in_process)

        def check_nodes_constraints(systemability_node, tag, ranges):
            """
            The number of a given node should be in a valid range
            """
            _format = ("The tag <{}> should be in range {},"
                       " but actually {} is found")
            tags_nodes = systemability_node.findall(tag)
            node_cnt = len(tags_nodes)
            if node_cnt < ranges[0] or node_cnt > ranges[1]:
                raise sa_err.BadFormatXMLError(
                    _format.format(tag, ranges, node_cnt),
                    self.file_in_process)
            return tags_nodes

        def strip_node_value(tag, name):
            """
            Check empty or None tag value
            """
            _format = ("The tag <{}>'s value cannot be empty, "
                       "but actually is {}")
            if tag.text is None or tag.text.strip() == '':
                raise sa_err.BadFormatXMLError(_format.format(name, tag.text),
                                               self.file_in_process)
            return tag.text.strip()

        default_bootphase = RearrangementPolicy.DEFAULT_START_PHASE
        for systemability_node in self.systemability_nodes:
            # Required <name> one and only one is expected
            name_node = check_nodes_constraints(systemability_node, "name",
                                                (1, 1))[0]
            nodename = strip_node_value(name_node, "name")
            validate_systemability_name(nodename)

            try:
                self.ordered_systemability_names.index(nodename)
                raise sa_err.SystemAbilityNameConflictError(nodename)
            except ValueError:
                self.ordered_systemability_names.append(nodename)
            self.name_node_dict[nodename] = copy.deepcopy(systemability_node)
            self.systemability_deps_dict[nodename] = []
            self.bootphase_dict[nodename] = default_bootphase

            # Optional <bootphase> zero or one are both accepted
            bootphase_nodes = check_nodes_constraints(systemability_node,
                                                      "bootphase", (0, 1))
            if len(bootphase_nodes) == 1:
                bootphase_value = strip_node_value(bootphase_nodes[0],
                                                   "bootphase")
                validate_bootphase(bootphase_value, nodename)
                self.bootphase_dict[nodename] = bootphase_value

            # Required <run-on-create> one and only one is expected
            runoncreate_node = check_nodes_constraints(systemability_node,
                                                       "run-on-create",
                                                       (1, 1))[0]
            runoncreate_value = strip_node_value(runoncreate_node,
                                                 "run-on-create")
            validate_creation(runoncreate_value)
            self.creation_dict[nodename] = runoncreate_value

            # Optional <depend>
            depend_nodes = check_nodes_constraints(systemability_node,
                                                   "depend", (0, sys.maxsize))
            for depend_node in depend_nodes:
                depend_value = strip_node_value(depend_node, "depend")
                deps = self.systemability_deps_dict[nodename]
                deps.append(depend_value)

    def sort(self, source_file, dest_file):
        self.file_in_process = source_file
        dependency_checkers = []
        dependency_checkers.append(self.__detect_invert_dependency)
        dependency_checkers.append(self.__detect_creation_dependency)

        self.__parse_xml_file(source_file)
        self.__extract_info_from_systemability_nodes()
        self.__read_xml_file_into_lines(source_file)
        self.__detect_invalid_dependency(dependency_checkers,
                                         self.ordered_systemability_names,
                                         self.systemability_deps_dict)
        self.__sort_systemability_by_bootphase_priority()
        self.__rearrange_systemability_node_strict(dest_file)

    @classmethod
    def detect_invalid_dependency_globally(cls,
                                           global_ordered_systemability_names,
                                           global_systemability_deps_dict):
        dependency_checkers = []
        cls.__detect_invalid_dependency(dependency_checkers,
                                        global_ordered_systemability_names,
                                        global_systemability_deps_dict)

    def get_deps_info(self):
        """
        Returns systemabilities and their dependencies for later detecting
        possible globally circular dependency problem
        """
        return [self.ordered_systemability_names, self.systemability_deps_dict]
