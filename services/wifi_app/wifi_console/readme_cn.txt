/***************************************************************************
 * 
 * 版权所有2015-2019 恒玄科技（上海）有限公司
 * 保留所有未发布的权利。
 *
 * 若没有恒玄科技（上海）有限公司的书面许可
 * 本作品的任何部分均不得以任何形式使用或复制
 * 存储在数据库或检索系统中
 * 
 * 此项工作的使用，受恒玄科技（上海）有限公司的许可管辖
 * 此作品包含机密和专有信息
 * 恒玄科技（上海）有限公司受版权和商业秘密，商标和其他知识产权保护。
 *
 * AT commad介绍
 *
 ****************************************************************************/


1.命令部分一律使用大写，如AT，以\r或\n结尾，若以\r\n, 或\n\r结尾
2.普通命令正常，但对于数据传输命令会将末尾的\n或\r作为字符发送
注：
1.参数部分以“,”(英文逗号)分隔，字符串部分建议使用引号，防止包含特殊符号时出错，如AT+WSCONN=“a”,“qqqqqqqq”
2.支持多命令同时发送，以”&&“分割。例如:AT+WSSCAN=&&AT+WSSCAN=?

命令类型：
设置命令：AT+<x>=<...>	如AT+WSSETRECON=1
查询命令：AT+<x>=?		如AT+WSGSTA=？
执行命令：AT+<x>		如AT+RST

返回值
+ok						执行成功
+ok=<...>				执行成功且有返回信息
+error=err code			执行失败并返回错误码

err code：
无此指令：	-1
不允许执行：-2
执行失败：	-3
参数有错误：-4


1.基础命令
1.1	AT+HELP											查看AT帮助
1.2 AT+MSLEEP										等待一定时间，（单位ms）
	AT+MSLEEP=1000						  			等待 1000ms
1.3 AT+ECHO                                         回显
    AT+ECHO=1                                       打开回显，0：关闭，1：打开 (默认 0)
	AT+ECHO=？                                      查询当前开闭状态
1.4 AT+OTA                                          OTA功能操作
    AT+WSACONF=@1,@2,@3,@4,@5,@6                    参数@1表示操作类型，枚举值如下：
                                                    typedef enum{
													    OTA_GET_BOOT_INFO,
														OTA_GET_BOOT_ZONE_NUM,
														OTA_SET_BOOT_ZONE_NUM,
														OTA_SET_ZONE_INFO,
														OTA_SET_BOOT_INFO_INVALIDE,
														OTA_COPY_ZONE,
														AG_OTA_COPY_ZONE
													}OTA_ACTION;
                             						参数@2，@3，@4，@5，@6表示各操作类型对应的参数：
                             						@1为OTA_GET_BOOT_INFO时，无@2，@3，@4，@5，@6参数；
                             						@1为OTA_GET_BOOT_ZONE_NUM时，无@2，@3，@4，@5，@6参数；
                             						@1为OTA_SET_BOOT_ZONE_NUM时，@2值为0表示zoneA，@2值为1表示zoneB；无@3，@4，@5，@6参数；
                             						@1为OTA_SET_ZONE_INFO时，@2值为0表示zoneA，@2值为1表示zoneB；@3值为bin body size（十六进制），@4值为bin crc32 value（十六进制），@5值为reserved（十六进制），@6值为version（字符串）；
                             						@1为OTA_SET_BOOT_INFO_INVALIDE时，boot info分区信息无效，BootLoader重新设置，无@2，@3，@4，@5，@6参数；
                             						@1为OTA_COPY_ZONE时，需要bin header crc32值校验文件完整性，@2值为目的分区，@3值为源分区，将源分区中数据拷贝到目的分区，并将启动分区设置为目的分区，无@4，@5，@6参数；
                             						@1为AG_OTA_COPY_ZONE时，无需bin header crc32值校验文件完整性，读、写flash进行crc32校验，@2值为目的分区,@3值为源分区,将源分区中数据拷贝到目的分区，并将启动分区设置为目的分区，无@4，@5，@6参数；

2.WIFI命令
*** W表示WIFI，S表示STA模式，
2.1 AT+WSACONF										增加AP配置信息，只保存到wpa_supplicant（最多保存8个），不保存到flash
	AT+WSACONF=@1,@2,@3,@4,@5						参数@1表示ssid，@2表示passwd，@3表示是否隐藏AP，@4表示bssid，@5表示wep_keyid[0-3]。
													
	示例如下：
	AT+WSACONF="abcd"								增加非加密AP配置，ssid="abcd"
	AT+WSACONF="abcd","12345678"					增加加密AP配置，ssid="abcd"，密码为"12345678"
	AT+WSACONF="abcd","",hidden						增加非加密隐藏AP配置，ssid="abcd"
	AT+WSACONF="abcd","12345678",hidden				增加加密隐藏AP配置，ssid="abcd"，密码为"12345678"
	AT+WSACONF="abcd","12345678",unhidden,"10:da:43:ad:b8:b5"
													增加加密AP配置，ssid="abcd"，密码为"12345678"，bssid="10:da:43:ad:b8:b5"
	AT+WSACONF="abcd","12345",unhidden,"10:da:43:ad:b8:b5",0						
													增加WEP40加密AP配置，ssid="abcd"，密码为"12345"，bssid="10:da:43:ad:b8:b5"，wep_keyid=0
	AT+WSACONF="abcd","12345",unhidden,"",1			增加WEP40加密AP配置，ssid="abcd"，密码为"12345"，wep_keyid=1

