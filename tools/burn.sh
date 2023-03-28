#!/bin/bash

cwd=$(dirname "$0")
echo cwd=$cwd

buildinfo=$cwd/.buildinfo
(-f $buildinfo) || (echo [ERROR] $buildinfo not exists. Run "tools/build.sh" && exit)


toolpath=$cwd/burn
binpath=$cwd/ota_bin
dldtool=${toolpath}/dldtool64
ota_rtos=${binpath}/ota_rtos.bin
littlefs=${binpath}/littlefs.bin
factory=${binpath}/factory.bin
ota_boot1=${binpath}/ota_boot1.bin
ota_boot2a=${binpath}/ota_boot2a.bin
pub_otp=${binpath}/pub_otp.bin

for LINE in `cat $buildinfo`  
do
  k=${LINE%=*}
  v=${LINE#*=}
  case "$k" in 
    FLASH_BASE)           flashBase=$v ;;
    FLASH_SIZE)           flashSize=$v ;;
    OTA_CODE_OFFSET)      fwOffset=$v ;;
    OTA_CODE_SIZE)        OTA_CODE_SIZE=$v ;;
    LFS_FLASH_BASE_ADDR)  fsOffset=$v ;;
    FACTORY_SECTION_SIZE) factorySize=$v ;;
    SECURE_BOOT)          SECURE_BOOT=$v ;;
  esac
done
programmer=${binpath}/programmer2003.bin
echo Using programmer: $programmer

fwAddr=$(($flashBase + $fwOffset))
echo fwAddr=$fwAddr

boot2Base=0x10000
boot2Size=0x10000
boot2Addr=$(($flashBase + $boot2Base))
fsaddr=$(($flashBase + $fsOffset))
factoryAddr=$(($flashBase + $flashSize - $factorySize - $factorySize))
factory2Addr=$(($flashBase + $flashSize - $factorySize))
echo boot2Addr=$boot2Addr
echo fsaddr=$fsaddr
echo factoryAddr=$factoryAddr


COM=/dev/ttyUSB0
progRate=3000000
cmd="$dldtool $COM --reboot --pgm-rate $progRate $programmer"

if [ "$1" == "all" ]; then 
  echo SECURE_BOOT=$SECURE_BOOT
  cmd="$cmd $ota_boot1"
  cmd="$cmd --addr $boot2Addr --remap-both $boot2Size $ota_boot2a -m $boot2Addr"
  cmd="$cmd --addr $fwAddr --remap-both $OTA_CODE_SIZE $ota_rtos -m $fwAddr"
  cmd="$cmd --addr $fsaddr $littlefs"
  # cmd="$cmd --addr $factoryAddr --remap-both $factorySize $factory"
  cmd="$cmd --addr $factoryAddr $factory"
  cmd="$cmd --addr $factory2Addr $factory"

  if ["$SECURE_BOOT" == "1"]; then
    cmd="$cmd --sec-file $pub_otp"
  fi
else
  cmd="$cmd --addr $fwAddr --remap-both $OTA_CODE_SIZE $ota_rtos -m $fwAddr"
  # cmd="$cmd --addr $fwAddr $ota_rtos -m $fwAddr"
fi

echo ${cmd}

# sudo ${cmd}
