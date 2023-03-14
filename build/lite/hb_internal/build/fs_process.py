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

from collections import namedtuple
import os
import shutil

from hb_internal.common.utils import exec_command
from hb_internal.common.utils import makedirs
from hb_internal.common.utils import read_yaml_file
from hb_internal.common.utils import hb_info
from hb_internal.common.utils import hb_warning
from hb_internal.common.config import Config


class Packer():
    def __init__(self) -> None:
        self.config = Config()
        self.replace_items = {
            r'${product_name}': self.config.product,
            r'${root_path}': self.config.root_path,
            r'${out_path}': self.config.out_path
        }
        self.packing_process = [
            self.mv_usr_libs, self.create_fs_dirs, self.fs_link,
            self.fs_filemode, self.fs_make_cmd, self.fs_tear_down
        ]
        self.fs_cfg = None
        self.chmod_dirs = []

    def mv_usr_libs(self):
        src_path = self.config.out_path
        libs = [lib for lib in os.listdir(src_path) if self.is_lib(lib)]
        target_path = os.path.join(src_path, 'usr', 'lib')
        makedirs(target_path, exist_ok=True)

        for lib in libs:
            source_file = os.path.join(src_path, lib)
            target_file = os.path.join(target_path, lib)
            shutil.move(source_file, target_file)

    @classmethod
    def is_lib(cls, lib):
        return lib.startswith('lib') and lib.endswith('.so')

    @classmethod
    def is_incr(cls, fs_incr):
        exist_ok = False if fs_incr is None else True
        with_rm = True if fs_incr is None else False
        return exist_ok, with_rm

    def create_fs_dirs(self):
        fs_path = os.path.join(self.config.out_path,
                               self.fs_cfg.get('fs_dir_name', 'rootfs'))
        exist_ok, with_rm = self.is_incr(self.fs_cfg.get('fs_incr', None))

        makedirs(fs_path, exist_ok=exist_ok, with_rm=with_rm)
        self.replace_items[r'${fs_dir}'] = fs_path

        for fs_dir in self.fs_cfg.get('fs_dirs', []):
            source_dir = fs_dir.get('source_dir', '')
            target_dir = fs_dir.get('target_dir', '')
            if target_dir == '':
                continue

            source_path = self.fs_dirs_replace(source_dir,
                                               self.config.out_path)
            target_path = self.fs_dirs_replace(target_dir, fs_path)

            if source_dir == '' or not os.path.exists(source_path):
                makedirs(target_path)
                target_mode_tuple = (target_path, fs_dir.get('dir_mode', 755))
                self.chmod_dirs.append(target_mode_tuple)
                continue

            self.copy_files(source_path, target_path, fs_dir)

    def fs_dirs_replace(self, path, default_path):
        source_path, is_changed = self.replace(path)
        if not is_changed:
            source_path = os.path.join(default_path, path)
        return source_path

    def copy_files(self, spath, tpath, fs_dir):
        ignore_files = fs_dir.get('ignore_files', [])
        dir_mode = fs_dir.get('dir_mode', 755)
        file_mode = fs_dir.get('file_mode', 555)

        def copy_file_process(source_path, target_path):
            if not os.path.isdir(target_path):
                makedirs(target_path)
                self.chmod_dirs.append((target_path, dir_mode))
            tfile = os.path.join(target_path, os.path.basename(source_path))
            try:
                shutil.copy(sfile, tfile, follow_symlinks=False)
                self.chmod_dirs.append((tfile, file_mode))
            except FileExistsError:
                hb_warning(f'Target file: {tfile} already exists!')

        if os.path.isfile(spath):
            sfile = spath
            copy_file_process(spath, tpath)
            return

        for srelpath, sfile in self.list_all_files(spath, ignore_files):
            tdirname = srelpath.replace(spath, tpath)
            copy_file_process(sfile, tdirname)

    @classmethod
    def chmod(cls, file, mode):
        mode = int(str(mode), base=8)
        if os.path.exists(file):
            os.chmod(file, mode)

    @classmethod
    def filter(cls, files, ignore_list):
        if ignore_list is None or not len(ignore_list):
            return files
        filter_files = []
        for file in files:
            flag = True
            for ignore in ignore_list:
                if file.startswith(ignore) or file.endswith(ignore):
                    flag = False
                    break
            if flag:
                filter_files.append(file)
        return filter_files

    def list_all_files(self, path, ignore_list=None):
        for relpath, _, files in os.walk(path):
            files = self.filter(files, ignore_list)
            for file in files:
                full_path = os.path.join(path, relpath, file)
                if os.path.isfile(full_path):
                    yield relpath, full_path

    def replace(self, raw_str):
        old_str = raw_str
        for old, new in self.replace_items.items():
            raw_str = raw_str.replace(old, new)
        return raw_str, old_str != raw_str

    def fs_link(self):
        fs_symlink = self.fs_cfg.get('fs_symlink', [])
        for symlink in fs_symlink:
            source, _ = self.replace(symlink.get('source', ''))
            link_name, _ = self.replace(symlink.get('link_name', ''))
            if os.path.exists(link_name):
                os.remove(link_name)
            os.symlink(source, link_name)

    def fs_filemode(self):
        fs_filemode = self.fs_cfg.get('fs_filemode', [])
        for filestat in fs_filemode:
            file_dir = os.path.join(self.replace_items[r'${fs_dir}'],
                                    filestat.get('file_dir', ''))
            file_mode = filestat.get('file_mode', 0)
            if os.path.exists(file_dir) and file_mode > 0:
                self.chmod_dirs.append((file_dir, file_mode))

        for file_dir, file_mode in self.chmod_dirs:
            self.chmod(file_dir, file_mode)

    def fs_make_cmd(self):
        fs_make_cmd = self.fs_cfg.get('fs_make_cmd', [])
        log_path = self.config.log_path

        for cmd in fs_make_cmd:
            cmd, _ = self.replace(cmd)
            cmd = cmd.split(' ')
            exec_command(cmd, log_path=log_path)

    def fs_tear_down(self):
        while len(self.chmod_dirs):
            tfile = self.chmod_dirs.pop()[0]

            if os.path.isfile(tfile):
                self.chmod(tfile, 555)
            elif os.path.isdir(tfile):
                self.chmod(tfile, 755)

    def fs_attr_process(self, fs_cfg):
        fs_attr = fs_cfg.get('fs_attr', {})
        for attr_key, attr_value in fs_attr.items():
            if attr_key in self.config.fs_attr:
                for target_key, target_value in attr_value.items():
                    if target_key in fs_cfg:
                        fs_cfg[target_key] += target_value
                    else:
                        fs_cfg[target_key] = target_value

        return fs_cfg

    def fs_make(self, cmd_args):
        fs_cfg_path = os.path.join(self.config.product_path, 'fs.yml')
        if not os.path.isfile(fs_cfg_path):
            hb_info(f'{fs_cfg_path} not found, stop packing fs. '
                    'If the product does not need to be packaged, ignore it.')
            return
        if self.config.fs_attr is None:
            hb_info('component compiling, no need to pack fs')
            return

        fs_cfg_list = read_yaml_file(fs_cfg_path)
        for fs_cfg in fs_cfg_list:
            self.fs_cfg = self.fs_attr_process(fs_cfg)
            if self.fs_cfg.get('fs_dir_name', None) is None:
                continue

            for fs_process_func in self.packing_process:
                fs_process_func()

if __name__ == "__main__":
    packer = Packer()
    ConfigTest = namedtuple(
        'config',
        ['product', 'root_path', 'out_path', 'log_path', 'product_path', 'fs_attr'])
    packer.config = ConfigTest('', '', '', '', set())
    packer.config.fs_attr.add('dmverity_enable')
    packer.replace_items = {
        r'${product_name}': packer.config.product,
        r'${root_path}': packer.config.root_path,
        r'${out_path}': packer.config.out_path
    }
    packer.fs_make('')
