# Fn-link AT command introduce

This document introduces Fn-link AT command. Fn-link AT command is based on BES version and enhances some functions.

##### command type:

command to set value : AT+<x>=<...>     example: *AT+WSSETRECON=1*
command to query     : AT+<x>=?         	example: *AT+WSGSTA=?*
command to execute  : AT+<x>            	example: *AT+RST*

##### return value:

+ok									   execute ok
+ok=<...>							 execute ok and return info
+error=err code			 	execute ok and error code

##### err code:

no command:     -1
no permission:   -2
execute fail:   	 -3
wrong parameter:	-4

##### source code reference:

*bes2600w_aiot\services\wifi_app\wifi_console\*

##### 1. Basic commands

###### 1.1 AT+HELP										

Print help info.



###### 1.2 AT+MSLEEP									

The current task(thread) waiting time(ms).
***AT+MSLEEP=1000***								
Waiting 1000ms



###### 1.3 AT+ECHO                 					

Input at command echo control.

***AT+ECHO=1***						
Enable ECHO, 0: disable, 1:enable (default 0).

***AT+ECHO=***?
Get echo status.



###### 1.4 AT+OTA                                      

OTA function operation. This command is obsolete and will update a new one.



##### 2. WiFi commands

Prefix:
***W:WiFi, S:STA mode, A:AP mode, @n:parameter n.***

###### 2.1 AT+WSACONF									

Add AP config info, save to wpa_supplicant(which in CP, only can save 8 AP config info), not save to flash. When restart the config info will be lost.

***AT+WSACONF=***@1,@2,@3,@4,@5					
parameter:
@1:ssid, 
@2:passwd, 
@3:whether hide AP, 
@4:bssid, 
@5:WEP_keyid[0-3]

example:
***AT+WSACONF="abcd"***							
add no encrypted AP ssid, ssid="abcd"
***AT+WSACONF="abcd","12345678"***				
add encrypted AP ssid and password, ssid="abcd", password="12345678"
***AT+WSACONF="abcd","",hidden***					
add no encrypted hidden AP ssid, ssid="abcd"
***AT+WSACONF="abcd","12345678",hidden***			
add encrypted hidden AP config, ssid="abcd",password="12345678"
***AT+WSACONF="abcd","12345678",unhidden,"10:da:43:ad:b8:b5"***
add encrypted AP config, ssid="abcd", password="12345678", bssid="10:da:43:ad:b8:b5"
***AT+WSACONF="abcd","12345",unhidden,"10:da:43:ad:b8:b5",0***		
add WEP40 encrypted AP config, ssid="abcd", password="12345", bssid="10:da:43:ad:b8:b5", WEP_keyid=0
***AT+WSACONF="abcd","12345",unhidden,"",1***		
add WEP40 encrypted AP config, ssid="abcd", password="12345", bssid="", WEP_keyid=1

Notice: the several conf commands is interfaces to access wpa_supplicant in CP. For add a conf item, there are two ways. One is by the command AT+WSACONF, the other is by connecting an AP successfully.  

 

###### 2.2 AT+WSSCONF								    

Add AP config info, save to flash (only can save 4 AP config info). not to wpa_supplicant.
parameters are same as 2.1.

Notice: All the command&interface of saving/getting/etc AP config info to/from Flash is  obsolete. We suggest that the customers maintain the conf info in Flash themselvse by Flash system interfaces.



###### 2.3 AT+WSDCONF								    

Delete AP config stored in flash and wpa_supplicant.
parameters are same as 2.1.

Notiec: Don't send AT+WSDCONF=all. That will cause CP to assert. Wait BES to fix it. 



###### 2.4 AT+WSGCCONF									

Read AP config stored in wpa_supplicant. 	
AT+WSGCCONF=?



###### 2.5 AT+WSGSCONF									

Read AP config stored in flash. It's obsolete.
AT+WSGSCONF=?



###### 2.6 AT+WSSCAN								    

WiFi scan.

