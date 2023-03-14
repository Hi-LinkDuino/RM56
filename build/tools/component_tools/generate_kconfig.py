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
import argparse
import json
import os

KCONFIG_STR = 'config {}\n\
    bool "{}"\n\
    default n\n'
PROPERTIES_STR = 'config {}\n\
        string "{}"\n\
        default ""\n'
KMENU_STR = 'menu "{}"\n'

FEATURE_STR = 'config feature$$%s\n\
        string "feature"\n\
        default ""\n\
        depends on %s\n'


def create_config(name, comment):
    return KCONFIG_STR.format(name, comment)

def create_property(name, comment):
    return PROPERTIES_STR.format(name, comment)

def create_menu(name):
    return KMENU_STR.format(name)

def end_menu():
    return "endmenu\n"

def create_feature(name):
    return FEATURE_STR % (name, name)

def read_json(file):
    data = {}
    with open(file, "rb") as f:
        data = json.load(f)
    return data

def write_kconfig(result, outdir):
    outpath = os.path.join(outdir, "kconfig")
    with open(outpath, "w") as f:
        f.writelines(result)
    print("output file in: ", os.path.abspath(outpath))

def gen_kconfig(config_path, outdir):
    data = read_json(config_path)
    subsystems = data.get("subsystems")
    result = 'mainmenu "Subsystem Component Kconfig Configuration"\n'
    for prop, _ in data.items():
        if prop == "subsystems":
            continue
        result += create_property("property$${}".format(prop), prop)

    for subsys_dict in subsystems:
        subsys_name = subsys_dict.get("subsystem")
        result += create_menu(subsys_name)
        for component_dict in subsys_dict.get("components"):
            component_name = component_dict.get("component")
            result += create_config("{}$${}".format(subsys_name, component_name), component_name)
            result += create_feature("{}$${}".format(subsys_name, component_name))
        result += end_menu()
    write_kconfig(result, outdir)


if __name__ == "__main__":
    INTRO = 'Genenrate newly kconfig input file.\n\
        For exmaple: python3 generate_kconfig.py\n\
        or           python3 generate_kconfig.py --base_product={$repo}/productdefine/common/base/base_product.json --outdir=./'
    parser = argparse.ArgumentParser(
    formatter_class=argparse.RawDescriptionHelpFormatter,
    description=INTRO
    )
    parser.add_argument('--base_product', type=str, default="./../../../productdefine/common/base/base_product.json",
                        help='Basic config path in repo prodcutdefine,\
                                defalut={$repo}/productdefine/common/base/base_product.json')
    parser.add_argument('--outdir', type=str, default="./",
                        help="define output file path dir, default={$current_dir}")
    args = parser.parse_args()
    print("read base_product fle: ", os.path.abspath(args.base_product))
    gen_kconfig(args.base_product, args.outdir)
