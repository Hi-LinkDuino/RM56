#!/usr/bin/env python
# -*- coding: utf-8 -*-

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

import os
import importlib
try:
    from queue import Queue
except ImportError:
    from Queue import Queue
from collections import defaultdict

from hb_internal.common.utils import read_json_file


def get_style(style_type):
    style = importlib.import_module('prompt_toolkit.styles')
    token = importlib.import_module('prompt_toolkit.token')
    if style_type == 'terminal':
        return style.style_from_dict({
            token.Token.Separator: '#75c951',
            token.Token.QuestionMark: '#5F819D',
            token.Token.Selected: '',  # default
            token.Token.Pointer: '#FF9D00 bold',  # AWS orange
            token.Token.Instruction: '',  # default
            token.Token.Answer: '#FF9D00 bold',  # AWS orange
            token.Token.Question: 'bold',
        })
    if style_type == 'answer':
        return style.style_from_dict({
            token.Token.Separator: '#75c951',
            token.Token.QuestionMark: '#E91E63 bold',
            token.Token.Selected: '#cc5454',  # default
            token.Token.Pointer: '#ed9164 bold',
            token.Token.Instruction: '',  # default
            token.Token.Answer: '#f44336 bold',
            token.Token.Question: '',
        })

    return None


def if_mousedown(handler):
    def handle_if_mouse_down(cli, mouse_event):
        mouse_events = importlib.import_module('prompt_toolkit.mouse_events')
        if mouse_event.event_type == mouse_events.MouseEventTypes.MOUSE_DOWN:
            return handler(cli, mouse_event)
        else:
            return NotImplemented

    return handle_if_mouse_down


def get_deps(platform_json):
    platform = read_json_file(platform_json)
    subsystem_dict = {}
    component_deps = defaultdict(list)
    component_targets = {}
    component_dirs = {}
    for subsystem in platform['subsystems']:
        subsystem_dict[subsystem['subsystem']] = []
        for component in subsystem['components']:
            cname = component['component']
            subsystem_dict[subsystem['subsystem']].append(cname)
            if 'components' in component['deps']:
                deps = component['deps']['components']
                if cname in deps:
                    deps.remove(cname)
            else:
                deps = []
            component_deps[cname] = deps
            component_targets[cname] = component['targets']
            component_dirs[cname] = [
                os.path.join(os.path.dirname(platform_json), os.pardir,
                             os.pardir, os.pardir, os.pardir, path)
                for path in component['dirs']
            ]

    return subsystem_dict, component_deps, component_targets, component_dirs


def select_node(node, selected, nodes_from, deps):
    queue = Queue()
    queue.put(node)
    nodes_from[node].append(node)

    while not queue.empty():
        now_node = queue.get()
        if now_node not in selected:
            selected.append(now_node)
        for dep in deps.get(now_node, []):
            if now_node != dep and dep not in selected:
                queue.put(dep)
            nodes_from[dep].append(node)


def deselect_node(node, selected, nodes_from, deps):
    queue = Queue()
    queue.put(node)
    node_list = []

    while not queue.empty():
        now_node = queue.get()
        for each_node in nodes_from[now_node]:
            queue.put(each_node)
        nodes_from[now_node].clear()
        if now_node in selected:
            selected.remove(now_node)
            node_list.append(now_node)

    [queue.put(n) for n in node_list]
    while not queue.empty():
        now_node = queue.get()
        for dep in deps.get(now_node, []):
            if dep not in selected:
                continue
            nodes_from[dep] = [n for n in nodes_from[dep] if n in selected]
            if not len(nodes_from[dep]):
                selected.remove(dep)
                queue.put(dep)


def get_deps_list(comp, deps):
    queue = Queue()
    visited = set()
    deps_list = [comp]
    queue.put(comp)

    while not queue.empty():
        node = queue.get()
        for index, dep_comp in enumerate(deps[node]):
            if dep_comp in visited:
                continue
            deps_list.append(dep_comp)
            queue.put(dep_comp)
            visited.add(dep_comp)

    return deps_list


def get_support_product(product_path):
    product_dict = defaultdict(list)
    for product in os.listdir(product_path):
        product_json = os.path.join(product_path, product)
        product_content = read_json_file(product_json)
        board = product_content.get('board')
        kernel = product_content.get('kernel')
        platform = "{}_{}".format(board, kernel)
        product_dict[platform].append(product.strip('.json'))

    return product_dict


def check_path(dep, path):
    dep = dep[:-1] if dep.endswith("/") else dep
    path = path[:-1] if path.endswith("/") else path

    if len(dep) > len(path):
        path_max = dep
        path_min = path
    else:
        path_max = path
        path_min = dep

    if path_min in path_max:
        path_sub = path_max.replace(path_min, "")
        if path_sub == "":
            return True
        if path_sub.startswith('/') or path_sub.startswith(':'):
            return True
    return False


class Separator(object):
    line = '-' * 15

    def __init__(self, line=None):
        if line:
            self.line = f'\n{line}'

    def __str__(self):
        return self.line