***AT+WSSCAN***								    
Scan neighboring APs, no scan info by default.
***AT+WSSCAN=***"abcd"							
Scan specific AP, no scan info by default. parameter @1 is AP's ssid, at most 3 sets of ssids can be specified.
***AT+WSSCAN=?***								    
Query scan info last time
***AT+WSSCAN=_FLUSH***								    
Flush last scan result. Before begin a new scan, flush first.

Notice: Here is a bug for scan specific AP function that all neighboring APs wiil be returned.  To be fixed. 



###### 2.7 AT+WSCONN								    

Connect to AP.
Parameters are same as 2.1.
@6:wfp, which can be "disable", "enable", "require" and "default".



###### 2.8 AT+WSACONN								    

Auto connect to AP.

***AT+WSACONN***									
Auto connect to AP, according to AP config stored in wpa_supplicant. Select the best RSSI to auto connect. 



###### 2.9 AT+WSNICONN

Start wifi sta connect to specific network id. network id is the index of conf  in wpa_supplicant.

***AT+WSNICONN=0***



###### 2.10 AT+WSGSTA									

Print AP info which is connected.

***AT+WSGSTA=***?									
Print ssid, bssid, channel, rssi, ip address, netmask and gateway.



###### 2.11 AT+WSDISCONN								

Disconnect from current connected AP.

***AT+WSDISCONN***



###### 2.12 AT+WSSETRECON								

Set policy to reconnect AP while AP is disconnected **passively**.

***AT+WSSETRECON=1***								
Open policy, reconnect AP while AP is disconnected passively(default config, reconnect the last AP for 100 times).
***AT+WSSETRECON=0***								
Close policy, don't reconnect AP while AP is disconnected passively.



###### 2.13 AT+WSSETIP									

Set static ip config.
parameter:
@1:enable static ip, 1 - static ip,
@2:IP address,
@3:netmask,
@4:gateway,

example:
***AT+WSSETIP=1,"192.168.1.3","255.255.255.0","192.168.1.1"***			
Set static ip, netmask, gateway. 



###### 2.14 AT+WSGLINKSIG								

Get current link signal (RSSI)
***AT+WSGLINKSIG=?***									
Print current wifi RSSI of the connected link.



###### 2.15 AT+WASTART									

Open softap. From here to 2.17 is softap AT commands. 
***AT+WASTART=***@1,@2,@3,@4,@5							
parameter:
@1: ssid (mandatory),
@2: channel (2.4G or 5G channel, if set 0 driver will auto select one; suffix +/- means channel type is HT40+/-, no suffix means HT20),
@3: hidden ssid (0:broadcast, 1:hidden),
@4: security mode ("open" or "wpa-psk" or "wpa2-psk"),
@5: password,

Notice: softap mode and sta mode can't coexist. Before start one must stop the other.

example:
***AT+WASTART="abcd"***								
Open softap, ssid="abcd", channel not specified, HT20, broadcast and not encrypted.
***AT+WASTART="abcd",1+,0***								
Open softap, ssid="abcd", channel=1, HT40+, broadcast and not encrypted.
***AT+WASTART="abcd",11-,1***								
Open softap, ssid="abcd", channel=11, HT40-, hidden and not encrypted.
***AT+WASTART="abcd",0,1,"open"***							
Open softap, ssid="abcd", channel not specified, HT20, hidden and not encrypted.
***AT+WASTART="abcd",6,0,"wpa2-psk","12345678"***					
Open softap, ssid="abcd", channel=6, HT20, broadcast, wpa2-psk encrypted, password="12345678".



###### 2.16 AT+WASTOP									

Close softap.
***AT+WASTOP***



###### 2.17 AT+WAGSTA									

Get current connected station info.
***AT+WAGSTA=?***									
Print mac address of current connected stations.



###### 2.18 AT+SNSTART									

Start sniffer.
***AT+WASTART=***@1,@2,@3,@4,@5,@6,@7							
parameter:
@1~@7: the combination of "MGMT", "FROMDS", "TODS", "LDPC", "ACK", "NULL", "RTS"



###### 2.18 AT+SNSETCH									

