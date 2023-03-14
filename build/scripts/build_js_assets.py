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

import optparse
import os
import sys
import json

from zipfile import ZipFile  # noqa: E402
from util import build_utils  # noqa: E402


def parse_args(args):
    args = build_utils.expand_file_args(args)

    parser = optparse.OptionParser()
    build_utils.add_depfile_option(parser)
    parser.add_option('--output', help='stamp file')
    parser.add_option('--js-assets-dir', help='js assets directory')
    parser.add_option('--nodejs-path', help='path to nodejs app')
    parser.add_option('--webpack-js', help='path to webpack.js')
    parser.add_option('--webpack-config-js', help='path to webpack.config.js')
    parser.add_option('--hap-profile', help='path to hap profile')
    parser.add_option('--build-mode', help='debug mode or release mode')
    parser.add_option('--js-sources-file', help='path to js sources file')
    parser.add_option('--js2abc',
                      action='store_true',
                      default=False,
                      help='whether to transform js to ark bytecode')
    parser.add_option('--ets2abc',
                      action='store_true',
                      default=False,
                      help='whether to transform ets to ark bytecode')
    parser.add_option('--ark-frontend-dir', help='path to ark frontend dir')
    parser.add_option('--ace-loader-home', help='path to ace-loader dir.')
    parser.add_option('--app-profile', default=False, help='path to app-profile.')

    options, _ = parser.parse_args(args)
    options.js_assets_dir = build_utils.parse_gn_list(options.js_assets_dir)
    return options

def make_my_env(build_dir, options):
    out_dir = os.path.abspath(os.path.dirname(options.output))
    assets_dir = os.path.join(out_dir, "assets")
    gen_dir = os.path.join(out_dir, "gen")
    my_env = {
        "aceModuleBuild": assets_dir,
        "buildMode": options.build_mode,
        "PATH": os.environ.get('PATH')
     }
    if options.app_profile:
        my_env["aceProfilePath"] = os.path.join(gen_dir, "resources/base/profile")
        my_env["appResource"] = os.path.join(gen_dir, "ResourceTable.txt")
        my_env["aceModuleJsonPath"] = os.path.abspath(options.hap_profile)
    else:
        manifest = os.path.join(build_dir, 'manifest.json')
        my_env["aceManifestPath"] = manifest
    return my_env

def make_manifest_data(config, options):
    data = dict()
    data['appID'] = config['app']['bundleName']
    data['appName'] = config['module']['abilities'][0]['label']
    if options.app_profile:
        data['versionName'] = config['app']['versionName']
        data['versionCode'] = config['app']['versionCode']
        data['pages'] = config['module']['pages']
        data['deviceType'] = config['module']['deviceTypes']
    else:
        data['versionName'] = config['app']['version']['name']
        data['versionCode'] = config['app']['version']['code']
        data['pages'] = config['module']['js'][0]['pages']
        data['deviceType'] = config['module']['deviceType']
        data['window'] = config['module']['js'][0]['window']
    if options.ets2abc:
        if not options.app_profile:
            data['mode'] = config['module']['js'][0]['mode']
    return data

def build_ace(cmd, options):
    js_assets_dir = os.path.relpath(
        options.js_assets_dir[0], options.ace_loader_home)
    if options.js_sources_file:
        with open(options.js_sources_file, 'wb') as js_sources_file:
            sources = get_all_js_sources(js_assets_dir)
            js_sources_file.write('\n'.join(sources).encode())
    with build_utils.temp_dir() as build_dir:
        my_env = make_my_env(build_dir, options)
        my_env["aceModuleRoot"] = js_assets_dir
        if options.js2abc:
            my_env.update({"cachePath": os.path.join(build_dir, ".cache")})
        src_path = 'ets'
        if not options.app_profile:
            src_path = 'default'
        manifest = os.path.join(build_dir, 'manifest.json')
        if not os.path.exists(manifest) and options.hap_profile:
            with open(options.hap_profile) as profile:
                config = json.load(profile)
                data = make_manifest_data(config, options)
                if options.ets2abc:
                    if 'srcPath' in config['module']['abilities'][0]:
                        src_path = config['module']['abilities'][0]['srcPath']
                build_utils.write_json(data, manifest)
        build_utils.check_output(
            cmd, cwd=options.ace_loader_home, env=my_env)
        gen_dir = my_env.get("aceModuleBuild")
        for root, _, files in os.walk(gen_dir):
            for file in files:
                filename = os.path.join(root, file)
                if filename.endswith('.js.map'):
                    os.unlink(filename)
        if options.app_profile:
            build_utils.zip_dir(options.output,
                                gen_dir,
                                zip_prefix_path=src_path)
        else:
            build_utils.zip_dir(options.output,
                                gen_dir,
                                zip_prefix_path='assets/js/{}/'.format(src_path))



def get_all_js_sources(base):
    sources = []
    for root, _, files in os.walk(base):
        for file in files:
            if file[-3:] in ('.js', '.ts'):
                sources.append(os.path.join(root, file))

    return sources


def main(args):
    options = parse_args(args)

    inputs = [
        options.nodejs_path, options.webpack_js, options.webpack_config_js
    ]
    depfiles = []
    if options.js_assets_dir:
        depfiles.extend(build_utils.get_all_files(options.js_assets_dir[0]))
    else:
        with ZipFile(options.output, 'w') as file:
            return

    if options.ark_frontend_dir:
        depfiles.extend(build_utils.get_all_files(options.ark_frontend_dir))

    depfiles.append(options.webpack_js)
    depfiles.append(options.webpack_config_js)
    depfiles.extend(build_utils.get_all_files(options.ace_loader_home))

    node_js = os.path.relpath(options.nodejs_path, options.ace_loader_home)
    cmd = [
        node_js,
        os.path.relpath(
            options.webpack_js, options.ace_loader_home),
        '--config',
        os.path.relpath(
            options.webpack_config_js, options.ace_loader_home)
    ]
    if options.js2abc or options.ets2abc:
        ark_frontend_dir = os.path.relpath(
            options.ark_frontend_dir, options.ace_loader_home)
        if options.app_profile:
            cmd.extend(['--env', 'buildMode=release', 'compilerType=ark',
                        'arkFrontendDir={}'.format(ark_frontend_dir), 'nodeJs={}'.format(node_js)])
        else:
            cmd.extend(['--env', 'compilerType=ark',
                        'arkFrontendDir={}'.format(ark_frontend_dir), 'nodeJs={}'.format(node_js)])
    build_utils.call_and_write_depfile_if_stale(
        lambda: build_ace(cmd, options),
        options,
        depfile_deps=depfiles,
        input_paths=depfiles + inputs,
        input_strings=cmd + [options.build_mode],
        output_paths=([options.output]),
        force=False,
        add_pydeps=False)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
