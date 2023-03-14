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
from setuptools import setup

from hb.__main__ import VERSION

WORK_PATH = os.path.abspath(os.path.dirname('__file__'))
README_PATH = os.path.join(WORK_PATH, 'README.md')
LONG_DESCRIPTION = open(README_PATH, 'r', encoding='utf-8').read()

setup(
    name='ohos-build',
    version=VERSION,
    author='Huawei',
    author_email='contact@openharmony.io',
    description='OHOS build command line tool',
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    url='https://gitee.com/openharmony/build_lite',
    license='Apache 2.0',
    python_requires='>=3.7',
    packages=['hb'],
    package_dir={'hb': 'hb'},
    package_data={'hb': ['common/config.json']},
    install_requires=[
        'prompt_toolkit==1.0.14', 'kconfiglib>=14.1.0', 'PyYAML', 'requests'
    ],
    entry_points={'console_scripts': [
        'hb=hb.__main__:main',
    ]},
)
