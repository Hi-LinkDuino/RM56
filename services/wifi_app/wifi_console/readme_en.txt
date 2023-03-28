/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 * AT commad introduce
 *
 ****************************************************************************/

1. Command must be upper-case letter, such as AT end of "\r" or "\n".
2. normal command end of "\r\n" or "\n\r" is OK,
   but for tranfer data command,"\n" or "\r" will send out with AT command
PS:
1. parameters need to split by charater ','(comma in english),if use charater string,suggested to use quotation marks,to make sure it's ok.
   example: AT+WSCONN="a","qqqqqqqq"
2. support multi command send out at the same time,must split by "&&"
   example: AT+WSSCAN=&&AT+WSSCAN=?

command type:
command to set value : AT+<x>=<...>     example: AT+WSSETRECON=1
command to query     : AT+<x>=?         example: AT+WSGSTA=?
command to execute  : AT+<x>            example: AT+RST

return value:
+ok										execute ok
+ok=<...>								execute ok and return info
+error=err code			 				execute ok and error code

err code��
no command��     -1
no permission��  -2
execute fail��   -3
wrong parameter��-4

1.Basic commands
1.1 AT+HELP										help info

1.2 AT+MSLEEP									waiting time,(ms)
	AT+MSLEEP=1000								waiting 1000ms

1.3 AT+ECHO                 					echo
	AT+ECHO=1									enable ECHO ,0: disable, 1:enable (default 0)
	AT+ECHO=?               					get echo status

1.4 AT+OTA                                      OTA function operation
    AT+WSACONF=@1,@2,@3,@4,@5,@6                paramter @1 is the type of operation,and the enumeration values are as follows:
                                                typedef enum{
												    OTA_GET_BOOT_INFO,
													OTA_GET_BOOT_ZONE_NUM,
													OTA_SET_BOOT_ZONE_NUM,
													OTA_SET_ZONE_INFO,
													OTA_SET_BOOT_INFO_INVALIDE,
													OTA_COPY_ZONE,
													AG_OTA_COPY_ZONE
												}OTA_ACTION;
                         						paramter @2,@3,@4,@5,@6 represent the parameters corresponding to each operation type:
                         						@1 equals OTA_GET_BOOT_INFO,no @2,@3,@4,@5,@6;
                         						@1 equals OTA_GET_BOOT_ZONE_NUM,no @2,@3,@4,@5,@6;
                         						@1 equals OTA_SET_BOOT_ZONE_NUM,paramter @2 equals 0 or 1(0:zoneA,1:zoneB);no @3,@4,@5,@6;
                         						@1 equals OTA_SET_ZONE_INFO,paramter @2 equals 0 or 1(0:zoneA,1:zoneB);paramter @3 is bin body size(hexadecimal Number),paramter @4 is bin crc32 value(hexadecimal Number),paramter @5 is reserved value(hexadecimal Number),paramter @6 is version string;
                         						@1 equals OTA_SET_BOOT_INFO_INVALIDE,set boot info partition information is invalid, BootLoader reset,no @2,@3,@4,@5,@6;
                         						@1 equals OTA_COPY_ZONE,Bin header CRC32 is required to verify file integrity,@2 is the destination partition,@3 is the source partition,copy data from the source partition to the destination partition,and set the startup partition to the destination partition;no @4,@5,@6;
                         						@1 equals AG_OTA_COPY_ZONE,no need to check file integrity by bin header CRC32 value,read and write flash with CRC32 check,@2 value for destination partition,@3 value for source partition,copy data from source partition to destination partition,and set startup partition as destination partition;no @4,@5,@6;

2.WIFI command
*** W:WIFI, S:STA mode

2.1 AT+WSACONF									add AP config info,save to wpa_supplicant(only can save 8 AP config info),not save to flash
 	AT+WSACONF=@1,@2,@3,@4,@5					parameter@1:ssid, @2:passwd, @3:whether hide AP, @4:bssid, @5:wep_keyid[0-3]
	example:
	AT+WSACONF="abcd"							add no encrypted AP ssid ,ssid="abcd"
	AT+WSACONF="abcd","12345678"				add encrypted AP ssid and password,ssid="abcd",password="12345678"
	AT+WSACONF="abcd","",hidden					add no encrypted hidden AP ssid ,ssid="abcd"
	AT+WSACONF="abcd","12345678",hidden			add encrypted hidden AP config ,ssid="abcd",password="12345678"
	AT+WSACONF="abcd","12345678",unhidden,"10:da:43:ad:b8:b5"
												add encrypted AP config,ssid="abcd",password="12345678",bssid="10:da:43:ad:b8:b5"
	AT+WSACONF="abcd","12345",unhidden,"10:da:43:ad:b8:b5",0		
												add WEP40 encrypted AP config ,ssid="abcd",password="12345",bssid="10:da:43:ad:b8:b5",wep_keyid=0
	AT+WSACONF="abcd","12345",unhidden,"",1		add WEP40 encrypted AP config ,ssid="abcd",password="12345",bssid="",wep_keyid=1

