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
import hidebug from '@ohos.hidebug'
import fileio from '@ohos.fileio'
import {describe, it, expect} from 'deccjsunit/index'

describe('HidebugJsTest', function () {

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0001
     * @tc.name      : getNativeHeapSize
     * @tc.desc      : Get total native heap memory size
     */
    it('DFX_DFR_Hiprofiler_Interface_0001', 0, function() {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0001 Test start*************');
        try {
            let heapSize = hidebug.getNativeHeapSize();
            expect(heapSize).assertEqual(BigInt(0));
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0001 Test end*************');
        done();
    });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0002
     * @tc.name      : getNativeHeapAllocatedSize
     * @tc.desc      : Get Native heap memory allocation size.
     */
    it('DFX_DFR_Hiprofiler_Interface_0002', 0, function(done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0002 Test start*************');
        try {
            let heapAllocatedSize = hidebug.getNativeHeapAllocatedSize();
            expect(heapAllocatedSize).assertEqual(BigInt(0));
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0002 Test end*************');
        done();
    });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0003
     * @tc.name      : getNativeHeapFreeSize
     * @tc.desc      : Get Native heap memory free size
     */
    it('DFX_DFR_Hiprofiler_Interface_0003', 0, function(done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0003 Test start*************');
        try {
            let heapFreeSize = hidebug.getNativeHeapFreeSize();
            expect(heapFreeSize).assertEqual(BigInt(0));
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0003 Test end*************');
        done();
    });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0004
     * @tc.name      : getPss
     * @tc.desc      : Get application process proportional set size memory information
     */
    it('DFX_DFR_Hiprofiler_Interface_0004', 0, async function (done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0004 Test start*************');
        try {
            const times = 3;
            let pssValue = BigInt(0);
            for(let i = 0; i < times; i++){
                let arr = new Array(i*100000).fill(1);
                let tmp = hidebug.getPss();
                console.info('No.' + i + ' tmp: ' + tmp);
                expect(tmp).assertLarger(pssValue);
                pssValue = tmp;
            }
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0004 Test end*************');
        done();
    });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0005
     * @tc.name      : getSharedDirty
     * @tc.desc      : Obtains the size of the shared dirty memory of a process.
     */
    it('DFX_DFR_Hiprofiler_Interface_0005', 0, async function (done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0005 Test start*************');
        try {
            let dirty = hidebug.getSharedDirty();
            let temp = hidebug.getSharedDirty();
            expect(dirty).assertEqual(temp);
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0005 Test end*************');
        done();
    });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0006
     * @tc.name      : startProfiling with normal parameter
     * @tc.desc      : Start CPU Profiling.
     */
    it('DFX_DFR_Hiprofiler_Interface_0006', 0, async function (done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0006 Test start*************');
        try {
            let timestamp = Date.now();
            let filename = 'cpuprofiler-' + timestamp.toString();
            console.info(filename);
            let path = '/data/accounts/account_0/appdata/com.hidebug.test/files/' + filename + '.json'
            hidebug.startProfiling(filename);
            let temp = 100;
            hidebug.stopProfiling();
            fileio.accessSync(path, 0);
            console.info('file is exists:', path);
            expect(true).assertTrue();
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0006 Test end*************');
        done();
    });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0007
     * @tc.name      : startProfiling repeatedly
     * @tc.desc      : Start CPU Profiling.
     */
    it('DFX_DFR_Hiprofiler_Interface_0007', 0, async function (done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0007 Test start*************');
        try {
            let path1 = '/data/accounts/account_0/appdata/com.hidebug.test/files/cpuprofiler-111.json'
            let path2 = '/data/accounts/account_0/appdata/com.hidebug.test/files/cpuprofiler-222.json'
            let path3 = '/data/accounts/account_0/appdata/com.hidebug.test/files/cpuprofiler-333.json'
            hidebug.startProfiling('cpuprofiler-111');
            hidebug.startProfiling('cpuprofiler-222');
            hidebug.startProfiling('cpuprofiler-333');
            let temp = 100;
            hidebug.stopProfiling();
            hidebug.stopProfiling();
            fileio.accessSync(path1, 0);
            fileio.accessSync(path2, 0);
            fileio.accessSync(path3, 0);
            let res = fileio.readTextSync(path1);
            let tmp = JSON.stringify(res);
            expect(tmp.length).assertLarger(2);
            res = fileio.readTextSync(path2);
            tmp = JSON.stringify(res);
            expect(tmp.length).assertEqual(2);
            res = fileio.readTextSync(path3);
            tmp = JSON.stringify(res);
            expect(tmp.length).assertEqual(2);
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0007 Test end*************');
        done();
    });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0008
     * @tc.name      : startProfiling with empty parameter
     * @tc.desc      : Start CPU Profiling.
     */
    it('DFX_DFR_Hiprofiler_Interface_0008', 0, async function (done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0008 Test start*************');
        let path = '/data/accounts/account_0/appdata/com.hidebug.test/files/.json'
        try {
            fileio.accessSync(path, 0);
            let res = fileio.unlinkSync(path);
            console.log(JSON.stringify(res));
        } catch(e) {
            console.info('the file is not exists');
        }
        try {
            hidebug.startProfiling('');
            let temp = 100;
            hidebug.stopProfiling();
            fileio.accessSync(path, 0);
            console.info('file is exists:', path);
            expect().assertFail();
        } catch (error) {
            expect(true).assertTrue();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0008 Test end*************');
        done();
    });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0009
     * @tc.name      : startProfiling with overlong parameter
     * @tc.desc      : Start CPU Profiling.
     */
    it('DFX_DFR_Hiprofiler_Interface_0009', 0, async function (done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0009 Test start*************');
        let path = '/data/accounts/account_0/appdata/com.hidebug.test/files/undefined.json'
        try {
            fileio.accessSync(path, 0);
            let res = fileio.unlinkSync(path);
            console.log(JSON.stringify(res));
        } catch(e) {
            console.info('the file is not exists');
        }
        let str = '';
        for (let i = 0; i < 1000; i++) {
            str += 'abcd';
        }
        try {
            hidebug.startProfiling(str);
            let temp = 100;
            hidebug.stopProfiling();
            fileio.accessSync(path, 0);
            console.info('file is exists:', path);
            expect(true).assertTrue();
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0009 Test end*************');
        done();
    });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0010
     * @tc.name      : dumpHeapData with normal parameter
     * @tc.desc      : Dump JS Virtual Machine Heap Snapshot.
     */
    it('DFX_DFR_Hiprofiler_Interface_0010', 0, async function (done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0010 Test start*************');
        try {
            let timestamp = Date.now();
            let filename = 'heap-' + timestamp.toString();
            console.info(filename);
            let path = '/data/accounts/account_0/appdata/com.hidebug.test/files/' + filename + '.heapsnapshot'
            hidebug.dumpHeapData(filename);
            fileio.accessSync(path, 0);
            console.info('file is exists:', path);
            expect(true).assertTrue();
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0010 Test end*************');
        done();
      });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0011
     * @tc.name      : dumpHeapData with empty parameter
     * @tc.desc      : Dump JS Virtual Machine Heap Snapshot.
     */
    it('DFX_DFR_Hiprofiler_Interface_0011', 0, async function (done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0011 Test start*************');
        let path = '/data/accounts/account_0/appdata/com.hidebug.test/files/.heapsnapshot'
        try {
            fileio.accessSync(path, 0);
            let res = fileio.unlinkSync(path);
            console.log(JSON.stringify(res));
        } catch(e) {
            console.info('the file is not exists');
        }
        try {
            hidebug.dumpHeapData('');
            fileio.accessSync(path, 0);
            console.info('file is exists:', path);
            expect().assertFail();
        } catch (error) {
            expect(true).assertTrue();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0011 Test end*************');
        done();
      });

    /*
     * @tc.number    : DFX_DFR_Hiprofiler_Interface_0012
     * @tc.name      : dumpHeapData with overlog parameter
     * @tc.desc      : Dump JS Virtual Machine Heap Snapshot.
     */
    it('DFX_DFR_Hiprofiler_Interface_0012', 0, async function (done) {
        console.log('************* DFX_DFR_Hiprofiler_Interface_0012 Test start*************');
        let path = '/data/accounts/account_0/appdata/com.hidebug.test/files/undefined.heapsnapshot'
        try {
            fileio.accessSync(path, 0);
            let res = fileio.unlinkSync(path);
            console.log(JSON.stringify(res));
        } catch(e) {
            console.info('the file is not exists');
        }
        let str = '';
        for (let i = 0; i < 1000; i++) {
            str += 'abcd';
        }
        try {
            hidebug.dumpHeapData(str);
            fileio.accessSync(path, 0);
            console.info('file is exists:', path);
            expect(true).assertTrue();
        } catch (error) {
            expect().assertFail();
        }
        console.log('************* DFX_DFR_Hiprofiler_Interface_0012 Test end*************');
        done();
    });
    
})
