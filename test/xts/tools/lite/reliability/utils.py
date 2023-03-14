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
import datetime
import os
import signal
import subprocess
import time
import logging
import platform


def get_decode(stream):
    if isinstance(stream, str):
        return stream

    if not isinstance(stream, bytes):
        return str(stream)

    try:
        ret = stream.decode("utf-8", errors="ignore")
    except (ValueError, AttributeError, TypeError):
        ret = str(stream)
    return ret


def exec_cmd(cmd, timeout=10, error_print=True, join_result=False, waitOut=False):

    sys_type = platform.system()
    if sys_type == "Linux" or sys_type == "Darwin":
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, shell=False,
                                preexec_fn=os.setsid)
    else:
        proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, shell=True)
    if waitOut:
        try:
            (out, err) = proc.communicate(timeout=timeout)
            err = get_decode(err).strip()
            out = get_decode(out).strip()
            if err and error_print:
                logging.exception(err, exc_info=False)
            if join_result:
                return "%s\n %s" % (out, err) if err else out
            else:
                return err if err else out

        except (TimeoutError, KeyboardInterrupt, AttributeError, ValueError,
                EOFError, IOError):
            sys_type = platform.system()
            if sys_type == "Linux" or sys_type == "Darwin":
                os.killpg(proc.pid, signal.SIGTERM)
            else:
                os.kill(proc.pid, signal.SIGINT)
            raise
