#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
#
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.

import json
import os
import re
import sys


class IdlType(object):
    INTERFACE = 1
    CALLBACK = 2
    TYPES = 3


def translate_file_name(file_name):
    name = file_name[1:] if file_name.startswith("I") else file_name
    translate_name = ""
    num = 0
    for c in name:
        if c >= 'A' and c <= 'Z':
            if num > 1:
                translate_name += "_"
            translate_name += c.lower()
        else:
            translate_name += c
        num += 1
    return translate_name


def get_idl_file_type(file_path):
    idl_type = IdlType.TYPES
    idl_file = open(file_path, "r")
    file_str = idl_file.read()
    idl_file.close()

    # delete comment information
    file_str = re.sub(r'//[^\r\n]*', "\n", file_str)
    file_str = re.sub(r'(/\*[\s|\S]*?\*/)', "\n", file_str)

    interface_index = file_str.find("interface")
    if interface_index != -1:
        if file_str.find("[callback]", 0, interface_index) != -1:
            idl_type = IdlType.CALLBACK
        else:
            idl_type = IdlType.INTERFACE
    return idl_type


def c_interface_file_translate(idl_file, out_dir, part, outputs):
    get_file_name = idl_file.split("/")[-1]
    file_name = translate_file_name(get_file_name.split(".")[0])

    iface_header_file = os.path.join(out_dir, "i" + file_name + ".h")
    client_proxy_source_file = os.path.join(out_dir, file_name + "_proxy.c")
    server_driver_source_file = os.path.join(out_dir, file_name + "_driver.c")
    server_stub_header_file = os.path.join(out_dir, file_name + "_stub.h")
    server_stub_source_file = os.path.join(out_dir, file_name + "_stub.c")
    server_impl_header_file = os.path.join(out_dir, file_name + "_service.h")
    server_impl_source_file = os.path.join(out_dir, file_name + "_service.c")

    if part == "-c":
        outputs.append(iface_header_file)
        outputs.append(client_proxy_source_file)
    elif part == "-s":
        outputs.append(iface_header_file)
        outputs.append(server_stub_header_file)
        outputs.append(server_stub_source_file)
    else:
        outputs.append(iface_header_file)
        outputs.append(client_proxy_source_file)
        outputs.append(server_driver_source_file)
        outputs.append(server_stub_header_file)
        outputs.append(server_stub_source_file)
        outputs.append(server_impl_header_file)
        outputs.append(server_impl_source_file)


def c_callback_file_translate(idl_file, out_dir, part, outputs):
    get_file_name = idl_file.split("/")[-1]
    file_name = translate_file_name(get_file_name.split(".")[0])

    iface_header_file = os.path.join(out_dir, "i" + file_name + ".h")
    client_proxy_source_file = os.path.join(out_dir, file_name + "_proxy.c")
    server_stub_header_file = os.path.join(out_dir, file_name + "_stub.h")
    server_stub_source_file = os.path.join(out_dir, file_name + "_stub.c")
    server_impl_header_file = os.path.join(out_dir, file_name + "_service.h")
    server_impl_source_file = os.path.join(out_dir, file_name + "_service.c")

    if part == "-c":
        outputs.append(iface_header_file)
        outputs.append(server_stub_header_file)
        outputs.append(server_stub_source_file)
    elif part == "-s":
        outputs.append(iface_header_file)
        outputs.append(client_proxy_source_file)
    else:
        outputs.append(iface_header_file)
        outputs.append(client_proxy_source_file)
        outputs.append(server_stub_header_file)
        outputs.append(server_stub_source_file)
        outputs.append(server_impl_header_file)
        outputs.append(server_impl_source_file)


def c_types_file_translate(idl_file, out_dir, outputs):
    get_file_name = idl_file.split("/")[-1]
    file_name = translate_file_name(get_file_name.split(".")[0])

    types_header_file = os.path.join(out_dir, file_name + ".h")
    types_source_file = os.path.join(out_dir, file_name + ".c")

    outputs.append(types_header_file)
    outputs.append(types_source_file)


