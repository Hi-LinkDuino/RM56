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

import shutil
import os
import hashlib
import json
import http.client as client
from . import build_utils


class Storage():
    def __init__(self):
        pass

    @classmethod
    def retrieve_object(cls, cache_artifact, obj):
        possible_dir_cache_artifact = '{}.directory'.format(cache_artifact)

        if os.path.exists(cache_artifact):
            os.makedirs(os.path.dirname(obj), exist_ok=True)
            shutil.copyfile(cache_artifact, obj)
            os.utime(cache_artifact)
            if pycache_debug_enable:
                print('Retrieve {} from cache'.format(obj))
        elif os.path.exists(possible_dir_cache_artifact):
            # Extract zip archive if it's cache artifact for directory.
            os.makedirs(obj, exist_ok=True)
            build_utils.extract_all(possible_dir_cache_artifact,
                                   obj,
                                   no_clobber=False)
            os.utime(possible_dir_cache_artifact)
            if pycache_debug_enable:
                print('Extract {} from cache'.format(obj))
        else:
            if pycache_debug_enable:
                print('Failed to retrieve {} from cache'.format(obj))
            return 0
        return 1

    @classmethod
    def add_object(cls, cache_artifact, obj):
        cache_dir = os.path.dirname(cache_artifact)
        os.makedirs(cache_dir, exist_ok=True)

        if not os.path.exists(obj):
            return
        # If path is directory, store an zip archive.
        if os.path.isdir(obj):
            dir_cache_artifact = '{}.directory'.format(cache_artifact)
            build_utils.zip_dir(dir_cache_artifact, obj)
            if pycache_debug_enable:
                print("archive {} to {}".format(obj, dir_cache_artifact))
        else:
            shutil.copyfile(obj, cache_artifact)
            if pycache_debug_enable:
                print("copying {} to {}".format(obj, cache_artifact))


class PyCache():
    def __init__(self, cache_dir=None):
        cache_dir = os.environ.get('PYCACHE_DIR')
        if cache_dir:
            self.pycache_dir = cache_dir
        else:
            raise Exception('Error: failed to get PYCACHE_DIR')
        self.storage = Storage()

    def retrieve(self, output_paths, prefix=''):
        for path in output_paths:
            _, cache_artifact = self.descend_directory('{}{}'.format(
                prefix, path))
            result = self.storage.retrieve_object(cache_artifact, path)
            if not result:
                return result

        try:
            self.report_cache_stat('cache_hit')
        except:  # noqa: E722 pylint: disable=bare-except
            pass
        return 1

    def save(self, output_paths, prefix=''):
        for path in output_paths:
            _, cache_artifact = self.descend_directory('{}{}'.format(
                prefix, path))
            self.storage.add_object(cache_artifact, path)

    def report_cache_stat(self, hit_or_miss):
        pyd_server, pyd_port = self.get_pyd()
        conn = client.HTTPConnection(pyd_server, pyd_port)
        conn.request(hit_or_miss, '/')
        conn.close()

    def get_pyd(self):
        daemon_config_file = '{}/.config'.format(self.pycache_dir)
        if not os.path.exists(daemon_config_file):
            raise Exception('Warning: no pycache daemon process exists.')
        with open(daemon_config_file, 'r') as jsonfile:
            data = json.load(jsonfile)
            return data.get('host'), data.get('port')

    @classmethod
    def cache_key(cls, path):
        sha256 = hashlib.sha256()
        sha256.update(path.encode())
        return sha256.hexdigest()

    def descend_directory(self, path):
        digest = self.cache_key(path)
        cache_dir = os.path.join(self.pycache_dir, digest[:2])
        return cache_dir, os.path.join(cache_dir, digest[2:])

    # Manifest file to record inputs/outputs/commands.
    def get_manifest_path(self, path):
        manifest_dir, manifest_file = self.descend_directory(path)
        os.makedirs(manifest_dir, exist_ok=True)
        return manifest_file


pycache_enabled = (os.environ.get('PYCACHE_DIR') is not None)
pycache_debug_enable = int(os.environ.get('PRINT_BUILD_EXPLANATIONS', 0))
if pycache_enabled:
    pycache = PyCache()
else:
    pycache = None  # pylint: disable=invalid-name
