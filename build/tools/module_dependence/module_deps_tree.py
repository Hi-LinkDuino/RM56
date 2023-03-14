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

import os
import json
import sys
import argparse
from pyecharts import options as opts
from pyecharts.charts import Tree
from pyecharts.globals import CurrentConfig


def read_json_file(input_file):
    if not os.path.exists(input_file):
        print("file '{}' doesn't exist.".format(input_file))
        return None

    data = None
    try:
        with open(input_file, 'r') as input_f:
            data = json.load(input_f)
    except json.decoder.JSONDecodeError:
        print("The file '{}' format is incorrect.".format(input_file))
        raise
    return data


class ModuleTree:
    def __init__(self, data, inner_kits_adapter):
        self.data = data
        self.inner_kits_adapter = inner_kits_adapter
        self.isroot = True
        self.module_count = {}

    def parse_part_module_name(self, node_name):
        split_result = node_name.split(':')
        part_name = split_result[0]
        module_name = split_result[1]
        return part_name, module_name

    def get_label_color(self, node_name):
        if self.module_count.get(node_name, 'no_module') == 'no_module':
            self.module_count[node_name] = 1
            label_opt = opts.LabelOpts(color='#000000',
                                       font_style='normal',
                                       font_family='Times New Roman',
                                       font_size=16)
        else:
            self.module_count[node_name] += 1
            label_opt = opts.LabelOpts(color='#ff0000',
                                       font_style='normal',
                                       font_family='Times New Roman',
                                       font_size=16)
        return label_opt

    def node_tree(self, node_name):
        if node_name not in self.data:
            part_name, module_name = self.parse_part_module_name(node_name)
            new_part_name = self.inner_kits_adapter.get(
                part_name, "no_adapter")
            old_node_name = node_name
            if new_part_name != "no_adapter":
                node_name = "{}:{}".format(new_part_name, module_name)
            if node_name not in self.data:
                if self.isroot:
                    raise Exception(
                        "module '{}' doesn't exist.".format(old_node_name))
                else:
                    print("Waring! module '{}' doesn't exist.".format(
                        old_node_name))
                    tree_data = {"name": node_name}
                    return tree_data

        self.isroot = False
        module_deps = self.data.get(node_name)
        if len(module_deps) > 0:
            children = []
            for module_dep in module_deps:
                children.append(self.node_tree(module_dep))
            tree_data = {"name": node_name, "children": children}
        else:
            tree_data = {"name": node_name}
        return tree_data

    def color_tree(self, tree_data):
        color_tree_data = {}
        queue = [tree_data]
        while len(queue) > 0:
            node = queue.pop(0)
            label_opt = self.get_label_color(node["name"])
            node["label_opt"] = label_opt
            children = node.get("children", "no_children")
            if children != "no_children":
                queue.extend(children)
        return tree_data

    def graph_tree(self, tree_data):
        children = tree_data.get("children", "no_children")
        if children != "no_children":
            for index, value in enumerate(children):
                children[index] = self.graph_tree(children[index])
            return opts.TreeItem(name=tree_data["name"],
                                 label_opts=tree_data["label_opt"],
                                 children=children)
        else:
            return opts.TreeItem(name=tree_data["name"],
                                 label_opts=tree_data["label_opt"])

    def get_node_tree(self, node_name):
        self.isroot = True
        tree_data = self.node_tree(node_name)
        self.module_count = {}
        tree_data = self.color_tree(tree_data)
        tree = self.graph_tree(tree_data)
        return tree


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--module-name', required=True)
    parser.add_argument('--module-deps-file', required=True)
    args = parser.parse_args(argv)

    node_name = args.module_name
    inner_kits_adapter_file = os.path.join(
        os.path.dirname(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__)))),
        "ohos", "inner_kits_adapter.json")
    output_file = os.path.join(os.path.dirname(args.module_deps_file),
                               "{}.html".format(node_name.replace(":", "__")))
    tree_title = "{} module dependency tree".format(node_name)
    CurrentConfig.ONLINE_HOST = ("https://cdn.jsdelivr.net/npm/"
                                 "echarts@latest/dist/")

    print("------Generate module dependency tree------")
    module_deps = read_json_file(args.module_deps_file)
    inner_kits_adapter = read_json_file(inner_kits_adapter_file)

    module_tree_ = ModuleTree(module_deps, inner_kits_adapter)
    tree_data = module_tree_.get_node_tree(node_name)

    tree = (Tree(opts.InitOpts(width="1920px", height="1080px")).add(
        "", [tree_data],
        orient="LR",
        initial_tree_depth=1,
        is_roam=True,
        symbol_size=10).set_global_opts(title_opts=opts.TitleOpts(
            title=tree_title)).render(output_file))
    print('module deps tree output to {}'.format(output_file))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
