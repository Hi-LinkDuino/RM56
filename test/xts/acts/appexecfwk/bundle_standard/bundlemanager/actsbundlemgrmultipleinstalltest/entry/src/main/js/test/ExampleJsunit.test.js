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

const PATH = "/data/test/resource/bms/install_bundle/"

const FIRST_RIGHT = "first_right.hap"
const SECOND_RIGHT = "second_right.hap"
const THIRD_RIGHT = "third_right.hap"
const FOURTH_RIGHT = "fourth_right.hap"
const EIGHTH_RIGHT = "eighth_right.hap"
const NINTH_RIGHT = "ninth_right.hap"
const TENTH_RIGHT = "tenth_right.hap"
const ELEVEBTH_RIGHT = "eleventh_right.hap"
const FIFTH_RIGHT = "fifth_right.hap"
const SECOND_BACKUP_RIGHT = "second_backup_right.hap"
const TWELFTH_RIGHT = "twelfth_right.hap"
const SIXTH_RIGHT = "sixth_right.hap"
const THIRTEENTH_RIGHT = "thirteenth_right.hap"
const FOURTEENTH_RIGHT = "fourteenth_right.hap"
const SIXTEENTH_RIGHT = "sixteenth_right.hap"

const NAME = "com.example.l3jsdemo"
const ERR_CODE = -1;
const STATUS_INSTALL_FAILURE_INVALID = 3;
const STATUS_INSTALL_FAILURE_CONFLICT = 4;

const STATUS_INSTALL_FAILURE_INCOMPATIBLE = 6;

