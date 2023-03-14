#!/bin/bash

# argv: <T>
#    -T: ota_boot1 | ota_boot2a | a7_dsp | best2600w_liteos

# called in //device/soc/bestechic/bes2600/BUILD.gn:bfd_pack_flash

# cwd=out/<T>
cwd=$(dirname $(readlink -f "$0"))
# echo cwd=$cwd

if [ "$T" != "" ]; then
#called in //bsp/Makefile for IMAGE_BIN: ota_boot1/ota_boot2a
   bspdir=${cwd}/..
   # echo T=$T
   case "$T" in
   ota_boot1 | ota_boot2a)
      # chip == best2003
      prefix=""
      if [ "$BEST2003_TYPE" != "" ]; then
         prefix+="_${BEST2003_TYPE}"
      fi
      if [ "$SECURE_SEL" != "" ]; then
         prefix+="_${SECURE_SEL}"
      fi
      cp -rf $bspdir/out/${T}/${T}.bin $bspdir/prebuild/${T}${prefix}.bin
      ;;
   a7_dsp) ;;
   sensor_hub_watch)
      # chip == best1502x
      cp -rf $bspdir/out/${T}/${T}.bin $bspdir/prebuild/
      ;;
   best1600_watch_bth)
      # chip == best1600
      cp -rf $bspdir/out/${T}/${T}.bin $bspdir/prebuild/
      ;;

   *)
      echo XXXXXXXXXXXXT=$T
      exit -1
      ;;
   esac

   exit 0
fi

chip=$(hb get chip)

case "${chip}" in
  "best2003")   T=best2600w_liteos ;;
  "best1600")   T=ohos_${chip}_watch ;; #T=ohos_${bes_chip}
  "best1502x")  T=ohos_${chip} ;;
  *)            echo Unsupported CHIP TYPE: ${chip}; exit -1 ;;
esac

productOut=$(hb get out)
productName=$(hb get name)
distGUI=$(hb get dist_gui)
devMode=$(hb get dev_mode)


echo T=${T}

bspdir=$(hb get bsp)
bspout=$bspdir/out/$T
log=$bspout/dist.log
if [ "$devMode" == "prod" ]; then
   log=$bspdir/dist.log
fi
# log=./dist.log
echo Running dist tool for $T ... > $log

CHIP=""
BEST2003_TYPE=""
SECURE_SEL=""
OTA=0

buildinfo=${bspout}/.buildinfo
if [ "$devMode" == "prod" ]; then
   buildinfo=${bspdir}/prebuild/.buildinfo
fi
if [ ! -f $buildinfo ]; then
   echo [DIST][ERROR] $buildinfo not exists. >> $log && exit -1
fi
while read -r LINE; do
   k=${LINE%=*}
   v=${LINE#*=}
   case "$k" in
   CHIP) CHIP=$v ;;
   BEST2003_TYPE) BEST2003_TYPE=$v ;;
   BFD_OTA_TYPE) OTA=$v ;;
   FLASH_TYPE) test "$v" == "0" && FLASH_SEL=16m_flash ;;
   SECURE_BOOT) test "$v" == "1" && SECURE_SEL=sec ;;
   FLASH_BASE) flashBase=$v ;;
   FLASH_SIZE) flashSize=$v ;;
   LFS_FLASH_BASE_ADDR) fsOffset=$v ;;
   FACTORY_SECTION_SIZE) factorySize=$v ;;
   esac
done < $buildinfo

echo CHIP=$CHIP >> $log
if [ "$CHIP" == "" ]; then
   echo "[DIST][ERROR] No CHIP defined in $buildinfo." >> $log && exit -1
fi

echo BEST2003_TYPE=$BEST2003_TYPE >> $log
echo SECURE_SEL=$SECURE_SEL >> $log

prefix=""
if [ "$BEST2003_TYPE" != "" ]; then
   prefix+="_${BEST2003_TYPE}"
fi
if [ "$SECURE_SEL" != "" ]; then
   prefix+="_${SECURE_SEL}"
fi


distdir=$productOut/release_bin
guidir=$productOut/write_flash_gui
signdir=$bspdir/../../../pack_tools
signdir=$(dirname $(readlink -f "$signdir/xxx"))
srcdir=$productOut/../../..
srcdir=$(dirname $(readlink -f "$srcdir/xxx"))

echo distdir=$distdir >> $log
echo bspdir=${bspdir} >> $log
echo bspout=${bspout} >> $log
echo distGUI=${distGUI} >> $log
echo guidir=${guidir} >> $log


