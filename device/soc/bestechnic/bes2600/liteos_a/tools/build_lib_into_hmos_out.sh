#!/bin/bash
# Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

# check if build sdk into lib

romfs_bin_path=$1
romfs_bin_name=$2

romfs_orig="${romfs_bin_path}/${romfs_bin_name}.bin"
echo "romfs_orig:${romfs_orig}"

dec2hex(){
  printf "0x%x" $1
}

FS_SIZE=0
if [ -f ${romfs_orig} ];then
  FS_SIZE=`du -b $romfs_orig | awk '{print $1}'`
  FS_SIZE=$(dec2hex ${FS_SIZE})
  echo "FS_SIZE:${FS_SIZE}"
fi

BUILD_PIECE="false"
flash_config=""
rel_flash_config=""

#pre-handle options of target
SPACE=" "
function pre_handle_opt()
{
    eval cmd_opt="\${$2}"
#    cmd=`echo $cmd_opt`
   # printf "\n[$1] $cmd\n"
    filter_para=$1
    filter=${filter_para%%=*}
    # printf "\nfilter:$filter\n"
    flag_match=0
    temp_cmd=""
    for input_opt in ${cmd_opt}
    do
        # parse key value by = 
        input_opt_key=${input_opt%%=*}
        # cover same parameter by check key
        if [ "x${input_opt_key}" = "x${filter}" ] ;then
            printf "[MOD][$2]${input_opt} covered by ${filter_para}\n"
            temp_cmd+="$filter_para$SPACE"
            flag_match=1
            continue
         fi

         # keep current parameter not matched
         temp_cmd+="$input_opt$SPACE"
    done
    # handle no matched parameters
    if [ "x${flag_match}" = "x0" ] ;then
        printf "[EXTRA CMD] ${filter_para}\n"
        temp_extra_cmd="$filter_para$SPACE"
    fi
}

source ./tools/config.sh
if [ -f ${product_path}/config.sh ]; then
    source ${product_path}/config.sh
fi

temp_basic_opts=""
temp_extra_opts=""
for VENDOR_OPT in ${OPT_LITEOS_A7}
do
    pre_handle_opt ${VENDOR_OPT} OPT_SOC_A7
    temp_basic_opts="${temp_cmd}"
    echo "temp_basic_opts:${temp_basic_opts}"
    temp_extra_opts+=" ${temp_extra_cmd} "
done
echo "temp_extra_opts:${temp_extra_opts}"

OPT_LITEOS_A7=" ${temp_basic_opts} ${temp_extra_opts}"

cd ../liteos_m/sdk/bsp

chmod a+x tools/build_best2600w_cortex-a_into_lib.sh

tools/build_best2600w_cortex-a_into_lib.sh \
-a="$OPT_LITEOS_A7 CONFIG_MCU_PSRAMUHS_SIZE=0 PSRAM_XCCELA_MODE=1 A7_DISPLAY=1 A7_UART=2 PSRAMFS_SIZE=$FS_SIZE MCU_SLAVE_FS_EN=1 FS_BIN_NAME=romfs $build_type OS_HEAP_SIZE=0x200000 MODULE_KERNEL_STUB=1  ALLOW_WARNING=1 " \
-d=" $rel_branch BUILD_PIECE=$BUILD_PIECE " \
GEN_LIB=1 $rel

out_dir="out/a7_dsp/"
if [ -f "${out_dir}/liba7_dsp.a" ];then
  rm -rf "${out_dir}/romfs.bin"
  rm -rf "${out_dir}/romfs.bin.orig"
  cp -rf $romfs_orig "${out_dir}/romfs.bin"
  cp -rf $romfs_orig "${out_dir}/romfs.bin.orig"
fi
cd -


