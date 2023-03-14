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
import platform
import subprocess
import signal
import stat
from tempfile import NamedTemporaryFile

from xdevice import ShellHandler
from xdevice import platform_logger
from xdevice import get_plugin
from xdevice import Plugin

LOG = platform_logger("Utils")


def get_filename_extension(file_path):
    _, fullname = os.path.split(file_path)
    filename, ext = os.path.splitext(fullname)
    return filename, ext


def start_standing_subprocess(cmd, pipe=subprocess.PIPE, return_result=False):
    """Starts a non-blocking subprocess that is going to continue running after
    this function returns.

    A subprocess group is actually started by setting sid, so we can kill all
    the processes spun out from the subprocess when stopping it. This is
    necessary in case users pass in pipe commands.

    Args:
        cmd: Command to start the subprocess with.
        pipe: pipe to get execution result
        return_result: return execution result or not

    Returns:
        The subprocess that got started.
    """
    sys_type = platform.system()
    process = subprocess.Popen(cmd, stdout=pipe, shell=False,
                               preexec_fn=None if sys_type == "Windows"
                               else os.setsid)
    if not return_result:
        return process
    else:
        rev = process.stdout.read()
        return rev.decode("utf-8").strip()


def stop_standing_subprocess(process):
    """Stops a subprocess started by start_standing_subprocess.

    Catches and ignores the PermissionError which only happens on Macs.

    Args:
        process: Subprocess to terminate.
    """
    try:
        sys_type = platform.system()
        signal_value = signal.SIGINT if sys_type == "Windows" \
            else signal.SIGTERM
        os.kill(process.pid, signal_value)
    except (PermissionError, AttributeError, FileNotFoundError,
            SystemError) as error:
        LOG.error("stop standing subprocess error '%s'" % error)


def get_decode(stream):
    if not isinstance(stream, str) and not isinstance(stream, bytes):
        ret = str(stream)
    else:
        try:
            ret = stream.decode("utf-8", errors="ignore")
        except (ValueError, AttributeError, TypeError):
            ret = str(stream)
    return ret


def is_proc_running(pid, name=None):
    if platform.system() == "Windows":
        proc_sub = subprocess.Popen(["C:\\Windows\\System32\\tasklist"],
                                    stdout=subprocess.PIPE,
                                    shell=False)
        proc = subprocess.Popen(["C:\\Windows\\System32\\findstr", "%s" % pid],
                                stdin=proc_sub.stdout,
                                stdout=subprocess.PIPE, shell=False)
    else:
        # /bin/ps -ef | /bin/grep -v grep | /bin/grep -w pid
        proc_sub = subprocess.Popen(["/bin/ps", "-ef"],
                                    stdout=subprocess.PIPE,
                                    shell=False)
        proc_v_sub = subprocess.Popen(["/bin/grep", "-v", "grep"],
                                      stdin=proc_sub.stdout,
                                      stdout=subprocess.PIPE,
                                      shell=False)
        proc = subprocess.Popen(["/bin/grep", "-w", "%s" % pid],
                                stdin=proc_v_sub.stdout,
                                stdout=subprocess.PIPE, shell=False)
    (out, _) = proc.communicate()
    out = get_decode(out).strip()
    LOG.debug("check %s proc running output: %s", pid, out)
    if out == "":
        return False
    else:
        return True if name is None else out.find(name) != -1


def create_dir(path):
    """Creates a directory if it does not exist already.

    Args:
        path: The path of the directory to create.
    """
    full_path = os.path.abspath(os.path.expanduser(path))
    if not os.path.exists(full_path):
        os.makedirs(full_path, exist_ok=True)


def modify_props(device, local_prop_file, target_prop_file, new_props):
    """To change the props if need
    Args:
        device: the device to modify props
        local_prop_file : the local file to save the old props
        target_prop_file : the target prop file to change
        new_props  : the new props
    Returns:
        True : prop file changed
        False : prop file no need to change
    """
    is_changed = False
    device.pull_file(target_prop_file, local_prop_file)
    old_props = {}
    changed_prop_key = []
    lines = []
    flags = os.O_RDONLY
    modes = stat.S_IWUSR | stat.S_IRUSR
    with os.fdopen(os.open(local_prop_file, flags, modes), "r") as old_file:
        lines = old_file.readlines()
        if lines:
            lines[-1] = lines[-1] + '\n'
        for line in lines:
            line = line.strip()
            if not line.startswith("#") and line.find("=") > 0:
                key_value = line.split("=")
                if len(key_value) == 2:
                    old_props[line.split("=")[0]] = line.split("=")[1]

    for key, value in new_props.items():
        if key not in old_props.keys():
            lines.append("".join([key, "=", value, '\n']))
            is_changed = True
        elif old_props.get(key) != value:
            changed_prop_key.append(key)
            is_changed = True

    if is_changed:
        local_temp_prop_file = NamedTemporaryFile(mode='w', prefix='build',
                                                  suffix='.tmp', delete=False)
        for index, line in enumerate(lines):
            if not line.startswith("#") and line.find("=") > 0:
                key = line.split("=")[0]
                if key in changed_prop_key:
                    lines[index] = "".join([key, "=", new_props[key], '\n'])
        local_temp_prop_file.writelines(lines)
        local_temp_prop_file.close()
        device.push_file(local_temp_prop_file.name, target_prop_file)
        device.execute_shell_command(" ".join(["chmod 644", target_prop_file]))
        LOG.info("Changed the system property as required successfully")
        os.remove(local_temp_prop_file.name)

    return is_changed


def convert_ip(origin_ip):
    addr = origin_ip.strip().split(".")
    if len(addr) == 4:
        return addr[0] + "." + '*'*len(addr[1]) + "." + '*'*len(addr[2]) + \
               "." + addr[-1]
    else:
        return origin_ip


def convert_port(port):
    _port = str(port)
    if len(_port) >= 2:
        return "{}{}{}".format(_port[0], "*" * (len(_port) - 2), _port[-1])
    else:
        return "*{}".format(_port[-1])


def convert_serial(serial):
    if serial.startswith("local_"):
        return "local_" + '*'*(len(serial)-6)
    elif serial.startswith("remote_"):
        return "remote_" + convert_ip(serial.split("_")[1])
    else:
        length = len(serial)//3
        return serial[0:length] + "*"*(len(serial)-length*2) + serial[-length:]


def get_shell_handler(request, parser_type):
    suite_name = request.root.source.test_name
    parsers = get_plugin(Plugin.PARSER, parser_type)
    if parsers:
        parsers = parsers[:1]
    parser_instances = []
    for listener in request.listeners:
        listener.device_sn = request.config.environment.devices[0].device_sn
    for parser in parsers:
        parser_instance = parser.__class__()
        parser_instance.suite_name = suite_name
        parser_instance.listeners = request.listeners
        parser_instances.append(parser_instance)
    handler = ShellHandler(parser_instances)
    return handler


def check_path_legal(path):
    if path and " " in path:
        return "\"%s\"" % path
    return path
