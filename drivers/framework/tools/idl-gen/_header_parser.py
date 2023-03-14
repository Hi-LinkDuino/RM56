# !/usr/bin/env python3
# coding=utf-8
"""
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
*
* HDF is dual licensed: you can use it either under the terms of
* the GPL, or the BSD license, at your option.
* See the LICENSE file in the root of this repository for complete details.
"""

import json
import os
import re
from shutil import copyfile, move

import CppHeaderParser


class HeaderParser:
    """
    Extract file path, file name, includes, enums, unions, structs, interfaces and callbacks
    from CppHeaderParser
    """

    def __init__(self):
        self._header_dict = {
            "name": "",
            "path": "",
            "import": [],
            "enum": [],
            "union": [],
            "struct": [],
            "typedef": [],
            "interface": [],
            "callback": []
        }
        self._rand_name_count = 0

    def parse(self, header_file):
        try:
            hjson = json.loads(CppHeaderParser.CppHeader(header_file).toJSON())
        except CppHeaderParser.CppParseError:
            back_file = self._pre_handle(header_file)
            hjson = json.loads(CppHeaderParser.CppHeader(header_file).toJSON())
            move(back_file, header_file)

        try:
            self._extract_path_and_file(header_file)
            self._extract_include(hjson["includes"])
            self._extract_enum(hjson["enums"])
            for i in hjson["classes"]:
                self._extract_union(hjson["classes"][i])
                self._extract_struct(hjson["classes"][i])
                self._extract_interface(hjson["classes"][i])
            self._extract_typedef(hjson["typedefs"])
            return self._header_dict
        except KeyError:
            pass

    @staticmethod
    def _pre_handle(header_file):
        back_file = header_file + ".back"
        copyfile(header_file, back_file)
        f = open(header_file, 'r')
        new_lines = ""
        for line in f:
            tt = re.match(r".*enum *((\w+) *\*) *\w* *;", line)
            if tt:
                new_line = line.replace(tt[1], tt[2] + "_ENUM_POINTER ")
                new_lines += new_line
                continue
            tt = re.match(r".*(\[\w* *\(.*\) *]) *", line)
            if tt:
                new_line = line.replace(tt[1], "[]")
                new_lines += new_line
                continue
            else:
                new_lines += line
        f.close()
        f = open(header_file, 'w')
        f.writelines(new_lines)
        f.close()
        return back_file

    @staticmethod
    def _has_function_pointer(jvs):
        for jv in jvs:
            if jv["function_pointer"] > 0:
                return True
        return False

    def _checkout_function_pointer_param(self, params):
        if params == '':
            return []
        tt = re.match(r"(typedef )* *\w+ \( \* \) \(( .* )\)", params)
        if tt:
            params = tt[2].strip() + ","
        else:
            print("error cant analyze function pointer params: ", params)
            return []
        ret = []
        while params:
            tt = re.match(" *(const )*(struct |enum |union )* *", params)
            if tt:  # 去掉结构类型头
                params = params[tt.regs[0][1]:]
                tt = re.match(r"((unsigned )*[a-zA-Z0-9_:]+( *\** *\**)) *", params)
                if tt:
                    param_type = params[tt.regs[1][0]:tt.regs[1][1]]  # 参数类型
                    params = params[tt.regs[0][1]:]
                    tt = re.match("([a-zA-Z0-9_]+) *(,)", params)
                    if tt:
                        param_name = params[tt.regs[1][0]:tt.regs[1][1]]
                        params = params[tt.regs[0][1]:]
                    else:  # 没有定义变量名的，设置一个默认变量名
                        param_name = "rand_name_%d" % self._rand_name_count
                        self._rand_name_count += 1
                        params = params[params.index(",") + 1:]
                    ret.append({"name": param_name, "type": param_type.strip()})
                else:
                    ret.append({"name": '', "type": params.strip(',').strip()})
                    break
            else:
                print("error cant analyze param :[%s]" % params)
                break
        return ret

    def _extract_path_and_file(self, path):
        self._header_dict["path"], self._header_dict["name"] = os.path.split(path)

    def _extract_include(self, includes):
        """
        Extract imports from includes
        """
        for include in includes:
            if '<' not in include:  # ignore system files
                self._header_dict.get("import").append(include[1:-1])

    def _extract_enum(self, enums):
        """
        Extract enums from enums
        """
        for enm in enums:
            if "name" in enm:
                enum_dict = {"name": enm["name"], "members": []}
            else:
                enum_dict = {"name": "LostName_%d" % self._rand_name_count, "members": []}
                self._rand_name_count += 1
            for value in enm["values"]:
                v_value = value["value"]
                if not isinstance(v_value, int):
                    tt = re.match(r"0x|0X|\(|-", v_value)
                    if tt:
                        errmsg = "unexpected '%s'" % tt[0]
                        v_value = v_value + " // " + errmsg
                        print("[HeaderParser]: %s[line %d] " % (enm["filename"], enm["line_number"]), errmsg)
                enum_dict["members"].append({"name": value["name"], "value": v_value})
            self._header_dict.get("enum").append(enum_dict)

    def _extract_union(self, stack):
        """
        Extract unions from classes
        """
        union_dict = {}
        if stack["declaration_method"] == "union":
            union_dict["name"] = stack["name"].split(" ")[-1]
            union_dict["type"] = "union"
            union_dict["members"] = []
            file_name = self._header_dict.get("path") + "/" + self._header_dict.get("name")
            for mb in stack["members"]:
                union_dict.get("members").append({"file_name": file_name,
                                                  "line_number": mb["line_number"],
                                                  "name": mb["name"], "type": mb["type"]})
            self._header_dict.get("union").append(union_dict)

    def _extract_struct(self, stack):
        """
        Extract structs from structs or classes that have no methods
        """
        struct_dict = {}
        if stack["declaration_method"] in ["struct", "class"]:
            if len(stack["methods"]["public"]) == 0:  # 不带函数
                if not self._has_function_pointer(stack["properties"]["public"]):  # 变量中没有函数指针
                    struct_dict["name"] = stack["name"]
                    struct_dict["type"] = "struct"
                    struct_dict["members"] = []
                    file_name = self._header_dict.get("path") + "/" + self._header_dict.get("name")
                    for mb in stack["properties"]["public"]:
                        if "enum_type" in mb:
                            struct_dict.get("members").append({"file_name": file_name,
                                                               "line_number": mb["line_number"],
                                                               "name": mb["name"],
                                                               "type": mb["enum_type"]["name"]})
                        elif mb["array"] == 1:
                            struct_dict.get("members").append({"file_name": file_name,
                                                               "line_number": mb["line_number"],
                                                               "name": mb["name"],
                                                               "type": mb["type"] + " *"})
                        else:
                            struct_dict.get("members").append({"file_name": file_name,
                                                               "line_number": mb["line_number"],
                                                               "name": mb["name"],
                                                               "type": mb["type"]})
                    self._header_dict.get("struct").append(struct_dict)

    def _extract_interface(self, stack):
        """
        Extract interfaces from structs or classes which have some methods
        """
        interface_dict = {}
        if stack["declaration_method"] in ["struct", "class"]:
            # 带函数，或变量中包含函数指针
            if len(stack["methods"]["public"]) > 0 or self._has_function_pointer(stack["properties"]["public"]):
                interface_dict["name"] = stack["name"]
                interface_dict["members"] = []
                for mb in stack["methods"]["public"]:
                    if mb["name"] in (stack["name"], "DECLARE_INTERFACE_DESCRIPTOR"):
                        continue
                    params = []
                    for param in mb["parameters"]:
                        para_name = param["name"]
                        if para_name == '':
                            para_name = "rand_name_%d" % self._rand_name_count
                            self._rand_name_count += 1
                        params.append({"name": para_name, "type": param["type"]})
                    interface_dict.get("members").append(
                        {"name": mb["name"],
                         "params": params,
                         "file_name":
                             self._header_dict.get("path") + "/" + self._header_dict.get("name"),
                         "line_number": mb["line_number"]})
                for mb in stack["properties"]["public"]:
                    if mb["function_pointer"] > 0:
                        interface_dict.get("members").append(
                            {"name": mb["name"],
                             "params": self._checkout_function_pointer_param(mb["type"]),
                             "file_name":
                                 self._header_dict.get("path") + "/" + self._header_dict.get("name"),
                             "line_number": mb["line_number"]})
                self._header_dict.get("interface").append(interface_dict)

    def _extract_typedef(self, typedefs):
        """
        Extract typedef from global typedefs
        e.g.
        "typedefs": {
            "HotPlugCallback": "typedef void ( * ) ( uint32_t devId , bool connected , void * data )",
            "AudioHandle": "void *"
        }
        """
        for td in typedefs:
            if "typedef" in typedefs[td]:
                self._header_dict.get("typedef").append({"name": td,
                                                         "type": "/* unsupported function pointer type: " + td + " */"})
            else:
                self._header_dict.get("typedef").append({"name": td, "type": typedefs[td]})
