#!/usr/bin/python
# coding: utf-8


import sys
print('目前系统的编码为：',sys.getdefaultencoding())

b = '发送成功'

print(type(b))
print(b)

b2 = b.encode('utf-8')

print(type(b2))
print(b2)

b3 = bytearray(b2)

print(type(b3))
print(b3)

for i in b3:
	print("0x{0:02x}".format(i) + ", ", end='')
	
	
print("\n")

for j in b3:
	print( "%" + "{0:02x}".format(j), end='')

