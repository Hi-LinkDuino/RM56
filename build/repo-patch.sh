#!/bin/bash

## Script to patch up diff reated by `repo diff`

if [ -z "$1" ] || [ ! -e "$1" ]; then
    echo "Usages: $0 repo_diff_file";
    exit 0;
fi

[ x"$2" != x"-R" ] || RESTORE=$2

rm -f _tmp_splits*
csplit -qzf '' -b "_tmp_splits.%d.diff" $1 '/^project .*\/$/' '{*}'

working_dir=`pwd`

for proj_diff in `ls _tmp_splits.*.diff`
do 
    chg_dir=`awk '/project/ {print $2}' $proj_diff`
    echo "FILE: $proj_diff $chg_dir"
    if [[ ! -z "$chg_dir" && -e $chg_dir ]]; then
        ( cd $chg_dir && grep -v "^project.*\/$" ${working_dir}/${proj_diff} | git apply -v ${RESTORE}; );
        #( cd $chg_dir && grep -v "^project.*\/$" ${working_dir}/${proj_diff} | patch -Np1 ${RESTORE}; );
    else
        echo "$0: Project directory $chg_dir don't exists.";
    fi
    rm -f $proj_diff 
done
