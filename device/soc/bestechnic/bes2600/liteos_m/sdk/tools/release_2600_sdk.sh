#!/bin/bash

SCRIPT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)

ROOT_DIR=$1
best2003_type=$2

## compile bsp for release
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk/bsp && rm best2600w_ohos_release.tar.gz bsp_rel -rf)
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk/ && ./tools/build_lib_into_hmos_out.sh false true release ../../../../../../vendor/bestechnic/2003_display_demo/ ${best2003_type})

## rename bsp to bsp_ori and create bsp folder for releasing bsp package
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk && mv bsp bsp_ori && mkdir bsp && tar xvf best2600w_ohos_release.tar.gz -C bsp)

## patching to force use lib
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk/ && sed -i "s#GEN_LIB=1#SDK=1#g" tools/build_lib_into_hmos_out.sh)

## extract all the pointed folder for release
(cd ${ROOT_DIR}/ && ${SCRIPT_DIR}/package_release_extract_ohos.sh)

(cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk/ && sed -i "s#SDK=1#GEN_LIB=1#g" tools/build_lib_into_hmos_out.sh)
## restore bsp folder
(cd ${ROOT_DIR}/device/soc/bestechnic/bes2600/liteos_m/sdk && mv bsp bsp_rel && mv bsp_ori bsp )
