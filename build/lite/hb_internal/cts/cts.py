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
#

import os
import json
import shutil
from subprocess import check_output
from subprocess import CalledProcessError
try:
    from queue import Queue
except ImportError:
    from Queue import Queue
from collections import defaultdict

from hb_internal import CONFIG_JSON
from hb_internal.common.utils import read_json_file
from hb_internal.cts.common import check_path
from hb_internal.common.utils import get_project_path
from hb_internal.common.utils import hb_info
from hb_internal.common.utils import hb_warning
from hb_internal.common.utils import OHOSException


class CTS():
    def __init__(self):
        self._board = None
        self._kernel = None
        self.kernel_of_board = defaultdict(list)
        self._set_path()
        self.fields = defaultdict(list)
        self.comp_fields = defaultdict(list)

    def __iter__(self):
        for subsystem_cls in self.subsystems:
            yield subsystem_cls

    def _set_path(self):
        self._code_path = get_project_path(CONFIG_JSON)
        if self._code_path is None:
            raise OHOSException('Please run command "hb set" to '
                                'init OHOS development environment')

        self._components_path = os.path.join(self._code_path,
                                             'build',
                                             'lite',
                                             'components')

        self._platform_path = os.path.join(self._code_path,
                                           'build',
                                           'lite',
                                           'platform')

        self._kernel_path = os.path.join(self._components_path,
                                         'kernel.json')

        self._product_path = os.path.join(self._code_path,
                                          'build',
                                          'lite',
                                          'product')

    @property
    def board(self):
        return self._board

    @board.setter
    def board(self, board):
        self._board = board
        self.update_special_deps()

    @property
    def kernel(self):
        return self._kernel

    @kernel.setter
    def kernel(self, kernel):
        self._kernel = kernel
        self.update_special_deps()

    @property
    def code_path(self):
        return self._code_path

    @property
    def components_path(self):
        return self._components_path

    @property
    def platform_path(self):
        return self._platform_path

    @property
    def kernel_path(self):
        return self._kernel_path

    @property
    def product_path(self):
        return self._product_path

    def init(self, board=None):
        if board is None:
            if self.board is None:
                raise OHOSException('no board selected')
        else:
            self.board = board

        self.init_from_json()

    def init_from_json(self):
        self.subsystems = []
        subsystem_list = os.listdir(self._components_path)
        subsystem_list.sort()

        for subsystem in subsystem_list:
            sname = subsystem.replace('.json', '')
            spath = os.path.join(self._components_path, subsystem)
            scontent = read_json_file(spath)
            subsystem_cls = Subsystem(sname, scontent, spath)
            self.subsystems.append(subsystem_cls)
            for cname, component in subsystem_cls:
                for fpath in component.dirs:
                    self.fields[fpath] = cname
                    self.comp_fields[cname].append(fpath)

    def update_special_deps(self):
        if self._board is None:
            return
        if self._kernel is None:
            return

        for subsystem_cls in self:
            for cname, component_cls in subsystem_cls:
                deps_board = component_cls.deps_board.get(self._board, [])
                deps_kernel = component_cls.deps_kernel.get(self._kernel, [])
                component_cls.deps_comp += list(set(deps_board + deps_kernel))

    def update_subsystems_product(self, nodes=None, board=None, kernel=None):
        if board is None:
            board = self.board
        if kernel is None:
            kernel = self.kernel

        subsystems_list = []
        comp_cls_dict = {}
        for subsystem_cls in self:
            subsystem_dict = {
                'subsystem': subsystem_cls.name,
                'components': []
            }
            for cname, component_cls in subsystem_cls:
                if not component_cls.is_board_in_comp(board):
                    continue
                if not component_cls.is_kernel_in_comp(kernel):
                    continue
                if nodes is not None and cname not in nodes:
                    continue

                comp_cls_dict[cname] = component_cls
                component_dict = {'component': component_cls.name,
                                  'features': component_cls.features}
                subsystem_dict['components'] += [component_dict]
            if len(subsystem_dict['components']):
                subsystems_list.append(subsystem_dict)
        self.update_comp_deps(comp_cls_dict)

        return subsystems_list

    def update_subsystems_platform(self, subsystem_list=None):
        for subsystem_cls in self:
            if len(subsystem_list) and \
                   subsystem_cls.name not in subsystem_list:
                continue
            subsystem_cls.update_json()

    @classmethod
    def update_comp_deps(cls, comp_cls_dict):
        for cname, comp_cls in comp_cls_dict.items():
            cls_deps_list = []
            for dep_comp_name in comp_cls.deps_comp:
                dep_comp_cls = comp_cls_dict.get(dep_comp_name, None)
                if dep_comp_cls is None:
                    pass
                cls_deps_list.append(dep_comp_cls)
            comp_cls.deps_comp = cls_deps_list


