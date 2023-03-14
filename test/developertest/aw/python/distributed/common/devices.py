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

import os
import time
import platform
import subprocess

##############################################################################
##############################################################################

__all__ = ["DeviceAdapter", "HDCDeviceAdapter"]

if platform.system() != 'Windows':
    QUOTATION_MARKS = "'"
else:
    QUOTATION_MARKS = "\""
USB_TOOLS = "hdc"
HDC_TOOLS = "hdc"


##############################################################################
##############################################################################


def get_package_name(hap_filepath):
    package_name = ""

    if os.path.exists(hap_filepath):
        filename = os.path.basename(hap_filepath)

        # unzip the hap file
        hap_bak_path = os.path.abspath(os.path.join(
            os.path.dirname(hap_filepath),
            "%s.bak" % filename))
        zf_desc = zipfile.ZipFile(hap_filepath)
        try:
            zf_desc.extractall(path=hap_bak_path)
        except RuntimeError as error:
            print(error)
        zf_desc.close()

        # verify config.json file
        app_profile_path = os.path.join(hap_bak_path, "config.json")
        if os.path.isfile(app_profile_path):
            load_dict = {}
            with open(app_profile_path, 'r') as json_file:
                load_dict = json.load(json_file)
            profile_list = load_dict.values()
            for profile in profile_list:
                package_name = profile.get("package")
                if not package_name:
                    continue
                break

        # delete hap_bak_path
        if os.path.exists(hap_bak_path):
            shutil.rmtree(hap_bak_path)
    else:
        print("file %s not exists" % hap_filepath)

    return package_name


##############################################################################
##############################################################################


class DeviceAdapter:
    def __init__(self, remote_ip="", repote_port="", device_sn="", name=""):
        self.device_sn = device_sn
        self.name = name
        self.test_path = "/%s/%s" % ("data", "test")
        self.device_para = self.get_device_para(
            remote_ip,
            repote_port,
            device_sn)
        self.init_device()

    ###############################################################
    ###############################################################

    def init_device(self):
        self.remount()
        self.shell('rm -rf %s' % self.test_path)
        self.shell('mkdir -p %s' % self.test_path)
        self.shell('chmod 777 %s' % self.test_path)
        self.shell("mount -o rw,remount,rw /%s" % "system")

    def remount(self):
        command = "%s %s remount" % (USB_TOOLS, self.device_para)
        self.execute_command(command)

    def push_file(self, srcpath, despath):
        command = "%s %s push %s %s" % (
            USB_TOOLS,
            self.device_para,
            srcpath,
            despath)
        return self.execute_command(command)

    def pull_file(self, srcpath, despath):
        command = "%s %s pull %s %s" % (
            USB_TOOLS,
            self.device_para,
            srcpath,
            despath)
        return self.execute_command(command)

    def unlock_screen(self):
        self.shell("svc power stayon true")

    def unlock_device(self):
        self.shell("input keyevent 82")
        self.shell("wm dismiss-keyguard")

    def lock_screen(self):
        self.shell("svc power stayon false")

    def disable_keyguard(self):
        self.unlock_screen()
        self.unlock_device()

    def install_hap(self, suite_file):
        file_name = os.path.basename(suite_file)
        message = self.shell_with_output("bm install -p %s" % os.path.join(
            self.test_path, file_name))
        message = str(message).rstrip()
        if message != "":
            print(message)
        if message == "" or "Success" in message:
            return_code = True
        else:
            return_code = False
        time.sleep(1)
        return return_code

    def uninstall_hap(self, suite_file):
        package_name = get_package_name(suite_file)
        result = self.shell("pm uninstall %s" % package_name)
        time.sleep(1)
        return result

    def install_app(self, file_path):
        command = "%s %s install %s" % (
            USB_TOOLS,
            self.device_para,
            file_path)
        message = self.execute_command(command)
        message = str(message).rstrip()
        if message != "":
            print(message)
        if message == "" or "Success" in message:
            return_code = True
        else:
            return_code = False
        time.sleep(1)
        return return_code

    def uninstall_app(self, package_name):
        command = "pm uninstall %s" % (package_name)
        return_code = self.shell_with_output(command)
        if return_code:
            time.sleep(1)
        return return_code


    ###############################################################
    ###############################################################

    @classmethod
    def get_device_para(cls, remote_ip="", remote_port="",
                          device_sn=""):
        if "" == remote_ip or "" == remote_port:
            if "" == device_sn:
                device_para = ""
            else:
                device_para = "-s %s" % device_sn
        else:
            if "" == device_sn:
                device_para = "-H %s -P %s" % (remote_ip, remote_port)
            else:
                device_para = "-H %s -P %s -s %s" % (
                    remote_ip, remote_port, device_sn)
        return device_para

    def execute_command(self, command, print_flag=True, timeout=900):
        try:
            if print_flag:
                print("command: " + command)
            if subprocess.call(command, shell=True, timeout=timeout) == 0:
                print("results: successed")
                return True
        except Exception as error:
            print("Exception: %s" % str(error))
        print("results: failed")
        return False

    def execute_command_with_output(self, command, print_flag=True):
        if print_flag:
            print("command: " + command)

        proc = subprocess.Popen(command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            shell=True)

        try:
            data, _ = proc.communicate()
            if isinstance(data, bytes):
                data = data.decode('utf-8', 'ignore')
        finally:
            proc.stdout.close()
            proc.stderr.close()
        return data

    def shell(self, command=""):
        return self.execute_command("%s %s shell %s%s%s" % (
            USB_TOOLS,
            self.device_para,
            QUOTATION_MARKS,
            command,
            QUOTATION_MARKS))

    def execute_shell_command(self, command):
        return self.shell(command)

    def shell_with_output(self, command=""):
        return self.execute_command_with_output("%s %s shell %s%s%s" % (
            USB_TOOLS,
            self.device_para,
            QUOTATION_MARKS,
            command,
            QUOTATION_MARKS))

    def check_path_legal(self, path):
        if path and " " in path:
            return "\"%s\"" % path
        return path

    def is_file_exist(self, file_path):
        file_path = self.check_path_legal(file_path)
        message = self.shell_with_output("ls %s" % file_path)
        return False if message == "" else True


