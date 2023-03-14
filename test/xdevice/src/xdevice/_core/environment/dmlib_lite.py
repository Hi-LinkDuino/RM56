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

import json
import time
import re

from _core.constants import DeviceTestType
from _core.exception import ExecuteTerminate
from _core.exception import LiteDeviceTimeout
from _core.exception import LiteDeviceConnectError
from _core.exception import LiteDeviceExecuteCommandError
from _core.logger import platform_logger
from _core.utils import convert_port

__all__ = ["generate_report", "LiteHelper"]

CPP_TEST_STANDARD_SIGN = "[==========]"
CPP_TEST_END_SIGN = "Gtest xml output finished"
CPP_SYS_STANDARD_SIGN = "OHOS #"
CPP_ERR_MESSAGE = "[ERR]No such file or directory: "
CTEST_STANDARD_SIGN = "Start to run test suite"
AT_CMD_ENDS = "OK"
CTEST_END_SIGN = "All the test suites finished"
CPP_TEST_STOP_SIGN = "Test Stop"
CPP_TEST_MOUNT_SIGN = "not mount properly"

_START_JSUNIT_RUN_MARKER = "[start] start run suites"
_END_JSUNIT_RUN_MARKER = "[end] run suites end"
INSTALL_END_MARKER = "resultMessage is install success !"

PATTERN = re.compile(r'\x1B(\[([0-9]{1,2}(;[0-9]{1,2})*)?m)*')
TIMEOUT = 90
LOG = platform_logger("DmlibLite")


def check_open_source_test(result_output):
    if result_output.find(CPP_TEST_STANDARD_SIGN) == -1 and \
            ("test pass" in result_output.lower() or
             "test fail" in result_output.lower() or
             "tests pass" in result_output.lower() or
             "tests fail" in result_output.lower()):
        return True
    return False


def check_read_test_end(result=None, input_command=None):
    temp_result = result.replace("\n", "")
    # if input_command not in temp_result:
    #     return False
    index = result.find(input_command) + len(input_command)
    result_output = result[index:]
    if input_command.startswith("./"):
        if result_output.find(CPP_TEST_STANDARD_SIGN) != -1:
            if result_output.count(CPP_TEST_STANDARD_SIGN) == 2 or \
                    result_output.find(CPP_TEST_END_SIGN) != -1:
                return True
        if check_open_source_test(result_output):
            return True
        if result_output.find(_START_JSUNIT_RUN_MARKER) >= 1 and \
                result_output.find(_END_JSUNIT_RUN_MARKER) >= 1:
            return True

        if result_output.find(INSTALL_END_MARKER) != -1:
            return True
        if (result_output.find(CPP_TEST_MOUNT_SIGN) != -1
                and result_output.find(CPP_TEST_STOP_SIGN) != -1):
            LOG.info("find test stop")
            return True
        if "%s%s" % (CPP_ERR_MESSAGE, input_command[2:]) in result_output:
            LOG.error("execute file not exist, result is %s" % result_output,
                      error_no="00402")
            raise LiteDeviceExecuteCommandError("execute file not exist",
                                                error_no="00402")
    elif input_command.startswith("zcat"):
        return False
    else:
        if "OHOS #" in result_output or "# " in result_output:
            if input_command == "reboot" or input_command == "reset":
                return False
            if input_command.startswith("mount"):
                if "Mount nfs finished." not in result_output:
                    return False
            return True
    return False


def generate_report(receiver, result):
    if result and receiver:
        if result:
            receiver.__read__(result)
            receiver.__done__()


def get_current_time():
    current_time = time.time()
    local_time = time.localtime(current_time)
    data_head = time.strftime("%Y-%m-%d %H:%M:%S", local_time)
    millisecond = (current_time - int(current_time)) * 1000
    return "%s.%03d" % (data_head, millisecond)


