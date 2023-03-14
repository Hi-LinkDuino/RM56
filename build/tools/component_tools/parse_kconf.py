#!/usr/bin/python3
"""
Copyright (c) 2021 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

"""
import json
import argparse
import os

SUBSYS_LIST = "subsystems"
SUBSYS = "subsystem"
COMPONENT_LIST = "components"
COMPONENT = "component"
FEATRUES = "features"
FEATRUE = "feature"
NONE_FEATURES = []
TYPE = "type"
VALUE = "value"
PROPERTY = "property"
KCONF_PREFIX = "CONFIG_"


def read_full_component(path):
    data = {}
    with open(path, "rb") as f:
        data = json.load(f)
    component_subsys_dict = {}
    for item in data.get(SUBSYS_LIST):
        subsys_name = item.get(SUBSYS)
        for component in item.get(COMPONENT_LIST):
            component_name = component.get(COMPONENT)
            component_subsys_dict[component_name] = subsys_name
    return component_subsys_dict


def read_deps_info(path):
    data = {}
    with open(path, "rb") as f:
        data = json.load(f)
    return data


def merge(component_subsys_dict, component_deps_dict, subsys_conf):
    config_component_list = []
    for _, components in subsys_conf.items():
        for component, _ in components.items():
            config_component_list.append(component)

    for component_name in config_component_list:
        deps = component_deps_dict.get(component_name)
        if deps is not None:
            for dep in deps:
                if dep in config_component_list:
                    continue
                else:
                    subsys_name = component_subsys_dict.get(dep)
                    if subsys_name is None:
                        continue
                    if subsys_conf.get(subsys_name) is None:
                        subsys_conf[subsys_name] = {dep: NONE_FEATURES}
                    else:
                        subsys_conf[subsys_name][dep] = NONE_FEATURES
                    config_component_list.append(dep)


def is_valid_line(line):
    return line.startswith(KCONF_PREFIX)


def handle_config_feature(items, arr, line):
    items[TYPE] = FEATRUE
    items[SUBSYS] = arr[1]
    items[COMPONENT] = arr[2].split("=")[0]
    features = line.split("=", 1)[1].strip("\"")
    if features == "":
        items[FEATRUES] = NONE_FEATURES
    else:
        items[FEATRUES] = features.split(",")


def handle_config_component(items, arr, line):
    items[TYPE] = COMPONENT
    items[SUBSYS] = arr[0].split("_", 1)[1]
    items[COMPONENT] = arr[1].split("=")[0]
    items[VALUE] = line.split("=", 1)[1]


def handle_config_property(items, arr, line):
    items[PROPERTY] = arr[1].split("=")[0]
    items[TYPE] = PROPERTY
    items[VALUE] = line.split("=", 1)[1].strip("\"")


def read_line_item(line):
    line = line.strip()
    items = {
        TYPE: None,
        SUBSYS: None,
        FEATRUES: None,
        COMPONENT: None,
        PROPERTY: None,
        VALUE: None
    }
    arr = line.split("$$")
    if arr[0] == "CONFIG_feature":
        handle_config_feature(items, arr, line)
    elif arr[0] == "CONFIG_property":
        handle_config_property(items, arr, line)
    else:
        handle_config_component(items, arr, line)
    return items


def read(kconf):
    result, subsys_conf = {}, {}
    with open(kconf, "r") as f:
        for line in f.readlines():
            if not is_valid_line(line):
                continue
            items = read_line_item(line)
            subsys = items.get(SUBSYS)
            component = items.get(COMPONENT)
            conf_type = items.get(TYPE)
            features = items.get(FEATRUES)
            if conf_type == PROPERTY:
                if len(items.get(VALUE)) > 0:
                    result[items.get(PROPERTY)] = items.get(VALUE)
            elif conf_type == FEATRUE or conf_type == COMPONENT:
                if subsys_conf.get(subsys) is None:
                    subsys_conf[subsys] = {}
                subsys_conf.get(subsys)[component] = features

    result[SUBSYS_LIST] = subsys_conf
    return result


def generate_config_with_full_deps(deps_path, base_product_path, config_path, out):
    kconf = read(config_path)
    subsys_conf = kconf.get(SUBSYS_LIST)
    component_subsys_dict = read_full_component(base_product_path)
    component_deps_dict = read_deps_info(deps_path)
    merge(component_subsys_dict, component_deps_dict, subsys_conf)
    subsystems_list = []
    for subsys, components in subsys_conf.items():
        temp = {SUBSYS: subsys, COMPONENT_LIST: []}
        for component, features in components.items():
            component_element = {COMPONENT: component, FEATRUES: features}
            temp.get(COMPONENT_LIST).append(component_element)
        subsystems_list.append(temp)
    result = kconf
    result[SUBSYS_LIST] = subsystems_list
    with open(out, "w") as f:
        f.write(json.dumps(result, indent=2))
    print("output file in: ", os.path.abspath(out))


def main():
    intro = 'Genenrate newly kconfig input file.\n\
        For exmaple: python3 parse_kconf.py --deps={$repo}/out/{your_product}/part_deps_info/part_deps_info.json \n\
        or           python3 parse_kconf.py --base_product={$repo}/productdefine/common/base/base_product.json \
--out=./product.json --deps={$repo}/out/{your_product}/part_deps_info/part_deps_info.json --config=./.config'
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=intro)
    parser.add_argument('--deps', type=str, required=True, default="",
                        help='Must Requried! Depencencies info after gn building')
    parser.add_argument('--base_product', type=str, default="./../../../productdefine/common/base/base_product.json",
                        help='base_product.json file')
    parser.add_argument('--config', type=str, default="./.config",
                        help='config file which is generated by kconfig operation, default is .config')
    parser.add_argument('--out', type=str, default="./product.json",
                        help="define output file path and name, like './product.json'")
    args = parser.parse_args()
    print("read deps fle: ", os.path.abspath(args.deps))
    print("read kconfig fle: ", os.path.abspath(args.config))
    print("read base_product fle: ", os.path.abspath(args.base_product))

    generate_config_with_full_deps(args.deps, args.base_product, args.config, args.out)


if __name__ == "__main__":
    main()
