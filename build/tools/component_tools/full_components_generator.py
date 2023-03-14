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
import os
import json
import argparse

"""
@Desc:
  This script is used to generate full components
  example for creating new products

@GUID:
basic:
  cmd1: cd ${repo_root}
  cmd2: python3 ./build/tools/component_tools/full_components_generator.py
  base_product.json will be created in ./productdefine/common/base

advanced:
  cmd: python3 full_components_generator.py -h
  Get more infomation

@Date 2022/01/14
"""

def find_files(path, name):
    ret, files, folders = [], [], [path]
    for folder in folders:
        for file in os.listdir(folder):
            abs_file = os.path.join(folder, file)
            if str(file) == name:
                files.append(abs_file)
            if os.path.isdir(abs_file):
                folders.append(abs_file)
    
    for file in files:
        if len(file) > 0 and os.path.exists(file):
            ret.append(file)
    return ret


def read_component_from_ohos_build(file):
    ret = {"subsystem": "", "components": []}
    with open(file, "rb") as f:
        data = json.load(f)
        ret["subsystem"] =  data.get("subsystem")
        for k, _ in data.get("parts").items():
            ret.get("components").append(k)
    return ret


def read_component_from_bundle_json(file):
    ret = {"subsystem": "", "components": []}
    with open(file, "rb") as f:
        data = json.load(f)
        ret["subsystem"] = data.get("component").get("subsystem")
        ret.get("components").append(data.get("component").get("name"))
    return ret


def find_component_in_path(subsys, path):
    ret = set()
    if not os.path.exists(path):
        return []
    files_ohos_build = find_files(path, "ohos.build")
    files_bundle_json = find_files(path, "bundle.json")
    for ohos_build in files_ohos_build:
        data = read_component_from_ohos_build(ohos_build)
        if data.get("subsystem") == subsys:
            ret = ret.union(set(data.get("components")))

    for bundle_json in files_bundle_json:
        data = read_component_from_bundle_json(bundle_json)
        if data.get("subsystem") == subsys:
            ret = ret.union(set(data.get("components")))
    return ret


def update_components(subsys_file):
    ret = {"subsystems": []}
    with open(subsys_file, "rb") as f:
        data = json.load(f)
        for subsys, v in data.items():
            components_path = v.get("path")
            parts = find_component_in_path(subsys, components_path)
            components = []
            for part in parts:
                components.append({"component": part, "features": []})
            ret.get("subsystems").append(
                {"subsystem": subsys, "components": components})
    return ret


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--subsys', type=str, default="./build/subsystem_config.json",
                        help='subsystem config file location, default=//build/subsystem_config.json')
    parser.add_argument('--out', type=str, default="./productdefine/common/base/base_product.json",
                        help='base_config output path default //productdefine/common/base')
    args = parser.parse_args()
    # Only for version 3.0 config
    # 'device_name' has been replaced by 'board' in 3.0
    # device info has been merged into product in 3.0
    # 'target cpu' need to be arm intead of arm64 due to adaption work has not been done
    ret = {
        "product_name": "ohos-arm64",
        "version": "3.0",
        "type": "standard",
        "ohos_version": "OpenHarmony 3.x",
        "board": "arm64",
        "kernel_type": "",
        "kernel_version": "",
        "device_name": "arm64",
        "device_company": "openharmony",
        "target_os": "ohos",
        "target_cpu": "arm",
        "subsystems": []
    }
    data = update_components(args.subsys)
    ret["subsystems"] = data.get("subsystems")
    with open(args.out, "w") as f:
        f.write(json.dumps(ret, indent=2))
    print("file has generated in path: {}".format(args.out))


if __name__ == '__main__':
    main()