Set sniffer channel.
***AT+WASTART=***@1							
parameter:
@1: the channel to sniffer. Before start sniffer, channel can be set.





###### 2.19 AT+SNSTOP									

Stop sniffer.
***AT+SNSTOP***



##### 3. NET command

###### 3.1 AT+NPING									

Ping command.
***AT+NPING="192.168.1.100",4,1,32,1***			
parameter:
@1:target IP address or domain,
@2: execute times (default 4),
@3: send interval (ms,10-10000)(default 10),
@4: packet size (max size 14600)(default 32),
@5: print info(default 1),

example:
***AT+NPING="192.168.1.1"***						
Ping 192.168.1.1,use default parameters value.
***AT+NPING="www.baidu.com",4,1,32,1***			
Ping www.baidu.com



###### 3.2 AT+NDNS										

Get IP address by domain or host name.
***AT+NDNS="www.baidu.com"***						
parameter:
@1: target domain



###### 3.3 AT+NTCPSER									

Start TCP service.
***AT+NTCPSER***
Notice: this command is obsolete.



###### 3.4 AT+NUDPSER									

Start UDP service.
***AT+NUDPSER***



###### 3.5 AT+NTCPCLI									

Open TCP client, start to test TCP.
***AT+NTCPCLI="192.168.10.13","6000",10***           
Parameter:
@1:target ip address or domain, 
@2: port, (default 6001),
@3: test time by second.
Notice: this command is obsolete.



###### 3.6 AT+NUDPCLI									

Open UDP client, start to test UDP.
***AT+NUDPCLI="192.168.10.13","6500"***           
parameter:
@1:target ip address or domain, 
@2: port, (default 6500)



###### 3.7 AT+NIPERF									

Iperf2 test. 
***AT+NIPERF=@1,@2,***@11***						
Parameters are same as standard iperf command.
Service mode & client mode support parameter:

```
	-h										   print help info
	-p5001								  set server port is 5001(Optional:default port 5001)
	-i1										  set period of printing throughput info (sec ,range 1-60) (optional, default 1)
	-u											UDP mode (optional, default TCP mode)
	-fM										 set unit MByte/sec of print result (optional, default auto)
	-l1200									set send buffer size (optional, TCP default 1460Byte, UDP default 1470Byte)
	service mode support parameter:
	-s											service mode
	-r											reset server after test end
	-B225.0.0.1							UDP mode bind to contributing ip address(optional, default machine ip address)
	client mode support parameter:
	-c192.168.1.2						client mode, server ip 192.168.1.2
	-b1M									  set UDP send bandwidth 1Mbit/s (optional, default 1Mbit/s)
	-t10										set send time 10s(sec)(optional, default 10s)
	-n1M									  set send byte number 1MByte, -n & -t can't set at the same time (optional, default -t10)
	-S0x00									set IP TOS (optional, range 0-0xff, default 0x00)
```

Notice:if want to modify lwip's TCP window, moddify macros TCP_WND and then rebuild.

example:
***AT+NIPERF=-s***								TCP Server
***AT+NIPERF=-c192.168.1.3***						TCP Client
***AT+NIPERF=-s,-u***								UDP Server
***AT+NIPERF=-c192.168.1.3,-u***					UDP Client



###### 3.8 AT+ifconfig

Show or config netif.     

example:
***AT+ifconfig***
Show ifconfig info for all netif.
***AT+ifconfig=en1,192.168.0.123,255.255.255.0,192.168.0.1***
Config en1.



###### 3.9 AT+RECDUMP                                  

Record mcu mic data dump to ftp server. 

```
-c                                          mic record audio channel num
-f                                          mic record audio format (default pcm)
-r                                          mic record audio sample rate (48000 16000 etc. default 160000)
-d                                          mic record audio duration (unit secord)
-s                                          ftp server ip address
-p                                          ftp server port (default 21)
-u                                          ftp server username
-w                                          ftp server password
-h                                          ftp server data path
```

example:
AT+RECDUMP=-c2,-fS16, -r16000, -d5, -s192.168.9.184, -p21, -uusername, -wpassword, -htest

