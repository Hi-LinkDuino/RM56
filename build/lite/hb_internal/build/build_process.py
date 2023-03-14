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
import sys
import platform
from collections import defaultdict
from distutils.spawn import find_executable

from hb_internal.common.utils import exec_command
from hb_internal.common.utils import makedirs
from hb_internal.common.utils import remove_path
from hb_internal.common.utils import hb_info
from hb_internal.common.utils import hb_warning
from hb_internal.common.utils import OHOSException
from hb_internal.common.utils import get_current_time
from hb_internal.common.config import Config
from hb_internal.cts.cts import CTS
from hb_internal.common.device import Device
from hb_internal.common.product import Product
from hb_internal.build.fs_process import Packer
from hb_internal.build.patch_process import Patch
from hb_internal.preloader.preloader import Preloader
from hb_internal.common.misc import PreBuild
from hb_internal.common.misc import PostBuild


class Build():
    def __init__(self, component=None, compact_mode=False):
        self.config = Config()

        # Get gn args ready
        self._args_list = []
        self._target = None
        self._compiler = None
        self._test = None
        self._compact_mode = compact_mode

        self.target = component
        self.start_time = get_current_time()
        self.check_in_device()

    @property
    def target(self):
        return self._target

    @target.setter
    def target(self, component):
        if component is None or not len(component):
            return
        component = component[0]

        cts = CTS()
        cts.init_from_json()
        for subsystem_cls in cts:
            for cname, component_cls in subsystem_cls:
                if cname == component:
                    if not len(component_cls.adapted_board) or\
                       self.config.board in component_cls.adapted_board:
                        if not len(component_cls.adapted_kernel) or\
                           self.config.kernel in component_cls.adapted_kernel:
                            self._target = component_cls.targets
                            self.register_args('ohos_build_target',
                                               self._target)
                            return

        raise OHOSException(f'Component {component} not found')

    @property
    def compiler(self):
        return self._compiler

    @compiler.setter
    def compiler(self, value):
        if value:
            self._compiler = value
            self.register_args('ohos_build_compiler_specified', self._compiler)
            if self._compiler == 'clang':
                self.register_args('ohos_build_compiler_dir',
                                   self.config.clang_path)

    @property
    def test(self):
        return self._test

    @test.setter
    def test(self, test_args):
        cmd_list = ['xts', 'notest']
        if test_args[0] in cmd_list:
            if test_args[0] == 'notest':
                self.register_args('ohos_test_args', 'notest')
            else:
                self._test = test_args[1]
                if len(test_args) > 1:
                    self.register_args('ohos_xts_test_args', self._test)
        else:
            raise OHOSException('Error: wrong input of test')

    @property
    def build_time(self):
        return get_current_time() - self.start_time

    def register_args(self, args_name, args_value, quota=True):
        quota = False if args_value in ['true', 'false'] else quota
        if quota:
            if isinstance(args_value, list):
                self._args_list += [
                    '{}="{}"'.format(args_name, "&&".join(args_value))
                ]
            else:
                self._args_list += ['{}="{}"'.format(args_name, args_value)]
        else:
            self._args_list += ['{}={}'.format(args_name, args_value)]
        if args_name == 'ohos_build_target' and len(args_value):
            self.config.fs_attr = None

    def build(self, full_compile, patch=False, ninja=True, cmd_args=None):
        cmd_list = self.get_cmd(full_compile, patch, ninja)

        # enable ccache if it installed.
        ccache_path = find_executable('ccache')
        if ccache_path is not None:
            self.register_args('ohos_build_enable_ccache', 'true', quota=False)

        if cmd_args is None:
            cmd_args = defaultdict(list)
        try:
            for exec_cmd in cmd_list:
                exec_cmd(cmd_args)
        except OHOSException:
            raise
        except Exception:
            raise
        finally:
            post_build = PostBuild(self.config)
            post_build.clean(self.start_time)

        hb_info(f'{os.path.basename(self.config.out_path)} build success')
        hb_info(f'cost time: {self.build_time}')
        return 0

    def get_cmd(self, full_compile, patch, ninja):
        cmd_list = []

        pre_build = PreBuild(self.config)
        cmd_list.append(pre_build.prepare)

        if patch:
            patch = Patch()
            cmd_list.append(patch.patch_make)

        preloader = Preloader(self.config)
        cmd_list.append(preloader.run)

        # if full_compile is set, remove out and do full build
        # if build_only_gn is set, only do gn parse
        # else do incremental build.
        if full_compile:
            remove_path(self.config.out_path)
            makedirs(self.config.out_path, exist_ok=True)
            cmd_list.extend([self.gn_build, self.ninja_build])
        elif not ninja:
            makedirs(self.config.out_path, exist_ok=True)
            cmd_list.append(self.gn_build)
            return cmd_list
        else:
            makedirs(self.config.out_path, exist_ok=True)
            cmd_list.extend([self.gn_build, self.ninja_build])

        if self.config.os_level != "standard":
            if self.config.fs_attr is not None:
                packer = Packer()
                cmd_list.append(packer.fs_make)

        return cmd_list

    def env(self):
        system = platform.system().lower()
        if self.config.os_level == 'standard':
            path = os.environ.get('PATH')
            my_python = os.path.join(
                self.config.root_path,
                f'prebuilts/python/{system}-x86/3.8.5/bin')
            os.environ['PATH'] = f'{my_python}:{path}'
        path = os.environ.get('PATH')
        my_build_tools = os.path.join(
            self.config.root_path, f'prebuilts/build-tools/{system}-x86/bin')
        os.environ['PATH'] = f'{my_build_tools}:{path}'
        return os.environ

    def gn_build(self, cmd_args):
        # Gn cmd init and execute
        # if self.config.os_level == "standard":
        gn_path = 'gn'
        # else:
        #     gn_path = self.config.gn_path
        gn_args = cmd_args.get('gn', [])
        os_level = self.config.os_level
        gn_cmd = [
            gn_path,
            'gen',
            '--args={}'.format(" ".join(self._args_list)),
            self.config.out_path,
        ] + gn_args
        if os_level == 'mini' or os_level == 'small':
            gn_cmd.append(f'--script-executable={sys.executable}')
        if self._compact_mode is False:
            gn_cmd.extend([
                '--root={}'.format(self.config.root_path),
                '--dotfile={}/.gn'.format(self.config.build_path),
            ])
        exec_command(gn_cmd, log_path=self.config.log_path, env=self.env())

    def gn_clean(self, out_path=None):
        remove_path(self.config.out_path)

    def ninja_build(self, cmd_args):
        # if self.config.os_level == "standard":
        ninja_path = 'ninja'
        # else:
        #     ninja_path = self.config.ninja_path

        ninja_args = cmd_args.get('ninja', {})
        my_ninja_args = []
        if ninja_args.get('verbose') == True:
            my_ninja_args.append('-v')
        if ninja_args.get('keep_ninja_going') == True:
            my_ninja_args.append('-k1000000')

        # Keep targets to the last
        if ninja_args.get('default_target') is not None:
            if self.config.os_level == "standard":
                if self.config.product == 'ohos-sdk':
                    my_ninja_args.append('build_ohos_sdk')
                else:
                    my_ninja_args.append('images')
            else:
                my_ninja_args.append('packages')
        if ninja_args.get('targets'):
            my_ninja_args.extend(ninja_args.get('targets'))
        ninja_cmd = [
            ninja_path, '-w', 'dupbuild=warn', '-C', self.config.out_path
        ] + my_ninja_args

        exec_command(ninja_cmd,
                     log_path=self.config.log_path,
                     log_filter=True,
                     env=self.env())

    def check_in_device(self):
        if self._target is None and Device.is_in_device():
            # Compile device board
            device_path, kernel, board = Device.device_menuconfig()
            hb_info(f'{device_path}')
            # build device, no need to set root manually,
            # so set it speculatively.
            self.config.root_path = os.path.abspath(
                os.path.join(device_path, os.pardir, os.pardir, os.pardir,
                             os.pardir))
            self.config.out_path = os.path.join(self.config.root_path, 'out',
                                                board)
            self.compiler = Device.get_compiler(device_path)
            gn_device_path = os.path.dirname(device_path)
            gn_kernel_path = device_path
            self.register_args('ohos_build_target', [gn_device_path])
            self.register_args('device_path', gn_kernel_path)
            self.register_args('device_company', self.config.device_company)
            self.register_args('ohos_kernel_type', kernel)
        else:
            # Compile product in "hb set"
            self.compiler = Device.get_compiler(self.config.device_path)
            self.register_args('product_path', self.config.product_path)
            if self._compact_mode:
                self.register_args('product_name', self.config.product)
                if self.config.board:
                    self.register_args('device_name', self.config.board)
                if self.config.target_cpu:
                    self.register_args('target_cpu', self.config.target_cpu)
                if self.config.target_os:
                    self.register_args('target_os', self.config.target_os)
                if self.config.os_level:
                    self.register_args(f'is_{self.config.os_level}_system',
                                       'true')

            if self.config.product == 'ohos-sdk':
                self.register_args('build_ohos_sdk', 'true')
                self.register_args('build_ohos_ndk', 'true')
            self.register_args('device_path', self.config.device_path)
            if self.config.os_level != "standard":
                self.register_args('device_company',
                                   self.config.device_company)
            if self.config.kernel:
                self.register_args('ohos_kernel_type', self.config.kernel)

            self._args_list += Product.get_features(self.config.product_json)
