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
    echo "       ./runtest.sh device=IP:PORT serial=[SERIAL_NUMBER] [module=MODULE_NAME] [test=TEST] [runonly=RUN_ONLY]"
    echo "                  First, you must be execute stf/start.bat in the windows which connected a device."
    echo "                  target_platform : TARGET_PLATFORM the target platform, such as phone or ivi; Default to phone"
    echo "                  device    : IP:PORT  the host ip:port of the connected device."
    echo "                  module    : MODULE_NAME name of the module to run. Run all modules by default."
    echo "                  test      : TEST_NAME name of the test to run. Run all tests by default. This should be a FULL-QUALIFIED className or methodName."
    echo "                  runonly   : RUN_ONLY TRUE=not build, only runtest; FALSE=build and runtest. FALSE by default."
    echo
    exit 1
}

parse_cmdline()
{
    echo parse_cmdline:$@
    SCRIPT_DIR=$(cd $(dirname $0); pwd)
    BUILD_SCRIPT=${SCRIPT_DIR}/build.sh
    BASE_HOME=${SCRIPT_DIR}/../../..
    OUT_DIR=${SCRIPT_DIR}/../../../out
    TARGET_ARCH=arm64
    TARGET_PLATFORM=all
    BUILD_VARIANT=release
    MODULE_NAME=""
    DEVICE_SN=""
    TEST_NAME=""
    RUN_ONLY=FALSE
    BUILD_PARAM=""
    ACTS_HOME=$SCRIPT_DIR/../../../out/$BUILD_VARIANT/suites/acts
    MODULE_INFO_FILE=$SCRIPT_DIR/../../../out/$BUILD_VARIANT/suites/acts/testcases/module_info.list
    export PATH=${BASE_HOME}/prebuilts/python/linux-x86/3.8.5/bin:$PATH
    BUILD_PARAM="$BUILD_PARAM target_arch=$TARGET_ARCH variant=$BUILD_VARIANT"
    
    while [ -n "$1" ]
    do
        var="$1"
        OPTIONS=$(echo ${var%%=*})
        PARAM=$(echo ${var#*=})
        case "$OPTIONS" in
        target_platform) TARGET_PLATFORM="$PARAM"
                         BUILD_PARAM="$BUILD_PARAM target_platform=$TARGET_PLATFORM"
                         ;;
        module)          MODULE_NAME="$PARAM"
                         BUILD_PARAM="$BUILD_PARAM suite=$MODULE_NAME"
                         ;;
        test)            TEST_NAME="$PARAM"
                         ;;
        device)          DEVICE_SN="$DEVICE_SN;$PARAM"
                         ;;
        runonly)         RUN_ONLY=$(echo $PARAM |tr [a-z] [A-Z])
                         ;;
        *)   usage
             break;;
        esac
        shift
    done
    if [ "$DEVICE_SN" == "" ];then
        usage
    fi
}

do_make()
{
    cd $BASE_HOME
    echo "================================"
    echo "start to build"
    echo "$BUILD_SCRIPT $BUILD_PARAM"
    echo "================================"
    
    echo BUILD_PARAM= $BUILD_PARAM
    $BUILD_SCRIPT $BUILD_PARAM
    if [ "$?" != 0 ]; then
        echo -e "\033[31mbuild error!\033[0m"
        exit 1
    fi
}

get_testmodule_name()
{
    BUILD_TARGET_NAME=$1
    # map from build_target name to actual testmodule name
    test_module_name=""
    OLD_IFS=$IFS
    IFS=$'\n'
    for line in $(cat $MODULE_INFO_FILE); do
        VAR1=$(echo $line |awk -F ' ' '{print $1}')
        VAR2=$(echo $line |awk -F ' ' '{print $2}')
        if [ "$BUILD_TARGET_NAME" == "$VAR1" ] ; then
           test_module_name=$VAR2
           break
        fi
    done
    IFS=$OLD_IFS
    echo $test_module_name
}

run_test_with_xdevice()
{
    TC_PATH=$ACTS_HOME/testcases
    RES_PATH=$ACTS_HOME/resource/
    REPORT_PATH=$ACTS_HOME/xdevice_reports/$(date "+%Y.%m.%d-%H.%M.%S")
    python -m easy_install --quiet $ACTS_HOME/tools/xdevice-0.0.0.tar.gz
    python -m easy_install --quiet $ACTS_HOME/tools/xdevice-extension-0.0.0.tar.gz

    run_arguments="run acts -tcpath $TC_PATH -respath $RES_PATH -rp $REPORT_PATH"
    if [ ! -z "$MODULE_NAME" ]; then
        run_arguments="$run_arguments -l $1"
    fi
    if [ ! -z "$TEST_NAME" ]; then
        TEST_LIST_FILE=$ACTS_HOME/include_tests
        echo "$TEST_NAME" > $TEST_LIST_FILE
        run_arguments="$run_arguments -ta test-file-include-filter:$TEST_LIST_FILE"
    fi
    if [ ! -z "$DEVICE_SN" ]; then
        run_arguments="$run_arguments $(echo $DEVICE_SN | sed 's/;/-sn /')"
    fi

    echo $run_arguments
    cd $ACTS_HOME
    python -m xdevice "$run_arguments"

    return $?
}

run_test()
{
    cd $BASE_HOME

    MAPPED_MODULE_NAME=$MODULE_NAME
    # map from build_target name to actual testmodule name
    if [ ! -z "$MODULE_NAME" ]; then
        MAPPED_MODULE_NAME=$(get_testmodule_name $MODULE_NAME $ACTS_HOME)
        if [ ! -z "$MAPPED_MODULE_NAME" ]; then
          echo -e "\033[32mTest module name of build_target "$MODULE_NAME is" "$MAPPED_MODULE_NAME"\033[0m"
        else
          echo -e "\033[31mTest module "$MODULE_NAME" does not exist!\033[0m"
          exit 1
        fi
    fi

    JSON_CONFIG_FILE=$ACTS_HOME/testcases/$MAPPED_MODULE_NAME.json
    ret_code=0
    if [ -f "$JSON_CONFIG_FILE" ]; then 
        echo -e "\033[32mRun $MAPPED_MODULE_NAME with xdevice ...\033[0m"
        run_test_with_xdevice $MAPPED_MODULE_NAME
        ret_code=$?
    elif [ -z "$MODULE_NAME" ]; then
        echo -e "\033[32mRun all modules with xdevice ...\033[0m"
        run_test_with_xdevice
        ret_code=$?
    else
        echo -e "\033[31mNon-xdevice module in acts!\033[0m"
        exit 1
    fi
 
    if [ "$ret_code" != "0" ]; then
        echo -e "\033[31mFailed to run test, ret=$ret_code\033[0m"
        exit 1
    fi
}

parse_cmdline $@
if [ "$RUN_ONLY" == FALSE ];then
    do_make
fi
mkdir -p $OUT_DIR/$BUILD_VARIANT/suites/acts/resource/tools
run_test
exit 0