Notice: 2 files named as "test2mic" and "test3mic" file will be dump to ftp. test2mic is 2mic data and test3mic is 2mic data + 1ref mic data. The ftp server should support 2 connects by one account.



###### 3.10 AT+A7DUMP                                  

Record processed mic data by DSP and dump to ftp server. 

```
-c                                          mic record audio channel num
-f                                          mic record audio format (default pcm)
-r                                          mic record audio sample rate (48000 16000 etc. default 160000)
-d                                          mic record audio duration (unit secord)
-s                                          ftp server ip address
-p                                          ftp server port (default 21)
-u                                          ftp server username
-w                                          ftp server password
-h                                          ftp server data path
```

example:
AT+A7DUMP=-c2,-fS16, -r16000, -d5, -s192.168.9.184, -p21, -uusername, -wpassword, -htest

Notice: test file is 2mic data + 1ref mic data.

##### 4. Debug and Misc

###### 4.1 AT+MD

Read address and length of memory to terminal or FTP.
parameter:
@1:memory address or register address to read,
@2:length to read by bit,
@3:ftp ip address,
@4:ftp port,
@5:ftp username,
@6:ftp password,
@7:ftp filename,

***AT+MD=0x28000000,128***
Read 128bit from 0x28000000 to terminal.
***AT+MD=0x28000000,128,192.168.0.111,21,ad,ad,dump.hex***
Dump 128bit from 0x28000000 to ftp as file named dump.hex.

**Notice**: this command is used with dldtool can R/W flash. e.g.
*AT+MD=0x28000000,2097152,192.168.0.111,21,ad,ad,dump.hex*
Modify the dump.hex as your need, then dowload it to another address
*dldtool 9  .\burn\programmer_32M.bin --addr 0x28010000 dump.hex*



###### 4.2 AT+MW

Write address with a 32bit value.
parameter:
@1:address to be wroten,
@2:value to be wroten

***AT+MW=0x20000000,0x12345678***
Write 0x12345678 to address 0x20000000.

**Notice**:Flash write not supported.



###### 4.3 AT+FREEHEAPSIZE

Show RTOS/application free heap size.

***AT+FREEHEAPSIZE***



###### 4.4 AT+THREAD

Show thread information.

***AT+THREAD***



###### 4.5 AT+RUNTIME

Show task/thread run time statistics.

***AT+RUNTIME***



###### 4.6 AT+SETGPIO

Set GPIO output low/high.

***AT+SETGPIO=@1,@2***
parameter:
@1:GPIO index to output, refer to the enum HAL_IOMUX_PIN_T. 
@2: 0 low and 1 high. 



###### 4.7  AT+WATCH                                        

Watch one 32bit address. 
 ***AT+WATCH=add***, 0x00000000, write                 add one 32bit address
when @1 is add, @3 can be write/read/access
 ***AT+WATCH=remove***, 0x00000000                     remove one 32bit address
 ***AT+WATCH=show***                                   				show watchpoint info

When a watch address is modified(write, read and access), an assert will trigger.



###### 4.8  AT+REBOOT                                        

Reboot system.

***AT+REBOOT***



###### 4.9  AT+A7CMD                                        

Send command to DSP.

***AT+A7CMD=PANIC***
Make a panic on DSP and MCU will restart DSP after DSP panic.
***AT+A7CMD=PS***,1000
Print DSP system run time statistics every 1000 ms.
***AT+A7CMD=disable***
Disable DSP. 
***AT+A7CMD=enable***
enable DSP.



###### 4.10  AT+A7RB                                        

Reboot the DSP.

***AT+A7RB***



###### 4.11  AT+CPRB                                        

Reboot the CP.

***AT+CPRB***



###### 4.12  AT+PANIC                                        

Start MCU panic.

***AT+PANIC***



##### 5. NV and partition

###### 5.1 AT+SETWMAC

Set WiFi mac address to factory
***AT+SETWMAC=00***:80:e1:30:50:66
Set WiFi mac address 00:80:e1:30:50:66 in factory.



###### 5.2 AT+GETWMAC

