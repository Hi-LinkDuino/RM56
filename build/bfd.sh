#!/bin/bash

#Usage
# exec "source hb.sh" in your shell command line, then you can use the following command in shell

help(){
	echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	echo "+ hb extension commands                                                          +"
	echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	echo "hb get xxx|?                get product info with specific key, ? for help"
	echo "build"
	echo "     build flags            build bsp flags for oh"
	echo "     build bsp              only build bsp"
	echo "     build                  build flags/bsp/oh/littlefs/mmcfs, and distribution"
	echo "clean"
	echo "     clean bsp              clean bsp/out"
	echo "     clean oh               clean out/xxx/product/"
	echo "     clean                  clean both bsp and oh"
	echo "go                          cd to specific path"
	echo "     go root                goto source root path"
	echo "     go bsp                 goto bsp path"
	echo "     go bsp/out             goto bsp out path"
	echo "     go out                 goto out/xxx/product/"
	echo "     go prod                goto verdor/xxx/product"
	echo "     go back                go back to last path"
	echo "lfs                         generate littlefs image specially after distribution"
	echo "mmcfs                       generate mmcfs image specially after distribution"
	echo "addr2line                   read addr from elf with -g"
	echo "asm                         generate asm from elf"
	echo "dist                        distribution: dist/burn"
	echo "custdist                    Only for internal use, package private sources"
	echo
}
build(){
	bsp=$(hb get bsp)
	if [ "$1" == "flags" ]; then
		$bsp/../tools/bfd_bspflags.sh
		out=$(hb get out)
		if [ $? -eq 0 ]; then
			echo bsp_cppflags.gni/bsp_cflags.gni generated in $out
		else
			cat $bsp/out/flags.log
		fi
		return
	fi
	if [ "$1" == "bsp" ]; then
		cd $bsp
		tools/build.sh
		cd -
		return
	fi
	hb build $@
}

clean(){
	bsp=$(hb get bsp)
	if [ "$1" == "bsp" ]; then
		rm -rf ${bsp}/out
		return
	fi
	root=$(hb get root)
	if [ "$1" == "oh" ]; then
		# hb clean
		rm -rf ${root}/out
		return
	fi
	# clean all
	rm -rf ${bsp}/out
	rm -rf ${root}/out
	# hb clean
}

# go bsp                to bsp dir
# go p|prod             to product dir
# go b|board            to device board dir
# go back               back to last dir

go(){
	path=$(hb get $1)
	if [ "$path" != "" ]; then
		cd $path
	fi
}

addr2line(){
	arm-none-eabi-addr2line -pfiCe $(hb get elf) $@
}

asm(){
	echo Generating $(hb get out)/$(hb get name).asm from $(hb get elf)

	arm-none-eabi-objdump -l -S $(hb get elf) > $(hb get out)/$(hb get name).asm

	echo Done
}

# generate littlefs after distribution
# tools/genfs.sh ${block_size} ${fs_size} ${fs_src} ${out_product_path}/release_bin ${fs_name} ohos_${bes_specific_chip} ${bfd_dev_mode}"
# tools/genfs.sh 512 16777216 /home/alvin/work/bfd2700/vendor/bestechnic/1502x_display_demo/fs/mmc_official_ui /home/alvin/work/bfd2700/out/2700ibp/1502x_display_demo/release_bin mmcfs ohos_best1502x
lfs(){
	fs_name=littlefs
	bsp=$(hb get bsp)
	$bsp/tools/genfs.sh $fs_name
	if [ $? -ne 0 ]; then
		out=$(hb get out)
		echo [GENFS][$fs_name] failed, please refer to $out/genfs_${fs_name}.log for details
	fi
}

mmcfs(){
	fs_name=mmcfs
	bsp=$(hb get bsp)
	$bsp/tools/genfs.sh $fs_name
	if [ $? -ne 0 ]; then
		out=$(hb get out)
		echo [GENFS][$fs_name] failed, please refer to $out/genfs_${fs_name}.log for details
	fi
}

dist(){
	bsp=$(hb get bsp)
	$bsp/tools/dist.sh
}

custdist(){
	bsp=$(hb get bsp)
	$bsp/tools/cust-dist.sh $@
}


