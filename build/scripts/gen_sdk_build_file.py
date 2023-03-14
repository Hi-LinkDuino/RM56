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
import json
from interface_mgr import InterfaceMgr

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from scripts.util.file_utils import read_json_file, write_file, write_json_file  # noqa: E402 E501
from scripts.util import build_utils  # noqa: E402

SHARED_LIBRARY_BUILD_TEMPLATE = '''
config("%s_config") {
  include_dirs = [ "%s/include", ]
}
ohos_prebuilt_shared_library("%s") {
  source = "%s"
  public_configs = [ ":%s_config" ]
  subsystem_name = "%s"
  part_name = "%s"
  install_enable = false
}

'''
JAR_BUILD_TEMPLATE = '''
java_prebuilt("%s") {
  jar_path = "%s"
'''
MAPLE_BUILD_TEMPLATE = '''
ohos_maple_java_prebuilt("%s") {
  mplt = %s
  jar_path = "%s"
'''


# check sdk header file signature,
# skip check if file dir not exist
def check_header_files(checkfile_dir, subsystem_sdk_dir, subsystem_name,
                       module_name, interface_mgr):
    if checkfile_dir is None or not os.path.exists(checkfile_dir):
        return
    interface_mgr.check(checkfile_dir, subsystem_sdk_dir, subsystem_name,
                        module_name)


def get_build_config_from_label(label, current_toolchain_dir):
    build_config = label.replace('//', 'gen/').replace(':', '/')
    bc_file = '{}/{}.build_config'.format(current_toolchain_dir, build_config)
    return bc_file


def add_dynamic_deps(output, build_config):
    external_deps = build_utils.expand_file_args(
        ['@FileArg({}:deps_info:external_deps)'.format(build_config)])
    if external_deps[0] != '[  ]':
        output += "external_deps = " + external_deps[0]
    output += "}"
    return output


