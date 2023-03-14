#!/bin/bash

SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

##default values
PRODUCT=1600_display_demo
LOG_FILE=bsp_build_make_best1600.log
_VERBOSE=0
BSP_ROOT=device/soc/bestechnic/bes2700/liteos_m/sdk/bsp
ACTION=ALL

bes1600_signle_flash_patch=vendor/bestechnic/1600_display_demo/patches/1600D-single-flash-littlefs.patch
bes1502x_evb13_touch_patch=vendor/bestechnic/1502x_display_demo/patches/1502x-evb-1.3-touch.patch

ohos2master_bsp_patch=${SCRIPT_DIR}/../vendor/bestechnic/1600_display_demo/patches/bsp-b5334bb2.patch
BSP_NODE=`echo ${ohos2master_bsp_patch} | sed -n "s#\(.*bsp-\)\([0-9a-f]*\)\(.*\)#\2#p"`

ohos2master_bthost_patch=${SCRIPT_DIR}/../vendor/bestechnic/1600_display_demo/patches/bthost-848d11fe.patch
BTHOST_NODE=`echo ${ohos2master_bthost_patch} | sed -n "s#\(.*bthost-\)\([0-9a-f]*\)\(.*\)#\2#p"`
BTHOST_NODE=7182b3a2

ohos2master_multimedia_patch=${SCRIPT_DIR}/../vendor/bestechnic/1600_display_demo/patches/multimedia-11a2bde8.patch
MULTIMEDIA_NODE=`echo ${ohos2master_multimedia_patch} | sed -n "s#\(.*multimedia-\)\([0-9a-f]*\)\(.*\)#\2#p"`

LIANGYOU_BRANCH=false

ohos_out_board=2700bp
BSP_PERL=${BSP_ROOT}/tools/fill_sec_base.pl

## none-do nothing single-patch with single flash dual-patch with dual flash
FLASH_PATCH=none
export FLASH_DUAL_CHIP=1
REVERSE_OPT=
eval set -- $(getopt -q -o b:cfhLmRsBVo:pPT --long help,bsp -- "$@")

help()
{
    echo "Usage: $0 [-b 1502x|1600] [-c] [-f] [-s] [-B] [-T] [-V] [-h]"
    echo "    -b 1502x, compile for target 1502x"
    echo "    -b 1600, compile for target 1600"
    echo "    -c only clean"
    echo "    -f force to compile"
    echo "    -h print this help information"
    echo "    -L switch to liangyou branch for bsp/bthost/multimedia"
    echo "    -m switch bsp/bthost/multimedia to master"
    echo "    -R reverse bsp patch"
    echo "    -s compile only for sensor hub"
    echo "    -B compile only for BTH"
    echo "    -p patch with 1600 single flash"
    echo "    -P patch with 1600 dual flash"
	echo "    -T 1502x evb 1.3 support touch"
    echo "    -V compile verbosely"
}

## compile bth image and then copy bin/elf/map to product release_bin folder
## compile_bth allclean, clean bth generated objects
## compile_bth V=1, compile bth with verbose
compile_bth()
{
    echo "compile bth image"
    #(cd ${BSP_ROOT} && make T=best1600_watch_bth COMPILE_TARGET=BTH_AS_SUBSYS -j12 $1)
    (cd ${BSP_ROOT} && make T=best1600_watch_bth COMPILE_TARGET=BTH_AS_SUBSYS -j12 $1)

    if [ x"$1" != x"allclean" ];then
    [ ! $? -eq 0 ] || {
        if [[ -e out/${ohos_out_board}/${PRODUCT}/release_bin/ ]] && [[ -e ${BSP_ROOT}/out/best1600_watch_bth/best1600_watch_bth.bin ]];then
            echo "copy images..."
            cp -af ${BSP_ROOT}/out/best1600_watch_bth/best1600_watch_bth.* out/${ohos_out_board}/${PRODUCT}/release_bin/;
        fi
    }
    fi
}

