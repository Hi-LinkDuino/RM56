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
import sys
import unittest

import CppHeaderParser

sys.path.insert(0, '..')
try:
    from _header_parser import HeaderParser
    from idl_generator import IDLGenerator
finally:
    pass


class IDLGeneratorTestCase(unittest.TestCase):
    def test_install_package(self):
        generator = IDLGenerator()
        generator._install_package("\\h\\audio\\test")
        self.assertEqual(generator._idl, "package h.audio.test;\n\n")

        generator._idl = ""
        generator._install_package(".\\h\\audio\\test")
        self.assertEqual(generator._idl, "package h.audio.test;\n\n")

        generator._idl = ""
        generator._install_package("C:\\h\\audio\\test")
        self.assertEqual(generator._idl, "package h.audio.test;\n\n")

        generator._idl = ""
        generator._install_package("./h/audio/test")
        self.assertEqual(generator._idl, "package h.audio.test;\n\n")

    def test_install_import_interface(self):
        generator = IDLGenerator()
        generator._parse_results = {
            'audio_render.h': {
                'name': 'audio_render.h',
                'path': '.\\include\\audio',
                'import': [], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioRender', 'members': []}],
                'callback': []
            },
            'audio_test.h': {
                'name': 'audio_test.h',
                'path': '.\\include\\audio',
                'import': ['audio_render.h'], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioRender', 'members': []}],
                'callback': []
            }
        }
        header = generator._parse_results['audio_test.h']
        generator._install_import(header)
        self.assertEqual(generator._idl, "import include.audio.AudioRender;\n\n")

    def test_install_import_interfaces(self):
        generator = IDLGenerator()
        generator._parse_results = {
            'audio_render.h': {
                'name': 'audio_render.h',
                'path': '.\\include\\audio',
                'import': [], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioRender', 'members': []}],
                'callback': []
            },
            'audio_adapter.h': {
                'name': 'audio_adapter.h',
                'path': '.\\include\\audio\\adapter',
                'import': [], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioAdapter', 'members': []}],
                'callback': []
            },
            'audio_test.h': {
                'name': 'audio_test.h',
                'path': '.\\include\\audio',
                'import': ['audio_render.h', 'audio_adapter.h'], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioRender', 'members': []}],
                'callback': []
            }
        }
        header = generator._parse_results['audio_test.h']
        generator._install_import(header)
        self.assertEqual(generator._idl, "import include.audio.AudioRender;\n"
                                         "import include.audio.adapter.AudioAdapter;\n\n")

    def test_install_import_multiple_interfaces(self):
        generator = IDLGenerator()
        generator._parse_results = {
            'audio_multiple_interfaces.h': {
                'name': 'audio_multiple_interfaces.h',
                'path': '.\\include',
                'import': [], 'enum': [], 'union': [], 'struct': [],
                'interface': [
                    {'name': 'AudioInterfaceOne', 'members': []},
                    {'name': 'AudioInterfaceTwo', 'members': []}
                ],
                'callback': []
            },
            'audio_test.h': {
                'name': 'audio_test.h',
                'path': '.\\include\\audio',
                'import': ['audio_multiple_interfaces.h'], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioRender', 'members': []}],
                'callback': []
            }
        }
        header = generator._parse_results['audio_test.h']
        generator._install_import(header)
        self.assertEqual(generator._idl, "import include.AudioInterfaceOne;\nimport include.AudioInterfaceTwo;\n\n")

    def test_install_import_types(self):
        generator = IDLGenerator()
        generator._parse_results = {
            'audio_types.h': {
                'name': 'audio_types.h',
                'path': '.\\include',
                'import': [],
                'enum': [{'name': 'AudioPortDirection', 'members': [{'name': 'PORT_OUT', 'value': 1}]}],
                'union': [], 'struct': [], 'interface': [], 'callback': []
            },
            'audio_test.h': {
                'name': 'audio_test.h',
                'path': '.\\include\\audio',
                'import': ['audio_types.h'], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioRender', 'members': []}],
                'callback': []
            }
        }
        header = generator._parse_results["audio_test.h"]
        generator._install_import(header)
        self.assertEqual(generator._idl, "import include.Types;\n\n")

    def test_install_import_merge_types(self):
        generator = IDLGenerator()
        generator._parse_results = {
            'audio_types.h': {
                'name': 'audio_types.h',
                'path': '.\\include',
                'import': ['adapter_types.h'],
                'enum': [{'name': 'AudioPortDirection', 'members': [{'name': 'PORT_OUT', 'value': 1}]}],
                'union': [], 'struct': [], 'interface': [], 'callback': []
            },
            'adapter_types.h': {
                'name': 'adapter_types.h',
                'path': '.\\include',
                'import': [],
                'enum': [{'name': 'AudioAdapter', 'members': [{'name': 'PORT_OUT', 'value': 1}]}],
                'union': [], 'struct': [], 'interface': [], 'callback': []
            },
            'audio_test.h': {
                'name': 'audio_test.h',
                'path': '.\\include\\audio',
                'import': ['audio_types.h', 'adapter_types.h'], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioRender', 'members': []}],
                'callback': []
            }
        }
        header = generator._parse_results['audio_test.h']
        generator._install_import(header)
        self.assertEqual(generator._idl, "import include.Types;\n\n")

    def test_install_import_types_and_interfaces(self):
        generator = IDLGenerator()
        generator._parse_results = {
            'audio_types.h': {
                'name': 'audio_types.h',
                'path': '.\\include',
                'import': ['adapter_types.h'],
                'enum': [{'name': 'AudioPortDirection', 'members': [{'name': 'PORT_OUT', 'value': 1}]}],
                'union': [], 'struct': [], 'interface': [], 'callback': []
            },
            'audio_adapter.h': {
                'name': 'audio_adapter.h',
                'path': '.\\include\\audio\\adapter',
                'import': [], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioAdapter', 'members': []}],
                'callback': []
            },
            'audio_test.h': {
                'name': 'audio_test.h',
                'path': '.\\include\\audio',
                'import': ['audio_types.h', 'audio_adapter.h'], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioRender', 'members': []}],
                'callback': []
            }
        }
        header = generator._parse_results['audio_test.h']
        generator._install_import(header)
        self.assertEqual(generator._idl, "import include.Types;\nimport include.audio.adapter.AudioAdapter;\n\n")

    def test_install_import_callback(self):
        generator = IDLGenerator()
        generator._parse_results = {
            'audio_test.h': {
                'name': 'audio_test.h',
                'path': '.\\include\\audio',
                'import': [], 'enum': [], 'union': [], 'struct': [],
                'interface': [{'name': 'AudioRender', 'members': []}],
                'callback': [
                    {'name': 'HotPlugCallback', 'members': []},
                    {'name': 'VBlankCallback', 'members': []}
                ]
            }
        }
        header = generator._parse_results['audio_test.h']
        generator._install_import(header)
        self.assertEqual(generator._idl, "import include.audio.HotPlugCallback;\n"
                                         "import include.audio.VBlankCallback;\n\n")

    def test_install_enum(self):
        generator = IDLGenerator()
        enum = [{
            'name': 'AudioPortDirection',
            'members': [
                {'name': 'PORT_OUT', 'value': 1},
                {'name': 'PORT_IN', 'value': '- 2 // unexpected \'-\''},
            ]}
        ]
        generator._install_enum(enum)
        self.assertEqual(generator._idl, "enum AudioPortDirection {\n"
                                         "    PORT_OUT = 1,\n"
                                         "    PORT_IN = - 2 // unexpected '-',\n"
                                         "};\n")

    def test_install_stack_union(self):
        generator = IDLGenerator()
        union = [{'name': 'SceneDesc',
                  'type': 'union',
                  'members': [
                      {'name': 'id', 'type': 'uint32_t'},
                      {'name': 'desc', 'type': 'const char *'}
                  ]}]
        generator._install_stack(union)
        self.assertEqual(generator._idl, "union SceneDesc {\n"
                                         "    unsigned int id;\n"
                                         "    byte[] desc;\n"
                                         "};\n")

    def test_install_stack_struct(self):
        generator = IDLGenerator()
        struct = [{'name': 'SceneDesc',
                   'type': 'struct',
                   'members': [
                       {'name': 'id', 'type': 'uint32_t'},
                       {'name': 'desc', 'type': 'const char *'}
                   ]}]
        generator._install_stack(struct)
        self.assertEqual(generator._idl, "struct SceneDesc {\n"
                                         "    unsigned int id;\n"
                                         "    byte[] desc;\n"
                                         "};\n")

    def test_install_interface(self):
        header_file = """
            typedef struct {
                int32_t (*RunExtraCommand)(struct InputController *self, uint32_t devIndex, uint32_t **cmd);
                int32_t (*RunExtra)(struct InputController *self, struct InputControllerDesc desc);
            } InputController;
        """
        parser = HeaderParser()
        hjson = json.loads(CppHeaderParser.CppHeader(header_file, "string").toJSON())
        parser._extract_interface(hjson["classes"]["InputController"])
        generator = IDLGenerator()
        generator._key_list["InputController"] = "struct"
        generator._key_list["InputControllerDesc"] = "struct"
        generator._install_interface(parser._header_dict.get("interface")[0])
        self.assertEqual(generator._idl, "interface InputController {\n"
                                         "    RunExtraCommand([in] unsigned int devIndex,[out] unsigned int cmd);\n"
                                         "    RunExtra([in] struct InputControllerDesc desc);\n"
                                         "}\n")

    def test_install_interface_with_unknown_type(self):
        header_file = """
            typedef struct {
                int32_t (*RunExtra)(struct InputControllerDesc desc);
            } InputController;
        """
        parser = HeaderParser()
        hjson = json.loads(CppHeaderParser.CppHeader(header_file, "string").toJSON())
        parser._extract_interface(hjson["classes"]["InputController"])
        generator = IDLGenerator()
        generator._install_interface(parser._header_dict.get("interface")[0])
        self.assertEqual(generator._idl, "interface InputController {\n"
                                         "    RunExtra([in] /* unknown type: [InputControllerDesc] */ desc);\n"
                                         "}\n")

    def test_install_interface_callback(self):
        header_file = """
            struct IFooCallback {
                int32_t (*PushData)(struct IFooCallback *self, const char* message);
            };
        """
        parser = HeaderParser()
        hjson = json.loads(CppHeaderParser.CppHeader(header_file, "string").toJSON())
        parser._extract_interface(hjson["classes"]["IFooCallback"])
        generator = IDLGenerator()
        generator._install_interface(parser._header_dict.get("interface")[0])
        self.assertEqual(generator._idl, "[callback] interface IFooCallback {\n"
                                         "    PushData([in] byte[] message);\n"
                                         "}\n")

    def test_convert_from_container_type(self):
        generator = IDLGenerator()
        self.assertEqual(generator._convert_container_type("const map<int, int> test"),
                         "Map<int, int>")
        self.assertEqual(generator._convert_container_type("const std::map< int , int > test"),
                         "Map<int, int>")
        self.assertEqual(generator._convert_container_type("const std::map<int, std::string> test"),
                         "Map<int, String>")
        self.assertEqual(generator._convert_container_type("const std::map<struct KeyType, struct ValueType> test"),
                         "Map<KeyType, ValueType>")
        self.assertEqual(generator._convert_container_type("const std::map<unsigned int, unsigned long> test"),
                         "Map<unsigned int, unsigned long>")
        self.assertEqual(generator._convert_container_type("const std::vector<int> test"),
                         "List<int>")
        self.assertEqual(generator._convert_container_type("const std::vector<struct ValueType> test"),
                         "List<ValueType>")
        self.assertEqual(generator._convert_container_type("const std::vector<unsigned long> test"),
                         "List<unsigned long>")


if __name__ == "__main__":
    unittest.main()
