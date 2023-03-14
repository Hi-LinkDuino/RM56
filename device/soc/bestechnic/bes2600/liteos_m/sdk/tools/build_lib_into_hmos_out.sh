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
best2003_type=$5
romfs_enable=$6

BUILD_PIECE="false"
flash_size=""
flash_config=""
rel_flash_config=""
romfs_config=""
bootinfo_config=""

# flash_size and flash_config depend on 2003 chip type: AX4D or AX4F or GX6D
if [ "x${best2003_type}" == "xAX4D" ]; then
    flash_size="16"
    flash_config=" PSRAM_XCCELA_MODE=0 PSRAMUHS_AP325608A=0 PSRAMULS_MODE=0 RF_TX_CONTROL_IO=16 FLASH_SIZE=0x1000000 "
    boot1_config=" REMAP_SUPPORT=1 BOOTINFO_OFFSET=0xFD0000 "
    boot2a_config=" REMAP_SUPPORT=1 RTOS_A_CODE_OFFSET=0x80000 RTOS_B_CODE_OFFSET=0x680000 BOOTINFO_OFFSET=0xFD0000 "
elif [ "x${best2003_type}" == "xAX4F" ]; then
    flash_size="16"
    flash_config=" ALT_BOOT_FLASH=1 PSRAMUHS_AP325608A=1 RF_TX_CONTROL_IO=100 FLASH_SIZE=0x1000000 "
    boot1_config=" REMAP_SUPPORT=1 BOOTINFO_OFFSET=0xFD0000 "
    boot2a_config=" REMAP_SUPPORT=1 RTOS_A_CODE_OFFSET=0x80000 RTOS_B_CODE_OFFSET=0x680000 BOOTINFO_OFFSET=0xFD0000 "
elif [ "x${best2003_type}" == "xGX6D" ]; then
    flash_size="32"
    flash_config=" PSRAM_XCCELA_MODE=1 RF_TX_CONTROL_IO=100 FLASH_SIZE=0x2000000 "
    boot1_config=" REMAP_SUPPORT=1 BOOTINFO_OFFSET=0x1FC0000 "
    boot2a_config=" REMAP_SUPPORT=1 RTOS_A_CODE_OFFSET=0x80000 RTOS_B_CODE_OFFSET=0x680000 BOOTINFO_OFFSET=0x1FC0000 "
fi

MAKEFILE="./bsp/platform/main/Makefile"
if [ ! -f $MAKEFILE ]; then
    echo -e "${MAKEFILE} not exist!"
    BUILD_PIECE="true"
    rel_flash_config=" CP_BIN_NAME=cmcp_${flash_size}  "
fi

if [ "x${romfs_enable}" == "xtrue" ]; then
    romfs_config=" PSRAMFS_SIZE=0x800000 MCU_SLAVE_FS_EN=1 "
    FILEFS_PATH="./bsp/out/a7_dsp/littlefs.bin"
    if [ ! -f $FILEFS_PATH ]; then
        cp ../../../write_flash_gui/ota_bin/ramfsdefault.bin ./bsp/lib/littlefs.bin
    fi
else
    romfs_config=""
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
[ -d out ] || mkdir out
chmod a+x tools/build_best2600w_ohos_release.sh

echo build_type=$build_type > out/mkbsp.log
echo romfs_config=$romfs_config >> out/mkbsp.log
echo rel_filter=$rel_filter >> out/mkbsp.log
echo rel_flash_config=$rel_flash_config >> out/mkbsp.log
echo flash_config=$flash_config >> out/mkbsp.log
echo build_trustzone=$build_trustzone >> out/mkbsp.log
echo build_mini_sys=$build_mini_sys >> out/mkbsp.log
echo BUILD_PIECE=$BUILD_PIECE >> out/mkbsp.log
echo rel=$rel >> out/mkbsp.log
echo OPT_LITEOS_A7=$OPT_LITEOS_A7 >> out/mkbsp.log
echo OPT_LITEOS_MAIN=$OPT_LITEOS_MAIN >> out/mkbsp.log
echo OPT_LITEOS_CP=$OPT_LITEOS_CP >> out/mkbsp.log
echo OPT_LITEOS_MAIN_MINI_SE=$OPT_LITEOS_MAIN_MINI_SE >> out/mkbsp.log
echo OPT_LITEOS_MINI=$OPT_LITEOS_MINI >> out/mkbsp.log
echo OPT_LITEOS_BOOT1=$OPT_LITEOS_BOOT1 >> out/mkbsp.log
echo OPT_LITEOS_BOOT2A=$OPT_LITEOS_BOOT2A >> out/mkbsp.log

tools/build_best2600w_ohos_release.sh \
-a="$OPT_LITEOS_A7 $build_type $romfs_config $flash_config " \
-m="$OPT_LITEOS_MAIN NO_LIBC=1 BOARD_OS_WRAP_MALLOC=1 $rel_filter $rel_flash_config $flash_config $romfs_config NO_LIBC=1 MODULE_KERNEL_STUB_INC=1 EXTERN_ROOT_PATH=./../../../../../../../ $build_type" \
-c="$OPT_LITEOS_CP $build_type $flash_config $romfs_config" \
-s="$OPT_LITEOS_MAIN_MINI_SE $build_type" \
-l="$OPT_LITEOS_MINI NO_LIBC=1 BOARD_OS_WRAP_MALLOC=1 $rel_flash_config $flash_config MODULE_KERNEL_STUB_INC=1 EXTERN_ROOT_PATH=./../../../../../../../ $build_type" \
-x="$OPT_LITEOS_BOOT1 $build_type $flash_config $boot1_config" \
-y="$OPT_LITEOS_BOOT2A $build_type $flash_config $boot2a_config" \
-d=" BUILD_SE=$build_trustzone BUILD_MINI=$build_mini_sys $rel_branch BUILD_PIECE=$BUILD_PIECE" \
SDK=1 $rel >> out/mkbsp.log

cd ../
