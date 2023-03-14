#! /bin/bash

cd /data/ndk-test

rm REPORT
touch REPORT

:<<LIC-TEST
for file in ./*
do
    if [ "$file" = "runtest" ] \
    || [ "$file" = "REPORT" ] \
    || [ "$file" = "libdlopen_dso.so" ] \
    || [ "$file" = "libtls_init_dso.so" ] \
    || [ "$file" = "libtls_align_dso.so" ] \
    || [ "$file" = "libtls_get_new-dtv_dso.so" ]
    then
        continue
    else
        /data/ndk-test/libc-test/common/runtest -w '' /data/ndk-test/libc-test/$file >> REPORT    
    fi
done
LIC-TEST

ndk_test_path="./ndk-test"
function get_result(){
	dirlist=$(ls ${ndk_test_path})
	for dirname in ${dirlist[*]}
	do
		if [ -d "${ndk_test_path}/${dirname}" ];
		then
			for filename in $(ls ${ndk_test_path}/${dirname})
			do
				# echo $filename
				cd ${ndk_test_path}/${dirname}
				var=${filename#*.}
				#echo $var
				if test $var = "so"
				then
					:
				elif test $var = "a"
				then
					:
				else
					FILE="../../REPORT"
					datetime=$(date '+%Y-%m-%d %H:%M:%S')
					echo $datetime >> $FILE
					RETURN=`./$var >> ../../REPORT 2>&1`

				fi
				cd ../..
			done
		fi
	done
}
get_result
# call script
# ./runtest-sanitize.sh





