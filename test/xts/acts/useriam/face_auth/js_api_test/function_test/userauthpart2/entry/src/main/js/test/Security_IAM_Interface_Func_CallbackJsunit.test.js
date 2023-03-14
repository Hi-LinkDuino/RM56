/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import {describe, it, expect} from 'deccjsunit/index'
import userAuth from '@ohos.userAuth'
import userIDM from '@ohos.userIDM'
import pinAuth from '@ohos.pinAuth'
import * as publicFC from './Publicfunction-n.js'


describe('userauthTest', function () {
	
    /*
        * @tc.number    : Security_IAM_Face_Interface_check_IDMAuthSubType_0101
        * @tc.name      : InnerKit interface enum AuthSubType
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_IDMAuthSubType_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_IDMAuthSubType_0101 start');
        expect(10000).assertEqual(userIDM.AuthSubType.PIN_SIX);
        expect(10001).assertEqual(userIDM.AuthSubType.PIN_NUMBER);
        expect(10002).assertEqual(userIDM.AuthSubType.PIN_MIXED);
        expect(20000).assertEqual(userIDM.AuthSubType.FACE_2D);
        expect(20001).assertEqual(userIDM.AuthSubType.FACE_3D);
        console.info('testFace Security_IAM_Face_Interface_check_IDMAuthSubType_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_IDMAuthType_0101
        * @tc.name      : InnerKit interface enum AuthType
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_IDMAuthType_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_IDMAuthSubType_0101 start');
        expect(1).assertEqual(userIDM.AuthType.PIN);
        expect(2).assertEqual(userIDM.AuthType.FACE);
        console.info('testFace Security_IAM_Face_Interface_check_IDMAuthSubType_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_pinauthAuthSubType_0101
        * @tc.name      : InnerKit interface enum AuthSubType
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_pinauthAuthSubType_0101', 0, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_pinauthAuthSubType_0101 start');
        expect(10000).assertEqual(pinAuth.AuthSubType.PIN_SIX);
        expect(10001).assertEqual(pinAuth.AuthSubType.PIN_NUMBER);
        expect(10002).assertEqual(pinAuth.AuthSubType.PIN_MIXED);
        expect(20000).assertEqual(pinAuth.AuthSubType.FACE_2D);
        expect(20001).assertEqual(pinAuth.AuthSubType.FACE_3D);
        console.info('testFace Security_IAM_Face_Interface_check_pinauthAuthSubType_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthAuthSubType_0101
        * @tc.name      : InnerKit interface enum AuthSubType
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthAuthSubType_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthAuthSubType_0101 start');
        expect(10000).assertEqual(userAuth.AuthSubType.PIN_SIX);
        expect(10001).assertEqual(userAuth.AuthSubType.PIN_NUMBER);
        expect(10002).assertEqual(userAuth.AuthSubType.PIN_MIXED);
        expect(20000).assertEqual(userAuth.AuthSubType.FACE_2D);
        expect(20001).assertEqual(userAuth.AuthSubType.FACE_3D);
        console.info('testFace Security_IAM_Face_Interface_check_userauthAuthSubType_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthAuthTrustLevel_0101
        * @tc.name      : InnerKit interface enum AuthTrustLevel
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthAuthTrustLevel_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthAuthTrustLevel_0101 start');
        expect(10000).assertEqual(userAuth.AuthTrustLevel.ATL1);
        expect(20000).assertEqual(userAuth.AuthTrustLevel.ATL2);
        expect(30000).assertEqual(userAuth.AuthTrustLevel.ATL3);
        expect(40000).assertEqual(userAuth.AuthTrustLevel.ATL4);
        console.info('testFace Security_IAM_Face_Interface_check_userauthAuthTrustLevel_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthAuthType_0101
        * @tc.name      : InnerKit interface enum AuthType
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthAuthType_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthAuthType_0101 start');
        expect(1).assertEqual(userAuth.AuthType.PIN);
        expect(2).assertEqual(userAuth.AuthType.FACE);
        console.info('testFace Security_IAM_Face_Interface_check_userauthAuthType_0101 end');
        done();
    })
	
    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthGetPropertyType_0101
        * @tc.name      : InnerKit interface enum GetPropertyType
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthGetPropertyType_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthGetPropertyType_0101 start');
        expect(1).assertEqual(userAuth.GetPropertyType.AUTH_SUB_TYPE);
        expect(2).assertEqual(userAuth.GetPropertyType.REMAIN_TIMES);
        expect(3).assertEqual(userAuth.GetPropertyType.FREEZING_TIME);
        console.info('testFace Security_IAM_Face_Interface_check_userauthGetPropertyType_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthAuthMethod_0101
        * @tc.name      : InnerKit interface enum AuthMethod
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthAuthMethod_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthAuthMethod_0101 start');
        expect(0xF).assertEqual(userAuth.AuthMethod.PIN_ONLY);
        expect(0xF0).assertEqual(userAuth.AuthMethod.FACE_ONLY);
        console.info('testFace Security_IAM_Face_Interface_check_userauthAuthMethod_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthSetPropertyType_0101
        * @tc.name      : InnerKit interface enum SetPropertyType
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthSetPropertyType_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthSetPropertyType_0101 start');
        expect(1).assertEqual(userAuth.SetPropertyType.INIT_ALGORITHM);
        console.info('testFace Security_IAM_Face_Interface_check_userauthSetPropertyType_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthModule_0101
        * @tc.name      : InnerKit interface enum Module
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthModule_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthModule_0101 start');
        expect(1).assertEqual(userAuth.Module.FACE_AUTH);
        console.info('testFace Security_IAM_Face_Interface_check_userauthModule_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthResultCode_0101
        * @tc.name      : InnerKit interface enum ResultCode
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthResultCode_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthResultCode_0101 start');
        expect(0).assertEqual(userAuth.ResultCode.SUCCESS);
        expect(1).assertEqual(userAuth.ResultCode.FAIL);
        expect(2).assertEqual(userAuth.ResultCode.GENERAL_ERROR);
        expect(3).assertEqual(userAuth.ResultCode.CANCELED);
        expect(4).assertEqual(userAuth.ResultCode.TIMEOUT);
        expect(5).assertEqual(userAuth.ResultCode.TYPE_NOT_SUPPORT);
        expect(6).assertEqual(userAuth.ResultCode.TRUST_LEVEL_NOT_SUPPORT);
        expect(7).assertEqual(userAuth.ResultCode.BUSY);
        expect(8).assertEqual(userAuth.ResultCode.INVALID_PARAMETERS);
        expect(9).assertEqual(userAuth.ResultCode.LOCKED);
        expect(10).assertEqual(userAuth.ResultCode.NOT_ENROLLED);
        console.info('testFace Security_IAM_Face_Interface_check_userauthResultCode_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthFaceTipsCode_0101
        * @tc.name      : InnerKit interface enum FaceTipsCode
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthFaceTipsCode_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthFaceTipsCode_0101 start');
        expect(1).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_TOO_BRIGHT);
        expect(2).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_TOO_DARK);
        expect(3).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_TOO_CLOSE);
        expect(4).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_TOO_FAR);
        expect(5).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_TOO_HIGH);
        expect(6).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_TOO_LOW);
        expect(7).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_TOO_RIGHT);
        expect(8).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_TOO_LEFT);
        expect(9).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_TOO_MUCH_MOTION);
        expect(10).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_POOR_GAZE);
        expect(11).assertEqual(userAuth.FaceTipsCode.FACE_AUTH_TIP_NOT_DETECTED);
        console.info('testFace Security_IAM_Face_Interface_check_userauthFaceTipsCode_0101 end');
        done();
    })

    /*
        * @tc.number    : Security_IAM_Face_Interface_check_userauthFingerprintTips_0101
        * @tc.name      : InnerKit interface enum FingerprintTips
        * @tc.size      : MediumTest
        * @tc.type      : Function
        * @tc.level     : Level 1
    */
    it('Security_IAM_Face_Interface_check_userauthFingerprintTips_0101', 1, async function (done) {
        console.info('testFace Security_IAM_Face_Interface_check_userauthFingerprintTips_0101 start');
        expect(0).assertEqual(userAuth.FingerprintTips.FINGERPRINT_TIP_GOOD);
        expect(1).assertEqual(userAuth.FingerprintTips.FINGERPRINT_TIP_IMAGER_DIRTY);
        expect(2).assertEqual(userAuth.FingerprintTips.FINGERPRINT_TIP_INSUFFICIENT);
        expect(3).assertEqual(userAuth.FingerprintTips.FINGERPRINT_TIP_PARTIAL);
        expect(4).assertEqual(userAuth.FingerprintTips.FINGERPRINT_TIP_TOO_FAST);
        expect(5).assertEqual(userAuth.FingerprintTips.FINGERPRINT_TIP_TOO_SLOW);
        console.info('testFace Security_IAM_Face_Interface_check_userauthFingerprintTips_0101 end');
        done();
    })
})
