#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#

import logging
import sys
import time
from logging.handlers import RotatingFileHandler

from _core.constants import LogType
from _core.plugin import Plugin
from _core.plugin import get_plugin
from _core.exception import ParamError


__all__ = ["Log", "platform_logger", "device_logger", "shutdown",
           "add_task_file_handler", "remove_task_file_handler",
           "change_logger_level", "add_encrypt_file_handler",
           "remove_encrypt_file_handler"]

_HANDLERS = []
_LOGGERS = []
MAX_LOG_LENGTH = 10 * 1024 * 1024
MAX_ENCRYPT_LOG_LENGTH = 5 * 1024 * 1024


class Log:
    task_file_handler = None

    def __init__(self):
        self.level = logging.INFO
        self.handlers = []
        self.loggers = {}
        self.task_file_handler = None
        self.encrypt_file_handler = None

    def __initial__(self, log_handler_flag, log_file=None, level=None,
                    log_format=None):
        if _LOGGERS:
            return

        self.handlers = []
        if log_file and "console" in log_handler_flag:
            file_handler = RotatingFileHandler(
                log_file, mode="a", maxBytes=MAX_LOG_LENGTH, backupCount=5,
                encoding="UTF-8")
            file_handler.setFormatter(logging.Formatter(log_format))
            self.handlers.append(file_handler)
        if "console" in log_handler_flag:
            stream_handler = logging.StreamHandler(sys.stdout)
            stream_handler.setFormatter(logging.Formatter(log_format))
            self.handlers.append(stream_handler)

        if level:
            self.level = level
        self.loggers = {}
        self.task_file_handler = None
        _HANDLERS.extend(self.handlers)

    def set_level(self, level):
        self.level = level

    def __logger__(self, name=None):
        if not name:
            return _init_global_logger(name)
        elif name in self.loggers:
            return self.loggers.get(name)
        else:
            log = self.loggers.setdefault(name, FrameworkLog(name))
            _LOGGERS.append(log)
            log.platform_log.setLevel(self.level)
            for handler in self.handlers:
                log.platform_log.addHandler(handler)
            if self.task_file_handler:
                log.add_task_log(self.task_file_handler)
            return log

    def add_task_file_handler(self, log_file):
        from xdevice import Variables
        file_handler = RotatingFileHandler(
                log_file, mode="a", maxBytes=MAX_LOG_LENGTH, backupCount=5,
                encoding="UTF-8")
        file_handler.setFormatter(logging.Formatter(
            Variables.report_vars.log_format))
        self.task_file_handler = file_handler
        for _, log in self.loggers.items():
            log.add_task_log(self.task_file_handler)

    def remove_task_file_handler(self):
        if self.task_file_handler is None:
            return
        for _, log in self.loggers.items():
            log.remove_task_log(self.task_file_handler)
        self.task_file_handler.close()
        self.task_file_handler = None

    def add_encrypt_file_handler(self, log_file):
        from xdevice import Variables

        file_handler = \
            EncryptFileHandler(log_file, mode="ab",
                               max_bytes=MAX_ENCRYPT_LOG_LENGTH,
                               backup_count=5, encoding="utf-8")
        file_handler.setFormatter(logging.Formatter(
            Variables.report_vars.log_format))
        self.encrypt_file_handler = file_handler
        for _, log in self.loggers.items():
            log.add_encrypt_log(self.encrypt_file_handler)

    def remove_encrypt_file_handler(self):
        if self.encrypt_file_handler is None:
            return
        for _, log in self.loggers.items():
            log.remove_encrypt_log(self.encrypt_file_handler)
        self.encrypt_file_handler.close()
        self.encrypt_file_handler = None


