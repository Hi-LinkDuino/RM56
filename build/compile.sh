#!/bin/bash

SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

source build/project-node.sh


##default values
PRODUCT=1600_display_demo
LOG_FILE=bsp_build_make_best1600.log
_VERBOSE=0
BSP_ROOT=device/soc/bestechnic/bes2700/liteos_m/sdk/bsp
ACTION=ALL

BSP_PATH=device/soc/bestechnic/bes2700/liteos_m/sdk/bsp
BTHOST_PATH=device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/bthost
MEDIA_PATH=device/soc/bestechnic/bes2700/liteos_m/sdk/bsp/multimedia

bes1600_signle_flash_patch=vendor/bestechnic/1600_display_demo/patches/1600D-single-flash-littlefs.patch
bes1502x_evb13_touch_patch=vendor/bestechnic/1502x_display_demo/patches/1502x-evb-1.3-touch.patch

ohos2master_bsp_patch=${SCRIPT_DIR}/../vendor/bestechnic/1600_display_demo/patches/bsp-12f53b56.patch
BSP_NODE=`echo ${ohos2master_bsp_patch} | sed -n "s#\(.*bsp-\)\([0-9a-f]*\)\(.*\)#\2#p"`

ohos2master_bthost_patch=${SCRIPT_DIR}/../vendor/bestechnic/1600_display_demo/patches/bthost-848d11fe.patch
BTHOST_NODE=`echo ${ohos2master_bthost_patch} | sed -n "s#\(.*bthost-\)\([0-9a-f]*\)\(.*\)#\2#p"`
BTHOST_NODE=d930d4de

ohos2master_multimedia_patch=${SCRIPT_DIR}/../vendor/bestechnic/1600_display_demo/patches/multimedia-824f86d7.patch
MULTIMEDIA_NODE=`echo ${ohos2master_multimedia_patch} | sed -n "s#\(.*multimedia-\)\([0-9a-f]*\)\(.*\)#\2#p"`

LIANGYOU_BRANCH=false

LCD_SUPPORT=rm69330
BOOT_LOGO_SUPPORT=false
unset AUTOTEST_SUPPORT
unset A2DP_SINK_SUPPORT
unset NET_SUPPORT

ohos_out_board=2700bp
BSP_PERL=${BSP_ROOT}/tools/fill_sec_base.pl

## none-do nothing single-patch with single flash dual-patch with dual flash
FLASH_PATCH=none
export FLASH_DUAL_CHIP=1
REVERSE_OPT=
eval set -- $(getopt -q -o ab:cC:E:fhRsBVo:O:pPTw:W:N --long help,bsp,sink -- "$@")

help()
{
    echo "Usage: $0 [-a] [-b 1502x|1600|2003] [-c] [-C bit-depth] [-f] [-s] [-B] [-T] [-V] [-h] [-w] [-W size] [-N] [-P|-p] [-O panel_type] [--sink]"
    echo "    -a enable auto-test feature."
    echo "    -b 1502x, compile for target 1502x"
    echo "    -b 1600, compile for target 1600"
    echo "    -b 2003, compile for target 2003"
    echo "    -c only clean"
    echo "    -C color-bit-depth, from among 16,24,32(default)"
    echo "    -E enable boot logo(true) or not(false), disabled by default."
    echo "    -f force to compile"
    echo "    -h print this help information"
    echo "    -R reverse bsp patch"
    echo "    -s compile only for sensor hub"
    echo "    -B compile only for BTH"
    echo "    -O choose panel between rm69330 and sh8601z, default with rm69330"
    echo "    -p(lowercase) patch with 1600 single flash"
    echo "    -P(capital) patch with 1600 dual flash, default with dual flash"
    echo "    -T 1502x evb 1.3 support touch, default without this option"
    echo "    -V compile verbosely"
    echo "    -w best2003_type AX4D/AX4F/GX6D"
    echo "    -W size, enable winbond psram support"
    echo "    -N enable net support"
    echo "    --sink enable a2dp sink feature, which would disable a2dp source feature. default without this option"
}

