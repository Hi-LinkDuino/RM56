#!/usr/bin/env python
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

import sys
import argparse
import os
import shutil

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util.file_utils import read_json_file, write_json_file, write_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402


def _get_modules_info(system_install_info, depfiles):
    modules_info_dict = {}
    for subsystem_info in system_install_info:
        part_name = subsystem_info.get('part_name')
        part_info_file = subsystem_info.get('part_info_file')

        # read subsystem module info
        part_install_info = read_json_file(part_info_file)
        if part_install_info is None:
            raise Exception(
                "read part '{}' installed modules info failed.".format(
                    part_name))
        depfiles.append(part_info_file)

        for info in part_install_info:
            module_def = info.get('module_def')
            module_info_file = info.get('module_info_file')
            depfiles.append(module_info_file)
            if module_def not in modules_info_dict:
                modules_info_dict[module_def] = module_info_file
    return modules_info_dict


def _get_post_process_modules_info(post_process_modules_info_files, depfiles):
    modules_info_list = []
    for _modules_info_file in post_process_modules_info_files:
        _modules_info = read_json_file(_modules_info_file)
        if _modules_info is None:
            raise Exception("read _modules_info_file '{}' failed.".format(
                _modules_info_file))
        modules_info_list.extend(_modules_info)
        depfiles.append(_modules_info_file)
    return modules_info_list


def copy_modules(system_install_info, install_modules_info_file,
                 modules_info_file, module_list_file,
                 post_process_modules_info_files, platform_installed_path,
                 additional_system_files, depfiles):
    output_result = []
    dest_list = []
    symlink_dest = []

    modules_info_dict = _get_modules_info(system_install_info, depfiles)
    for value in modules_info_dict.values():
        module_info = read_json_file(value)
        if not module_info:
            raise Exception(
                "read module install info file '{}' error.".format(value))
        install = module_info.get('install_enable')
        if not install:
            continue
        output_result.append(module_info)

    # get post process modules info
    post_process_modules = _get_post_process_modules_info(
        post_process_modules_info_files, depfiles)
    for _module_info in post_process_modules:
        install = _module_info.get('install_enable')
        if not install:
            continue
        output_result.append(_module_info)

    for source, system_path in additional_system_files:
        shutil.copy(source, os.path.join(platform_installed_path, system_path))

    # copy modules
    for module_info in output_result:
        if module_info.get('type') == 'none':
            continue
        # copy module lib
        source = module_info.get('source')
        dests = module_info.get('dest')
        # check source
        if not os.path.exists(source):
            raise Exception("source '{}' doesn't exist.".format(source))
        depfiles.append(source)
        for dest in dests:
            if dest.startswith('/'):
                dest = dest[1:]
            dest_list.append(dest)
            # dest_dir_prefix
            dest_dir = os.path.join(platform_installed_path,
                                    os.path.dirname(dest))
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)
            shutil.copy(source, os.path.join(platform_installed_path, dest))

        # add symlink
        if 'symlink' in module_info:
            symlink_dest = module_info.get('symlink')
            for dest in dests:
                symlink_src_file = os.path.basename(dest)
                for name in symlink_dest:
                    symlink_dest_dir = os.path.dirname(dest)
                    symlink_dest_file = os.path.join(platform_installed_path,
                                                     symlink_dest_dir, name)
                    if not os.path.exists(symlink_dest_file):
                        os.symlink(symlink_src_file, symlink_dest_file)

    # write install module info to file
    write_json_file(install_modules_info_file, modules_info_dict)

    # write all module info
    write_json_file(modules_info_file, output_result)

    # output module list to file
    write_file(module_list_file, '\n'.join(dest_list))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--system-install-info-file', required=True)
    parser.add_argument('--install-modules-info-file', required=True)
    parser.add_argument('--modules-info-file', required=True)
    parser.add_argument('--modules-list-file', required=True)
    parser.add_argument('--platform-installed-path', required=True)
    parser.add_argument('--system-dir', required=True)
    parser.add_argument('--sa-profile-extract-dir', required=True)
    parser.add_argument('--merged-sa-profile', required=True)
    parser.add_argument('--depfile', required=True)
    parser.add_argument('--system-image-zipfile', required=True)
    parser.add_argument(
        '--additional-system-files',
        action='append',
        help=
        "additional system files is used for files don't have module infos. use with caution"
    )
    parser.add_argument('--post-process-modules-info-files',
                        nargs='*',
                        default=[])
    args = parser.parse_args()

    additional_system_files = []
    for tuple in args.additional_system_files or []:
        filepath, system_path = tuple.split(':')
        additional_system_files.append((filepath, system_path))

    depfiles = []
    build_utils.extract_all(args.merged_sa_profile,
                            args.sa_profile_extract_dir,
                            no_clobber=False)
    sa_files = build_utils.get_all_files(args.sa_profile_extract_dir)

    system_install_info = read_json_file(args.system_install_info_file)
    if system_install_info is None:
        raise Exception("read file '{}' failed.".format(
            args.system_install_info_file))

    system_install_base_dir = args.system_dir
    if os.path.exists(system_install_base_dir):
        shutil.rmtree(system_install_base_dir)
        print('remove system dir...')
    os.makedirs(system_install_base_dir)

    vendor_install_base_dir = os.path.join(args.platform_installed_path,
                                           'vendor')
    if os.path.exists(vendor_install_base_dir):
        shutil.rmtree(vendor_install_base_dir)
        print('remove vendor dir...')

    updater_install_base_dir = os.path.join(args.platform_installed_path,
                                            'updater')
    if os.path.exists(updater_install_base_dir):
        shutil.rmtree(updater_install_base_dir)
        print('remove updater dir...')

    ramdisk_install_base_dir = os.path.join(args.platform_installed_path,
                                            'ramdisk')
    if os.path.exists(ramdisk_install_base_dir):
        shutil.rmtree(ramdisk_install_base_dir)
        print('remove ramdisk dir...')

    print('copy modules...')
    copy_modules(system_install_info, args.install_modules_info_file,
                 args.modules_info_file, args.modules_list_file,
                 args.post_process_modules_info_files,
                 args.platform_installed_path, additional_system_files,
                 depfiles)

    if os.path.exists(args.system_image_zipfile):
        os.unlink(args.system_image_zipfile)
    build_utils.zip_dir(args.system_image_zipfile, args.system_dir)
    depfiles.extend([item for item in depfiles if item not in sa_files])
    build_utils.write_depfile(args.depfile, args.install_modules_info_file,
                              depfiles)

    return 0


if __name__ == '__main__':
    sys.exit(main())
