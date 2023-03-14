/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

import wifi from '@ohos.wifi'

function sleep(delay) {
    return new Promise(resovle => setTimeout(resovle, delay))
}

async function tryToEnableWifi(){
    if(!wifi.isWifiActive()){
        console.info("wifi_test/enable wifi:" + wifi.enableWifi());
        await sleep(3000);
    }
    console.info("wifi_test/wifi status:" + wifi.isWifiActive());
}

let WifiSecurityType = {
    WIFI_SEC_TYPE_INVALID: 0,
    WIFI_SEC_TYPE_OPEN: 1,
    WIFI_SEC_TYPE_WEP: 2,
    WIFI_SEC_TYPE_PSK: 3,
    WIFI_SEC_TYPE_SAE: 4,
}

let SuppState = {
    DISCONNECTED: 0,
    INTERFACE_DISABLED: 1,
    INACTIVE : 2,
    SCANNING: 3,
    AUTHENTICATING: 4,
    ASSOCIATING: 5,
    ASSOCIATED: 6,
    FOUR_WAY_HANDSHAKE: 7,
    GROUP_HANDSHAKE: 8,
    COMPLETED: 9,
    UNINITIALIZED: 10,
    INVALID: 11,
}

let ConnState = {
    SCANNING: 0,
    CONNECTING: 1,
    AUTHENTICATING: 2,
    OBTAINING_IPADDR: 3,
    CONNECTED: 4,
    DISCONNECTING: 5,
    DISCONNECTED: 6,
    UNKNOWN: 7,
}

let ipConfig = {
    "ipAddress": 1284752956,
    "gateway": 17017024,
    "dnsServers": 12345678,
    "domains": ["aaa"],
}

let IpType = {
    STATIC : 0,
    DHCP : 1,
    UNKNOWN: 2
}

