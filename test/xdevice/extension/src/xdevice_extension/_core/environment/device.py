#!/usr/bin/env python3
# coding=utf-8

#
# Copyright (c) 2021 Huawei Device Co., Ltd.
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

import threading

from xdevice import DeviceOsType
from xdevice import ProductForm
from xdevice import ReportException
from xdevice import IDevice
from xdevice import platform_logger
from xdevice import Plugin
from xdevice import exec_cmd
from xdevice import ConfigConst

from xdevice_extension._core import utils
from xdevice_extension._core.environment.dmlib import HdcHelper
from xdevice_extension._core.exception import HdcError
from xdevice_extension._core.environment.dmlib import CollectingOutputReceiver
from xdevice_extension._core.environment.device_state import \
    DeviceAllocationState
from xdevice_extension._core.utils import check_path_legal
from xdevice_extension._core.utils import convert_serial
from xdevice_extension._core.constants import DeviceConnectorType

__all__ = ["Device"]
TIMEOUT = 90 * 1000
RETRY_ATTEMPTS = 2
DEFAULT_UNAVAILABLE_TIMEOUT = 20 * 1000
BACKGROUND_TIME = 2 * 60 * 1000
LOG = platform_logger("Device")


def perform_device_action(func):
    def device_action(self, *args, **kwargs):
        if not self.get_recover_state():
            LOG.debug("device %s %s is false" % (self.device_sn,
                                                 ConfigConst.recover_state))
            return
        # avoid infinite recursion, such as device reboot
        abort_on_exception = bool(kwargs.get("abort_on_exception", False))
        if abort_on_exception:
            result = func(self, *args, **kwargs)
            return result

        tmp = int(kwargs.get("retry", RETRY_ATTEMPTS))
        retry = tmp + 1 if tmp > 0 else 1
        exception = None
        for _ in range(retry):
            try:
                result = func(self, *args, **kwargs)
                return result
            except ReportException as error:
                self.log.exception("Generate report error!", exc_info=False)
                exception = error
            except (ConnectionResetError, ConnectionRefusedError) as error:
                self.log.error("error type: %s, error: %s" %
                               (error.__class__.__name__, error))
                if self.usb_type == DeviceConnectorType.hdc:
                    cmd = "hdc reset"
                    self.log.info("re-execute hdc reset")
                    exec_cmd(cmd)
                if not self.recover_device():
                    LOG.debug("set device %s %s false" % (
                        self.device_sn, ConfigConst.recover_state))
                    self.set_recover_state(False)
                    raise error
                exception = error
            except HdcError as error:
                self.log.error("error type: %s, error: %s" %
                               (error.__class__.__name__, error))
                if not self.recover_device():
                    LOG.debug("set device %s %s false" % (
                        self.device_sn, ConfigConst.recover_state))
                    self.set_recover_state(False)
                    raise error
                exception = error
            except Exception as error:
                self.log.exception("error type: %s, error: %s" % (
                    error.__class__.__name__, error), exc_info=False)
                exception = error
        raise exception

    return device_action


