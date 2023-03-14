#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

import os
import sys
import argparse
import errno
import json
import datetime
import http.client as client

from http.server import BaseHTTPRequestHandler
from http.server import HTTPServer

PYCACHE_PORT = 7970  # Ascii code for 'yp'
LOCALHOST = '127.0.0.1'
DEBUG = int(os.environ.get('PRINT_BUILD_EXPLANATIONS', 0))


class PycacheDaemonRequestHandler(BaseHTTPRequestHandler):
    # Suppress logs
    def log_message(self, format, *args):  # pylint: disable=redefined-builtin
        if DEBUG:
            super().log_message(format, *args)
        else:
            pass

    def do_cache_hit(self):
        self.server.hit_times += 1
        self.send_response(200)

    def do_cache_miss(self):
        self.server.miss_times += 1
        self.send_response(200)

    def do_cache_manage(self):
        self.send_response(200)
        self.server.cache_manage()

    def do_show_statistics(self):
        self.send_response(200)
        self.server.show_statistics()

    def do_stop_service(self):
        self.send_response(200)
        self.server.stop_service = True


class PycacheDaemon(HTTPServer):
    def __init__(self, *args, **kargs):
        self.hit_times = 0
        self.miss_times = 0
        self.stop_service = False
        self.pycache_dir = None
        self.pycache_config_file = None
        super().__init__(*args, **kargs)

    def serve_forever(self, poll_interval=0.5):
        while not self.stop_service:
            self.handle_request()
        os.unlink(self.pycache_config_file)

    def record_pycache_config(self, pycache_dir):
        root = os.path.realpath(pycache_dir)
        self.pycache_dir = root
        self.pycache_config_file = os.path.join(root, '.config')
        os.makedirs(root, exist_ok=True)
        host, port = self.server_address[:2]
        config = {
            'root': root,
            'config_file': self.pycache_config_file,
            'debug': bool(DEBUG),
            'host': host,
            'port': port,
        }
        with open(self.pycache_config_file, 'w') as jsonfile:
            json.dump(config, jsonfile, indent=2, sort_keys=True)

    def cache_manage(self):
        now = datetime.datetime.now()
        days = 15
        earlier_time = (now - datetime.timedelta(days)).timestamp()
        # Pycache pool holds as much as 40GB  or as long as 15 days
        # cache objects
        while days > 0:
            disk_usage = 0
            for root, _, files in os.walk(self.pycache_dir):
                for file in files:
                    path = os.path.join(root, file)
                    stat = os.stat(path)
                    if stat.st_atime < int(earlier_time):
                        os.unlink(path)
                    else:
                        disk_usage += stat.st_size
            if disk_usage >= 40 * 1024 * 1024 * 1024:
                days -= 1
                earlier_time = (now - datetime.timedelta(days)).timestamp()
            else:
                break

    def show_statistics(self):
        actions = self.hit_times + self.miss_times
        if actions != 0:
            print('-' * 80)
            print('pycache statistics:')
            print('pycache hit targets: {}'.format(self.hit_times))
            print('pycache miss targets: {}'.format(self.miss_times))
            hit_rate = float(self.hit_times) / actions * 100
            miss_rate = float(self.miss_times) / actions * 100
            print('pycache hit rate: {:.2f}%'.format(hit_rate))
            print('pycache miss rate: {:.2f}%'.format(miss_rate))
            print('-' * 80)
        else:
            print('-' * 80)
            print('No pycache actions in pycache, skip statistics')
            print('-' * 80)


def start_server(host, port, root):
    if root is None:
        print('Warning: missing pycache root directory')
        return
    server_address = (host, port)
    try:
        pyd = PycacheDaemon(server_address, PycacheDaemonRequestHandler)
        print('Starting pycache daemon at {}:{}'.format(host, port))
        pyd.record_pycache_config(root)
        pyd.serve_forever()
    except OSError as err:
        if err.errno == errno.EADDRINUSE:
            start_server(host, port + 2, root)
        else:
            print('Warning: Failed to start pycache daemon process')


def get_pyd():
    cache_dir = os.environ.get('PYCACHE_DIR')
    daemon_config_file = '{}/.config'.format(cache_dir)
    if not os.path.exists(daemon_config_file):
        raise Exception('Warning: {} not exists'.format(daemon_config_file))
    with open(daemon_config_file, 'r') as jsonfile:
        data = json.load(jsonfile)
        return data.get('host'), data.get('port')


def stop_server():
    try:
        host, port = get_pyd()
        conn = client.HTTPConnection(host, port)
        conn.request('stop_service', '/')
        conn.close()
    except:  # noqa: E722 pylint: disable=bare-except
        pass


def show_statistics():
    try:
        host, port = get_pyd()
        conn = client.HTTPConnection(host, port)
        conn.request('show_statistics', '/')
        conn.close()
    except:  # noqa: E722 pylint: disable=bare-except
        pass


def manage_cache_contents():
    try:
        host, port = get_pyd()
        conn = client.HTTPConnection(host, port)
        conn.request('cache_manage', '/')
        conn.close()
    except:  # noqa: E722 pylint: disable=bare-except
        pass


def main(args):
    parser = argparse.ArgumentParser()
    parser.add_argument('--root', help='path to pycache root directory')
    parser.add_argument('--port',
                        default=PYCACHE_PORT,
                        help='which port to listen')
    parser.add_argument('--start',
                        action='store_true',
                        help='start daemon process for pycache')
    parser.add_argument('--stop',
                        action='store_true',
                        help='stop pycache daemon process')
    parser.add_argument('--stat',
                        action='store_true',
                        help='report cache statistics')
    parser.add_argument('--manage',
                        action='store_true',
                        help='manage pycache contents')

    options = parser.parse_args(args)
    if options.start:
        start_server(LOCALHOST, int(options.port), options.root)
    if options.stop:
        stop_server()
    if options.stat:
        show_statistics()
    if options.manage:
        manage_cache_contents()


if __name__ == '__main__':
    main(sys.argv[1:])
