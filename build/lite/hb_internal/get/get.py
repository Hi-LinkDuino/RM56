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
from hb_internal.build.build_process import Build
from hb_internal.common.utils import remove_path
from hb_internal.common.utils import hb_info
from hb_internal.common.utils import read_json_file

flash_partition_dir=''
def add_options(parser):
    parser.add_argument('target',
                        nargs='?',
                        default=None,
                        help='goto specific path.')

def get_fs_config(build, fs_name):
    json = read_json_file(os.path.join(build.config.product_path, 'config.json'))
    flash_partition_dir = json.get('flash_partition_dir', '')
    fs_list = json.get('fs_list', [])
    if build.config.bes_specific_chip == 'best2003' and build.config.best2003_type != None and build.config.best2003_type != '':
        fs_name += f'_{build.config.best2003_type}'
    for fs in fs_list:
        if fs.get('fs_name','') == fs_name and fs.get('enable','') == 'true':
            fs['flash_partition_dir'] = flash_partition_dir
            return fs
    return None

def exec_command(args):
    t = args.target
    build = Build()
    if t == 'back':
        return print('-')
        # return os.chdir('-')
    if t == 'bsp':
        return print(build.config.bsp_path)
    if t in ['bspout', 'bsp/out']:
        chip=build.config.bes_specific_chip
        t=''
        if chip == 'best1502x':
            t = f'ohos_{chip}'
        elif chip == 'best1600':
            t = f'ohos_{chip}_watch'
        elif chip == 'best2003':
            t = 'best2600w_liteos'
        return print(os.path.join(build.config.bsp_path, 'out', t))

    if t in ['board', 'b']:
        return print(build.config.device_path)

    if t in ['product', 'p', 'prod']:
        return print(build.config.product_path)

    if t == 'elf':
        return print(
            os.path.join(
                build.config.out_path,
                'unstripped',
                'bin',
                f'wifiiot_{build.config.board}.elf',
            )
        )

    if t == 'name':
        return print(build.config.product)

    if t == 'out':
        return print(build.config.out_path)

    if t == 'root':
        return print(build.config.root_path)

    if t == 'chip':
        return print(build.config.bes_specific_chip)
    if t == 'best2003_type':
        return print(build.config.best2003_type)

    if t == 'lcd':
        json = read_json_file(os.path.join(build.config.product_path, 'config.json'))
        return print(json.get('lcd_support', ''))
    if t == 'dist_gui':
        json = read_json_file(os.path.join(build.config.product_path, 'config.json'))
        return print(json.get('bfd_dist_gui', 'false'))
    if t == 'dev_mode':
        json = read_json_file(os.path.join(build.config.product_path, 'config.json'))
        return print(json.get('bfd_dev_mode', ''))

    if t == 'fs_bsize':
        fs = get_fs_config(build, 'littlefs')
        return print('') if fs is None else print(fs.get('block_size', '0'))

    if t == 'fs_size':
        fs = get_fs_config(build, 'littlefs')
        return print('') if fs is None else print(fs.get('fs_size', '0'))

    if t == 'fs_path':
        fs = get_fs_config(build, 'littlefs')
        return print('') if fs is None else print(os.path.join(build.config.product_path, fs.get('flash_partition_dir','fs'), fs.get('fs_src', '')))

    if t == 'fs_args':
        return _extracted_from_exec_command_51(build, 'littlefs')

    if t == 'mmcfs_bsize':
        fs = get_fs_config(build, 'mmcfs')
        return print('') if fs is None else print(fs.get('block_size', '0'))

    if t == 'mmcfs_size':
        fs = get_fs_config(build, 'mmcfs')
        return print('') if fs is None else print(fs.get('fs_size', '0'))

    if t == 'mmcfs_path':
        fs = get_fs_config(build, 'mmcfs')
        return print('') if fs is None else print(os.path.join(build.config.product_path, fs.get('flash_partition_dir','fs'), fs.get('fs_src', '')))

    if t == 'mmcfs_args':
        return _extracted_from_exec_command_51(build, 'mmcfs')

    if t in ['?', '-h', 'help']:
        print('root                             root path of the sources')
        print('out                              out path of the current product')
        print('bsp                              bsp path')
        print('bsp/out                          out path of bsp')
        print('name                             product name')
        print('prod                             product path in vendor')
        print('board                            device board path')
        print('elf                              path of built elf')
        print('chip                             soc_specific_chip defined in config.json')
        print('dev_mode                         bfd_dev_mode defined in config.json')
        print('lcd                              lcd_support defined in config.json')
        print('fs_bsize                         block size of littlefs defined in config.json')
        print('fs_size                          fs size of littlefs defined in config.json')
        print('fs_path                          littlefs image source path')
        print('mmcfs_bsize                      block size of mmcfs defined in config.json')
        print('mmcfs_size                       fs size of mmcfs defined in config.json')
        print('mmcfs_path                       mmcfs image source path')
        return

# TODO Rename this here and in `exec_command`
def _extracted_from_exec_command_51(build, arg1):
    fs = get_fs_config(build, arg1)
    if fs is None:
        return print('')
    bsize = fs.get('block_size')
    fs_size = fs.get('fs_size')
    path = os.path.join(build.config.product_path, fs.get('flash_partition_dir','fs'), fs.get('fs_src', ''))
    return print(f'{bsize} {fs_size} {path}')