@Plugin(type=Plugin.DEVICE, id=DeviceOsType.default)
class Device(IDevice):
    """
    Class representing a device.

    Each object of this class represents one device in xDevice,
    including handles to hdc, fastboot, and test agent.

    Attributes:
        device_sn: A string that's the serial number of the device.
    """

    device_sn = None
    host = None
    port = None
    usb_type = None
    is_timeout = False
    device_hilog_proc = None
    device_os_type = DeviceOsType.default
    test_device_state = None
    device_allocation_state = DeviceAllocationState.available
    label = None
    log = platform_logger("Device")
    device_state_monitor = None
    reboot_timeout = 2 * 60 * 1000
    hilog_file_pipe = None

    model_dict = {
        'default': ProductForm.phone,
        'car': ProductForm.car,
        'tv': ProductForm.television,
        'watch': ProductForm.watch,
        'tablet':  ProductForm.tablet
    }

    def __init__(self):
        self.extend_value = {}
        self.device_lock = threading.RLock()

    def __eq__(self, other):
        return self.device_sn == other.__get_serial__() and \
               self.device_os_type == other.device_os_type

    def __set_serial__(self, device_sn=""):
        self.device_sn = device_sn
        return self.device_sn

    def __get_serial__(self):
        return self.device_sn

    def get(self, key=None, default=None):
        if not key:
            return default
        value = getattr(self, key, None)
        if value:
            return value
        else:
            return self.extend_value.get(key, default)

    def recover_device(self):
        if not self.get_recover_state():
            LOG.debug("device %s %s is false, cannot recover device" % (
                self.device_sn, ConfigConst.recover_state))
            return

        LOG.debug("wait device %s to recover" % self.device_sn)
        return self.device_state_monitor.wait_for_device_available()

    def get_device_type(self):
        self.label = self.model_dict.get("default", None)

    def get_property(self, prop_name, retry=RETRY_ATTEMPTS,
                     abort_on_exception=False):
        """
        Hdc command, ddmlib function.
        """
        command = "param get %s" % prop_name
        stdout = self.execute_shell_command(
            command, timeout=5 * 1000, output_flag=False, retry=retry,
            abort_on_exception=abort_on_exception).strip()
        if stdout:
            LOG.debug(stdout)
        return stdout

    @perform_device_action
    def hdc_command(self, command, **kwargs):
        timeout = int(kwargs.get("timeout", TIMEOUT)) / 1000
        error_print = bool(kwargs.get("error_print", True))
        join_result = bool(kwargs.get("join_result", False))
        timeout_msg = '' if timeout == 300.0 else \
            " with timeout %ss" % timeout
        if self.usb_type == DeviceConnectorType.hdc:
            LOG.debug("%s execute command hdc %s%s" % (
                convert_serial(self.device_sn), command, timeout_msg))
        cmd = ["hdc_std", "-t", self.device_sn]
        if isinstance(command, list):
            cmd.extend(command)
        else:
            command = command.strip()
            cmd.extend(command.split(" "))
        result = exec_cmd(cmd, timeout, error_print, join_result)
        if not result:
            return result
        for line in str(result).split("\n"):
            if line.strip():
                LOG.debug(line.strip())
        return result

    @perform_device_action
    def execute_shell_command(self, command, timeout=TIMEOUT,
                              receiver=None, **kwargs):
        if not receiver:
            collect_receiver = CollectingOutputReceiver()
            HdcHelper.execute_shell_command(
                self, command, timeout=timeout,
                receiver=collect_receiver, **kwargs)
            return collect_receiver.output
        else:
            return HdcHelper.execute_shell_command(
                self, command, timeout=timeout,
                receiver=receiver, **kwargs)

    def execute_shell_cmd_background(self, command, timeout=TIMEOUT,
                                     receiver=None):
        status = HdcHelper.execute_shell_command(self, command,
                                                 timeout=timeout,
                                                 receiver=receiver)

        self.wait_for_device_not_available(DEFAULT_UNAVAILABLE_TIMEOUT)
        self.device_state_monitor.wait_for_device_available(BACKGROUND_TIME)
        cmd = "target mount" \
            if self.usb_type == DeviceConnectorType.hdc else "remount"
        self.hdc_command(cmd)
        self.start_catch_device_log()
        return status

    def wait_for_device_not_available(self, wait_time):
        return self.device_state_monitor.wait_for_device_not_available(
            wait_time)

    def _wait_for_device_online(self, wait_time=None):
        return self.device_state_monitor.wait_for_device_online(wait_time)

    def _do_reboot(self):
        HdcHelper.reboot(self)
        if not self.wait_for_device_not_available(
                DEFAULT_UNAVAILABLE_TIMEOUT):
            LOG.error("Did not detect device {} becoming unavailable "
                      "after reboot".format(convert_serial(self.device_sn)))

    def _reboot_until_online(self):
        self._do_reboot()
        self._wait_for_device_online()

    def reboot(self):
        self._reboot_until_online()
        self.device_state_monitor.wait_for_device_available(
            self.reboot_timeout)
        self.enable_hdc_root()
        self.start_catch_device_log()

    @perform_device_action
    def install_package(self, package_path, command=""):
        if package_path is None:
            raise HdcError(
                "install package: package path cannot be None!")
        return HdcHelper.install_package(self, package_path, command)

    @perform_device_action
    def uninstall_package(self, package_name):
        return HdcHelper.uninstall_package(self, package_name)

    @perform_device_action
    def push_file(self, local, remote, **kwargs):
        """
        Push a single file.
        The top directory won't be created if is_create is False (by default)
        and vice versa
        """
        if local is None:
            raise HdcError("XDevice Local path cannot be None!")

        remote_is_dir = kwargs.get("remote_is_dir", False)
        if remote_is_dir:
            ret = self.execute_shell_command("test -d %s && echo 0" % remote)
            if not (ret != "" and len(str(ret).split()) != 0 and
                    str(ret).split()[0] == "0"):
                self.execute_shell_command("mkdir -p %s" % remote)

        is_create = kwargs.get("is_create", False)
        timeout = kwargs.get("timeout", TIMEOUT)
        HdcHelper.push_file(self, local, remote, is_create=is_create,
                            timeout=timeout)

    @perform_device_action
    def pull_file(self, remote, local, **kwargs):
        """
        Pull a single file.
        The top directory won't be created if is_create is False (by default)
        and vice versa
        """

        is_create = kwargs.get("is_create", False)
        timeout = kwargs.get("timeout", TIMEOUT)
        HdcHelper.pull_file(self, remote, local, is_create=is_create,
                            timeout=timeout)

    def is_hdc_root(self):
        output = self.execute_shell_command("id")
        return "uid=0(root)" in output

    def enable_hdc_root(self):
        if self.is_hdc_root():
            return True
        for index in range(3):
            cmd = "smode" \
                if self.usb_type == DeviceConnectorType.hdc else "root"
            output = self.hdc_command(cmd)
            if self.is_hdc_root():
                return True
            LOG.debug(
                "hdc root on %s unsuccessful on attempt %d. "
                "Output: %s" % (
                 convert_serial(self.device_sn), index, output))
        return False

    def is_directory(self, path):
        path = check_path_legal(path)
        output = self.execute_shell_command("ls -ld {}".format(path))
        if output and output.startswith('d'):
            return True
        return False

    def is_file_exist(self, file_path):
        file_path = check_path_legal(file_path)
        output = self.execute_shell_command("ls {}".format(file_path))
        if output and "No such file or directory" not in output:
            return True
        return False

    def start_catch_device_log(self, hilog_file_pipe=None):
        """
        Starts hdc log for each device in separate subprocesses and save
        the logs in files.
        """
        if hilog_file_pipe:
            self.hilog_file_pipe = hilog_file_pipe
        self._start_catch_device_log()

    def stop_catch_device_log(self):
        """
        Stops all hdc log subprocesses.
        """
        self._stop_catch_device_log()

    def _start_catch_device_log(self):
        if self.hilog_file_pipe:
            command = "hilog"
            if self.usb_type == DeviceConnectorType.hdc:
                cmd = ['hdc_std', "-t", self.device_sn, "shell", command]
                LOG.info("execute command: %s" % " ".join(cmd).replace(
                    self.device_sn, convert_serial(self.device_sn)))
                self.device_hilog_proc = utils.start_standing_subprocess(
                    cmd, self.hilog_file_pipe)

    def _stop_catch_device_log(self):
        if self.device_hilog_proc:
            utils.stop_standing_subprocess(self.device_hilog_proc)
            self.device_hilog_proc = None
            self.hilog_file_pipe = None

    def get_recover_result(self, retry=RETRY_ATTEMPTS):
        command = "getparam ro.product.model"
        stdout = self.execute_shell_command(command, timeout=5 * 1000,
                                            output_flag=False, retry=retry,
                                            abort_on_exception=True).strip()
        if stdout:
            LOG.debug(stdout)
        return stdout

    def set_recover_state(self, state):
        with self.device_lock:
            setattr(self, ConfigConst.recover_state, state)

    def get_recover_state(self, default_state=True):
        with self.device_lock:
            state = getattr(self, ConfigConst.recover_state, default_state)
            return state

    def close(self):
        pass
