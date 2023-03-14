#!/usr/bin/env python3
# coding=utf-8

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


def _init_global_config():
    import sys
    import os

    # insert src path for loading xdevice modules
    sys.framework_src_dir = os.path.abspath(os.path.dirname(
        os.path.dirname(__file__)))
    sys.path.insert(0, sys.framework_src_dir)
    sys.framework_root_dir = os.path.abspath(os.path.dirname(
        os.path.dirname(os.path.dirname(__file__))))

    sys.xdevice_dir = os.path.abspath(os.path.join(
        sys.framework_root_dir,
        "..",
        "xdevice",
        "src"))
    sys.path.insert(0, sys.xdevice_dir)

    sys.xdevice_extension_dir = os.path.abspath(os.path.join(
        sys.framework_root_dir,
        "..",
        "xdevice",
        "extension",
        "src"))
    sys.path.insert(1, sys.xdevice_extension_dir)

    sys.pytest_dir = os.path.abspath(os.path.join(
        sys.framework_root_dir,
        "aw",
        "python"))
    sys.path.insert(2, sys.pytest_dir)

    sys.adapter_dir = os.path.abspath(os.path.join(
        sys.framework_root_dir,
        "adapter"
        "aw",
        "python"))
    sys.path.insert(3, sys.adapter_dir)

    sys.hmh_script = os.path.abspath(os.path.join(
        sys.framework_root_dir,
        "libs"))
    sys.path.insert(4, sys.hmh_script)

    sys.framework_res_dir = sys.framework_root_dir
    sys.exec_dir = sys.framework_root_dir

    from core.common import get_source_code_root_path
    sys.source_code_root_path = get_source_code_root_path(
        sys.framework_root_dir)

    from xdevice import Variables
    Variables.exec_dir = sys.framework_root_dir


def _iter_module_plugins(packages):
    import importlib
    import pkgutil
    for package in packages:
        pkg_path = getattr(package, "__path__", "")
        pkg_name = getattr(package, "__name__", "")
        if not pkg_name or not pkg_path:
            continue
        _iter_modules = pkgutil.iter_modules(pkg_path, "%s%s" % (
            pkg_name, "."))
        for _, name, _ in _iter_modules:
            importlib.import_module(name)


def _load_internal_plugins():
    import core.driver
    import benchmark.report.benchmark_reporter
    _iter_module_plugins([core.driver, benchmark.report.benchmark_reporter])

    try:
        import xdevice_extension._core.environment
        _iter_module_plugins([xdevice_extension._core.environment])
        import xdevice_extension._core.driver
        _iter_module_plugins([xdevice_extension._core.driver])
    except (ModuleNotFoundError, ImportError):
        pass

    try:
        import script.report
        _iter_module_plugins([script.report])
    except (ModuleNotFoundError, ImportError):
        pass

_init_global_config()
del _init_global_config

_load_internal_plugins()
del _load_internal_plugins
