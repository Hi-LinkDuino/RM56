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
        "h")
            echo "help"
            ;;
        ?)
            echo "unkonw argument"
            exit 1
            ;;
    esac
done


ndk_dir=$(ls ${SOURCE_DIR}/lib/|more |awk '{print $NF}')

for i in $ndk_dir
do
	find ${OUT_DIR}/lib/$i -name 'libc++.a' -type f  -exec bash -c 'echo "INPUT(-lc++_static -lc++abi)" > $1' _ {}  \;
	find ${OUT_DIR}/lib/$i -name 'libc++.so' -type f -exec bash -c 'echo "INPUT(-lc++_shared)" > $1'  _ {}  \;
done

cp -rfp ${SOURCE_DIR}/lib   ${OUT_DIR}
cp -rfp ${SOURCE_DIR}/include  ${OUT_DIR}

