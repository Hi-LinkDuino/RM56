#!/usr/bin/env sh

# not check args

while getopts o: opt
do
    case $opt in
        s) 
            src=$OPTARG
            ;;
        o)
            output=$OPTARG
            ;;
        ?)
            #usage
            ;;
    esac
done
shift $(($OPTIND - 1))

echo $0 "$@" $output 

sed -f $@ > $output