class FrameworkLog:

    def __init__(self, name):
        self.name = name
        self.platform_log = logging.Logger(name)
        self.task_log = None
        self.encrypt_log = None

    def set_level(self, level):
        # apply to dynamic change logger level, and
        # only change the level of platform log
        cache = getattr(self.platform_log, "_cache", None)
        if cache and isinstance(cache, dict):
            cache.clear()
        self.platform_log.setLevel(level)

    def add_task_log(self, handler):
        if self.task_log:
            return
        self.task_log = logging.Logger(self.name)
        log_level = getattr(sys, "log_level", logging.INFO) if hasattr(
            sys, "log_level") else logging.DEBUG
        self.task_log.setLevel(log_level)
        self.task_log.addHandler(handler)

    def remove_task_log(self, handler):
        if not self.task_log:
            return
        self.task_log.removeHandler(handler)
        self.task_log = None

    def add_encrypt_log(self, handler):
        if self.encrypt_log:
            return
        self.encrypt_log = logging.Logger(self.name)
        log_level = getattr(sys, "log_level", logging.INFO) if hasattr(
            sys, "log_level") else logging.DEBUG
        self.encrypt_log.setLevel(log_level)
        self.encrypt_log.addHandler(handler)

    def remove_encrypt_log(self, handler):
        if not self.encrypt_log:
            return
        self.encrypt_log.removeHandler(handler)
        self.encrypt_log = None

    def info(self, msg, *args, **kwargs):
        additional_output = self._get_additional_output(**kwargs)
        updated_msg = self._update_msg(additional_output, msg)
        self.platform_log.info(updated_msg, *args)
        if self.task_log:
            self.task_log.info(updated_msg, *args)
        if self.encrypt_log:
            self.encrypt_log.info(updated_msg, *args)

    def debug(self, msg, *args, **kwargs):
        additional_output = self._get_additional_output(**kwargs)
        updated_msg = self._update_msg(additional_output, msg)
        from _core.report.encrypt import check_pub_key_exist
        if not check_pub_key_exist():
            self.platform_log.debug(updated_msg, *args)
            if self.task_log:
                self.task_log.debug(updated_msg, *args)
        else:
            if self.encrypt_log:
                self.encrypt_log.debug(updated_msg, *args)

    def error(self, msg, *args, **kwargs):
        error_no = kwargs.get("error_no", "00000")
        additional_output = self._get_additional_output(error_no, **kwargs)
        updated_msg = self._update_msg(additional_output, msg)

        self.platform_log.error(updated_msg, *args)
        if self.task_log:
            self.task_log.error(updated_msg, *args)
        if self.encrypt_log:
            self.encrypt_log.error(updated_msg, *args)

    def warning(self, msg, *args, **kwargs):
        additional_output = self._get_additional_output(**kwargs)
        updated_msg = self._update_msg(additional_output, msg)

        self.platform_log.warning(updated_msg, *args)
        if self.task_log:
            self.task_log.warning(updated_msg, *args)
        if self.encrypt_log:
            self.encrypt_log.warning(updated_msg, *args)

    def exception(self, msg, *args, **kwargs):
        error_no = kwargs.get("error_no", "00000")
        exc_info = kwargs.get("exc_info", True)
        if exc_info is not True and exc_info is not False:
            exc_info = True
        additional_output = self._get_additional_output(error_no, **kwargs)
        updated_msg = self._update_msg(additional_output, msg)

        self.platform_log.exception(updated_msg, exc_info=exc_info, *args)
        if self.task_log:
            self.task_log.exception(updated_msg, exc_info=exc_info, *args)
        if self.encrypt_log:
            self.encrypt_log.exception(updated_msg, exc_info=exc_info, *args)

    @classmethod
    def _update_msg(cls, additional_output, msg):
        msg = "[%s]" % msg if msg else msg
        if msg and additional_output:
            msg = "%s [%s]" % (msg, additional_output)
        return msg

    def _get_additional_output(self, error_number=None, **kwargs):
        dict_str = self._get_dict_str(**kwargs)
        if error_number:
            additional_output = "ErrorNo=%s" % error_number
        else:
            return dict_str

        if dict_str:
            additional_output = "%s, %s" % (additional_output, dict_str)
        return additional_output

    @classmethod
    def _get_dict_str(cls, **kwargs):
        dict_str = ""
        for key, value in kwargs.items():
            if key in ["error_no", "exc_info"]:
                continue
            dict_str = "%s%s=%s, " % (dict_str, key, value)
        if dict_str:
            dict_str = dict_str[:-2]
        return dict_str


def platform_logger(name=None):
    plugins = get_plugin(Plugin.LOG, LogType.tool)
    for log_plugin in plugins:
        if log_plugin.get_plugin_config().enabled:
            return log_plugin.__logger__(name)
    return _init_global_logger(name)


def device_logger(name=None):
    plugins = get_plugin(Plugin.LOG, LogType.device)
    for log_plugin in plugins:
        if log_plugin.get_plugin_config().enabled:
            return log_plugin.__logger__(name)
    return _init_global_logger(name)


def shutdown():
    # logging will be shutdown automatically, when the program exits.
    # This function is used by testing.
    for log in _LOGGERS:
        for handler in log.handlers:
            log.removeHandler(handler)
    for handler in _HANDLERS:
        handler.close()
    _HANDLERS.clear()
    _LOGGERS.clear()


def add_task_file_handler(log_file=None):
    if log_file is None:
        return
    plugins = get_plugin(Plugin.LOG, LogType.tool)
    for log_plugin in plugins:
        if log_plugin.get_plugin_config().enabled:
            log_plugin.add_task_file_handler(log_file)


