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

bin_name=$1
out_path=$2
bin_path="${out_path}release_bin"
bin_sig_path="${out_path}auto_build_tool/hash_sig"

# gen hash of fs
./gen_hash_of_bin ${bin_path}/${bin_name}.bin
mv ${bin_path}/${bin_name}_hash.bin ${bin_sig_path}/

# todo implment sigature
