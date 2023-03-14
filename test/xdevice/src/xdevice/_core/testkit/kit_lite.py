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

import os
import random
import re
import string
import subprocess
import shutil
import platform
import glob
import time

from _core.logger import platform_logger
from _core.plugin import Plugin
from _core.config.config_manager import UserConfigManager
from _core.constants import CKit
from _core.constants import ConfigConst
from _core.constants import ComType
from _core.constants import DeviceLiteKernel
from _core.constants import DeviceTestType
from _core.exception import LiteDeviceMountError
from _core.exception import ParamError
from _core.exception import LiteDeviceError
from _core.interface import ITestKit
from _core.utils import get_config_value
from _core.utils import get_file_absolute_path
from _core.utils import get_local_ip
from _core.utils import get_test_component_version
from _core.exception import LiteDeviceConnectError
from _core.constants import DeviceLabelType
from _core.constants import FilePermission
from _core.environment.manager_env import DeviceAllocationState


__all__ = ["DeployKit", "MountKit", "RootFsKit", "QueryKit", "LiteShellKit",
           "LiteAppInstallKit", "DeployToolKit"]
LOG = platform_logger("KitLite")

RESET_CMD = "0xEF, 0xBE, 0xAD, 0xDE, 0x0C, 0x00, 0x87, 0x78, 0x00, 0x00, " \
            "0x61, 0x94"


@Plugin(type=Plugin.TEST_KIT, id=CKit.deploy)
class DeployKit(ITestKit):
    def __init__(self):
        self.burn_file = ""
        self.burn_command = ""
        self.timeout = ""
        self.paths = ""

    def __check_config__(self, config):
        self.timeout = str(int(get_config_value(
            'timeout', config, is_list=False, default=0)) * 1000)
        self.burn_file = get_config_value('burn_file', config, is_list=False)
        burn_command = get_config_value('burn_command', config, is_list=False,
                                        default=RESET_CMD)
        self.burn_command = burn_command.replace(" ", "").split(",")
        self.paths = get_config_value('paths', config)
        if self.timeout == "0" or not self.burn_file:
            msg = "The config for deploy kit is invalid with timeout:{}, " \
                  "burn_file:{}".format(self.timeout, self.burn_file)
            raise ParamError(msg, error_no="00108")

    def _reset(self, device):
        cmd_com = device.device.com_dict.get(ComType.cmd_com)
        try:
            cmd_com.connect()
            cmd_com.execute_command(
                command='AT+RST={}'.format(self.timeout))
            cmd_com.close()
        except (LiteDeviceConnectError, IOError) as error:
            device.device_allocation_state = DeviceAllocationState.unusable
            LOG.error(
                "The exception {} happened in deploy kit running".format(
                    error), error_no=getattr(error, "error_no",
                                             "00000"))
            raise LiteDeviceError("%s port set_up wifiiot failed" %
                                  cmd_com.serial_port,
                                  error_no=getattr(error, "error_no",
                                                   "00000"))
        finally:
            if cmd_com:
                cmd_com.close()

    def _send_file(self, device):
        burn_tool_name = "HiBurn.exe" if os.name == "nt" else "HiBurn"
        burn_tool_path = get_file_absolute_path(
            os.path.join("tools", burn_tool_name), self.paths)
        patch_file = get_file_absolute_path(self.burn_file, self.paths)
        deploy_serial_port = device.device.com_dict.get(
            ComType.deploy_com).serial_port
        deploy_baudrate = device.device.com_dict.\
            get(ComType.deploy_com).baud_rate
        port_number = re.findall(r'\d+$', deploy_serial_port)
        if not port_number:
            raise LiteDeviceError("The config of serial port {} to deploy is "
                                  "invalid".format(deploy_serial_port),
                                  error_no="00108")
        new_temp_tool_path = copy_file_as_temp(burn_tool_path, 10)
        cmd = '{} -com:{} -bin:{} -signalbaud:{}' \
            .format(new_temp_tool_path, port_number[0], patch_file,
                    deploy_baudrate)
        LOG.info('The running cmd is {}'.format(cmd))
        LOG.info('The burn tool is running, please wait..')
        return_code, out = subprocess.getstatusoutput(cmd)
        LOG.info(
            'Deploy kit to execute burn tool finished with return_code: {} '
            'output: {}'.format(return_code, out))
        os.remove(new_temp_tool_path)
        if 0 != return_code:
            device.device_allocation_state = DeviceAllocationState.unusable
            raise LiteDeviceError("%s port set_up wifiiot failed" %
                                  deploy_serial_port, error_no="00402")

    def __setup__(self, device, **kwargs):
        """
        Execute reset command on the device by cmd serial port and then upload
        patch file by deploy tool.
        Parameters:
            device: the instance of LocalController with one or more
                    ComController
        """
        del kwargs
        self._reset(device)
        self._send_file(device)

    def __teardown__(self, device):
        pass


