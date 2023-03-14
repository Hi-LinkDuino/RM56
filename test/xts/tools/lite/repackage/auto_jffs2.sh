#!/bin/bash
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

set -e
current_user_name=$(whoami)
if [ "$current_user_name" != "root" ]
then
    echo "请使用root用户执行该脚本,程序退出"
    exit 1
fi

# 检查输入参数数量
if [ $# -lt 3 ]
then
    echo "输入参数不足3个,程序退出!"
    exit 1
fi

set +e
mtd_str=$(dpkg -l|grep mtd-utils)
set -e

if [ "$mtd_str" ]
then
    echo "已经安装了mtd-utils"
else
    echo "开始安装mtd-utils"
    apt-get install mtd-utils
    echo "mtd-utils安装成功"
fi

mtd_device="/dev/mtdblock0"
if [ ! -b "$mtd_device" ]
then
    # 安装并初始化mtd模块
    echo "安装并初始化mtd模块"
    modprobe mtdblock
    modprobe mtdram total_size=102400
    chmod 666 /dev/mtdblock0
    echo "mtd模块操作完成"
else
    echo "mtd模块已安装"
fi
echo "环境部署完成"

old_img_path=$1
# 检查文件是否存在
if [ ! -e "$old_img_path" ]
then
    echo "Line:$LINENO,ERROR:$old_img_path can not be found, exit!"
    exit 1
fi

# 检查输入数据目录
input_dir=$2
if [ ! -d "$input_dir" ]
then
    echo "Line:$LINENO,ERROR:intput_dir $input_dir can not be found, exit!"
    exit 1
fi

# 检查输出目录
output_dir=$3
output_filepath="$output_dir/new_rootfs.img"
if [ ! -d "$output_dir" ]
then
    echo "Line:$LINENO,INFO:output_dir $output_dir not exists and it will be created!"
    mkdir -p $output_dir
fi

# 检查挂在目录
mount_dir="$HOME/mnt"
if [ ! -d "$mount_dir" ]
then
    echo "Line:$LINENO,WARN:$mount_dir not exists and it will be created!"
    mkdir -p $mount_dir
fi

set +e
mount_flag=$(df|grep "$mount_dir")
set -e
# 将块设备挂载到本地目录,并赋予相关权限
if [ "$mount_flag" ]
then
    echo "$mount_dir 已挂在其他设备,卸载该目录"
    umount $mount_dir
    # 将镜像文件写入块设备 
    dd if=$old_img_path of=$mtd_device
    mount -t jffs2 $mtd_device $mount_dir
    chmod -R 755 $mount_dir
    echo "$mtd_device 挂在到本地成功"
else
    dd if=$old_img_path of=$mtd_device
    mount -t jffs2 $mtd_device $mount_dir
    chmod -R 755 $mount_dir
    echo "$mtd_device 挂在到本地成功"
fi

if [ ! -d "$mount_dir/bin" ]
then
    echo "Line:$LINENO,INFO:$mount_dir/bin not exists and it will be created!"
    mkdir -p $mount_dir/bin
fi
# 遍历目录拷贝数据文件
for file_name in $(ls $input_dir)
do
    file_path=$input_dir"/"$file_name
    if [ -f "$file_path" ]
    then
        \cp $file_path $mount_dir/bin
        echo "文件$file_name 拷贝成功"
    fi
done

# 制作jffs2镜像文件
/usr/sbin/mkfs.jffs2 -q -d $mount_dir -o $output_filepath
chmod 755 $output_filepath
echo "新镜像 $output_filepath 制作成功"
# 卸载块设备
umount $mount_dir
echo "$mount_dir 卸载成功"
modprobe -r mtdram
modprobe -r mtdblock
echo "mtd模块卸载成功"
echo "Normal completion,OK!"
exit 0
