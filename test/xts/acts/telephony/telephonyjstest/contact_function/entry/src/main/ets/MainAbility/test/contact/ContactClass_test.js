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

import contact from '@ohos.contact';
import utils from '../Utils.ets'
import {describe, expect, it} from 'deccjsunit/index'

export default function ContactClassTest() {
    describe('ContactClassTest', function () {
        console.log('*************ContactClassTest is start*************');

        /* *
        * @tc.number    : telephony_contact_phoneNumber_test_0100
        * @tc.name      : CUSTOM_LABEL NUM_HOME NUM_MOBILE NUM_WORK NUM_FAX_WORK
        * @tc.desc      : check class PhoneNumber
    */
        it("telephony_contact_phoneNumber_test_0100", 0, async function (done) {
            console.log("************* telephony_contact_phoneNumber_test_0100 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            try {
                expect(0).assertEqual(contact.PhoneNumber.CUSTOM_LABEL);
                expect(1).assertEqual(contact.PhoneNumber.NUM_HOME);
                expect(2).assertEqual(contact.PhoneNumber.NUM_MOBILE);
                expect(3).assertEqual(contact.PhoneNumber.NUM_WORK);
                expect(4).assertEqual(contact.PhoneNumber.NUM_FAX_WORK);
                done();
            } catch (error) {
                console.log("telephony_contact_phoneNumber_test_0100 error " + error.message);
            }
            console.log("************* telephony_contact_phoneNumber_test_0100 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_phoneNumber_test_0200
        * @tc.name      : NUM_FAX_HOME NUM_PAGER NUM_OTHER NUM_CALLBACK NUM_CAR
        * @tc.desc      : check class PhoneNumber
    */
        it("telephony_contact_phoneNumber_test_0200", 0, async function (done) {
            console.log("************* telephony_contact_phoneNumber_test_0200 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            expect(5).assertEqual(contact.PhoneNumber.NUM_FAX_HOME);
            expect(6).assertEqual(contact.PhoneNumber.NUM_PAGER);
            expect(7).assertEqual(contact.PhoneNumber.NUM_OTHER);
            expect(8).assertEqual(contact.PhoneNumber.NUM_CALLBACK);
            expect(9).assertEqual(contact.PhoneNumber.NUM_CAR);
            done();
            console.log("************* telephony_contact_phoneNumber_test_0200 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_phoneNumber_test_0300
        * @tc.name      : NUM_COMPANY_MAIN NUM_ISDN NUM_MAIN NUM_OTHER_FAX NUM_RADIO
        * @tc.desc      : check class PhoneNumber
    */
        it("telephony_contact_phoneNumber_test_0300", 0, async function (done) {
            console.log("************* telephony_contact_phoneNumber_test_0300 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            expect(10).assertEqual(contact.PhoneNumber.NUM_COMPANY_MAIN);
            expect(11).assertEqual(contact.PhoneNumber.NUM_ISDN);
            expect(12).assertEqual(contact.PhoneNumber.NUM_MAIN);
            expect(13).assertEqual(contact.PhoneNumber.NUM_OTHER_FAX);
            expect(14).assertEqual(contact.PhoneNumber.NUM_RADIO);
            done();
            console.log("************* telephony_contact_phoneNumber_test_0300 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_phoneNumber_test_0400
        * @tc.name      : NUM_TELEX NUM_TTY_TDD NUM_WORK_MOBILE NUM_WORK_PAGER NUM_ASSISTANT NUM_MMS INVALID_LABEL_ID labelName
        * @tc.desc      : check class PhoneNumber
    */
        it("telephony_contact_phoneNumber_test_0400", 0, async function (done) {
            console.log("************* telephony_contact_phoneNumber_test_0400 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            expect(15).assertEqual(contact.PhoneNumber.NUM_TELEX);
            expect(16).assertEqual(contact.PhoneNumber.NUM_TTY_TDD);
            expect(17).assertEqual(contact.PhoneNumber.NUM_WORK_MOBILE);
            expect(18).assertEqual(contact.PhoneNumber.NUM_WORK_PAGER);
            expect(19).assertEqual(contact.PhoneNumber.NUM_ASSISTANT);
            expect(20).assertEqual(contact.PhoneNumber.NUM_MMS);
            expect(-1).assertEqual(contact.PhoneNumber.INVALID_LABEL_ID);
            expect(typeof contact.PhoneNumber.labelName).assertEqual("string");
            done();
            console.log("************* telephony_contact_phoneNumber_test_0400 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_postalAddress_test_0100
        * @tc.name      : CUSTOM_LABEL ADDR_HOME ADDR_WORK ADDR_OTHER INVALID_LABEL_ID
        * @tc.desc      : check class PostalAddress
    */
        it("telephony_contact_postalAddress_test_0100", 0, async function (done) {
            console.log("************* telephony_contact_postalAddress_test_0100 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            expect(0).assertEqual(contact.PostalAddress.CUSTOM_LABEL);
            expect(1).assertEqual(contact.PostalAddress.ADDR_HOME);
            expect(2).assertEqual(contact.PostalAddress.ADDR_WORK);
            expect(3).assertEqual(contact.PostalAddress.ADDR_OTHER);
            expect(-1).assertEqual(contact.PostalAddress.INVALID_LABEL_ID);
            done();
            console.log("************* telephony_contact_postalAddress_test_0100 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_postalAddress_test_0200
        * @tc.name      : labelName neighborhood pobox postalAddress postcode street
        * @tc.desc      : check class PostalAddress
    */
        it("telephony_contact_postalAddress_test_0200", 0, async function (done) {
            console.log("************* telephony_contact_postalAddress_test_0200 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            try {
                var postalAddress = new contact.PostalAddress({
                    'labelName': 'testLabel',
                    'neighborhood': 'beijing',
                    'pobox': 'testBox',
                    'postalAddress': 'testAddress',
                    'postcode': 'testCode',
                    'street': 'testStreet'
                })
                expect(postalAddress.labelName).assertEqual('testLabel');
                expect(postalAddress.neighborhood).assertEqual('beijing');
                expect(postalAddress.pobox).assertEqual('testBox');
                expect(postalAddress.postalAddress).assertEqual('testAddress');
                expect(postalAddress.postcode).assertEqual('testCode');
                expect(postalAddress.street).assertEqual('testStreet');
            } catch (err) {
                console.log("telephony_contact_postalAddress_test_0200 test error " + err.message);
            }
            done();
            console.log("************* telephony_contact_postalAddress_test_0200 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_relation_test_0100
        * @tc.name      : CUSTOM_LABEL ADDR_HOME ADDR_WORK ADDR_OTHER INVALID_LABEL_ID
        * @tc.desc      : check class Relation
    */
        it("telephony_contact_relation_test_0100", 0, async function (done) {
            console.log("************* telephony_contact_relation_test_0100 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            expect(0).assertEqual(contact.Relation.CUSTOM_LABEL);
            expect(1).assertEqual(contact.Relation.RELATION_ASSISTANT);
            expect(2).assertEqual(contact.Relation.RELATION_BROTHER);
            expect(3).assertEqual(contact.Relation.RELATION_CHILD);
            expect(4).assertEqual(contact.Relation.RELATION_DOMESTIC_PARTNER);
            done();
            console.log("************* telephony_contact_relation_test_0100 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_relation_test_0200
        * @tc.name      : RELATION_FATHER RELATION_FRIEND RELATION_MANAGER RELATION_MOTHER RELATION_PARENT
        * @tc.desc      : check class Relation
    */
        it("telephony_contact_relation_test_0200", 0, async function (done) {
            console.log("************* telephony_contact_relation_test_0200 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            expect(5).assertEqual(contact.Relation.RELATION_FATHER);
            expect(6).assertEqual(contact.Relation.RELATION_FRIEND);
            expect(7).assertEqual(contact.Relation.RELATION_MANAGER);
            expect(8).assertEqual(contact.Relation.RELATION_MOTHER);
            expect(9).assertEqual(contact.Relation.RELATION_PARENT);
            done();
            console.log("************* telephony_contact_relation_test_0200 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_Relation_test_0300
        * @tc.name      : RELATION_PARTNER RELATION_REFERRED_BY RELATION_RELATIVE
                          RELATION_SISTER RELATION_SPOUSE INVALID_LABEL_ID
        * @tc.desc      : check class Relation
    */
        it("telephony_contact_relation_test_0300", 0, async function (done) {
            console.log("************* telephony_contact_relation_test_0300 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            expect(10).assertEqual(contact.Relation.RELATION_PARTNER);
            expect(11).assertEqual(contact.Relation.RELATION_REFERRED_BY);
            expect(12).assertEqual(contact.Relation.RELATION_RELATIVE);
            expect(13).assertEqual(contact.Relation.RELATION_SISTER);
            expect(14).assertEqual(contact.Relation.RELATION_SPOUSE);
            expect(-1).assertEqual(contact.Relation.INVALID_LABEL_ID);
            done();
            console.log("************* telephony_contact_relation_test_0300 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_relation_test_0400
        * @tc.name      : labelName relationName
        * @tc.desc      : check class Relation
    */
        it("telephony_contact_relation_test_0400", 0, async function (done) {
            console.log("************* telephony_contact_relation_test_0400 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            try {
                var relation = new contact.Relation({
                    'labelName': 'testLabel',
                    'relationName': 'testRelation'
                })
                expect(relation.labelName).assertEqual('testLabel');
                expect(relation.relationName).assertEqual('testRelation');
            } catch (err) {
                console.log("telephony_contact_relation_test_0400 test error " + err.message);
            }
            done();
            console.log("************* telephony_contact_relation_test_0400 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_sipAddress_test_0100
        * @tc.name      : CUSTOM_LABEL SIP_HOME SIP_WORK SIP_OTHER INVALID_LABEL_ID
        * @tc.desc      : check class SipAddress
    */
        it("telephony_contact_sipAddress_test_0100", 0, async function (done) {
            console.log("************* telephony_contact_sipAddress_test_0100 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }
            expect(0).assertEqual(contact.SipAddress.CUSTOM_LABEL);
            expect(1).assertEqual(contact.SipAddress.SIP_HOME);
            expect(2).assertEqual(contact.SipAddress.SIP_WORK);
            expect(3).assertEqual(contact.SipAddress.SIP_OTHER);
            expect(-1).assertEqual(contact.SipAddress.INVALID_LABEL_ID);
            done();
            console.log("************* telephony_contact_sipAddress_test_0100 Test end*************");
        });

        /* *
        * @tc.number    : telephony_contact_sipAddress_test_0200
        * @tc.name      : labelName sipAddress
        * @tc.desc      : check class SipAddress
    */
        it("telephony_contact_sipAddress_test_0200", 0, async function (done) {
            console.log("************* telephony_contact_sipAddress_test_0200 Test start*************");
            if (utils.notCheck) {
                expect(true).assertTrue();
                done();
                return;
            }

            try {
                var sip = new contact.SipAddress({
                    'labelName': 'testLabel',
                    'sipAddress': 'testSip'
                })
                expect(sip.labelName).assertEqual('testLabel');
                expect(sip.sipAddress).assertEqual('testSip');
            } catch (err) {
                console.log("telephony_contact_sipAddress_test_0200 test error " + err.message);
            }
            done();
            console.log("************* telephony_contact_sipAddress_test_0200 Test end*************");
        });
        console.log('*************ContactClassTest is end*************');
    })
}