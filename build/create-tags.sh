#!/bin/bash

tag_time=`date +%s`
out_node=build/project-node.sh
repo forall -c git tag test-v${tag_time}
repo forall -p -c git log -1 test-v${tag_time} --pretty=format:'%h' | sed '/^$/d' |awk 'BEGIN {print "project_nodes=("} {tmp=$2;getline;print tmp""$0 } END {print ")"}' > ${out_node}
sed -i "/^build\/.*/d" ${out_node}
