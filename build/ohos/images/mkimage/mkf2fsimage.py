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


def args_parse(args):
    parser = argparse.ArgumentParser(description='mkf2fsimage.py')

    parser.add_argument("src_dir", help="The source file for sload.")
    parser.add_argument("device", help="The deivce for mkfs.")
    parser.add_argument("mount_point", help="The filesystem mountpoint.")
    parser.add_argument("fs_size", help="The size of filesystem.")
    parser.add_argument("--fs_type", help="The filesystem type.")
    parser.add_argument("--sparse", action='store_true',
                        help="The sparse opt(not support).")
    parser.add_argument("--prjquota", action='store_true',
                        help="The prjquota opt for mkf2fs.")
    parser.add_argument("--casefold", action='store_true',
                        help="The casefold opt for mkf2fs.")
    parser.add_argument("--dac_config",
                        help="The path of fs config to sload.f2fs.")
    parser.add_argument("--timestamp", help="The timestamp for filesystem.")
    parser.add_argument("--label", help="The lable for filesystem.")
    parser.add_argument("--file_context",
                        help="The path of file_context to sload.f2fs.")
    parser.add_argument("--root_dir", help="The root dir for root image.")
    parser.add_argument("--encrypt", help="The fscrypt support.")

    args = parser.parse_known_args(args)[0]
    return args


def run_cmd(cmd):
    res = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE)
    sout, serr = res.communicate()

    return res.pid, res.returncode, sout, serr


def build_run_mkf2fs(args):
    libuuid_path = os.path.realpath("./clang_x64/distributeddatamgr/e2fsprogs/")
    os.environ['LD_LIBRARY_PATH'] = libuuid_path
    mkf2fs_opts = ""
    mkf2fs_cmd = ""

    if args.sparse:
        mkf2fs_opts += " -S " + args.fs_size
    if args.label:
        mkf2fs_opts += " -l " + args.label
    else:
        mkf2fs_opts += " -l " + args.mount_point
    if args.prjquota:
        mkf2fs_opts += " -O project_quota,extra_attr"
    if args.casefold:
        mkf2fs_opts += " -O casefold -C utf8 "

    mkf2fs_cmd += ("mkfs.f2fs -d1 -f -O encrypt -O quota " +
                   " -O verity -w 4096 -R 0:0 " + mkf2fs_opts +
                   " " + args.device)

    res = run_cmd(mkf2fs_cmd)
    if res[1] != 0:
        print("info " + mkf2fs_cmd)
        print("pid " + str(res[0]) + " ret " + str(res[1]) + "\n" +
              res[2].decode() + res[3].decode())
    return res[1]


def build_run_sloadf2fs(args):
    libselinux_path = os.path.realpath("./clang_x64/security/selinux/")
    libuuid_path = os.path.realpath("./clang_x64/distributeddatamgr/e2fsprogs/")
    os.environ['LD_LIBRARY_PATH'] = libselinux_path + ":" + libuuid_path

    oldcwd = os.getcwd();
    os.chdir("./clang_x64/filemanagement/storage_service")
    run_cmd("ln -s fsck.f2fs sload.f2fs")
    os.chdir(oldcwd);

    sloadf2fs_opts = ""
    sloadf2fs_cmd = ""

    if args.sparse:
        sloadf2fs_opts += " -S"
    if args.dac_config:
        sloadf2fs_opts += " -C " + args.dac_config
    sloadf2fs_opts += " -f " + args.src_dir
    if args.file_context:
        sloadf2fs_opts += " -s " + args.file_context
    if args.mount_point[0] != '/':
        args.mount_point = "/" + args.mount_point
    sloadf2fs_opts += " -t " + args.mount_point
    if args.timestamp:
        sloadf2fs_opts += " -T " + args.timestamp

    sloadf2fs_cmd += ("sload.f2fs " + sloadf2fs_opts + " " + args.device)
    res = run_cmd(sloadf2fs_cmd)
    if res[1] != 0:
        print("info " + sloadf2fs_cmd)
        print("pid " + str(res[0]) + " ret " + str(res[1]) + "\n" +
              res[2].decode() + res[3].decode())
    return res[1]


def build(args):
    args = args_parse(args)

    if not args.sparse:
        trunc_cmd = "truncate -s " + args.fs_size + " " + args.device
        res = run_cmd(trunc_cmd)
        if res[1] != 0:
            sys.exit(1)
    res = build_run_mkf2fs(args)
    if res != 0:
        print("error run mkf2fs errno: " + str(res))
        sys.exit(2)
    res = build_run_sloadf2fs(args)
    if res != 0 and res != 1:
        print("error run sload.f2fs errno: " + str(res))
        os.remove(args.device)
        sys.exit(3)


if __name__ == '__main__':
    build(sys.argv[1:])