## compile bth image and then copy bin/elf/map to product release_bin folder
## compile_bth allclean, clean bth generated objects
## compile_bth V=1, compile bth with verbose
compile_bth()
{
    echo "compile bth image"
    #(cd ${BSP_ROOT} && make T=best1600_watch_bth COMPILE_TARGET=BTH_AS_SUBSYS -j12 $1)
    if [ x"${A2DP_SINK_SUPPORT}" = x"1" ];then
    (cd ${BSP_ROOT} && make T=best1600_watch_bth COMPILE_TARGET=BTH_AS_SUBSYS AUDIO_QUEUE_SUPPORT=1 BTH_SUBSYS_ONLY=0 BT_SOURCE=0 A2DP_AAC_ON=0 -j12 $1)
    else
    (cd ${BSP_ROOT} && make T=best1600_watch_bth COMPILE_TARGET=BTH_AS_SUBSYS -j12 $1)
    fi

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
      export BES1502x_EVB13=true;
      if [ x"${touch_patched}" != x"1" ];then
        ${SCRIPT_DIR}/repo-patch.sh ${bes1502x_evb13_touch_patch}
        [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.touch_patch; }
      fi
	else
    export BES1502x_EVB13=false;
	  if [ x"${touch_patched}" = x"1" ];then
        ${SCRIPT_DIR}/repo-patch.sh ${bes1502x_evb13_touch_patch} -R
        [ $? -ne 0 ] || { echo 0 > ${SCRIPT_DIR}/../out/.touch_patch; }
      fi
	fi
}

prj_change_branch()
{
    (cd $1;git fetch -j12 -q;
    cur_node=`git branch | awk -F "[() ]" '/*/ {print $(NF-1)}'`
    [ x"${cur_node}" = x"$2" ] || { git checkout $2; }
    )
}
ohos_sdk_tags()
{
    [ -e ${SCRIPT_DIR}/../out ] || { mkdir ${SCRIPT_DIR}/../out; }
    touch ${SCRIPT_DIR}/../out/.prj_branched
    prj_branched=`cat ${SCRIPT_DIR}/../out/.prj_branched`
    if [ x"${prj_branched}" = x'1' ];then
        return 0;
    fi
    for node in ${project_nodes[*]};do
        prj=${node%/*}
        nd=${node##*/}
        if [ x"${BSP_NODE}" = x'master' ];then
            if [[ ${prj} = ${BSP_PATH} || ${prj} = ${BTHOST_PATH} || ${prj} = ${MEDIA_PATH} || ${prj} = ${BSP_PATH}/thirdparty ]];then
                nd=master
            fi
        fi
#        prj_change_branch ${prj} ${nd}
    done
    [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.prj_branched; }
}

bsp_patch_reverse()
{
    if [ "$1" = x"1" ];then
        (cd ${BSP_PATH} && git apply ${ohos2master_bsp_patch} -R )
        [ $? -ne 0 ] || { echo 0 > ${SCRIPT_DIR}/../out/.bsp_patch; }
    fi
    if [ "$2" = x"1" ];then
        #(cd ${BTHOST_PATH} && git apply ${ohos2master_bthost_patch} -R )
        [ $? -ne 0 ] || { echo 0 > ${SCRIPT_DIR}/../out/.bthost_patch; }
    fi
    if [ "$3" = x"1" ];then
      (cd ${MEDIA_PATH} && git apply ${ohos2master_multimedia_patch} -R )
      [ $? -ne 0 ] || { echo 0 > ${SCRIPT_DIR}/../out/.media_patch; }
    fi
    echo 0 > ${SCRIPT_DIR}/../out/.prj_branched
}

bsp_patch_going()
{
    if [ "$1" != x"1" ];then
      (cd ${BSP_PATH} && git apply ${ohos2master_bsp_patch} )
      [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.bsp_patch; }
    fi

    if [ "$2" != x"1" ];then
      #(cd ${BTHOST_PATH} && git checkout ${BTHOST_NODE})
      [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.bthost_patch; }
    fi

    if [ "$3" != x"1" ];then
      (cd ${MEDIA_PATH} && git apply ${ohos2master_multimedia_patch} )
      [ $? -ne 0 ] || { echo 1 > ${SCRIPT_DIR}/../out/.media_patch; }
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

    if [ x'${BSP_NODE}' != x'master' ];then
        ohos_sdk_tags;
    fi

    if [ x"$1" = x"-R" ];then
      bsp_patch_reverse x${bsp_patched} x${bthost_patched} x${media_patched}
    else
      bsp_patch_going x${bsp_patched} x${bthost_patched} x${media_patched}
    fi
}

apply_auto_test()
{
    if [ x"${AUTOTEST_SUPPORT}" != x"true" ];then
      AUTOTEST_SUPPORT=false
    fi
    sed -i "s#\"auto_test\": .*#\"auto_test\": \"${AUTOTEST_SUPPORT}\",#g" vendor/bestechnic/${PRODUCT}/config.json
}

