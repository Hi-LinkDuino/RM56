#!/bin/bash

#Usage: build.sh CT=xxx LCD_SUPPORT=xxx BEST2003_TYPE=xxx
#CT=ohos_best1502x/ohos_best1600_watch/best2600w_liteos/best2600w_aiot(默认)
#LCD_SUPPORT：for 1600(1502x?)
#BEST2003_TYPE：for best2600(2003)
#这些都需要放到pre_handle_global_opt里解析

branch=`git rev-parse --abbrev-ref HEAD`

# echo branch:${branch}
cwd=$(dirname $(readlink -f "$0"))

function bsp_ct_type(){
  bes_chip=$(hb get chip)

  case "${bes_chip}" in
    "best2003")   CT=best2600w_liteos ;;
    "best1600")   CT=ohos_${bes_chip}_watch ;; #CT=ohos_${bes_chip}
    "best1502x")  CT=ohos_${bes_chip} ;;
    *)            echo Unsupported CHIP TYPE: ${bes_chip}; exit -1 ;;
  esac
  echo $CT
}
CT=$(bsp_ct_type)

function bsp_build_args(){
  fs_size=$(hb get fs_size)
  lcd_support=$(hb get lcd)
  bes_chip=$(hb get chip)

  if [ "$bes_chip" == "best2003" ]; then
    best2003_type=$(hb get best2003_type)
  fi

  ARGS="LFS_SIZE=${fs_size}"

  if [ "$lcd_support" != "" ]; then
    ARGS="$ARGS LCD_SUPPORT=${lcd_support}"
  fi

  if [ "$best2003_type" != "" ]; then
    ARGS="$ARGS BEST2003_TYPE=${best2003_type}"
  fi
  echo $ARGS
}
ARGS=$(bsp_build_args)

################################ functions list begin #########################
declare -A targets
targets=([dsp]="a7_dsp all lst" [cp]="cmcp" [mcu]="best2600w_aiot" [b1]="ota_boot1 all lst" [b2]="ota_boot2a all lst")
targets[mcu]=$CT
case "$CT" in
   ohos_best1502x) 
      # targets[cp]="cp_subsys all lst"
      targets[dsp]="sensor_hub_watch CHIP=best1502x FORCE_TO_USE_LIB=1"
      ;;
   ohos_best1600)
      targets[mcu]="${CT}_watch"
      ;;
   ohos_best1600_watch) 
      targets[dsp]="best1600_watch_bth COMPILE_TARGET=BTH_AS_SUBSYS FORCE_TO_USE_LIB=1"
      ;;
esac


SPACE=" "
GLOBAL_OPTS=""
RELEASE_SDK=0
CMD_LIST=()
CMD_NUM=0
let DEFAULT_BUILD_JOBS=8
let GUYS_BUILD_NUM=5
# Set variables according to real environment
# on ubuntu
dev_env=`uname | sed 'y/LINUX/linux/'`
if [ "x$dev_env" == "xlinux" ]; then
    let JOBS=`grep ^processor /proc/cpuinfo|wc -l`
else
    error "Unsupported Build System : $dev_env"
fi
echo -e "machine support JOBS: ${JOBS}"
let JOBS=(JOBS+GUYS_BUILD_NUM-1)/GUYS_BUILD_NUM
if [ $DEFAULT_BUILD_JOBS -gt $JOBS ] ;then
   let JOBS=$DEFAULT_BUILD_JOBS
fi
echo -e "build in machine use JOBS: ${JOBS}"

####### each project need to config project #########

error () {
    set +u
    echo "$0: error: $@" >&2
    exit -1
    set -u
}

build_cmd_echo="\n"
tmp_path=$PATH
function build_result()
{
   eval cmd="\${$1}"
   cmd=`echo $cmd`

   toolchain_path=`which arm-none-eabi-gcc`
   toolchain_info="\n=== build with toolchain [$toolchain_path] ===\n"
   PATH=$tmp_path
   if [ "x$2" != "x0" ];then
      echo -e "$toolchain_info"
      echo -e "\033[41m[build err]\033[0m ${cmd}"
      exit -1
   fi
   build_cmd_echo+=$toolchain_info
   build_cmd_echo+="${cmd}\n\n"
}

