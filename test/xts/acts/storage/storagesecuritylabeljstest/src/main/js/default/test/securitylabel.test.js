/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {
    describe, it, expect, securityLabel, nextFileName, fileio
} from './Common';

describe('securitylabel', function () {

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0000
      * @tc.name securitylabel_test_set_security_label_async_000
      * @tc.desc Test that the setSecurityLabel() interface callback to set the data label. Test the getSecurityLabel() interface callback to get the data label.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_000', 0, async function (done) {
        let fpath = await nextFileName('test.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            securityLabel.setSecurityLabel(fpath, 's0', function (err) {
                securityLabel.getSecurityLabel(fpath, function (err, dataLevel) {
                    expect(dataLevel == 's0').assertTrue();
                    expect(fileio.unlinkSync(fpath) == null).assertTrue();
                    done();
                })
            })
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_000 has failed for ' + e);
            expect(null).assertFail();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0010
      * @tc.name securitylabel_test_set_security_label_async_001
      * @tc.desc Test that the setSecurityLabel() interface callback to set the data label. Test the getSecurityLabel() interface promises to get the data label.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_001', 0, async function (done) {
        let fpath = await nextFileName('test1.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            securityLabel.setSecurityLabel(fpath, 's1', async function (err) {
                let dataLevel = await securityLabel.getSecurityLabel(fpath);
                expect(dataLevel == 's1').assertTrue();
                expect(fileio.unlinkSync(fpath) == null).assertTrue();
                done();
            })
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_001 has failed for ' + e);
            expect(null).assertFail();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0020
      * @tc.name securitylabel_test_set_security_label_async_002
      * @tc.desc Test that the setSecurityLabel() interface promises to set the data label. Test the getSecurityLabel() interface callback to get the data label.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_002', 0, async function (done) {
        let fpath = await nextFileName('test2.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            await securityLabel.setSecurityLabel(fpath, 's2');
            securityLabel.getSecurityLabel(fpath, function (err, dataLevel) {
                expect(dataLevel == 's2').assertTrue();
                expect(fileio.unlinkSync(fpath) == null).assertTrue();
                done();
            })
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_002 has failed for ' + e);
            expect(null).assertFail();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0030
      * @tc.name securitylabel_test_set_security_label_async_003
      * @tc.desc Test that the setSecurityLabel() interface promises to set the data label. Test the getSecurityLabel() interface promises to get the data label.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_003', 0, async function (done) {
        let fpath = await nextFileName('test3.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            await securityLabel.setSecurityLabel(fpath, 's3');
            let dataLevel = await securityLabel.getSecurityLabel(fpath);
            expect(dataLevel == 's3').assertTrue();
            expect(fileio.unlinkSync(fpath) == null).assertTrue();
            done();
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_003 has failed for ' + e);
            expect(null).assertFail();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0040
      * @tc.name securitylabel_test_set_security_label_async_004
      * @tc.desc Test that the setSecurityLabel() interface promises to set the data label. Test the getSecurityLabel() interface promises to get the data label.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_004', 0, async function (done) {
        let fpath = await nextFileName('test4.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            await securityLabel.setSecurityLabel(fpath, 's4');
            let dataLevel = await securityLabel.getSecurityLabel(fpath);
            expect(dataLevel == 's4').assertTrue();
            expect(fileio.unlinkSync(fpath) == null).assertTrue();
            done();
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_004 has failed for ' + e);
            expect(null).assertFail();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0050
      * @tc.name securitylabel_test_set_security_label_async_005
      * @tc.desc Test setSecurityLabel() interfaces. return false when path is empty string.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_005', 0, async function (done) {
        try {
            securityLabel.setSecurityLabel('', 's0', function (err) {
                expect(err.code == '-1').assertTrue();
                done();
            });
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_005 has failed for ' + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0060
      * @tc.name securitylabel_test_set_security_label_async_006
      * @tc.desc Test setSecurityLabel() interfaces. return false when the path is invalid.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_006', 0, async function (done) {
        try {
            securityLabel.setSecurityLabel('/data/test.txt', 's0', function (err) {
                expect(err.code == '-1').assertTrue();
                done();
            });
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_006 has failed for ' + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0070
      * @tc.name securitylabel_test_set_security_label_async_007
      * @tc.desc Test setSecurityLabel() interfaces. Throw exception when path is empty.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_007', 0, async function (done) {
        try {
            await securityLabel.setSecurityLabel('s0');
            expect(null).assertFail();
            done();
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_007 has failed for ' + e);
            expect(!!e).assertTrue();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0080
      * @tc.name securitylabel_test_set_security_label_async_008
      * @tc.desc Test setSecurityLabel() interfaces. return false when dataLevel is empty string.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_008', 0, async function (done) {
        let fpath = await nextFileName('test5.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            securityLabel.setSecurityLabel(fpath, '', async function (err) {
                expect(err.code == '-1').assertTrue();
                let dataLevel = await securityLabel.getSecurityLabel(fpath);
                expect(dataLevel == '').assertTrue();
                expect(fileio.unlinkSync(fpath) == null).assertTrue();
                done();
            });
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_008 has failed for ' + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0090
      * @tc.name securitylabel_test_set_security_label_async_009
      * @tc.desc Test setSecurityLabel() interfaces. return false when the dataLevel is invalid.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_009', 0, async function (done) {
        let fpath = await nextFileName('test6.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            securityLabel.setSecurityLabel(fpath, 'ss', async function (err) {
                expect(err.code == '-1').assertTrue();
                let dataLevel = await securityLabel.getSecurityLabel(fpath);
                expect(dataLevel == '').assertTrue();
                expect(fileio.unlinkSync(fpath) == null).assertTrue();
                done();
            });
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_009 has failed for ' + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0100
      * @tc.name securitylabel_test_set_security_label_async_010
      * @tc.desc Test setSecurityLabel() interfaces. Throw exception when dataLevel is empty.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_010', 0, async function (done) {
        let fpath = await nextFileName('test7.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            await securityLabel.setSecurityLabel(fpath);
            expect(null).assertFail();
            done();
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_010 has failed for ' + e);
            expect(fileio.unlinkSync(fpath) == null).assertTrue();
            expect(!!e).assertTrue();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_SET_SECURITY_LABEL_0110
      * @tc.name securitylabel_test_set_security_label_async_011
      * @tc.desc Test setSecurityLabel() interfaces. Throws an exception when passing multiple parameters.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_set_security_label_async_011', 0, async function (done) {
        let fpath = await nextFileName('test8.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            securityLabel.setSecurityLabel(fpath, 's0', '', function (err) {
                done();
            })
        } catch (e) {
            console.log('securitylabel_test_set_security_label_async_011 has failed for ' + e);
            expect(fileio.unlinkSync(fpath) == null).assertTrue();
            expect(!!e).assertTrue();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_GET_SECURITY_LABEL_0000
      * @tc.name securitylabel_test_get_security_label_async_000
      * @tc.desc Test getSecurityLabel() interfaces. return empty when path is empty string.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_get_security_label_async_000', 0, async function (done) {
        try {
            let dataLevel = await securityLabel.getSecurityLabel('');
            expect(dataLevel == '').assertTrue();
            done();
        } catch (e) {
            console.log('securitylabel_test_get_security_label_async_000 has failed for ' + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_GET_SECURITY_LABEL_0010
      * @tc.name securitylabel_test_get_security_label_async_001
      * @tc.desc Test getSecurityLabel() interfaces. return empty when the path is invalid.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_get_security_label_async_001', 0, async function (done) {
        try {
            let dataLevel = await securityLabel.getSecurityLabel('/data/test.txt');
            expect(dataLevel == '').assertTrue();
            done();
        } catch (e) {
            console.log('securitylabel_test_get_security_label_async_001 has failed for ' + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_GET_SECURITY_LABEL_0020
      * @tc.name securitylabel_test_get_security_label_async_002
      * @tc.desc Test getSecurityLabel() interfaces. Throw exception when path is empty.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_get_security_label_async_002', 0, async function (done) {
        try {
            await securityLabel.getSecurityLabel();
            expect(null).assertFail();
            done();
        } catch (e) {
            console.log('securitylabel_test_get_security_label_async_002 has failed for ' + e);
            expect(!!e).assertTrue();
            done();
        }
    });

    /**
      * @tc.number SUB_DF_SECURITYLABEL_GET_SECURITY_LABEL_0030
      * @tc.name securitylabel_test_get_security_label_async_003
      * @tc.desc Test getSecurityLabel() interfaces. Throws an exception when passing multiple parameters.
      * @tc.size MEDIUM
      * @tc.type Function
      * @tc.level Level 0
      * @tc.require
      */
    it('securitylabel_test_get_security_label_async_003', 0, async function (done) {
        let fpath = await nextFileName('test9.txt');
        fileio.openSync(fpath, 0o102, 0o666);

        try {
            securityLabel.getSecurityLabel(fpath, '', function(err, dataLevel) {
                done();
            });
        } catch (e) {
            console.log('securitylabel_test_get_security_label_async_003 has failed for ' + e);
            expect(fileio.unlinkSync(fpath) == null).assertTrue();
            expect(!!e).assertTrue();
            done();
        }
    });
});
