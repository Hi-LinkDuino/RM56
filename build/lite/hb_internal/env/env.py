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

from hb_internal import CONFIG_JSON
from hb_internal.common.utils import read_json_file
from hb_internal.common.utils import hb_info


def add_options(parser):
    pass


def exec_command(args):
    json_data = read_json_file(CONFIG_JSON)
    root_path = json_data.get('root_path', 'not set')
    board = json_data.get('board', 'not set')
    kernel = json_data.get('kernel', 'not set')
    product = json_data.get('product', 'not set')
    product_path = json_data.get('product_path', 'not set')
    device_path = json_data.get('device_path', 'not set')
    device_company = json_data.get('device_company', 'not set')

    hb_info('root path: {}'.format(root_path))
    hb_info('board: {}'.format(board))
    hb_info('kernel: {}'.format(kernel))
    hb_info('product: {}'.format(product))
    hb_info('product path: {}'.format(product_path))
    hb_info('device path: {}'.format(device_path))
    hb_info('device company: {}'.format(device_company))