@Plugin(type=Plugin.TEST_KIT, id=CKit.mount)
class MountKit(ITestKit):
    def __init__(self):
        self.remote = None
        self.paths = ""
        self.mount_list = []
        self.mounted_dir = set()
        self.server = ""
        self.file_name_list = []
        self.remote_info = None

    def __check_config__(self, config):
        self.remote = get_config_value('server', config, is_list=False)
        self.paths = get_config_value('paths', config)
        self.mount_list = get_config_value('mount', config, is_list=True)
        self.server = get_config_value('server', config, is_list=False,
                                       default="NfsServer")
        if not self.mount_list:
            msg = "The config for mount kit is invalid with mount:{}" \
                  .format(self.mount_list)
            LOG.error(msg, error_no="00108")
            raise TypeError("Load Error[00108]")

    def mount_on_board(self, device=None, remote_info=None, case_type=""):
        """
        Init the environment on the device server, eg. mount the testcases to
        server

        Parameters:
            device: DeviceLite, device lite on local or remote
            remote_info: dict, includes
                         linux_host: str, nfs_server ip
                         linux_directory: str, the directory on the linux
                         is_remote: str, server is remote or not
            case_type: str, CppTestLite or CTestLite, default value is
                       DeviceTestType.cpp_test_lite

        Returns:
            True or False, represent init Failed or success
        """
        if not remote_info:
            raise ParamError("failed to get server environment",
                             error_no="00108")

        linux_host = remote_info.get("ip", "")
        linux_directory = remote_info.get("dir", "")
        is_remote = remote_info.get("remote", "false")
        liteos_commands = ["cd /", "umount device_directory",
                           "mount nfs_ip:nfs_directory  device"
                           "_directory nfs"]
        linux_commands = ["cd /%s" % "storage",
                          "umount -f /%s/%s" % ("storage", "device_directory"),
                          "toybox mount -t nfs -o nolock,addr=nfs_ip nfs_ip:nfs_directory "
                          "/%s/%s" % ("storage", "device_directory"),
                          "chmod 755 -R /%s/%s" % (
                          "storage", "device_directory")]
        if not linux_host or not linux_directory:
            raise LiteDeviceMountError(
                "nfs server miss ip or directory[00108]", error_no="00108")

        commands = []
        if device.label == "ipcamera":
            env_result, status, _ = device.execute_command_with_timeout(
                command="uname", timeout=1, retry=2)
            if status:
                if env_result.find(DeviceLiteKernel.linux_kernel) != -1 or \
                        env_result.find("Linux") != -1:
                    commands = linux_commands
                    device.__set_device_kernel__(DeviceLiteKernel.linux_kernel)
                else:
                    commands = liteos_commands
                    device.__set_device_kernel__(DeviceLiteKernel.lite_kernel)
            else:
                raise LiteDeviceMountError("failed to get device env[00402]",
                                           error_no="00402")

        for mount_file in self.mount_list:
            target = mount_file.get("target", "/test_root")
            if target in self.mounted_dir:
                LOG.debug("%s is mounted" % target)
                continue
            mkdir_on_board(device, target)

            # local nfs server need use alias of dir to mount
            if is_remote.lower() == "false":
                linux_directory = get_mount_dir(linux_directory)
            for command in commands:
                command = command.replace("nfs_ip", linux_host). \
                    replace("nfs_directory", linux_directory).replace(
                    "device_directory", target).replace("//", "/")
                timeout = 15 if command.startswith("mount") else 1
                if command.startswith("mount"):
                    self.mounted_dir.add(target)
                    for mount_time in range(1, 4):
                        result, status, _ = device.\
                            execute_command_with_timeout(command=command,
                                                         case_type=case_type,
                                                         timeout=timeout)
                        if status:
                            break
                        if "already mounted" in result:
                            LOG.info("{} is mounted".format(target))
                            break
                        LOG.info("Mount failed,try "
                                 "again {} time".format(mount_time))
                        if mount_time == 3:
                            raise LiteDeviceMountError("Failed to mount the "
                                                       "device[00402]",
                                                       error_no="00402")
                else:
                    result, status, _ = device.execute_command_with_timeout(
                        command=command, case_type=case_type, timeout=timeout)
        LOG.info('prepare environment success')

    def __setup__(self, device, **kwargs):
        """
        Mount the file to the board by the nfs server.
        """
        LOG.debug("start mount kit setup")

        request = kwargs.get("request", None)
        if not request:
            raise ParamError("MountKit setup request is None",
                             error_no="02401")
        device.connect()

        config_manager = UserConfigManager(
            config_file=request.get(ConfigConst.configfile, ""),
            env=request.get(ConfigConst.test_environment, ""))
        remote_info = config_manager.get_user_config("testcases/server",
                                                     filter_name=self.server)

        copy_list = self.copy_to_server(remote_info.get("dir"),
                                        remote_info.get("ip"),
                                        request, request.config.testcases_path)

        self.mount_on_board(device=device, remote_info=remote_info,
                            case_type=DeviceTestType.cpp_test_lite)

        return copy_list

    def copy_to_server(self, linux_directory, linux_host, request,
                       testcases_dir):
        file_local_paths = []
        for mount_file in self.mount_list:
            source = mount_file.get("source")
            if not source:
                raise TypeError("The source of MountKit can not be empty "
                                "in Test.json!")
            source = source.replace("$testcases/", "").\
                replace("$resources/", "")
            file_path = get_file_absolute_path(source, self.paths)
            if os.path.isdir(file_path):
                for root, _, files in os.walk(file_path):
                    for _file in files:
                        if _file.endswith(".json"):
                            continue
                        file_local_paths.append(os.path.join(root, _file))
            else:
                file_local_paths.append(file_path)

        config_manager = UserConfigManager(
            config_file=request.get(ConfigConst.configfile, ""),
            env=request.get(ConfigConst.test_environment, ""))
        remote_info = config_manager.get_user_config("testcases/server",
                                                     filter_name=self.server)
        self.remote_info = remote_info

        if not remote_info:
            err_msg = "The name of remote device {} does not match". \
                format(self.remote)
            LOG.error(err_msg, error_no="00403")
            raise TypeError(err_msg)
        is_remote = remote_info.get("remote", "false")
        if (str(get_local_ip()) == linux_host) and (
                linux_directory == ("/data%s" % testcases_dir)):
            return
        remote_ip = remote_info.get("ip", "")
        port = remote_info.get("port", "")
        remote_dir = remote_info.get("dir", "")
        if not remote_ip or not port or not remote_dir:
            LOG.warning("nfs server's ip or port or dir is empty")
            return
        for _file in file_local_paths:
            # remote copy
            LOG.info("Trying to copy the file from {} to nfs server".
                     format(_file))
            if not is_remote.lower() == "false":
                try:
                    import paramiko
                    client = paramiko.Transport(remote_ip, int(port))
                    client.connect(username=remote_info.get("username"),
                                   password=remote_info.get("password"))
                    sftp = paramiko.SFTPClient.from_transport(client)
                    sftp.put(localpath=_file, remotepath=os.path.join(
                        remote_info.get("dir"), os.path.basename(_file)))
                    client.close()
                except (OSError, Exception) as exception:
                    msg = "copy file to nfs server failed with error {}" \
                        .format(exception)
                    LOG.error(msg, error_no="00403")
            # local copy
            else:
                for count in range(1, 4):
                    try:
                        os.remove(os.path.join(remote_info.get("dir"),
                                               os.path.basename(_file)))
                    except Exception as _:
                        pass
                    shutil.copy(_file, remote_info.get("dir"))
                    if check_server_file(_file, remote_info.get("dir")):
                        break
                    else:
                        LOG.info(
                            "Trying to copy the file from {} to nfs "
                            "server {} times".format(_file, count))
                        if count == 3:
                            msg = "copy {} to nfs server " \
                                  "failed {} times".format(
                                os.path.basename(_file), count)
                            LOG.error(msg, error_no="00403")
                            LOG.debug("Nfs server:{}".format(glob.glob(
                                os.path.join(remote_info.get("dir"), '*.*'))))

            self.file_name_list.append(os.path.basename(_file))

        return self.file_name_list

    def __teardown__(self, device):
        if device.__get_device_kernel__() == DeviceLiteKernel.linux_kernel:
            device.execute_command_with_timeout(command="cd /storage",
                                                timeout=1)
            for mounted_dir in self.mounted_dir:
                device.execute_command_with_timeout(command="umount -f "
                                                            "/storage{}".
                                                    format(mounted_dir),
                                                    timeout=2)
                device.execute_command_with_timeout(command="rm -r /storage{}".
                                                    format(mounted_dir),
                                                    timeout=1)
        else:
            device.execute_command_with_timeout(command="cd /", timeout=1)
            for mounted_dir in self.mounted_dir:
                for mount_time in range(1, 3):
                    result, status, _ = device.execute_command_with_timeout(
                        command="umount {}".format(mounted_dir),
                        timeout=2)
                    if result.find("Resource busy") == -1:
                        device.execute_command_with_timeout(
                            command="rm -r {}".format(mounted_dir), timeout=1)
                    if status:
                        break
                    LOG.info("umount failed,try "
                             "again {} time".format(mount_time))
                    time.sleep(1)


