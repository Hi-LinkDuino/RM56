#!/bin/bash

# generate fs
# Usage: tools/genfs.sh littlefs|mmcfs
#
cwd=$(dirname $(readlink -f "$0"))

##################################################
# 获取fs参数
##################################################
fs_name="$1"
[ "$fs_name" == "" ] && fs_name="littlefs"
is_littlefs=true
[ "${fs_name:0:8}" == "littlefs" ] || is_littlefs=false
if [ "$is_littlefs" == "true" ]; then
   fs_args=($(hb get fs_args))
else
   fs_args=($(hb get mmcfs_args))
fi

block_size=${fs_args[0]}
fs_size=${fs_args[1]}
fs_src_path=${fs_args[2]}

echo block_size=$block_size
echo fs_size=$fs_size
echo fs_src_path=$fs_src_path

##################################################
# 获取hb参数
##################################################
root=$(hb get root)
#默认fs.bin生成到$out/目录，log也在$out/中，fs.bin生成完毕，自动拷贝都release_bin和dist/burn中
out=$(hb get out)
[ -d $out ] || mkdir -p $out
devMode=$(hb get dev_mode)
bsp=${cwd}/..
# bsp=$(hb get bsp)
chip=$(hb get chip) 
case "${chip}" in
  "best2003")   T=best2600w_liteos ;;
  "best1600")   T=ohos_${chip}_watch ;; #T=ohos_${bes_chip}
  "best1502x")  T=ohos_${chip} ;;
  *)            echo Unsupported CHIP TYPE: ${chip}; exit -1 ;;
esac

log=${out}/genfs_${fs_name}.log
echo "GENERATING ${fs_dst_path}/${fs_name}.bin" > $log


fs_dst_path=$out
#available flash space size for lfs
fs_space_size=0
#lfs size specified in config.json, if different with fs_size, that MUST means config.json changed and not rebuild
lfs_size=0


#mySub(a,b)转换成10进制进行减法运算
function mySub() {
   a=$1
   b=$2
   if [ ${a:0:2} == "0x" ]; then
      #printf -v a 0x"%x" "$a" #十进制转16进制
      a=${a/0x/}
      a=`echo "obase=10;ibase=16;$a" | bc` #`echo "obase=10;ibase=16;${a^^}" | bc`
   fi
   if [ ${b:0:2} == "0x" ]; then
      b=${b/0x/}
      b=`echo "obase=10;ibase=16;$b" | bc`
   fi
   result=$((a-b))
   # echo a-b=$result
   # result=`echo "$a-$b" | bc`
   # echo result=$result
   return $esult
   # return $((a-b))
}

if [ "$is_littlefs" == "true" ]; then
   buildinfo=${bsp}/out/$T/.buildinfo
   if [ "$devMode" == "prod" ]; then
      buildinfo=${bsp}/prebuild/.buildinfo
      if [ ! -f $buildinfo ]; then
         echo [ERROR] $buildinfo not exists.  && exit -1
      fi
   else
      # support command line entry from hb after distributed, 
      if [ ! -f $buildinfo ]; then
         echo [GENFS][WARN] $buildinfo not exists.
         buildinfo=${srcroot}/dist/burn/.buildinfo
      fi
   fi
   echo "buildinfo = $buildinfo" >> $log

   if [ ! -f $buildinfo ]; then
      echo [GENFS][ERROR] $buildinfo not exists.  && exit -1
   fi

   # NUM_TRY=0
   # while [ ! -f $buildinfo ]; do
   #    #wait till .buildinfo created
   #    dt=$(date +"%Y%m%d_%H:%M:%S")
   #    echo "[$dt] buildinfo not exist, just wait..." >> $log
   #    if [ $NUM_TRY -ge 5 ]; then
   #       echo [ERROR] $buildinfo not exists. >> $log && exit
   #    fi
   #    sleep 1s
   #    let NUM_TRY=NUM_TRY+1
   # done

   # for LINE in $(cat $buildinfo); do
   while read -r LINE; do
      k=${LINE%=*}
      v=${LINE#*=}
      case "$k" in
      LFS_SIZE)
         lfs_size=$v
         ;;
      LFS_FLASH_TOTAL_SIZE) 
         fs_space_size=$v
         ;;
      LFS_FLASH_BASE_ADDR)
         echo $LINE >> $log
         ;;
      esac
   done < $buildinfo
   echo fs_size=${fs_size} >> $log
   echo lfs_size=${lfs_size} >> $log
   echo fs_space_size=${fs_space_size} >> $log

   if [ "$fs_size" == "0x0" ] || [ "$fs_size" == "0" ]; then
      fs_size=${lfs_size}
   else
      # specifed fs_size, MUST check with .buildinfo
      mySub $fs_size $lfs_size
      if [ $? -ne 0 ]; then
         echo [FSGEN][ERROR] You modified fs_size in config.json and not rebuild, last value is ${lfs_size} >> $log
         exit -1
      fi
   fi

   ##############################################
   # check preferred size with available size
   ##############################################
   mySub $fs_size $fs_space_size
   result=$?
   if [ $result -gt 0 ]; then
      echo "Preferred fs size($fs_size, defined in config.json) is too large. Over $result bytes than available space size($fs_space_size)"
      exit -1
   fi
fi
echo fs_size=${fs_size} >> $log
##############################################
#check fs size
##############################################
src_file_size=`du -s -b ${fs_src_path} | awk '{print $1}'`
echo "size(${fs_src_path})=${src_file_size}" >> $log
mySub $src_file_size $fs_size
result=$?
if [ $result -gt 0 ]; then
   echo "size(${fs_src_path})=${src_file_size} is too large. Over $result bytes than image size($fs_size)"
   exit -1
fi

let page_size=fs_size/block_size
echo block_size=$block_size >> $log
echo page_size=$page_size >> $log
echo image_size=$fs_size >> $log

echo ${cwd}/mklittlefs -c ${fs_src_path} -d 5 -b ${block_size} -p ${page_size} -s ${fs_size} ${fs_dst_path}/${fs_name}.bin >> $log
echo ===================== >> $log
${cwd}/mklittlefs -c ${fs_src_path} -d 5 -b ${block_size} -p ${page_size} -s ${fs_size} ${fs_dst_path}/${fs_name}.bin >> $log
echo ===================== >> $log
echo result:$? >> $log

#############################################################################
# We have to check log because
# mklittlefs just returns 0 when <fs_src_path> size is bigger than <fs_size> , 
# and print warning msg as follows
# --------------------
# lfs warn:475: No more free space 497
# data left: 1043220 
#############################################################################
r=`cat $log | grep "No more free space"`
if [ "$r" != "" ]; then
   echo $r
   exit -1
fi

[ -d "${root}/dist/burn" ] && cp -rf ${fs_dst_path}/${fs_name}.bin ${root}/dist/burn/
[ -d "${out}/release_bin" ] && cp -rf ${fs_dst_path}/${fs_name}.bin ${out}/release_bin/
rm -f ${fs_dst_path}/${fs_name}.bin
[ -d "${root}/dist/burn" ] && echo ${root}/dist/burn/${fs_name}.bin generated | tee -a $log
[ -d "${out}/release_bin" ] && echo ${out}/release_bin/${fs_name}.bin generated | tee -a $log

exit 0