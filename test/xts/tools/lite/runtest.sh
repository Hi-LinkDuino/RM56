#!/usr/bin/env sh
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

BASE_HOME=$(dirname $(dirname $(cd `dirname $0`; pwd)))
NFS_IP=
NFS_PORT=
NFS_ROOT=/data/nfsuser/data
NFS_USER=
NFS_PWD=
USER=
DEVICE=
DEVICE_IP=
DEVICE_PORT=
NFS_DIR=
MODULE=
TEST_FILE_NAME=
PRODUCT=
SUITE_OUT=
MODULE_FILES=

usage()
{
    echo
    echo "USAGE"
    echo "       ./runtest.sh user=USERNAME product=PRODUCT platform=PLATFORM module=MODULE device=IP:PORT"
    echo
    echo "            user      : USERNAME  your name, no spaces allowed, such as xiaoming"
    echo "            module    : MODULE   the module name"
    echo "                                  which is same with BUILD.gn target name, such as wifiaware_test."
    echo "            device    : IP:PORT  the host ip:port of the connected device, such as: xx.xx.xx.xx:xxxx"
    echo "            product   : PRODUCT  the product name. such as wifiiot or ipcamera"
    echo "            platform  : PLATFORM  the platform name."
    echo
    echo
    exit 1
}

parse_cmdline()
{
    while [ -n "$1" ]
    do
        var="$1"
        OPTIONS=`echo ${var%%=*}`
        PARAM=`echo ${var#*=}`
        case "$OPTIONS" in
        user)      USER="$PARAM"
                   NFS_DIR=$NFS_ROOT/$USER/
                   ;;
        device)    DEVICE="$PARAM"
                   DEVICE_IP=`echo ${PARAM%%:*}`
                   DEVICE_PORT=`echo ${PARAM#*:}`
                   ;;
        module)    MODULE="$PARAM"
                   ;;
        product)   PRODUCT="$PARAM"
                   ;;
        platform)  PLATFORM="$PARAM"
                   ;;
        *)   usage
             break;;
        esac
        shift
    done
    if [ "$USER" = "" ];then
        echo "user is required!"
        usage
    fi
    if [ "$PRODUCT" = "" ];then
        echo "product is required!"
        usage
    fi
    if [ "$DEVICE" = "" ];then
        echo "device is required!"
        usage
    fi
    if [ "$DEVICE_IP" = "" ] || [ "$DEVICE_PORT" = "" ];then
        echo "device is required!"
        usage
    fi
    if [ "$MODULE" = "" ];then
        echo "module is required!"
        usage
    fi
    if [ "$PLATFORM" = "" ];then
        echo "platform is required!"
        usage
    fi
    if [ "$PRODUCT" != "ipcamera" ];then
        echo "Only ipcamera is supported!"
        exit 1
    fi
    SUITE_OUT="${BASE_HOME}/out/${PRODUCT}_${PLATFORM}/suites"
}

check_environment()
{
  if ! [ -x "$(command -v sshpass)" ]; then
    echo "Please run 'sudo apt-get install sshpass' first."
    exit 1
  fi
  local suite_zip=$(basename "$(find $SUITE_OUT -name *.zip)")
  if [ -z "$suite_zip" ];then
    echo "Please run xts/tools/build.sh to build target first"
    exit 1
  fi
  local suite_name=$(echo ${suite_zip%%.*})
  SUITE_OUT=$SUITE_OUT/$suite_name
  MODULE_FILES=$(find $SUITE_OUT/testcases -name ${MODULE}*)
  if [ -z "${MODULE_FILES}" ];then
    echo "Can not find the target: ${MODULE}"
    echo "Please run xts/tools/build.sh to build target first"
    exit 1
  fi
}

copy_files_to_nfs()
{
    sshpass -p $NFS_PWD ssh -p $NFS_PORT -t $NFS_USER@$NFS_IP "mkdir -p $NFS_DIR"
    for file in $MODULE_FILES; do
      echo "sync $file to nfs server"
      sshpass -p $NFS_PWD scp -P $NFS_PORT $file $NFS_USER@$NFS_IP:$NFS_DIR
    done

}

set_environment()
{
    USER_CONFIG=$SUITE_OUT/config/user_config.xml
    sed -i 's/\$device_ip\$/'$DEVICE_IP'/g' $USER_CONFIG
    sed -i 's/\$device_port\$/'$DEVICE_PORT'/g' $USER_CONFIG
    sed -i 's/\$user\$/'$USER'/g' $USER_CONFIG
}

run_test()
{
    cd $SUITE_OUT
    chmod +x run.sh
    ./run.sh run -l $MODULE
}

parse_cmdline $@
check_environment
copy_files_to_nfs
set_environment
run_test
