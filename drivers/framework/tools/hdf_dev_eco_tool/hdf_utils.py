#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import json
import os
import hashlib


from hdf_tool_exception import HdfToolException
from hdf_tool_settings import HdfToolSettings
from command_line.hdf_command_error_code import CommandErrorCode


class WordsConverter(object):
    def __init__(self, words_str):
        self.words_str = words_str
        if len(self.words_str) == 0:
            raise HdfToolException('empty words')
        self.words_list = self.split()
        self.upper = '_'.join([w.upper() for w in self.words_list])
        self.lower = '_'.join([w.lower() for w in self.words_list])
        self.upper_camel = ''.join([w.capitalize() for w in self.words_list])
        if len(self.upper_camel) == 1:
            self.lower_camel = self.upper_camel[0].lower()
        else:
            self.lower_camel = '%s%s' % (self.upper_camel[0].lower(),
                                         self.upper_camel[1:])

    def _capital_pos_generator(self):
        count = len(self.words_str)
        for i in range(count):
            if self.words_str[i].isupper():
                yield i
        yield count

    def split(self):
        words_list = []
        capital_split_words = []
        start = 0
        for i in self._capital_pos_generator():
            end = i
            if end != start:
                capital_split_words.append(self.words_str[start:end])
                start = end
        for word in capital_split_words:
            words_list.extend([w for w in word.split('_') if w])
        return words_list

    def upper_case(self):
        return self.upper

    def lower_case(self):
        return self.lower

    def upper_camel_case(self):
        return self.upper_camel

    def lower_camel_case(self):
        return self.lower_camel


class SectionRange(object):
    def __init__(self, start_pos, end_pos):
        self.start_pos = start_pos
        self.end_pos = end_pos


class SectionContent(object):
    def __init__(self, begin_flag, content, end_flag):
        self.begin_flag = begin_flag
        self.content = content
        self.end_flag = end_flag

    def to_str(self):
        return '%s%s%s' % (self.begin_flag, self.content, self.end_flag)


def find_section(file_content, section_content):
    if not file_content or not section_content:
        return False
    start_pos = file_content.find(section_content.begin_flag)
    if start_pos == -1:
        return False
    end_pos = file_content.find(section_content.end_flag)
    if end_pos == -1:
        return False
    return SectionRange(start_pos, end_pos + len(section_content.end_flag) - 1)


def _write_one_section(file_obj, section_content):
    if not file_obj or not section_content:
        return
    file_obj.write(section_content.begin_flag)
    file_obj.write(section_content.content)
    file_obj.write(section_content.end_flag)


def add_before_and_save(file_content, file_path, pos_range, new_section):
    if not file_content or not file_path or not pos_range or not new_section:
        return
    with open(file_path, 'w', newline='\n') as file_write:
        file_write.write(file_content[:pos_range.start_pos])
        _write_one_section(file_write, new_section)
        file_write.write(file_content[pos_range.start_pos:])


def add_after_and_save(file_content, file_path, pos_range, new_section):
    if not file_content or not file_path or not pos_range or not new_section:
        return
    with open(file_path, 'w', newline='\n') as file_write:
        file_write.write(file_content[:pos_range.end_pos + 1])
        _write_one_section(file_write, new_section)
        if len(file_content) > pos_range.end_pos + 1:
            file_write.write(file_content[pos_range.end_pos + 1:])


def append_and_save(file_content, file_path, new_section):
    if not file_content or not file_path or not new_section:
        return
    with open(file_path, 'w', newline='\n') as file_write:
        file_write.write(file_content)
        _write_one_section(file_write, new_section)


def delete_and_save(file_content, file_path, delete_range):
    if not file_content or not file_path or not delete_range:
        return
    length = len(file_content)
    with open(file_path, 'w', newline='\n') as file_write:
        file_write.write(file_content[:delete_range.start_pos])
        if delete_range.end_pos + 1 < length:
            file_write.write(file_content[delete_range.end_pos + 1:])


def replace_and_save(file_content, file_path, old_range, new_section):
    if not file_content or not file_path or not old_range or not new_section:
        return
    with open(file_path, 'w', newline='\n') as file_write:
        file_write.write(file_content[:old_range.start_pos])
        _write_one_section(file_write, new_section)
        file_write.write(file_content[old_range.end_pos + 1:])


def get_id(part2):
    full_name = part2
    return hashlib.sha256(full_name.encode('utf8')).hexdigest()[:32]


def create_dirs(dir_path):
    if dir_path and not os.path.exists(dir_path):
        try:
            os.makedirs(dir_path)
        except Exception:
            raise HdfToolException('create dirs fail: %s' % dir_path)


def read_file(file_path):
    with open(file_path, encoding="utf-8") as file_read:
        content = file_read.read()
    return content


def read_file_lines(file_path, code_type="utf-8"):
    with open(file_path, encoding=code_type) as file_read:
        lines = file_read.readlines()
    return lines


def write_file(file_path, content):
    with open(file_path, 'w+', newline='\n') as file_write:
        file_write.write(content)


def write_file_lines(file_path, content, code_type="utf-8"):
    with open(file_path, 'w', encoding=code_type) as file_write:
        file_write.writelines(content)


def get_framework_lite_dir(root):
    return os.path.join(root, 'drivers', 'adapter', 'khdf', 'liteos')


def get_vendor_root_dir(root):
    return os.path.join(root, 'vendor')


