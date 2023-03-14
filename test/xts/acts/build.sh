#!/bin/bash

# Copyright (C) 2021 Huawei Device Co., Ltd.
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

usage()
{
    echo
    echo "USAGE"
    echo "       ./build.sh [suite=BUILD_TARGET] [target_os=TARGET_OS] [target_arch=TARGET_ARCH] [variant=BUILD_VARIANT] [target_subsystem=TARGET_SUBSYSTEM]"
    echo "                  target_platform  : TARGET_PLATFORM  the target platform, such as phone or ivi; Default to phone"
    echo "                  suite            : BUILD_TARGET       cts/hit/vts and so on, default value is hit"
    echo "                  target_arch      : TARGET_ARCH      arm64 or arm32, default value is arm64"
    echo "                  variant          : BUILD_VARIANT    release or debug, default value is debug"
    echo "                  target_subsystem : TARGET_SUBSYSTEM the target subsystem to build"
    echo "                  system_size      : SYSTEM_SIZE      standard, large and son on, large is for L3-L5, standard is for L2 default value is large"
    echo "                  product_name     : PRODUCT_NAME     the name of product. such as hikey960, Hi3516DV300, and so on."
    echo
    exit 1
}


parse_cmdline()
{
    BASE_HOME=$(dirname $(cd $(dirname $0); pwd))
    BASE_HOME=${BASE_HOME}/../..
    BUILD_TOOLS_DIR=${BASE_HOME}/prebuilts/build-tools/linux-x86/bin
    OUT_DIR=${BASE_HOME}/out
    BUILD_SHELL=${BASE_HOME}/build.sh
    # build all parts for all products by default
    BUILD_TARGET=""
    TARGET_PLATFORM=all
    GN_ARGS="is_dbt_test=true include_all=false"
    TARGET_ARCH=arm64
    BUILD_VARIANT=release
    UPLOAD_API_INFO=False
    SYSTEM_SIZE=large
    PRODUCT_NAME=""
    export PATH=${BASE_HOME}/prebuilts/python/linux-x86/3.8.3/bin:$PATH

    while [ -n "$1" ]
    do
        var="$1"
        OPTIONS=${var%%=*}
        PARAM=${var#*=}
        echo "OPTIONS=$OPTIONS"
        echo "PARAM=$PARAM"
        echo "-------------------"
        case "$OPTIONS" in
        suite)            BUILD_TARGET="$PARAM"
                          ;;
        target_arch)      TARGET_ARCH="$PARAM"
                          ;;
        variant)          BUILD_VARIANT="$PARAM"
                          ;;
	target_platform)  TARGET_PLATFORM="$PARAM"
                          ;;
        target_subsystem) export target_subsystem=${PARAM}
                          ;;
        system_size)      SYSTEM_SIZE="$PARAM"
                          ;;
        product_name)     PRODUCT_NAME="$PARAM"
                          ;;
        upload_api_info)  UPLOAD_API_INFO=$(echo $PARAM |tr [a-z] [A-Z])
                         ;;
        *)   usage
             break;;
        esac
        shift
    done
    if [ "$SYSTEM_SIZE" = "standard" ]; then
       BUILD_TARGET=${BUILD_TARGET:-"acts"}
       PRODUCT_NAME=${PRODUCT_NAME:-"Hi3516DV300"}
    else
       BUILD_TARGET=${BUILD_TARGET:-"acts acts_ivi acts_intellitv acts_wearable"}
       PRODUCT_NAME=${PRODUCT_NAME:-"arm64"}
    fi
}


do_make()
{
    cd $BASE_HOME
    ACTS_ROOT="$BASE_HOME/test/xts/acts"

    rm -rf "$BASE_HOME/test/xts/autogen_apiobjs"
    export XTS_SUITENAME=acts
    if [ "$SYSTEM_SIZE" = "standard" ]; then
        MUSL_ARGS=""
        if [ "$PRODUCT_NAME" = "m40" ]; then
            MUSL_ARGS="--gn-args use_musl=false --gn-args use_custom_libcxx=true --gn-args use_custom_clang=true"
        fi
       ./build.sh --product-name $PRODUCT_NAME --gn-args build_xts=true --build-target $BUILD_TARGET --build-target "deploy_testtools" --gn-args is_standard_system=true $MUSL_ARGS
    else
       if [ "$BUILD_TARGET" = "acts acts_ivi acts_intellitv acts_wearable" ]; then
         ./build.sh --product-name $PRODUCT_NAME --gn-args build_xts=true --build-target "acts" --build-target "acts_ivi" --build-target "acts_intellitv" --build-target "acts_wearable" --build-target "deploy_testtools"
       else
         ./build.sh --product-name $PRODUCT_NAME --gn-args build_xts=true --build-target $BUILD_TARGET --build-target "deploy_testtools"
       fi
    fi
    ret=$?

    rm -rf "$BASE_HOME/test/xts/autogen_apiobjs"
    if [ "$ret" != 0 ]; then
        echo "build error"
        exit 1
    fi
}
parse_cmdline $@
do_make
exit 0
