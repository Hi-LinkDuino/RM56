#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2022 Huawei Device Co., Ltd.
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
import subprocess
import shutil

def main(argv):
    project_path = os.path.abspath(argv[1])
    target_out_path = os.path.abspath(argv[2])
    nodejs_path = os.path.abspath(argv[3])
    js2bundle_path = os.path.abspath(argv[4])
    interface_path = os.path.abspath(argv[5])
    parse5_path = os.path.abspath(argv[6])
    weex_loader_path = os.path.abspath(argv[7])

    buildEnv = os.environ
    buildEnv["PATH"] = nodejs_path + ":" + buildEnv["PATH"]

    interface_target_path = os.path.join(target_out_path, "interface")
    if os.path.exists(interface_target_path):
        shutil.rmtree(interface_target_path)
    shutil.copytree(interface_path, interface_target_path, ignore=shutil.ignore_patterns('.git'))

    third_party_target_path = os.path.join(target_out_path, "third_party")
    if os.path.exists(third_party_target_path):
        shutil.rmtree(third_party_target_path)
    os.makedirs(third_party_target_path);

    parse5_target_path = os.path.join(third_party_target_path, "parse5")
    shutil.copytree(parse5_path, parse5_target_path, ignore=shutil.ignore_patterns('.git'))

    weex_loader_target_path = os.path.join(third_party_target_path, "weex-loader")
    shutil.copytree(weex_loader_path, weex_loader_target_path, ignore=shutil.ignore_patterns('.git'))

    js2bundle_target_path = os.path.join(target_out_path, "developtools")
    if os.path.exists(js2bundle_target_path):
        shutil.rmtree(js2bundle_target_path)
    os.makedirs(js2bundle_target_path);
    js2bundle_target_path = os.path.join(js2bundle_target_path, "ace-js2bundle")
    shutil.copytree(js2bundle_path, js2bundle_target_path, True, ignore=shutil.ignore_patterns('.git'))
    js2bundle_target_path = os.path.join(js2bundle_target_path, "ace-loader")
    os.chdir(js2bundle_target_path)

    js2bundle_node_modules_target_path = os.path.join(js2bundle_target_path, "node_modules")
    if not os.path.exists(js2bundle_node_modules_target_path):
        subprocess.call(["npm", "install"], env=buildEnv)

    target_i18n_path = os.path.join(js2bundle_target_path, "sample", "rich", "i18n")
    if os.path.exists(target_i18n_path):
        shutil.rmtree(target_i18n_path)
    source_i18n_path = os.path.join(project_path, "i18n")
    if os.path.exists(source_i18n_path):
        shutil.copytree(source_i18n_path, target_i18n_path, ignore=shutil.ignore_patterns('.git'))

    target_common_path = os.path.join(js2bundle_target_path, "sample", "rich", "common")
    if os.path.exists(target_common_path):
        shutil.rmtree(target_common_path)
    source_common_path = os.path.join(project_path, "common")
    if os.path.exists(source_common_path):
        shutil.copytree(source_common_path, target_common_path, ignore=shutil.ignore_patterns('.git'))

    target_index_path = os.path.join(js2bundle_target_path, "sample", "rich", "pages", "index")
    shutil.copyfile(
        os.path.join(project_path, "pages", "index", "index.css"), os.path.join(target_index_path, "index.css"))
    shutil.copyfile(
        os.path.join(project_path, "pages", "index", "index.hml"), os.path.join(target_index_path, "index.hml"))
    shutil.copyfile(
        os.path.join(project_path, "pages", "index", "index.js"), os.path.join(target_index_path, "index.js"))

    subprocess.call(["npm", "run", "build"], env=buildEnv)
    subprocess.call(["npm", "run", "rich"], env=buildEnv)

    target_build_path = os.path.join(js2bundle_target_path, "sample", "rich", "build")
    target_build_dest_path = os.path.join(target_out_path, "default")
    if os.path.exists(target_build_dest_path):
        shutil.rmtree(target_build_dest_path)
    shutil.copytree(target_build_path, target_build_dest_path)

if __name__ == '__main__':
    main(sys.argv)
