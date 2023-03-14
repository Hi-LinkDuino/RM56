#!/bin/bash
# Copyright (c) 2021 bestechnic (Shanghai) Technologies CO., LIMITED.
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

# generate fs
let block_size=$1
let fs_image_size=$2
let page_size=fs_image_size/block_size
let fs_image_size=page_size*block_size
fs_src_path=$3
fs_dst_path=$4
fs_name=$5
mklittlefs_path="$6/tools"

fs_path="${fs_dst_path}/release_bin"

${mklittlefs_path}/mklittlefs -c ${fs_src_path} -d 5 -b ${block_size} -p ${page_size} -s ${fs_image_size} ${fs_path}/${fs_name}.bin