2.2 AT+WSSCONF										增加AP配置信息，只保存到flash（最多可以保存4个)，不保存到wpa_supplicant
	参数同2.1

2.3 AT+WSDCONF										删除AP配置信息，同时删除flash和wpa_supplicant里的配置
	参数同2.1

2.4 AT+WSGCCONF										读取当前wpa_supplicant中保存的AP配置信息
	AT+WSGCCONF=?

2.5 AT+WSGSCONF										读取当前flash中保存的AP配置信息
	AT+WSGSCONF=?

2.6 AT+WSSCAN										执行扫描操作
	AT+WSSCAN										执行扫描操作，默认不会打印扫描结果
	AT+WSSCAN="abcd"								扫描指定路由器，默认不会打印扫描结果。参数@1表示路由器ssid，最多可以带三组ssid（@1,@2,@3）
	AT+WSSCAN=?										查看上次扫描结果

2.7 AT+WSCONN										连接指定AP
	参数同2.1

2.8 AT+WSACONN										自动连接AP
	AT+WSACONN										根据wpa_supplicant中保存的AP信息，自动选择要连接的AP

2.9 AT+WSGSTA										打印当前连接AP的信息
	AT+WSGSTA=?										打印信息包括ssid，bssid，信道，rssi，ip地址

2.10 AT+WSDISCONN									断开已连接的AP
	AT+WSDISCONN

2.11 AT+WSSETRECON									设置被动断开AP连接后重连策略
	AT+WSSETRECON=1									被动断开后，根据连接策略自动重连（默认设置）
	AT+WSSETRECON=0									被动断开后，不发起重连

2.12 AT+WSSETIP										设置静态IP相关信息
												参数@1：启用静态IP，1 - 静态IP； 0 - DHCP
												   @2：IP地址
												   @3：网络掩码
												   @4：网关
	示例如下：
	AT+WSSETIP=0									不使用静态IP，通过DHCPCD服务器获取IP地址（默认设置）
	AT+WSSETIP=1,"192.168.1.3","255.255.255.0”,"192.168.1.1"					使用静态IP，参数@1表示IP地址，参数@2表示子网掩码，参数@3表示默认网关

2.13 AT+WSGLINKSIG									获取当前连接的信号强度（RSSI）
	AT+WSGLINKSIG=?										打印当前连接的RSSI值

*** W:WIFI，A:ap mode
2.14 AT+WASTART										打开热点功能（需要打开支持该功能的宏）
	AT+WASTART=@1,@2,@3,@4,@5								参数@1：ssid （必填项）
												   @2：channel（2.4G或5G信道，0则自动选择，后缀+/-表示信道带宽为HT40+/-，不带后缀默认HT20）
												   @3：隐藏ssid（0：broadcast 1：hidden）
												   @4：加密模式（"open" or "wpa-psk" or "wpa2-psk"）
												   @5：password

	示例如下：
	AT+WASTART="abcd"									打开热点功能，ssid="abcd"，不指定信道，HT20，不隐藏ssid，无密码
	AT+WASTART="abcd",1+,0									打开热点功能，ssid="abcd"，channel=1，HT40+，不隐藏ssid，无密码
	AT+WASTART="abcd",11-,1									打开热点功能，ssid="abcd"，channel=11，HT40-，隐藏ssid，无密码
	AT+WASTART="abcd",0,1,"open"								打开热点功能，ssid="abcd"，不指定信道，HT20，隐藏ssid，无密码
	AT+WASTART="abcd",6,0,"wpa2-psk","12345678"						打开热点功能，ssid="abcd"，channel=6，HT20，不隐藏ssid，加密模式wpa2-psk，密码为"12345678"

2.15 AT+WASTOP										关闭热点功能（需要打开支持该功能的宏）

2.16 AT+WAGSTA										获取热点当前连接的设备信息
	AT+WAGSTA=?										打印当前连接设备的mac地址

3.NET命令
3.1 AT+NPING										触发PING操作
	AT+NPING="192.168.1.100",4,1,32,1				参数@1表示目标IP地址或域名，
													参数@2表示执行次数，（默认4）
													参数@3表示发送时间间隔（单位ms，10-10000）（默认10）
													参数@4表示发送数据包长度（最大长度14600）（默认32）
													参数@5表示是否打印（默认1）
	示例如下：
	AT+NPING="192.168.1.1"							ping 192.168.1.1，使用默认参数
	AT+NPING="www.baidu.com",4,1,32,1				ping www.baidu.com

