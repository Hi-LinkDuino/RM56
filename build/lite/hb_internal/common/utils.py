#!/usr/bin/env python
# -*- coding: utf-8 -*-

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

import os
import re
import subprocess
import shutil
import sys
import json
import tarfile
import zipfile
import importlib
from datetime import datetime
from collections import namedtuple


def encode(data, encoding='utf-8'):
    if sys.version_info.major == 2:
        return data.encode(encoding)
    return data


def decode(data, encoding='utf-8'):
    if sys.version_info.major == 2:
        return data.decode(encoding)
    return data


def remove_path(path):
    if os.path.exists(path):
        shutil.rmtree(path)


# Read json file data
def read_json_file(input_file):
    if not os.path.isfile(input_file):
        raise OHOSException(f'{input_file} not found')

    with open(input_file, 'rb') as input_f:
        data = json.load(input_f)
        return data


def dump_json_file(dump_file, json_data):
    with open(dump_file, 'wt', encoding='utf-8') as json_file:
        json.dump(json_data, json_file, ensure_ascii=False, indent=2)


def read_yaml_file(input_file):
    if not os.path.isfile(input_file):
        raise OHOSException(f'{input_file} not found')

    yaml = importlib.import_module('yaml')
    with open(input_file, 'rt', encoding='utf-8') as yaml_file:
        try:
            return yaml.safe_load(yaml_file)
        except yaml.YAMLError as exc:
            if hasattr(exc, 'problem_mark'):
                mark = exc.problem_mark
                raise OHOSException(f'{input_file} load failed, error line:'
                                    f' {mark.line + 1}:{mark.column + 1}')


def get_input(msg):
    try:
        user_input = input
    except NameError:
        raise OHOSException('python2.x not supported')
    return user_input(msg)


def exec_command(cmd, log_path='out/build.log', **kwargs):
    useful_info_pattern = re.compile(r'\[\d+/\d+\].+')
    is_log_filter = kwargs.pop('log_filter', False)

    with open(log_path, 'at', encoding='utf-8') as log_file:
        process = subprocess.Popen(cmd,
                                   stdout=subprocess.PIPE,
                                   stderr=subprocess.STDOUT,
                                   encoding='utf-8',
                                   **kwargs)
        for line in iter(process.stdout.readline, ''):
            if is_log_filter:
                info = re.findall(useful_info_pattern, line)
                if len(info):
                    hb_info(info[0])
            else:
                hb_info(line)
            log_file.write(line)

    process.wait()
    ret_code = process.returncode

    if ret_code != 0:
        if is_log_filter:
            get_failed_log(log_path)

        raise OHOSException('Please check build log in {}'.format(log_path))


def get_failed_log(log_path):
    with open(log_path, 'rt', encoding='utf-8') as log_file:
        data = log_file.read()
    failed_pattern = re.compile(
        r'(\[\d+/\d+\].*?)(?=\[\d+/\d+\]|'
        'ninja: build stopped)', re.DOTALL)
    failed_log = failed_pattern.findall(data)
    for log in failed_log:
        if 'FAILED:' in log:
            hb_error(log)

    failed_pattern = re.compile(r'(ninja: error:.*?)\n', re.DOTALL)
    failed_log = failed_pattern.findall(data)
    for log in failed_log:
        hb_error(log)

    error_log = os.path.join(os.path.dirname(log_path), 'error.log')
    if os.path.isfile(error_log):
        with open(error_log, 'rt', encoding='utf-8') as log_file:
            hb_error(log_file.read())


def check_output(cmd, **kwargs):
    try:
        ret = subprocess.check_output(cmd,
                                      stderr=subprocess.STDOUT,
                                      universal_newlines=True,
                                      **kwargs)
    except subprocess.CalledProcessError as called_exception:
        ret = called_exception.output
        if isinstance(cmd, list):
            cmd = ' '.join(cmd)
        raise OHOSException(f'command: "{cmd}" failed\n'
                            f'return code: {ret}\n'
                            f'execution path: {os.getcwd()}')

    return ret


