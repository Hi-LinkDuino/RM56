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

OUTPUT_DIR=$1
TYPE=$2

if [ "${TYPE}" = "veritysetup" ]; then
    veritysetup format ${OUTPUT_DIR}/systemfs_ext4.img ${OUTPUT_DIR}/systemhashfs/system_hashtree\
    --data-block-size=1024 --hash-block-size=1024 > ${OUTPUT_DIR}/system_verityheader
    sed -n "9p" ${OUTPUT_DIR}/system_verityheader | awk '{print $3}' > ${OUTPUT_DIR}/systemhashfs/system_roothash
elif [ "${TYPE}" = "addS82ohos" ]; then
    mkdir ${OUTPUT_DIR}/rootfs/systemfs
    mkdir ${OUTPUT_DIR}/rootfs/systemhashfs
    CMD='
if [ ! -e /dev/mmcblk0p6 ]; then
exit 0
fi
mount -t ext4 -o ro /dev/mmcblk0p6 /systemhashfs
/bin/veritysetup create dm-systemfs /dev/mmcblk0p5 /systemhashfs/system_hashtree $(cat /systemhashfs/system_roothash)
mount -t ext4 /dev/dm-0 /systemfs
echo 3840 > /sys/block/dm-0/queue/read_ahead_kb
echo 0 > /sys/module/dm_verity/parameters/prefetch_cluster'
    echo "$CMD" >> ${OUTPUT_DIR}/rootfs/etc/init.d/S82ohos
else
    echo "Unsupported type!" >&2
fi
