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
import configparser
import os
import shutil
import sys
import argparse
import subprocess

DTC_419 = "dtc_419_path"
DTC_510 = "dtc_510_path"
FSTAB_REQUIRED = "fstab_required_path"
NEED_CLEAR_RESOURCE_SECTION = \
    [DTC_419, DTC_510, FSTAB_REQUIRED, "mkimage_path"]


def args_parse(args):
    parser = argparse.ArgumentParser(description='mkcpioimage.py')

    parser.add_argument("src_dir", help="The source file for sload.")
    parser.add_argument("device", help="The deivce for mkfs.")
    parser.add_argument("resource_config",
                        help="The ramdisk resource config file.")

    args = parser.parse_known_args(args)[0]
    return args


def run_cmd(cmd, dir_list=None):
    res = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                           stdin=subprocess.PIPE,
                           stderr=subprocess.PIPE)
    if dir_list is not None:
        for each_path in dir_list:
            print("mkcpio image, cpio stdin: ", each_path)
            res.stdin.write(("%s\n" % each_path).encode('utf-8'))
    sout, serr = res.communicate()
    res.wait()
    return res.pid, res.returncode, sout, serr


def get_dir_list(input_path, dir_list):
    if os.path.isdir(input_path):
        dir_list.append(input_path)
        tem_list = os.listdir(input_path)
        for each in tem_list:
            each_path = os.path.join(input_path, each)
            get_dir_list(each_path, dir_list)
    else:
        dir_list.append(input_path)


def build_run_fitimage(args):
    src_dir = args.src_dir
    index = src_dir.rfind('/')
    root_dir = src_dir[:index]

    if BOOT_TYPE == "two_stages":
        fit_cmd = \
            [os.path.join(root_dir, "make-boot.sh"),
             os.path.join(root_dir, "../../..")]
    else:
        if not os.path.exists("./ohos.its"):
            print("error there is no configuration file")
            return -1
        if not os.path.exists(os.path.join(root_dir, "images", "zImage-dtb")):
            print("error there is no kernel image")
            return -1

        fit_cmd = \
            ["mkimage", '-f', "./ohos.its",
             os.path.join(root_dir, "images", "boot.img")]

    res = run_cmd(fit_cmd)
    if res[1] != 0:
        print(" ".join(["pid ", str(res[0]), " ret ", str(res[1]), "\n",
                        res[2].decode(), res[3].decode()]))

    return res[1]


def build_run_cpio(args):
    work_dir = os.getcwd()
    os.chdir(args.src_dir)

    output_path = os.path.join(work_dir, args.device)
    ramdisk_cmd = ['cpio', '-o', '-H', 'newc', '-O', output_path]
    dir_list = []
    get_dir_list("./", dir_list)
    res = run_cmd(ramdisk_cmd, dir_list)
    if res[1] != 0:
        print(" ".join(["pid ", str(res[0]), " ret ", str(res[1]), "\n",
                        res[2].decode(), res[3].decode()]))
    os.chdir(work_dir)
    return res[1]


def build_run_chmod(args):
    src_dir = args.src_dir
    src_index = src_dir.rfind('/')
    root_dir = src_dir[:src_index]

    if BOOT_TYPE == "two_stages":
        return 0

    chmod_cmd = ['chmod', '664', os.path.join(root_dir, "images", "boot.img")]
    res = run_cmd(chmod_cmd)
    if res[1] != 0:
        print(" ".join(["pid ", str(res[0]), " ret ", str(res[1]), "\n",
                        res[2].decode(), res[3].decode()]))
    return res[1]


def parse_resource_config(resource_config_file_path):
    """
    parse ramdisk_resource_config.ini
    :param resource_config_file_path: ramdisk_resource_config.ini path
    :return:
    """
    dtc_419_source_path = ""
    dtc_510_source_path = ""
    global BOOT_TYPE
    BOOT_TYPE = ""
    need_clear_section_target_path_list = []
    if os.path.exists(resource_config_file_path):
        ramdisk_config = configparser.ConfigParser()
        ramdisk_config.read(resource_config_file_path)
        for each_section in ramdisk_config.items():
            if each_section[0] == "DEFAULT":
                continue
            section_options = dict(ramdisk_config.items(each_section[0]))
            source_path = section_options.get("source_path", None)
            target_path = section_options.get("target_path", None)
            if each_section[0] in NEED_CLEAR_RESOURCE_SECTION:
                if each_section[0] == FSTAB_REQUIRED:
                    need_clear_section_target_path_list.append(source_path)
                else:
                    need_clear_section_target_path_list.append(target_path)
            if each_section[0] == DTC_419:
                dtc_419_source_path = source_path
            if each_section[0] == "board":
                BOOT_TYPE = section_options.get("boot_type", None)
            if each_section[0] == DTC_510:
                dtc_510_source_path = source_path
            if os.path.exists(source_path):
                shutil.copy(source_path, target_path)
            else:
                if each_section[0] not in [DTC_419, DTC_510]:
                    print("Error: source file does not exist! path: %s" %
                          source_path)
                    sys.exit(1)
        if not os.path.exists(dtc_419_source_path) and \
                not os.path.exists(dtc_510_source_path):
            print("Warning: dtc tool does not exists, ")
    else:
        print("Error: ramdisk_resource_config.ini file does not exist!")
        sys.exit(1)
    return need_clear_section_target_path_list


def clear_resource_file(input_file_path_list):
    """
    clear resource file
    :param input_file_path_list: List of files to clean up
    :return:
    """
    for each_path in input_file_path_list:
        if os.path.exists(each_path):
            os.remove(each_path)


def main(args):
    args = args_parse(args)
    print("Make cpio image!")

    need_clear_section_target_path_list = \
        parse_resource_config(args.resource_config)

    res = build_run_cpio(args)
    if res != 0:
        print("error run cpio ramdisk errno: %s" % str(res))
        clear_resource_file(need_clear_section_target_path_list)
        sys.exit(1)
    res = build_run_fitimage(args)
    if res != 0:
        print("error run fit image errno: %s" % str(res))
        clear_resource_file(need_clear_section_target_path_list)
        sys.exit(2)
    res = build_run_chmod(args)
    if res != 0:
        print("error run chmod errno: %s" % str(res))
        clear_resource_file(need_clear_section_target_path_list)
        sys.exit(3)
    clear_resource_file(need_clear_section_target_path_list)


if __name__ == '__main__':
    main(sys.argv[1:])
