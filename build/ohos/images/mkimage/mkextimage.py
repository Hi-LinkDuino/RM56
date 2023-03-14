#!/usr/bin/env python
# coding: utf-8
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
import argparse
import subprocess
import os

FS_TYPE = "ext4"
BLOCKSIZE = 4096


def args_parse(argv):
    parser = argparse.ArgumentParser(description='mkextimage.py')

    parser.add_argument("src_dir", help="The source file for sload.")
    parser.add_argument("device", help="The deivce for mkfs.")
    parser.add_argument("mount_point", help="The filesystem mountpoint.")
    parser.add_argument("fs_size", help="The size of filesystem.")
    parser.add_argument("--fs_type", help="The filesystem type.")
    parser.add_argument("--dac_config",
                        help="The path of dac config to e2fsdroid.")
    parser.add_argument("--inode_size", help="The inode size to mke2fs.")
    parser.add_argument("--file_context",
                        help="The path of file_context to e2fsdroid.")
    parser.add_argument("--root_dir", help="The root dir for root image.")
    parser.add_argument("--journal_size", help="The journal_size for mke2fs.")
    parser.add_argument("--reserve_percent",
                        help="The reserve_percent for mke2fs.")
    parser.add_argument("--extend_opts", nargs='+',
                        help="The extend opt for mke2fs.(not support sparse)")
    parser.add_argument("--encrypt", help="The fscrypt support.")

    args = parser.parse_known_args(argv)[0]
    return args


def run_cmd(cmd):
    res = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE)
    sout, serr = res.communicate()

    return res.pid, res.returncode, sout, serr


def build_run_mke2fs(args):
    mke2fs_opts = ""
    mke2fs_cmd = ""
    is_data = False

    if "data" in args.mount_point:
        is_data = True
    if args.extend_opts:
        mke2fs_opts += " -E " + ",".join(args.extend_opts)
    if args.inode_size:
        mke2fs_opts += " -I " + args.inode_size
    else:
        mke2fs_opts += " -I " + "256"
    if args.journal_size:
        mke2fs_opts += " -J size=" + args.journal_size
    elif not is_data:
        mke2fs_opts += " -O ^has_journal"
    if args.reserve_percent:
        mke2fs_opts += " -m " + args.reserve_percent
    elif not is_data:
        mke2fs_opts += " -m 0"
    if is_data:
        mke2fs_opts += " -O encrypt"
    mke2fs_opts += " -L " + args.mount_point + " -M " + args.mount_point

    blocks = int(int(args.fs_size) / BLOCKSIZE)
    mke2fs_cmd += ("mke2fs " + str(mke2fs_opts) + " -t " + FS_TYPE + " -b "
                   + str(BLOCKSIZE) + " " + args.device + " " + str(blocks))
    res = run_cmd(mke2fs_cmd)
    if res[1] != 0:
        print("info: " + mke2fs_cmd)
        print("pid " + str(res[0]) + " ret " + str(res[1]) + "\n" +
              res[2].decode() + res[3].decode())
    return res[1]


def build_run_e2fsdroid(args):
    libselinux_path = os.path.realpath("./clang_x64/security/selinux/")
    os.environ['LD_LIBRARY_PATH'] = libselinux_path

    e2fsdroid_opts = ""
    e2fsdroid_cmd = ""

    if not args.extend_opts or not "sparse" in args.extend_opts:
        e2fsdroid_opts += " -e"
    if args.dac_config:
        e2fsdroid_opts += " -C " + args.dac_config
    if args.file_context:
        if(os.path.exists(args.file_context)):
            e2fsdroid_opts += " -S " + args.file_context

    e2fsdroid_cmd += ("e2fsdroid" + e2fsdroid_opts + " -f " +
                      args.src_dir + " -a " + args.mount_point +
                      " " + args.device)
    res = run_cmd(e2fsdroid_cmd)
    if res[1] != 0:
        print("info: " + e2fsdroid_cmd)
        print("pid " + str(res[0]) + " ret " + str(res[1]) + "\n" +
              res[2].decode() + res[3].decode())
    return res[1]


def build(args):
    args = args_parse(args)

    res = build_run_mke2fs(args)
    if res != 0:
        print("error run mke2fs errno: " + str(res))
        sys.exit(1)
    res = build_run_e2fsdroid(args)
    if res != 0:
        print("error run e2fsdroid errno: " + str(res))
        os.remove(args.device)
        sys.exit(2)


if __name__ == '__main__':
    build(sys.argv[1:])
