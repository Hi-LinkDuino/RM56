#!/usr/bin/python

# 将一个文件转换成C语言数组
import sys
textFile = open(sys.argv[1], 'rb') # 打开文件
ofileName = sys.argv[1]
if len(sys.argv) > 2:
    fileName = sys.argv[2]
else :
    fileName = "hex_"+ ofileName 
print("covert " + ofileName + " to " + fileName)

with open(fileName,'w') as f:

    while True:
        data = textFile.read(16) # 每一次读取16个字符
        if not data:
            break
        str_line = "" 
        for i in data:
            str_line += "0x{0:02x}".format(i) + ", "
        #print(str_line)
        f.write(str_line)
print("covert ok")

