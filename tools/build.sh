#!/bin/bash

branch=`git rev-parse --abbrev-ref HEAD`

# echo branch:${branch}



####### each project need to config following toolchain path #########
TOOLCHAIN_9_2_1="/home/toolchain/9.2.1/gcc-arm-none-eabi-9-2019-q4-major/bin"
TOOLCHAIN_5_4_1="/home/toolchain/ali_5.4.1/gcc-arm-none-eabi/Linux64/bin"

rm -rf .release
################################ functions list begin #########################
DEV=0
CMD_LIST_ALL=("dsp" "mcu")
SPACE=" "
GLOBAL_OPTS=""
RELEASE_SDK=0
CMD_LIST=()
CMD_NUM=0
declare -A targets
targets=([dsp]="a7_dsp" [cp]="cmcp" [mcu]="best2600w_aiot" [b1]="ota_boot1 all lst" [b2]="ota_boot2a all lst")


# Set variables according to real environment
# on ubuntu
dev_env=`uname | sed 'y/LINUX/linux/'`
if [ "x$dev_env" == "xlinux" ]; then
    JOBS=`grep ^processor /proc/cpuinfo|wc -l`
else
    error "Unsupported Build System : $dev_env"
fi

####### each project need to config project #########

error () {
    set +u
    echo "$0: error: $@" >&2
    exit 1
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
   if [ "x$2" != "x1" ];then
      echo -e "$toolchain_info"
      echo -e "\033[41m[build err]\033[0m ${cmd}"
      exit 1
   fi
   build_cmd_echo+=$toolchain_info
   build_cmd_echo+="${cmd}\n\n"
}

GEN_OPTIONS=0
FORCE=0
#pre-handle option of every target,for now GEN_LIB and SDK
FSGEN=0
BUILDINFO=0
function pre_handle_global_opt()
{
   if [ $1 == "fsgen" ]; then
      FSGEN=1
      return
   fi 
   if [ $1 == "genflags" ]; then
      GEN_OPTIONS=1
      return
   fi 

   GLOBAL_OPTS+="$SPACE$1"
   printf "pre_handle_global_opt $GLOBAL_OPTS \n"
}


function pre_handle_relase()
{
   # printf "pre_handle_relase\n "
   RELEASE_SDK=1
}

function build_cmd()
{
   T=${targets[$1]}
   # echo T=${T}
   if [ "x${T}" = "x" ] ;then
      echo -e "$toolchain_info"
      echo -e "\033[41m[build err]\033[0m unsupported target:$1"
      exit 1
   fi
   mk="make"

   build_target="${mk} T=${T} -j${JOBS}$GLOBAL_OPTS DEV=$DEV"
   if [ "x${GEN_OPTIONS}" = "x1" ] ;then
      build_target+=" genflags"
   fi
   if [ "x${FSGEN}" = "x1" ] ;then
      build_target+=" fsgen"
   fi
   if [ "x${BUILDINFO}" = "x1" ] ;then
      build_target+=" buildinfo"
   fi

   # if [ "x${FORCE}" = "x1" ] ;then
   #    build_target+=" FORCE=1"
   # fi   
   printf "\n[BUILD][$1] $build_target\n"

   ### config toolchain ###
   if [ "x$2" = "x" ] ;then
      if [ -d $M33_TOOLCHAIN ] ;then
         export PATH=$M33_TOOLCHAIN:$PATH
      fi
   else
      eval toolchain="\${$2}"
      # printf "\ntoolchain : $toolchain \n"
      if [ -d $toolchain ]; then
         export PATH=$toolchain:$PATH
      fi
   fi
   rm -rf out/log/bsp_build_$1.log
   if [ "${branch}" == "master" ]&&[ "x$1" == "xmcu" ] ;then
      # rm -f out/${T}/.release
      rm -f .release
      echo -e "REMOVING release"
   fi
   mkdir -p out/log
   ${build_target} 2>&1 | tee out/log/bsp_build_$1.log
   cat out/log/bsp_build_$1.log | awk 'END {print}' | grep "Error" > /dev/null
   rc=$?
   # echo "rc = $rc"
   echo "${build_target}" >> out/log/bsp_build_$1.log
   build_result build_target $rc
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
function prepare_bt_ramrun()
{
  # compress prebuild bins
  # bt ramrun
  cp platform/drivers/bt/best2003/best2003_ramrun_fw.bin out/prebuild/.
  tools/lzma e out/prebuild/best2003_ramrun_fw.bin out/prebuild/best2003_ramrun_fw.lzma
  mv out/prebuild/best2003_ramrun_fw.lzma out/prebuild/best2003_ramrun_fw.bin
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
buildinfo         Generate out/best2600w_aiot/buildinfo used for burning
genflags          Generate compile options to files used for 3rd parties
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
tools/build.sh fsgen        # build fs image: out/best2600w_aiot/fs.bin
tools/build.sh              # build for 32M norflash with 32M psram
                            # output：out/best2600w_aiot/best2600w_aiot.bin
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
        *)              pre_handle_global_opt $1;              shift 1;;
    esac
done

mkdir -p out/prebuild

if [ $BUILD_ALL -eq 1 ]; then
   CMD_LIST=("${CMD_LIST_ALL[@]}")
   CMD_NUM=4
fi
echo "CMD[$CMD_NUM]=${CMD_LIST}"

if [ $CMD_NUM -eq 0 ]; then
  build_cmd dsp TOOLCHAIN_9_2_1; build_cmd mcu
  exit
fi

for ((index=0; index<$CMD_NUM; index++))
do
  cmd=${CMD_LIST[index]}
#   echo "cmd[$index]=$cmd"
  case "x$cmd" in
    xcp)       build_cmd cp;;
    xdsp)      build_cmd dsp TOOLCHAIN_9_2_1;;
    xmcu)      build_cmd mcu;;
    xb1)       build_cmd b1;;
    xb2)       build_cmd b2;;
    xhelp)     usage;;
    xrealse)   pre_handle_relase;;
    *)         echo "Unsupported command:$1";;
  esac


done

# release script
realse_sdk best2600w_aiot

# print all make cmds
# print_all_cmds



