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

build_trustzone=$1
build_mini_sys=$2
build_bin_type=$3
product_path=$4
flash_size=$5

BUILD_PIECE="false"
flash_config=""
rel_flash_config=""
MAKEFILE="./bsp/platform/main/Makefile"
if [ ! -f $MAKEFILE ]; then
    echo -e "${MAKEFILE} not exist!"
    BUILD_PIECE="true"
    rel_flash_config=" CP_BIN_NAME=cmcp_${flash_size}  "
fi


if [ "x${flash_size}" == "x32" ]; then
    flash_config=" PSRAM_XCCELA_MODE=1 FLASH_SIZE=0x2000000 "
fi

if [ "x$build_trustzone" == "x" ];then
    build_trustzone="false"
fi

rel=" "
rel_branch=" "
rel_filter=" "
build_type="BUILD_TYPE=DEBUG"
if [ "x$build_bin_type" == "xrelease" ];then
    build_type=`echo $build_bin_type | tr '[a-z]' '[A-Z]'`
    build_type="BUILD_TYPE=$build_type"
    rel="-r"
    rel_branch=" branch=soc "
    rel_filter=" FORCE_TO_FILTER_MODULE=1 "
fi

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
for VENDOR_OPT in ${OPT_LITEOS_MAIN}
do
    pre_handle_opt ${VENDOR_OPT} OPT_SOC_MAIN
    temp_basic_opts="${temp_cmd}"
    echo "temp_basic_opts:${temp_basic_opts}"
    temp_extra_opts+=" ${temp_extra_cmd} "
done
echo "temp_extra_opts:${temp_extra_opts}"

OPT_LITEOS_MAIN=" ${temp_basic_opts} ${temp_extra_opts}"

if [ "x${build_trustzone}" == "xtrue" ]; then
    if [ "x${build_mini_sys}" == "xtrue" ]; then
        pre_handle_opt ARM_CMNS=1 OPT_LITEOS_MINI
        OPT_LITEOS_MINI="${temp_cmd} ${temp_extra_cmd}"
        pre_handle_opt TZ_MINI_OFFSET=0x56000 OPT_LITEOS_BOOT2A
        OPT_LITEOS_BOOT2A="${temp_cmd} ${temp_extra_cmd} "
    fi
    pre_handle_opt ARM_CMNS=1 OPT_LITEOS_MAIN
    OPT_LITEOS_MAIN="${temp_cmd} ${temp_extra_cmd} "
    pre_handle_opt TZ_MAIN_OFFSET=0x30000 OPT_LITEOS_BOOT2A
    OPT_LITEOS_BOOT2A="${temp_cmd} ${temp_extra_cmd} "
else
    if [ "x${build_mini_sys}" == "xtrue" ]; then
        pre_handle_opt ARM_CMNS=0 OPT_LITEOS_MINI
        OPT_LITEOS_MINI="${temp_cmd} ${temp_extra_cmd} "
        pre_handle_opt TZ_MINI_OFFSET=0xB00000 OPT_LITEOS_BOOT2A
        OPT_LITEOS_BOOT2A="${temp_cmd} ${temp_extra_cmd} "
    fi
    pre_handle_opt ARM_CMNS=0 OPT_LITEOS_MAIN
    OPT_LITEOS_MAIN="${temp_cmd} ${temp_extra_cmd} "
    pre_handle_opt TZ_MAIN_OFFSET=0x80000 OPT_LITEOS_BOOT2A
    OPT_LITEOS_BOOT2A="${temp_cmd} ${temp_extra_cmd} "
fi


cd bsp

out_dir="out/a7_dsp/"
romfs_orig="${out_dir}romfs.bin"
romfs_orig_keep="${out_dir}romfs.bin.orig"

echo "romfs_orig:${romfs_orig}"
dec2hex(){
  printf "0x%x" $1
}

FS_SIZE=0
if [ -f ${romfs_orig_keep} ];then
  FS_SIZE=`du -b $romfs_orig_keep | awk '{print $1}'`
  FS_SIZE=$(dec2hex ${FS_SIZE})
  echo "FS_SIZE:${FS_SIZE}"
fi
tools/lzma e $romfs_orig "$romfs_orig.lzma"
mv "$romfs_orig.lzma" $romfs_orig
cp -rf ../../../../../../../out/v200zra/liteos-a_demo/unstripped/bin/a7_dsp.bin out/a7_dsp/

chmod a+x tools/build_best2600w_cortex-m_into_lib.sh

tools/build_best2600w_cortex-m_into_lib.sh \
-a="$OPT_LITEOS_A7 $build_type" \
-m="$OPT_LITEOS_MAIN PSRAMFS_SIZE=$FS_SIZE MCU_SLAVE_FS_EN=1 FS_BIN_NAME=romfs NO_LIBC=1 BOARD_OS_WRAP_MALLOC=1 $rel_filter $rel_flash_config $flash_config NO_LIBC=1 MODULE_KERNEL_STUB_INC=1 EXTERN_ROOT_PATH=./../../../../../../../ GEN_SDK_LIB_NET=1 $build_type" \
-c="$OPT_LITEOS_CP PSRAMFS_SIZE=$FS_SIZE MCU_SLAVE_FS_EN=1 FS_BIN_NAME=romfs $build_type $flash_config " \
-s="$OPT_LITEOS_MAIN_MINI_SE $build_type" \
-l="$OPT_LITEOS_MINI NO_LIBC=1 BOARD_OS_WRAP_MALLOC=1 $rel_flash_config $flash_config MODULE_KERNEL_STUB_INC=1 EXTERN_ROOT_PATH=./../../../../../../../ $build_type" \
-x="$OPT_LITEOS_BOOT1 $build_type $flash_config " \
-y="$OPT_LITEOS_BOOT2A $build_type $flash_config " \
-d=" BUILD_SE=$build_trustzone BUILD_MINI=$build_mini_sys $rel_branch BUILD_PIECE=$BUILD_PIECE" \
GEN_LIB=1 $rel

cd ../
