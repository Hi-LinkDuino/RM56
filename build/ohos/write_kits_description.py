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

import argparse
import sys
import os
import zipfile
import json

sys.path.append(
    os.path.dirname(os.path.dirname(
        os.path.abspath(__file__))))
from scripts.util import build_utils  # noqa: E402


def get_java_sources_file(build_config):
    with open(build_config, 'r') as file:
        data = json.load(file)
        return data.get('deps_info').get('java_sources_file')


def main():
    parser = argparse.ArgumentParser()
    build_utils.add_depfile_option(parser)

    parser.add_argument('--input', required=True)
    parser.add_argument('--output', default=None)

    options = parser.parse_args()

    depfiles = []
    with open(options.input, 'r') as file:
        kits_info = json.load(file)
        for module in kits_info:
            if module.get('type') != 'jar':
                continue
            build_config = module.get('build_config')
            depfiles.append(build_config)
            module['source_list_file'] = get_java_sources_file(build_config)

        build_utils.write_json(kits_info, options.output)

    build_utils.write_depfile(options.depfile,
                             options.output,
                             depfiles,
                             add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main())
