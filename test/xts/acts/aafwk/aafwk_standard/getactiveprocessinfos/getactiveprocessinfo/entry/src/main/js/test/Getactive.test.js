/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
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
import appmgr from '@ohos.app.abilityManager'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('Getacitve', function () {

/*
* @tc.number: FWK_ProcessInfo_0100
* @tc.name: getActiveProcessInfos 
* @tc.desc: A process retrieves information
*/
it('FWK_ProcessInfo_0100', 0, async function (done) {
    console.log('FWK_ProcessInfo_0100 start');
    try{
        appmgr.getActiveProcessInfos(
        (error, data) => {
        expect(data[0].uid > 0).assertTrue();
        expect(data[0].pid > 0).assertTrue();
        expect(data[0].processName !=  null).assertTrue();
        expect(data[0].bundleNames !=  null).assertTrue();
        done();
    });
    }catch(err){
        console.log('FWK_ProcessInfo_0100 err' + err);
        done();
}

})

/*
* @tc.number: FWK_ProcessInfo_0200
* @tc.name: getActiveProcessInfos 
* @tc.desc: Two processes get the information
*/
it('FWK_ProcessInfo_0200', 0, async function (done) {
    console.log('FWK_ProcessInfo_0200 start');
    try{
    appmgr.getActiveProcessInfos(
        (error, data) => {
            var len = data.length;
            for(var i =1; i<len; ++i){
            expect(data[i].uid > 0).assertTrue();
            expect(data[i].pid > 0).assertTrue();
            expect(data[i].processName !=  null).assertTrue();
            expect(data[i].bundleNames !=  null).assertTrue();        
            }            
            done();            
        })
    }catch(err){
        console.log('FWK_ProcessInfo_0200 err' + err);
        done();
    }
    
})

/*
* @tc.number: FWK_ProcessInfo_0300
* @tc.name: getActiveProcessInfos 
* @tc.desc: A process retrieves information
*/
it('FWK_ProcessInfo_0300', 0, async function (done) {
    console.log('FWK_ProcessInfo_0300 Begin');
    try{
        appmgr.getActiveProcessInfos().then((data) => {
        expect(data[0].uid > 0).assertTrue();
        expect(data[0].pid > 0).assertTrue();
        expect(data[0].processName !=  null).assertTrue();
        expect(data[0].bundleNames !=  null).assertTrue();
        done();
    }).catch((error) => {
        console.log('FWK_ProcessInfo_0300 error code is ' + error.code);
        done();
    });
    }catch(err){
    console.log('FWK_ProcessInfo_0300 err' + err);
        done();
}
})

/*
* @tc.number: FWK_ProcessInfo_0400
* @tc.name: getActiveProcessInfos 
* @tc.desc: Two processes get the information
*/
it('FWK_ProcessInfo_0400', 0, async function (done) {
    console.log('FWK_ProcessInfo_0400 Begin');
    try{
    appmgr.getActiveProcessInfos().then((data) => {
        var len = data.length;
        for(var i =1; i<len; ++i){
        expect(data[i].uid > 0).assertTrue();
        expect(data[i].pid > 0).assertTrue();
        expect(data[i].processName !=  null).assertTrue();
        expect(data[i].bundleNames !=  null).assertTrue();
        }
        done();
    }).catch((error) => {
        console.log('FWK_ProcessInfo_0400 error code is ' + error.code);
        done();
    });
    }catch(err){
    console.log('FWK_ProcessInfo_0400 err' + err);
        done();
}
})

/*
* @tc.number: FWK_MemoryInfo_0100
* @tc.name: getSystemMemoryAttr 
* @tc.desc: Anapplication memory information
*/
it('FWK_MemoryInfo_0100', 0, async function (done) {
    console.log('FWK_MemoryInfo_0100 start');
    try{
    appmgr.getSystemMemoryAttr(
        (error, data) => {
            expect(data.availSysMem > 0).assertTrue();
            expect(data.totalSysMem > 0).assertTrue();
            expect(data.threshold > 0).assertTrue();
            expect(data.isSysInlowMem == (data.availSysMem < data.threshold)).assertTrue();
            done();
        });
    }catch(err){
        console.log('FWK_ProcessInfo_0400 err' + err); 
    }
    console.log('ZLM-----onClick1 End');
})

/*
* @tc.number: FWK_MemoryInfo_0200
* @tc.name: getSystemMemoryAttr 
* @tc.desc: Anapplication memory information
*/
it('FWK_MemoryInfo_0200', 0, async function (done) {
    console.log('FWK_MemoryInfo_0200 Begin');
    try{
    appmgr.getSystemMemoryAttr().then((data) => {
        expect(data.availSysMem > 0).assertTrue();
        expect(data.totalSysMem > 0).assertTrue();
        expect(data.threshold > 0).assertTrue();
        expect(data.isSysInlowMem == (data.availSysMem < data.threshold)).assertTrue();
        done();
    }).catch((error) => {
        console.log('FWK_MemoryInfo_0200 error code is ' + error.code);
        done();
    });
    }catch(err){
       console.log('FWK_ProcessInfo_0400 err' + err); 
    }
    console.log('FWK_MemoryInfo_0200 End');
})
})