def cpp_interface_file_translate(idl_file, out_dir, part, outputs):
    get_file_name = idl_file.split("/")[-1]
    file_name = translate_file_name(get_file_name.split(".")[0])

    iface_header_file = os.path.join(out_dir, "i" + file_name + ".h")
    client_proxy_header_file = os.path.join(out_dir, file_name + "_proxy.h")
    client_proxy_source_file = os.path.join(out_dir, file_name + "_proxy.cpp")
    server_driver_source_file = os.path.join(out_dir, file_name + "_driver.cpp")
    server_stub_header_file = os.path.join(out_dir, file_name + "_stub.h")
    server_stub_source_file = os.path.join(out_dir, file_name + "_stub.cpp")
    server_impl_header_file = os.path.join(out_dir, file_name + "_service.h")
    server_impl_source_file = os.path.join(out_dir, file_name + "_service.cpp")

    if part == "-c":
        outputs.append(iface_header_file)
        outputs.append(client_proxy_header_file)
        outputs.append(client_proxy_source_file)
    elif part == "-s":
        outputs.append(iface_header_file)
        outputs.append(server_stub_header_file)
        outputs.append(server_stub_source_file)
    else:
        outputs.append(iface_header_file)
        outputs.append(client_proxy_header_file)
        outputs.append(client_proxy_source_file)
        outputs.append(server_driver_source_file)
        outputs.append(server_stub_header_file)
        outputs.append(server_stub_source_file)
        outputs.append(server_impl_header_file)
        outputs.append(server_impl_source_file)


def cpp_callback_file_translate(idl_file, out_dir, part, outputs):
    get_file_name = idl_file.split("/")[-1]
    file_name = translate_file_name(get_file_name.split(".")[0])

    iface_header_file = os.path.join(out_dir, "i" + file_name + ".h")
    client_proxy_header_file = os.path.join(out_dir, file_name + "_proxy.h")
    client_proxy_source_file = os.path.join(out_dir, file_name + "_proxy.cpp")
    server_stub_header_file = os.path.join(out_dir, file_name + "_stub.h")
    server_stub_source_file = os.path.join(out_dir, file_name + "_stub.cpp")
    server_impl_header_file = os.path.join(out_dir, file_name + "_service.h")
    server_impl_source_file = os.path.join(out_dir, file_name + "_service.cpp")

    if part == "-c":
        outputs.append(iface_header_file)
        outputs.append(server_stub_header_file)
        outputs.append(server_stub_source_file)
    elif part == "-s":
        outputs.append(iface_header_file)
        outputs.append(client_proxy_header_file)
        outputs.append(client_proxy_source_file)
    else:
        outputs.append(iface_header_file)
        outputs.append(client_proxy_header_file)
        outputs.append(client_proxy_source_file)
        outputs.append(server_stub_header_file)
        outputs.append(server_stub_source_file)
        outputs.append(server_impl_header_file)
        outputs.append(server_impl_source_file)


def cpp_types_file_translate(idl_file, out_dir, outputs):
    get_file_name = idl_file.split("/")[-1]
    file_name = translate_file_name(get_file_name.split(".")[0])

    types_header_file = os.path.join(out_dir, file_name + ".h")
    types_source_file = os.path.join(out_dir, file_name + ".cpp")

    outputs.append(types_header_file)
    outputs.append(types_source_file)


def c_get_compile_source_file(idl_files, out_dir, part):
    outputs = []
    for idl_file in idl_files:
        idl_file_type = get_idl_file_type(idl_file)
        if idl_file_type == IdlType.INTERFACE:
            c_interface_file_translate(idl_file, out_dir, part, outputs)
        elif idl_file_type == IdlType.CALLBACK:
            c_callback_file_translate(idl_file, out_dir, part, outputs)
        elif idl_file_type == IdlType.TYPES:
            c_types_file_translate(idl_file, out_dir, outputs)
    return outputs


def cpp_get_compile_source_file(idl_files, out_dir, part):
    outputs = []
    for idl_file in idl_files:
        idl_file_type = get_idl_file_type(idl_file)
        if idl_file_type == IdlType.INTERFACE:
            cpp_interface_file_translate(idl_file, out_dir, part, outputs)
        elif idl_file_type == IdlType.CALLBACK:
            cpp_callback_file_translate(idl_file, out_dir, part, outputs)
        elif idl_file_type == IdlType.TYPES:
            cpp_types_file_translate(idl_file, out_dir, outputs)
    return outputs