2.2 AT+WSSCONF								    add AP config��save to flash (only can save 4 AP config info)��not to wpa_supplicant
    parameters are same as 2.1

2.3 AT+WSDCONF								    delete AP config��delete AP config stored in flash and wpa_supplicant
	parameters are same as 2.1

2.4 AT+WSGCCONF									read AP config stored in wpa_supplicant	
    AT+WSGCCONF=?

2.5 AT+WSGSCONF									read AP config stored in flash
	AT+WSGSCONF=?

2.6 AT+WSSCAN								    wifi scan
	AT+WSSCAN								    scan neighboring APs, no scan info by default
	AT+WSSCAN="abcd"							scan specific AP, no scan info by default. parameter @1 is AP's ssid, at most 3 sets of ssids can be specified.
	AT+WSSCAN=?								    query scan info last time

2.7 AT+WSCONN								    connect to AP
	parameters are same as 2.1

2.8 AT+WSACONN								    auto connect to AP
	AT+WSACONN									auto connect to AP,according to AP config stored in wpa_supplicant

2.9 AT+WSGSTA									print AP info which is connected
	AT+WSGSTA=?									print ssid, bssid, channel, rssi, ip address

2.10 AT+WSDISCONN								disconnect AP
	AT+WSDISCONN

2.11 AT+WSSETRECON								set policy to reconnect AP while AP is disconnected passively
	AT+WSSETRECON=1								open policy,reconnect AP while AP is disconnected passively(default config)
	AT+WSSETRECON=0								close policy,don't reconnect AP while AP is disconnected passively

2.12 AT+WSSETIP									set static ip config
											parameter@1:enable static ip, 1 - static ip; 0 - DHCP
												@2:IP address
												@3:netmask
												@4:gateway
	example:
	AT+WSSETIP=0									get ip address by DHCPCD server(default config)
	AT+WSSETIP=1,"192.168.1.3","255.255.255.0","192.168.1.1"			set static ip, netmask, gateway

2.13 AT+WSGLINKSIG								get current link signal (RSSI)
	AT+WSGLINKSIG=?									print current wifi RSSI of the connected link

*** W:WIFI, A:ap mode
2.14 AT+WASTART									open softap (must enable the funtion by macro)
	AT+WASTART=@1,@2,@3,@4,@5							parameter@1: ssid (mandatory)
												@2: channel (2.4G or 5G channel, if set 0 driver will auto select one; suffix +/- means channel type is HT40+/-, no suffix means HT20)
												@3: hidden ssid (0:broadcast 1:hidden)
												@4: security mode ("open" or "wpa-psk" or "wpa2-psk")
												@5: password

	example:
	AT+WASTART="abcd"								open softap, ssid="abcd", channel not specified, HT20, broadcast and not encrypted
	AT+WASTART="abcd",1+,0								open softap, ssid="abcd", channel=1, HT40+, broadcast and not encrypted
	AT+WASTART="abcd",11-,1								open softap, ssid="abcd", channel=11, HT40-, hidden and not encrypted
	AT+WASTART="abcd",0,1,"open"							open softap, ssid="abcd", channel not specified, HT20, hidden and not encrypted
	AT+WASTART="abcd",6,0,"wpa2-psk","12345678"					open softap, ssid="abcd", channel=6, HT20, broadcast, wpa2-psk encrypted, password="12345678"

2.15 AT+WASTOP									close softap (must enable the funtion by macro)

2.16 AT+WAGSTA									get current connected station info
	AT+WAGSTA=?									print mac address of current connected stations

