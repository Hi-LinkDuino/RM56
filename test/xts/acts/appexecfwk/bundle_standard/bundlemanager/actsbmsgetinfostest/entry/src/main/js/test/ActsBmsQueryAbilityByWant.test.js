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
import bundle from '@ohos.bundle'
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit'

const BUNDLE_PATH1 = '/data/test/bmsThirdBundleTest1.hap';
const BUNDLE_NAME1 = 'com.example.third1';

describe('ActsBmsQueryAbilityByWant', function () {
    /*
    * @tc.number: bms_queryAbilityByWant_0100
    * @tc.name:  queryAbilityByWant callback by other callback
    * @tc.desc: 1.queryAbilityByWant callback
    *           2.queryAbilityByWant for third app
    */
    it('bms_queryAbilityByWant_0100', 0, async function (done){
        console.info('=====================bms_queryAbilityByWant_0100==================');
        let bundlePath = [BUNDLE_PATH1]
        bundle.getBundleInstaller().then(installer => {
            function onReceiveinstallEvent(err, data) {
                console.log('bms_queryAbilityByWant_0100 install called: ' + data)
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
    
                bundle.queryAbilityByWant({
                    action: ['action.system.home'],
                    entities: ['entity.system.home'],
                    bundleName: BUNDLE_NAME1
                }, bundle.BundleFlag.GET_ABILITY_INFO_WITH_APPLICATION|bundle.BundleFlag.GET_ABILITY_INFO_SYSTEMAPP_ONLY, 
                100, (err, data) => {
                    if (err) {
                        console.log('bms_queryAbilityByWant_0100 test query system app err is ' + err)
                        expect(err).assertEqual(1);
                    }
                    installer.uninstall(BUNDLE_NAME1,
                         {
                            userId: 100,
                            installFlag: 1,
                            isKeepData: false
                        }
                    , (err, data) => {
                        expect(err.code).assertEqual(0);
                        expect(data.status).assertEqual(0);
                        expect(data.statusMessage).assertEqual('SUCCESS');
                        done();
                    });
                }
                )
            }

            installer.install(['/data/test/bmsThirdBundleTest1.hap'], 
            {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false,
            }, onReceiveinstallEvent);
        })
    })

    /*
    * @tc.number: bms_queryAbilityByWant_0200
    * @tc.name:  queryAbilityByWant callback by other callback
    * @tc.desc: 1.queryAbilityByWant callback
    *           2.queryAbilityByWant for systemapp
    */
    it('bms_queryAbilityByWant_0200', 0, async function (done){
        console.info('=====================bms_queryAbilityByWant_0200==================');
        bundle.queryAbilityByWant(
            {
                action: ['action.system.home'],
                entities: ['entity.system.home']
            }, 
            bundle.BundleFlag.GET_ABILITY_INFO_WITH_APPLICATION|bundle.BundleFlag.GET_ABILITY_INFO_SYSTEMAPP_ONLY, 
        100).then(data => {
                for(let i = 0; i < data.length; ++i) {
                    let jsondata = JSON.stringify(data[i]);
                    console.log('bms_queryAbilityByWant_0200 test query system app ' + jsondata)
                    expect(data[i].applicationInfo.systemApp).assertEqual(true)
                }
                done();
            }
        ).catch(err => {
            console.log('bms_queryAbilityByWant_0200 test query system app err is ' + err)
            expect(err).assertFail()
            done();
        })
    })

})