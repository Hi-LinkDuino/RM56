#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2021 bestechnic (Shanghai) Technologies CO., LIMITED.
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
import re
import subprocess
import shutil
import sys
import json


def usage():
    print('\r\n{0} Usage:\r\n'.format(sys.argv[0]))
    print(' -h: for help\r\n')
    print(' {0} {1} {2} {3} {4}\r\n'.format(sys.argv[0],'<product path>','<origin bin path>','<gen hash and sec path>','<sig tool path>'))
    print('parameters guide:')
    print('     <product path> {}'.format('example: ./vendor/bestechnic/iotlink_demo'))
    print('     <origin bin path> {}'.format('example: ./out/bes2600w/iotlink_demo/release_bin'))
    print('     <gen hash and sec path> {}'.format('example: ./device/bestechnic/bes2600w/burn_tools/tools'))
    print('     <sig tool path> {}'.format('producter own sig function,need custom complement,which is a python or shell file path'))
    print('\r\n')
    sys.exit(1)

for var in sys.argv[1:]:
    if var == '-h':
        usage()

if len(sys.argv) < 5:
    usage()

sig_args = ''
for var in sys.argv[1:]:
    sig_args = sig_args + ' ' + var

product_path = sys.argv[1]
src_bin_path = sys.argv[2]
tools_path = sys.argv[3]
gen_sig_tool_path = sys.argv[4]

if not os.path.isfile(gen_sig_tool_path):
    raise OSError('{} not found'.format(gen_sig_tool_path))

# Read json file data
def read_json_file(input_file):
    if not os.path.isfile(input_file):
        raise OSError('{} not found'.format(input_file))
    print("readjson {}".format(input_file))
    with open(input_file, 'rb') as input_f:
        try:
            data = json.load(input_f)
            return data
        except json.JSONDecodeError:
            raise Exception('{} parsing error!'.format(input_file))


# transfer path
def transfer_path(dirpath,file):
    if not dirpath.endswith('/'):
        file_path = dirpath + '/' + file
    else:
        file_path = dirpath + file
    return file_path

# gen hash of bin
def gen_bin_hash(tool,src_bin):

    if not os.path.isfile(tool):
        raise OSError('{} not found'.format(tool))

    if not os.path.isfile(src_bin):
        raise OSError('{} not found'.format(src_bin))

    os.system('chmod a+x '+tool)
    os.system('./'+tool+' '+ src_bin)

# gen sec bin
def gen_sec_bin(tool,src_bin,bin_sig):

    if not os.path.isfile(tool):
        raise OSError('{} not found'.format(tool))

    if not os.path.isfile(src_bin):
        raise OSError('{} not found'.format(src_bin))

    if not os.path.isfile(bin_sig):
        raise OSError('{} not found'.format(bin_sig))

    os.system('chmod a+x '+ tool)
    print("./{0} {1} {2}".format(tool,src_bin,bin_sig))
    os.system('./'+ tool +' '+ src_bin + ' ' + bin_sig)

# gen bin sig
def gen_bin_sig(tool,bin_hash,bin_sig,args_list):
    if not os.path.isfile(tool):
        raise OSError('{} not found'.format(tool))

    if not os.path.isfile(bin_hash):
        raise OSError('{} not found'.format(bin_hash))

    os.system('chmod a+x '+ tool)
    os.system('./'+ tool + ' ' + bin_hash +  ' ' + bin_sig + ' ' + args_list)

product_json = os.path.join(product_path,'config.json')
product_obj = read_json_file(product_json)

# harmonyos image
bin_list = product_obj.get('bin_list', [])
for bin_obj in bin_list:
    bin_name = bin_obj.get('burn_name')
    if bin_obj.get('enable') != 'true':
        continue

    src_bin = transfer_path(src_bin_path,bin_name + '.bin')
    if not os.path.isfile(src_bin):
        raise OSError('{} not found'.format(src_bin))

    tool_hash = transfer_path(tools_path,'gen_hash_of_bin')
    if not os.path.isfile(tool_hash):
        raise OSError('{} not found'.format(tool_hash))

    gen_bin_hash(tool_hash,src_bin)

    bin_hash = transfer_path(src_bin_path,bin_name + '_hash.bin')
    if not os.path.isfile(bin_hash):
        raise OSError('{} not found'.format(bin_hash))

    bin_sig = transfer_path(src_bin_path,bin_name + '_sig.bin')
    gen_bin_sig(gen_sig_tool_path,bin_hash,bin_sig,sig_args)

    tool_sec = transfer_path(tools_path,'gen_sec_bin')
    if not os.path.isfile(tool_sec):
        raise OSError('{} not found'.format(tool_sec))

    gen_sec_bin(tool_sec,src_bin,bin_sig)


# file system image
fs_list = product_obj.get('fs_list', [])
for fs_obj in fs_list:
    fs_name = fs_obj.get('burn_name')
    if fs_obj.get('enable') != 'true':
        continue

    fs_bin = transfer_path(src_bin_path,fs_name + '.bin')
    if not os.path.isfile(fs_bin):
        raise OSError('{} not found'.format(fs_bin))

    tool_hash = transfer_path(tools_path,'gen_hash_of_littlefs')
    if not os.path.isfile(tool_hash):
        raise OSError('{} not found'.format(tool_hash))

    gen_bin_hash(tool_hash,fs_bin)

    fs_hash = transfer_path(src_bin_path,fs_name + '_hash.bin')
    if not os.path.isfile(fs_hash):
        raise OSError('{} not found'.format(fs_hash))

    fs_sig = transfer_path(src_bin_path,fs_name + '_sig.bin')
    gen_bin_sig(gen_sig_tool_path,fs_hash,fs_sig,sig_args)

    tool_sec = transfer_path(tools_path,'gen_sec_littlefs')
    if not os.path.isfile(tool_sec):
        raise OSError('{} not found'.format(tool_sec))

    gen_sec_bin(tool_sec,fs_bin,fs_sig)

# bsp own bin
bsp_bin_list = product_obj.get('bsp_bin_list', [])
for bsp_bin_obj in bsp_bin_list:
    bsp_bin_name = bsp_bin_obj.get('burn_name')
    if bin_obj.get('enable') != 'true':
        continue

    src_bsp_bin = transfer_path(src_bin_path,bsp_bin_name + '.bin')
    if not os.path.isfile(src_bsp_bin):
        raise OSError('{} not found'.format(src_bsp_bin))

    tool_hash = transfer_path(tools_path,'gen_hash_of_bin')
    if not os.path.isfile(tool_hash):
        raise OSError('{} not found'.format(tool_hash))

    gen_bin_hash(tool_hash,src_bsp_bin)

    bsp_bin_hash = transfer_path(src_bin_path,bsp_bin_name + '_hash.bin')
    if not os.path.isfile(bsp_bin_hash):
        raise OSError('{} not found'.format(bsp_bin_hash))

    bsp_bin_sig = transfer_path(src_bin_path,bsp_bin_name + '_sig.bin')
    gen_bin_sig(gen_sig_tool_path,bsp_bin_hash,bsp_bin_sig,sig_args)

    tool_sec = transfer_path(tools_path,'gen_sec_bin')
    if not os.path.isfile(tool_sec):
        raise OSError('{} not found'.format(tool_sec))

    gen_sec_bin(tool_sec,src_bsp_bin,bsp_bin_sig)
