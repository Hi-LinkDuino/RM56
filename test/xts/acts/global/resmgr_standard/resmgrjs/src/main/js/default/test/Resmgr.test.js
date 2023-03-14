/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import resmgr from '@ohos.resourceManager'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('resMgrTest', function () {
    console.log('*************start ResmgrTest*************');

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_0100
    * @tc.name test getResourceManager method in callback mode
    * @tc.desc get the resource manager in callback mode
    */
    it('getResourceManager_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            expect(mgr !== null).assertTrue();
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_0200
    * @tc.name test getResourceManager method in callback mode with bundlename
    * @tc.desc get the resource manager in callback mode with bundlename
    */
    it('getResourceManager_test_002', 0, async function (done) {
        resmgr.getResourceManager('com.ohos.settings', (error, mgr) => {
            expect(mgr !== null).assertTrue();
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_0300
    * @tc.name test getResourceManager method in promise mode
    * @tc.desc get the resource manager in promise mode
    */
    it('getResourceManager_test_003', 0, async function (done) {
        resmgr.getResourceManager().then(mgr => {
            expect(mgr !== null).assertTrue();
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_0400
    * @tc.name test getResourceManager method in promise mode with bundlename
    * @tc.desc get the resource manager in promise mode with bundlename
    */
    it('getResourceManager_test_004', 0, async function (done) {
        resmgr.getResourceManager('com.ohos.settings').then(mgr => {
            expect(mgr !== null).assertTrue();
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_0500
    * @tc.name test getString method in callback mode
    * @tc.desc get the string in callback mode
    */
    it('getString_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getString(0x1000000, (err, value) => {
                expect(value !== null).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_0600
    * @tc.name test getString method in promise mode
    * @tc.desc get the string in promise mode
    */
    it('getString_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getString(0x1000000).then(value => {
                expect(value !== null).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_0700
    * @tc.name test getStringArray method in callback mode
    * @tc.desc get the string array in callback mode
    */
    it('getStringArray_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getStringArray(0x1000002, (err, value) => {
                expect(value !== null).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_0800
    * @tc.name test getStringArray method in promise mode
    * @tc.desc get the string array in promise mode
    */
    it('getStringArray_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getStringArray(0x1000002).then(value => {
                expect(value !== null).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_0900
    * @tc.name test getMedia method in callback mode
    * @tc.desc get the media in callback mode
    */
    it('getMedia_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getMedia(0x1000004, (err, value) => {
                expect(value.length > 0).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1000
    * @tc.name test getMedia method in promise mode
    * @tc.desc get the media in promise mode
    */
    it('getMedia_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getMedia(0x1000004).then(value => {
                expect(value.length > 0).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1100
    * @tc.name test getMediaBase64 method in callback mode
    * @tc.desc get the media base64 in callback mode
    */
    it('getMediaBase64_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getMediaBase64(0x1000004, (err, value) => {
                expect(value.length > 0).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1200
    * @tc.name test getMediaBase64 method in promise mode
    * @tc.desc get the media base64 in promise mode
    */
    it('getMediaBase64_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getMediaBase64(0x1000004).then(value => {
                expect(value.length > 0).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1300
    * @tc.name test getConfiguration method in callback mode
    * @tc.desc get the configuration in callback mode
    */
    it('getConfiguration_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getConfiguration((error, cfg) => {
                cfg.direction = Direction.DIRECTION_VERTICAL;
                cfg.locale = 'zh';
                expect(cfg !== null).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1400
    * @tc.name test getConfiguration method in promise mode
    * @tc.desc get the configuration in promise mode
    */
    it('getConfiguration_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getConfiguration().then(cfg => {
                expect(cfg !== null).assertTrue();
                cfg.direction = Direction.DIRECTION_HORIZONTAL;
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1500
    * @tc.name test getDeviceCapability method in callback mode
    * @tc.desc get the device capability in callback mode
    */
    it('getDeviceCapability_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getDeviceCapability((error, dc) => {
                dc.screenDensity = ScreenDensity.SCREEN_SDPI;
                dc.deviceType = DeviceType.DEVICE_TYPE_PHONE;
                dc.screenDensity = ScreenDensity.SCREEN_MDPI;
                dc.deviceType = DeviceType.DEVICE_TYPE_CAR;
                dc.screenDensity = ScreenDensity.SCREEN_LDPI;
                dc.deviceType = DeviceType.DEVICE_TYPE_TV;
                expect(dc !== null).assertTrue();
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1600
    * @tc.name test getDeviceCapability method in promise mode
    * @tc.desc get the device capability in promise mode
    */
    it('getDeviceCapability_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getDeviceCapability().then(dc => {
                dc.screenDensity = ScreenDensity.SCREEN_XLDPI;
                dc.deviceType = DeviceType.DEVICE_TYPE_TABLET;
                dc.screenDensity = ScreenDensity.SCREEN_XXLDPI;
                dc.deviceType = DeviceType.DEVICE_TYPE_WEARABLE;
                dc.screenDensity = ScreenDensity.SCREEN_XXXLDPI;
                dc.deviceType = DeviceType.DEVICE_TYPE_PC;
                expect(dc !== null).assertTrue();
                console.log('getDeviceCapability_test_002 ' + dc);
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1700
    * @tc.name test getPluralString method in callback mode
    * @tc.desc get the plural string in callback mode
    */
    it('getPluralString_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getPluralString(0x1000003, 1, (error, value) => {
                expect(value !== null).assertTrue();
                console.log('getPluralString_test_001 ' + value);
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1800
    * @tc.name test getPluralString method in promise mode
    * @tc.desc get the plural string in promise mode
    */
    it('getPluralString_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getPluralString(0x1000003, 1).then(value => {
                expect(value !== null).assertTrue();
                console.log('getPluralString_test_002 ' + value);
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1900
    * @tc.name test getString method in callback mode
    * @tc.desc get the string in callback mode
    */
    it('getString_test_003', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getString(0x7000000, (err, value) => {
                expect(value !== null).assertTrue();
                console.log('getString_test_003 ' + value);
                expect(value).assertEqual('hello world!');
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1700
    * @tc.name test getRawFile method in callback mode
    * @tc.desc get the getRawFile in callback mode
    */
    it('getRawFile_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getRawFile('rawfile/rawfiletest.xml', (error, value) => {
                expect(value !== null).assertTrue();
                console.log('getRawFile_test_001 ' + value);
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1800
    * @tc.name test getRawFile method in promise mode
    * @tc.desc get the getRawFile in promise mode
    */
    it('getRawFile_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getRawFile('rawfile/rawfiletest.xml').then(value => {
                expect(value !== null).assertTrue();
                console.log('getRawFile_test_002 ' + value);
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_1900
    * @tc.name test release method
    * @tc.desc get the release function
    */
    it('release_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getString(0x1000000, (err, value) => {
                expect(value !== null).assertTrue();
            })
            mgr.release();
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_2000
    * @tc.name test getRawFileDescriptor method in callback mode
    * @tc.desc get the RawFileDescriptor in callback mode
    */
    it('getRawFileDescriptor_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getRawFileDescriptor('rawfiletest.xml', (error, rawfile) => {
                let fdValue = rawfile.fd;
                let offsetValue = rawfile.offset;
                let lengthValue = rawfile.length;
                expect(rawfile !== null).assertTrue();
                console.log('getRawFileDescriptor_test_001--'
                            +'fd:' + fdValue
                            + 'offset:' + offsetValue
                            + 'length:' + lengthValue);
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_2100
    * @tc.name test getRawFileDescriptor method in promise mode
    * @tc.desc get the RawFileDescriptor in promise mode
    */
    it('getRawFileDescriptor_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.getRawFileDescriptor('rawfiletest.xml').then(rawfile => {
                rawfile.fd = 2000;
                rawfile.offset = 20;
                rawfile.length = 200;
                expect(rawfile !== null).assertTrue();
                console.log('getRawFileDescriptor_test_002--' + rawfile);
                console.log('getRawFileDescriptor_test_002--'
                +'fd:' + rawfile.fd
                + 'offset:' + rawfile.offset
                + 'length:' + rawfile.length);
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_2200
    * @tc.name test closeRawFileDescriptor method in callback mode
    * @tc.desc get the closeRawFileDescriptor in callback mode
    */
    it('closeRawFileDescriptor_test_001', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.closeRawFileDescriptor('rawfiletest.xml', (error, value) => {
                expect(error == null).assertTrue();
                console.log('closeRawFileDescriptor_test_001--' + error);
            })
        })
        done();
    })

    /* *
    * @tc.number SUB_GLOBAL_RESMGR_JS_2300
    * @tc.name test closeRawFileDescriptor method in promise mode
    * @tc.desc get the closeRawFileDescriptor in promise mode
    */
    it('closeRawFileDescriptor_test_002', 0, async function (done) {
        resmgr.getResourceManager((error, mgr) => {
            mgr.closeRawFileDescriptor('rawfiletest.xml').then(value => {
                expect(value !== null).assertTrue();
                console.log('closeRawFileDescriptor_test_002--' + value);
            })
        })
        done();
    })

    console.log('*************end ResmgrTest*************');
}) 