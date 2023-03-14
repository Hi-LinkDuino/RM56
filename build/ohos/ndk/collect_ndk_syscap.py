#!/usr/bin/env python3
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

from io import RawIOBase
import sys
import os
import argparse

sys.path.append(
    os.path.dirname(os.path.dirname(os.path.dirname(
        os.path.abspath(__file__)))))
from scripts.util import build_utils  # noqa: E402
from scripts.util.file_utils import read_json_file, write_json_file  # noqa: E402, E501


def _get_system_capability(targets_build_config):
    _syscap_info = {}
    for _bc_file in targets_build_config:
        if not os.path.exists(_bc_file):
            continue
        _ndk_config_infos = read_json_file(_bc_file)
        if _ndk_config_infos is None:
            raise Exception("read file '{}' failed.".format(_bc_file))
        _ndk_config_info = _ndk_config_infos[0]
        _target_lable = _ndk_config_info.get('label')
        _bc_syscap = _ndk_config_info.get('system_capability')
        if not _bc_syscap or not _target_lable:
            continue
        _lib_name = _ndk_config_info.get('lib_name')
        _syscap_info[_lib_name] = _bc_syscap
    return _syscap_info


def run(args):
    _syscap_info = _get_system_capability(args.targets_build_config)
    write_json_file(args.system_capability_file, _syscap_info)

    if args.depfile:
        _dep_files = []
        for _bc_file in args.targets_build_config:
            if os.path.exists(_bc_file):
                _dep_files.append(_bc_file)
        list.sort(_dep_files)
        build_utils.write_depfile(args.depfile,
                                  args.system_capability_file,
                                  _dep_files,
                                  add_pydeps=False)


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument('--targets-build-config', nargs='+', required=True)
    parser.add_argument('--system-capability-file', required=True)
    parser.add_argument('--depfile', required=False)
    args = parser.parse_args(argv)
    return run(args)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
