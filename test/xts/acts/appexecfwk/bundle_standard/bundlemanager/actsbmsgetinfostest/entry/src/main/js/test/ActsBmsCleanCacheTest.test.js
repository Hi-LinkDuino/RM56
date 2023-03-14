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

describe('ActsCleancache', function () {
    
    /*
    * @tc.number: bms_cleancache_0200
    * @tc.name:  cleancache callback by other callback
    * @tc.desc: 1.cleancache callback
    *           2.cleancache for right input
    */
     it('bms_cleancache_0100', 0, async function (done){
        console.info('=====================bms_cleancache_0100==================');

        let bundlePath = [BUNDLE_PATH1]
        bundle.getBundleInstaller().then(installer => {
            function onReceiveinstallEvent(err, data) {
                console.log('bms_cleancache_0100 install called: ' + data)
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
    
                bundle.cleanBundleCacheFiles('com.example.third1', (err) => {
                    if (err) {
                        console.log('bms_cleancache_0100 test query system app err is ' + err)
                        expect(err).assertEqual(1);
                    }
                    expect(err).assertEqual(undefined);
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
    * @tc.number: bms_cleancache_0200
    * @tc.name:  cleancache callback by other callback
    * @tc.desc: 1.cleancache callback
    *           2.cleancache for null input
    */
     it('bms_cleancache_0200', 0, async function (done){
        console.info('=====================bms_cleancache_0100==================');

        let bundlePath = [BUNDLE_PATH1]
        bundle.getBundleInstaller().then(installer => {
            function onReceiveinstallEvent(err, data) {
                console.log('bms_cleancache_0100 install called: ' + data)
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
    
                bundle.cleanBundleCacheFiles(null, (err) => {
                    if (err) {
                        console.log('bms_cleancache_0200 test query system app err is ' + err)
                        expect(err).assertEqual(2);
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
      * @tc.number: bms_cleancache_0300
      * @tc.name:  cleancache callback by other callback
      * @tc.desc: 1.cleancache callback
      *           2.cleancache for wrong input
      */
      it('bms_cleancache_0300', 0, async function (done){
        console.info('=====================bms_cleancache_0100==================');

        let bundlePath = [BUNDLE_PATH1]
        bundle.getBundleInstaller().then(installer => {
            function onReceiveinstallEvent(err, data) {
                console.log('bms_cleancache_0300 install called: ' + data)
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                expect(data.statusMessage).assertEqual('SUCCESS');
    
                bundle.cleanBundleCacheFiles("wrong bundle name", (err) => {
                    if (err) {
                        console.log('bms_cleancache_0300 test query system app err is ' + err)
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

})