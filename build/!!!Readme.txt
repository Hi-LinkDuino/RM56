1. 编译说明
为了编译通过，编译之前BSP路径需要cherry-pick这3个patch：
bsp-src: http://172.16.40.40/c/bes-aos/bsp-src/+/61112
bthost-src: http://172.16.40.40/c/bes-aos/bthost-src/+/61113
multimedia-src: http://172.16.40.40/c/bes-aos/multimedia-src/+/61114

2. 编译命令
2700BP单flash编译：./build/compile.sh -b 1600 -m -p -f
2700BP双flash编译：./build/compile.sh -b 1600 -m -P -f
2700iBP编译：./build/compile.sh -b 1502x -m -f