SHOW_OPTIONS=0
GEN_OPTIONS=0
GNI_OPTIONS=0
FORCE=0
#pre-handle option of every target,for now GEN_LIB and SDK
FSGEN=0
BUILDINFO=0

function pre_handle_global_opt()
{
   arg="$1"
   if [ $arg == "fsgen" ]; then
      FSGEN=1
      return
   fi 
   if [ $arg == "showflags" ]; then
      SHOW_OPTIONS=1
      return
   fi 
   if [ $arg == "genflags" ]; then
      GEN_OPTIONS=1
      return
   fi 
   if [ $arg == "gniflags" ]; then
      GNI_OPTIONS=1
      return
   fi 
   GLOBAL_OPTS+="$SPACE$1"
   printf "pre_handle_global_opt $GLOBAL_OPTS \n"
}

#如果有编译boot1和boot2，则需拷贝到prebuild目录下重新命名保存
function best2003_post_build() {
   bspdir=${cwd}/..
   outdir=$bspdir/out/$CT

   BEST2003_TYPE=""
   SECURE_SEL=""

   buildinfo=${outdir}/.buildinfo
   if [ ! -f $buildinfo ]; then
      echo [ERROR] $buildinfo not exists. >> $log && exit -1
   fi
   while read -r LINE; do
      k=${LINE%=*}
      v=${LINE#*=}
      case "$k" in
      SECURE_BOOT) test "$v" == "1" && SECURE_SEL="sec" ;;
      BEST2003_TYPE) BEST2003_TYPE="$v" ;;
      esac
   done < $buildinfo
   prefix=""
   if [ "$BEST2003_TYPE" != "" ]; then
      prefix+="_${BEST2003_TYPE}"
   fi
   if [ "$SECURE_SEL" != "" ]; then
      prefix+="_${SECURE_SEL}"
   fi

   if [ -f "$bsp/out/ota_boot1/ota_boot1.bin" ]; then
      cp -rf $bsp/out/ota_boot1/ota_boot1.bin $bsp/prebuild/ota_boot1${prefix}.bin
   fi
   if [ -f "$bsp/out/ota_boot2a/ota_boot2a.bin" ]; then
      cp -rf $bsp/out/ota_boot2a/ota_boot2a.bin $bsp/prebuild/ota_boot2a${prefix}.bin
   fi
}
function mcu_post_build(){
   case "$CT" in
      ohos_best1502x) 
         ;;
      ohos_best1600)
         ;;
      ohos_best1600_watch)
         mkdir out/ohos_best1600
         cp -rf out/ohos_best1600_watch/libohos_best1600_watch.a out/ohos_best1600/libohos_best1600.a
         cp -rf out/ohos_best1600_watch/_best1600.lds out/ohos_best1600/
         cp -rf out/ohos_best1600_watch/.buildinfo out/ohos_best1600/
         ;;
      best2600w_liteos|best2600w_aiot)
         best2003_post_build      
         ;;
   esac
}

