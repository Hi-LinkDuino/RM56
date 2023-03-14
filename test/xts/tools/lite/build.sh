#!/usr/bin/env sh
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

BASE_HOME=$(dirname $(dirname $(dirname $(dirname $(cd `dirname $0`; pwd)))))
PRODUCT=
PLATFORM=
TARGET=
TARGET_PARAM=
XTS=
WIFIIOT_OUTFILE=Hi3861_wifiiot_app_allinone.bin
DIST_DIR=$BASE_HOME/dist
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/communication_lite/lwip_hal:ActsLwipTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/communication_lite/wifiservice_hal:ActsWifiServiceTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/utils_lite/file_hal:ActsUtilsFileTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/startup_lite/syspara_hal:ActsParameterTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/iot_hardware_lite/iot_controller_hal:ActsWifiIotTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/utils_lite/kv_store_hal:ActsKvStoreTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/security_lite/huks/liteos_m_adapter:ActsHuksHalFunctionTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/hiviewdfx_lite/hilog_hal:ActsDfxFuncTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/hiviewdfx_lite/hievent_hal:ActsHieventLiteTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/distributed_schedule_lite/system_ability_manager_hal:ActsSamgrTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/update_lite/dupdate_hal:ActsUpdaterFuncTest"
WIFIIOT_ACTS_MODULES="${WIFIIOT_ACTS_MODULES},//test/xts/acts/startup_lite/bootstrap_hal:ActsBootstrapTest"

error_report() {
    echo "Error on line $1"
}
trap 'error_report $LINENO' ERR


usage()
{
  echo
  echo "USAGE"
  echo "       ./build.sh product=PRODUCT [platform=PLATFORM] [target=TARGET] xts=XTS"
  echo
  echo "                  product   : PRODUCT  product name, such as ipcamera or wifiiot"
  echo "                  platform  : PLATFORM  the platform of device"
  echo "                  target    : TARGET   the target for build, such as //xts/acts/communication_lite/wifiaware_test."
  echo "                  xts       : XTS   the name of xts, such as acts/hits etc."
  echo
  exit 1
}

check_python()
{
  python_cmd=
  ver=$(python -c 'import platform; major, minor, patch = platform.python_version_tuple(); print(major);')
  if [ "$ver" = "3" ];then
    python_cmd=python
  else
    ver=$(python3 -c 'import platform; major, minor, patch = platform.python_version_tuple(); print(major);')
    if [ "$ver" = "3" ];then
      python_cmd=python3
    fi
  fi
  if [ -z "$python_cmd" ];then
    echo "Enviroment variable 'python3' is required, and python verion must be greater than 3.7"
    exit 1
  fi
}

parse_cmdline()
{
  while [ -n "$1" ]
  do
    var="$1"
    OPTIONS=`echo ${var%%=*}`
    PARAM=`echo ${var#*=}`
    case "$OPTIONS" in
    product)   PRODUCT="$PARAM"
               ;;
    platform)  PLATFORM="$PARAM"
               ;;
    target)    TARGET="$PARAM"
               ;;
    xts)       XTS="$PARAM"
               ;;
    *)   usage
         break;;
    esac
    shift
  done
  if [ "$PRODUCT" = "" ] || [ "$XTS" = "" ];then
    usage
  fi
  if [ "$PRODUCT" = "wifiiot" ];then
    PLATFORM="hi3861v100_liteos_riscv"
    if [ "$TARGET" = "" ];then
	  if [ "$XTS" = "acts" ];then
         TARGET=$WIFIIOT_ACTS_MODULES
	  elif [ "$XTS" = "hits" ];then
	     TARGET=$WIFIIOT_HITS_MODULES
	  fi
    fi
  elif [ "$PLATFORM" = "" ];then
    echo "platform is required, for product $PRODUCT"
    usage
  fi
  if [ "$TARGET" != "" ];then
    TARGET_PARAM=" --target $TARGET"
  fi
}



build()
{
  out_dir="${BASE_HOME}/out/hispark_pegasus/wifiiot_hispark_pegasus"
  suite_root_dir="${out_dir}/suites"
  cd $BASE_HOME
  if [ "$PRODUCT" = "wifiiot" ]; then
    if [ "$XTS" = "all" ];then
	  build_wifiiot "acts" $WIFIIOT_ACTS_MODULES
	  build_wifiiot "hits" $WIFIIOT_HITS_MODULES
	  cp -rf ${DIST_DIR}/acts ${suite_root_dir}
	  cd $suite_root_dir
      zip -rv acts.zip acts
      rm -rf $DIST_DIR
    else
	  build_wifiiot $XTS $TARGET
      rm -rf $DIST_DIR
    fi
  else
    python build.py ${PRODUCT}_${PLATFORM} -b debug --test xts $TARGET
  fi
}

build_wifiiot()
{
    current_xts=$1
    current_target=$2
    xts_root_dir="${suite_root_dir}/${current_xts}"
    suite_out_dir="${xts_root_dir}/testcases"
    suite_out_zip="${xts_root_dir}.zip"
    mkdir -p $DIST_DIR
    IFS=',' read -r -a array <<< "${current_target}"
    echo "--------------------------------------------${array[@]}"
    set -e
	mkdir -p ${DIST_DIR}/json
    for element in ${array[*]}
    do
      python build.py -p wifiiot_hispark_pegasus@hisilicon -f --test xts ${element} --gn-args build_xts=true
      suite_build_target=`echo "${element}" | awk -F "[/:]" '{print $NF}'`
      module_list_file=$suite_out_dir/module_info.json
      suite_module_name=`python test/xts/tools/lite/build/utils.py --method_name get_modulename_by_buildtarget --arguments module_list_file=${module_list_file}#build_target=${suite_build_target}`
      subsystem_name=`python test/xts/tools/lite/build/utils.py --method_name get_subsystem_name --arguments path=${element}`
	  
      python test/xts/tools/lite/build/utils.py --method_name record_testmodule_info --arguments build_target_name=${suite_module_name}#module_name=${suite_module_name}#subsystem_name=${subsystem_name}#suite_out_dir=${DIST_DIR}/json#same_file=True

      mkdir -p ${suite_out_dir}/${subsystem_name}
      cp -f ${BASE_HOME}/out/hispark_pegasus/wifiiot_hispark_pegasus/${WIFIIOT_OUTFILE} ${suite_out_dir}/${subsystem_name}/${suite_module_name}.bin
      rm -f ${suite_out_dir}/${subsystem_name}/*.a
      cp -rf ${xts_root_dir}  ${DIST_DIR}
    done
	
    cp -rf ${DIST_DIR}/${current_xts} ${suite_root_dir}
    rm -f ${suite_out_dir}/.bin
    cp -rf ${DIST_DIR}/json/module_info.json ${suite_out_dir}
    cd $suite_root_dir
    rm -f ${suite_out_zip}
    zip -rv ${suite_out_zip} ${current_xts}
    cd $BASE_HOME

}

echo $BASE_HOME
check_python
parse_cmdline $@
build