class LiteHelper:
    @staticmethod
    def execute_remote_cmd_with_timeout(telnet, command="", timeout=TIMEOUT,
                                        receiver=None):
        """
        Executes command on the device.

        Parameters:
            telnet:
            command: the command to execute
            timeout: timeout for read result
            receiver: parser handler
        """
        from xdevice import Scheduler
        time.sleep(2)
        start_time = time.time()
        status = True
        error_message = ""
        result = ""
        if not telnet:
            raise LiteDeviceConnectError("remote device is not connected.",
                                         error_no="00402")

        telnet.write(command.encode('ascii') + b"\n")
        while time.time() - start_time < timeout:
            data = telnet.read_until(bytes(command, encoding="utf8"),
                                     timeout=1)
            data = PATTERN.sub('', data.decode('gbk', 'ignore')).replace(
                "\r", "")
            result = "{}{}".format(result, data)
            if command in result:
                break

        expect_result = [bytes(CPP_TEST_STANDARD_SIGN, encoding="utf8"),
                         bytes(CPP_SYS_STANDARD_SIGN, encoding="utf8"),
                         bytes(CPP_TEST_END_SIGN, encoding="utf8"),
                         bytes(CPP_TEST_STOP_SIGN, encoding="utf8")]
        while time.time() - start_time < timeout:
            if not Scheduler.is_execute:
                raise ExecuteTerminate("Execute terminate", error_no="00300")
            _, _, data = telnet.expect(expect_result, timeout=1)
            data = PATTERN.sub('', data.decode('gbk', 'ignore')).replace(
                "\r", "")
            result = "{}{}".format(result, data)
            if receiver and data:
                receiver.__read__(data)
            if check_read_test_end(result, command):
                break
        else:
            error_message = "execute %s timed out %s " % (command, timeout)
            status = False

        if receiver:
            receiver.__done__()

        if not status and command.startswith("uname"):
            raise LiteDeviceTimeout("Execute command time out:%s" % command)

        return result, status, error_message

    @staticmethod
    def read_local_output_test(com=None, command=None, timeout=TIMEOUT,
                               receiver=None):
        input_command = command
        linux_end_command = ""
        if "--gtest_output=" in command:
            linux_end_command = input_command.split(":")[1].split(
                "reports")[0].rstrip("/") + " #"
        error_message = ""
        start_time = time.time()
        result = ""
        status = True
        from xdevice import Scheduler

        # while time.time() - start_time < timeout:
        #     data = com.readline().decode('gbk', errors='ignore')
        #     data = PATTERN.sub('', data).replace("\r", "")
        #     result = "{}{}".format(result, data)
        #     if command in result or linux_end_command in result:
        #         break

        while time.time() - start_time < timeout:
            if not Scheduler.is_execute:
                raise ExecuteTerminate("Execute terminate", error_no="00300")
            data = com.readline().decode('gbk', errors='ignore')
            data = PATTERN.sub('', data).replace("\r", "")
            result = "{}{}".format(result, data)
            if receiver and data:
                receiver.__read__(data)
            if check_read_test_end(result, input_command):
                break
        else:
            error_message = "execute %s timed out %s " % (command, timeout)
            status = False

        if receiver:
            receiver.__done__()

        if not status and command.startswith("uname"):
            raise LiteDeviceTimeout("Execute command time out:%s" % command)

        return result, status, error_message

    @staticmethod
    def read_local_output_ctest(com=None, command=None, timeout=TIMEOUT,
                                receiver=None):
        result = ""
        input_command = command

        start = time.time()
        from xdevice import Scheduler
        while True:
            if not Scheduler.is_execute:
                raise ExecuteTerminate("Execute terminate", error_no="00300")
            data = com.readline().decode('gbk', errors='ignore')
            data = PATTERN.sub('', data)
            if isinstance(input_command, list):
                if len(data.strip()) > 0:
                    data = "{} {}".format(get_current_time(), data)
                    if data and receiver:
                        receiver.__read__(data.replace("\r", ""))
                    result = "{}{}".format(result, data.replace("\r", ""))
                    if re.search(r"\d+\s+Tests\s+\d+\s+Failures\s+\d+\s+"
                                 r"Ignored", data):
                        start = time.time()
                    if CTEST_END_SIGN in data:
                        break
                if (int(time.time()) - int(start)) > timeout:
                    break
            else:
                result = "{}{}".format(
                    result, data.replace("\r", "").replace("\n", "").strip())
                if AT_CMD_ENDS in data:
                    return result, True, ""
                if (int(time.time()) - int(start)) > timeout:
                    return result, False, ""

        if receiver:
            receiver.__done__()
        LOG.info('Info: execute command success')
        return result, True, ""

    @staticmethod
    def read_local_output(com=None, command=None, case_type="",
                          timeout=TIMEOUT, receiver=None):
        if case_type == DeviceTestType.ctest_lite:
            return LiteHelper.read_local_output_ctest(com, command,
                                                      timeout, receiver)
        else:
            return LiteHelper.read_local_output_test(com, command,
                                                     timeout, receiver)

    @staticmethod
    def execute_local_cmd_with_timeout(com, **kwargs):
        """
        Execute command on the serial and read all the output from the serial.
        """
        args = kwargs
        command = args.get("command", None)
        input_command = command
        case_type = args.get("case_type", "")
        timeout = args.get("timeout", TIMEOUT)
        receiver = args.get("receiver", None)
        if not com:
            raise LiteDeviceConnectError("local device is not connected.",
                                         error_no="00402")

        LOG.info("local_%s execute command shell %s with timeout %ss" %
                 (convert_port(com.port), command, str(timeout)))

        if isinstance(command, str):
            command = command.encode("utf-8")
            if command[-2:] != b"\r\n":
                command = command.rstrip() + b'\r\n'
                com.write(command)
        else:
            com.write(command)
        return LiteHelper.read_local_output(
            com, command=input_command, case_type=case_type, timeout=timeout,
            receiver=receiver)

    @staticmethod
    def execute_local_command(com, command):
        """
        Execute command on the serial and read all the output from the serial.
        """
        if not com:
            raise LiteDeviceConnectError("local device is not connected.",
                                         error_no="00402")

        LOG.info(
            "local_%s execute command shell %s" % (convert_port(com.port),
                                                   command))
        command = command.encode("utf-8")
        if command[-2:] != b"\r\n":
            command = command.rstrip() + b'\r\n'
            com.write(command)

    @staticmethod
    def execute_agent_cmd_with_timeout(self, command,
                                       **kwargs):
        import requests

        base_url = self.base_url
        headers = self.headers
        local_source_dir = self.local_source_dir
        target_save_path = self.target_save_path
        args = kwargs
        command_type = args.get("type", None)
        sync = args.get("sync", 1)
        response = None
        try:
            if "cmd" == command_type:
                url = base_url + "/_processes/"
                data = {
                    'cmd': command,
                    'sync': sync,
                    'stdoutRedirect': 'file',
                    'stderrRedirect': 'file'
                }
                response = requests.post(url=url, headers=headers,
                                         data=json.dumps(data))
                if response.status_code == 200:
                    LOG.info("connect OK")
                else:
                    LOG.info("connect failed")
                response.close()

            elif "put" == command_type:
                url = base_url + target_save_path + command
                with open(local_source_dir + command, "rb") as data:
                    response = requests.put(url=url, headers=headers,
                                            data=data)
                if response.status_code == 200:
                    LOG.info("{} upload file to {} "
                             "success".format(local_source_dir,
                                              target_save_path))
                else:
                    LOG.info(
                        "{} upload file to {} fail".format(local_source_dir,
                                                           target_save_path))
                response.close()
            elif "get" == command_type:
                url = base_url + target_save_path + command
                response = requests.get(url=url, headers=headers, stream=True)
                if response.status_code == 200:
                    with open(local_source_dir + command + "bak", "wb") \
                            as file_name:
                        for file_data in response.iter_content(chunk_size=512):
                            file_name.write(file_data)
                    LOG.info("from {} download  file to {} success".format(
                        target_save_path + command,
                        local_source_dir))
                else:
                    LOG.info("{} download  file to {} fail".format(
                        target_save_path + command,
                        command,
                        local_source_dir))
            elif "delete" == command_type:
                url = base_url + target_save_path + command
                LOG.info(url)
                response = requests.delete(url)
                if response.status_code == 200:
                    LOG.info("delete {}  file  success".format(
                        target_save_path + command))
                else:
                    LOG.info("delete {}  file  fail".format(
                        target_save_path + command))
            else:
                LOG.info("type error")
        except Exception as error_message:
            LOG.info("error_message:{}".format(error_message))
        finally:
            if response:
                response.close()

        return "", True, ""
