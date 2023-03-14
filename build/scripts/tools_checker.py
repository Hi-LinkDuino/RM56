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
import subprocess


def run_command(cmd, verbose=None):
    if verbose:
        print("Running: {}".format(' '.join(cmd)))
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    output, _ = p.communicate()
    if verbose:
        print(output.decode().rstrip())
    return output, p.returncode


def package_installed(pkg_name):
    cmd = ['dpkg', '-s', pkg_name]
    _, r = run_command(cmd)
    return r


def check_build_repuried_packages():
    _build_package_list = [
        'binutils', 'flex', 'bison', 'git', 'build-essential', 'zip', 'curl',
        'unzip', 'm4', 'wget', 'perl', 'bc'
    ]
    for pkg in _build_package_list:
        if package_installed(pkg):
            print("\033[33m {} is not installed. please install it.\033[0m".
                  format(pkg))
            sys.exit(1)


def check_os_version():
    available_os = ('Ubuntu')
    available_releases = ('18.04', '20.04')
    _os_info, _returncode = run_command(['cat', '/etc/issue'])
    if _returncode != 0:
        return -1

    _os_info = _os_info.decode().rstrip().split()
    host_os = _os_info[0]
    host_version = _os_info[1]
    if host_os not in available_os:
        print("\033[33m OS {} is not supported for ohos build.\033[0m".format(
            host_os))
        return -1
    version_available = False
    for _version in available_releases:
        if host_version == _version or host_version.startswith(_version):
            version_available = True
            break
    if not version_available:
        print("\033[33m OS version {} is not supported for ohos build.\033[0m".
              format(host_version))
        print("\033[33m Avaliable OS version are {}.\033[0m".format(
            ', '.join(available_releases)))
        return -1
    return 0


def main():
    check_result = check_os_version()
    if check_result != 0:
        return

    check_build_repuried_packages()
    return 0


if __name__ == '__main__':
    sys.exit(main())