function dist_prepare() {
   echo dist_prepare $bspout
   prebuild=$bspdir/prebuild
   boot1=$prebuild/ota_boot1${prefix}.bin
   boot2=$prebuild/ota_boot2a${prefix}.bin
   # boot1=$prebuild/ota_boot1.bin
   # boot2=$prebuild/ota_boot2a.bin

   if [ "$SECURE_SEL" == "sec" ] && [ ! -d "$signdir/" ]; then
      echo [DIST][ERROR] $signdir not exists. >> $log
      exit -1
   fi

   if [ "$OTA" != "0" ] && [ ! -f $boot1 ]; then
      echo [DIST][ERROR] $boot1 not exists, please run "tools/build.sh -x" or "tools/build.sh -a" >> $log
      exit -1
   fi
   if [ "$OTA" != "0" ] && [ ! -f $boot2 ]; then
      echo [DIST][ERROR] $boot2 not exists, please run "tools/build.sh -y" or "tools/build.sh -a" >> $log
      exit -1
   fi

   # dev mode
   if [ "$devMode" == "dev" ]; then
      [ -d $prebuild ] || mkdir -p $prebuild
      python3 $bspdir/tools/cust-dist.py $devMode $productOut >> $log
      rc=$?
      if [ "$rc" != "0" ]; then
         echo =====================================================
         cat $log
         echo [DIST][ERROR] cust-dist.py result: $rc
         exit -1
      fi
      cd $prebuild
      # rm -f ./*.lds
      cp $bspout/*.lds ./
      for f in _best*.lds; do mv {,${CHIP}}"$f"; done
      cp -rf $bspout/lib${T}.a ./
      cp -rf $bspout/.buildinfo ./
      cp -rf $productOut/bsp_*flags*.gni ./
      
      # rm -f $prebuild/dist.tgz
      # tar -cvzf $prebuild/dist.tgz * .[!.]*
      # rm -rf $prebuild/dist
      cd -
   fi
}
function sign_bin() {
   bin_name=$1

   ./gen_hash_of_bin ${distdir}/${bin_name}.bin
   cp -f ${distdir}/${bin_name}_hash.bin ${distdir}/${bin_name}.bin
}
function dist_cmd_line_package() {
   prebuild=$bspdir/prebuild
   tools=$bspdir/tools
   if [ -d $prebuild ]; then
      cp -rf $prebuild/*.bin $distdir/
      rm -rf $distdir/ota_boot*.bin
      if [ "$OTA" != "0" ]; then
         cp -rf $prebuild/ota_boot1${prefix}.bin $distdir/ota_boot1.bin
         cp -rf $prebuild/ota_boot2a${prefix}.bin $distdir/ota_boot2a.bin
      fi
   fi
   cp -rf ${buildinfo} $distdir/
   cp -rf $tools/dld $distdir/
   cp -rf $tools/select_com.bat $distdir/
   cp -rf $tools/burn*.bat $distdir/

   rm -rf $distdir/*.elf
   chmod +x $distdir/*.bat
   chmod +x $distdir/dld/*.exe

   # cp -f $bspdir/tools/*.bat $distdir/
   if [ "$SECURE_SEL" == "sec" ]; then
      # cd $signdir
      # sign_bin rtos_main
      # sign_bin ota_boot1
      # sign_bin ota_boot2a
      # sign_bin trustzone_main
      # sign_bin trustzone_mini
      cd $bspdir/tools/auto_build_tool/
      python3 besfd_genbin.py $distdir ${CHIP} $FLASH_SEL
      cd -
   fi
   echo Done >> $log

   # package
   echo Generating command line package >> $log
   [ -d $srcdir/dist ] || mkdir $srcdir/dist
   cd $distdir
   # rm -rf dist.tar.gz
   dt=$(date +"%Y%m%d_%H%M%S")
   tar -cvzf $srcdir/dist/dist_${productName}${prefix}_${dt}.tar.gz * .[!.]*
   cd -
   rm -rf $srcdir/dist/burn
   cp -rf $distdir $srcdir/dist/burn
}

function dist_gui_package() {
   echo Generating gui package >> $log
   bindir=$guidir/ota_bin
   cp -f $distdir/programmer2003.bin $guidir/
   cp -f $distdir/ota_boot1.bin $bindir/
   cp -f $distdir/ota_boot2a.bin $bindir/
   cp -f $distdir/littlefs*.bin $bindir/
   cp -f $distdir/rtos_main.bin $bindir/
   cp -f $distdir/trustzone_main.bin $bindir/
   cp -f $distdir/trustzone_mini.bin $bindir/


   dt=$(date +"%Y%m%d_%H%M%S")
   cd $guidir
   tar -cvzf $srcdir/dist/write_flash_gui_${productName}${prefix}_${dt}.tar.gz * .[!.]*
   cd - 
}

dist_prepare

dist_cmd_line_package 

if [ "$distGUI" == "true" ]; then 
   dist_gui_package
fi

echo please refer to $log for details