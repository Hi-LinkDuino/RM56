#!/usr/bin/env sh

out_putdir=$3

if [ ! -d $out_putdir ]
then
    mkdir -p $out_putdir
fi

cp $1 $out_putdir/VERSION
cp $2 $out_putdir/version.sh

cd $out_putdir

version=$(sh version.sh)

printf '#define VERSION "%s"\n' $version  > version.h

rm VERSION version.sh
