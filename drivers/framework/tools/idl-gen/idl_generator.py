# !/usr/bin/env python3
# coding=utf-8
"""
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
*
* HDF is dual licensed: you can use it either under the terms of
* the GPL, or the BSD license, at your option.
* See the LICENSE file in the root of this repository for complete details.
"""

import argparse
import os
import re

import pip


def lost_module(module_name):
    print("""
need %s module, try install first:

    pip install %s""" % (module_name, module_name))
    exit()


try:
    import CppHeaderParser
    from _header_parser import HeaderParser
except ImportError:
    pip.main(["--disable-pip-version-check", "install", "robotpy-cppheaderparser"])
    try:
        import CppHeaderParser
        from _header_parser import HeaderParser
    except ImportError:
        HeaderParser = None
        lost_module("robotpy-cppheaderparser")


class IDLGenerator:
    def __init__(self):
        self._idl = ""
        self._output_path = ""
        self._file_list = []
        self._key_list = {}
        self._typedef_list = {}
        self._parse_results = {}

    def generate(self):
        try:
            self._parse_option()
            self._parse_header()
            for i in self._parse_results:
                self._generate_type(self._parse_results[i])
                self._generate_interface(self._parse_results[i])
        except KeyError:
            pass

    def _parse_option(self):
        parser = argparse.ArgumentParser(description="Compile C/C++ header files and generate .idl files.")
        parser.add_argument("-v", "--version", help="Display version information", action="store_true")
        parser.add_argument("-f", "--file", metavar="<*.h>", required=True, action='append',
                            help="Compile the C/C++ header file")
        parser.add_argument("-o", "--out", metavar="<directory>", default=".", required=True,
                            help="Place generated .idl files into the <directory>")
        args = parser.parse_args()

        self._file_list = args.file
        self._output_path = args.out

    def _search_file(self, root_path, file_name):
        file_list = os.listdir(root_path)
        for file in file_list:
            path = os.path.join(root_path, file)
            if os.path.isdir(path):
                file_path = self._search_file(path, file_name)
                if file_path is not None:
                    return file_path
            if os.path.isfile(path):
                if os.path.split(path)[1] == file_name:
                    return path

    def _parse_header(self):
        try:
            for file in self._file_list:
                root_path, _ = os.path.split(file)  # 输入文件所在目录
                file_list = [file]  # 需要解析的头文件列表,包括include包含的头文件
                while len(file_list) > 0:
                    include_file = []
                    for header_file in file_list:
                        result = HeaderParser().parse(header_file)
                        if result is not None:
                            self._parse_results[result.get("name")] = result
                            for file_name in result["import"]:  # 把include的文件加入列表
                                if file_name not in self._parse_results:  # 解析过的不重复解析
                                    file_path = self._search_file(root_path, file_name)
                                    if file_path is not None:
                                        include_file.append(file_path)
                    file_list = include_file

            for i in self._parse_results:
                for enum in self._parse_results[i]["enum"]:
                    self._key_list[enum["name"]] = "enum"
                for union in self._parse_results[i]["union"]:
                    self._key_list[union["name"]] = "union"
                for struct in self._parse_results[i]["struct"]:
                    self._key_list[struct["name"]] = "struct"
                for clas in self._parse_results[i]["interface"]:
                    self._key_list[clas["name"]] = ""
                for cb in self._parse_results[i]["callback"]:
                    self._key_list[cb["name"]] = ""
                for td in self._parse_results[i]["typedef"]:
                    self._typedef_list[td["name"]] = td["type"]
        except KeyError:
            pass

    def _generate_type(self, header):
        if self._has_user_define_type(header):
            self._install_package(header["path"])
            self._install_import(header)
            self._install_enum(header["enum"])
            self._install_stack(header["union"])
            self._install_stack(header["struct"])

            self._write_file(header["path"], "Types.idl")

    def _generate_interface(self, header):
        for iface in header["interface"]:
            self._install_package(header["path"])
            self._install_import(header)
            self._install_interface(iface)

            self._write_file(header["path"], iface["name"] + ".idl")

    def _install_package(self, file_path):
        self._idl += "package " + self._get_package(file_path) + ";\n\n"

    def _install_import(self, header):
        try:
            original_idl = self._idl
            for file_name in header["import"]:
                if file_name in self._parse_results:
                    include_file = self._parse_results[file_name]
                    if self._has_user_define_type(include_file):
                        tt = re.search("import\\s+\\S+.Types", self._idl)
                        if tt is None:
                            self._idl += "import " + self._get_package(include_file['path']) + ".Types;\n"
                    for iface in include_file["interface"]:
                        self._idl += "import " + self._get_package(include_file['path']) + ".%s;\n" % iface["name"]
                    for cb in include_file["callback"]:
                        self._idl += "import " + self._get_package(include_file['path']) + ".%s;\n" % cb["name"]
                else:
                    self._idl += "// can't import %s\n" % file_name
                    print("[IDLGenerator]: %s[line ?] can't find %s"
                          % (os.path.normpath(header["path"] + "/" + header["name"]), file_name))

            for cb in header['callback']:
                self._idl += "import " + self._get_package(header['path']) + ".%s;\n" % cb["name"]

            if original_idl != self._idl:
                self._idl += "\n"
        except KeyError:
            pass

    def _install_enum(self, enums):
        for enum in enums:
            self._idl += "enum %s {\n" % enum["name"]
            for member in enum["members"]:
                self._idl += "    %s = %s,\n" % (member["name"], member["value"])
            self._idl += "};\n"

    def _install_stack(self, stacks):
        for stack in stacks:
            self._idl += stack["type"] + " %s {\n" % stack["name"]
            for member in stack["members"]:
                param_type = self._swap_type_c2idl(member["type"])
                if "unknown type" in param_type:
                    print("[IDLGenerator]: %s[line %d] %s" % (
                        os.path.normpath(member["file_name"]), member["line_number"], param_type))
                self._idl += "    %s %s;\n" % (param_type, member["name"])
            self._idl += "};\n"

    def _install_interface(self, iface):
        if re.search("[Cc]all[Bb]ack", iface["name"]):
            self._idl += "[callback] "
        self._idl += "interface %s {\n" % iface["name"]
        for member in iface["members"]:
            self._install_function(iface["name"], member)
        self._idl += "}\n"

    def _install_function(self, iface_name, member):
        self._idl += "    %s(" % member["name"]
        for i, param in enumerate(member["params"]):
            tt = re.fullmatch(r"(enum)*(union)*(struct)* *%s *\** * *\**" % iface_name, param["type"])
            if tt:
                continue
            param_type = self._swap_type_c2idl(param["type"])
            if "unknown type" in param_type:
                print("[IDLGenerator]: %s[line %d] %s" % (
                    os.path.normpath(member["file_name"]), member["line_number"], param_type))
            self._idl += "%s %s %s," % (
                '* *' in param["type"] and "[out]" or "[in]",
                param_type,
                param["name"])
        if self._idl.endswith(','):
            self._idl = self._idl[:-1] + ");\n"
        else:
            self._idl += ");\n"

    @staticmethod
    def _convert_basic_type(c_type):
        type_c2idl = [
            [["bool"], "boolean"],
            [["int8_t", "char"], "byte"],
            [["int16_t"], "short"],
            [["int32_t", "int"], "int"],
            [["int64_t"], "long"],
            [["float"], "float"],
            [["double"], "double"],
            [["std::string", "string", "cstring"], "String"],
            [["uint8_t", "unsigned char"], "unsigned char"],
            [["uint16_t", "unsigned short"], "unsigned short"],
            [["uint32_t", "unsigned int"], "unsigned int"],
            [["uint64_t", "unsigned long"], "unsigned long"],
            [["void"], "void"]
        ]
        for cti in type_c2idl:
            for ct in cti[0]:
                tt = re.match(r"(const )* *%s *(\**) *" % ct, c_type)
                if tt:
                    idl_type = cti[1]
                    if c_type.count('*') == 1:
                        idl_type += '[]'
                    return idl_type
        return ""

    def _convert_structure(self, c_type):
        for type_name in self._key_list:
            if "_ENUM_POINTER" in c_type:
                c_type = c_type.replace("_ENUM_POINTER", " * ")
            tt = re.fullmatch(r"(const )* *(enum)*(union)*(struct)* *%s *[*&]* * *[*&]*" % type_name, c_type)
            if tt:
                if len(self._key_list.get(type_name)) > 0:
                    idl_type = self._key_list.get(type_name) + ' ' + type_name
                else:
                    idl_type = type_name
                if c_type.count('*') == 1:
                    idl_type += '[]'
                return idl_type
        return ""

    def _convert_typedef(self, c_type):
        for type_name in self._typedef_list:
            tt = re.match(r"(const )* *%s *\** *" % type_name, c_type)
            if tt:
                if self._typedef_list.get(type_name).count('*') == 1:
                    idl_type = self._typedef_list.get(type_name).split(' ')[0] + '[]'
                else:
                    idl_type = self._typedef_list.get(type_name)
                return idl_type
        return ""

    def _convert_container_type(self, c_type):
        type_pattern = " *(std::)*(struct *)*((unsigned *)*\\w+) *"
        tt = re.match("(const *)* *(std::)*map *<%s, %s>" % (type_pattern, type_pattern), c_type)
        if tt:
            key_type = self._convert_basic_type(tt[5])
            value_type = self._convert_basic_type(tt[9])
            return "Map<%s, %s>" % (key_type == "" and tt[5] or key_type, value_type == "" and tt[9] or value_type)

        tt = re.match("(const *)* *(std::)*vector *<%s>" % type_pattern, c_type)
        if tt:
            v_type = self._convert_basic_type(tt[5])
            return "List<%s>" % (v_type == "" and tt[5] or v_type)
        return ""

    def _swap_type_c2idl(self, c_type):
        idl_type = self._convert_basic_type(c_type)
        if idl_type != "":
            return idl_type

        idl_type = self._convert_structure(c_type)
        if idl_type != "":
            return idl_type

        idl_type = self._convert_typedef(c_type)
        if idl_type != "":
            return idl_type

        idl_type = self._convert_container_type(c_type)
        if idl_type != "":
            return idl_type

        idl_type = "/* unknown type: [%s] */" % c_type
        return idl_type

    def _write_file(self, file_path, file_name):
        file = os.path.join(self._make_output_dir(file_path), file_name).replace('\\', '/')
        with open(file, "w", encoding="utf8") as fp:
            fp.write(self._idl)
        print("Generate: --------------------- %s ---------------------\n" % os.path.normpath(file))
        self._idl = ""

    @staticmethod
    def _split_path(file_path):
        normpath = os.path.normpath(file_path)
        drive, out_path = os.path.splitdrive(normpath)
        out_path = out_path.replace('\\', '/')
        new_path = out_path.strip('.').strip('/')
        while new_path != out_path:
            out_path = new_path
            new_path = out_path.strip('.').strip('/')

        return new_path

    def _make_output_dir(self, file_path):
        output_dir = self._output_path + '/' + self._split_path(file_path)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        return output_dir

    @staticmethod
    def _has_user_define_type(h):
        if len(h["enum"]) > 0 or len(h["union"]) > 0 or len(h["struct"]) > 0:
            return True
        return False

    def _get_package(self, file_path):
        path_name = re.split(r'[/\\]', self._split_path(file_path))
        out_path = ""
        for name in path_name:
            out_path += name + '.'
        return out_path.strip('.')


if __name__ == "__main__":
    generator = IDLGenerator()
    generator.generate()
