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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

import wifi from '@ohos.wifi'

function sleep(delay) {
    return new Promise(resovle => setTimeout(resovle, delay))
}

async function tryToDisableWifi(){
    if(wifi.isWifiActive()){
        console.info("wifi_test/disable wifi:" + wifi.disableWifi());
        await sleep(5000);
    }
    console.info("wifi_test/wifi status:" + wifi.isWifiActive());
}

let WifiSecurityType = {
    WIFI_SEC_TYPE_INVALID : 0,
    WIFI_SEC_TYPE_OPEN : 1,
    WIFI_SEC_TYPE_WEP : 2,
    WIFI_SEC_TYPE_PSK : 3,
    WIFI_SEC_TYPE_SAE : 4,
}

let HotspotConfig = {
    "ssid": "testWgr",
    "band": 1,
    "preSharedKey": "12345678",
    "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
    "maxConn": 8
}

describe('ACTS_WifiTest', function() {
    beforeEach(function () {
    })

    afterEach(function () {
    })

    /**
    * @tc.number     OPEN_0002
    * @tc.name       SUB_Communication_WiFi_Hotspot_Open_0002
    * @tc.desc       Test wifi.enableHotspot and wifi.disableHotspot API functionality1.
    * @tc.level Level 0
    */
    it('SUB_Communication_WiFi_Hotspot_Open_0002', 0, async function (done) {
        if (wifi.isWifiActive()) {
            let disable = wifi.disableWifi();
            await sleep(5000);
            expect(disable).assertTrue();
        }
        let close =wifi.isWifiActive();
        console.info("[wifi_test] wifi status result: " + close);
        expect(close).assertFalse();
        if (!wifi.isHotspotActive()) {
            let on = wifi.enableHotspot();
            await sleep(5000);
            expect(on).assertTrue();
        }
        console.info("[wifi_test] Hotspot status result -> " + wifi.isHotspotActive());
        expect(wifi.isHotspotActive()).assertTrue();
        done();
    })

    /**
    * @tc.number  CONFIG_0001
    * @tc.name    SUB_Communication_WiFi_Hotspot_Config_0001
    * @tc.desc    Test set a valid 2.4G hotspot config
    * @tc.level   Level 0
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0001', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(3000);
            expect(off).assertTrue();
        }
        console.info("[wifi_test] Hotspot status result -> " + wifi.isHotspotActive());
        expect(wifi.isHotspotActive()).assertFalse();
        let isSuccess = wifi.setHotspotConfig(HotspotConfig);
        expect(isSuccess).assertTrue();

        let hotspotConfig = wifi.getHotspotConfig();
        console.log("[wifi_test] getHotspotConfig  result: " + JSON.stringify(hotspotConfig));
        expect(hotspotConfig.band).assertEqual(1);
        done()
    })

    /**
    * @tc.number     CONFIG_0002
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0002
    * @tc.desc       Test set a valid 5G hotspot config
    * @tc.level Level 1
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0002', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(3000);
            expect(off).assertTrue();
        }
        console.info("[wifi_test] Hotspot status result -> " + wifi.isHotspotActive());
        let HotspotConfig5G = {
            "ssid": "testWgr",
            "band": 2,
            "preSharedKey": "12345678",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess5 = wifi.setHotspotConfig(HotspotConfig5G);
        expect(isSuccess5).assertTrue();

        let hotspotConfig5 = wifi.getHotspotConfig();
        console.log("[wifi_test] getHotspotConfig  result: " + JSON.stringify(hotspotConfig5));
        expect(hotspotConfig5.band).assertEqual(2);
        done()
    })

    /**
    * @tc.number     CONFIG_0003
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0003
    * @tc.desc       Test set a invalid band hotspot config
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0003', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(3000);
            console.info("[wifi_test] disableHotspot result -> " + off);
            expect(off).assertTrue();
        }
        let HotspotConfig1= {
            "ssid": "testWgr",
            "band": 21,
            "preSharedKey": "12345678",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfig1);
        console.log("[wifi_test] set invalid band hotspot config  result1:" + isSuccess1);
        expect(isSuccess1).assertFalse();
        done();

    })


    /**
    * @tc.number     CONFIG_0004
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0004
    * @tc.desc       Test set a max preSharedKey is hotspot config
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0004', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(5000);
            expect(off).assertTrue();
        }
        let HotspotConfigM= {
            "ssid": "testWgr",
            "band": 1,
            "preSharedKey": "123456789123456789123456789123456789123456789123456789123456789",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigM);
        console.log("[wifi_test] set max preSharedKey valid hotspot config  result: " + isSuccess1);
        expect(isSuccess1).assertTrue();

        let HotspotConfigM1= {
            "ssid": "testWgr",
            "band": 1,
            "preSharedKey": "1234567891234567891234567891234567891234567891234567891234567891",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess2 = wifi.setHotspotConfig(HotspotConfigM1);
        expect(isSuccess2).assertFalse();
        let hotspotConfigS= wifi.getHotspotConfig();
        console.log("[wifi_test] getHotspotConfig  result: " + JSON.stringify(hotspotConfigS));
        expect(hotspotConfigS.preSharedKey.length).assertEqual(63);
        done();
    })

    /**
    * @tc.number     CONFIG_0005
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0005
    * @tc.desc       Test set a min preSharedKey is hotspot config
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0005', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(5000);
            expect(off).assertTrue();
        }

        let HotspotConfigI= {
            "ssid": "testWgr",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigI);
        expect(isSuccess1).assertTrue();

        let config = wifi.getHotspotConfig();
        console.info("[wifi_test] getHotspotConfig result -> " + JSON.stringify(config));
        console.info("preSharedKey: " + config.preSharedKey);
        expect(config.preSharedKey.length).assertEqual(8);

        let HotspotConfigI2= {
            "ssid": "testWgr",
            "band": 1,
            "preSharedKey": "1234567",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess2 = wifi.setHotspotConfig(HotspotConfigI2);
        expect(isSuccess2).assertFalse();

        let config1 = wifi.getHotspotConfig();
        console.info("[wifi_test] getHotspotConfig result -> " + JSON.stringify(config1));
        expect(config1.preSharedKey.length).assertEqual(8);
        done();
    })

    /**
    * @tc.number     CONFIG_0006
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0006
    * @tc.desc       Test set a max ssid for hotspot config
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0006', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(5000);
            expect(off).assertTrue();
        }
        let isHotspotActive = wifi.isHotspotActive();
        console.info("[wifi_test] isHotspotActive -> " + isHotspotActive);
        expect(isHotspotActive).assertFalse();

        let HotspotConfigS= {
            "ssid": "testWgr123testWgr123testWgr12356",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigS);
        expect(isSuccess1).assertTrue();

        let HotspotConfigS1= {
            "ssid": "testWgr123testWgr123testWgr123567",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess2 = wifi.setHotspotConfig(HotspotConfigS1);
        console.log("[wifi_test] set 33bit ssid invalid hotspot config  result: " + isSuccess2);
        expect(isSuccess2).assertFalse();
        let config = wifi.getHotspotConfig();
        console.info("[wifi_test] getHotspotConfig result -> " + JSON.stringify(config));
        expect(config.ssid.length).assertEqual(32);
        done();

    })

    /**
    * @tc.number     CONFIG_0007
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0007
    * @tc.desc       Test set a null ssid invalid  hotspot config
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0007', 0, async function(done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(2000);
            expect(off).assertTrue();
        }
        let HotspotConfigN= {
            "ssid": "",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigN);
        console.log("[wifi_test] set a null ssid invalid hotspot config  result1:" + isSuccess1);
        expect(isSuccess1).assertFalse();
        done();
    })

    /**
    * @tc.number     CONFIG_0008
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0008
    * @tc.desc       Test set a contains Chinese.special.digits valid hotspot config
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0008', 0, async function(done) {
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(3000);
            expect(off).assertTrue();
        }
        let HotspotConfigC= {
            "ssid": "测试123！@#￥%……&*（ ",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigC);
        expect(isSuccess1).assertTrue();

        let isHotspotActive = wifi.isHotspotActive();
        console.info("[wifi_test] isHotspotActive -> " + isHotspotActive);
        expect(isHotspotActive).assertFalse();
        let config = wifi.getHotspotConfig();
        console.info("[wifi_test] getHotspotConfig result -> " + JSON.stringify(config));
        expect(true).assertEqual(config.ssid==HotspotConfigC.ssid);
        done();
    })

    /**
    * @tc.number     CONFIG_0009
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0009
    * @tc.desc       Test set a invalid preSharedKey hotspot config
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0009', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(3000);
            expect(off).assertTrue();

        }
        let HotspotConfigWep= {
            "ssid": "test123",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType":WifiSecurityType.WIFI_SEC_TYPE_WEP,
            "maxConn": 8
        };
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigWep);
        console.log("[wifi_test] set a invalid preSharedKey hotspot config  result1: " + isSuccess1);
        expect(isSuccess1).assertFalse();

        let HotspotConfigSAE= {
            "ssid": "test123",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType":WifiSecurityType.WIFI_SEC_TYPE_SAE,
            "maxConn": 8
        };
        let isSuccess2 = wifi.setHotspotConfig(HotspotConfigSAE);
        console.log("[wifi_test] set a invalid preSharedKey hotspot config  result1: " + isSuccess2);
        expect(isSuccess2).assertFalse();

        let HotspotConfigC= {
            "ssid": "test123",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType":12,
            "maxConn": 8
        };
        let isSuccess3 = wifi.setHotspotConfig(HotspotConfigC);
        console.log("[wifi_test] set a invalid preSharedKey hotspot config  result1: " + isSuccess3);
        expect(isSuccess3).assertFalse();
        done();

    })

    /**
    * @tc.number     CONFIG_0010
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0010
    * @tc.desc       Test set a valid PSK preSharedKey hotspot config
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0010', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(5000);
            expect(off).assertTrue();
        }

        let HotspotConfigPSK= {
            "ssid": "test123",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType":WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        }
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigPSK);
        expect(isSuccess1).assertTrue();
        let config = wifi.getHotspotConfig();
        console.info("[wifi_test] getHotspotConfig result -> " + JSON.stringify(config));
        expect(config.securityType).assertEqual(3);
        console.log("[wifi_test] check the state of Hotspot" );
        let isHotspotActive = wifi.isHotspotActive();
        console.info("[wifi_test] isHotspotActive -> " + isHotspotActive);
        expect(isHotspotActive).assertFalse();
        done();

    })

    /**
    * @tc.number     CONFIG_0011
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0011
    * @tc.desc       Test set a valid OPEN preSharedKey hotspot config
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0011', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(5000);
            expect(off).assertTrue();
        }

        let HotspotConfigO= {
            "ssid": "test123",
            "band": 1,
            "preSharedKey": "",
            "securityType":WifiSecurityType.WIFI_SEC_TYPE_OPEN,
            "maxConn": 8
        }
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigO);
        console.log("[wifi_test] set a open preSharedKey hotspot config  result1: " + isSuccess1);
        expect(isSuccess1).assertTrue();
        let config = wifi.getHotspotConfig();
        console.info("[wifi_test] getHotspotConfig result -> " + JSON.stringify(config));
        expect(config.securityType).assertEqual(1);
        done();

    })

    /**
    * @tc.number     CONN_0001
    * @tc.name       SUB_Communication_WiFi_Hotspot_Conn_0001
    * @tc.desc       Test set a maxConn and more maxConn hotspot config
    * @tc.level Level 2
    */
     it('SUB_Communication_WiFi_Hotspot_Conn_0001', 0, async function (done) {
        await tryToDisableWifi();
        if (wifi.isHotspotActive()) {
            let off = wifi.disableHotspot();
            await sleep(5000);
            expect(off).assertTrue();

        }
        let HotspotConfigI= {
            "ssid": "testWgr",
            "band": 1,
            "preSharedKey": "12345678",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        };
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigI);
        console.log("[wifi_test] set maxConn valid hotspot config  result: " + isSuccess1);
        expect(isSuccess1).assertTrue();
        let config = wifi.getHotspotConfig();
        console.info("[wifi_test] getHotspotConfig result -> " + JSON.stringify(config));
        expect(config.maxConn).assertEqual(8);

        let HotspotConfigI2= {
            "ssid": "testWgr",
            "band": 1,
            "preSharedKey": "1234567",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 9
        };
        let isSuccess2 = wifi.setHotspotConfig(HotspotConfigI2);
        console.log("[wifi_test] set  more maxConn invalid hotspot config  result: " + isSuccess2);
        expect(isSuccess2).assertFalse();
        let config1 = wifi.getHotspotConfig();
        console.info("[wifi_test] getHotspotConfig result -> " + JSON.stringify(config1));
        expect(config1.maxConn).assertEqual(8);
        done();
    })

    /**
    * @tc.number     CONFIG_0012
    * @tc.name       SUB_Communication_WiFi_Hotspot_Config_0012
    * @tc.desc       Test set a valid hotspot config after open Hotspot
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Config_0012', 0, async function (done) {
        await tryToDisableWifi();
        if (!wifi.isHotspotActive()) {
            let on = wifi.enableHotspot();
            await sleep(5000);
            expect(on).assertTrue();
        }
        let isHotspotActive = wifi.isHotspotActive();
        console.info("[wifi_test] isHotspotActive -> " + isHotspotActive);
        expect(isHotspotActive).assertTrue();
        let HotspotConfigWO= {
            "ssid": "WIFITEST",
            "band": 1,
            "preSharedKey": "123456789",
            "securityType": WifiSecurityType.WIFI_SEC_TYPE_PSK,
            "maxConn": 8
        }
        let isSuccess1 = wifi.setHotspotConfig(HotspotConfigWO);
        console.log("[wifi_test] set a psk preSharedKey hotspot config result1: " + isSuccess1);
        expect(isSuccess1).assertTrue();
        await sleep(5000);
        let config = wifi.getHotspotConfig();
        console.info("[wifi_test] getHotspotConfig result -> " + JSON.stringify(config));
        expect(config.ssid).assertEqual('WIFITEST');
        done();

    })

    
    /**
    * @tc.number     CONN_0002
    * @tc.name       SUB_Communication_WiFi_Hotspot_Conn_0002
    * @tc.desc       Test Obtains the list of clients that are connected to a Wi-Fi hotspot.
    * @tc.level Level 2
    */
    it('SUB_Communication_WiFi_Hotspot_Conn_0002', 0, async function (done) {
        await tryToDisableWifi();
        if (!wifi.isHotspotActive()) {
            let on = wifi.enableHotspot();
            await sleep(5000);
            expect(on).assertTrue();
        }
        let isHotspotActive = wifi.isHotspotActive();
        console.info("[wifi_test] isHotspotActive -> " + isHotspotActive);
        expect(isHotspotActive).assertTrue();
        let stationInfo = wifi.getStations();
        console.info("[wifi_test] getStations result ->" + JSON.stringify(stationInfo));
        console.info("ssid: " + stationInfo.name +
         "macAddress: " + stationInfo.macAddress + 
        "ipAddress: " + stationInfo.ipAddress);
        expect(stationInfo.length).assertEqual(0);
        done();

    })
    
    /**
     * @tc.number SUB_Communication_WiFi_Hotspot_ON_0001
     * @tc.name testhotspotStateChangeOn
     * @tc.desc Test hotspotStateChangeOn api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Hotspot_ON_0001', 0, async function (done) {
        console.info("[wifi_test]hotspotStateChange On test");
        try {
            await wifi.on('hotspotStateChange', (data) => {
                console.info("[wifi_test] hotspotStateChange On ->" + data);
                expect(true).assertEqual(data != null);
            });
            
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })
    
    /**
     * @tc.number SUB_Communication_WiFi_Hotspot_Off_0002
     * @tc.name testhotspotStateChangeOff
     * @tc.desc Test hotspotStateChange api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Sta_Off_0002', 0, async function (done) {
        try {
           await wifi.off('hotspotStateChange', (data) => {
                console.info("[wifi_test] hotspotStateChange Off ->" + data);
                expect(true).assertEqual(data != null);
                console.info("[wifi_test] wifiRssiChange unregister end");
            });
            
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_Communication_WiFi_Hotspot_ON_0003
     * @tc.name testhotspotStaJoinOn
     * @tc.desc Test hotspotStaJoinOn api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Hotspot_ON_0003', 0, async function (done) {
        console.info("[wifi_test]hotspotStaJoin On test");
        try {
          await wifi.on('hotspotStaJoin', (data) => {
                console.info("[wifi_test] hotspotStaJoin On ->" + data);
                expect(true).assertEqual(data != null);
            });
            
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_Communication_WiFi_Hotspot_Off_0004
     * @tc.name testhotspotStaJoinOff
     * @tc.desc Test hotspotStaJoin api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Sta_Off_0004', 0, async function (done) {
        try {
            await wifi.off('hotspotStaJoin', (data) => {
                console.info("[wifi_test] hotspotStaJoin Off ->" + data);
                expect(true).assertEqual(data != null);
                console.info("[wifi_test] wifiRssiChange unregister end");
            });
            
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_Communication_WiFi_Hotspot_ON_0005
     * @tc.name testhotspotStaLeaveOn
     * @tc.desc Test hotspotStaLeaveOn api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Hotspot_ON_0005', 0, async function (done) {
        console.info("[wifi_test]hotspotStaLeave On test");
        try {
           await wifi.on('hotspotStaLeave', (data) => {
                console.info("[wifi_test] hotspotStaLeave On ->" + data);
                expect(true).assertEqual(data != null);
            });
            
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })

    /**
     * @tc.number SUB_Communication_WiFi_Hotspot_Off_0006
     * @tc.name testhotspotStaLeaveOff
     * @tc.desc Test hotspotStaLeave api.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_Communication_WiFi_Hotspot_Off_0006', 0, async function (done) {
        try {
            await wifi.off('hotspotStaLeave', (data) => {
                console.info("[wifi_test] hotspotStaLeave Off ->" + data);
                expect(true).assertEqual(data != null);
                console.info("[wifi_test] hotspotStaLeave unregister end");
            });
            
        }catch(e) {
            expect(null).assertFail();
        }
        done();
    })

    console.log("*************[wifi_test] start wifi js unit test end*************");
})



