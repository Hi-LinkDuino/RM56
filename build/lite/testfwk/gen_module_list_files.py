#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
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
#

import optparse
import os
import sys
import json
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from utils import makedirs


def get_args(args):
    parser = optparse.OptionParser()
    if hasattr(parser, 'add_option'):
        func = parser.add_option
    else:
        func = parser.add_argument
    func('--depfile', help='Path to depfile (refer to `gn help depfile`)')
    parser.add_option('--output_dir', help='output directory')
    parser.add_option('--source_dir', help='source directory')
    parser.add_option('--target', help='name of target')
    parser.add_option('--target_label')
    parser.add_option('--test_type')
    parser.add_option('--module_list_file', help='file name of module list')
    parser.add_option('--sources_file_search_root_dir', \
        help='root dir to search xx.sources files')
    parser.add_option('--sources', \
        help='case sources path defined in test template')
    options, _ = parser.parse_args(args)
    return options, parser


def main(args):
    options, _ = get_args(args)
    print("test module_list_file = {}".\
        format(os.path.dirname(options.module_list_file)))
    if not os.path.exists(os.path.dirname(options.module_list_file)):
        makedirs(os.path.dirname(options.module_list_file))
    with open(options.module_list_file, 'w') as module_list_file:
        contents = json.dumps([{
            'target': options.target,
            'label': options.target_label,
            'source_directory': options.source_dir,
            'output_directory': options.output_dir,
            'test_type': options.test_type
        }])
        module_list_file.write(contents)

    # create xx.sources file
    fold = os.path.join(options.sources_file_search_root_dir, \
        options.source_dir[(options.source_dir.rfind("../") + len("../")):])
    if not os.path.exists(fold):
        makedirs(fold)
    sources_file_name = fold[fold.rfind("/") + len("/"):] + ".sources"

    arg_sources = options.sources[0: (len(options.sources) - len(","))]
    with open(os.path.join(fold, sources_file_name), 'a') \
        as source_defined_file:
        list_sources = arg_sources.split(",")
        for source in list_sources:
            content = "{}/{}\n".format(
                os.path.dirname(options.source_dir), source)
            source_defined_file.write(content)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