describe('ACTS_WifiTest', function() {
    beforeEach(function() {})

    afterEach(function() {})

    /**
     * @tc.number     open_0001
     * @tc.name       SUB_Communication_WiFi_Sta_Open_0001
     * @tc.desc       Test wifi.isWifiEnable API functionality.
     */
    it('SUB_Communication_WiFi_Sta_Open_0001', 0, async function(done) {
        console.info("[wifi_test] check the state of wifi, if it's close, open it.");
        let active = wifi.isWifiActive();
        if (!active) {
            let enable = wifi.enableWifi();
            await sleep(5000);
            console.log("[wifi_test] wifi open result: " + enable);
            expect(enable).assertTrue();
        }
        let disable = wifi.disableWifi();
        await sleep(5000);
        console.log("[wifi_test] wifi close result: " + disable);
        expect(disable).assertTrue();
        
        console.log("[wifi_test]  check the state of wifi: " + wifi.isWifiActive());
        expect(wifi.isWifiActive()).assertFalse();
        done();
    })

    /**
     * @tc.number     Scan_0001
     * @tc.name       SUB_Communication_WiFi_Sta_Scan_0001
     * @tc.desc       Test get ScanInfos callback API functionality.
     */
    it('SUB_Communication_WiFi_Sta_Scan_0001', 0, async function(done) {
        await tryToEnableWifi();
        let scan = wifi.scan();
        await sleep(3000);
        console.log("[wifi_test] open wifi scan result: " + scan);
        expect(scan).assertTrue();

        await wifi.getScanInfos()
            .then(result => {
                let clen = Object.keys(result).length;
                expect(clen).assertLarger(0);
                console.info("[wifi_test] getScanInfos promise result " + JSON.stringify(result));
            });

        wifi.getScanInfos(
            (err,result) => {
                if(err) {
                    console.log("[wifi_test] wifi getScanInfos failed " + err);
                }
                let clen = Object.keys(result).length;
                if (!(clen == 0)) {
                    expect(clen).assertLarger(0);
                    console.info("[wifi_test] getScanInfos callback result: " + JSON.stringify(result));
                    for (let j = 0; j < clen; ++j) {
                        console.info("ssid: " + result[j].ssid + "bssid: " + result[j].bssid + 
                        "securityType: " + result[j].securityType + 
                         "rssi: " + result[j].rssi + "band: " + result[j].band + 
                          "frequency: " + result[j].frequency +
                          "timestamp" + result[j].timestamp + "capabilities" + result[j].capabilities
                          + "channelWidth: " + result[j].channelWidth);
                    }
                }
                done()
            });
    })

    /**
     * @tc.number     Scan_0004
     * @tc.name       SUB_Communication_WiFi_Sta_Scan_0004
     * @tc.desc       Test wifi.getSignalLevel API functionality.
     */
    it('SUB_Communication_WiFi_Sta_Scan_0004', 0, async function(done) {
        await tryToEnableWifi();
        console.info("[wifi_test] check the 2.4G rssi assgined to level test.");
        expect(wifi.getSignalLevel(-65, 1)).assertEqual(4);

        expect(wifi.getSignalLevel(-66, 1)).assertEqual(3);
        expect(wifi.getSignalLevel(-75, 1)).assertEqual(3);

        expect(wifi.getSignalLevel(-76, 1)).assertEqual(2);
        expect(wifi.getSignalLevel(-82, 1)).assertEqual(2);

        expect(wifi.getSignalLevel(-83, 1)).assertEqual(1);
        expect(wifi.getSignalLevel(-88, 1)).assertEqual(1);

        expect(wifi.getSignalLevel(-89, 1)).assertEqual(0);
        expect(wifi.getSignalLevel(-127, 1)).assertEqual(0);

        console.info("[wifi_test] check the 5G rssi assgined to level test.");
        expect(wifi.getSignalLevel(-65, 2)).assertEqual(4);

        expect(wifi.getSignalLevel(-66, 2)).assertEqual(3);
        expect(wifi.getSignalLevel(-72, 2)).assertEqual(3);

        expect(wifi.getSignalLevel(-73, 2)).assertEqual(2);
        expect(wifi.getSignalLevel(-79, 2)).assertEqual(2);

        expect(wifi.getSignalLevel(-80, 2)).assertEqual(1);
        expect(wifi.getSignalLevel(-85, 2)).assertEqual(1);

        expect(wifi.getSignalLevel(-86, 2)).assertEqual(0);
        expect(wifi.getSignalLevel(-127, 2)).assertEqual(0);
        done();
    })

    /**
     * @tc.number     Config_0001
     * @tc.name       SUB_Communication_WiFi_Sta_Config_0001
     * @tc.desc       Test create a OPEN SecurityType wifi device config
     */
    it('SUB_Communication_WiFi_Sta_Config_0001', 0, async function(done) {
        await tryToEnableWifi();
        wifi.removeAllNetwork();
        let wifiDeviceConfig1 = {
            "ssid": "TEST_OPEN",
            "bssid": "",
            "preSharedKey": "",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_OPEN,
            "netId": -1,
            "ipType": IpType.DHCP,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": ipConfig,    
        };

        await wifi.addDeviceConfig(wifiDeviceConfig1)
            .then(netWorkId => {
                console.info("[wifi_test]add OPEN Deviceconfig promise : " + JSON.stringify(netWorkId));
                expect(true).assertEqual(netWorkId != -1);
                
                let configs = wifi.getDeviceConfigs();
                console.info("[wifi_test] wifi getDeviceConfigs result1 : " + JSON.stringify(configs));
                expect(true).assertEqual(configs[0].securityType == wifiDeviceConfig1.securityType);
                expect(true).assertEqual(configs[0].isHiddenSsid == wifiDeviceConfig1.isHiddenSsid);
                expect(true).assertEqual(configs[0].ssid == wifiDeviceConfig1.ssid);
            });

        wifi.addDeviceConfig(wifiDeviceConfig1,
            (err, netWorkId) => {
                if(err) {
                    console.info("[wifi_test]add OPEN callback failed : " + JSON.stringify(err));
                    return;
                 }
                console.info("[wifi_test]add OPEN Deviceconfig callback : " + JSON.stringify(netWorkId));
                expect(true).assertEqual(netWorkId != -1);
                
                let configs1 = wifi.getDeviceConfigs();
                console.info("[wifi_test] wifi getDeviceConfigs result2 : " + JSON.stringify(configs1));
                expect(true).assertEqual(configs1[0].securityType == wifiDeviceConfig1.securityType);
                expect(true).assertEqual(configs1[0].isHiddenSsid == wifiDeviceConfig1.isHiddenSsid);
                expect(true).assertEqual(configs1[0].ssid == wifiDeviceConfig1.ssid);

                let isRemoved = wifi.removeAllNetwork();
                expect(isRemoved).assertTrue();
                let configs2 = wifi.getDeviceConfigs();
                expect(true).assertEqual(configs2.length == 0);
                done()
            });
    })

    /**
     * @tc.number     Config_0002
     * @tc.name       SUB_Communication_WiFi_Sta_Config_0002
     * @tc.desc       Test create a PSK SecurityType wifi device config
     */
    it('SUB_Communication_WiFi_Sta_Config_0002', 0, async function(done) {
        await tryToEnableWifi();
        
        let wifiDeviceConfig1 = {
            "ssid": "TEST_PSK",
            "bssid": "",
            "preSharedKey": "12345678",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };

        await wifi.addDeviceConfig(wifiDeviceConfig1)
            .then(netWorkId => {
                console.info("[wifi_test]add PSK Deviceconfig promise : " + JSON.stringify(netWorkId));
                expect(true).assertEqual(netWorkId != -1);

                let configs = wifi.getDeviceConfigs();
                console.info("[wifi_test] wifi getDeviceConfigs result1 : " + JSON.stringify(configs));
                expect(true).assertEqual(configs[0].securityType == wifiDeviceConfig1.securityType);
                expect(true).assertEqual(configs[0].isHiddenSsid == wifiDeviceConfig1.isHiddenSsid);
                expect(true).assertEqual(configs[0].preSharedKey == wifiDeviceConfig1.preSharedKey);
                expect(true).assertEqual(configs[0].ssid == wifiDeviceConfig1.ssid);

                let isRemoved = wifi.removeDevice(netWorkId);
                expect(isRemoved).assertTrue();
                let configs1 = wifi.getDeviceConfigs();
                console.info("[wifi_test]remove config,current getConfig: " + JSON.stringify(configs1));
                expect(true).assertEqual(configs1.length == 0);
            });
        done()
    })
    
    /**
     * @tc.number     Config_0003
     * @tc.name       SUB_Communication_WiFi_Sta_Config_0003
     * @tc.desc       Test create a WEP SecurityType wifi device config
     */
    it('SUB_Communication_WiFi_Sta_Config_0003', 0, async function(done) {
        await tryToEnableWifi();
        
        let wifiDeviceConfig1 = {
            "ssid": "TEST_WEP",
            "bssid": "",
            "preSharedKey": "ABCDEF1234",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_WEP,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };

        await wifi.addDeviceConfig(wifiDeviceConfig1)
            .then(netWorkId => {
                console.info("[wifi_test]add WEP Deviceconfig promise : " + JSON.stringify(netWorkId));
                expect(true).assertEqual(netWorkId != -1);

                let configs = wifi.getDeviceConfigs();
                console.info("[wifi_test]connectdevice result: " + wifi.connectToNetwork(netWorkId));
                expect(wifi.connectToNetwork(netWorkId)).assertTrue();
                console.info("[wifi_test] wifi getDeviceConfigs result1 : " + JSON.stringify(configs));
                expect(true).assertEqual(configs[0].securityType == wifiDeviceConfig1.securityType);
                expect(true).assertEqual(configs[0].isHiddenSsid == wifiDeviceConfig1.isHiddenSsid);
                expect(true).assertEqual(configs[0].preSharedKey == wifiDeviceConfig1.preSharedKey);
                expect(true).assertEqual(configs[0].ssid == wifiDeviceConfig1.ssid);

                let isRemoved = wifi.removeDevice(netWorkId);
                expect(isRemoved).assertTrue();
                let configs1 = wifi.getDeviceConfigs();
                expect(true).assertEqual(configs1.length == 0);
            });
        done()
    })

    /**
     * @tc.number     Config_0004
     * @tc.name       SUB_Communication_WiFi_Sta_Config_0004
     * @tc.desc       Test remove all wifi device config
     */
    it('SUB_Communication_WiFi_Sta_Config_0004', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfig1 = {
            "ssid": "TESTWgr1",
            "bssid": "",
            "preSharedKey": "123456789",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
        };
        let wifiDeviceConfig2 = {
            "ssid": "TESTWgr2",
            "bssid": "",
            "preSharedKey": "",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_OPEN,
        };
        let promiseOne = new Promise((resolve, reject) => {
            wifi.addDeviceConfig(wifiDeviceConfig1,
                (err,netWorkId1) => {
                    if(err) {
                        console.info("[wifi_test]add callback failed : " + JSON.stringify(err));
                        return;
                    }
                    console.info("[wifi_test] wifi addDeviceconfig1 callback:" + JSON.stringify(netWorkId1));
                    expect(true).assertEqual(netWorkId1 != -1);

                    let configs = wifi.getDeviceConfigs();
                    console.info("[wifi_test] wifi getDeviceConfigs result : " + JSON.stringify(configs));
                    resolve()
                });
        })
        let promiseTwo = new Promise((resolve, reject) => {
            wifi.addDeviceConfig(wifiDeviceConfig2,
                (err,netWorkId2) => {
                    if(err) {
                        console.info("[wifi_test]add callback failed : " + JSON.stringify(err));
                        return;
                    }
                    console.info("[wifi_test] wifi addDeviceconfig2 callback : " + JSON.stringify(netWorkId2));
                    expect(true).assertEqual(netWorkId2 != -1);

                    let configs1 = wifi.getDeviceConfigs();
                    console.info("[wifi_test] wifi getDeviceConfigs result : " + JSON.stringify(configs1));
                    expect(true).assertEqual(configs1.length >= 1);

                    let isRemoved = wifi.removeAllNetwork();
                    expect(isRemoved).assertTrue();
                    let configs2 = wifi.getDeviceConfigs();
                    expect(true).assertEqual(configs2.length == 0);
                    resolve()
                });
        })
        await promiseOne.then(()=>{
            return promiseTwo
        }).then(done)
    })

    /**
     * @tc.number     Config_0005
     * @tc.name       SUB_Communication_WiFi_Sta_Config_0005
     * @tc.desc       Test disableNetwork device config
     */
     it('SUB_Communication_WiFi_Sta_Config_0005', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfigD = {
            "ssid": "TESTD",
            "bssid": "",
            "preSharedKey": "12345678",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        await wifi.addDeviceConfig(wifiDeviceConfigD)
            .then(netWorkId => {
                console.info("[wifi_test]add PSK Deviceconfig promise : " + JSON.stringify(netWorkId));
                expect(true).assertEqual(netWorkId != -1);
                expect(wifi.connectToNetwork(netWorkId)).assertTrue();
                let disconNet = wifi.disableNetwork(netWorkId);
                expect(disconNet).assertTrue();

                let configs = wifi.getDeviceConfigs();
                console.info("[wifi_test] wifi device config result : " + JSON.stringify(configs));
                expect(true).assertEqual(configs[0].ssid == wifiDeviceConfigD.ssid);

                let isRemoved = wifi.removeAllNetwork();
                expect(isRemoved).assertTrue();
                let configs1 = wifi.getDeviceConfigs();
                expect(true).assertEqual(configs1.length == 0);
            });
        done()
    })
    
    /**
     * @tc.number     Config_0006
     * @tc.name       SUB_Communication_WiFi_Sta_Config_0006
     * @tc.desc       Test update wifi device config
     */
    it('SUB_Communication_WiFi_Sta_Config_0006', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfigU = {
            "ssid": "TEST",
            "bssid": "",
            "preSharedKey": "12345678",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        await wifi.addDeviceConfig(wifiDeviceConfigU)
            .then(netWorkId => {
                console.info("[wifi_test]add PSK Deviceconfig promise : " + JSON.stringify(netWorkId));
                expect(true).assertEqual(netWorkId != -1);
               
                let configs = wifi.getDeviceConfigs();
                console.info("[wifi_test] wifi getDeviceConfigs result1 : " + JSON.stringify(configs));

                wifiDeviceConfigU.ssid = "UPDATE";
                wifiDeviceConfigU.preSharedKey = "1234567890";
                expect(true).assertEqual(wifi.updateNetwork(wifiDeviceConfigU) >= 0);
                let configs1 = wifi.getDeviceConfigs();
                console.info("[wifi_test] updated wifi device config result : " + JSON.stringify(configs1));
                expect(true).assertEqual(configs1[1].ssid == wifiDeviceConfigU.ssid);
                expect(true).assertEqual(configs1[1].preSharedKey == wifiDeviceConfigU.preSharedKey);

                let isRemoved = wifi.removeAllNetwork();
                expect(isRemoved).assertTrue();
                let configs2 = wifi.getDeviceConfigs();
                expect(true).assertEqual(configs2.length == 0);
            });
        done()
    })

    /**
     * @tc.number     Config_SSID_0001
     * @tc.name       SSUB_Communication_WiFi_Sta_Conn_SSID_0001
     * @tc.desc       Test SSID wifi device config
     */
    it('SSUB_Communication_WiFi_Sta_Conn_SSID_0001', 0, async function(done) {
        await tryToEnableWifi();

        let wifiDeviceConfigZ = {
            "ssid": "中文测试SSID",
            "bssid": "",
            "preSharedKey": "12345678",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        await wifi.addDeviceConfig(wifiDeviceConfigZ)
            .then(netWorkId => {
                console.info("[wifi_test]add contains chinese Deviceconfig promise : " + JSON.stringify(netWorkId));
                expect(true).assertEqual(netWorkId != -1);

                expect(wifi.connectToNetwork(netWorkId)).assertTrue();

                let disconNet = wifi.disableNetwork(netWorkId);
                expect(disconNet).assertTrue();

                let configs = wifi.getDeviceConfigs();
                console.info("[wifi_test] wifi getDeviceConfigs result1 : " + JSON.stringify(configs));
                expect(true).assertEqual(configs[0].ssid == wifiDeviceConfigZ.ssid);
                expect(true).assertEqual(configs[0].preSharedKey == wifiDeviceConfigZ.preSharedKey);

                let isRemoved = wifi.removeAllNetwork();
                expect(isRemoved).assertTrue();
                let configs1 = wifi.getDeviceConfigs();
                console.info("[wifi_test]remove config,current get Config : " + JSON.stringify(configs1));
                expect(true).assertEqual(configs1.length == 0);
            });
        done()
    })


    /**
     * @tc.number     Config_SSID_0002
     * @tc.name       SSUB_Communication_WiFi_Sta_Conn_SSID_0002
     * @tc.desc       Test SSID wifi device config
     */
    it('SSUB_Communication_WiFi_Sta_Conn_SSID_0002', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfigK = {
            "ssid": "test text",
            "bssid": "",
            "preSharedKey": "12345678",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };

        await wifi.addDeviceConfig(wifiDeviceConfigK)
            .then(netWorkId => {
                console.info("[wifi_test]add kongge SSID Deviceconfig promise : " + JSON.stringify(netWorkId));
                expect(true).assertEqual(netWorkId != -1);
                
                expect(wifi.connectToNetwork(netWorkId)).assertTrue();
                
                let disconNet = wifi.disableNetwork(netWorkId);
                expect(disconNet).assertTrue();
                
                let configs = wifi.getDeviceConfigs();
                console.info("[wifi_test] wifi getDeviceConfigs result1 : " + JSON.stringify(configs));
                expect(true).assertEqual(configs[0].ssid == wifiDeviceConfigK.ssid);
                expect(true).assertEqual(configs[0].preSharedKey == wifiDeviceConfigK.preSharedKey);

                let isRemoved = wifi.removeAllNetwork();
                expect(isRemoved).assertTrue();
                let configs1 = wifi.getDeviceConfigs();
                expect(true).assertEqual(configs1.length == 0);
            });
        done()
    })

    /**
     * @tc.number     Config_SSID_0003
     * @tc.name       SSUB_Communication_WiFi_Sta_Conn_SSID_0003
     * @tc.desc       Test contains chinese SSID device config
     */
    it('SSUB_Communication_WiFi_Sta_Conn_SSID_0003', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfigT = {
            "ssid": "！@#￥%&*/",
            "bssid": "",
            "preSharedKey": "12345678",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };

        await wifi.addDeviceConfig(wifiDeviceConfigT)
            .then(netWorkId => {
              
                expect(true).assertEqual(netWorkId != -1);
                console.info("[wifi_test]connectdevice result: " + wifi.connectToNetwork(netWorkId));
                let configs = wifi.getDeviceConfigs();
                console.info("[wifi_test] wifi getDeviceConfigs result1 : " + JSON.stringify(configs));
                expect(true).assertEqual(configs[0].ssid == wifiDeviceConfigT.ssid);
                expect(true).assertEqual(configs[0].preSharedKey == wifiDeviceConfigT.preSharedKey);

                let isRemoved = wifi.removeAllNetwork();
                expect(isRemoved).assertTrue();

                let configs1 = wifi.getDeviceConfigs();
                console.info("[wifi_test]remove config,current get Config : " + JSON.stringify(configs1));
                expect(true).assertEqual(configs1.length == 0);
            });
        done()
    })

    /**
     * @tc.number     Config_SSID_0007
     * @tc.name       SSUB_Communication_WiFi_Sta_Conn_SSID_0007
     * @tc.desc       Test hidden SSID wifi device config
     */
    it('SUB_Communication_WiFi_Sta_Conn_SSID_0007', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfigT = {
            "ssid": "test",
            "bssid": "",
            "preSharedKey": "12345678",
            "isHiddenSsid": true,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };

        await wifi.addDeviceConfig(wifiDeviceConfigT)
            .then(netWorkId => {
                
                expect(true).assertEqual(netWorkId != -1);
                console.info("[wifi_test]connectdevice result: " + wifi.connectToNetwork(netWorkId));
                let configs = wifi.getDeviceConfigs();
                console.info("[wifi_test] wifi getDeviceConfigs result1 : " + JSON.stringify(configs));
                expect(true).assertEqual(configs[0].ssid == wifiDeviceConfigT.ssid);
                expect(true).assertEqual(configs[0].isHiddenSsid == wifiDeviceConfigT.isHiddenSsid);

                let isRemoved = wifi.removeAllNetwork();
                expect(isRemoved).assertTrue();
                let configs1 = wifi.getDeviceConfigs();
                console.info("[wifi_test]remove config,current get Config : " + JSON.stringify(configs1));
                expect(true).assertEqual(configs1.length == 0);
            });
        done()
    })

    /**
     * @tc.number SUB_Communication_WiFi_Sta_info_0001
     * @tc.name testgetMacAddress
     * @tc.desc Test getMacAddress api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Sta_info_0001', 0, async function (done) {
       await tryToEnableWifi();
       expect(wifi.isWifiActive()).assertTrue();
       console.info('[wifi_test] getDeviceMacAddress test start ...');
       let result = wifi.getDeviceMacAddress();
       console.info("[wifi_test] getDeviceMacAddress -> " + JSON.stringify(result));
       expect(true).assertEqual(result.length >= 1)
       done();
   })

    /**
     * @tc.number SUB_Communication_WiFi_Sta_info_0002
     * @tc.name testgetCountryCode
     * @tc.desc Test getCountryCode api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Sta_Info_0002', 0, async function(done) {
        await tryToEnableWifi();
        expect(wifi.isWifiActive()).assertTrue();
        console.info(" [wifi_test] getCountryCode start ... ");
        let countryCode = wifi.getCountryCode();
        console.info("[wifi_test] getCountryCode -> " + JSON.stringify(countryCode));
        expect(JSON.stringify(countryCode)).assertEqual('"CN"');
        done()
    })
   
    /**
     * @tc.number SUB_Communication_WiFi_Sta_info_0004
     * @tc.name testFeatureSupported
     * @tc.desc Test FeatureSupported api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Sta_info_0004', 0, async function (done) {
        await tryToEnableWifi();
        expect(wifi.isWifiActive()).assertTrue();
        let result = wifi.getSupportedFeatures();
        console.info("[wifi_test] getFeatureSupported -> " + JSON.stringify(result));
        console.info("[wifi_test] isFeatureSupported: " + result +"->" + wifi.isFeatureSupported(result));
        expect(wifi.isFeatureSupported(result)).assertTrue();
        done();
    })

    /**
     * @tc.number SUB_Communication_WiFi_Sta_info_0005
     * @tc.name testHotspotDualBandSupported
     * @tc.desc Test HotspotDualBandSupported api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Sta_Info_0005', 0, async function(done) {
        console.info(" [wifi_test] isHotspotDualBandSupported start ... ");
        await tryToEnableWifi();
        expect(wifi.isWifiActive()).assertTrue();
        let DualBand = wifi.isHotspotDualBandSupported();
        console.info("[wifi_test] isHotspotDualBandSupported -> " + JSON.stringify(DualBand));
        expect(DualBand).assertFalse();
        done();
        
    })

    /**
    * @tc.number     Conn_Info_0001
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Info_0001
    * @tc.desc       Test reconnect wifi
    */
   it('SUB_Communication_WiFi_Sta_Conn_Info_0001', 0, async function (done) {
        await tryToEnableWifi();
        let wifiDeviceConfigConn = {
            "ssid": "TESTCONN",
            "bssid": "",
            "preSharedKey": "12345678",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        expect(wifi.isWifiActive()).assertTrue();
        let result1 = wifi.connectToDevice(wifiDeviceConfigConn);
        expect(result1).assertTrue();
        console.info("[wifi_test]isConnected : " + wifi.isConnected());
        expect(wifi.isConnected()).assertFalse();
        expect(wifi.reconnect()).assertTrue();
        let isRemoved = wifi.removeAllNetwork();
        expect(isRemoved).assertTrue();
        let configs = wifi.getDeviceConfigs();
        console.info("[wifi_test]remove config,current get Config : " + JSON.stringify(configs));
        expect(true).assertEqual(configs.length == 0);
        done()

    })
    
    /**
     * @tc.number     conn_Config_0002
     * @tc.name       SUB_Communication_WiFi_Sta_Conn_Info_0002
     * @tc.desc       Test getLinkedInfo information
     */
    it('SUB_Communication_WiFi_Sta_Conn_Info_0002', 0, async function(done) {
        await tryToEnableWifi();
        await wifi.getLinkedInfo()
            .then((result) => {
                console.info("[wifi_test] get wifi link [promise] -> " + JSON.stringify(result));
                expect(JSON.stringify(result)).assertContain('band');
                done();
            }).catch((error) => {
                console.info("[wifi_test] promise then error." + JSON.stringify(error));
                expect().assertFail();
            });
    })

    /**
     * @tc.number     conn_Config_0003
     * @tc.name       SUB_Communication_WiFi_Sta_Conn_Info_0003
     * @tc.desc       Test getLinkedInfo callback information
     */
    it('SUB_Communication_WiFi_Sta_Conn_Info_0003', 0, async function(done) {
        await tryToEnableWifi();
        await wifi.getLinkedInfo(
            (err,result) => {
                if(err) {
                    console.log("[wifi_test] wifi getLinkedInfo failed " + err);
                }
                let clen = Object.keys(result).length;
                expect(clen).assertLarger(0);
                console.info("[wifi_test] getLinkedInfo callback result: " + JSON.stringify(result));
                console.info("ssid: " + result.ssid + "bssid:"+ result.bssid +"band: " + result.band+
                "isHidden: " + result.isHidden + "isRestricted: " + result.isRestricted +
                "chload: " + result.chload + "rssi " + result.rssi + "netWorkId: " + result.netWorkId+ 
                "linkSpeed: " + result.linkSpeed + "frequency:" 
                 + result.frequency +"snr:" + result.snr+
                "macAddress: " + result.macAddress + "ipAddress: " + result.ipAddress + 
                "suppState: " + result.suppState + "connState: " + result.connState);
                done();
            });   
    })

   /**
    * @tc.number     Conn_Security_0001
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Security_0001
    * @tc.desc       Test connectToDevice 10 bits WEP Deviceconfig
    */
    it('SUB_Communication_WiFi_Sta_Conn_Security_0001', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfig1 = {
            "ssid": "TEST10",
            "bssid": "",
            "preSharedKey": "ABCDEF1234",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_WEP,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        let result1 = wifi.connectToDevice(wifiDeviceConfig1);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();
        let isConnected= wifi.isConnected();
        expect(isConnected).assertFalse();
        let isRemoved = wifi.removeAllNetwork();
        console.info("[wifi_test] check remove configs successfully,result:" + isRemoved);
        expect(isRemoved).assertTrue();
        done()
    })

   /**
    * @tc.number     Conn_Security_0002
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Security_0002
    * @tc.desc       Test disconnect Deviceconfig
    */
    it('SUB_Communication_WiFi_Sta_Conn_Security_0002', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfig1 = {
            "ssid": "TESTDIS",
            "bssid": "",
            "preSharedKey": "12345678",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        let result1 = wifi.connectToDevice(wifiDeviceConfig1);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();
  
        let isConnected= wifi.isConnected();
        expect(isConnected).assertFalse();
 
        expect(wifi.disconnect()).assertTrue();
        let isRemoved = wifi.removeAllNetwork();
        console.info("[wifi_test] check remove configs successfully,result:" + isRemoved);
        expect(isRemoved).assertTrue();
        done()
    })

   /**
    * @tc.number     Conn_Security_0003
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Security_0003
    * @tc.desc       Test connectToDevice 26 bits WEP Deviceconfig
    */
    it('SUB_Communication_WiFi_Sta_Conn_Security_0003', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfig1 = {
            "ssid": "TEST26",
            "bssid": "",
            "preSharedKey": "12345678901234567890123456",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_WEP,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        let result1 = wifi.connectToDevice(wifiDeviceConfig1);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();

        let isConnected= wifi.isConnected();
        expect(isConnected).assertFalse();
        let isRemoved = wifi.removeAllNetwork();
        console.info("[wifi_test] check remove configs successfully,result:" + isRemoved);
        expect(isRemoved).assertTrue();
        done()
    })

    /**
    * @tc.number     Conn_Security_0005
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Security_0005
    * @tc.desc       Test connectToDevice 5bits ASCLL WEP Deviceconfig
    */
    it('SUB_Communication_WiFi_Sta_Conn_Security_0005', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfig1 = {
            "ssid": "TEST5",
            "bssid": "",
            "preSharedKey": "aB1@g",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_WEP,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        let result1 = wifi.connectToDevice(wifiDeviceConfig1);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();

        let isConnected= wifi.isConnected();
        expect(isConnected).assertFalse();
        let isRemoved = wifi.removeAllNetwork();
        console.info("[wifi_test] check remove configs successfully,result:" + isRemoved);
        expect(isRemoved).assertTrue();
        done()
    })

   /**
    * @tc.number     Conn_Security_0007
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Security_0007
    * @tc.desc       Test connectToDevice 13bits ASCLL WEP Deviceconfig
    */
    it('SUB_Communication_WiFi_Sta_Conn_Security_0007', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfig1 = {
            "ssid": "TEST7",
            "bssid": "",
            "preSharedKey": "1234aA@fFgGzZ",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_WEP,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        let result1 = wifi.connectToDevice(wifiDeviceConfig1);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();
        console.info("[wifi_test] check isconnected wifi");
        let isConnected= wifi.isConnected();
        expect(isConnected).assertFalse();
        let isRemoved = wifi.removeAllNetwork();
        console.info("[wifi_test] check remove configs successfully,result:" + isRemoved);
        expect(isRemoved).assertTrue();
        done()
    })

   /**
    * @tc.number     Conn_Security_0013
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Security_0013
    * @tc.desc       Test connectToDevice 63bits ASCLL PSK Deviceconfig
    */
    it('SUB_Communication_WiFi_Sta_Conn_Security_0013', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfig1 = {
            "ssid": "TEST13",
            "bssid": "",
            "preSharedKey": "ABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGH1234567",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        let result1 = wifi.connectToDevice(wifiDeviceConfig1);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();

        let isConnected= wifi.isConnected();
        expect(isConnected).assertFalse();
        let isRemoved = wifi.removeAllNetwork();
        console.info("[wifi_test] check remove configs successfully,result:" + isRemoved);
        expect(isRemoved).assertTrue();
        done()
    })

   /**
    * @tc.number     Conn_Security_0014
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Security_0014
    * @tc.desc       Test connectToDevice 8bits ASCLL PSK Deviceconfig
    */
    it('SUB_Communication_WiFi_Sta_Conn_Security_0014', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfig1 = {
            "ssid": "TEST13",
            "bssid": "",
            "preSharedKey": "ABCDEFGH",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        let result1 = wifi.connectToDevice(wifiDeviceConfig1);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();

        let isConnected= wifi.isConnected();
        expect(isConnected).assertFalse();
        let isRemoved = wifi.removeAllNetwork();
        console.info("[wifi_test] check remove configs successfully,result:" + isRemoved);
        expect(isRemoved).assertTrue();
        done()
    })

   /**
    * @tc.number     Conn_Security_0015
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Security_0015
    * @tc.desc       Test connectToDevice 63bits ASCLL PSK Deviceconfig
    */
    it('SUB_Communication_WiFi_Sta_Conn_Security_0015', 0, async function(done) {
        await tryToEnableWifi();
        let wifiDeviceConfig1 = {
            "ssid": "TEST13",
            "bssid": "",
            "preSharedKey": "ABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGH1234567",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        let result1 = wifi.connectToDevice(wifiDeviceConfig1);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();
        console.info("[wifi_test] check isconnected wifi");
        let isConnected= wifi.isConnected();
        expect(isConnected).assertFalse();
        let isRemoved = wifi.removeAllNetwork();
        console.info("[wifi_test] check remove configs successfully,result:" + isRemoved);
        expect(isRemoved).assertTrue();
        done()
    })

    /**
    * @tc.number     Conn_Info_0003
    * @tc.name       SUB_Communication_WiFi_Sta_Conn_Info_0003
    * @tc.desc       Test get IpInfo information
    */
    it('SUB_Communication_WiFi_Sta_Conn_Info_0003', 0, async function (done) {
        await tryToEnableWifi();
        let wifiDeviceConfigIp = {
            "ssid": "TEST1",
            "bssid": "",
            "preSharedKey": "123456789",
            "isHiddenSsid": false,
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "netId": -1,
            "ipType": 1,
            "creatorUid": 7,
            "disableReason": 0,
            "randomMacType": 0,
            "randomMacAddr": "11:22:33:44:55:66",
            "staticIp": {"ipAddress": 1284752956,"gateway": 1284752936},
        };
        let result1 = wifi.connectToDevice(wifiDeviceConfigIp);
        console.log("[wifi_test] wifi connectToDevice result: " + result1);
        expect(result1).assertTrue();

        let isConnected= wifi.isConnected();
        expect(isConnected).assertFalse();

        let reass= wifi.reassociate();
        expect(reass).assertTrue();

        let ipInfo = wifi.getIpInfo();
        expect(JSON.stringify(ipInfo)).assertContain("gateway");
        console.info("gateway: " + ipInfo.gateway + "ipAddress: " + ipInfo.ipAddress
         + "leaseDuration: " + ipInfo.leaseDuration + 
        "leaseDuration: " + ipInfo.leaseDuration + 
        "netmask: " + ipInfo.netmask + "primaryDns:" + ipInfo.primaryDns +
        "secondDns: " + ipInfo.secondDns + "serverIp: " + ipInfo.serverIp );
        
        let isRemoved = wifi.removeAllNetwork();
        expect(isRemoved).assertTrue();
        let configs = wifi.getDeviceConfigs();
        console.info("[wifi_test]remove config,current get Config : " + JSON.stringify(configs));
        expect(true).assertEqual(configs.length == 0);
        done()
    })

   /**
    * @tc.number     wifiStateChange_0001
    * @tc.name       SUB_Communication_WiFi_Sta_wifiStateChange_0001
    * @tc.desc       Test wifiStateChange callback
    */
    it('SUB_Communication_WiFi_Sta_wifiStateChange_0001', 0, async function (done) {
        await tryToEnableWifi();
        wifi.on('wifiStateChange', async result => {
             console.info("wifiStateChange callback, result:" + JSON.stringify(result));
             expect(true).assertEqual(result !=null);
             let promise = new Promise((resolve) => {
                wifi.off('wifiStateChange', result => {
                    console.info("offwifiStateChange callback, result: " + JSON.stringify(result));
                    expect(true).assertEqual(result !=null);
                    resolve()
                });
            })
             await promise.then(done)
        });
        let disable = wifi.disableWifi()
        await sleep(5000);
        let enable = wifi.enableWifi();
        await sleep(5000);
        console.log("[wifi_test]  check the state of wifi: " + wifi.isWifiActive());   
        done();  
       
    })

  /**
    * @tc.number     wifiConnectionChange_0002
    * @tc.name       SUB_Communication_WiFi_Sta_wifiConnectionChange_0002
    * @tc.desc       Test wifiStateChange callback
    */
    it('SUB_Communication_WiFi_Sta_wifiConnectionChange_0002', 0, async function (done) {
        await tryToEnableWifi();
        wifi.on('wifiConnectionChange', async result => {
            console.info("wifiConnectionChange callback, result:" + JSON.stringify(result));
            expect(true).assertEqual(result !=null);
            let promise = new Promise((resolve) => {
                console.info('[wifi_test] offwifiConnectionChange test start ...');
                wifi.off('wifiConnectionChange', result => {
                    console.info("offwifiConnectionChange callback, result:  " + JSON.stringify(result));
                    expect(true).assertEqual(result !=null);
                    resolve()
                });
            })
            await promise.then(done)
         });   
        done();  
       
    })

   /**
    * @tc.number     wifiScanStateChange_0003
    * @tc.name       SUB_Communication_WiFi_Sta_wifiScanStateChange_0003
    * @tc.desc       Test wifiScanStateChange callback
    */
    it('SUB_Communication_WiFi_Sta_wifiScanStateChange_0003', 0, async function (done) {
        await tryToEnableWifi();
        wifi.on('wifiScanStateChange', async result => {
             console.info("wifiScanStateChange callback, result:" + JSON.stringify(result));
             expect(true).assertEqual(result !=null);
             let promise = new Promise((resolve) => {
                console.info('[wifi_test] offwifiScanStateChange test start ...');
                wifi.off('wifiScanStateChange', result => {
                    console.info("offwifiScanStateChange callback, result:  " + JSON.stringify(result));
                    expect(true).assertEqual(result !=null);
                    resolve()
                });
            })
             await promise.then(done)
         });  
        let scan = wifi.scan();
        await sleep(3000);
        done();  
       
    })
 
   /**
    * @tc.number     wifiRssiChange_0004
    * @tc.name       SUB_Communication_WiFi_Sta_wifiRssiChange_0004
    * @tc.desc       Test wifiRssiChange callback
    */
        it('SUB_Communication_WiFi_Sta_wifiRssiChange_0004', 0, async function (done) {
            await tryToEnableWifi(); 
            wifi.on('wifiRssiChange', async result => {
                 console.info("wifiRssiChange callback, result:" + JSON.stringify(result));
                 expect(true).assertEqual(result !=null);
                 let promise = new Promise((resolve) => {
                    console.info('[wifi_test] offwifiRssiChange test start ...');
                    wifi.off('wifiRssiChange', result => {
                        console.info("offwifiRssiChange callback, result:  " + JSON.stringify(result));
                        expect(true).assertEqual(result !=null);
                        resolve()
                    });
                })
                 await promise.then(done)
             });  
            done();  
           
        })
 
   /**
    * @tc.number     streamChange_0005
    * @tc.name       SUB_Communication_WiFi_Sta_wifiRssiChange_0005
    * @tc.desc       Test streamChange callback
    */
    it('SUB_Communication_WiFi_Sta_streamChange_0005', 0, async function (done) {
        await tryToEnableWifi();

        wifi.on('streamChange', async result => {
             console.info("streamChange callback, result:" + JSON.stringify(result));
             expect(true).assertEqual(result !=null);
             let promise = new Promise((resolve) => {
                console.info('[wifi_test] offstreamChange test start ...');
                wifi.off('streamChange', result => {
                    console.info("offstreamChange callback, result:  " + JSON.stringify(result));
                    expect(true).assertEqual(result !=null);
                    resolve()
                });
            })
             await promise.then(done)
         });  
        done();  
       
    })
    console.log("*************[wifi_test] start wifi js unit test end*************");
})


