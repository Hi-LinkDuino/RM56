#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
# 
# HDF is dual licensed: you can use it either under the terms of
# the GPL, or the BSD license, at your option.
# See the LICENSE file in the root of this repository for complete details.


import os
import json

from hdf_tool_exception import HdfToolException
from command_line.hdf_command_error_code import CommandErrorCode


def singleton(clazz):
    _instances = {}

    def create_instance():
        if clazz not in _instances:
            _instances[clazz] = clazz()
        return _instances.get(clazz)
    return create_instance


def get_hdf_tool_settings_path():
    cur_dir = os.path.realpath(os.path.dirname(__file__))
    return os.path.join(cur_dir, 'resources', 'settings.json')


@singleton
class HdfToolSettings(object):
    def __init__(self):
        self.file_path = get_hdf_tool_settings_path()
        self.settings = {}
        if not os.path.exists(self.file_path):
            return
        with open(self.file_path) as file_write:
            try:
                self.settings = json.load(file_write)
            except ValueError as exc:
                raise HdfToolException('file: %s format wrong, %s' %
                                       (self.file_path, str(exc)),
                                       CommandErrorCode.FILE_FORMAT_WRONG)
        self.supported_boards_key = 'supported_boards'
        self.drivers_path_key = 'drivers_path_relative_to_vendor'
        self.drivers_adapter_path_key = 'drivers_path_relative_adapter'
        self.user_adapter_path_key = 'user_model_path_relative_adapter'
        self.dot_configs_key = 'dot_configs'
        self.board_path_key = 'board_parent_path'
        self.dot_config_path_key = 'dot_config_path'
        self.template_path_key = 'template_file_path'

    def get_supported_boards(self):
        key = self.supported_boards_key
        if key in self.settings:
            return ','.join(self.settings.get(key).keys())
        return ''

    def get_board_parent_path(self, board_name):
        key = self.supported_boards_key
        board_entry = {}
        if key in self.settings:
            if board_name in self.settings.get(key):
                board_entry = self.settings.get(key).get(board_name)
        key = self.board_path_key
        return board_entry.get(key, '')

    def get_drivers_path_framework(self):
        key = self.drivers_path_key
        return self.settings.get(key, 'hdf')

    def get_drivers_path_adapter(self):
        key = self.drivers_adapter_path_key
        return self.settings.get(key, 'hdf')

    def get_template_path(self):
        key = self.template_path_key
        return self.settings.get(key, 'hdf')

    def get_dot_configs(self, board_name):
        key = self.supported_boards_key
        boards = self.settings.get(key, None)
        if not boards:
            return []
        board = boards.get(board_name, None)
        if not board:
            return []
        dot_config_path = board.get(self.dot_config_path_key, '')
        if not dot_config_path:
            return []
        configs = board.get(self.dot_configs_key, [])
        return [os.path.join(dot_config_path, config) for config in configs]

    def get_board_parent_file(self, board_name):
        key = self.supported_boards_key
        if key in self.settings:
            if board_name in self.settings.get(key):
                return self.settings.get(key).get(board_name).get("patch_and_config")
        return ''

    def get_board_list(self):
        key = self.supported_boards_key
        return list(self.settings.get(key).keys())

    def get_user_adapter_path(self):
        key = self.user_adapter_path_key
        return self.settings.get(key, 'hdf')