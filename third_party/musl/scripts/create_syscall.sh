#!/usr/bin/env sh

if [ $# -ne 2 ]; then
    echo "$0 need 2 parameters, but the actual number is $#, please check!"
    exit 1
fi

rm -f $2

cp $1 $2

sed  -n -e s/__NR_/SYS_/p < $1 >> $2 
