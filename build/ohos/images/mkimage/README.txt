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

# 主要介绍拍包脚本使用的参数以及相关格式，文件行首字符为'#'为注释，拍包脚本扫描时将自动忽略
# 拍包时调用脚本应该为mkimages.py 传递四个参数，即拍包使用的源文件路径，拍包时使用的参数的文件，拍包的目录镜像路径，以及拍包输出的out目录
# 此文档时介绍拍包时使用参数文件的配置方法

# 第一行固定为挂载路径，必选参数顺序要保持一直
/vendor
# 第二行固定为镜像制作大小
268434944
# 镜像文件系统类型，不可缺少,当前仅支持ext4，f2fs将在后续支持
--fs_type=ext4
# 后面跟着可选参数

# ext4 文件系统类型配置参数
# mke2fs 参数
# inode size 大小，不配置默认为256
--inode_size 256
# journal_size用来配置journal参数，不配置默认不开启journal（data分区例外）
--journal_size xxx
# 镜像预留空间比例，不配置默认0（data分区例外，使用mke2fs默认预留比例）
--reserve_percent xxx
# extend_opts 参数，后面可以跟多个参数例如discard
--extend_opts discard

# e2fsdroid 参数
# dac_config 为配置路径DAC使用
--dac_config path

# f2fs 文件系统类型配置参数
# make_f2fs 配置参数
# label标签，不设置默认使用mountpoint为lable
--label xxx
# prjquota 使能开关
--prjquota
# casefold 使能开关
--casefold

#sload_f2fs 配置参数
# dac_config 为配置路径DAC使用
--dac_config path
# timestamp, 默认不使用
--timestamp xxx