Get WiFi mac address to factory
***AT+GETWMAC***



###### 5.3 AT+SETBTMAC

Set BT mac address to factory
***AT+SETWMAC=00***:80:e1:30:50:66
Set BT mac address 00:80:e1:30:50:66 in factory.



###### 5.4 AT+GETBTMAC

Get BT mac address to factory
***AT+GETBTMAC***



###### 5.5 AT+SETBLEMAC

Set BLE mac address to factory
***AT+SETBLEMAC=00***:80:e1:30:50:66
Set BLE mac address 00:80:e1:30:50:66 in factory.



###### 5.6 AT+SETBTNAME

Set BT name to factory
***AT+SETBTNAME=fnlink-test***
Set BT name as fnlink-test in factory.



##### 6. Multimedia

###### 6.1 AT+LPLAYER_START

lplayer create and start. the arg could be a url or local file path.

***AT+LPLAYER_START=***"http://music.163.com/song/media/outer/url?id=5264842.mp3"
Play the url mp3.
***AT+LPLAYER_START***
Resume the play which was paused.



###### 6.2 AT+LPLAYER_STOP

lplayer stop and destory.

***AT+LPLAYER_STOP***



###### 6.3 AT+LPLAYER_PAUSE

lplayer pause, resume could use AT+LPLAYER_START.

***AT+LPLAYER_PAUSE***



###### 6.4 AT+LPLAYER_SET_VOL

lplayer set vol. [0,100].

***AT+LPLAYER_SET_VOL=100***
Set volume to Max.



###### 6.5 AT+LPLAYER_SEEK

lplayer seek to specific position. unit: s.

***AT+LPLAYER_SEEK=30***
Seek the play position at 30s.



###### 6.6 AT+LPLAYER_SINGLE_LOOP

lplayer enable or disable single loop.

***AT+LPLAYER_SINGLE_LOOP=1***
Enable lplayer single loop.

***AT+LPLAYER_SINGLE_LOOP=0***
Disable lplayer single loop.



###### 6.7 AT+LPLAYER_PLAY_TIME

show current program play time.

***AT+LPLAYER_PLAY_TIME***



###### 6.8 AT+ALSA_SET_FVOL

The same as AT+LPLAYER_SET_VOL. 



###### 6.9 AT+ALSA_GET_FVOL

Get current alsa/lplayer volume[0-100].

 ***AT+ALSA_GET_FVOL***

Notice:lplayer vol function will call the functions of alsa. So whether set vol by lplayer or alsa, ALSA_GET_FVOL will get the right result.

e.g., AT+LPLAYER_SET_VOL=100, ALSA_GET_FVOL get 100; then AT+ALSA_SET_FVOL=50, and ALSA_GET_FVOL get 50.



##### 7. BT and BLE

###### 7.1 AT+BTINIT

BT/BLE init power on. This command must be called by calling any BT/BLE functions.

***AT+BTINIT=1***



###### 7.2 AT+BTSCAN

BT scan open/close.

***AT+BTSCAN=1***
BT is discoverable and connectable.
***AT+BTSCAN=0***
BT is Non-discoverable or connectable.



###### 7.3 AT+BLE

BLE adv open/close. 

***AT+BLE=1***,200
Start BLE ADV by 200ms interval.
***AT+BLE=0***,200
Stop BLE ADV.

Notice: first send AT+BTINIT=1.



###### 7.4 AT+BLEADV

BLE ADV test. 

***AT+BLEADV=1***,1
Start BLE ADV by 200ms interval.

Notice: first send AT+BTINIT=1.



###### 7.5 AT+BLESCAN

BLE scan open/close.

***AT+BLESCAN=1***,10,100
@1: enable or disable
@2: scan window
@3: scan interval.



###### 7.6 AT+BLECONN

BLE start connect/cancel.

***AT+BLECONN=1***,00:80:e1:30:50:66,0
@1: enable or disable
@2: remote MAC address
@3: 0:Public or Private Static Address according to device address configuration
	    1:Generated resolvable private random address
		2:Generated non-resolvable private random address