function build_cmd()
{
   target=$1
   T=${targets[$target]}
   # echo T=${T}
   if [ "x${T}" = "x" ] ;then
      echo -e "$toolchain_info"
      echo -e "\033[41m[build err]\033[0m unsupported target:$target"
      exit -1
   fi
   mk="make"

   build_target="${mk} T=${T} $ARGS -j${JOBS}$GLOBAL_OPTS DEV=1"
   if [ "x${SHOW_OPTIONS}" == "x1" ] ;then
      build_target+=" showflags"
   fi
   if [ "x${GEN_OPTIONS}" == "x1" ] ;then
      build_target+=" genflags"
   fi
   if [ "x${GNI_OPTIONS}" == "x1" ] ;then
      build_target+=" gniflags"
   fi
   if [ "x${FSGEN}" == "x1" ] ;then
      build_target+=" fsgen"
   fi
   if [ "x${BUILDINFO}" == "x1" ] ;then
      build_target+=" buildinfo"
   fi
   if [ "${target}_${GEN_OPTIONS}_${GNI_OPTIONS}_${FSGEN}_${BUILDINFO}" == "mcu_0_0_0_0" ] && [ $CT != "best26002_aiot" ] ;then
      build_target+=" lib"
   fi

   # if [ "x${FORCE}" = "x1" ] ;then
   #    build_target+=" FORCE=1"
   # fi   
   printf "\n[BUILD][$target] $build_target\n"

   log=out/log/bsp_build_${target}.log
   rm -rf $log
   if [ "${branch}" == "master" ]&&[ "x$target" == "xmcu" ] ;then
      # rm -f out/${T}/.release
      rm -f .release
      echo -e "REMOVING release"
   fi
   mkdir -p out/log
   ${build_target} 2>&1 | tee $log
   rc=$?
   cat $log | awk 'END {print}' | grep "Error" > /dev/null
   #build error, but returns 0, have to checkout log to set the right result code
   if [ "x$rc" == "x0" ];then
      rr=`cat $log | awk 'END {print}' | grep "Error"`
      if [ "$rr" != "" ]; then
         rc=-1
      fi
   fi
   # echo "rc = $rc"
   echo "${build_target}" | tee -a $log
   echo "MAKE RESULT: $rc" | tee -a $log
   build_result build_target $rc

   if [ "${target}_${GEN_OPTIONS}_${GNI_OPTIONS}_${FSGEN}_${BUILDINFO}" = "mcu_0_0_0_0" ] ;then
      mcu_post_build
   fi
}

function print_all_cmds()
{
   echo -e "\n~~~ build success! show all build command list ~~~\n"
   echo -e "${build_cmd_echo}"
   echo -e "~~~ build success! show all build command over  ~~~\n"
}

function realse_sdk()
{
   #release 手工添加的模块（本次未参与编译）和out/${T}/.release文件的modules
   modules=("utils/hsm")

   if [ "x$RELEASE_SDK" == "x1" ]; then
      relase_cmd="tools/relsw_$1 $1 WORKING"
      echo -e "[realse_sdk] $relase_cmd\n"
      $relase_cmd
   fi
}

usage ()
{
#-f|--force             force to compile

cat<<EOF
Usage:
tools/build.sh [command]
tools/build.sh [options] [K-V variables] [modules]

[command]
----------------------------------------
help              Show this page
buildinfo         Generate out/best2600w_liteos/buildinfo used for burning
genflags          Generate compile options to files used for 3rd parties
showflags         Print compile options
fsgen             Build littlefs image for mcu, burned to flash
clean             Remove most generated files
allclean          Remove all generated files and the output directory if possible


[options]
----------------------------------------
-a|--all          Compile boot1/boot2a/a7_dsp/m33
-x|--b1           Compile ota_boot1
-y|--b2           Compile ota_boot2a
-d|--dsp          Compile a7_dsp(default)
-m|--mcu          Compile m33 mcu(default)


[K-V vaiables]    [FLASH_TYPE=<f>] [DEBUG=<d>] [<k>=<v>] ... [<k>=<v>]
----------------------------------------
FLASH_TYPE=<f>    0 - 16M norflash with 8M psram (default)
                  1 - 32M norflash with 32M psram
SECURE_BOOT=<s>   0 - No supports for secure boot (default)
                  1 - Support secure boot
DEBUG=<d>         0 - release version
                  1 - debug version (default)
[<k>=<v>]         Any build variables you want to assign.


[modules]         [module] [module] ... [module]
----------------------------------------
For example:
tools/build.sh genflags
tools/build.sh fsgen        # build fs image: out/best2600w_liteos/fs.bin
tools/build.sh              # build for 32M norflash with 32M psram
                            # output：out/best2600w_liteos/best2600w_liteos.bin
tools/build.sh FLASH_TYPE=0 # build for 16M norflash with 8M psram
tools/build.sh FLASH_TYPE=1 # build for 32M norflash with 32M psram
tools/build.sh -m apps/myapp MYFLAG=1      # build apps/myapp/

EOF
exit 1
}
################################ functions list end   #########################

CMD=""
GETOPT_ARGS=`getopt -o foadmxyhr -al force,options,all,dsp,mcu,b1,b2,help,release -- "$@"`

#echo GETOPT_ARGS=$GETOPT_ARGS
eval set -- "$GETOPT_ARGS"

