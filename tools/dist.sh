#!/bin/bash

# argv: <T>
#    -T: ota_boot1 | ota_boot2a | a7_dsp | best2600w_aiot

# called in Makefile after <T>.bin is generated
# cwd=out/<T>
cwd=$(dirname $(readlink -f "$0"))
# echo cwd=$cwd

T=$1

if [ "$T" == "" ]; then
   # T not provided when running in the source dir
   T=best2600w_aiot
fi
echo Running dist tool...
echo T=${T}

srcdir=${cwd}/..
outdir=$srcdir/out/$T

# echo srcdir=${srcdir}
# echo outdir=${outdir}

FLASH_SEL=32m_flash
SECURE_SEL=sec

buildinfo=${outdir}/.buildinfo

if [ ! -f $buildinfo ]; then
   echo [ERROR] $buildinfo not exists. Run "tools/build.sh" && exit
fi
for LINE in $(cat $buildinfo); do
   k=${LINE%=*}
   v=${LINE#*=}
   case "$k" in
   FLASH_TYPE) test "$v" == "0" && FLASH_SEL=16m_flash ;;
   SECURE_BOOT) test "$v" == "0" && SECURE_SEL=nosec ;;
   FLASH_BASE) flashBase=$v ;;
   FLASH_SIZE) flashSize=$v ;;
   LFS_FLASH_BASE_ADDR) fsOffset=$v ;;
   FACTORY_SECTION_SIZE) factorySize=$v ;;
   esac
done

echo FLASH_SEL=$FLASH_SEL
echo SECURE_SEL=$SECURE_SEL

signdir=$srcdir/tools/sign_write_tool

function dist_prepare() {
   echo dist_prepare $outdir/${T}.bin
   prebuild=$srcdir/prebuild
   boot1=$prebuild/ota_boot1_${FLASH_SEL}_${SECURE_SEL}.bin
   boot2=$prebuild/ota_boot2a_${FLASH_SEL}_${SECURE_SEL}.bin

   if [ ! -d "$signdir/" ]; then
      echo [ERROR] $signdir not exists.
      exit
   fi
   if [ ! -d "$signdir/" ]; then
      echo [ERROR] $signdir not exists.
      exit
   fi
   if [ ! -f $boot1 ]; then
      echo [ERROR] $boot1 not exists, please run "tools/build.sh -x" or "tools/build.sh -a"
      exit
   fi
   if [ ! -f $boot2 ]; then
      echo [ERROR] $boot2 not exists, please run "tools/build.sh -y" or "tools/build.sh -a"
      exit
   fi
   if [ ! -f $outdir/${T}.bin ]; then
      echo [ERROR] $outdir/${T}.bin not exists, please run "tools/build.sh"
      exit
   fi
   if [ ! -f $outdir/.buildinfo ]; then
      echo [ERROR] $outdir/.buildinfo not exists, please run "tools/build.sh"
      exit
   fi
   if [ ! -f "${outdir}/fs.bin" ]; then
      echo [ERROR] ${outdir}/fs.bin not exists, please run "tools/build.sh"
      exit
   fi

   if [ ! -f "$srcdir/dist" ]; then
      mkdir $srcdir/dist 
   fi

   cd $signdir

   if [ -d "prebuild/$FLASH_SEL/$SECURE_SEL/" ]; then
      cp -f prebuild/$FLASH_SEL/$SECURE_SEL/*.bin ./prebuild/
      cp -f prebuild/$FLASH_SEL/$SECURE_SEL/bes_sign_cfg.yaml ./release/auto_build_tool/
      cp -f prebuild/$FLASH_SEL/bes_dld_cfg_best2600w.yaml ./release/write_flash_gui/dld_cfg/
      # nosec programmer can't work after sec bin downloaded by sec programmer
      # cp -f prebuild/$FLASH_SEL/sec/programmer2003.bin ./prebuild/
   fi
   if [ -f "${outdir}/fs.bin" ]; then
      cp -f ${outdir}/fs.bin ./prebuild/littlefs.bin
   fi

   # ota_boot1.bin
   cp $boot1 ./prebuild/ota_boot1.bin
   # ota_boot2a.bin
   cp $boot2 ./prebuild/ota_boot2a.bin
}
function dist_cmd_line_package() {
   dist=$srcdir/tools

   if [ -d $dist/ota_bin ]; then
      rm -rf $dist/ota_bin
   fi
   rm -f $dist/burn/programmer2003.bin
   mkdir -p $dist/ota_bin

   cp -f $outdir/$T.bin $dist/ota_bin/ota_rtos.bin
   cp -f $signdir/prebuild/ota_boot1.bin $dist/ota_bin/
   cp -f $signdir/prebuild/ota_boot2a.bin $dist/ota_bin/
   cp -f $signdir/prebuild/littlefs.bin $dist/ota_bin/
   cp -f $signdir/prebuild/factory.bin $dist/ota_bin/
   cp -f $signdir/prebuild/programmer2003.bin $dist/ota_bin/
   cp -f $outdir/.buildinfo $dist/
   # cp -f $srcdir/tools/*.bat $dist/
   if [ $SECURE_SEL == "nosec" ]; then
      echo Done
   else
      cd $signdir/release/auto_build_tool/
      python3 besfd_genbin.py $dist/ota_bin best2003 $FLASH_SEL $SECURE_SEL || exit
      # mv -f $dist/ota_bin/programmer2003.bin $dist/burn/
   fi

   # package
   echo Generating command line package
   cd $dist
   # rm -rf dist.tar.gz
   dt=$(date +"%Y%m%d_%H%M%S")
   tar -cvzf dist_${FLASH_SEL}_${SECURE_SEL}_${dt}.tar.gz .buildinfo select_com.bat burn.bat burn-all.bat burn.sh burn ota_bin

   mv -f dist_${FLASH_SEL}_${SECURE_SEL}_${dt}.tar.gz $srcdir/dist/
}

function dist_gui_package() {
   cd $signdir/release/auto_build_tool/
   python3 bes_genbin.py best2003 $FLASH_SEL

   cd $srcdir
   if [ -f "tools/sign_write_tool/release/write_flash_gui_best2600w_$FLASH_SEL.tar.gz" ]; then
      echo Generating command line package
      cd $dist
      # rm -rf dist.tar.gz
      dt=$(date +"%Y%m%d_%H%M%S")
      mv $srcdir/tools/sign_write_tool/release/write_flash_gui_best2600w_${FLASH_SEL}.tar.gz $srcdir/dist/write_flash_gui_${FLASH_SEL}_${SECURE_SEL}_${dt}.tar.gz
   fi
}

case "$T" in
ota_boot1 | ota_boot2a)
   cp ${T}.bin $srcdir/prebuild/${T}_${FLASH_SEL}_${SECURE_SEL}.bin
   ;;
a7_dsp) ;;

best2600w_aiot)
   dist_prepare

   #release to tools/sign_write_tool/release/write_flash_gui/ and dist/write_flash_gui_xxxx.tar.gz
   dist_gui_package
   #release to tools/ & dist/dist_xxxx.tar.gz
   dist_cmd_line_package 
   ;;
esac
