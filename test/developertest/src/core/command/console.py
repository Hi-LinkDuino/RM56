#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2020 Huawei Device Co., Ltd.
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

import argparse
import sys
import signal
import platform

from core.constants import ToolCommandType
from xdevice import platform_logger
from xdevice import EnvironmentManager
from core.command.run import Run
from core.command.gen import Gen
from core.command.display import display_help_info
from core.command.display import display_show_info
from core.command.display import show_wizard_mode
from core.config.config_manager import UserConfigManager
from core.utils import is_lite_product

try:
    if platform.system() != 'Windows':
        import readline
except ModuleNotFoundError:
    print("ModuleNotFoundError: readline module is not exist.")
except ImportError:
    print("ImportError: libreadline.so is not exist.")

__all__ = ["Console"]
LOG = platform_logger("Console")


##############################################################################
##############################################################################


class Console(object):
    """
    Class representing an console for executing test.
    Main xDevice console providing user with the interface to interact
    """
    __instance = None
    wizard_dic = {}

    def __new__(cls, *args, **kwargs):
        if cls.__instance is None:
            cls.__instance = super(Console, cls).__new__(cls, *args, **kwargs)
        return cls.__instance

    def __init__(self):
        pass

    def handler_ctrl_c(self, signalnum, frame):
        pass

    def handler_ctrl_z(self, signalnum, frame):
        pass

    def console(self, args):
        """
        Main xDevice console providing user with the interface to interact
        """
        EnvironmentManager()
        if args is None or len(args) < 2:
            self.wizard_dic = show_wizard_mode()
            print(self.wizard_dic)
            if self._build_version(self.wizard_dic["productform"]):
                self._console()
            else:
                LOG.error("Build version failed, exit test framework.")
        else:
            self.command_parser(" ".join(args[1:]))

    def _console(self):
        if platform.system() != 'Windows':
            signal.signal(signal.SIGTSTP, self.handler_ctrl_z)  # ctrl+x linux
        signal.signal(signal.SIGINT, self.handler_ctrl_c)  # ctrl+c

        while True:
            try:
                usr_input = input(">>> ")
                if usr_input == "":
                    continue
                self.command_parser(usr_input)
            except SystemExit:
                LOG.info("Program exit normally!")
                return
            except (IOError, EOFError, KeyboardInterrupt) as error:
                LOG.exception("Input Error: %s" % error)

    @classmethod
    def argument_parser(cls, para_list):
        """
        argument parser
        """
        options = None
        unparsed = []
        valid_param = True
        parser = None

        try:
            parser = argparse.ArgumentParser(description="Specify test para.")
            parser.add_argument("action", type=str.lower,
                                help="Specify action")

            # Developer test general test parameters
            parser.add_argument("-p", "--productform",
                                action="store",
                                type=str.lower,
                                dest="productform",
                                default="phone",
                                help="Specified product form"
                                )
            parser.add_argument("-t", "--testtype",
                                nargs='*',
                                type=str.upper,
                                dest="testtype",
                                default=["UT"],
                                help="Specify test type(UT,MST,ST,PERF,ALL)"
                                )
            parser.add_argument("-ss", "--subsystem",
                                nargs='*',
                                dest="subsystem",
                                default=[],
                                help="Specify test subsystem"
                                )
            parser.add_argument("-tp", "--testpart",
                                nargs='*',
                                dest="testpart",
                                default=[],
                                help="Specify test testpart"
                                )
            parser.add_argument("-tm", "--testmodule",
                                action="store",
                                type=str,
                                dest="testmodule",
                                default="",
                                help="Specified test module"
                                )
            parser.add_argument("-ts", "--testsuit",
                                action="store",
                                type=str,
                                dest="testsuit",
                                default="",
                                help="Specify test suit"
                                )
            parser.add_argument("-tc", "--testcase",
                                action="store",
                                type=str,
                                dest="testcase",
                                default="",
                                help="Specify test case"
                                )
            parser.add_argument("-tl", "--testlevel",
                                action="store",
                                type=str,
                                dest="testlevel",
                                default="",
                                help="Specify test level"
                                )

            # Developer test extended test parameters
            parser.add_argument("-cov", "--coverage",
                                action="store_true",
                                dest="coverage",
                                default=False,
                                help="Specify coverage"
                                )
            parser.add_argument("-tf", "--testfile",
                                action="store",
                                type=str,
                                dest="testfile",
                                default="",
                                help="Specify test suites list file"
                                )
            parser.add_argument("-res", "--resource",
                                action="store",
                                type=str,
                                dest="resource",
                                default="",
                                help="Specify test resource"
                                )
            parser.add_argument("-dp", "--dirpath",
                                action="store",
                                type=str,
                                dest="dirpath",
                                default="",
                                help="Specify fuzz test dirpath"
                                )
            parser.add_argument("-fn", "--fuzzername",
                                action="store",
                                type=str,
                                dest="fuzzername",
                                default="",
                                help="Specify fuzzer name"
                                )
            (options, unparsed) = parser.parse_known_args(para_list)

            # Set default value
            options.target_os_name = "OHOS"
            options.build_variant = "release"
            options.device_sn = ""
            options.config = ""
            options.reportpath = ""
            options.exectype = "device"
            options.testdriver = ""

        except SystemExit:
            valid_param = False
            parser.print_help()
            LOG.warning("Parameter parsing systemexit exception.")

        return options, unparsed, valid_param

    def command_parser(self, args):
        try:
            para_list = args.split()
            options, _, valid_param = self.argument_parser(para_list)
            if options is None or not valid_param:
                LOG.warning("options is None.")
                return

            command = options.action
            if command == "":
                LOG.warning("action is empty.")
                return

            if "productform" in self.wizard_dic.keys():
                productform = self.wizard_dic["productform"]
                options.productform = productform
            else:
                productform = options.productform

            if command.startswith(ToolCommandType.TOOLCMD_KEY_HELP):
                self._process_command_help(para_list)
            elif command.startswith(ToolCommandType.TOOLCMD_KEY_SHOW):
                self._process_command_show(para_list, productform)
            elif command.startswith(ToolCommandType.TOOLCMD_KEY_GEN):
                self._process_command_gen(command, options)
            elif command.startswith(ToolCommandType.TOOLCMD_KEY_RUN):
                self._process_command_run(command, options)
            elif command.startswith(ToolCommandType.TOOLCMD_KEY_QUIT):
                self._process_command_quit(command)
            elif command.startswith(ToolCommandType.TOOLCMD_KEY_LIST):
                self._process_command_device(command)
            else:
                print("The %s command is not supported." % command)
        except (AttributeError, IOError, IndexError, ImportError, NameError,
                RuntimeError, SystemError, TypeError, ValueError,
                UnicodeError) as exception:
            LOG.exception(exception, exc_info=False)

    @classmethod
    def _process_command_help(cls, para_list):
        if para_list[0] == ToolCommandType.TOOLCMD_KEY_HELP:
            display_help_info(para_list)
        else:
            LOG.error("Wrong help command.")
        return

    @classmethod
    def _process_command_show(cls, para_list, productform="phone"):
        if para_list[0] == ToolCommandType.TOOLCMD_KEY_SHOW:
            display_show_info(para_list, productform)
        else:
            LOG.error("Wrong show command.")
        return

    @classmethod
    def _process_command_gen(cls, command, options):
        if command == ToolCommandType.TOOLCMD_KEY_GEN:
            Gen().process_command_gen(options)
        else:
            LOG.error("Wrong gen command.")
        return

    @classmethod
    def _process_command_run(cls, command, options):
        if command == ToolCommandType.TOOLCMD_KEY_RUN:
            Run().process_command_run(command, options)
        else:
            LOG.error("Wrong run command.")
        return

    @classmethod
    def _process_command_device(cls, command):
        if command == ToolCommandType.TOOLCMD_KEY_LIST:
            env_manager = EnvironmentManager()
            env_manager.list_devices()
        else:
            LOG.error("Wrong list command.")
        return

    @classmethod
    def _process_command_quit(cls, command):
        if command == ToolCommandType.TOOLCMD_KEY_QUIT:
            env_manager = EnvironmentManager()
            env_manager.env_stop()
            sys.exit(0)
        else:
            LOG.error("Wrong exit command.")
        return

    @classmethod
    def _build_version(cls, product_form):
        is_build_version = UserConfigManager().get_user_config_flag(
            "build", "version")

        project_root_path = sys.source_code_root_path
        if project_root_path == "":
            return True

        build_result = True
        if is_lite_product(product_form, sys.source_code_root_path):
            if not is_build_version:
                return True
            from core.build.build_lite_manager import BuildLiteManager
            build_lite_manager = BuildLiteManager(project_root_path)
            build_result = build_lite_manager.build_version(product_form)
        else:
            from core.build.build_manager import BuildManager
            build_manager = BuildManager()
            if is_build_version:
                build_result = build_manager.build_version(project_root_path,
                                                           product_form)
            else:
                build_result = build_manager.build_gn_file(project_root_path,
                                                           product_form)
        return build_result


##############################################################################
##############################################################################
