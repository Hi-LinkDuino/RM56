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

import os
from collections import defaultdict

from hb_internal.common.utils import read_json_file
from hb_internal.common.utils import OHOSException
from hb_internal.common.config import Config
from hb_internal.cts.menuconfig import Menuconfig
from hb_internal.cts.common import Separator


class Product():
    @staticmethod
    def get_products():
        config = Config()
        for company in os.listdir(config.vendor_path):
            company_path = os.path.join(config.vendor_path, company)
            if not os.path.isdir(company_path):
                continue

            for product in os.listdir(company_path):
                product_path = os.path.join(company_path, product)
                config_path = os.path.join(product_path, 'config.json')

                if os.path.isfile(config_path):
                    info = read_json_file(config_path)
                    product_name = info.get('product_name')
                    if product_name is not None:
                        yield {
                            'company': company,
                            "name": product_name,
                            'path': product_path,
                            'version': info.get('version', '3.0'),
                            'os_level': info.get('type', "mini"),
                            'config': config_path
                        }
        bip_path = config.built_in_product_path
        for item in os.listdir(bip_path):
            if item[0] in ".":
                continue
            else:
                product_name = item[0:-len('.json')] if item.endswith('.json') else item
                config_path = os.path.join(bip_path, item)
                info = read_json_file(config_path)
                yield {
                    'company': 'built-in',
                    "name": product_name,
                    'path': bip_path,
                    'version': info.get('version', '2.0'),
                    'os_level': info.get('type', 'standard'),
                    'config': config_path
                }

    @staticmethod
    def get_device_info(product_json):
        info = read_json_file(product_json)
        config = Config()
        version = info.get('version', '3.0')

        if version == '2.0':
            device_json = os.path.join(config.built_in_device_path,
                                       f'{info["product_device"]}.json')
            device_info = read_json_file(device_json)
            return {
                'board': device_info.get('device_name'),
                'kernel': device_info.get('kernel_type', 'linux'),
                'kernel_version': device_info.get('kernel_version'),
                'company': device_info.get('device_company'),
                'board_path': device_info.get('device_build_path'),
                'target_cpu': device_info.get('target_cpu'),
                'target_os': device_info.get('target_os')
            }
        elif version == '3.0':
            device_company = info.get('device_company')
            board = info.get('board')
            board_path = os.path.join(config.root_path, 'device',
                                      device_company, board)
            # board and soc decoupling feature will add boards directory path here.
            if not os.path.exists(board_path):
                board_path = os.path.join(config.root_path, 'device', 'board',
                                          device_company, board)

            return {
                'board': info.get('board'),
                'kernel': info.get('kernel_type'),
                'kernel_version': info.get('kernel_version'),
                'company': info.get('device_company'),
                'board_path': board_path,
                'target_cpu': info.get('target_cpu'),
                'target_os': info.get('target_os')
            }
        else:
            raise OHOSException(f'wrong version number in {product_json}')

    @staticmethod
    def get_features(product_json):
        if not os.path.isfile(product_json):
            raise OHOSException(f'features {product_json} not found')

        features_list = []
        config = Config()
        if config.version == '3.0':
            subsystems = read_json_file(product_json).get('subsystems', [])
            for subsystem in subsystems:
                for component in subsystem.get('components', []):
                    features = component.get('features', [])
                    features_list += [
                        feature for feature in features if len(feature)
                    ]
        elif config.version == '2.0':
            features = read_json_file(product_json).get('parts', {}).values()
            for feature in features:
                if feature.get('features'):
                    features_kv = feature.get('features')
                    for key, val in features_kv.items():
                        _item = ''
                        if isinstance(val, bool):
                            _item = f'{key}={str(val).lower()}'
                        elif isinstance(val, int):
                            _item = f'{key}={val}'
                        elif isinstance(val, str):
                            _item = f'{key}="{val}"'
                        else:
                            raise Exception(
                                "part feature '{key}:{val}' type not support.")
                        features_list.append(_item)

        return features_list

    @staticmethod
    def get_components(product_json, subsystems):
        if not os.path.isfile(product_json):
            raise OHOSException(f'{product_json} not found')

        components_dict = defaultdict(list)
        product_data = read_json_file(product_json)
        for subsystem in product_data.get('subsystems', []):
            sname = subsystem.get('subsystem', '')
            if not len(subsystems) or sname in subsystems:
                components_dict[sname] += [
                    comp['component']
                    for comp in subsystem.get('components', [])
                ]

        return components_dict, product_data.get('board', ''),\
            product_data.get('kernel_type', '')

    @staticmethod
    def get_product_info(product_name, company=None):
        for product_info in Product.get_products():
            cur_company = product_info['company']
            cur_product = product_info['name']
            if company:
                if cur_company == company and cur_product == product_name:
                    return product_info
            else:
                if cur_product == product_name:
                    return product_info

        raise OHOSException(f'product {product_name}@{company} not found')

    @staticmethod
    def product_menuconfig():
        product_path_dict = {}
        company_separator = None
        for product_info in Product.get_products():
            company = product_info['company']
            product = product_info['name']
            if company_separator is None or company_separator != company:
                company_separator = company
                product_key = Separator(company_separator)
                product_path_dict[product_key] = None

            product_path_dict['{}@{}'.format(product, company)] = product_info

        if not len(product_path_dict):
            raise OHOSException('no valid product found')

        choices = [
            product if isinstance(product, Separator) else {
                'name': product.split('@')[0],
                'value': product.split('@')[1]
            } for product in product_path_dict.keys()
        ]
        menu = Menuconfig()
        product = menu.list_promt('product', 'Which product do you need?',
                                  choices).get('product')
        product_key = f'{product[0]}@{product[1]}'
        return product_path_dict[product_key]