apply_winbond_psram()
{
    if [ x"${WINBOND_PSRAM_SUPPORT}" != x"true" ];then
      WINBOND_PSRAM_SUPPORT=false
    else
      sed -i "s#\"psram_size\": .*#\"psram_size\": `echo $((${WINBOND_PSRAM_SIZE}))`,#g" vendor/bestechnic/${PRODUCT}/config.json
    fi
    sed -i "s#\"winbond_psram_support\": .*#\"winbond_psram_support\": \"${WINBOND_PSRAM_SUPPORT}\",#g" vendor/bestechnic/${PRODUCT}/config.json
}

apply_wifi_test()
{
    if [ x"${NET_SUPPORT}" != x"true" ];then
      NET_SUPPORT=false
    fi
    sed -i "s#\"wifi_test\": .*#\"wifi_test\": \"${NET_SUPPORT}\",#g" vendor/bestechnic/${PRODUCT}/config.json
}

## parse all the parameters and change setting to compile
while [ -n "$1" ]
do
  case $1 in
    -a)
      AUTOTEST_SUPPORT=true
    ;;
    -b)
    if [ x"$2" == x"1502x" ];then
      unset FLASH_DUAL_CHIP
      PRODUCT=1502x_display_demo
      LOG_FILE=bsp_build_make_best1502x.log
      ohos_out_board=2700ibp
    elif [ x"$2" == x"2003" ];then
      PRODUCT=2003_display_demo
      ohos_out_board=v200zr
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
    -C)
      COLOR_BIT_DEPTH=$2
    shift
    ;;
    -O)
      LCD_SUPPORT=$2
    shift
    ;;
    -E)
      BOOT_LOGO_SUPPORT=$2
      shift
    ;;
    -f)
    FORCE=-f
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
    -w)
    ACTION=BES2600WM;
    best2003_type=$2
    shift
    ;;
    -W)
    WINBOND_PSRAM_SUPPORT=true;
    export WINBOND_PSRAM_SIZE=$2
    shift
    ;;
    -N)
    NET_SUPPORT=true
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
    --sink)
    export A2DP_SINK_SUPPORT=1
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

    if [ x"${PRODUCT}" = x"1502x_display_demo" ];then
      apply_touch_patch ${TOUCH_PATCH}
      sed -i "s#\"lcd_support\": .*#\"lcd_support\": \"${LCD_SUPPORT}\",#g" vendor/bestechnic/${PRODUCT}/config.json
      sed -i "s#\"boot_logo_support\": .*#\"boot_logo_support\": \"${BOOT_LOGO_SUPPORT}\",#g" vendor/bestechnic/${PRODUCT}/config.json
    fi

    apply_auto_test
    apply_wifi_test
    apply_winbond_psram;

    if [ -z ${COLOR_BIT_DEPTH} ];then
        sed -i "s#define COLOR_DEPTH.*#define COLOR_DEPTH 32#g" vendor/bestechnic/${PRODUCT}/graphic_config/product_graphic_lite_config.h
        unset COLOR_BIT_DEPTH;
    else
        export COLOR_BIT_DEPTH=${COLOR_BIT_DEPTH}
        sed -i "s#define COLOR_DEPTH.*#define COLOR_DEPTH ${COLOR_BIT_DEPTH}#g" vendor/bestechnic/${PRODUCT}/graphic_config/product_graphic_lite_config.h
    fi
	
    ##1502x or bsp master wouldn't be patched for single/dual flash.
    [[ x"${PRODUCT}" != x"1600_display_demo" ]] || apply_flash_patch
 
    ##bsp master would be patched, otherwise reversed.
    #apply_bsp_patch ${REVERSE_OPT}
 
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
  "BES2600WM")
## select "best2003_type" in vendor/bestechnic/2003_display_demo/config.json
    if [ -z ${best2003_type} ];then
        best2003_type="GX6D"
        sed -i "/best2003_type/s/:.*/: \"${best2003_type}\",/" vendor/bestechnic/2003_display_demo/config.json
    else
        sed -i "/best2003_type/s/:.*/: \"${best2003_type}\",/" vendor/bestechnic/2003_display_demo/config.json
    fi
    echo "~~~~~select best2003_type: ${best2003_type}~~~~~"
    hb set -p ${PRODUCT} || true
    hb build ${FORCE} -p ${PRODUCT}
  ;;
  "CLEAN")
## ohos clean
  hb clean;
## bth clean
  compile_bth allclean
  ;;
esac