def copy_file_as_temp(original_file, str_length):
    """
    To obtain a random string with specified length
    Parameters:
        original_file : the original file path
        str_length: the length of random string
    """
    if os.path.isfile(original_file):
        random_str = random.sample(string.ascii_letters + string.digits,
                                   str_length)
        new_temp_tool_path = '{}_{}{}'.format(
            os.path.splitext(original_file)[0], "".join(random_str),
            os.path.splitext(original_file)[1])
        return shutil.copyfile(original_file, new_temp_tool_path)


def mkdir_on_board(device, dir_path):
    """
    liteos L1 board dont support mkdir -p
    Parameters:
        device : the L1 board
        dir_path: the dir path to make
    """
    if device.__get_device_kernel__() == DeviceLiteKernel.linux_kernel:
        device.execute_command_with_timeout(command="cd /storage", timeout=1)
    else:
        device.execute_command_with_timeout(command="cd /", timeout=1)
    for sub_dir in dir_path.split("/"):
        if sub_dir in ["", "/"]:
            continue
        device.execute_command_with_timeout(command="mkdir {}".format(sub_dir),
                                            timeout=1)
        device.execute_command_with_timeout(command="cd {}".format(sub_dir),
                                            timeout=1)
    if device.__get_device_kernel__() == DeviceLiteKernel.linux_kernel:
        device.execute_command_with_timeout(command="cd /storage", timeout=1)
    else:
        device.execute_command_with_timeout(command="cd /", timeout=1)


