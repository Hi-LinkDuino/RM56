#!/bin/bash

relsw_2600_sdk_both=$1

release_files=(
applications
base
build
build.sh
.gn
build.py
developtools
device/board/fnlink
device/soc/bestechnic/.gitattributes
device/soc/bestechnic/BUILD.gn
device/soc/bestechnic/Kconfig.liteos_m.defconfig
device/soc/bestechnic/Kconfig.liteos_m.series
device/soc/bestechnic/Kconfig.liteos_m.soc
device/soc/bestechnic/bes2700/BUILD.gn
device/soc/bestechnic/bes2700/Kconfig.liteos_m.defconfig.bes2700
device/soc/bestechnic/bes2700/Kconfig.liteos_m.defconfig.series
device/soc/bestechnic/bes2700/Kconfig.liteos_m.series
device/soc/bestechnic/bes2700/Kconfig.liteos_m.soc
device/soc/bestechnic/bes2700/liteos_m/BUILD.gn
device/soc/bestechnic/bes2700/liteos_m/components
device/soc/bestechnic/bes2700/liteos_m/sdk/bsp
device/soc/bestechnic/bes2700/liteos_m/sdk/BUILD.gn
device/soc/bestechnic/bes2700/liteos_m/sdk/tools
device/soc/bestechnic/bes2700/pack_tools
device/soc/bestechnic/hals
device/soc/bestechnic/write_flash_gui
domains
drivers
foundation
interface
kernel/liteos_m
prebuilts
productdefine
test
vendor/bestechnic
out/preloader
out/v200zr/
third_party/jinja2
third_party/bounds_checking_function
third_party/freetype
third_party/giflib
third_party/libpng
third_party/jerryscript
third_party/libjpeg
third_party/mbedtls
third_party/qrcodegen
third_party/cmsis
third_party/musl
third_party/littlefs
third_party/FatFs
third_party/lwip
third_party/cJSON
third_party/zlib
third_party/minimp3
utils/.gitattributes
utils/LICENSE
utils/OAT.xml
utils/README.md
utils/README_zh.md
utils/native
)

if [ x"${relsw_2600_sdk_both}" == x"true" ]; then
release_files+=(
device/soc/bestechnic/Kconfig.liteos_a.defconfig
device/soc/bestechnic/Kconfig.liteos_a.series
device/soc/bestechnic/Kconfig.liteos_a.soc
device/soc/bestechnic/bes2600/liteos_a
device/soc/bestechnic/bes2600/BUILD.gn
device/soc/bestechnic/bes2600/Kconfig.liteos_a.defconfig.bes2600w
device/soc/bestechnic/bes2600/Kconfig.liteos_a.defconfig.series
device/soc/bestechnic/bes2600/Kconfig.liteos_a.series
device/soc/bestechnic/bes2600/Kconfig.liteos_a.soc
device/soc/bestechnic/bes2600/Kconfig.liteos_m.defconfig.bes2600w
device/soc/bestechnic/bes2600/Kconfig.liteos_m.defconfig.series
device/soc/bestechnic/bes2600/Kconfig.liteos_m.series
device/soc/bestechnic/bes2600/Kconfig.liteos_m.soc
device/soc/bestechnic/bes2600/liteos_m/BUILD.gn
device/soc/bestechnic/bes2600/liteos_m/components
device/soc/bestechnic/bes2600/liteos_m/sdk/bsp
device/soc/bestechnic/bes2600/liteos_m/sdk/BUILD.gn
device/soc/bestechnic/bes2600/liteos_m/sdk/tools
device/soc/bestechnic/bes2600/pack_tools
)
fi

## the following code process compile.sh script for release 
sed -i 's#bes2600#bes2700#g' vendor/bestechnic/1502x_display_demo/config.json
sed -i 's#bes2600#bes2700#g' vendor/bestechnic/1600_display_demo/config.json
if [ x"${relsw_2600_sdk_both}" != x"true" ]; then
    sed -i 's#bes2600#bes2700#g' vendor/bestechnic/2003_display_demo/config.json
fi
sed -i "s#\(COMPILE_TARGET=BTH_AS_SUBSYS\)#\1 FORCE_TO_USE_LIB=1#g" build/compile.sh

## remove patch expression
start_del_line=`sed -n "/|| apply_flash_patch/=" build/compile.sh`
end_del_line=`sed -n "/patch for bsp failure/=" build/compile.sh`
start_del_line=$((start_del_line+1))
end_del_line=$((end_del_line))
sed -i "${start_del_line},${end_del_line}d" build/compile.sh

## package sdk to tar file
if [ x"${relsw_2600_sdk_both}" == x"true" ]; then
    sdk_name_prefix="sdk-2600-and"
else
    sdk_name_prefix="sdk"
fi

time tar -zc --exclude='.github' --exclude='.gitee' --exclude='.git' --exclude='.gitignore' --exclude='__pycache__' --exclude='out' --exclude='bsp_ori' -f ${sdk_name_prefix}-2700-ohos-`date +%Y%m%d`.tar.gz ${release_files[*]}


##the following code would restore the compile.sh script
sed -i "s#\(COMPILE_TARGET=BTH_AS_SUBSYS\) \(FORCE_TO_USE_LIB=1\)#\1#g" build/compile.sh

##the following code would restore the 2003_display_demo/config.json
if [ x"${relsw_2600_sdk_both}" != x"true" ]; then
    sed -i 's#bes2700#bes2600#g' vendor/bestechnic/2003_display_demo/config.json
fi

## \n:insert next line
## \ :insert space character
## \:backslash for continue current line.
##@sed -i "/hb set -p/a \ \n\
##@    ##1502x or bsp master wouldn't be patched for single/dual flash." build/compile.sh
##@sed -i "/1502x or bsp master/a \ \ \ \ [[ x\"\${PRODUCT}\" != x\"1600_display_demo\" ]] || apply_flash_patch" build/compile.sh
sed -i "/|| apply_flash_patch/a \ \n\
    ##bsp master would be patched, otherwise reversed." build/compile.sh
sed -i "/would be patched/a \ \ \ \ \#apply_bsp_patch \${REVERSE_OPT}" build/compile.sh
sed -i "/apply_bsp_patch \${REVERSE_OPT}/a \ \n\
    [ -z \"\${REVERSE_OPT}\" ] || { echo \"just only reverse bsp patched \"; exit 0; }" build/compile.sh
sed -i "/just only reverse/a \ \n\
    [ \$? -eq 0 ] || { echo \"patch for bsp failure.\"; exit 255; }" build/compile.sh
