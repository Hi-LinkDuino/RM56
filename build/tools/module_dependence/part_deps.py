#!/usr/bin/env python3
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

import sys
import os
import argparse

import file_utils
import dependence_analysis


def gen_part_dependence(deps_data):
    part_allowlist = ['unittest', 'moduletest', 'systemtest']
    label_to_alias = {}
    for _module_alias, _info in deps_data.items():
        _module_label = _info.get('module_label').split('(')[0]
        label_to_alias[_module_label] = _module_alias

    part_deps_data = {}
    for _module_alias, _info in deps_data.items():
        deps_part_list = []
        _part_name = _info.get('part_name')
        if _part_name in part_allowlist:
            continue
        _deps_label_list = _info.get('deps')
        for _deps_label in _deps_label_list:
            _alias = label_to_alias.get(_deps_label)
            if _alias is None:
                continue
            _dep_part_name = _alias.split(':')[0]
            if _dep_part_name == _part_name:
                continue
            deps_part_list.append(_dep_part_name)
        _external_deps_list = _info.get('external_deps')
        for _ext_deps in _external_deps_list:
            _dep_part_name = _ext_deps.split(':')[0]
            if _dep_part_name == _part_name:
                continue
            deps_part_list.append(_dep_part_name)

        deps_value = part_deps_data.get(_part_name, [])
        deps_value.extend(deps_part_list)
        part_deps_data[_part_name] = list(set(deps_value))
    return part_deps_data


def _drawing_part_deps(part_deps_data, output_path):
    from pyecharts.charts import Graph
    from pyecharts import options as opts
    from pyecharts.globals import CurrentConfig

    CurrentConfig.ONLINE_HOST = "https://cdn.jsdelivr.net/npm"\
                                "/echarts@latest/dist/"

    part_allowlist = ['unittest', 'moduletest', 'systemtest']
    part_nodes = []
    parts_links = []
    nodes_sets = set()
    for _part_name, _dep_parts in part_deps_data.items():
        if _part_name in part_allowlist:
            continue
        if _part_name not in nodes_sets:
            nodes_sets.add(_part_name)
            part_nodes.append(opts.GraphNode(
                name=_part_name,
                symbol='circle',
                symbol_size=20,
                label_opts=opts.LabelOpts(
                    font_style='normal',
                    font_family='Times New Roman',
                    font_size=16
                )
            ))
        for _dep_part in _dep_parts:
            if _part_name in part_deps_data.get(_dep_part, []):
                parts_links.append(opts.GraphLink(
                    source=_part_name,
                    target=_dep_part,
                    linestyle_opts=opts.LineStyleOpts(
                        color='#ff0000', width=0.5)
                ))
            else:
                parts_links.append(opts.GraphLink(
                    source=_part_name,
                    target=_dep_part,
                    linestyle_opts=opts.LineStyleOpts(
                        color='#000000', width=0.5)
                ))
    _output_graph_file = os.path.join(output_path, 'part-deps-grahp.html')
    graph = (
        Graph(opts.InitOpts(width="1920px", height="1080px"))
        .add(
            "",
            part_nodes,
            parts_links,
            repulsion=800,
            edge_symbol=['', 'arrow'],
            edge_symbol_size=6
        )
        .set_global_opts(title_opts=opts.TitleOpts(title="part-deps-grahp"))
        .render(_output_graph_file)
    )


def run(deps_files_path, output_path, is_graph):
    all_deps_data = dependence_analysis.get_all_deps_data(deps_files_path)
    all_deps_data_file = os.path.join(output_path, 'all_deps_data.json')
    file_utils.write_json_file(all_deps_data_file, all_deps_data)

    part_deps_data = gen_part_dependence(all_deps_data)
    part_deps_data_file = os.path.join(output_path, 'part_deps_info.json')
    file_utils.write_json_file(part_deps_data_file, part_deps_data)
    if is_graph:
        _drawing_part_deps(part_deps_data, output_path)


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--deps-files-path', required=True)
    parser.add_argument('--graph', action='store_true')
    parser.set_defaults(graph=False)
    args = parser.parse_args(argv)

    if not os.path.exists(args.deps_files_path):
        raise Exception("'{}' doesn't exist.".format(args.deps_files_path))
    output_path = os.path.join(os.path.dirname(
        args.deps_files_path), 'part_deps_info')
    print("------Generate part dependency info------")
    run(args.deps_files_path, output_path, args.graph)
    print('part deps data output to {}'.format(output_path))


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