def get_mount_dir(mount_dir):
    """
    use windows path to mount directly when the system is windows
    Parameters:
        mount_dir : the dir to mount that config in user_config.xml
        such as: the mount_dir is: D:\mount\root
                 the mount command should be: mount ip:/d/mount/root
    """
    if platform.system() == "Windows":
        mount_dir = mount_dir.replace(":", "").replace("\\", "/")
        _list = mount_dir.split("/")
        if mount_dir.startswith("/"):
            _list[1] = _list[1].lower()
        else:
            _list[0] = _list[0].lower()
        mount_dir = "/".join(_list)
        mount_dir = "/%s" % mount_dir
    return mount_dir


def check_server_file(local_file, target_path):
    for file_list in glob.glob(os.path.join(target_path, '*.*')):
        if os.path.basename(local_file) in file_list:
            return True
    return False


@Plugin(type=Plugin.TEST_KIT, id=CKit.rootfs)
class RootFsKit(ITestKit):
    def __init__(self):
        self.checksum_command = None
        self.hash_file_name = None
        self.device_label = None

    def __check_config__(self, config):
        self.checksum_command = get_config_value("command", config,
                                                 is_list=False)
        self.hash_file_name = get_config_value("hash_file_name", config,
                                               is_list=False)
        self.device_label = get_config_value("device_label", config,
                                             is_list=False)
        if not self.checksum_command or not self.hash_file_name or \
                not self.device_label:
            msg = "The config for rootfs kit is invalid : checksum :{}" \
                  " hash_file_name:{} device_label:{}" \
                .format(self.checksum_command, self.hash_file_name,
                        self.device_label)
            LOG.error(msg, error_no="00108")
            return TypeError(msg)

    def __setup__(self, device, **kwargs):
        del kwargs

        # check device label
        if not device.label == self.device_label:
            LOG.error("device label is not match '%s '" % "demo_label",
                      error_no="00108")
            return False
        else:
            report_path = self._get_report_dir()
            if report_path and os.path.exists(report_path):

                # execute command of checksum
                device.connect()
                device.execute_command_with_timeout(
                    command="cd /", case_type=DeviceTestType.cpp_test_lite)
                result, _, _ = device.execute_command_with_timeout(
                    command=self.checksum_command,
                    case_type=DeviceTestType.cpp_test_lite)
                device.close()
                # get serial from device and then join new file name
                pos = self.hash_file_name.rfind(".")
                serial = "_%s" % device.__get_serial__()
                if pos > 0:
                    hash_file_name = "".join((self.hash_file_name[:pos],
                                              serial,
                                              self.hash_file_name[pos:]))
                else:
                    hash_file_name = "".join((self.hash_file_name, serial))
                hash_file_path = os.path.join(report_path, hash_file_name)
                # write result to file
                hash_file_path_open = os.open(hash_file_path, os.O_WRONLY |
                                              os.O_CREAT | os.O_APPEND,
                                              FilePermission.mode_755)

                with os.fdopen(hash_file_path_open, mode="w") as hash_file:
                    hash_file.write(result)
                    hash_file.flush()
            else:
                msg = "RootFsKit teardown, log path [%s] not exists!" \
                      % report_path
                LOG.error(msg, error_no="00440")
                return False
            return True

    def __teardown__(self, device):
        pass

    @staticmethod
    def _get_report_dir():
        from xdevice import Variables
        report_path = os.path.join(Variables.exec_dir,
                                   Variables.report_vars.report_dir,
                                   Variables.task_name)
        return report_path


