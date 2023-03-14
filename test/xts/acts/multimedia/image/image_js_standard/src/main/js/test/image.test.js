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

import image from '@ohos.multimedia.image'
import fileio from '@ohos.fileio'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import {testPng, testJpg, tc_020buf, tc_020_1buf, tc_021buf, tc_021_1buf, tc_022buf} from './testImg'

describe('Image', function () {
    var pathJpg = '/data/accounts/account_0/appdata/image/test.jpg';
    var pathBmp = '/data/accounts/account_0/appdata/image/test.bmp';
    var pathGif = '/data/accounts/account_0/appdata/image/test.gif';
    var pathPng = '/data/accounts/account_0/appdata/image/test.png';
    var pathMovingGif = '/data/accounts/account_0/appdata/image/moving_test.gif';
    var pathExifJpg = '/data/accounts/account_0/appdata/image/test_exif.jpg';
    var pathTiff = '/data/accounts/account_0/appdata/image/test.tiff';
    beforeAll(function () {
        console.info('beforeAll case');
    })

    beforeEach(function () {
        isTimeOut = false;
        console.info('beforeEach case');
    })

    afterEach(function () {
        console.info('afterEach case');
    })

    afterAll(function () {
        console.info('afterAll case');
    })

    /**
     * @tc.number    : TC_001
     * @tc.name      : create pixelmap-promise (editable: true, pixelFormat: RGBA_8888, size: { height: 4, width: 6 }, bytes = buffer)
     * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixeFormat,size
     *                 3.using color and opts create newPixelMap
     *                 4.return newpixelmap not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_001', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(Color, opts)
            .then( pixelmap => {
                expect(pixelmap != undefined).assertTrue();
                console.info('TC_001 success');
                done();
            })
            .catch(error => {
                console.log('TC_001 error: ' + error);
                expect().assertFail();
                done();
            })
        })
    
    /**
     * @tc.number    : TC_001-1
     * @tc.name      : create pixelmap-callback (editable: false, pixelFormat: RGBA_8888, size: { height: 4, width: 6 },bytes = buffer)
     * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixelFormat,size
     *                 3.using colorand opts create newPixelMap
     *                 4.return newpixelmap not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
    */
    it('TC_001-1', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: false, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(Color, opts, (err, pixelmap) => {
            expect(pixelmap != undefined).assertTrue();
            console.info('TC_001-1 success');
            done();
        })
    })
    /**
     * @tc.number    : TC_001-2
     * @tc.name      : createpixelmap-promise (editable: true, pixelFormat: RGB_565, size: { height: 6, width: 8 },bytes = buffer)
     * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixelFormat,size
     *                 3.using colorand opts create newPixelMap
     *                 4.return newpixelmap not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
     it('TC_001-2', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 2, size: { height: 6, width: 8 } }
        image.createPixelMap(Color, opts)
            .then( pixelmap => {
                expect(pixelmap != undefined).assertTrue();
                console.info('TC_001-2 success');
                done();
            })
            .catch(error => {
                console.log('TC_001-2 error: ' + error);
                expect().assertFail();
                done();
            })
        })

    /**
     * @tc.number    : TC_001-3
     * @tc.name      : createpixelmap-callback (editable: false, pixelFormat: RGB_565, size: { height: 6, width: 8 },bytes = buffer)
     * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixelFormat,size
     *                 3.using colorand opts create newPixelMap
     *                 4.return newpixelmap not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_001-3', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: false, pixelFormat: 2, size: { height: 6, width: 8 } }
        image.createPixelMap(Color, opts, (err, pixelmap) => {
            expect(pixelmap != undefined).assertTrue();
            console.info('TC_001-3 success');
            done();
           
        })
    })
    /**
     * @tc.number    : TC_001-4
     * @tc.name      : createpixelmap-promise(editable: true, pixelFormat: unkonwn, size: { height: 6, width: 8 })
     * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixelFormat,size
     *                 3.using colorand opts create newPixelMap
     *                 4.return newpixelmap not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
     it('TC_001-4', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 0, size: { height: 6, width: 8 } }
        image.createPixelMap(Color, opts)
            .then( pixelmap => {
                expect(pixelmap != undefined).assertTrue();
                console.info('TC_001-4 success');
                done();
            })
            .catch(error => {
                console.log('TC_001-4 error: ' + error);
                expect().assertFail();
                done();
            })
    })
    
    /**
     * @tc.number    : TC_001-5
     * @tc.name      : create pixelmap-callback(editable: false, pixelFormat: unkonwn, size: { height: 6, width: 8 })
     * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixelFormat,size
     *                 3.using colorand opts create newPixelMap
     *                 4.return newpixelmap not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
     it('TC_001-5', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: false, pixelFormat: 0, size: { height: 6, width: 8 } }
        image.createPixelMap(Color, opts, (err, pixelmap) => {
            expect(pixelmap != undefined).assertTrue();
            console.info('TC_001-5 success');
            done();
        })
    })
    /**
     * @tc.number    : TC_001-6
     * @tc.name      : create pixelmap-callback(editable: true, pixelFormat: RGBA_8888, size: { height: 6, width: 8 } bytes > buffer )
    * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixeFormat,size
     *                 3.using color and opts create newPixelMap
     *                 4.return newpixelmap not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_001-6', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 6, width: 8 } }
         image.createPixelMap(Color, opts, (err, pixelmap) => {
            expect(pixelmap != undefined).assertTrue();
            console.info('TC_001-6 success');
            done();
        })
    })

    /**
     * @tc.number    : TC_001-7
     * @tc.name      : create pixelmap-callback(editable: true, pixelFormat: RGB_565, size: { height: 2, width: 3 }, bytes < buffer)
     * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixeFormat,size
     *                 3.using color and opts create newPixelMap
     *                 4.return newpixelmap not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_001-7', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 2, size: { height: 2, width: 3 } }
         image.createPixelMap(Color, opts, (err, pixelmap) => {
            expect(pixelmap != undefined).assertTrue();
            console.info('TC_001-7 success');
            done();
        })
    })

    /**
     * @tc.number    : TC_001-8
     * @tc.name      : create pixelmap-callback(editable: true, pixelFormat: unkonwn, size: { height: -1, width: -1 })
     * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixeFormat,size
     *                 3.using color and opts create newPixelMap
     *                 4.return newpixelmap empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_001-8', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 0, size: { height: -1, width: -1 } }
        image.createPixelMap(Color, opts, (err, pixelmap) => {
            expect(pixelmap == undefined).assertTrue();
            console.info('TC_001-8 success');
            done();
        })
    })

    /**
     * @tc.number    : TC_001-9
     * @tc.name      : create pixelmap-callback(editable: true, pixelFormat: unsupported format, size: { height: 6, width: 8 }) 
     * @tc.desc      : 1.create InitializationOptions object
     *                 2.set editable,pixeFormat,size
     *                 3.using color and opts create newPixelMap
     *                 4.return newpixelmap empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_001-9', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 1, size: { height: 6, width: 8 } }
        image.createPixelMap(Color, opts, (err, pixelmap) => {
            expect(pixelmap == undefined).assertTrue();
            console.info('TC_001-9 success');
            done();
        })
    })

    /**
     * @tc.number    : TC_020
     * @tc.name      : readPixelsToBuffer-promise
     * @tc.desc      : read all pixels to an buffer
     *                 1.create PixelMap,buffer
     *                 2.call readPixelsToBuffer
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_020', 0, async function (done) {
        console.info('TC_020 in');
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }

        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts)
            .then( pixelmap => {
                if (pixelmap == undefined) {
                    console.info('TC_020 createPixelMap failed');
                    expect(false).assertTrue()
                    done();
                }
                const readBuffer = new ArrayBuffer(96);
                pixelmap.readPixelsToBuffer(readBuffer).then(() => {
                    var bufferArr2 = new Uint8Array(readBuffer);
                    var res = true;
                    for (var i = 0; i < bufferArr2.length; i++) {
                             if (bufferArr2[i] != tc_020buf[i]) {
                                res = false;
                                console.info('TC_20_buffer'+ bufferArr2[i]);
                                console.info('TC_020 failed');
                                expect(false).assertTrue();
                                done();
                                break;
                            }
                        }
                    if (res) {
                        console.info('TC_020 success');
                        expect(true).assertTrue()
                        done();
                    }
                }).catch(error => {
                    console.log('TC_020 read error: ' + error);
                    expect().assertFail();
                    done();
            })
        }).catch(error => {
            console.log('TC_020 error: ' + error);
            expect().assertFail();
            done();
        })    
    })
    
    /**
     * @tc.number    : TC_020-1
     * @tc.name      : readPixelsToBuffer-callback
     * @tc.desc      : read all pixels to an buffer
     *                 1.create PixelMap,buffer
     *                 2.call readPixelsToBuffer
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_020-1', 0, async function (done) {
        console.info('TC_020-1 in');
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }

        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts, (err, pixelmap) => {
            if(pixelmap == undefined){
                console.info('TC_020-1 createPixelMap failed');
                expect(false).assertTrue();
                done();
            }else{
                const readBuffer = new ArrayBuffer(96);
                pixelmap.readPixelsToBuffer(readBuffer,() => {
                    var bufferArr = new Uint8Array(readBuffer);
                    var res = true;
                    for (var i = 0; i < bufferArr.length; i++) {
                        if (bufferArr[i] != tc_020_1buf[i]) {
                            res = false;
                            console.info('TC_020-1 failed');
                            expect(false).assertTrue();
                            done();
                            break;
                        }
                    }
                    if (res) {
                        console.info('TC_020-1 success');
                        expect(true).assertTrue()
                        done();
                    }
                })
            }
        })
    })

    /**
     * @tc.number    : TC_020-2
     * @tc.name      : readPixelsToBuffer-callback(buffer:0)
     * @tc.desc      : read all pixels to an buffer
     *                 1.create PixelMap,buffer
     *                 2.call readPixelsToBuffer
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_020-2', 0, async function (done) {
        console.info('TC_020-2 in');
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }

        let opts = { editable: true, pixelFormat: 2, size: { height: 6, width: 8 } }
        image.createPixelMap(color, opts, (err,pixelmap) => { 
            if(pixelmap == undefined){
                console.info('TC_020-2 createPixelMap failed');
                expect(false).assertTrue();
                done();
            }else{
                const readBuffer = new ArrayBuffer(0);
                pixelmap.readPixelsToBuffer(readBuffer,() => {
                    var bufferArr = new Uint8Array(readBuffer);
                    var res = true;
                    for (var i = 0; i < bufferArr.length; i++) {
                        if (bufferArr[i] == 0) {
                            res = false;
                            console.info('TC_020-2 failed');
                            expect(false).assertTrue();
                            done();
                            break;
                        }
                    }
                    if (res) {
                        console.info('TC_020-2 success');
                        expect(true).assertTrue()
                        done();
                    }
                })
            }   
        })    
    })
    
    /**
     * @tc.number    : TC_021
     * @tc.name      : readPixels-promise
     * @tc.desc      : 1.create PixelMap
     *                 2.call readPixels
     *                 3.promise return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_021', 0, async function (done) {
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts)
            .then( pixelmap => {
                if (pixelmap == undefined) {
                    console.info('TC_021 createPixelMap failed');
                    expect(false).assertTrue()
                    done();
                }
                const area = { pixels: new ArrayBuffer(8),
                    offset: 0,
                    stride: 8,
                    region: { size: { height: 1, width: 2 }, x: 0, y: 0 }
                }
                pixelmap.readPixels(area).then(() => {
                    var bufferArr2 = new Uint8Array(area.pixels);
                    var res = true;
                    for (var i = 0; i < bufferArr2.length; i++) {
                    	if (bufferArr2[i] != tc_021buf[i]) {
                            res = false;
                            console.info('TC_021 failed');
                            expect(false).assertTrue();
                            done();
                            break;
                        }
                    }
                    if (res) {
                        console.info('TC_021 success');
                        expect(true).assertTrue()
                        done();
                    }
                })
            })
            .catch(error => {
                console.log('TC_021 error: ' + error);
                expect().assertFail();
                done();
            })
    })

    /**
     * @tc.number    : TC_021-1
     * @tc.name      : readPixels-callback
     * @tc.desc      : 1.create PixelMap
     *                 2.call readPixels
     *                 3.promise return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_021-1', 0, async function (done) {
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts, (err, pixelmap) => {
            if(pixelmap == undefined){
                console.info('TC_020-1 createPixelMap failed');
                expect(false).assertTrue();
                done();
            }else{
                const area = { pixels: new ArrayBuffer(8),
                    offset: 0,
                    stride: 8,
                    region: { size: { height: 1, width: 2 }, x: 0, y: 0 }}
                pixelmap.readPixels(area, () => {
                    var bufferArr = new Uint8Array(area.pixels);
                    var res = true;
                    for (var i = 0; i < bufferArr.length; i++) {
                        console.info('TC_021-1 buffer ' + bufferArr[i]);
                        if(bufferArr[i] != tc_021_1buf[i]) {
                            res = false;
                            console.info('TC_021-1 failed');
                            expect(false).assertTrue();
                            done();
                            break;
                        }    
                    }
                    if (res) {
                        console.info('TC_021-1 success');
                        expect(true).assertTrue()
                        done();
                    }
                })
            }
        })
    })

    /**
     * @tc.number    : TC_021-2
     * @tc.name      : readPixels-callback( region: { size: { height: 1, width: 2 }, x: -1, y: -1 })
     * @tc.desc      : 1.create PixelMap
     *                 2.call readPixels
     *                 3.promise return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_021-2', 0, async function (done) {
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts, (err,pixelmap) => {
            if(pixelmap == undefined){
                expect(false).assertTrue();
                console.info('TC_021-2 create pixelmap fail');
                done();
            }else{
                const area = { pixels: new ArrayBuffer(20),
                    offset: 0,
                    stride: 8,
                    region: { size: { height: 1, width: 2 }, x: -1, y: -1 }}
                pixelmap.readPixels(area).then(()=>{
                    console.info('TC_021-2 failed');
                    expect(false).assertTrue();
                    done();
                }).catch(()=>{
                    expect(true).assertTrue();
                    console.info('TC_021-2 success');
                    done(); 
                })
            }    
        })  
    })

    /**
     * @tc.number    : TC_021-3
     * @tc.name      : readPixels-promise(buffer:0)
     * @tc.desc      : 1.create PixelMap
     *                 2.call readPixels
     *                 3.promise return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_021-3', 0, async function (done) {
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts, (err,pixelmap) => {
            if(pixelmap == undefined){
                expect(false).assertTrue();
                console.info('TC_021-3 create pixelmap failed');
                done();    
            }else{
                const area = { pixels: new ArrayBuffer(0),
                    offset: 0,
                    stride: 8,
                    region: { size: { height: 1, width: 2 }, x: 0, y: 0 }}
                pixelmap.readPixels(area).then(()=>{
					console.info('TC_021-3 failed');
					expect(false).assertTrue();
					done();
				}).catch(()=>{
					expect(true).assertTrue();
					console.info('TC_021-3 success');
					done();
				})
			}
        })
    })

    /**
     * @tc.number    : TC_021-4
     * @tc.name      : readPixels-promise(offset > buffer)
     * @tc.desc      : 1.create PixelMap
     *                 2.call readPixels
     *                 3.promise return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_021-4', 0, async function (done) {
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts, (err, pixelmap) => {
            if(pixelmap == undefined){
                expect(false).assertTrue();
                console.info('TC_021-4 createPixelMap success');
                done(); 
            }
            const area = { pixels: new ArrayBuffer(20),
                offset: 21,
                stride: 8,
                region: { size: { height: 1, width: 2 }, x: 0, y: 0 }}
            pixelmap.readPixels(area).then(()=>{
                console.info('TC_021-4 failed');
                expect(false).assertTrue();
                done();
            }).catch(()=>{
                expect(true).assertTrue();
                console.info('TC_021-4 success');
                done(); 
            })
        })  
    })

    /**
     * @tc.number    : TC_021-5
     * @tc.name      : readPixels-promise(region: { size: { height: -1, width:-1}, x: 0, y: 0 })
     * @tc.desc      : 1.create PixelMap
     *                 2.call readPixels
     *                 3.promise return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_021-5', 0, async function (done) {
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts, (err,pixelmap) => {
            if(pixelmap == undefined){
                expect(false).assertTrue();
                console.info('TC_021-5 createPixelMap success');
                done(); 
            }
            const area = { pixels: new ArrayBuffer(20),
                offset: 0,
                stride: 8,
                region: { size: { height: -1, width:-1}, x: 0, y: 0 }}
            pixelmap.readPixels(area).then(()=>{
                console.info('TC_021-5 failed');
                expect(false).assertTrue();
                done();
            }).catch(()=>{
                expect(true).assertTrue();
                console.info('TC_021-5 success');
                done(); 
            })
        })
    })   
    
    /**
     * @tc.number    : TC_022
     * @tc.name      : writePixels-promise
     * @tc.desc      : 1.create PixelMap
     *                 2.call writePixels
     *                 3.call return undefined
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_022', 0, async function (done) {
        const color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts)
            .then( pixelmap => {
                if (pixelmap == undefined) {
                    console.info('TC_022 createPixelMap failed');
                    expect(false).assertTrue()
                    done();
                }

                const area = { pixels: new ArrayBuffer(8),
                    offset: 0,
                    stride: 8,
                    region: { size: { height: 1, width: 2 }, x: 0, y: 0 }
                }
                var bufferArr = new Uint8Array(area.pixels);
                for (var i = 0; i < bufferArr.length; i++) {
                    bufferArr[i] = i + 1;
                }

                pixelmap.writePixels(area).then(() => {
                    const readArea = { pixels: new ArrayBuffer(8),
                        offset: 0,
                        stride: 8,
                        region: { size: { height: 1, width: 2 }, x: 0, y: 0 }
                    }
                    pixelmap.readPixels(readArea).then(() => {
                        var readArr = new Uint8Array(readArea.pixels);
                        var res = true;
                        for (var i = 0; i < readArr.length; i++) {
                            if (readArr[i] != tc_022buf[i]) {
                                res = false;
                                console.info('TC_022 failed');
                                expect(false).assertTrue();
                                done();
                                break;
                            }    
                        }
                        if (res) {
                            console.info('TC_022 success');
                            expect(true).assertTrue()
                            done();
                        }
                    })
                })
            })
            .catch(error => {
                console.log('TC_022 error: ' + error);
                expect().assertFail();
                done();
            })
    })

    /**
     * @tc.number    : TC_022-1
     * @tc.name      : writePixels-callback
     * @tc.desc      : 1.create PixelMap
     *                 2.call writePixels
     *                 3.call return undefined
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
    */
    it('TC_022-1', 0, async function (done) {
        const color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts, (err, pixelmap) => {
            if (pixelmap == undefined) {
                console.info('TC_022-1 createPixelMap failed');
                expect(false).assertTrue()
                done();
            }
            const area = { pixels: new ArrayBuffer(8),
                offset: 0,
                stride: 8,
                region: { size: { height: 1, width: 2 }, x: 0, y: 0 }
            }
            var bufferArr = new Uint8Array(area.pixels);
            for (var i = 0; i < bufferArr.length; i++) {
                bufferArr[i] = i + 1;
            }
            pixelmap.writePixels(area, () => {
                const readArea = { pixels: new ArrayBuffer(8),
                    offset: 0,
                    stride: 8,
                    region: { size: { height: 1, width: 2 }, x: 0, y: 0 }
                }
                pixelmap.readPixels(readArea,() => {
                    var readArr = new Uint8Array(readArea.pixels);
                    var res = true;
                    for (var i = 0; i < readArr.length; i++) {
                        if (readArr[i] == 0) {
                            res = false;
                            console.info('TC_022-1 failed');
                            expect(false).assertTrue();
                            done();
                            break;
                        }
                    }
                    if (res) {
                        console.info('TC_022-1 success');
                        expect(true).assertTrue()
                        done();
                    }
                })
            })
        })
        .catch(error => {
            console.log('TC_022-1 error: ' + error);
            expect().assertFail();
            done();
        })
    })

    /**
     * @tc.number    : TC_023
     * @tc.name      : writeBufferToPixels-promise
     * @tc.desc      : 1.create PixelMap,buffer
     *                 2.call writeBufferToPixels
     *                 3.call return undefined
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_023', 0, async function (done) {
        const color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 }}
        image.createPixelMap(color, opts)
            .then( pixelmap => {
                if (pixelmap == undefined) {
                    console.info('TC_023 createPixelMap failed');
                    expect(false).assertTrue()
                    done();
                }

                const writeColor = new ArrayBuffer(96);
 		var bufferArr = new Uint8Array(writeColor);
                for (var i = 0; i < bufferArr.length; i++) {
                    bufferArr[i] = i + 1;
                }
                pixelmap.writeBufferToPixels(writeColor).then(() => {
                    const readBuffer = new ArrayBuffer(96);
                    pixelmap.readPixelsToBuffer(readBuffer).then(() => {
                        var bufferArr = new Uint8Array(readBuffer);
                        var res = true;
                        for (var i = 0; i < bufferArr.length; i++) {
                            if (bufferArr[i] == 0) {
                                res = false;
                                console.info('TC_023 failed');
                                expect(false).assertTrue()
                                done();
                                break;
                            }
                        }
                        if (res) {
                            console.info('TC_023 success');
                            expect(true).assertTrue();
                            done();
                        }
                    })
                })
            })
            .catch(error => {
                console.log('TC_023 error: ' + error);
                expect().assertFail();
                done();
            })
    })

    /**
     * @tc.number    : TC_023-1
     * @tc.name      : writeBufferToPixels-callback
     * @tc.desc      : 1.create PixelMap,buffer
     *                 2.call writeBufferToPixels
     *                 3.call return undefined
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_023-1', 0, async function (done) {
        const color = new ArrayBuffer(96);
        var bufferArr = new Uint8Array(color);
        for (var i = 0; i < bufferArr.length; i++) {
            bufferArr[i] = i + 1;
        }
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts).then( pixelmap => {
            if(pixelmap == undefined){
                expect(false).assertTrue()
                console.info('TC_023-1 failed');
                done();
            }
            const writeColor = new ArrayBuffer(96);
            pixelmap.writeBufferToPixels(writeColor,() => {
                const readBuffer = new ArrayBuffer(96);
                pixelmap.readPixelsToBuffer(readBuffer,() => {
                    var bufferArr = new Uint8Array(readBuffer);
                    var res = true;
                    for (var i = 0; i < bufferArr.length; i++) {
                        if(res) {
                            if (bufferArr[i] == 0) {
                                res = false;
                                console.info('TC_023-1 Success');
                                expect(true).assertTrue()
                                done();
                                break;
                            }
                        }
                    }
                    if (res) {
                        console.info('TC_023-1 no change after writeBuffer');
                        expect(false).assertTrue();
                        done();
                    }
                })            
            })
        })
    })

    /**
     * @tc.number    : TC_024
     * @tc.name      : getImageInfo-pixelmap-promise
     * @tc.desc      : 1.create PixelMap,ImageInfo
     *                 2.call getImageInfo
     *                 3.call return imageinfo
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_024', 0, async function (done) {
        const color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 2, size: { height: 6, width: 8 } }
        image.createPixelMap(color, opts)
        .then( pixelmap => {
            if (pixelmap == undefined) {
                console.info('TC_024 createPixelMap failed');
                expect(false).assertTrue()
                done();
            }
            pixelmap.getImageInfo().then( imageInfo => {
                if (imageInfo == undefined) {
                    console.info('TC_024 imageInfo is empty');
                    expect(false).assertTrue()
                    done();
                } 
                if(imageInfo.size.height == 4 && imageInfo.size.width == 6){
                    console.info('TC_024 success ');
                    expect(true).assertTrue()
                    done();
                }                     
                done();
            }).catch(error => {
                console.log('TC_024 getimageinfo error: ' + error);
                expect().assertFail();
                done();
            })
            done();
        })
        .catch(error => {
            console.log('TC_024 error: ' + error);
            expect().assertFail();
            done();
        })
    })

    /**
     * @tc.number    : TC_024-1
     * @tc.name      : getImageInfo-pixelmap-callback
     * @tc.desc      : 1.create PixelMap,ImageInfo
     *                 2.call getImageInfo
     *                 3.call return imageinfo
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_024-1', 0, async function (done) {
        const color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts, (err,pixelmap) => {
            if(pixelmap == undefined){
                expect(false).assertTrue()
                console.info('TC_024-1 create pixelmap fail');
                done();
            }
            pixelmap.getImageInfo( (err,imageInfo) => {
                if (imageInfo == undefined) {
                    console.info('TC_024-1 imageInfo is empty');
                    expect(false).assertTrue()
                    done();
                } 
                if(imageInfo.size.height == 4 && imageInfo.size.width == 6){
                    console.info('TC_024-1 imageInfo success');
                    expect(true).assertTrue()
                    done();
                } 
                done();
            })
        })
    })

    /**
     * @tc.number    : TC_025-1
     * @tc.name      : getBytesNumberPerRow
     * @tc.desc      : 1.create PixelMap
     *                 2.set PixelMap
     *                 3.call getBytesNumberPerRow
     *                 4. call return number
     *                 5.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_025-1', 0, async function (done) {
        const color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        const expectNum = 4 * opts.size.width;
        image.createPixelMap(color, opts, (err,pixelmap) => {
            if(pixelmap == undefined){
                expect(false).assertTrue()
                console.info('TC_25-1 create pixelmap fail');
                done();
            } else {
                const num = pixelmap.getBytesNumberPerRow();
                console.info('TC_025-1 num is ' + num);
                expect(num == expectNum).assertTrue();
                if(num == expectNum) {
                    console.info('TC_25-1 success');
                } else {
                    console.info('TC_25-1 fail');
                }
                done();
            }
        })
    })

    /**
     * @tc.number    : TC_026-1
     * @tc.name      : getPixelBytesNumber
     * @tc.desc      : 1.create PixelMap
     *                 2.set Pixel
     *                 3.call getPixelBytesNumber
     *                 4. call return number
     *                 5.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_026-1', 0, async function (done) {
        const color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        const expectNum = 4 * opts.size.width * opts.size.height;
        image.createPixelMap(color, opts,(err,pixelmap) => {
            if(pixelmap == undefined){
                expect(false).assertTrue()
                console.info('TC_026-1 create pixelmap fail');
                done();
            } else {
                const num = pixelmap.getPixelBytesNumber();
                console.info('TC_026-1 num is ' + num);
                expect(num == expectNum).assertTrue();
                if(num == expectNum) {
                    console.info('TC_026-1 success');
                } else {
                    console.info('TC_026-1 fail');
                }
                done();
            }
        })
    })

    /**
     * @tc.number    : TC_027
     * @tc.name      : release-pixelmap-promise
     * @tc.desc      : 1.create PixelMap
     *                 2.set Pixel
     *                 3.call release
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_027', 0, async function (done) {
        const color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts).then(pixelmap => {
            if (pixelmap == undefined) {
                console.info('TC_027 createPixelMap failed');
                expect(false).assertTrue()
                done();
            }
            pixelmap.release().then(() => {
                console.info('TC_027 success');
                expect(true).assertTrue();
                done();
            }).catch(error => {
                console.log('TC_027 error: ' + error);
                expect().assertFail();
                done();
            })
        }).catch(error => {
            console.log('TC_027 createPixelMap failed error: ' + error);
            expect().assertFail();
            done();
        })
    })

    /**
     * @tc.number    : TC_027-1 
     * @tc.name      : release-pixelmap-callback
     * @tc.desc      : 1.create PixelMap
     *                 2.set Pixel
     *                 3.call release
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_027-1', 0, async function (done) {
        const color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(color, opts, (err, pixelmap) => {
            if (pixelmap == undefined) {
                console.info('TC_027-1 createPixelMap failed');
                expect(false).assertTrue()
                done();
            }
            pixelmap.release(()=>{
                expect(true).assertTrue();
                console.log('TC_027-1 success');
                done();
            })    
        })   
    })

    /**
     * @tc.number    : TC_041
     * @tc.name      : createImageSource(uri)-jpg
     * @tc.desc      : 1.set uri
     *                 2.call createImageSource(uri)
     *                 3.return imagesource
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_041', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        expect(imageSourceApi != undefined).assertTrue();
        console.info('TC_041 success');
        done();
    })

    /**
     * @tc.number    : TC_041-1
     * @tc.name      : createImageSource(uri)-bmp
     * @tc.desc      : 1.seturi
     *                 2.call createImageSource(uri)
     *                 3.return imagesource
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_041-1', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        expect(imageSourceApi != undefined).assertTrue();
        console.info('TC_041-1 success');
        done();
    })

    /**
     * @tc.number    : TC_041-2
     * @tc.name      : createImageSource(uri)-gif
     * @tc.desc      : 1.seturi
     *                 2.call createImageSource(uri)
     *                 3.return imagesource
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_041-2', 0, async function (done) {
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        expect(imageSourceApi != undefined).assertTrue();
        console.info('TC_041-2 success');
        done();
    })

    /**
     * @tc.number    : TC_041-3
     * @tc.name      : createImageSource(uri)-png
     * @tc.desc      : 1.seturi
     *                 2.call createImageSource(uri)
     *                 3.return imagesource
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_041-3', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        expect(imageSourceApi != undefined).assertTrue();
        console.info('TC_041-3 success');
        done();
    })

    /**
     * @tc.number    : TC_041-4
     * @tc.name      : createImageSource(uri)-wrong suffix file 
     * @tc.desc      : 1.call createImageSource(uri)
     *                 2.Incoming wrong suffix file 
     *                 3.imagesource null
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_041-4', 0, async function (done) {
        const imageSourceApi = image.createImageSource('file:///data/local/tmp/test.123');
        expect(imageSourceApi == undefined).assertTrue();
        console.info('TC_041-4 success');
        done();
    })

    /**
     * @tc.number    : TC_041-5
     * @tc.name      : createImageSource(uri)-wrong uri
     * @tc.desc      : 1.call createImageSource(uri)
     *                 2.set wrong uri
     *                 3.return null
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_041-5', 0, async function (done) {
        const imageSourceApi = image.createImageSource('file:///multimedia/test.jpg');
        expect(imageSourceApi == undefined).assertTrue();
        console.info('TC_041-5 success');
        done();
    })

    /**
     * @tc.number    : TC_042
     * @tc.name      : createImageSource(fd)
     * @tc.desc      : 1.call createImageSource
     *                 2.set fd
     *                 3.return imagesource
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_042', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_042 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo((err,imageInfo) => {
                console.info('TC_042 success');
                expect(imageInfo != undefined).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_042-1
     * @tc.name      : createImageSource(fd) fd<0
     * @tc.desc      : 1.call createImageSource
     *                 2.set wrong fd
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_042-1', 0, async function (done) {
        const imageSourceApi = image.createImageSource(-2);
        expect(imageSourceApi == undefined).assertTrue();
        console.info('TC_042-1 success');
        done();
    })

    /**
     * @tc.number    : TC_043
     * @tc.name      : createImageSource(data)
     * @tc.desc      : 1.setdata
     *                 2.createImageSource
     *                 3.return imagesource
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_043', 0, async function (done) {
	    console.info('TC_043 start');
        const data = testJpg.buffer;
        const imageSourceApi = image.createImageSource(data);
        if (imageSourceApi == undefined) {
            console.info('TC_043 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo((err,imageInfo) => {
                console.info('TC_043 imageInfo');
                expect(imageInfo != undefined).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_043-1
     * @tc.name      : createImageSource(data) buffer:0
     * @tc.desc      : 1.setdata
     *                 2.createImageSource
     *                 3.return imagesource
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */
    it('TC_043-1', 0, async function (done) {
	    console.info('TC_043-1 start');
        const data = new ArrayBuffer(0);
        const imageSourceApi = image.createImageSource(data);
        expect(imageSourceApi == undefined).assertTrue();
        console.info('TC_043-1 success');
        done();
    })

    /**
     * @tc.number    : TC_044
     * @tc.name      : release-imagesource-promise-jpg
     * @tc.desc      : 1.create ImageSource
     *                 2.call release()
     *                 3.return undefined
     * @tc.size      : MEDIUM  
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_044', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_044 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.release().then(()=>{
                console.info('TC_044 success');
                expect(true).assertTrue();
                done();
            }).catch(error => {
                console.info('TC_044 error');
                expect(false).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_044-1
     * @tc.name      : release-imagesource-callback-jpg
     * @tc.desc      : 1.create ImageSource
     *                 2.call release()
     *                 3.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_044-1', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_044-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.release(() => {
                console.info('TC_044-1 Success');
                expect(true).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_045
     * @tc.name      : getImageInfo(callback: AsyncCallback<ImageInfo>)-jpg
     * @tc.desc      : 1.create imageSource
     *                 2.imageSourcecall getImageInfo(ImageInfo)
     *                 3.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_045', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_045 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo((err,imageInfo) => {
                console.info('TC_045 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                expect(imageInfo != undefined).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_045-1
     * @tc.name      : getImageInfo(callback: AsyncCallback<ImageInfo>)-bmp
     * @tc.desc      : 1.create imageSource
     *                 2.imageSourcecall getImageInfo(ImageInfo)
     *                 3.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_045-1', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_045-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo((err,imageInfo) => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_045-1 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_045-2
     * @tc.name      : getImageInfo(callback: AsyncCallback<ImageInfo>)-png
     * @tc.desc      : 1.create imageSource
     *                 2.imageSourcecall getImageInfo(ImageInfo)
     *                 3.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_045-2', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_045-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo((err,imageInfo) => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_045-2 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_045-3
     * @tc.name      : getImageInfo(callback: AsyncCallback<ImageInfo>)-gif
     * @tc.desc      : 1.create ImageInfo
     *                 2.call getImageInfo(index, ImageInfo)
     *                 3.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_045-3', 0, async function (done) {
        console.info('TC_045-3');
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        if (imageSourceApi == undefined) {
            console.info('TC_045-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo((err,imageInfo) => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_045-3 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_046
     * @tc.name      : getImageInfo(index: number, callback: AsyncCallback<ImageInfo>)-jpg
     * @tc.desc      : 1.create ImageInfo
     *                 2.call getImageInfo(index, ImageInfo)
     *                 3.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_046', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_046 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(0, (err, imageInfo) => {
                console.info('TC_046 imageInfo');
                expect(imageInfo != undefined).assertTrue();
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_046-1
     * @tc.name      : getImageInfo(index: number, callback: AsyncCallback<ImageInfo>)-bmp
     * @tc.desc      : 1.create ImageInfo
     *                 2.call getImageInfo(index, ImageInfo)
     *                 3.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_046-1', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_046-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(0, (err, imageInfo) => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_046-1 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            })
        }
    })

    /**
     * @tc.number: TC_046-2
     * @tc.name  : getImageInfo(index: number, callback: AsyncCallback<ImageInfo>)-png
     * @tc.desc  : 1.create ImageInfo
     *             2.call getImageInfo(index, ImageInfo)
     *             3.callback return undefined
     * @tc.size  : MEDIUM MEDIUM
     * @tc.type  : Functional
     * @tc.level : Level 1
     */    
    it('TC_046-2', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_046-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(0, (err, imageInfo) => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_046-2 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            })
        }
    })

    /**
     * @tc.number: TC_046-3
     * @tc.name  : getImageInfo(index: number, callback: AsyncCallback<ImageInfo>)-gif
     * @tc.desc  : 1.create ImageInfo
     *             2.call getImageInfo(index, ImageInfo)
     *             3.callback return undefined
     * @tc.size  : MEDIUM MEDIUM
     * @tc.type  : Functional
     * @tc.level : Level 1
     */    
    it('TC_046-3', 0, async function (done) {
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        if (imageSourceApi == undefined) {
            console.info('TC_046-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(0,(err, imageInfo) => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_046-3 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            })
        }
    })

    /**
     * @tc.number: TC_046-4
     * @tc.name  : getImageInfo(index: number, callback: AsyncCallback<ImageInfo>)-gif(frame:1)-index:1
     * @tc.desc  : 1.create ImageInfo
     *             2.call getImageInfo(index, ImageInfo)
     *             3.callback return undefined
     * @tc.size  : MEDIUM
     * @tc.type  : Functional
     * @tc.level : Level 1
     */    
    it('TC_046-4', 0, async function (done) {
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        if (imageSourceApi == undefined) {
            console.info('TC_046-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(1, (err, imageInfo) => {
                if(imageInfo == undefined) {
                    expect(true).assertTrue();
                    done();
                } else {
                    expect(false).assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number: TC_046-5
     * @tc.name  : getImageInfo(index: number, callback: AsyncCallback<ImageInfo>)-gif-index:-1
     * @tc.desc  : 1.create ImageInfo
     *             2.call getImageInfo(index, ImageInfo)
     *             3.callback return undefined
     * @tc.size  : MEDIUM 
     * @tc.type  : Functional
     * @tc.level : Level 1
     */    
    it('TC_046-5', 0, async function (done) {
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        if (imageSourceApi == undefined) {
            console.info('TC_046-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(-1, (err, imageInfo) => {
                expect(imageInfo == undefined).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_047
     * @tc.name      : getImageInfo(index?: number): Promise<ImageInfo>-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageInfo(index)
     *                 3.callbackcall ,return imageinfo
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_047', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_047 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(0)
            .then(imageInfo => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_047 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            }).catch(error => {
                console.log('TC_047 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_047-1
     * @tc.name      : getImageInfo(index?: number): Promise<ImageInfo>-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageInfo(index)
     *                 3.callbackcall ,return imageinfo
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_047-1', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_047-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(0)
            .then(imageInfo => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_047-1 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            }).catch(error => {
                console.log('TC_047-1 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_047-2
     * @tc.name      : getImageInfo(index?: number): Promise<ImageInfo>-png
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageInfo(index)
     *                 3.callbackcall ,return imageinfo
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_047-2', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_047-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(0)
            .then(imageInfo => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_047-2 imageInfo');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            }).catch(error => {
                console.log('TC_047-2 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_047-3
     * @tc.name      : getImageInfo(index?: number): Promise<ImageInfo>-gif
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageInfo(index)
     *                 3.callbackcall ,return imageinfo
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_047-3', 0, async function (done) {
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        if (imageSourceApi == undefined) {
            console.info('TC_047-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(0)
            .then(imageInfo => {
                expect(imageInfo != undefined).assertTrue();
                console.info('TC_047-3  ');
                console.info('imageInfo.size.height:'+imageInfo.size.height);
                console.info('imageInfo.size.width:'+imageInfo.size.width);
                done();
            }).catch(error => {
                console.log('TC_047-3 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number: TC_047-4
     * @tc.name  : getImageInfo(index?: number): Promise<ImageInfo>-gif(frame:1)-index:1
     * @tc.desc  : 1.create imagesource
     *             2.call getImageInfo(index=1)
     *             3.callback return imageinfo undefined
     * @tc.size  : MEDIUM 
     * @tc.type  : Functional
     * @tc.level : Level 1
     */    
    it('TC_047-4', 0, async function (done) {
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        if (imageSourceApi == undefined) {
            console.info('TC_047-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(1)
            .then(() => {
                console.log('TC_047-4 failed');
                expect().assertFail();
                done();
            }).catch(error => {
                console.log('TC_047-4 success');
                expect(true).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_047-5
     * @tc.name      : getImageInfo(index?: number): Promise<ImageInfo>-gif-index:-1
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageInfo(index=-1)
     *                 3.callback return imageinfo undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */   
    it('TC_047-5', 0, async function (done) {
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        if (imageSourceApi == undefined) {
            console.info('TC_047-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageInfo(-1)
            .then(() => {
                console.log('TC_047-5 failed');
                expect().assertFail();
                done();
            }).catch(error => {
                console.log('TC_047-5 success');
                expect(true).assertTrue();
                done();
            })
        }
    })
        
    /**
     * @tc.number    : TC_050
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:RGBA_8888-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:3,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions, (err, pixelmap) => {
                console.info('TC_050 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }        
    })
    /**
     * @tc.number    : TC_050-1
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:RGB_565-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_050-1', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions, (err, pixelmap) => {
                console.info('TC_050-1 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }        
    })
    /**
     * @tc.number    : TC_050-2
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:unknown-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return null
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_050-2', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions, (err, pixelmap) => {
                console.info('TC_050-2 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }        
    })
    /**
     * @tc.number    : TC_050-3
     * @tc.name      : createPixelMap(decodingOptions: index 1})-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050-3', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:1
            };
            imageSourceApi.createPixelMap(decodingOptions, (err, pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_050-3 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_050-4
     * @tc.name      : createPixelMap(decodingOptions:index -1})-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050-4', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:-1
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.error('TC_050-4 success');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }    
            })
        }
    })

    /**
     * @tc.number    : TC_050-5
     * @tc.name      : createPixelMap(decodingOptions:sampleSize -1})-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050-5', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:-1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_050-5 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_050-6
     * @tc.name      : createPixelMap(decodingOptions:rotate -10})-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050-6', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-6 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:-10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_050-6 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_050-7
     * @tc.name      : createPixelMap(decodingOptions:unsupported pixelformat)-jpg 
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050-7', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-7 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:60,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_050-7 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_050-8
     * @tc.name      : createPixelMap(decodingOptions:editable false})-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1             
     */
    it('TC_050-8', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-8 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: false, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_050-8 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_050-9
     * @tc.name      : createPixelMap(decodingOptions:desiredSize>imagesize)-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050-9', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-9 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:10000, height:10000},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_050-9 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
	}
    })

    /**
     * @tc.number    : TC_050-10
     * @tc.name      : createPixelMap(decodingOptions:desiredRegion>imagesize)-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050-10', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-10 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 10000, width: 10000 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_050-10 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })
        
    /**
     * @tc.number    : TC_050-11
     * @tc.name      : createPixelMapdecodingOptions:x -1 y -1)-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_050-11', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-11 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: -1, y: -1 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_050-11 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

        /**
     * @tc.number    : TC_050-12
     * @tc.name      : createPixelMap(decodingOptions:x > image.height y > image.width)-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
         it('TC_050-12', 0, async function (done) {
            let fdJpg = fileio.openSync(pathJpg);
            const imageSourceApi = image.createImageSource(fdJpg);
            if (imageSourceApi == undefined) {
                console.info('TC_050-12 create image source failed');
                expect(false).assertTrue();
                done();
            } else {
                let decodingOptions = {
                    sampleSize:1,
                    editable: true, 
                    desiredSize:{ width:1, height:2},
                    rotate:10,
                    desiredPixelFormat:2,
                    desiredRegion: { size: { height: 1, width: 2 }, x: 10000, y: 10000 },
                    index:0
                };
                imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                    if(pixelmap == undefined){
                        console.info('TC_050-12 success ');
                        expect(true).assertTrue();
                        done();
                    }else{
                        expect(false).assertTrue();
                        done();
                    }   
                }) 
            }        
        })
    
        /**
         * @tc.number    : TC_050-13
         * @tc.name      : createPixelMap(decodingOptions:rotate>360)-jpg
         * @tc.desc      : 1.create imagesource
         *                 2.set index and DecodeOptions
         *                 3.create PixelMap
         *                 4.callback return undefined
         * @tc.size      : MEDIUM 
         * @tc.type      : Functional
         * @tc.level     : Level 1
         */
        it('TC_050-13', 0, async function (done) {
            let fdJpg = fileio.openSync(pathJpg);
            const imageSourceApi = image.createImageSource(fdJpg);
            if (imageSourceApi == undefined) {
                console.info('TC_050-13 create image source failed');
                expect(false).assertTrue();
                done();
            } else {
                let decodingOptions = {
                    sampleSize:1,
                    editable: true, 
                    desiredSize:{ width:1, height:2},
                    rotate:500,
                    desiredPixelFormat:2,
                    desiredRegion: { size: { height: 1, width: 2 }, x: 1, y: 2 },
                    index:0
                };
                imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                    if(pixelmap == undefined){
                        console.info('TC_050-13 success ');
                        expect(true).assertTrue();
                        done();
                    }else{
                        expect(false).assertTrue();
                        done();
                    }   
                }) 
            }        
        })

    /**
     * @tc.number    : TC_050-14
     * @tc.name      : createPixelMap-promise-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050-14', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-14 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.createPixelMap().then(pixelmap => {
                console.info('TC_050-14 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            }).catch(error => {
                console.log('TC_050-14 error: ' + error);
                expect().assertFail();
                done();
            })
        }        
    })

    /**
     * @tc.number    : TC_050-15
     * @tc.name      : createPixelMap-callback-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return null
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_050-15', 0, async function (done) { 
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_050-15 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.createPixelMap((err, pixelmap) => {
                console.info('TC_050-15 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }        
    })

    /**
     * @tc.number    : TC_053
     * @tc.name      : createIncrementalSource-updateData-png
     * @tc.desc      : 1.create imagesource
     *                 2.update data
     *                 3.create pixelmap
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_053', 0, async function (done) {
        try {
            let testimagebuffer = testPng;
            console.info('TC_053 0003 ' + testimagebuffer.length);
            let bufferSize = testimagebuffer.length;
            let offset = 0;
            const incSouce = image.createIncrementalSource(new ArrayBuffer(1));
            let ret;
            let isFinished = false;
            while (offset < testimagebuffer.length) {
                console.info('TC_053 0006 ' + testimagebuffer.length);
                var oneStep = testimagebuffer.slice(offset, offset + bufferSize);
                console.info('TC_053 0007 ' + oneStep.length);
                if (oneStep.length < bufferSize) {
                    isFinished = true;
                }
                ret = await incSouce.updateData(oneStep, isFinished, 0, oneStep.length);
                if (!ret) {
                    console.info('TC_053 updateData failed');
                    expect(ret).assertTrue();
                    break;
                }
                offset = offset + oneStep.length;
                console.info('TC_053 0011 ' + offset);
            }
            if (ret) {
                console.info('TC_053 updateData success ');
                let decodingOptions = {
                    sampleSize:1
                };
                incSouce.createPixelMap(decodingOptions, (err, pixelmap) => {
                    console.info('TC_053 0014' + pixelmap);
                    expect(pixelmap != undefined ).assertTrue();
                    done();
                })
            } else {
                done();
            }
        } catch (error) {
            console.info('TC_053 updateData failed ' + error);
        }    
    })

    /**
     * @tc.number    : TC_053-1
     * @tc.name      : createIncrementalSource-updateData-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.update data
     *                 3.create pixelmap
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_053-1', 0, async function (done) {
        try {
            let testimagebuffer = testJpg;
            console.info('TC_053-1 0003 ' + testimagebuffer.length);
            let bufferSize = testimagebuffer.length;
            let offset = 0;
            const incSouce = image.createIncrementalSource(new ArrayBuffer(1));
            let isFinished = false;
            let ret;
            while (offset < testimagebuffer.length) {
                console.info('TC_053-1 0006 ' + testimagebuffer.length);
                var oneStep = testimagebuffer.slice(offset, offset + bufferSize);
                console.info('TC_053-1 0007 ' + oneStep.length);
                if (oneStep.length < bufferSize) {
                    isFinished = true;
                }
                ret = await incSouce.updateData(oneStep, isFinished, 0, oneStep.length);
                if (!ret) {
                    console.info('TC_053-1 updateData failed');
                    expect(ret).assertTrue();
                    break;
                }
                offset = offset + oneStep.length;
                console.info('TC_053-1 0011 ' + offset);
            }
            if (ret) {
                console.info('TC_053-1 updateData success ');
                let decodingOptions = {
                    sampleSize:1
                };
                incSouce.createPixelMap(decodingOptions, (err, pixelmap) => {
                    expect(pixelmap != undefined ).assertTrue();
                    done();
                })
            } else {
                done();
            }
        } catch (error) {
            console.info('TC_053-1 updateData failed ' + error);
        }   
    })

    /**
     * @tc.number    : TC_062
     * @tc.name      : packing ImageSource - promise
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_062', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                let packOpts = { format:["image/jpeg"], quality:99 }
                imagePackerApi.packing(imageSourceApi, packOpts)
                .then( data => {
                    console.info('TC_062 success');
                    expect(data != undefined).assertTrue();
                    done();
                }).catch(error => {
                    console.log('TC_062 error: ' + error);
                    expect(false).assertFail();
                    done();
                })
            }
        }
    })

    /**
     * @tc.number    : TC_062-1 
     * @tc.name      : packing ImageSource - callback
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_062-1', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062-1 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                let packOpts = { format:["image/jpeg"], quality:1 }
                imagePackerApi.packing(imageSourceApi, packOpts, (err, data) => {
                    console.info('TC_062-1 success');
                    expect(data != undefined).assertTrue();
                    done();
                })
            }
        }
    })

    /**
     * @tc.number    : TC_062-2
     * @tc.name      : packing ImageSource - callback - wrong format
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_062-2', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062-2 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                let packOpts = { format:["image/gif"], quality:98 }
                imagePackerApi.packing(imageSourceApi, packOpts, (err, data) => {
                    console.info('TC_062-2 success');
                    expect(data == undefined).assertTrue();
                    console.info(data);
                    done();
                })
            }
        }
    })

    /**
     * @tc.number    : TC_062-3
     * @tc.name      : packing ImageSource - callback - wrong quality
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.call return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_062-3', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062-3 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                let packOpts = { format:["image/jpg"], quality:101 }
                imagePackerApi.packing(imageSourceApi, packOpts, (err, data) => {
                    console.info('TC_062-3 success');
                    expect(data == undefined).assertTrue();
                    console.info(data);
                    done();
                })
            }
        }
    })

    /**
     * @tc.number    : TC_062-4 
     * @tc.name      : createImagePacker
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_062-4', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062-4 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                console.info('TC_062-4 create image packer success');
                expect(true).assertTrue();
                done();
            }
        }
    })
	
	/**
     * @tc.number    : TC_062-5
     * @tc.name      : packing ImageSource - promise - no quality
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.call return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_062-5', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062-5 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                let packOpts = { format:["image/jpg"] }
                imagePackerApi.packing(imageSourceApi, packOpts)
                .then( data => {
                    console.info('TC_062-5 failed');
                    expect(data == undefined).assertTrue();
                    done();
                }).catch(error => {
                    console.log('TC_062-5 error: ' + error);
					console.log('TC_062-5 success');
                    expect(true).assertTrue();
                    done();
                })
            }
        }
    })
	
	/**
     * @tc.number    : TC_062-6
     * @tc.name      : packing ImageSource - promise - no format
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.call return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_062-6', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062-6 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062-6 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                let packOpts = { quality:50 }
                imagePackerApi.packing(imageSourceApi, packOpts)
                .then( data => {
                    console.info('TC_062-6 failed');
                    expect(data == undefined).assertTrue();
                    done();
                }).catch(error => {
                    console.log('TC_062-6 error: ' + error);
					console.log('TC_062-6 success');
                    expect(true).assertTrue();
                    done();
                })
            }
        }
    })
	
	/**
     * @tc.number    : TC_062-7 
     * @tc.name      : packing ImageSource - callback - quality 100
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_062-7', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062-7 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062-7 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                let packOpts = { format:["image/jpeg"], quality:100 }
                imagePackerApi.packing(imageSourceApi, packOpts, (err, data) => {
                    console.info('TC_062-7 success');
                    expect(data != undefined).assertTrue();
                    done();
                })
            }
        }
    })
	
	/**
     * @tc.number    : TC_062-8 
     * @tc.name      : packing ImageSource - callback - quality 0
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_062-8', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062-8 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062-8 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                let packOpts = { format:["image/jpeg"], quality:0 }
                imagePackerApi.packing(imageSourceApi, packOpts, (err, data) => {
                    console.info('TC_062-8 success');
                    expect(data != undefined).assertTrue();
                    done();
                })
            }
        }
    })
	
	/**
     * @tc.number    : TC_062-9 
     * @tc.name      : packing ImageSource - callback - quality -1
     * @tc.desc      : 1.create ImageSource
     *                 2.call packing
     *                 3.return array
     *                 4.callbackcall return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_062-9', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_062-9 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            const imagePackerApi = image.createImagePacker();
            if (imagePackerApi == undefined) {
                console.info('TC_062-9 create image packer failed');
                expect(false).assertTrue();
                done();
            } else {
                let packOpts = { format:["image/jpeg"], quality:-1 }
                imagePackerApi.packing(imageSourceApi, packOpts, (err, data) => {
                    console.info('TC_062-9 success');
                    expect(data == undefined).assertTrue();
                    done();
                })
            }
        }
    })

    /**
     * @tc.number    : TC_063
     * @tc.name      : release ImagePacker - promise
     * @tc.desc      : 1.create ImagePacker
     *                 2.call release
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_063', 0, async function (done) {
        const imagePackerApi = image.createImagePacker();
        if (imagePackerApi == undefined) {
            console.info('TC_063 create image packer failed');
            expect(false).assertTrue();
            done();
        } else {
            imagePackerApi.release().then(()=>{
                console.info('TC_063 success');
                expect(true).assertTrue();
                done();
            }).catch(()=>{
                console.log('TC_063 error: ' + error);
                expect(false).assertTrue();
                done();
            }) 
        }
    })

    /**
     * @tc.number    : TC_063-1 
     * @tc.name      : release ImagePacker - callback
     * @tc.desc      : 1.create ImagePacker
     *                 2.call release
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_063-1', 0, async function (done) {
        const imagePackerApi = image.createImagePacker();
        if (imagePackerApi == undefined) {
            console.info('TC_063-1 create image packer failed');
            expect(false).assertTrue();
            done();
        } else {
            imagePackerApi.release(()=>{
            console.info('TC_063-1 success');
            expect(true).assertTrue();
            done();
            })
        }
    })

    /**
     * @tc.number    : TC_064
     * @tc.name      : release ImageSource - promise - png  
     * @tc.desc      : 1.create ImageSource
     *                 2.call release()
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_064', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_064 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.release().then(()=>{
                console.info('TC_064 success');
                expect(true).assertTrue();
                done();  
            }).catch(error => {
                console.log('TC_064 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_064-1
     * @tc.name      : release ImageSource - callback - png 
     * @tc.desc      : 1.create ImageSource
     *                 2.call release()
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_064-1', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_064-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.release(() => {
                console.info('TC_064-1 Success');
                expect(true).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_065
     * @tc.name      : release ImageSource - promise - bmp 
     * @tc.desc      : 1.create ImageSource
     *                 2.call release()
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_065', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_065 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.release().then(()=>{
                console.info('TC_065 success');
                expect(true).assertTrue();
                done();  
            }).catch(error => {
                console.log('TC_065 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_065-1
     * @tc.name      : release ImageSource - callback - bmp 
     * @tc.desc      : 1.create ImageSource
     *                 2.create SourceStream
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_065-1', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_065-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.release(() => {
                console.info('TC_065-1 Success');
                expect(true).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_066
     * @tc.name      : release ImageSource - promise - gif
     * @tc.desc      : 1.create ImageSource
     *                 2.call release()
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_066', 0, async function (done) {
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        if (imageSourceApi == undefined) {
            console.info('TC_066 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.release().then(()=>{
                console.info('TC_066 success');
                expect(true).assertTrue();
                done();
            }).catch(error => {
                console.log('TC_066 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })
       
    /**
     * @tc.number    : TC_066-1
     * @tc.name      : release ImageSource - callback - gif
     * @tc.desc      : 1.create ImageSource
     *                 2.call release()
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_066-1', 0, async function (done) {
        let fdGif = fileio.openSync(pathGif);
        const imageSourceApi = image.createImageSource(fdGif);
        if (imageSourceApi == undefined) {
            console.info('TC_066-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.release(() => {
                console.info('TC_066-1 Success');
                expect(true).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_067
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:RGBA_8888-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_067', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:3,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions, (err,pixelmap) => {
                console.info('TC_067 createPixelMap ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_067-1
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:RGBA_565-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return null
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_067-1', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions, (err,pixelmap) => {
                console.info('TC_067-1 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_067-2
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:unkonwn-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_067-2', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions, (err,pixelmap) => {
                console.info('TC_067-2 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_067-3
     * @tc.name      : createPixelMap(decodingOptions:index 1})-gif 
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return null
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_067-3', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:1
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_067-3 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();   
            })
        }
    })

    /**
     * @tc.number    : TC_067-4
     * @tc.name      : createPixelMap(decodingOptions:index -1})-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return null
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_067-4', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:-1
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_067-4 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_067-5
     * @tc.name      : createPixelMap(decodingOptions:sampleSize -1})-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_067-5', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:-1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_067-5 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_067-6
     * @tc.name      : createPixelMap(decodingOptions:rotate -10})-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_067-6', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-6 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:-10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_067-6 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_067-7
     * @tc.name      : createPixelMap(decodingOptions:unsupported pixelformat)-gif 
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_067-7', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-7 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:60,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_067-7 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })
    /**
     * @tc.number    : TC_067-8
     * @tc.name      : createPixelMap(decodingOptions:editable false})-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_067-8', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-8 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: false, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_067-8 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })  

    /**
     * @tc.number    : TC_067-9
     * @tc.name      : createPixelMap(decodingOptions:desiredSize>imagesize)-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_067-9', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-9 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:10000, height:10000},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_067-9 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();   
            })
        }
    })

    /**
     * @tc.number    : TC_067-10
     * @tc.name      : createPixelMap(decodingOptions:desiredRegion>imagesize)-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_067-10', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info(' TC_067-10 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 10000, width: 10000 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_067-10 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_067-11
     * @tc.name      : createPixelMapdecodingOptions:x -1 y -1)-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_067-11', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-11 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: -1, y: -1 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_067-11 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    }) 

    /**
     * @tc.number    : TC_067-12
     * @tc.name      : createPixelMap(decodingOptions:x > image.height y > image.width)-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_067-12', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-12 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 10000, y: 10000 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_067-12 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })  

    /**
     * @tc.number    : TC_067-13
     * @tc.name      : createPixelMap(decodingOptions:rotate>360)-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_067-13', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-13 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:500,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 1, y: 2 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_067-13 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    }) 
    
    /**
     * @tc.number    : TC_067-14
     * @tc.name      : createPixelMap-promise-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_067-14', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-14 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.createPixelMap().then(pixelmap => {
                console.info('TC_067-14 success ');
                expect(pixelmap !== undefined ).assertTrue();
                done();
            }).catch(error => {
                console.log('TC_067-14 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_067-15
     * @tc.name      : createPixelMap-pcallback-gif
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return null
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_067-15', 0, async function (done) {
        let fdMovingGif = fileio.openSync(pathMovingGif);
        const imageSourceApi = image.createImageSource(fdMovingGif);
        if (imageSourceApi == undefined) {
            console.info('TC_067-15 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.createPixelMap((err, pixelmap) => {
                console.info('TC_067-15 success ');
                expect(pixelmap !== undefined ).assertTrue();
                done();
            })
        }
    })


    /**
     * @tc.number    : TC_068
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:RGBA_8888-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_068', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:3,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0 
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_068 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_068-1
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:RGB_565-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_068-1', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0 
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_068-1 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_068-2
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:unkonwn-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_068-2', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0 
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_068-2 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_068-3
     * @tc.name      : createPixelMap(decodingOptions: index 1})-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_068-3', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:1
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_068-3 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_068-4
     * @tc.name      : createPixelMap(decodingOptions:index -1})-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_068-4', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:-1
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_068-4 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_068-5
     * @tc.name      : createPixelMap(decodingOptions:sampleSize -1})-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_068-5', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:-1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_068-5 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    }) 

    /**
     * @tc.number    : TC_068-6
     * @tc.name      : createPixelMap(decodingOptions:rotate -10})-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_068-6', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-6 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:-10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_068-6 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })  

    /**
     * @tc.number    : TC_068-7
     * @tc.name      : createPixelMap(decodingOptions:unsupported pixelformat)-bmp 
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_068-7', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-7 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:60,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_068-7 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_068-8
     * @tc.name      : createPixelMap(decodingOptions:editable false})-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */  
    it('TC_068-8', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-8 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: false, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_068-8 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    }) 

    /**
     * @tc.number    : TC_068-9
     * @tc.name      : createPixelMap(decodingOptions:desiredSize>imagesize)-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_068-9', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-9 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:10000, height:10000},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_068-9 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    }) 

    /**
     * @tc.number    : TC_068-10
     * @tc.name      : createPixelMap(decodingOptions:desiredRegion>imagesize)-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_068-10', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info(' TC_068-10 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 10000, width: 10000 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_068-10 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    }) 

    /**
     * @tc.number    : TC_068-11
     * @tc.name      : createPixelMapdecodingOptions:x -1 y -1)-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_068-11', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-11 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: -1, y: -1 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_068-11 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    }) 

    /**
     * @tc.number    : TC_068-12
     * @tc.name      : createPixelMap(decodingOptions:x > image.height y > image.width)-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_068-12', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-12 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 10000, y: 10000 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_068-12 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })
    /**
     * @tc.number    : TC_068-13
     * @tc.name      : createPixelMap(decodingOptions:rotate>360)-jpg
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_068-13', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-13 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:500,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 1, y: 2 },
                index:0 
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_068-13 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })
    /**
     * @tc.number    : TC_068-14
     * @tc.name      : createPixelMap-promise-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_068-14', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-14 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.createPixelMap().then(pixelmap => {
                console.info('TC_068-14 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            }).catch(error => {
                console.log('TC_068-14 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_068-15
     * @tc.name      : createPixelMap-callback-bmp
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_068-15', 0, async function (done) {
        let fdBmp = fileio.openSync(pathBmp);
        const imageSourceApi = image.createImageSource(fdBmp);
        if (imageSourceApi == undefined) {
            console.info('TC_068-15 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.createPixelMap((err, pixelmap) => {
                console.info('TC_068-15 success ');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_163
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:RGBA_8888-png
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_163', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163 create image source failed');
            expect(false).assertTrue();
            done();
        } else {     
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:3,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0  
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_163 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_163-1
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:RGB_565-png
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_163-1', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {     
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0  
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_163-1 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_163-2
     * @tc.name      : createPixelMap(decodingOptions)-pixelformat:unkonwn-png
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_163-2', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {     
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0  
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_163-2 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_163-3
     * @tc.name      : createPixelMap(decodingOptions: index 1})-png
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_163-3', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:1
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_163-3 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_163-4
     * @tc.name      : createPixelMap(decodingOptions:index -1})-png
     * @tc.desc      : 1.create imagesource
     *                 2.set decodingOptions
     *                 3.call createPixelMap
     *                 4.set index=-1,options
     *                 5.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_163-4', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:-1
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_163-4 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_163-5
     * @tc.name      : createPixelMap(decodingOptions:sampleSize -1})-png
     * @tc.desc      : 1.create imagesource
     *                 2.set decodingOptions
     *                 3.call createPixelMap
     *                 4.set index=-1,options
     *                 5.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_163-5', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:-1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:0,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_163-5 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_163-6
     * @tc.name      : createPixelMap(decodingOptions:rotate -10})-png
     * @tc.desc      : 1.create imagesource
     *                 2.set decodingOptions
     *                 3.call createPixelMap
     *                 4.set index=-1,options
     *                 5.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_163-6', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-6 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:-10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_163-6 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })  

    /**
     * @tc.number    : TC_163-7
     * @tc.name      : createPixelMap(decodingOptions:unsupported pixelformat)-png
     * @tc.desc      : 1.create imagesource
     *                 2.set decodingOptions
     *                 3.call createPixelMap
     *                 4.set index=-1,options
     *                 5.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_163-7', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-7 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:60,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_163-7 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })  

    /**
     * @tc.number    : TC_163-8
     * @tc.name      : createPixelMap(decodingOptions:editable false})-png
     * @tc.desc      : 1.create imagesource
     *                 2.set decodingOptions
     *                 3.call createPixelMap
     *                 4.set index=-1,options
     *                 5.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */  
    it('TC_163-8', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-8 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: false, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_163-8 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_163-9
     * @tc.name      : createPixelMap(decodingOptions:desiredSize>imagesize)-png
     * @tc.desc      : 1.create imagesource
     *                 2.set decodingOptions
     *                 3.call createPixelMap
     *                 4.set index=-1,options
     *                 5.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_163-9', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-9 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:10000, height:10000},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                console.info('TC_163-9 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_163-10
     * @tc.name      : createPixelMap(decodingOptions:desiredRegion>imagesize)-png
     * @tc.desc      : 1.create imagesource
     *                 2.set decodingOptions
     *                 3.call createPixelMap
     *                 4.set index=-1,options
     *                 5.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */    
    it('TC_163-10', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info(' TC_163-10 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 10000, width: 10000 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_163-10 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    }) 

    /**
     * @tc.number    : TC_163-11
     * @tc.name      : createPixelMapdecodingOptions:x -1 y -1)-png
     * @tc.desc      : 1.create imagesource
     *                 2.set decodingOptions
     *                 3.call createPixelMap
     *                 4.set index=-1,options
     *                 5.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_163-11', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-11 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: -1, y: -1 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_163-11 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })

    /**
     * @tc.number    : TC_163-12
     * @tc.name      : createPixelMap(decodingOptions:x > image.height y > image.width)-png
     * @tc.desc      : 1.create imagesource
     *                 2.set decodingOptions
     *                 3.call createPixelMap
     *                 4.set index=-1,options
     *                 5.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */ 
    it('TC_163-12', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-12 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:10,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 10000, y: 10000 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_163-12 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }
    })
    /**
         * @tc.number    : TC_163-13
         * @tc.name      : createPixelMap(decodingOptions:rotate>360)-png
         * @tc.desc      : 1.create imagesource
         *                 2.set index and DecodeOptions
         *                 3.create PixelMap
         *                 4.callback return undefined
         * @tc.size      : MEDIUM 
         * @tc.type      : Functional
         * @tc.level     : Level 1
         */
     it('TC_163-13', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-13 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:500,
                desiredPixelFormat:2,
                desiredRegion: { size: { height: 1, width: 2 }, x: 1, y: 2 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions,(err,pixelmap) => {
                if(pixelmap == undefined){
                    console.info('TC_163-13 success ');
                    expect(true).assertTrue();
                    done();
                }else{
                    expect(false).assertTrue();
                    done();
                }   
            }) 
        }        
    })
    /**
     * @tc.number    : TC_163-14
     * @tc.name      : createPixelMap-promise-png
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_163-14', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-14 create image source failed');
            expect(false).assertTrue();
            done();
        } else {     
            imageSourceApi.createPixelMap().then(pixelmap => {
                console.info('TC_163-14 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            }).catch(error => {
                console.log('TC_163-14 error: ' + error);
                expect().assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_163-15
     * @tc.name      : createPixelMap-callback-png
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return null
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_163-15', 0, async function (done) {
        let fdPng = fileio.openSync(pathPng);
        const imageSourceApi = image.createImageSource(fdPng);
        if (imageSourceApi == undefined) {
            console.info('TC_163-15 create image source failed');
            expect(false).assertTrue();
            done();
        } else {     
            imageSourceApi.createPixelMap((err, pixelmap) => {
                console.info('TC_163-15 success');
                expect(pixelmap != undefined ).assertTrue();
                done();
            })
        }
    })
    /**
     * @tc.number    : TC_164
     * @tc.name      : imagesource supportedFormats
     * @tc.desc      : 1.create imagesource
     *                 2.call supportedFormats
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */    
    it('TC_164', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_164 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            expect(imageSourceApi.supportedFormats != undefined).assertTrue();
            console.info(imageSourceApi.supportedFormats); 
            console.info('TC_164 success ');
            done();
        }
    })

    /**
     * @tc.number    : TC_166
     * @tc.name      : imagepacker supportedFormats
     * @tc.desc      : 1.create imagepacker
     *                 2.call supportedFormats
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 0
     */    
    it('TC_166', 0, async function (done) {
        const imagePackerApi = image.createImagePacker();
        if (imagePackerApi == undefined) {
            console.info('TC_166 create image packer failed');
            expect(false).assertTrue();
            done();
        } else {
            expect(imagePackerApi.supportedFormats != undefined).assertTrue();
            console.info(imagePackerApi.supportedFormats); 
            console.info('TC_166 success ');
            done();
        }
    })

    /**
     * @tc.number    : TC_167
     * @tc.name      : createPixelMap-unsupported image format
     * @tc.desc      : 1.create imagesource
     *                 2.set index and DecodeOptions
     *                 3.create PixelMap
     *                 4.callback return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_167', 0, async function (done) {
        let fdTiff = fileio.openSync(pathTiff);
        const imageSourceApi = image.createImageSource(fdTiff);
        let decodingOptions = {
            sampleSize:1,
            editable: true, 
            desiredSize:{ width:1, height:2},
            rotate:10,
            desiredPixelFormat:3,
            desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
            index:0
        };
        imageSourceApi.createPixelMap(decodingOptions, (err, pixelmap) => {
            console.info('TC_167 success ');
            expect(pixelmap == undefined ).assertTrue();
            done();
        })
    })   

    /**
     * @tc.number    : TC_168
     * @tc.name      : isEditable
     * @tc.desc      : 1.create pixelmap
     *                 2.call isEditable 
     *                 3.return true
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_168', 0, async function (done) {
        const Color = new ArrayBuffer(96);
        let opts = { editable: true, pixelFormat: 3, size: { height: 4, width: 6 } }
        image.createPixelMap(Color, opts, (error,pixelmap) => {
            if(pixelmap == undefined){
                console.info('TC_168 create pixelmap failed');
                expect(false).assertTrue();
                done();  
            }else {
                expect(pixelmap.isEditable == true).assertTrue();
                console.info('TC_168 success ');
                done();
            }
        })   
    })

    /**
     * @tc.number    : TC_169
     * @tc.name      : Decode the image to generate a bitmap 
     * @tc.desc      : 1.create imagesource
     *                 2.create pixelmap
     *                 3.call getimageinfo
     *                 4.Judging the length and width are opposite to the original
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
     it('TC_169', 0, async function (done) {
        let fdJpg = fileio.openSync(pathJpg);
        const imageSourceApi = image.createImageSource(fdJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_169 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let decodingOptions = {
                sampleSize:1,
                editable: true, 
                desiredSize:{ width:1, height:2},
                rotate:90,
                desiredPixelFormat:3,
                desiredRegion: { size: { height: 1, width: 2 }, x: 0, y: 0 },
                index:0
            };
            imageSourceApi.createPixelMap(decodingOptions, (err, pixelmap) => {
                pixelmap.getImageInfo( (err,imageInfo) => {
                    if (imageInfo != undefined) {
                        console.info('TC_169 success');
                        expect(imageInfo.size.height == 2).assertTrue();
                        expect(imageInfo.size.width == 1).assertTrue();
                        done();
                    }else {
                        console.info('TC_169 imageInfo is empty');
                        expect(false).assertTrue()
                        done();
                    }
                })
            })
        }
    })
    /**
     * @tc.number    : TC_171
     * @tc.name      : getImageProperty(BitsPerSample)-promise
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(BitsPerSample)
     *                 4.The return value is not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_171', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_171 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("BitsPerSample")
            .then(data => {
                console.info('TC_171 BitsPerSample ' + data);
                expect(data != undefined  && data != '' ).assertTrue();
                done();
            })
            .catch(error => {
                console.log('TC_171 error: ' + error);
                expect(false).assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_171-1
     * @tc.name      : getImageProperty(Orientation)-promise
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(Orientation)
     *                 4.The return value is not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_171-1', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_171-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("Orientation")
            .then(data => {
                console.info('TC_171-1 Orientation ' + data);
                expect(data != undefined && data != '' ).assertTrue();
                done();
            })
            .catch(error => {
                console.log('TC_171-1 error: ' + error);
                expect(false).assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_171-2
     * @tc.name      : getImageProperty(ImageLength)-promise
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(ImageLength)
     *                 4.The return value is not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_171-2', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_171-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("ImageLength")
            .then(data => {
                console.info('TC_171-2 ImageLength ' + data);
                expect(data != undefined && data != '' ).assertTrue();
                done();
            })
            .catch(error => {
                console.log('TC_171-2 error: ' + error);
                expect(false).assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_171-3
     * @tc.name      : getImageProperty(ImageWidth)-promise
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(ImageWidth)
     *                 4.The return value is not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_171-3', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_171-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("ImageWidth")
            .then(data => {
                console.info('TC_171-3 ImageWidth ' + data);
                expect(data != undefined && data != '' ).assertTrue();
                done();
            })
            .catch(error => {
                console.log('TC_171-3 error: ' + error);
                expect(false).assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_171-4
     * @tc.name      : getImageProperty(GPSLatitude)-promise
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(GPSLatitude)
     *                 4.The return value is not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_171-4', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_171-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("GPSLatitude")
            .then(data => {
                console.info('TC_171-4 GPSLatitude ' + data);
                expect(data != undefined && data != '' ).assertTrue();
                done();
            })
            .catch(error => {
                console.log('TC_171-4 error: ' + error);
                expect(false).assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_171-5
     * @tc.name      : getImageProperty(GPSLongitude)-promise
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(GPSLongitude)
     *                 4.The return value is not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_171-5', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_171-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("GPSLongitude")
            .then(data => {
                console.info('TC_171-5 GPSLongitude ' + data);
                expect(data != undefined && data != '' ).assertTrue();
                done();
            })
            .catch(error => {
                console.log('TC_171-5 error: ' + error);
                expect(false).assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_171-6
     * @tc.name      : getImageProperty(GPSLatitudeRef)-promise
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(GPSLatitudeRef)
     *                 4.The return value is not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_171-6', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_171-6 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("GPSLatitudeRef")
            .then(data => {
                console.info('TC_171-6 GPSLatitudeRef ' + data);
                expect(data != undefined && data != '' ).assertTrue();
                done();
            })
            .catch(error => {
                console.log('TC_171-6 error: ' + error);
                expect(false).assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_171-7
     * @tc.name      : getImageProperty(GPSLongitudeRef)-promise
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(GPSLongitudeRef)
     *                 4.The return value is not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_171-7', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_171-7 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("GPSLongitudeRef")
            .then(data => {
                console.info('TC_171-7 GPSLongitudeRef ' + data);
                expect(data != undefined && data != '' ).assertTrue();
                done();
            })
            .catch(error => {
                console.log('TC_171-7 error: ' + error);
                expect(false).assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_171-8
     * @tc.name      : getImageProperty(DateTimeOriginal)
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(ImageLength)
     *                 4.The return value is not empty
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_171-8', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_171-8 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("DateTimeOriginal")
            .then(data => {
                console.info('TC_171-8 DateTimeOriginal ' + data);
                expect(data != undefined && data != '' ).assertTrue();
                done();
            })
            .catch(error => {
                console.log('TC_171-8 error: ' + error);
                expect(false).assertFail();
                done();
            })
        }
    })

    /**
     * @tc.number    : TC_172
     * @tc.name      : getImageProperty(BitsPerSample)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageProperty(BitsPerSample)
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_172', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_172 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("BitsPerSample",(error,data) => {
                if (error){
                    console.info('TC_172 getImageProperty BitsPerSample error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_172 BitsPerSample ' + data);
                    expect(data != undefined && data != '' ).assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_172-1
     * @tc.name      : getImageProperty(Orientation)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageProperty(Orientation)
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_172-1', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_172-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("Orientation",(error,data) => {
                if (error){
                    console.info('TC_172-1 getImageProperty Orientation error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_172-1 Orientation ' + data);
                    expect(data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_172-2
     * @tc.name      : getImageProperty(ImageLength)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageProperty(ImageLength)
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_172-2', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_172-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("ImageLength",(error,data) => {
                if (error){
                    console.info('TC_172-2 getImageProperty ImageLength error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_172-2 ImageLength ' + data);
                    expect(data != undefined && data != '' ).assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_172-3
     * @tc.name      : getImageProperty(ImageWidth)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageProperty(ImageWidth)
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_172-3', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_172-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("ImageWidth",(error,data) => {
                if (error){
                    console.info('TC_172-3 getImageProperty ImageWidth error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_172-3 ImageWidth ' + data);
                    expect(data != undefined && data != '' ).assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_172-4
     * @tc.name      : getImageProperty(GPSLatitude)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageProperty(GPSLatitude)
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_172-4', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_172-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("GPSLatitude",(error,data) => {
                if (error){
                    console.info('TC_172-4 getImageProperty GPSLatitude error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_172-4 GPSLatitude ' + data);
                    expect(data != undefined && data != '' ).assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_172-5
     * @tc.name      : getImageProperty(GPSLongitude)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageProperty(GPSLongitude)
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_172-5', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_172-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("GPSLongitude",(error,data) => {
                if (error){
                    console.info('TC_172-5 getImageProperty GPSLongitude error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_172-5 GPSLongitude ' + data);
                    expect(data != undefined && data != '' ).assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_172-6
     * @tc.name      : getImageProperty(GPSLatitudeRef)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageProperty(GPSLatitudeRef)
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_172-6', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_172-6 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("GPSLatitudeRef",(error,data) => {
                if (error){
                    console.info('TC_172-6 getImageProperty GPSLatitudeRef error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_172-6 GPSLatitudeRef ' + data);
                    expect(data != undefined && data != '' ).assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_172-7
     * @tc.name      : getImageProperty(GPSLongitudeRef)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageProperty(GPSLongitudeRef)
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_172-7', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_172-7 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("GPSLongitudeRef",(error,data) => {
                if (error){
                    console.info('TC_172-7 getImageProperty GPSLongitudeRef error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_172-7 GPSLongitudeRef ' + data);
                    expect(data != undefined && data != '' ).assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_172-8
     * @tc.name      : getImageProperty(DateTimeOriginal)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.call getImageProperty(DateTimeOriginal)
     *                 3.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_172-8', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_172-8 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            imageSourceApi.getImageProperty("DateTimeOriginal",(error,data) => {
                if (error){
                    console.info('TC_172-8 getImageProperty DateTimeOriginal error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_172-8 DateTimeOriginal ' + data);
                    expect(data != undefined && data != '' ).assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_173
     * @tc.name      : getImageProperty(BitsPerSample,property)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(BitsPerSample,property)
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_173', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_173 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let property = {index:0,defaultValue:'9999'}
            imageSourceApi.getImageProperty("BitsPerSample",property,(error,data) => {
                if (error){
                    console.info('TC_173 getImageProperty BitsPerSample error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_173 BitsPerSample ' + data);
                    expect(data != '9999' && data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_173-1
     * @tc.name      : getImageProperty(Orientation,property)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(Orientation,property)
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_173-1', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_173-1 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let property = {index:0,defaultValue:'9999'}
            imageSourceApi.getImageProperty("Orientation",property,(error,data) => {
                if (error){
                    console.info('TC_173-1 getImageProperty Orientation error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_173-1 Orientation ' + data);
                    expect(data != '9999' && data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_173-2
     * @tc.name      : getImageProperty(ImageLength,property)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(ImageLength,property)
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_173-2', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_173-2 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let property = {index:0,defaultValue:'9999'}
            imageSourceApi.getImageProperty("ImageLength",property,(error,data) => {
                if (error){
                    console.info('TC_173-2 getImageProperty ImageLength error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_173-2 ImageLength ' + data);
                    expect(data != '9999' && data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_173-3
     * @tc.name      : getImageProperty(ImageWidth,property)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(ImageWidth,property)
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_173-3', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_173-3 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let property = {index:0,defaultValue:'9999'}
            imageSourceApi.getImageProperty("ImageWidth",property,(error,data) => {
                if (error){
                    console.info('TC_173-3 getImageProperty ImageWidth error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_173-3 ImageWidth ' + data);
                    expect(data != '9999' && data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_173-4
     * @tc.name      : getImageProperty(GPSLatitude,property)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(GPSLatitude,property)
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_173-4', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_173-4 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let property = {index:0,defaultValue:'9999'}
            imageSourceApi.getImageProperty("GPSLatitude",property,(error,data) => {
                if (error){
                    console.info('TC_173-4 getImageProperty GPSLatitude error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_173-4 GPSLatitude ' + data);
                    expect(data != '9999' && data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_173-5
     * @tc.name      : getImageProperty(GPSLongitude,property)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(GPSLongitude,property)
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_173-5', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_173-5 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let property = {index:0,defaultValue:'9999'}
            imageSourceApi.getImageProperty("GPSLongitude",property,(error,data) => {
                if (error){
                    console.info('TC_173-5 getImageProperty GPSLongitude error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_173-5 GPSLongitude ' + data);
                    expect(data != '9999' && data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_173-6
     * @tc.name      : getImageProperty(GPSLatitudeRef,property)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(GPSLatitudeRef,property)
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_173-6', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_173-6 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let property = {index:0,defaultValue:'9999'}
            imageSourceApi.getImageProperty("GPSLatitudeRef",property,(error,data) => {
                if (error){
                    console.info('TC_173-6 getImageProperty GPSLatitudeRef error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_173-6 GPSLatitudeRef ' + data);
                    expect(data != '9999' && data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_173-7
     * @tc.name      : getImageProperty(GPSLongitudeRef,property)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(GPSLongitudeRef,property)
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_173-7', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_173-7 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let property = {index:0,defaultValue:'9999'}
            imageSourceApi.getImageProperty("GPSLongitudeRef",property,(error,data) => {
                if (error){
                    console.info('TC_173-7 getImageProperty GPSLongitudeRef error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_173-7 GPSLongitudeRef ' + data);
                    expect(data != '9999' && data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })

    /**
     * @tc.number    : TC_173-8
     * @tc.name      : getImageProperty(DateTimeOriginal,property)-callback
     * @tc.desc      : 1.create imagesource
     *                 2.set property
     *                 3.call getImageProperty(DateTimeOriginal,property)
     *                 4.return undefined
     * @tc.size      : MEDIUM 
     * @tc.type      : Functional
     * @tc.level     : Level 1
     */
    it('TC_173-8', 0, async function (done) {
        let fdExifJpg = fileio.openSync(pathExifJpg);
        const imageSourceApi = image.createImageSource( fdExifJpg);
        if (imageSourceApi == undefined) {
            console.info('TC_173-8 create image source failed');
            expect(false).assertTrue();
            done();
        } else {
            let property = {index:0,defaultValue:'9999'}
            imageSourceApi.getImageProperty("DateTimeOriginal",property,(error,data) => {
                if (error){
                    console.info('TC_173-8 getImageProperty DateTimeOriginal error');
                    expect(false).assertTrue();
                    done();
                }else{
                    console.info('TC_173-8 DateTimeOriginal ' + data);
                    expect(data != '9999' && data != undefined && data != '').assertTrue();
                    done();
                }
            })
        }
    })
})