3.2 AT+NDNS											触发PING操作
	AT+NDNS="www.baidu.com"							参数@1表示目标域名

3.3 AT+NTCPSER										开启TCP service，测试完reset设备
	AT+NTCPSER

3.4 AT+NUDPSER										开启UDP service，测试完reset设备
	AT+NUDPSER

3.3 AT+NTCPCLI										开启TCP client，测试完reset设备
	AT+NTCPCLI="192.168.10.13","6000"               参数@1表示目标IP地址或域名，参数@2表示端口,(默认6000)

3.4 AT+NUDPCLI										开启UDP client，测试完reset设备
	AT+NUDPCLI="192.168.10.13","6500"               参数@1表示目标IP地址或域名，参数@2表示端口,(默认6500)

3.5 AT+NIPERF										开始iperf测试(iperf功能默认关闭，需要在编译前打开config/wifi_test/target.mk中WIFI_IPERF_SUPPORT)
	AT+NIPERF=@1,@2,***@11							不同模式支持参数最多11个，参数和标准iperf参数含意相同，参数不分顺序
	service模式和client模式都支持的参数如下：
	-h												打印帮助信息
	-p5001											设置服务器端口号为5001（可选参数，默认端口号5001）
	-i1												设置吞吐量打印周期为1s（单位s，取值范围1-60）（可选参数，默认1）
	-u												UDP模式（可选参数，默认为TCP模式）
	-fM												设置统计结果打印单位为MByte/sec（可选参数，默认为自动）
	-l1200											设置发送buffer长度（可选参数，TCP默认1460Byte，UDP默认1470Byte）
	service模式支持参数如下:
	-s												service模式
	-r												连接、测试结束重启server，进行新的连接
	-B225.0.0.1										UDP模式绑定特定ip地址（可选参数，默认为本机ip地址）
	client模式支持参数如下：
	-c192.168.1.2									client模式，且service ip地址为192.168.1.2
	-b1M											设置UDP发送带宽1Mbit/s(可选参数，默认1Mbit/s)
	-t10											设置发送时间10s（单位s）（可选参数，默认10s）
	-n1M											设置发送字节数1MByte，-n和-t不能同时设置(可选参数，默认为-t10)
	-S0x00											设置IP TOS值（可选参数，范围0-0xff，默认0x00)

	注释：想要修改lwip的TCP Windows只能修改宏TCP_WND，重新编译
	AT+NIPERF=-s									TCP Server
	AT+NIPERF=-c192.168.1.3							TCP Client
	AT+NIPERF=-s,-u									UDP Server
	AT+NIPERF=-c192.168.1.3,-u						UDP Client

3.6 AT+RECDUMP                                      开始录音到ftp服务器 (配置NET_FTP_CLIENT_SUPPORT 在target.mk)

    -c                                              mic 几路录音
    -f                                              mic 录音数据的格式，默认PCM
    -r                                              mic 录音采样率 (48000 16000 等,默认16000)
    -d                                              mic 录音时间 (默认秒)
    -s                                              ftp 服务器的ip地址
    -p                                              ftp 服务器的端口
    -u                                              ftp 服务器的登录用户名
    -w                                              ftp 服务器的登录密码
    -h                                              ftp 服务器的保存文件路径

     示例如下：
     AT+RECDUMP=-c2,-fS16, -r16000, -d5, -s192.168.9.184, -p21, -uusername, -wpassword, -h/test.wav

4.工厂数据
4.1 AT+SETCALI										写TX校准数据
	AT+SETCALI=@1,@2,@3							    参数@1表示frq_cal, 0xc2寄存器
													参数@2表示iQ_cal, 0x90138068寄存器
													参数@3表示iQ_offset, 0x901380a8寄存器
																	
4.1 AT+GETCALI										读TX校准数据
	AT+SETCALI=?					

5.内存监控命令
5.1 AT+WATCHTEST                                    测试指令，监控一个变量地址是否被更改，若被更改，则触发excpetion
    AT+WATCHTEST=add                               监控测试全局变量地址
    AT+WATCHTEST=rm                                移除测试全局变量地址
    AT+WATCHTEST=trig                              触发内存监控
    AT+WATCHTEST=show                              查看内存监控信息

5.2 AT+WATCH                                        监控一个32位地址是否被更改，若被更改，则触发excpetion,最多可监控4个
    AT+WATCH=add, 0x00000000, write                 监控一个32位地址，0x00000000为想要监控的地址
    AT+WATCH=remove, 0x00000000                     移除一个被监控的地址
    AT+WATCH=show                                   查看内存监控信息