class Subsystem():
    def __init__(self, subsystem_name, subsystem_json, subsystem_json_path):
        self.name = subsystem_name
        self.comps = {}
        self.json_path = subsystem_json_path
        self.json_content = subsystem_json
        self._init_comps(subsystem_json)

    def __iter__(self):
        for cname, component_cls in self.comps.items():
            yield cname, component_cls

    def _init_comps(self, subsystem_json):
        component_list = subsystem_json.get('components')
        for component_json in component_list:
            cname = component_json.get('component')
            self.comps[cname] = (Component(cname, component_json))

    def update_json(self):
        component_list = self.json_content.get('components', [])
        for index, component in enumerate(component_list):
            cname = component.get('component', '')
            component_cls = self.comps.get(cname, None)
            component_list[index]['deps'] = component_cls.get_real_deps()

        self.json_content['components'] = component_list
        with open(self.json_path, 'wt', encoding='utf-8') as file:
            json.dump(self.json_content, file,
                      ensure_ascii=False, indent=2)


class Component():
    def __init__(self, component_name, component_json):
        self.name = component_name
        self.deps_comp = []
        self.deps_thirdparty = []
        self.copy_dirs = []
        self._init_comp(component_json)
        self.deps_dict = {}
        self.thirdparty_set = set()
        self.deps_real_dict = {}

    def _init_comp(self, component_json):
        self.dirs = component_json.get('dirs', [])
        self.targets = component_json.get('targets', [])
        self.adapted_board = component_json.get('adapted_board', [])
        self.adapted_kernel = component_json.get('adapted_kernel', [])
        self.features = component_json.get('features', None)

        deps = component_json.get('deps', {})
        self.deps_comp += deps.get('components', [])
        self.deps_thirdparty += deps.get('third_party', [])
        self.deps_board = deps.get('board_special', {})
        self.deps_kernel = deps.get('kernel_special', {})

    def is_dir_in_comp(self, path):
        if path in self.dirs:
            return True
        return False

    def is_board_in_comp(self, board):
        if not len(self.adapted_board):
            return True
        if board in self.adapted_board:
            return True

        return False

    def is_kernel_in_comp(self, kernel):
        if not len(self.adapted_kernel):
            return True
        if kernel in self.adapted_kernel:
            return True

        return False

    def remove_copy_dirs(self, work_path):
        os.chdir(work_path)
        for copy_dir in self.copy_dirs:
            if os.path.exists(copy_dir):
                shutil.rmtree(copy_dir)

    def get_deps_ready(self, work_path, root_path):
        queue = Queue()
        visited = set()
        deps_cls_list = [self]
        tree = [{'name': self.name, 'children': []}]
        now_tree = tree[0]["children"]
        queue.put((self, now_tree))

        while not queue.empty():
            cur_comp, now_tree = queue.get()
            for i, dep_comp_cls in enumerate(cur_comp.deps_comp):
                if dep_comp_cls in visited:
                    continue
                if dep_comp_cls is None:
                    hb_warning('{} has NoneType dep'.format(cur_comp.name))
                    continue
                deps_cls_list.append(dep_comp_cls)
                now_tree.append({'name': dep_comp_cls.name, 'children': []})
                queue.put((dep_comp_cls, now_tree[-1]['children']))
                visited.add(dep_comp_cls)

        if not hasattr(self, 'copy_dirs'):
            self.copy_dirs = []
        for comp_cls in deps_cls_list:
            for path in comp_cls.dirs:
                src_path = os.path.join(root_path, path)
                des_path = os.path.join(work_path, path.split("../")[-1])
                try:
                    shutil.copytree(src_path,
                                    des_path,
                                    symlinks=False,
                                    ignore=shutil.ignore_patterns('.git',
                                                                  '.repo'))
                except OSError:
                    pass
                else:
                    self.copy_dirs.append(des_path)

        return tree

    @classmethod
    def gn_desc(cls, out_file, target, *args):
        cmd = ['./prebuilts/build-tools/linux-x86/bin/gn',
               '--root=.',
               '--dotfile=build/lite/.gn',
               'desc',
               out_file,
               target, *args]

        cwd = os.path.abspath(os.path.join(out_file, os.pardir,
                                           os.pardir, os.pardir))
        ret = check_output(cmd, cwd=cwd).decode('utf-8')
        return ret

    def get_component_deps(self, out_file, now_target, comp_fields):
        include_list = []
        include_dirs_list = [now_target]
        deps_list = []

        ret = self.gn_desc(out_file, now_target, 'deps', '--tree')
        hb_info(f'target: {now_target} cmd: deps --tree')
        hb_info(ret)

        ret_content = [line for line in ret.split('\n')
                       if len(line) and '\n' not in line]

        index = 0
        while index < len(ret_content):  # get deps of the now_target
            cur_line = ret_content[index]
            if '...' in cur_line:  # cur_line already exists.
                index += 1
                continue
            space_now = cur_line.count(' ')
            for path in comp_fields[self.name]:
                # cur_line is part of current comp,
                # needs to be further processed.
                if path in cur_line:
                    index += 1
                    include_dirs_list.append(cur_line.replace(' ', ''))
                    break
            else:
                deps_list.append(cur_line.replace(' ', '').replace('//', ''))
                index += 1
                while index < len(ret_content) and \
                        ret_content[index].count(' ') > space_now:
                    index += 1
        for target in include_dirs_list:
            try:
                ret = self.gn_desc(out_file, target, 'include_dirs')
                hb_info(f'target: {now_target} cmd: include_dirs')
                hb_info(ret)
                include_list += [include.replace('//', '')
                                 for include in ret.split('\n')
                                 if '\n' not in include and len(include)]
            except CalledProcessError:
                pass
        return deps_list, include_list

    def get_deps(self, board, kernel, out_file, comp_fields, fields):
        deps_list = []
        include_list = []
        for now_target in self.targets:
            now_deps, now_include = self.get_component_deps(out_file,
                                                            now_target,
                                                            comp_fields)
            deps_list += now_deps
            include_list += now_include

        deps_ori = list(set(deps_list) | set(include_list))

        comp_deps_set = set()
        for dep in deps_ori:
            if 'prebuilts' in dep or 'build/lite' in dep:
                continue
            for path in comp_fields[self.name]:
                if dep in path or path in dep:
                    break
            else:
                for path, cname in fields.items():
                    if check_path(dep, path):
                        if 'hdf' in cname:  # special processing for hdf
                            cname = 'hdf_{}_{}'.format(board, kernel)
                        if cname != self.name:
                            comp_deps_set.add(cname)
                        break
                else:
                    # special processing for third_party
                    if dep.startswith('third_party'):
                        strlist = dep.split('/')[1].split(':')[0].lower()
                        self.thirdparty_set.add(strlist.replace('-', '_'))
                    else:
                        comp_deps_set.add(dep)
        self.deps_dict[(board, kernel)] = comp_deps_set

    def get_real_deps(self):
        components_set = set()
        board_common = None
        kernel_common = None
        board_special = {}
        kernel_special = {}

        if len(self.deps_dict) == 1:
            _, components_set = self.deps_dict.popitem()
        elif len(self.deps_dict) > 1:
            for _, now_deps in self.deps_dict.items():
                board_common = now_deps if board_common is None\
                    else board_common & now_deps
            for _, now_deps in self.deps_dict.items():
                kernel_common = now_deps if kernel_common is None\
                    else kernel_common & now_deps

            for (board, kernel), now_deps in self.deps_dict.items():
                board_special[board] = now_deps - board_common
                kernel_special[kernel] = now_deps - kernel_common

            special_common = None
            for _, now_deps in board_special.items():
                special_common = now_deps if special_common is None\
                    else special_common & now_deps
            for board, now_deps in board_special.items():
                board_special[board] -= special_common

            components_set = components_set.union(board_common,
                                                  kernel_common,
                                                  special_common)

            special_common = None
            for _, now_deps in kernel_special.items():
                special_common = now_deps if special_common is None\
                    else special_common & now_deps
            for kernel, now_deps in kernel_special.items():
                kernel_special[kernel] -= special_common

            components_set = components_set.union(special_common)

        return self.get_deps_sort(components_set,
                                  kernel_special,
                                  board_special)

    def get_deps_sort(self, components_set, kernel_special, board_special):
        thirdparty_list = list(self.thirdparty_set)
        thirdparty_list.sort()
        components_list = list(components_set)
        components_list.sort()

        kernel_list = [kernel for kernel in kernel_special.keys()]
        kernel_list.sort()
        for kernel in kernel_list:
            kernel_special[kernel] = list(kernel_special[kernel])
            kernel_special[kernel].sort()

        board_list = [board for board in board_special.keys()]
        board_list.sort()
        for board in board_list:
            board_special[board] = list(board_special[board])
            board_special[board].sort()

        self.deps_real_dict = {
            'third_party': thirdparty_list,
            'kernel_special': {kernel: kernel_special[kernel]
                               for kernel in kernel_list
                               if len(kernel_special[kernel])},
            'board_special': {board: board_special[board]
                              for board in board_list
                              if len(board_special[board])},
            'components': components_list
        }

        return self.deps_real_dict
