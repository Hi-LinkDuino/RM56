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

import sys
import os
import argparse
import subprocess
from utils import makedirs
import shutil
import zipfile


def cmd_popen(cmd):
    proc = subprocess.Popen(cmd)
    proc.wait()
    ret_code = proc.returncode
    if ret_code != 0:
        print("hap warning: {} failed, return code is {}".format(
            cmd, ret_code))


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--packing-tool-path', help='packing tool path ')
    parser.add_argument('--mode', help='package mode')
    parser.add_argument('--json-path', help='profile')
    parser.add_argument('--resources-path', help='resources')
    parser.add_argument('--assets-path', help='assets')
    parser.add_argument('--lib-path', help='lib')
    parser.add_argument('--shared-libs-path', help='shared-libs')
    parser.add_argument('--ability-so-path', help='ability so')
    parser.add_argument('--index-path', help='index')
    parser.add_argument('--unsignhap-path', help='unsignhap path')
    parser.add_argument('--force', help='force')
    parser.add_argument('--signtool-path', help='sign tool path')
    parser.add_argument('--signhap-path', help='sign hap path')
    parser.add_argument('--privatekey', help='privatekey')
    parser.add_argument('--sign-server', help='sign_server')
    parser.add_argument('--sign-algo', help='sign algo')
    parser.add_argument('--cert-profile', help='cert profile')
    parser.add_argument('--jks-path', help='jks path')
    parser.add_argument('--cert-path', help='cert path')
    parser.add_argument('--sign-by-server', help='sign mode')
    args = parser.parse_args()

    return args


def hap_packing(args):
    if not args.packing_tool_path:
        print('hap warning: packing tool path empty')
        return
    packing_cmd = ['java', '-jar', args.packing_tool_path]
    cmd_dict = {
        '--mode': args.mode,
        '--json-path': args.json_path,
        '--resources-path': args.resources_path,
        '--assets-path': args.assets_path,
        '--lib-path': args.lib_path,
        '--shared-libs-path': args.shared_libs_path,
        '--ability-so-path': args.ability_so_path,
        '--index-path': args.index_path,
        '--out-path': args.unsignhap_path,
        '--force': args.force,
        '--sign-by-server': args.sign_by_server
    }
    for key, value in cmd_dict.items():
        if value:
            packing_cmd.extend([key, value])
    cmd_popen(packing_cmd)


def hap_signing(args):
    user_name = ''
    password = ''
    if not args.signtool_path:
        print('hap warning: signing tool path empty')
        return

    # sign by server
    if args.sign_by_server == "True":
        if 'ONLINE_USERNAME' in os.environ:
            user_name = os.environ.get('ONLINE_USERNAME')
        else:
            print('hap warning: Environment variable ONLINE_USERNAME and ' +
                  'ONLINE_PASSWD are needed for app signning. ' +
                  'Please export it in bash.')
            return
        if 'ONLINE_PASSWD' in os.environ:
            password = os.environ.get('ONLINE_PASSWD')
        else:
            print('hap warning: Environment variable ONLINE_USERNAME and ' +
                  'ONLINE_PASSWD are needed for app signning. ' +
                  'Please export it in bash.')
            return
        signing_cmd = [
            'java', '-jar', args.signtool_path, 'sign', '-mode', 'remote',
            '-profileSigned', '1'
        ]
        cmd_dict = {
            '-privatekey': args.privatekey,
            '-server': args.sign_server,
            '-inputFile': args.unsignhap_path,
            '-outputFile': args.signhap_path,
            '-username': user_name,
            '-password': password,
            '-signAlg': args.sign_algo,
            '-profile': args.cert_profile
        }
    # sign by software.
    else:
        signtool_path = args.signtool_path
        #The default password of the key is 123456.
        # You are advised to use a key and certificate management tool (
        # such as keytool) to change the default password.
        # For details, see section "Application Signature Verification
        # Development Guide" in the Security Subsystem Development Guide.
        signing_cmd = [
            'java', '-jar', signtool_path, 'sign-app', '-mode', 'localsign',
            '-profileSigned', '1', '-keystorePwd', '123456',
            '-keyPwd', '123456','inForm','zip'
        ]
        cmd_dict = {
            '-keyAlias': args.privatekey,
            '-inFile': args.unsignhap_path,
            '-outFile': args.signhap_path,
            '-signAlg': args.sign_algo,
            '-profileFile': args.cert_profile,
            '-keystoreFile': args.jks_path,
            '-appCertFile': args.cert_path
        }
    for key, value in cmd_dict.items():
        if value:
            signing_cmd.extend([key, value])
    cmd_popen(signing_cmd)


def main():
    args = parse_args()

    # Workaround: hap packing tools multi-thread contention issue.
    makedirs(os.path.dirname(args.unsignhap_path), exist_ok=True)

    hap_packing(args)
    if os.path.exists(args.unsignhap_path):
        hap_signing(args)
        os.remove(args.unsignhap_path)


if __name__ == '__main__':
    sys.exit(main())
