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

from setuptools import setup

INSTALL_REQUIRES = [
]


def main():
    setup(name='xdevice-extension',
          description='xdevice extension test framework',
          url='',
          package_dir={'': 'src'},
          packages=['xdevice_extension',
                    'xdevice_extension._core',
                    'xdevice_extension._core.driver',
                    'xdevice_extension._core.environment',
                    'xdevice_extension._core.executor',
                    'xdevice_extension._core.testkit'
                    ],
          package_data={
          },
          entry_points={
              'driver': [
                  'drivers=xdevice_extension._core.driver.drivers',
                  'kunpeng=xdevice_extension._core.driver.kunpeng'
              ],
              'parser': [
                  'parser=xdevice_extension._core.driver.parser'
              ],
              'device': [
                  'default=xdevice_extension._core.environment.device'
              ],
              'manager': [
                  'device=xdevice_extension._core.environment.manager_device'
              ],
              'listener': [
                  'listener=xdevice_extension._core.executor.listener'
              ],
              'testkit': [
                  'kit=xdevice_extension._core.testkit.kit'
              ]
          },
          zip_safe=False,
          install_requires=INSTALL_REQUIRES,
          )


if __name__ == "__main__":
    main()