@Plugin(type=Plugin.TEST_KIT, id=CKit.query)
class QueryKit(ITestKit):
    def __init__(self):
        self.mount_kit = MountKit()

    def __check_config__(self, config):
        setattr(self.mount_kit, "mount_list",
                get_config_value('mount', config))
        setattr(self.mount_kit, "server", get_config_value(
            'server', config, is_list=False, default="NfsServer"))
        self.query = get_config_value('query', config, is_list=False)
        self.properties = get_config_value('properties', config, is_list=False)

        if not self.query:
            msg = "The config for query kit is invalid with query:{}" \
                  .format(self.query)
            LOG.error(msg, error_no="00108")
            raise TypeError(msg)

    def __setup__(self, device, **kwargs):
        LOG.debug("start query kit setup")
        if device.label != DeviceLabelType.ipcamera:
            return
        request = kwargs.get("request", None)
        if not request:
            raise ParamError("the request of queryKit is None",
                             error_no="02401")
        self.mount_kit.__setup__(device, request=request)
        if device.__get_device_kernel__() == DeviceLiteKernel.linux_kernel:
            device.execute_command_with_timeout(command="cd /storage",
                                                timeout=0.2)
            output, _, _ = device.execute_command_with_timeout(
                command=".{}{}".format("/storage", self.query), timeout=5)
        else:
            device.execute_command_with_timeout(command="cd /", timeout=0.2)
            output, _, _ = device.execute_command_with_timeout(
                command=".{}".format(self.query), timeout=5)
        product_info = {}
        for line in output.split("\n"):
            process_product_info(line, product_info)
        product_info["version"] = get_test_component_version(request.config)
        request.product_info = product_info

    def __teardown__(self, device):
        if device.label != DeviceLabelType.ipcamera:
            return
        device.connect()
        self.mount_kit.__teardown__(device)
        device.close()


