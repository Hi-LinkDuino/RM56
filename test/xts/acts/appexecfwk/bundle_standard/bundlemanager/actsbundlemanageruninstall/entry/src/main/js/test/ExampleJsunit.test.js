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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import demo from '@ohos.bundle'

const PATH = "/data/"
const ERROR = "error.hap"
const BMSJSTEST1 = "bmsJstest1.hap"
const BMSJSTEST2 = "bmsJstest2.hap"
const BMSJSTEST3 = "bmsJstest3.hap"
const BMSJSTEST4 = "bmsJstest4.hap"
const BMSJSTEST5 = "bmsJstest5.hap"
const BMSJSTEST6 = "bmsJstest6.hap"
const BMSJSTEST8 = "bmsJstest8.hap"
const NAME1 = "com.example.myapplication1"
const NAME2 = "com.example.myapplication2"
const NAME3 = "com.example.myapplication4"
const NAME4 = "com.example.myapplication5"
const NAME5 = "com.example.myapplication6"
const THIRD1 = "com.example.third1"
const LAUNCHER = "com.ohos.launcher"
const SUCCESS = "SUCCESS"
let installParam = {
    userId: 100,
    installFlag: 1,
    isKeepData: false
}

describe('ActsBundleManagerUninstall', function () {

    /**
     * @tc.number uninstall_0100
     * @tc.name BUNDLE::uninstall
     * @tc.desc Test uninstall interfaces.
     */
    it('uninstall_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST1], installParam, async (err, data) => {
            checkInstallResult(data)
            await queryInfo(NAME1, 0)
            installData.uninstall(NAME1, installParam, async (err, data) => {
                checkInstallResult(data)
                await queryInfo(NAME1, 1)
                done()
            })
        })
    })

    /**
     * @tc.number uninstall_0200
     * @tc.name BUNDLE::uninstall
     * @tc.desc Test uninstall interfaces.
     */
    it('uninstall_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST2, PATH + BMSJSTEST3], installParam, async (err, data) => {
            checkInstallResult(data)
            let bundleInfo = await demo.getBundleInfo(NAME2, demo.BundleFlag.GET_BUNDLE_DEFAULT);
            expect(bundleInfo.appInfo.moduleSourceDirs.length).assertEqual(2)
            installData.uninstall(NAME2, installParam, async(err, data) => {
                checkInstallResult(data)
                await queryInfo(NAME2, 1)
                done()
            })
        })
    })

    /**
     * @tc.number uninstall_0300
     * @tc.name BUNDLE::uninstall
     * @tc.desc Test uninstall interfaces.
     */
    it('uninstall_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller()
        installData.install([PATH + BMSJSTEST4], installParam, async (err, data) => {
            checkInstallResult(data)
            installData.install([PATH + BMSJSTEST5], installParam, async (err, data) => {
                checkInstallResult(data)
                installData.install([PATH + BMSJSTEST6], installParam, async (err, data) => {
                    checkInstallResult(data)
                    await queryInfo(NAME3, 0)
                    await queryInfo(NAME4, 0)
                    await queryInfo(NAME5, 0)
                    installData.uninstall(NAME3, installParam, async (err, data) => {
                        checkInstallResult(data)
                        installData.uninstall(NAME4, installParam, async (err, data) => {
                            checkInstallResult(data)
                            installData.uninstall(NAME5, installParam, async (err, data) => {
                                checkInstallResult(data)
                                await queryInfo(NAME3, 1)
                                await queryInfo(NAME4, 1)
                                await queryInfo(NAME5, 1)
                                done()
                            })
                        })
                    })
                })
            })
        })
    })

    /**
     * @tc.number uninstall_0400
     * @tc.name BUNDLE::uninstall
     * @tc.desc Test uninstall interfaces.
     */
    it('uninstall_0400', 0, async function (done) {
        demo.getBundleInstaller().then((data) => {
            data.uninstall(ERROR, installParam, (err, data) => {
                expect(data.status).assertEqual(demo.InstallErrorCode.STATUS_UNINSTALL_FAILURE)
                expect(data.statusMessage).assertEqual("STATUS_UNINSTALL_FAILURE")
                done()
            })
        })
    })

    /**
     * @tc.number uninstall_0500
     * @tc.name BUNDLE::uninstall
     * @tc.desc Test uninstall interfaces.
     */
    it('uninstall_0500', 0, async function (done) {
        demo.getBundleInstaller().then((data) => {
            data.uninstall('', installParam, (err, data) => {
                expect(data.status).assertEqual(demo.InstallErrorCode.STATUS_UNINSTALL_FAILURE_ABORTED)
                expect(data.statusMessage).assertEqual("STATUS_UNINSTALL_FAILURE_ABORTED")
                done()
            })
        })
    })

    /**
     * @tc.number uninstall_0600
     * @tc.name BUNDLE::uninstall
     * @tc.desc Test uninstall interfaces.
     */
    it('uninstall_0600', 0, async function (done) {
        demo.getBundleInstaller().then((data) => {
            data.uninstall(LAUNCHER, installParam, (err, data) => {
                expect(data.status).assertEqual(demo.InstallErrorCode.STATUS_UNINSTALL_FAILURE_CONFLICT)
                expect(data.statusMessage).assertEqual("STATUS_UNINSTALL_FAILURE_CONFLICT")
                done()
            })
        })
    })

    /**
     * @tc.number uninstall_0700
     * @tc.name BUNDLE::uninstall
     * @tc.desc Test uninstall interfaces.
     */
    it('uninstall_0700', 0, async function (done) {
        let result = await demo.getBundleInstaller();
        result.install([PATH + BMSJSTEST1], {
            userId: 100,
            installFlag: 1,
            isKeepData: true
        }, async (err, data) => {
            checkInstallResult(data)
            await queryInfo(NAME1, 0)
            result.uninstall(NAME1, installParam, async (err, data) => {
                checkInstallResult(data)
                await queryInfo(NAME1, 1)
                done()
            })
        })
    })

    /**
     * @tc.number uninstall_0800
     * @tc.name BUNDLE::uninstall
     * @tc.desc Test uninstall interfaces.
     */
    it('uninstall_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + BMSJSTEST8], installParam, async (err, data) => {
            checkInstallResult(data)
            let bundleInfo = await demo.getBundleInfo('com.example.third1', demo.BundleFlag.GET_BUNDLE_DEFAULT);
            expect(bundleInfo.uid).assertLarger(10000);
            installData.uninstall(THIRD1, installParam, async (err, data) => {
                checkInstallResult(data)
                await queryInfo('com.example.third1', 1)
                done()
            })
        })
    })

    async function queryInfo(bundleName, assertFlag){
        if (!assertFlag) {
            await demo.getBundleInfo(bundleName, demo.BundleFlag.GET_BUNDLE_DEFAULT
                ).then(datainfo => {
                console.info("getBundleInfo result0"+JSON.stringify(datainfo))
                expect(datainfo.name).assertEqual(bundleName)
            }).catch(err => {
                console.info("getBundleInfo result0"+JSON.stringify(datainfo))
                expect(err).assertFail()
            })
        } else {
            await demo.getBundleInfo(bundleName, demo.BundleFlag.GET_BUNDLE_DEFAULT
                ).then(datainfo => {
                console.info("getBundleInfo result1"+JSON.stringify(datainfo))
                expect(datainfo).assertFail()
            }).catch(err => {
                console.info("getBundleInfo result1"+JSON.stringify(err))
                expect(err).assertEqual(1)
            })
        }
    }

    function checkInstallResult(result) {
        console.info("install uninstall result" + JSON.stringify(result))
        expect(result.status).assertEqual(demo.InstallErrorCode.SUCCESS)
        expect(result.statusMessage).assertEqual(SUCCESS)
    }
})