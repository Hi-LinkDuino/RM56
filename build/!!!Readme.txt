1. ����˵��
Ϊ�˱���ͨ��������֮ǰBSP·����Ҫcherry-pick��3��patch��
bsp-src: http://172.16.40.40/c/bes-aos/bsp-src/+/61112
bthost-src: http://172.16.40.40/c/bes-aos/bthost-src/+/61113
multimedia-src: http://172.16.40.40/c/bes-aos/multimedia-src/+/61114

2. ��������
2700BP��flash���룺./build/compile.sh -b 1600 -m -p -f
2700BP˫flash���룺./build/compile.sh -b 1600 -m -P -f
2700iBP���룺./build/compile.sh -b 1502x -m -f
2600WM-${best2003_type} compile��./build/compile.sh -b 2003 -w ${best2003_type} ${FORCE}
2600WM-GX6D: ./build/compile.sh -b 2003 -w GX6D -f
2600WM-AX4D: ./build/compile.sh -b 2003 -w AX4D -f
2600WM-AX4F: ./build/compile.sh -b 2003 -w AX4F -f