BUILD_ALL=0
while [ -n "$1" ]
do
   #printf "arg: $1\n"
   case "$1" in
      -a|--all)         BUILD_ALL=1; shift 1;;
      #   -c|--cmcp)    CMD_LIST[CMD_NUM]="cp"; let CMD_NUM=CMD_NUM+1;              shift 1;;
      -d|--dsp)         CMD_LIST[CMD_NUM]="dsp"; let CMD_NUM=CMD_NUM+1;              shift 1;;
      -m|--mcu)         CMD_LIST[CMD_NUM]="mcu"; let CMD_NUM=CMD_NUM+1;              shift 1;;
      #   -s|--se)        CMD_LIST[CMD_NUM]="se"; let CMD_NUM=CMD_NUM+1;              shift 1;;
      #   -l|--lmini)     CMD_LIST[CMD_NUM]="mini"; let CMD_NUM=CMD_NUM+1;              shift 1;;
      -x|--b1)          CMD_LIST[CMD_NUM]="b1"; let CMD_NUM=CMD_NUM+1;              shift 1;;
      -y|--b2)          CMD_LIST[CMD_NUM]="b2"; let CMD_NUM=CMD_NUM+1;              shift 1;;
      #   -z|--zboot2b)   CMD_LIST[CMD_NUM]="boot2b"; let CMD_NUM=CMD_NUM+1;              shift 1;;
        -h|--help|help)      usage; exit;;
      #   -r|--release)   CMD_LIST[CMD_NUM]="release"; let CMD_NUM=CMD_NUM+1;              shift 1;;
        --)                                                    shift 1;;
        -o|--options)   GEN_OPTIONS=1;                         shift 1;; #生成options，此时submods编译无效
        -f|--force)     pre_handle_global_opt FORCE=1;         shift 1;; #强制编译
        fsgen)          FSGEN=1;CMD_LIST[CMD_NUM]="mcu"; let CMD_NUM=CMD_NUM+1;              shift 1;;
        buildinfo)      BUILDINFO=1;CMD_LIST[CMD_NUM]="mcu"; let CMD_NUM=CMD_NUM+1;          shift 1;;
        genflags)       GEN_OPTIONS=1;CMD_LIST[CMD_NUM]="mcu"; let CMD_NUM=CMD_NUM+1;          shift 1;;
        gniflags)       GNI_OPTIONS=1;CMD_LIST[CMD_NUM]="mcu"; let CMD_NUM=CMD_NUM+1;          shift 1;;
        *)              pre_handle_global_opt $1;              shift 1;;
    esac
done

mkdir -p out/prebuild

if [ $BUILD_ALL -eq 1 ]; then
   case "$CT" in
      ohos_best1502x) CMD_LIST=("dsp" "mcu"); CMD_NUM=2 ;;
      ohos_best1600|ohos_best1600_watch) CMD_LIST=( "dsp" "mcu"); CMD_NUM=2 ;;
      best2600w_aiot|best2600w_liteos) CMD_LIST=("b1" "b2" "dsp" "mcu"); CMD_NUM=4 ;;
   esac
else 
if [ $CMD_NUM -eq 0 ]; then
   case "$CT" in
      ohos_best1502x) CMD_LIST=("dsp" "mcu"); CMD_NUM=2 ;;
      ohos_best1600|ohos_best1600_watch) CMD_LIST=( "dsp" "mcu"); CMD_NUM=2 ;;
      best2600w_aiot|best2600w_liteos) CMD_LIST=("b1" "b2" "dsp" "mcu"); CMD_NUM=4 ;;
   esac
fi
fi
echo "CMD_NUM=${CMD_NUM}"
echo "CMD_LIST=${CMD_LIST[@]}"



for ((index=0; index<$CMD_NUM; index++))
do
  cmd=${CMD_LIST[index]}
#   echo "cmd[$index]=$cmd"
  case "x$cmd" in
    xcp)       build_cmd cp;;
    xdsp)      build_cmd dsp;;
    xmcu)      build_cmd mcu;;
    xb1)       build_cmd b1;;
    xb2)       build_cmd b2;;
    xhelp)     usage;;
    *)         echo "Unsupported command:$1";;
  esac


done

print_all_cmds

exit 0