def get_compile_source_file(idl_files, language, out_dir, part):
    outputs = []
    if language == "c":
        outputs = c_get_compile_source_file(idl_files, out_dir, part)
    elif language == "cpp":
        outputs = cpp_get_compile_source_file(idl_files, out_dir, part)
    return outputs


# ./build_hdi_files.py -o <language> <gen_dir> <part> <idl_files>
def get_output_files(argv):
    result = []
    if len(argv) >= 5:
        language = argv[2]
        gen_dir = argv[3]
        part = argv[4]
        idl_files = argv[5:]

        result = get_compile_source_file(idl_files, language, gen_dir, part)
    sys.stdout.write('\n'.join(result))


# parse package name of this idl file
def parse_file_package(file_path):
    package_str = ""
    idl_file = open(file_path, "r")
    file_str = idl_file.read()
    result = re.findall(r'package\s(\w+(?:\.\w+)*);', file_str)
    if (len(result) > 0):
        package_str = result[0]
    idl_file.close()
    return package_str


# search all idl files and parse package name
def parse_package(idl_files):
    package_str = ""
    for idl_file in idl_files:
        idl_file_type = get_idl_file_type(idl_file)
        if idl_file_type == IdlType.INTERFACE:
            package_str = parse_file_package(idl_file)
            break
    return package_str


# parse version from package name
def parse_version(package_str):
    # the version is empty by default
    version_str = ""
    result = re.findall(r'\w+(?:\.\w+)*\.[V|v](\d+)_(\d+)', package_str)
    if len(result) > 0:
        major_version = result[0][0]
        minor_version = result[0][1]
        version_str = str(major_version) + "." + str(minor_version)
    return version_str


def parse_out_dir(gen_dir, package_str, path_map):
    package_path_vec = path_map.split(":")
    if (len(package_path_vec) != 2):
        return gen_dir

    root_package = package_path_vec[0]
    root_package_path = package_path_vec[1]

    temp_dir = package_str.replace(root_package, root_package_path)
    temp_dir = temp_dir.replace(".", "/")
    return gen_dir.replace(temp_dir, "")


def parse_root_package_dir(gen_dir, package_str, path_map):
    package_path_vec = path_map.split(":")
    if (len(package_path_vec) != 2):
        return gen_dir
    
    root_package = package_path_vec[0]
    root_package_path = package_path_vec[1]

    temp_dir = package_str.replace(root_package, "")
    temp_dir = temp_dir.replace(".", "/")
    return gen_dir.replace(temp_dir, "")


def parse_version_dir(gen_dir, version_str):
    version_dir_name = "v" + version_str.replace(".", "_")
    return gen_dir.replace(version_dir_name, "")


# get information of package, version and generation files from idl files
# ./build_hdi_files.py -i <language> <gen_dir> <path_mapping> <idl_files>
def get_information(argv):
    result = {}
    if len(argv) >= 5:
        language = argv[2]
        gen_dir = argv[3]
        path_map = argv[4]
        idl_files = argv[5:]

        package_str = parse_package(idl_files)
        version_str = parse_version(package_str)

        out_dir = parse_out_dir(gen_dir, package_str, path_map)

        root_package_dir = parse_root_package_dir(gen_dir, package_str, path_map)

        version_dir = parse_version_dir(gen_dir, version_str)

        result = {
            "package" : package_str,
            "version" : version_str,
            "out_dir" : out_dir,
            "root_package_dir" : root_package_dir,
            "version_dir" : version_dir,
        }
    sys.stdout.write(json.dumps(result))


if __name__ == "__main__":
    if len(sys.argv) < 1:
        sys.stdout.write('\n')
    option = sys.argv[1]
    if option == "-i":
        get_information(sys.argv)
    elif option == "-o":
        get_output_files(sys.argv)
    else:
        sys.stdout.write('\n')