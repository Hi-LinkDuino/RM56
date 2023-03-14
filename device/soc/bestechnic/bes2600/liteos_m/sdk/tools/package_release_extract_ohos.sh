#!/bin/bash

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

time tar -zc --exclude='.github' --exclude='.gitee' --exclude='.git' --exclude='.gitignore' --exclude='__pycache__' --exclude='out' --exclude='bsp_ori' -f sdk-2600-ohos-`date +%Y%m%d`.tar.gz ${release_files[*]}