describe('ActsBundleMgrMultipleInstallTest', function () {

    /**
     * @tc.number BMS_Multiple_Hap_Install_0100
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([""], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_INVALID);
            console.info('===BMS_Multiple_Hap_Install_0100.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_INVALID");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_0200
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT,""], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_INVALID);
            console.info('===BMS_Multiple_Hap_Install_0200.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_INVALID");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_0300
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Install_0300.Message1===' + JSON.stringify(data.statusMessage));
            installData.uninstall(NAME, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data1) => {
                console.info('===BMS_Multiple_Hap_Install_0300.Message3===' + JSON.stringify(data1.statusMessage));
                expect(data1.statusMessage).assertEqual('SUCCESS');
                done();
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_0400
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_0400', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT, PATH + THIRD_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_CONFLICT);
            console.info('===BMS_Multiple_Hap_Install_0400.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_CONFLICT");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_0500
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_0500', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT, PATH + FOURTH_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_CONFLICT);
            console.info('===BMS_Multiple_Hap_Install_0500.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_CONFLICT");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_0600
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + EIGHTH_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_CONFLICT);
            console.info('===BMS_Multiple_Hap_Install_0600.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_CONFLICT");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_0700
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_0700', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + NINTH_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_CONFLICT);
            console.info('===BMS_Multiple_Hap_Install_0700.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_CONFLICT");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_0800
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + TENTH_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_CONFLICT);
            console.info('===BMS_Multiple_Hap_Install_0800.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_CONFLICT");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_0900
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_0900', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + ELEVEBTH_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_CONFLICT);
            console.info('===BMS_Multiple_Hap_Install_0900.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_CONFLICT");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_1000
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_1000', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + FIFTH_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_INVALID);
            console.info('===BMS_Multiple_Hap_Install_1000.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_INVALID");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_1100
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_1100', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT, PATH + SECOND_BACKUP_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Install_1100.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            installData.uninstall(NAME, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data1) => {
                console.info('===BMS_Multiple_Hap_Install_1100.Message2===' + JSON.stringify(data1.statusMessage));
                expect(data1.statusMessage).assertEqual('SUCCESS');
                done();
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_1200
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_1200', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Install_1200.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            installData.uninstall(NAME, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data1) => {
                console.info('===BMS_Multiple_Hap_Install_1200.Message2===' + JSON.stringify(data1.statusMessage));
                expect(data1.statusMessage).assertEqual('SUCCESS');
                done();
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_1300
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_1300', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT, PATH + TWELFTH_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Install_1300.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            installData.uninstall(NAME, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data1) => {
                console.info('===BMS_Multiple_Hap_Install_1300.Message2===' + JSON.stringify(data1.statusMessage));
                expect(data1.statusMessage).assertEqual('SUCCESS');
                done();
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_1400
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_1400', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + "test_right.hap"], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_INVALID);
            console.info('===BMS_Multiple_Hap_Install_1400.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_INVALID");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Install_1500
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Install_1500', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + SIXTH_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(ERR_CODE);
            expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_CONFLICT);
            console.info('===BMS_Multiple_Hap_Install_1500.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual("STATUS_INSTALL_FAILURE_CONFLICT");
            done();
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_0100
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_0100', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_0100.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + SECOND_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_0100.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_0100.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_0200
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
    it('BMS_Multiple_Hap_Update_0200', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_0200.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            installData.install([PATH + FOURTH_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(ERR_CODE);
                expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_CONFLICT);
                console.info('===BMS_Multiple_Hap_Update_0200.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('STATUS_INSTALL_FAILURE_CONFLICT');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_0200.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        })
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_0300
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_0300', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_0300.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            installData.install([PATH + THIRTEENTH_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(ERR_CODE);
                expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_INCOMPATIBLE);
                console.info('===BMS_Multiple_Hap_Update_0300.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('STATUS_INSTALL_FAILURE_INCOMPATIBLE');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_0300.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        })
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_0400
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_0400', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_0400.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + FIRST_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_0400.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_0400.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_0500
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_0500', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_0500.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + FOURTEENTH_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_0500.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_0500.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_0600
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_0600', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_0600.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + FIRST_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_0600.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_0600.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_0700
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_0700', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_0700.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + FOURTEENTH_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_0700.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_0700.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_0800
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_0800', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_0800.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + SECOND_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_0800.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_0800.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_0900
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_0900', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_0900.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + FOURTH_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_0900.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_0900.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_1000
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_1000', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_1000.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            installData.install([PATH + THIRTEENTH_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(ERR_CODE);
                expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_INCOMPATIBLE);
                console.info('===BMS_Multiple_Hap_Update_1000.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('STATUS_INSTALL_FAILURE_INCOMPATIBLE');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_1000.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        })
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_1100
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_1100', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_1100.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + TWELFTH_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_1100.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_1100.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_1200
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_1200', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_1200.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + SIXTEENTH_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_1200.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_1200.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_1300
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_1300', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_1300.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            installData.install([PATH + THIRTEENTH_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(ERR_CODE);
                expect(data.status).assertEqual(STATUS_INSTALL_FAILURE_INCOMPATIBLE);
                console.info('===BMS_Multiple_Hap_Update_1300.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('STATUS_INSTALL_FAILURE_INCOMPATIBLE');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_1300.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        })
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_1400
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_1400', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 1,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_1400.Message1===' + JSON.stringify(data.statusMessage));
            installData.uninstall(NAME, {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data1) => {
                console.info('===BMS_Multiple_Hap_Update_1400.Message2===' + JSON.stringify(data1.statusMessage));
                expect(data1.statusMessage).assertEqual('SUCCESS');
                done();
            })
        });
    })

    /**
     * @tc.number BMS_Multiple_Hap_Update_1500
     * @tc.name BUNDLE::install
     * @tc.desc Test install interfaces.
     */
     it('BMS_Multiple_Hap_Update_1500', 0, async function (done) {
        let installData = await demo.getBundleInstaller();
        installData.install([PATH + FIRST_RIGHT, PATH + SECOND_RIGHT], {
            userId: 100,
            installFlag: 0,
            isKeepData: false
        }, async (err, data) => {
            expect(err.code).assertEqual(0);
            expect(data.status).assertEqual(0);
            console.info('===BMS_Multiple_Hap_Update_1500.Message1===' + JSON.stringify(data.statusMessage));
            expect(data.statusMessage).assertEqual('SUCCESS');
            
            installData.install([PATH + FIRST_RIGHT, PATH + SECOND_RIGHT], {
                userId: 100,
                installFlag: 1,
                isKeepData: false
            }, async (err, data) => {
                expect(err.code).assertEqual(0);
                expect(data.status).assertEqual(0);
                console.info('===BMS_Multiple_Hap_Update_1500.Message2===' + JSON.stringify(data.statusMessage));
                expect(data.statusMessage).assertEqual('SUCCESS');
                installData.uninstall(NAME, {
                    userId: 100,
                    installFlag: 1,
                    isKeepData: false
                }, async (err, data) => {
                    console.info('===BMS_Multiple_Hap_Update_1500.Message3===' + JSON.stringify(data.statusMessage));
                    expect(data.statusMessage).assertEqual('SUCCESS');
                    done();
                })
            })
        });
    })
})