apply_flash_patch()
{
    [ -e ${SCRIPT_DIR}/../out ] || { mkdir ${SCRIPT_DIR}/../out; }
    touch ${SCRIPT_DIR}/../out/.flash_patch
    flash_patched=`cat ${SCRIPT_DIR}/../out/.flash_patch`
    if [ ${FLASH_PATCH} = "single" ];then
      export FLASH_DUAL_CHIP=0
      if [ x"${flash_patched}" != x"1" ];then
        ${SCRIPT_DIR}/repo-patch.sh ${bes1600_signle_flash_patch}
        [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.flash_patch; }
      fi
    elif [ ${FLASH_PATCH} = "dual" ];then
      export FLASH_DUAL_CHIP=1
      if [ x"${flash_patched}" = x"1" ];then
        ${SCRIPT_DIR}/repo-patch.sh ${bes1600_signle_flash_patch} -R
        [ $? -ne 0 ] || { echo 0 > ${SCRIPT_DIR}/../out/.flash_patch; }
      fi 
    fi
}

apply_touch_patch()
{
    [ -e ${SCRIPT_DIR}/../out ] || { mkdir ${SCRIPT_DIR}/../out; }
    touch ${SCRIPT_DIR}/../out/.touch_patch
    touch_patched=`cat ${SCRIPT_DIR}/../out/.touch_patch`
	
	if [ x"$1" = x"1" ];then
      if [ x"${touch_patched}" != x"1" ];then
        ${SCRIPT_DIR}/repo-patch.sh ${bes1502x_evb13_touch_patch}
        [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.touch_patch; }
      fi
	else
	  if [ x"${touch_patched}" = x"1" ];then
        ${SCRIPT_DIR}/repo-patch.sh ${bes1502x_evb13_touch_patch} -R
        [ $? -ne 0 ] || { echo 0 > ${SCRIPT_DIR}/../out/.touch_patch; }
      fi
	fi
}

apply_bsp_patch()
{
    [ -e ${SCRIPT_DIR}/../out ] || { mkdir ${SCRIPT_DIR}/../out; }
    touch ${SCRIPT_DIR}/../out/.bsp_patch
    touch ${SCRIPT_DIR}/../out/.bthost_patch
    touch ${SCRIPT_DIR}/../out/.media_patch
    bsp_patched=`cat ${SCRIPT_DIR}/../out/.bsp_patch`
    bthost_patched=`cat ${SCRIPT_DIR}/../out/.bthost_patch`
    media_patched=`cat ${SCRIPT_DIR}/../out/.media_patch`

    if [ x"$1" = x"-R" ];then
      if [ x"${bsp_patched}" = x"1" ];then
        (cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp && git apply ${ohos2master_bsp_patch} -R )
        [ $? -ne 0 ] || { echo 0 > ${SCRIPT_DIR}/../out/.bsp_patch; }
      fi
      if [ x"${bthost_patched}" = x"1" ];then
        #(cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/bthost && git apply ${ohos2master_bthost_patch} -R )
        [ $? -ne 0 ] || { echo 0 > ${SCRIPT_DIR}/../out/.bthost_patch; }
      fi
      if [ x"${media_patched}" = x"1" ];then
        (cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/multimedia && git apply ${ohos2master_multimedia_patch} -R )
        [ $? -ne 0 ] || { echo 0 > ${SCRIPT_DIR}/../out/.media_patch; }
      fi
    else
      if [ x"${bsp_patched}" != x"1" ];then
        if [ x"${LIANGYOU_BRANCH}" = x"true" ];then
          (cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp && git fetch && git checkout ${BSP_NODE} -f)
        else
          (cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp && git fetch && git checkout ${BSP_NODE} && git apply ${ohos2master_bsp_patch} )
        fi
        [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.bsp_patch; }
      fi
      if [ x"${bthost_patched}" != x"1" ];then
        if [ x"${LIANGYOU_BRANCH}" = x"true" ];then
          #(cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/bthost && git fetch && git checkout ${BTHOST_NODE} && git apply ${ohos2master_bthost_patch} )
          (cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/bthost && git fetch && git checkout ${BTHOST_NODE} -f)
        else
          #(cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/bthost && git fetch && git checkout ${BTHOST_NODE} && git apply ${ohos2master_bthost_patch} )
          (cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/bthost && git fetch && git checkout ${BTHOST_NODE})
        fi
        [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.bthost_patch; }
      fi
      if [ x"${media_patched}" != x"1" ];then
        if [ x"${LIANGYOU_BRANCH}" = x"true" ];then
          (cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/multimedia && git fetch && git checkout ${MULTIMEDIA_NODE} -f)
        else
          (cd device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/multimedia && git fetch && git checkout ${MULTIMEDIA_NODE} && git apply ${ohos2master_multimedia_patch} )
        fi
        [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.media_patch; }
      fi
    fi
}