def makedirs(path, exist_ok=True, with_rm=False):
    try:
        os.makedirs(path)
    except OSError:
        if not os.path.isdir(path):
            raise OHOSException(f"{path} makedirs failed")
        if with_rm:
            remove_path(path)
            return os.makedirs(path)
        if not exist_ok:
            raise OHOSException(f"{path} exists, makedirs failed")


def get_project_path(json_path):
    json_data = read_json_file(json_path)

    return json_data.get('root_path')


def args_factory(args_dict):
    if not len(args_dict):
        raise OHOSException('at least one k_v param is '
                            'required in args_factory')

    args_cls = namedtuple('Args', [key for key in args_dict.keys()])
    args = args_cls(**args_dict)
    return args


def get_current_time(type='default'):
    if type == 'timestamp':
        return int(datetime.utcnow().timestamp() * 1000)
    if type == 'datetime':
        return datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    return datetime.now().replace(microsecond=0)


class Colors:
    HEADER = '\033[95m'
    WARNING = '\033[93m'
    ERROR = '\033[91m'
    INFO = '\033[92m'
    END = '\033[0m'


def hb_info(msg):
    level = 'info'
    for line in str(msg).splitlines():
        sys.stdout.write(message(level, line))
        sys.stdout.flush()


def hb_warning(msg):
    level = 'warning'
    for line in str(msg).splitlines():
        sys.stderr.write(message(level, line))
        sys.stderr.flush()


def hb_error(msg):
    level = 'error'
    for line in str(msg).splitlines():
        sys.stderr.write(message(level, line))
        sys.stderr.flush()


def message(level, msg):
    if isinstance(msg, str) and not msg.endswith('\n'):
        msg += '\n'
    if level == 'error':
        msg = msg.replace('error:', f'{Colors.ERROR}error{Colors.END}:')
        return f'{Colors.ERROR}[OHOS {level.upper()}]{Colors.END} {msg}'
    elif level == 'info':
        return f'[OHOS {level.upper()}] {msg}'
    else:
        return f'{Colors.WARNING}[OHOS {level.upper()}]{Colors.END} {msg}'


class Singleton(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(Singleton,
                                        cls).__call__(*args, **kwargs)
        return cls._instances[cls]


class OHOSException(Exception):
    pass


def download_tool(url, dst, tgt_dir=None):
    requests = importlib.import_module('requests')
    try:
        res = requests.get(url, stream=True, timeout=(5, 9))
    except OSError:
        raise OHOSException(f'download {url} timeout!')

    if res.status_code == 200:
        hb_info(f'Downloading {url} ...')
    else:
        hb_error(f'Downloading {url} failed with code: {res.status_code}!')
        return res.status_code

    total_size = int(res.headers['content-length'])
    download_size = 0
    download_percent = 0

    try:
        with open(dst, "wb") as f:
            for chunk in res.iter_content(chunk_size=1024):
                if chunk:
                    f.write(chunk)
                    download_size += len(chunk)
                    download_percent = round(
                        float(download_size / total_size * 100), 2)
                    print('Progress: %s%%\r' % download_percent, end=' ')
            hb_info('Download complete!')
    except OSError:
        raise OHOSException(
            f'{url} download failed, please install it manually!')

    if tgt_dir is not None:
        extract_tool(dst, tgt_dir)


def extract_tool(src, tgt_dir):
    hb_info(f'Extracting to {tgt_dir}, please wait...')
    try:
        if tarfile.is_tarfile(src):
            ef = tarfile.open(src)
        elif zipfile.is_zipfile(src):
            ef = zipfile.ZipFile(src)
        else:
            raise OHOSException(f'Extract file type not support!')
        ef.extractall(tgt_dir)
        ef.close()
    except OSError:
        raise OHOSException(
            f'{src} extract failed, please install it manually!')
