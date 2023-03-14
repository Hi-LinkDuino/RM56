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

from hb_internal.common.utils import OHOSException
from hb_internal.common.utils import exec_command
from hb_internal.common.utils import read_yaml_file
from hb_internal.common.config import Config


class Patch():
    """Patch class for hb_internal.build --patch parameter
    Install the patch based on the configuration file.
    """
    def __init__(self):
        self.config = Config()
        # Patch configuration file path
        self.patch_cfg = os.path.join(self.config.product_path, 'patch.yml')

    def patch_make(self, cmd_args=None, reverse=False):
        """Patch installation entry function

        Args:
            reverse (bool, optional): True if the patch is rolled back,
                                      else False. Defaults to False.

        Raises:
            OHOSException: if configuration file does not exist.
        """
        patch_cfg = self.config.patch_cache
        # Check whether the rollback is in progress
        if not reverse:
            # Try to update patch cache and check whether rollback is required
            self.patch_cache_update()
            patch_cfg = self.patch_cfg

        if not os.path.isfile(patch_cfg):
            raise OHOSException(f'{patch_cfg} not found, '
                                'which "--patch" parameter needs')

        patch_cfg_dict = read_yaml_file(patch_cfg)
        for src_path, patch_list in patch_cfg_dict.items():
            self.patch_apply(src_path, patch_list, reverse)

    def patch_apply(self, src_path, patch_list, reverse=False):
        """Run the patch installation command.

        Args:
            src_path (string): Path to which the patch needs to be installed
            patch_list (list): List of paths for storing patches
            reverse (bool, optional): True if the patch is rolled back,
                                      else False. Defaults to False.

        Raises:
            OHOSException: src_path or patch_path is invalid,
                           or the command fails to be executed.
        """
        src_path = os.path.join(self.config.root_path, src_path)
        if not os.path.exists(src_path):
            raise OHOSException(f'{src_path} not exist, stop applying patch')

        cmd = ''
        for patch in patch_list:
            patch_path = os.path.join(self.config.root_path, patch)
            if not os.path.isfile(patch_path):
                patch_path = os.path.join(self.config.root_path,
                                          src_path, patch)
                if not os.path.isfile(patch_path):
                    raise OHOSException(f'patch {patch} not exist for '
                                        f'{src_path}, stop applying patch')
            if reverse:
                cmd = f'patch -p1 -R < {patch_path}'
            else:
                cmd = f'patch -p1 < {patch_path}'
            try:
                exec_command(cmd, log_path=self.config.log_path,
                             cwd=src_path, shell=True)
            except OHOSException as ohos_exeception:
                # Failed to roll back the patch, clear patch cache
                self.config.patch_cache = None
                raise ohos_exeception

    def patch_cache_update(self):
        """Try to update patch cache and Check whether rollback is required
        If patch_cache is None, no rollback is required,
        otherwise roll back patch.
        """
        # patch_cache stores the configuration file of the last patch.
        if self.config.patch_cache is not None:
            self.patch_make(reverse=True)
        # Update patch cache to current path configuration file path
        self.config.patch_cache = self.patch_cfg


if __name__ == "__main__":
    patch = Patch()
    patch.patch_make()