##############################################################################
##############################################################################


class HDCDeviceAdapter:
    def __init__(self, remote_ip="", repote_port="", device_sn="", name=""):
        self.device_sn = device_sn
        self.name = name
        self.test_path = "/%s/%s/" % ("data", "test")
        self.device_para = self.get_device_para(
            remote_ip,
            repote_port,
            device_sn)
        self.init_device()

    ###############################################################
    ###############################################################

    def init_device(self):
        self.remount()
        self.shell('rm -rf %s' % self.test_path)
        self.shell('mkdir -p %s' % self.test_path)
        self.shell('chmod 777 %s' % self.test_path)
        self.shell("mount -o rw,remount,rw /%s" % "system")

    def remount(self):
        command = "%s %s target mount" % (HDC_TOOLS)
        self.execute_command(command)

    def push_file(self, srcpath, despath):
        command = "%s %s file send %s %s" % (
            HDC_TOOLS,
            self.device_para,
            srcpath,
            despath)
        return self.execute_command(command)

    def pull_file(self, srcpath, despath):
        command = "%s %s file recv %s %s" % (
            HDC_TOOLS,
            self.device_para,
            srcpath,
            despath)
        return self.execute_command(command)

    def unlock_screen(self):
        self.shell("svc power stayon true")

    def unlock_device(self):
        self.shell("input keyevent 82")
        self.shell("wm dismiss-keyguard")

    def lock_screen(self):
        self.shell("svc power stayon false")

    def disable_keyguard(self):
        self.unlock_screen()
        self.unlock_device()


    ###############################################################
    ###############################################################

    @classmethod
    def get_device_para(cls, remote_ip="", remote_port="",
                          device_sn=""):
        if "" == remote_ip or "" == remote_port:
            if "" == device_sn:
                device_para = ""
            else:
                device_para = "-t %s" % device_sn
        else:
            if "" == device_sn:
                device_para = "-s tcp:%s:%s" % (remote_ip, remote_port)
            else:
                device_para = "-s tcp:%s:%s -t %s" % (
                    remote_ip, remote_port, device_sn)
        return device_para

    def execute_command(self, command, print_flag=True, timeout=900):
        try:
            if print_flag:
                print("command: " + command)
            if subprocess.call(command, shell=True, timeout=timeout) == 0:
                print("results: successed")
                return True
        except Exception as error:
            print("Exception: %s" % str(error))
        print("results: failed")
        return False

    def execute_command_with_output(self, command, print_flag=True):
        if print_flag:
            print("command: " + command)

        proc = subprocess.Popen(command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            shell=True)

        try:
            data, _ = proc.communicate()
            if isinstance(data, bytes):
                data = data.decode('utf-8', 'ignore')
        finally:
            proc.stdout.close()
            proc.stderr.close()
        return data

    def shell(self, command=""):
        return self.execute_command("%s %s shell %s%s%s" % (
            HDC_TOOLS,
            self.device_para,
            QUOTATION_MARKS,
            command,
            QUOTATION_MARKS))

    def shell_with_output(self, command=""):
        return self.execute_command_with_output("%s %s shell %s%s%s" % (
            HDC_TOOLS,
            self.device_para,
            QUOTATION_MARKS,
            command,
            QUOTATION_MARKS))

    def check_path_legal(self, path):
        if path and " " in path:
            return "\"%s\"" % path
        return path

    def is_file_exist(self, file_path):
        file_path = self.check_path_legal(file_path)
        message = self.shell_with_output("ls %s" % file_path)
        return False if message == "" else True


##############################################################################
##############################################################################

