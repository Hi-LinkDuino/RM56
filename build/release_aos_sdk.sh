#!/bin/bash

SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

ROOT_DIR=$1
relsw_2600_sdk_both=$2

# default release BES2600 sdk both
if [ x"${relsw_2600_sdk_both}" == "x" ];then
    relsw_2600_sdk_both="true"
fi

if [ x"${relsw_2600_sdk_both}" == x"true" ]; then
    ## BES2600 compile bsp for release
    (cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk && rm best2600w_ohos_release.tar.gz bsp_rel -rf)
    (cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk/ && ./tools/build_lib_into_hmos_out.sh false true release ../../../../../../vendor/bestechnic/2003_display_demo/ GX6D)

    ## BES2600 rename bsp to bsp_ori and create bsp folder for releasing bsp package
    (cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk && mv bsp bsp_ori && mkdir bsp && tar xvf best2600w_ohos_release.tar.gz -C bsp)

    ## BES2600 patching to force use lib
    (cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk/ && sed -i "s#GEN_LIB=1#SDK=1#g" tools/build_lib_into_hmos_out.sh)
fi

## BES2700 compile bsp for release
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2700/liteos_m/sdk && rm *.tar.gz.sha512 *.tar.gz bsp_rel -rf)
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2700/liteos_m/sdk/bsp && sed -i "/NULL/!s#\(\&sdio_host_event_callback\)#NULL, \1#g" net/wifi/best1600/wifi_sdio_host.c) 
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2700/liteos_m/sdk/bsp && ./tools/relsw_ohos_bsp_2700.sh ../bsp_package WORKING)

## BES2700 rename bsp to bsp_ori and create bsp folder for releasing bsp package
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2700/liteos_m/sdk && mv bsp bsp_ori && mkdir bsp && tar xvf bsp_package.tar.gz -C bsp)

## BES2700 patching to force use lib
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2700/liteos_m/sdk/bsp && sed -i "s#\(MODULE_KERNEL_STUB_INC=1\)#\1 FORCE_TO_USE_LIB=1#g" tools/build_best1502x_ohos_into_lib.sh)
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2700/liteos_m/sdk/bsp && sed -i "s#\(CHIP=best1502x\)#\1 FORCE_TO_USE_LIB=1#g" tools/build_best1502x_ohos_into_lib.sh)
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2700/liteos_m/sdk/bsp && sed -i "s#\(MODULE_KERNEL_STUB_INC=1\)#\1 FORCE_TO_USE_LIB=1#g" tools/build_best1600_ohos_into_lib.sh)

## extract all the pointed folder for release
${SCRIPT_DIR}/package_release_extract_ohos.sh ${relsw_2600_sdk_both}

if [ x"${relsw_2600_sdk_both}" == x"true" ]; then
    ## BES2600 restore bsp folder
    (cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk/ && sed -i "s#SDK=1#GEN_LIB=1#g" tools/build_lib_into_hmos_out.sh)
    (cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk && mv bsp bsp_rel && mv bsp_ori bsp )
fi

## BES2700 restore bsp folder
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2700/liteos_m/sdk && mv bsp bsp_rel && mv bsp_ori bsp )
