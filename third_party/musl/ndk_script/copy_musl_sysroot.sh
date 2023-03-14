#!/bin/bash
# Copyright (c) Huawei Technologies Co., Ltd. 2020-2030. All rights reserved.
set -e

while getopts "o:i:t:h" arg
do
    case "${arg}" in
        "o")
            OUT_DIR=${OPTARG}
            ;;
        "i")
            SOURCE_DIR=${OPTARG}
            ;;
        "t")
	    TARGET_ARCH=${OPTARG}
            ;;
        "h")
            echo "help"
            ;;
        ?)
            echo "unkonw argument"
            exit 1
            ;;
    esac
done

if [ ! -d "${OUT_DIR}" ];then
    mkdir -p ${OUT_DIR}
fi

mkdir -p  ${OUT_DIR}/${TARGET_ARCH}-linux-ohos
mkdir -p  ${OUT_DIR}/x86_64-linux-ohos
mkdir -p  ${OUT_DIR}/i686-linux-ohos

cp -rp ${SOURCE_DIR}/* ${OUT_DIR}
mv  ${OUT_DIR}/asm  ${OUT_DIR}/${TARGET_ARCH}-linux-ohos/
mv  ${OUT_DIR}/bits ${OUT_DIR}/${TARGET_ARCH}-linux-ohos/

cp -rp ${OUT_DIR}/asm-x86/asm ${OUT_DIR}/x86_64-linux-ohos
mv ${OUT_DIR}/asm-x86/asm ${OUT_DIR}/i686-linux-ohos
rm -fr ${OUT_DIR}/asm-x86