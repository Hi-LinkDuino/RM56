#!/bin/bash
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
set -e

system=$(uname -s)
ROOTFS_DIR=$1
FSTYPE=$2
ROOTFS_IMG=${ROOTFS_DIR}"_"${FSTYPE}".img"
JFFS2_TOOL=mkfs.jffs2
WIN_JFFS2_TOOL=mkfs.jffs2.exe
VFAT_TOOL=mkfs.vfat
MCOPY_TOOL=mcopy
EXT4_TOOL=mkfs.ext4
EXT4_SH=$(dirname $(readlink -f "$0"))/../../../third_party/e2fsprogs/contrib/populate-extfs.sh

tool_check() {
local ret='0'
command -v "$1" >/dev/null 2>&1 || { local ret='1'; }
if [ "$ret" -ne 0  ]; then
    echo "$1 tool is not exit, please install it" >&2
fi
return 0
}

if [[ "${ROOTFS_DIR}" = *"rootfs" ]]; then
    if [ -d "${ROOTFS_DIR}" ]; then
        chmod -R 755 ${ROOTFS_DIR}
    fi
    if [ -f "${ROOTFS_DIR}/bin/init" ] && [ -f "${ROOTFS_DIR}/bin/shell" ]; then
        chmod 700 ${ROOTFS_DIR}/bin/init 2> /dev/null
        chmod 700 ${ROOTFS_DIR}/bin/shell 2> /dev/null
    fi
fi

CONFIG_PATH=$(dirname $(readlink -f "$0"))/rootfs_linux.config

if [ "${FSTYPE}" = "jffs2" ]; then
    if [ "${system}" != "Linux" ] ; then
        tool_check ${WIN_JFFS2_TOOL}
        ${WIN_JFFS2_TOOL} -q -o ${ROOTFS_IMG} -d ${ROOTFS_DIR} --pagesize=4096
    else
        tool_check ${JFFS2_TOOL}
        if [[ "${ROOTFS_DIR}" = *"rootfs" ]]; then
            ${JFFS2_TOOL} -q -o ${ROOTFS_IMG} -d ${ROOTFS_DIR} --pagesize=4096 --devtable ${CONFIG_PATH}
        else
            ${JFFS2_TOOL} -q -o ${ROOTFS_IMG} -d ${ROOTFS_DIR} --pagesize=4096
        fi
    fi
elif [ "${FSTYPE}" = "vfat" ]; then
    if [ "${system}" != "Linux" ] ; then
        echo "Unsupported fs type!" >&2
    else
        tool_check ${VFAT_TOOL}
        tool_check ${MCOPY_TOOL}
        BLK_SIZE=512
        CLT_SIZE=2048
        FAT_TAB_NUM=2
        CLT_CNT=$(( ${CLT_SIZE} / ${BLK_SIZE} ))
        if [ $# -eq 3 ]; then
            IMG_SIZE=$3
        else
            FAT32_ITEM_SIZE=4
            RESV_CNT=38
            IMG_MIN_SIZE=1048576
            DIR_SIZE=$(( $(echo $(du -s ${ROOTFS_DIR} | awk '{print $1}')) * 1024 ))
            IMG_SIZE=$(( ${DIR_SIZE} / (1 - ${FAT_TAB_NUM} * ${FAT32_ITEM_SIZE} / ${CLT_SIZE}) + ${RESV_CNT} * ${BLK_SIZE}))
            if [ ${IMG_SIZE} -le ${IMG_MIN_SIZE} ]; then
                IMG_SIZE=${IMG_MIN_SIZE}
            fi
        fi
        IMG_CNT=$(( (${IMG_SIZE} + ${BLK_SIZE} - 1) / ${BLK_SIZE} ))
        echo mtools_skip_check=1 >> ~/.mtoolsrc
        dd if=/dev/zero of=${ROOTFS_IMG} count=${IMG_CNT} bs=${BLK_SIZE}
        ${VFAT_TOOL} ${ROOTFS_IMG} -s ${CLT_CNT} -f ${FAT_TAB_NUM} -S ${BLK_SIZE} > /dev/null
        ${MCOPY_TOOL} -i ${ROOTFS_IMG} ${ROOTFS_DIR}/* -/ ::/
    fi
elif [ "${FSTYPE}" = "ext4" ]; then
    if [ "${system}" != "Linux" ] ; then
        echo "Unsupported fs type!" >&2
    else
        if [ $# -eq 3 ]; then
            EMMC_ROOTFS_SIZE=$3
        else
            if [[ "${ROOTFS_DIR}" = *"rootfs" ]]; then
                EMMC_ROOTFS_SIZE=50
            else
                EMMC_ROOTFS_SIZE=50
            fi
        fi
        COUNT_SIZE=`expr $EMMC_ROOTFS_SIZE \* 1024 \* 2`
        dd if=/dev/zero of=${ROOTFS_IMG} bs=512 count=${COUNT_SIZE}
        ${EXT4_TOOL} ${ROOTFS_IMG}
        ${EXT4_SH} ${ROOTFS_DIR} ${ROOTFS_IMG} ${CONFIG_PATH}
    fi
else
    echo "Unsupported fs type!" >&2
fi