def remove_task_file_handler():
    plugins = get_plugin(Plugin.LOG, LogType.tool)
    for log_plugin in plugins:
        if log_plugin.get_plugin_config().enabled:
            log_plugin.remove_task_file_handler()


def add_encrypt_file_handler(log_file=None):
    if log_file is None:
        return
    plugins = get_plugin(Plugin.LOG, LogType.tool)
    for log_plugin in plugins:
        if log_plugin.get_plugin_config().enabled:
            log_plugin.add_encrypt_file_handler(log_file)


def remove_encrypt_file_handler():
    plugins = get_plugin(Plugin.LOG, LogType.tool)
    for log_plugin in plugins:
        if log_plugin.get_plugin_config().enabled:
            log_plugin.remove_encrypt_file_handler()


def _init_global_logger(name=None):
    handler = logging.StreamHandler(sys.stdout)
    log_format = "[%(asctime)s] [%(name)s] [%(levelname)s] [%(message)s]"
    handler.setFormatter(logging.Formatter(log_format))
    log = FrameworkLog(name)
    log.platform_log.setLevel(logging.INFO)
    log.platform_log.addHandler(handler)
    return log


def change_logger_level(leve_dict):
    level_map = {"debug": logging.DEBUG, "info": logging.INFO}
    if "console" in leve_dict.keys():
        level = leve_dict["console"]
        if not level:
            return
        if str(level).lower() in level_map.keys():
            logger_level = level_map.get(str(level).lower(), logging.INFO)

            # change level of loggers which will to be instantiated.
            # Actually, it changes the level attribute in ToolLog,
            # which will be used when instantiating the FrameLog object.
            plugins = get_plugin(Plugin.LOG, LogType.tool)
            for log_plugin in plugins:
                log_plugin.set_level(logger_level)
            # change level of loggers which have instantiated
            for logger in _LOGGERS:
                if getattr(logger, "setLevel", None):
                    logger.setLevel(logger_level)
                elif getattr(logger, "set_level", None):
                    logger.set_level(logger_level)

    if "file" in leve_dict.keys():
        level = leve_dict["file"]
        if not level:
            return
        if str(level).lower() in level_map.keys():
            logger_level = level_map.get(str(level).lower(), logging.INFO)
            setattr(sys, "log_level", logger_level)


class EncryptFileHandler(RotatingFileHandler):

    def __init__(self, filename, mode='ab', max_bytes=0, backup_count=0,
                 encoding=None, delay=False):
        RotatingFileHandler.__init__(self, filename, mode, max_bytes,
                                     backup_count, encoding, delay)
        self.mode = mode
        self.encrypt_error = None

    def _open(self):
        if not self.mode == "ab":
            self.mode = "ab"

        # baseFilename is the attribute in FileHandler
        base_file_name = getattr(self, "baseFilename", None)
        with open(base_file_name, self.mode) as encrypt_file:
            return encrypt_file

    def emit(self, record):
        try:
            if not self._encrypt_valid():
                return

            # shouldRoller and doRoller is the method in RotatingFileHandler
            should_rollover = getattr(self, "shouldRollover", None)
            if callable(should_rollover) and should_rollover(record):
                self.doRollover()

            # stream is the attribute in StreamHandler
            if not getattr(self, "stream", None):
                setattr(self, "stream", self._open())
            msg = self.format(record)
            stream = getattr(self, "stream", self._open())
            stream.write(msg)
            self.flush()
        except RecursionError:
            raise

    def _encrypt_valid(self):
        from _core.report.encrypt import check_pub_key_exist
        if check_pub_key_exist() and not self.encrypt_error:
            return True

    def format(self, record):
        """
        Customize the implementation method. If the log format of the
        framework changes, update the return value format of the method
        in a timely manner.
        :param record: logging.LogRecord
        :return: bytes
        """
        from _core.report.encrypt import do_rsa_encrypt
        create_time = "{},{}".format(
            time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(record.created)),
            "{:0>3d}".format(int("%d" % record.msecs)))
        name = record.name
        level_name = record.levelname
        msg = record.msg
        if msg and "%s" in msg:
            msg = msg % record.args
        info = "[%s] [%s] [%s] %s%s" \
               % (create_time, name, level_name, msg, "\n")

        try:
            return do_rsa_encrypt(info)
        except ParamError as error:
            error_no_str = \
                "ErrorNo={}".format(getattr(error, "error_no", "00113"))
            info = "[%s] [%s] [%s] [%s] [%s]\n" % (
                create_time, name, "ERROR", error, error_no_str)
            self.encrypt_error = bytes(info, "utf-8")
            return self.encrypt_error
