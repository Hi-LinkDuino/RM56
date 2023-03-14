#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Copyright (c) 2021 Huawei Device Co., Ltd.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at


  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import json
import sys


def filter_modules(subsystem_data_file, test_packages):
    subsystems = set()
    with open(subsystem_data_file) as fd:
        subsystem_data = json.load(fd)
        subsystems = set(subsystem_data)
    subsystems.add("demo")
    subsystems.add("storage")
    selected_packages = []
    for dep in test_packages:
        if 'selected_subsystem=' in dep:
            selected_subsystem = dep.split('=')[1]
            init_subsystems = subsystems.copy()
            subsystems.clear()
            for item in selected_subsystem.split(','):
                if item in init_subsystems:
                    subsystems.add(item)
            break
    for dep in test_packages:
        rslash_index = dep.find('/hits')
        if rslash_index < 0:
            rslash_index = dep.find('/acts')
        if rslash_index < 0:
            rslash_index = dep.find('/hats')
        if rslash_index < 0:
            rslash_index = dep.find('/dcts')
        colon_index = dep.find(':')
        if rslash_index < 0 or colon_index < 0:
            continue
        rslash_index += 5
        subsystem_name = dep[rslash_index + 1:colon_index]
        if subsystem_name in subsystems:
            selected_packages.append(dep)
        else:
            pass
    return selected_packages


if __name__ == "__main__":
    main_subsystem_data_file = sys.argv[1]
    main_test_packages = sys.argv[2:]
    main_selected_packages = filter_modules(main_subsystem_data_file,
                                            main_test_packages)
    sys.stdout.write('\n'.join(main_selected_packages))