# Copy the SDK module to the sdk output directory,
# and generate the prebuilt template to BUILD.gn file.
def gen_build_file(input_file, sdk_out_dir, generate_sig,
                   signature_file_check_dir):
    data = read_json_file(input_file)
    if data is None:
        raise Exception(
            "sdk interface description info error, file [{}] does not exist.".
            format(input_file))

    build_file_content = 'import("//build/ohos.gni")\n'
    build_file_content += 'import("//build/config/ohos/rules.gni")\n'
    sdk_info = {}
    # interface check
    interface_mgr = InterfaceMgr()
    for module_desc in data:
        subsystem_name = module_desc.get('subsystem_name')
        part_name = module_desc.get('part_name')
        origin_name = module_desc.get('origin_name')
        module_type = module_desc.get('type')
        module_name = module_desc.get('name')
        module_info_file = module_desc.get('source_info')
        _current_toolchain_dir = module_desc.get('current_toolchain_dir')

        # read module_info.json file, get module source path
        source_data = read_json_file(module_info_file)
        if not source_data:
            print("read module [{}] info file failed, filename [{}].".format(
                module_name, module_info_file))
            continue

        # copy lib file
        source = source_data.get('source')
        if module_type == 'jar':
            source = source_data.get('alternative_source')

        # check sdk type consistency
        suffix = module_type
        if module_type == "none":
            continue
        # Don't check suffix for maple sdk.
        if module_type == "maple":
            pass
        elif not source.endswith(suffix):
            raise Exception(
                "sdk module [{}] type configuration is inconsistent.".format(
                    module_name))

        module_sdk_out_dir = os.path.join(sdk_out_dir, module_name)
        if not os.path.exists(module_sdk_out_dir):
            os.makedirs(module_sdk_out_dir)
        if module_type == "maple":
            pass
        else:
            shutil.copy(source, module_sdk_out_dir)

        # copy headers file
        header_base = module_desc.get('header_base')
        if not header_base:
            raise Exception("{}: header_base config error.".format(
                module_desc.get('label')))

        header_files = module_desc.get('header_files')
        # copy to sdk/{subsystem_name}/{module_name}/include/
        for header_file in header_files:
            rel_h_file = os.path.join(header_base, header_file)

            header_file_dest = os.path.join(module_sdk_out_dir, 'include',
                                            header_file)
            header_file_dest_dir = os.path.dirname(header_file_dest)
            if not os.path.exists(header_file_dest_dir):
                os.makedirs(header_file_dest_dir)

            shutil.copy(rel_h_file, header_file_dest)

        # generate or check header files
        if module_type == 'so' and generate_sig is False:
            check_header_files(signature_file_check_dir, sdk_out_dir,
                               origin_name, module_name, interface_mgr)

        # gen build file
        lib_file = os.path.join(module_name, os.path.basename(source))
        if module_type == 'so':
            build_file_content += SHARED_LIBRARY_BUILD_TEMPLATE % (
                module_name, module_name, module_name, lib_file, module_name,
                subsystem_name, part_name)
        elif module_type == 'jar':
            bc = get_build_config_from_label(source_data.get("label"),
                                             _current_toolchain_dir)
            build_file_content += JAR_BUILD_TEMPLATE % (module_name, lib_file)
            build_file_content = add_dynamic_deps(build_file_content, bc)
        elif module_type == 'maple':
            bc = get_build_config_from_label(source_data.get("label"),
                                             _current_toolchain_dir)
            bc_mplts = build_utils.expand_file_args(
                ['@FileArg({}:outputs:output_mplt)'.format(bc)])
            bc_mplts = build_utils.parse_and_flatten_gn_lists(bc_mplts)
            sdk_mplts = []
            base_dir = os.path.basename(module_sdk_out_dir)
            for mplt in bc_mplts:
                shutil.copy(mplt, module_sdk_out_dir)
                sdk_mplts.append(os.path.join(base_dir,
                                              os.path.basename(mplt)))

            bc_jar_path = build_utils.expand_file_args(
                ["@FileArg({}:deps_info:unprocessed_jar_path)".format(bc)])
            bc_jar_path = build_utils.parse_and_flatten_gn_lists(bc_jar_path)
            sdk_jar_path = []
            for jar in bc_jar_path:
                shutil.copy(jar, module_sdk_out_dir)
                sdk_jar_path.append(
                    os.path.join(base_dir, os.path.basename(jar)))
            build_file_content += MAPLE_BUILD_TEMPLATE % (
                module_name, json.dumps(sdk_mplts, sort_keys=True,
                                        indent=2), sdk_jar_path[0])
            build_file_content = add_dynamic_deps(build_file_content, bc)
        else:
            raise Exception(
                'The type of sdk module configuration is not supported.')

        module_info = {
            'source': lib_file,
            'label': module_desc.get('label'),
            'type': module_type
        }
        sdk_info[module_name] = module_info
    return build_file_content, sdk_info


def generate_sdk(input_file, sdk_out_dir, output_build_file, sdk_info_file,
                 generate_sig, signature_file_check_dir,
                 signature_file_gen_dir):
    build_file_content, sdk_info = gen_build_file(input_file, sdk_out_dir,
                                                  generate_sig,
                                                  signature_file_check_dir)
    # write file, build gn file
    write_file(output_build_file, build_file_content)
    # sdk info return
    write_json_file(sdk_info_file, sdk_info)

    # gen signature file
    if generate_sig is True:
        interface_mgr = InterfaceMgr()
        interface_mgr.gen_sig_file_by_subsystem(sdk_out_dir,
                                                signature_file_gen_dir)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--input-file', help='', required=True)
    parser.add_argument('--sdk-out-dir', help='', required=True)
    parser.add_argument('--output-build-file', help='', required=True)
    parser.add_argument('--sdk-info-file', help='', required=True)
    parser.add_argument('--generate-sig',
                        dest='generate_sig',
                        action='store_true')
    parser.set_defaults(generate_sig=False)
    parser.add_argument('--signature-file-check-dir', help='', required=False)
    parser.add_argument('--signature-file-gen-dir', help='', required=False)
    args = parser.parse_args()

    generate_sdk(args.input_file, args.sdk_out_dir, args.output_build_file,
                 args.sdk_info_file, args.generate_sig,
                 args.signature_file_check_dir, args.signature_file_gen_dir)
    return 0


if __name__ == '__main__':
    sys.exit(main())