## parse all the parameters and change setting to compile
while [ -n "$1" ]
do
  case $1 in
    -b)
    if [ x"$2" == x"1502x" ];then
      unset FLASH_DUAL_CHIP
      PRODUCT=1502x_display_demo
      LOG_FILE=bsp_build_make_best1502x.log
      ohos_out_board=2700ibp
    else
      PRODUCT=1600_display_demo
      LOG_FILE=bsp_build_make_best1600.log
      ohos_out_board=2700bp
    fi
    shift
    ;;
    -c)
    ACTION=CLEAN;
    ;;
    -f)
    FORCE=-f
    ;;
    -L)
    LIANGYOU_BRANCH=true
    BSP_NODE=tp/feat-bsp-src-ly
    BTHOST_NODE=tp/feat-bthost-src-ly
    MULTIMEDIA_NODE=tp/feat-multimedia-src-ly
    ;;
    -m)
    BSP_NODE=master
    BTHOST_NODE=master
    MULTIMEDIA_NODE=master
    ;;
    -R)
    REVERSE_OPT=-R
    ;;
    -s)
    ;;
    -h|--help)
    help
    exit 0
    ;;
    -B)
    ACTION=BTH
    ;;
    -V)
    VERBOSE=-v
    _VERBOSE=1
    ;;
    -o)
    shift
    ;;
    -p)
    FLASH_PATCH=single
    ;;
    -P)
    FLASH_PATCH=dual
    ;;
	-T)
	TOUCH_PATCH=1
	;;
    --)
    shift
    break;
    ;;
  esac
  shift
done

## reserved
## retain all the left parameter, which could be used for compile bsp for platform_tester or noapp
for param in "$@"
do
    echo "left parameters: $param"
done


## compile action
case $ACTION in
  "ALL")
## compile ohos for 1502x or 1600, force and verbose depends on -f and -V, respectively
    hb set -p ${PRODUCT} || true

    ##1502x or bsp master wouldn't be patched for single/dual flash.
    [[ x"${PRODUCT}" != x"1600_display_demo" ]] || apply_flash_patch
	
	if [ x"${PRODUCT}" = x"1502x_display_demo" ];then
	  apply_touch_patch ${TOUCH_PATCH}
	fi

    ##bsp master would be patched, otherwise reversed.
    apply_bsp_patch ${REVERSE_OPT}
    
    [ -z "${REVERSE_OPT}" ] || { echo "just only reverse bsp patched "; exit 0; }

    [ $? -eq 0 ] || { echo "patch for bsp failure."; exit 255; }
 
    hb build ${FORCE} -p ${PRODUCT} ${VERBOSE}

## checking command result, if failure, output BSP log to current terminal and exit
    [ $? -eq 0 ] || { exit 255; cat ${BSP_ROOT}/out/log/${LOG_FILE}; exit 255; }

## copy the unstripped elf to product release_bin folder
    BIN_SRC_PATH=out/${ohos_out_board}/${PRODUCT}/unstripped/bin
    BIN_REL_PATH=out/${ohos_out_board}/${PRODUCT}/release_bin
    cp -af ${BIN_SRC_PATH}/wifiiot_${ohos_out_board}.elf ${BIN_REL_PATH}/
    ${BSP_PERL} ${BIN_SRC_PATH}/wifiiot_${ohos_out_board}.elf ${BIN_SRC_PATH}/wifiiot_${ohos_out_board}.bin ${BIN_REL_PATH}/rtos_main.bin

## as for 1600, compile bth also, and verbose or not depends on -V parameter.
## 1502x compiled sensor hub, so just copy the binary to product release_bin folder
    if [ x"${PRODUCT}" == x"1600_display_demo" ];then
        compile_bth V=${_VERBOSE};
    else
        cp -af ${BSP_ROOT}/out/sensor_hub_watch/sensor_hub_watch.bin out/${ohos_out_board}/${PRODUCT}/release_bin/
    fi
  ;;
  "BTH")
    if [ x"${PRODUCT}" == x"1600_display_demo" ];then
## if compile bth with -f parameter, clean it
        [ -z "${FORCE}" ] || compile_bth allclean
## compile with verbose or not, depends on parameter -V
        compile_bth V=${_VERBOSE};
    fi
  ;;
  "CLEAN")
## ohos clean
  hb clean;
## bth clean
  compile_bth allclean
  ;;
esac
