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

gui_path=$1
pack_burn=$2
product_name=$3
best2003_type=$4

if [ "x${pack_burn}" == "xtrue" ]; then
cd $gui_path
time=$(date "+%Y-%m-%d-%H-%M-%S")
tar -zcvf write_flash_gui-$best2003_type-$product_name-$time.tar.gz write_flash_gui/
cd -
fi