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
import pathlib
import re

sys.path.append(
    os.path.dirname(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
from scripts.util.file_utils import write_file, read_json_file, \
    write_json_file  # noqa: E402
from scripts.util import build_utils  # noqa: E402

_SOURCE_ROOT = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..'))
# Import jinja2 from third_party/jinja2
sys.path.insert(1, os.path.join(_SOURCE_ROOT, 'third_party'))
from jinja2 import Template  # noqa: E402  # pylint: disable=F0401

KEYS = ['target_os', 'install_dir', 'module_label', 'build_only']


class SdkTargets:

    def __init__(self, os_type):
        self.os_type = os_type
        self.targets = []

    def add_target(self, target):
        if target not in self.targets:
            self.targets.append(target)

    def get_targets(self):
        return self.targets


def check_keys(keys):
    for key in keys:
        if key not in KEYS:
            raise Exception(
                'Error: failed to parse ohos sdk description file, missing {}.'
                .format(key))


def get_sdk_type(path_name):
    p = pathlib.Path(path_name)
    if path_name.startswith('/'):
        top_dir = p.parts[1]
    else:
        top_dir = p.parts[0]
    return top_dir


def add_target(item, target, sdk_systems):
    for _os in sdk_systems:
        if _os == 'linux' or _os == 'Linux':
            item.get('targets').get('linux').add_target('"%s",' % target)
        elif _os == 'windows' or _os == 'Windows':
            item.get('targets').get('windows').add_target('"%s",' % target)
        elif _os == 'darwin' or _os == 'Darwin':
            item.get('targets').get('darwin').add_target('"%s",' % target)


def write_sdk_build_gni(sdk_targets, build_only_targets, gni):
    template = Template(
        """#Generated code, DONOT modify it.
            ohos_sdk_modules = {
                {% for item in sdk_targets %}

                    {% set sdk_type = item.get('type') %}
                    {% set targets = item.get('targets') %}
                    {% set systems = targets.keys() %}
                    {% set _sdk_type = sdk_type.replace('-', '_') %}

                        {{ _sdk_type }} = {
                            {% for os in systems %}
                                {{ os }} = [
                                    {% for t in targets.get(os).get_targets() %}
                                        {{ t }}
                                    {% endfor %}
                                ]
                            {% endfor %}
                        }
                {% endfor %}

                extras = [
                {% for t in build_only_targets%}
                    "{{ t }}",
                {% endfor %}
                ]
            }
        """,  # noqa E501
        trim_blocks=True,
        lstrip_blocks=True)

    contents = template.render(
        sdk_targets=sdk_targets, build_only_targets=build_only_targets)
    write_file(gni, contents)


def get_build_gn(label):
    match = re.search(r"(.*?):(.*?)", label)
    if match:
        gn = '{}/BUILD.gn'.format(match.group(1))
        if gn.startswith("//"):
            return gn[len("//"):]
        else:
            return gn
    else:
        raise Exception("failed to get BUILD.gn of {}".format(label))


def variant_to_product(variant, options):
    relations = read_json_file(options.variant_to_product)
    if variant in relations.keys():
        return relations.get(variant)
    else:
        raise Exception('Error: failed to read {} in {}'.format(
            variant, options.variant_to_product))


def expand_platform_targets(options, label, install_dir):
    base = options.base_platform
    platforms = options.platforms
    variant = list(set(platforms) - set([base]))

    if label.find('${base}') != -1:
        return [label.replace('${base}', base)], [install_dir]
    elif label.find('${platforms}') != -1:
        return [label.replace('${platforms}', p) for p in platforms], [
            install_dir.replace('${platforms}',
                                variant_to_product(c, options))
            for c in platforms
        ]
    elif label.find('${variant}') != -1:
        return [label.replace('${variant}', c) for c in variant], [
            install_dir.replace('${variant}', variant_to_product(c, options))
            for c in variant
        ]
    else:
        return [label], [install_dir]


def parse_description_file(options):
    data = read_json_file(options.sdk_description_file)
    if data is None:
        raise Exception("read file '{}' failed.".format(
            options.sdk_description_file))

    module_install_infos = []
    sdk_types = []
    sdk_targets = []
    build_only_targets = []

    for d in data:
        check_keys(d.keys())

        label = d.get('module_label')
        install_dir = d.get('install_dir')
        build_only = d.get('build_only')

        # skip labels that we cannot find.
        rebased_build_gn = build_utils.rebase_path(
            get_build_gn(label), current_base=options.source_root_dir)
        if not os.path.exists(rebased_build_gn):
            continue

        if build_only:
            build_only_targets.append(label)
            continue

        module_labels, install_dirs = expand_platform_targets(
            options, label, install_dir)
        target_os = d.get('target_os')

        sdk_type = get_sdk_type(install_dir)
        if sdk_type not in sdk_types:
            sdk_targets.append({
                'type': sdk_type,
                'targets': {
                    'linux': SdkTargets('linux'),
                    'windows': SdkTargets('windows'),
                    'darwin': SdkTargets('darwin')
                }
            })
            sdk_types.append(sdk_type)
        for item in sdk_targets:
            if item['type'] == sdk_type:
                for m in module_labels:
                    add_target(item, m, target_os)

        for i in range(len(module_labels)):
            install_info = {
                'label': module_labels[i],
                'install_dir': install_dirs[i]
            }
            module_install_infos.append(install_info)

    return {
        "sdk_targets": sdk_targets,
        "install_infos": module_install_infos,
        "sdk_types": sdk_types,
        "build_only_targets": build_only_targets
    }


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--sdk-description-file', required=True)
    parser.add_argument('--sdk-install-info-file', required=True)
    parser.add_argument('--sdk-modules-gni', required=True)
    parser.add_argument('--sdk-types-file', required=True)
    parser.add_argument('--base-platform', required=True)
    parser.add_argument('--platforms', action='append', required=True)
    parser.add_argument('--source-root-dir', required=True)
    parser.add_argument('--variant-to-product', required=True)

    options = parser.parse_args()

    data = parse_description_file(options)

    write_sdk_build_gni(
        data.get('sdk_targets'), data.get('build_only_targets'),
        options.sdk_modules_gni)
    write_json_file(options.sdk_install_info_file, data.get('install_infos'))
    with open(options.sdk_types_file, 'w') as f:
        f.write('\n'.join(data.get('sdk_types')))


if __name__ == '__main__':
    sys.exit(main())