@Plugin(type=Plugin.TEST_KIT, id=CKit.liteshell)
class LiteShellKit(ITestKit):
    def __init__(self):
        self.command_list = []
        self.tear_down_command = []
        self.paths = None

    def __check_config__(self, config):
        self.command_list = get_config_value('run-command', config)
        self.tear_down_command = get_config_value('teardown-command', config)

    def __setup__(self, device, **kwargs):
        del kwargs
        LOG.debug("LiteShellKit setup, device:{}".format(device.device_sn))
        if len(self.command_list) == 0:
            LOG.info("No setup_command to run, skipping!")
            return
        for command in self.command_list:
            run_command(device, command)

    def __teardown__(self, device):
        LOG.debug("LiteShellKit teardown: device:{}".format(device.device_sn))
        if len(self.tear_down_command) == 0:
            LOG.info("No teardown_command to run, skipping!")
            return
        for command in self.tear_down_command:
            run_command(device, command)


def run_command(device, command):
    LOG.debug("The command:{} is running".format(command))
    if command.strip() == "reset":
        device.reboot()
    else:
        device.execute_shell_command(command)


@Plugin(type=Plugin.TEST_KIT, id=CKit.liteinstall)
class LiteAppInstallKit(ITestKit):
    def __init__(self):
        self.app_list = ""
        self.is_clean = ""
        self.alt_dir = ""
        self.bundle_name = None
        self.paths = ""
        self.signature = False

    def __check_config__(self, options):
        self.app_list = get_config_value('test-file-name', options)
        self.is_clean = get_config_value('cleanup-apps', options, False)
        self.signature = get_config_value('signature', options, False)
        self.alt_dir = get_config_value('alt-dir', options, False)
        if self.alt_dir and self.alt_dir.startswith("resource/"):
            self.alt_dir = self.alt_dir[len("resource/"):]
        self.paths = get_config_value('paths', options)

    def __setup__(self, device, **kwargs):
        del kwargs
        LOG.debug("LiteAppInstallKit setup, device:{}".
                  format(device.device_sn))
        if len(self.app_list) == 0:
            LOG.info("No app to install, skipping!")
            return

        for app in self.app_list:
            if app.endswith(".hap"):
                device.execute_command_with_timeout("cd /", timeout=1)
                if self.signature:
                    device.execute_command_with_timeout(
                        command="./bin/bm set -d enable", timeout=10)
                else:
                    device.execute_command_with_timeout(
                        command="./bin/bm set -s disable", timeout=10)

                device.execute_command_with_timeout(
                    "./bin/bm install -p %s" % app, timeout=60)

    def __teardown__(self, device):
        LOG.debug("LiteAppInstallKit teardown: device:{}".format(
            device.device_sn))
        if self.is_clean and str(self.is_clean).lower() == "true" \
                and self.bundle_name:
            device.execute_command_with_timeout(
                "./bin/bm uninstall -n %s" % self.bundle_name, timeout=90)


def process_product_info(message, product_info):
    if "The" in message:
        message = message[message.index("The"):]
        items = message[len("The "):].split(" is ")
        product_info.setdefault(items[0].strip(),
                                items[1].strip().strip("[").strip("]"))


@Plugin(type=Plugin.TEST_KIT, id=CKit.deploytool)
class DeployToolKit(ITestKit):
    def __init__(self):
        self.config = None
        self.auto_deploy = None
        self.time_out = None

    def __check_config__(self, config):
        self.config = config
        self.auto_deploy = get_config_value('auto_deploy', config, False)
        self.time_out = get_config_value('timeout', config, False)
        if self.auto_deploy or not self.time_out:
            msg = "The config for deploytool kit is invalid " \
                  "with auto_deploy:{},timeout:{}".format(self.auto_deploy,
                                                          self.time_out)
            LOG.error(msg, error_no="00108")
            raise ParamError(msg, error_no="00108")

    def __setup__(self, device, **kwargs):
        args = kwargs
        request = args.get("request", None)
        request.confing.deploy_tool_kit = self.config

    def __teardown__(self, device):
        pass