def get_vendor_dir(root, vendor):
    return os.path.join(get_vendor_root_dir(root), vendor)


def get_vendor_hdf_dir_framework(root):
    relative_path = HdfToolSettings().get_drivers_path_framework()
    return os.path.join(root, relative_path)


def get_vendor_hdf_dir_adapter(root, kernel='liteos'):
    relative_path = HdfToolSettings().get_drivers_path_adapter()
    return os.path.join(root, relative_path, kernel)


def get_vendor_lite_mk_path(root):
    return os.path.join(get_vendor_hdf_dir_adapter(root), 'hdf_lite.mk')


def get_vendor_makefile_path(root, kernel):
    return os.path.join(get_vendor_hdf_dir_adapter(root, kernel), 'Makefile')


def get_dot_configs_path(root, vendor, board):
    path = os.path.join(root, "vendor", vendor, board, 'kernel_configs')
    return [os.path.join(path, i) for i in os.listdir(path)]


def get_module_dir(root, module=""):
    return os.path.join(get_vendor_hdf_dir_framework(root), 'model', module)


def get_drv_root_dir(root, module):
    return os.path.join(get_module_dir(root, module), 'driver')


def get_drv_dir(root, module, driver):
    return os.path.join(get_drv_root_dir(root, module), driver)


def get_drv_src_dir(root, module):
    return get_drv_root_dir(root, module)


def get_drv_include_dir(root, module, driver):
    return os.path.join(get_drv_dir(root, module, driver), 'include')


def get_vendor_kconfig_path(root,  kernel):
    hdf_dir = get_vendor_hdf_dir_adapter(root, kernel)
    return os.path.join(hdf_dir, 'Kconfig')


def get_module_kconfig_path(root, module):
    return os.path.join(get_drv_root_dir(root, module), 'Kconfig')


def get_module_mk_path(root, module):
    return os.path.join(get_drv_root_dir(root, module), 'Makefile')


def get_liteos_a_dot_config_path(root):
    return os.path.join(root, 'kernel', 'liteos_a', '.config')


def get_resources_dir():
    cur_dir = os.path.realpath(os.path.dirname(__file__))
    return os.path.join(cur_dir, 'resources')


def get_templates_dir():
    return os.path.join(get_resources_dir(), 'templates')


def get_templates_lite_dir():
    return os.path.join(get_templates_dir(), 'lite')


def get_template(template_name, type_='lite'):
    templates_dir = os.path.join(get_templates_dir(), type_)
    template = os.path.join(templates_dir, template_name)
    with open(template) as file_read:
        template_str = file_read.read()
    return template_str


def get_hdf_lite_settings_mk_path(root_dir):
    return os.path.join(get_framework_lite_dir(root_dir),
                        'hdf_lite_settings.mk')


def get_hdf_lite_mk_path(root_dir):
    return os.path.join(get_framework_lite_dir(root_dir),
                        'hdf_lite.mk')


def get_hdf_lite_kconfig_path(root_dir):
    return os.path.join(get_framework_lite_dir(root_dir),
                        'Kconfig')


def is_commented_line(line, comment_start):
    if line.strip().startswith(comment_start):
        return True
    else:
        return False


def get_vendor_gn_path(root):
    return os.path.join(get_vendor_hdf_dir_adapter(root), 'model', 'BUILD.gn')


def get_template_file_path(root):
    template_relative_path = HdfToolSettings().get_template_path()
    relative_path2 = HdfToolSettings().get_drivers_path_framework()
    return os.path.join(root, relative_path2, template_relative_path)


def get_hcs_file_path(root, vendor, board):
    l2_board_list = list(filter(
        lambda x: x[0].isupper(),
        HdfToolSettings().get_board_list()))
    if board in l2_board_list:
        return os.path.join(root, "vendor", vendor, board,
                            "hdf_config", "khdf", "device_info",
                            "device_info.hcs")
    else:
        return os.path.join(root, "vendor", vendor, board,
                            "hdf_config", "device_info", "device_info.hcs")


def template_filename_filtrate(dir_path, kernal):
    filename_list = []
    for filename in os.listdir(dir_path):
        if filename.split("_")[0] == kernal.capitalize():
            filename_list.append(filename)
    return filename_list


def get_create_model_info(root, create_data):
    data = json.loads(create_data)
    out_model_list = []
    if not data:
        return []
    file_key_list = list(list(data.items())[0][-1].keys())[:-1]
    for k, _ in data.items():
        model_file_path = {}
        for key in file_key_list:
            if key.split("_")[-1] == "path":
                path_dict = data[k][key]
                if isinstance(path_dict, dict):
                    for k_filename, file_path in path_dict.items():
                        if not os.path.exists(os.path.join(root, file_path)):
                            model_file_path[k_filename] = " "
                        else:
                            model_file_path[k_filename] = path_dict[k_filename]
                else:
                    hcs_file_path = os.path.join(root, path_dict)
                    if not os.path.exists(hcs_file_path):
                        model_file_path[key] = " "
                    else:
                        model_file_path[key] = path_dict
        out_model_list.append({k: model_file_path})
    return out_model_list


def get_config_config_path(root, kernel):
    return os.path.join(root, "kernel", kernel, "config", "linux-5.10")


def judge_file_path_exists(temp_path):
    if not os.path.exists(temp_path):
        raise HdfToolException('path  "%s" not exist' %
                               temp_path, CommandErrorCode.TARGET_NOT_EXIST)