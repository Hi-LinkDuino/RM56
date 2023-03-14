#!/bin/bash
# Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
date +%F' '%H:%M:%S
echo $@

BASE_HOME=$(dirname $(dirname $(dirname $(dirname $(dirname $(cd $(dirname $0); pwd))))))
PRODUCT=""
PLATFORM=""
TARGET=""
KERNEL=""
DEVICECOMPANY="hisilicon"
BUILD_OUTFILE=*_allinone.bin
DIST_DIR=$BASE_HOME/dist

BUILD_TARGETS=""
#BUILD_TARGETS="${BUILD_TARGETS},//utils/native/lite/test/kv_store_hal:KvStoreTest"


usage()
{
    echo
    echo "USAGE"
    echo "       ./build_testcase.sh product=PRODUCT [kernel=KERNEL] [devicecompany=DEVICECOMPANY] [target=TARGET]"
    echo
    echo "        product   : PRODUCT  product name, such as wifiiot_hispark_pegasus"
    echo "        target    : TARGET   the target for build, such as //utils/native/lite/test:liteos_m_unittest."
    echo "        kernel : KERNEL   the target for kernel, such as liteos_m"
    echo "        devicecompany : DEVICECOMPANY   the devicecompany for build, such as hisilicon"
    echo
    exit 1
}

parse_parameter()
{
    while [ -n "$1" ]
    do
        var="$1"
        OPTIONS=$(echo ${var%%=*})
        PARAM=$(echo ${var#*=})
        case "$OPTIONS" in
        product)    PRODUCT="$PARAM"
                    ;;
        target)     TARGET="$PARAM"
                    ;;
        kernel)     KERNEL="$PARAM"
                    ;;
        devicecompany) DEVICECOMPANY="$PARAM"
                    ;;
        *)   usage
             break;;
        esac
        shift
    done

    if [ "$KERNEL" = "" ];then
        echo "kernel is required, for kernel $KERNEL"
        usage
    fi

    if [ "$PRODUCT" = "" ];then
        echo "product is required, for product $PRODUCT"
        usage
    fi
}


build_testcases()
{
    cd $BASE_HOME
    if [ "$KERNEL" = "liteos_m" ]; then
        board_name=${PRODUCT#*_}
        out_dir="${BASE_HOME}/out/${board_name}/${PRODUCT}"
        suite_root_dir="${out_dir}/suites"
        if [ "${BUILD_TARGETS}" != "" ];then
            build_liteos_m_targets "acts" $BUILD_TARGETS
        else
            echo "BUILD_TARGETS=${BUILD_TARGETS}"
        fi
        if [ -d "$DIST_DIR" ];then
            rm -rf $DIST_DIR
        fi
    fi
}


build_liteos_m_targets()
{
    current_name=$1
    current_target=$2
    tdd_root_dir="${suite_root_dir}/${current_name}"
    suite_out_dir="${tdd_root_dir}/testcases"
    suite_out_zip="${tdd_root_dir}.zip"

    mkdir -p $DIST_DIR
    IFS=',' read -r -a array <<< "${current_target}"
    echo "--------------------------------------------${array[@]}"
    set -e
    mkdir -p ${DIST_DIR}/json

    for element in ${array[*]}
    do
        echo "python build.py -p ${PRODUCT}@${DEVICECOMPANY} -f -t xts ${element}"
        python build.py -p ${PRODUCT}@${DEVICECOMPANY} -f -t xts ${element}

        suite_build_target=$(echo "${element}" | awk -F "[/:]" '{print $NF}')
        echo "suite_build_target=${suite_build_target}"
        module_list_file=$suite_out_dir/module_info.json
        echo "module_list_file=${module_list_file}"
        suite_module_name=$(python test/xts/tools/lite/build/utils.py --method_name get_modulename_by_buildtarget --arguments module_list_file=${module_list_file}#build_target=${suite_build_target})
        echo "suite_module_name=${suite_module_name}"
        subsystem_name=$(python test/xts/tools/lite/build/utils.py --method_name get_subsystem_name --arguments path=${element})
        echo "subsystem_name=${subsystem_name}"
        python test/xts/tools/lite/build/utils.py --method_name record_testmodule_info --arguments build_target_name=${suite_module_name}#module_name=${suite_module_name}#subsystem_name=${subsystem_name}#suite_out_dir=${DIST_DIR}/json#same_file=True

        mkdir -p ${suite_out_dir}/${subsystem_name}
        cp -rf ${BASE_HOME}/out/${board_name}/${PRODUCT}/${BUILD_OUTFILE} ${suite_out_dir}/${subsystem_name}/${suite_module_name}.bin
        if [ -f "${suite_out_dir}/${subsystem_name}/*.a" ];then
            rm -f ${suite_out_dir}/${subsystem_name}/*.a
        fi
        cp -rf ${tdd_root_dir} ${DIST_DIR}
    done

    cp -rf ${DIST_DIR}/${current_name} ${suite_root_dir}
    rm -rf ${suite_out_dir}/.bin
    cp -rf ${DIST_DIR}/json/module_info.json ${suite_out_dir}
    cd $suite_root_dir
    if [ -f "${suite_out_zip}" ];then
        rm -f ${suite_out_zip}
    fi
    zip -rv ${suite_out_zip} ${current_name}

    mkdir -p ${out_dir}/test
    mv ${tdd_root_dir}/testcases/module_info.json ${out_dir}/test/
    mv ${tdd_root_dir}/testcases/test_component.json ${out_dir}/test/
    mv ${tdd_root_dir}/testcases ${out_dir}/test/unittest
    cd $BASE_HOME
}

echo $BASE_HOME
parse_parameter $@
build_testcases

date +%F' '%H:%M:%S
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