3.NET command:
3.1 AT+NPING									ping command
	AT+NPING="192.168.1.100",4,1,32,1			parameter @1:target IP address or domain,
												parameter@2: execute times (default 4)
												parameter@3: send interval (ms,10-10000)(default 10)
												parameter@4: packet size (max size 14600)(default 32)
												parameter@5: print info��default 1)
	example:
	AT+NPING="192.168.1.1"						ping 192.168.1.1,use default parameters' value
	AT+NPING="www.baidu.com",4,1,32,1			ping www.baidu.com

3.2 AT+NDNS										PING domain
	AT+NDNS="www.baidu.com"						parameter@1: target domain

3.3 AT+NTCPSER									open TCP service,reset after test end
	AT+NTCPSER

3.4 AT+NUDPSER									open UDP service,reset after test end
	AT+NUDPSER

3.3 AT+NTCPCLI									open TCP client,reset after test end
	AT+NTCPCLI="192.168.10.13","6000"           arameter@1:target ip address or domain, parameter@2: port,(default 6000)

3.4 AT+NUDPCLI									open UDP client,reset after test end
	AT+NUDPCLI="192.168.10.13","6500"           parameter@1:target ip address or domain, parameter@2: port,(default 6500)

3.5 AT+NIPERF									iperf test(iperf function default close,config by WIFI_IPERF_SUPPORT in config/wifi_test/target.mk before building)
	AT+NIPERF=@1,@2,***@11						parameters are same as standard iperf command
	service mode & client mode support parameter:
	-h											print help info
	-p5001										set server port is 5001(Optional:default port 5001)
	-i1											set period of printing throughput info (sec ,range 1-60) (optional, default 1)
	-u											UDP mode (optional, default TCP mode)
	-fM											set unit MByte/sec of print result (optional, default auto)
	-l1200										set send buffer size (optional, TCP default 1460Byte, UDP default 1470Byte)
	service mode support parameter:
	-s											service mode
	-r											connect��reset server after test end
	-B225.0.0.1									UDP mode bind to contributing ip address(optional, default machine ip address)
	client mode support parameter:
	-c192.168.1.2								client mode, server ip 192.168.1.2
	-b1M										set UDP send bandwidth 1Mbit/s (optional, default 1Mbit/s)
	-t10										set send time 10s(sec)(optional, default 10s)
	-n1M										set send byte number 1MByte, -n & -t can't set at the same time (optional, default -t10)
	-S0x00										set IP TOS (optional, range 0-0xff, default 0x00)
	
	PS:if want to modify lwip's TCP, moddify macros TCP_WND only in windows,rebuild
	AT+NIPERF=-s								TCP Server
	AT+NIPERF=-c192.168.1.3						TCP Client
	AT+NIPERF=-s,-u								UDP Server
	AT+NIPERF=-c192.168.1.3,-u					UDP Client

3.6 AT+RECDUMP                                  record data dump to ftp server (config NET_FTP_CLIENT_SUPPORT in target.mk)

    -c                                          mic record audio channel num
    -f                                          mic record audio format (default pcm)
    -r                                          mic record audio sample rate (48000 16000 etc. default 160000)
    -d                                          mic record audio duration (unit secord)
    -s                                          ftp server ip address
    -p                                          ftp server port (default 21)
    -u                                          ftp server username
    -w                                          ftp server password
    -h                                          ftp server data path

    example:
    AT+RECDUMP=-c2,-fS16, -r16000, -d5, -s192.168.9.184, -p21, -uusername, -wpassword, -h/test.wav

4.	factory data
4.1	AT+SETCALI									set TX calibration data
	AT+SETCALI=@1,@2,@3						    parameter@1:frq_cal, 0xc2 register
												parameter@2:iQ_cal, 0x90138068 register
												parameter@3:iQ_offset, 0x901380a8 register

4.2	AT+GETCALI									read TX calibration data
	AT+GETCALI=?


5.watchpoint
5.1 AT+WATCHTEST                                    test command，watch one test global int
    AT+WATCHTEST=add                               watch one test global int
    AT+WATCHTEST=rm                                remove the test watchpoint
    AT+WATCHTEST=trig                              trigger watchpoint, system will crash
    AT+WATCHTEST=show                              show watchpoint info

5.2 AT+WATCH                                        watch one 32bit address
    AT+WATCH=add, 0x00000000, write                 add one 32bit address
    AT+WATCH=remove, 0x00000000                     remove one 32bit address
    AT+WATCH=show                                   show watchpoint info

