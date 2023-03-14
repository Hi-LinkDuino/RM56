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

import featureAbility from '@ohos.ability.featureAbility';
import ohosDataAbility from '@ohos.data.dataAbility';
import utils from '../Utils.ets'
import {afterAll, afterEach, beforeAll, beforeEach, describe, expect, it} from 'deccjsunit/index'

const URI_CALLLOG = "dataability:///com.ohos.calllogability";
const calllogUri = "dataability:///com.ohos.calllogability/calls/calllog";

const URI_VOICEMAIL = "dataability:///com.ohos.voicemailability";
const voicemailUri = "dataability:///com.ohos.voicemailability/calls/voicemail";

const URI_CONTACTS = "dataability:///com.ohos.contactsdataability";
const rawContactUri = "dataability:///com.ohos.contactsdataability/contacts/raw_contact";
const contactDataUri = "dataability:///com.ohos.contactsdataability/contacts/contact_data";
const deletedUri = "dataability:///com.ohos.contactsdataability/contacts/deleted_raw_contact";

export default function StabilityTest() {
    describe('StabilityTest', function () {
        console.log(' StabilityTest is start');

        function sleep(numberMillis) {
            var now = new Date();
            var exitTime = now.getTime() + numberMillis;
            while (true) {
                now = new Date();
                if (now.getTime() > exitTime)
                return;
            }
        }

        /**
     * @tc.number  raw_contact_insert_stability_test_900
     * @tc.name    Add 10000 pieces of data to the raw_contact table to see if they can be successfully inserted
     * @tc.desc    Function test
     */
        it("raw_contact_insert_stability_test_900", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var listAddBluk = [];
            for (var i = 0; i < 1000; i++) {
                var add = {
                    "display_name": "xiaoli" + i, "company": "testCompany" + i, "position": "testPosition" + i
                };
                listAddBluk[i] = add;
            }
            try {
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(rawContactUri, listAddBluk);
                sleep(1000);
                console.info("logMessage raw_contact_insert_stability_test_900: contactDataId1 = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info('logMessage raw_contact_insert_stability_test_900:  insert error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  raw_contact_update_stability_test_1000
     * @tc.name    The raw_contact table updates the data whose ID is not equal to 0 to see whether they can be updated
     * successfully
     * @tc.desc    Function test
     */
        it("raw_contact_update_stability_test_1000", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var updateValues = {
                "favorite": 1
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            condition.and();
            condition.equalTo("is_deleted", "0");
            try {
                var updateCode = await DAHelper.update(rawContactUri, updateValues, condition);
                sleep(1000);
                console.info("logMessage raw_contact_update_stability_test_1000: updateCode = " + updateCode);
                expect(updateCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage raw_contact_update_stability_test_1000: update error = " + error);
                done();
            }
        });

        /**
     * @tc.number  raw_contact_query_stability_test_1100
     * @tc.name    The raw_contact table queries 10000 pieces of data to see whether they can be queried successfully
     * @tc.desc    Function test
     */
        it("raw_contact_query_stability_test_1100", 0, async function (done) {
            var tag = "raw_contact_query_stability_test_1100";
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': start ! DAHelper = ' + DAHelper);
            var resultColumns = ["id"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            condition.and();
            condition.equalTo("is_deleted", "0");
            try {
                var resultSet = await DAHelper.query(rawContactUri, resultColumns, condition);
                sleep(1000);
                console.info(tag + ' : logMessage : rowCount' + resultSet.rowCount);
                expect(resultSet.rowCount == 10000).assertTrue();
                resultSet.close();
                done();
            } catch (error) {
                console.info(tag + " :logMessage : error = " + error);
                done();
            }
        });

        /**
     * @tc.number  raw_contact_delete_stability_test_1200
     * @tc.name    Delete 10000 pieces of data in raw_contact table to see if they can be deleted successfully
     * @tc.desc    Function test
     */
        it("raw_contact_delete_stability_test_1200", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('raw_contact_delete_stability_test_1200 : start ! DAHelper = ' + DAHelper);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            condition.and();
            condition.equalTo("is_deleted", "0");
            try {
                var deleteCode = await DAHelper.delete(rawContactUri, condition);
                sleep(1000);
                console.info("logMessage raw_contact_delete_stability_test_1200: deleteCode = " + deleteCode);
                expect(deleteCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info('logMessage raw_contact_delete_stability_test_1200: delete error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  contact_data_insert_stability_test_1300
     * @tc.name    Add 10000 pieces of data to the contact_data table to see if they can be successfully inserted
     * @tc.desc    Function test
     */
        it("contact_data_insert_stability_test_1300", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoli",
            };
            try {
                var rawContactId = await DAHelper.insert(rawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contact_data_insert_stability_test_1300: rawContactId = " + rawContactId);
            } catch (error) {
                console.info("logMessage contact_data_insert_stability_test_1300: raw_contact insert error = " + error);
            }

            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactId.toString());
            var deleteCode = await DAHelper.delete(rawContactUri, condition);
            sleep(1000);
            console.info("logMessage contact_data_insert_stability_test_1300: deleteCode = " + deleteCode);

            var listAddBluk = [];
            for (var i = 0; i < 1000; i++) {
                var add = {
                    "raw_contact_id": rawContactId, "detail_info": "xxx" + i, "content_type": "name"
                };
                listAddBluk[i] = add;
            }
            try {
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                sleep(2000);
                console.info("logMessage contact_data_insert_stability_test_1300: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info('logMessage contact_data_insert_stability_test_1300:  insert error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  contact_data_update_stability_test_1400
     * @tc.name    The contact_data table updates the data whose ID is not equal to 0 to see whether they can be updated
     * successfully
     * @tc.desc    Function test
     */
        it("contact_data_update_stability_test_1400", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage contact_data_update_stability_test_1400 DAHelper success! DAHelper = ' + DAHelper);
            var updateValues = {
                "syn_1": "test"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            try {
                var updateCode = await DAHelper.update(contactDataUri, updateValues, condition);
                sleep(2000);
                console.info("logMessage contact_data_update_stability_test_1400: updateCode = " + updateCode);
                expect(updateCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage contact_data_update_stability_test_1400: update error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contact_data_query_stability_test_1500
     * @tc.name    The contact_data table queries 10000 pieces of data to see whether they can be queried successfully
     * @tc.desc    Function test
     */
        it("contact_data_query_stability_test_1500", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('contact_data_query_stability_test_1500 start ! DAHelper = ' + DAHelper);
            var resultColumns = ["id"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            try {
                var resultSet = await DAHelper.query(contactDataUri, resultColumns, condition);
                sleep(2000);
                console.info(' contact_data_query_stability_test_1500 :  resultSet.rowCount = ' + resultSet.rowCount);
                expect(resultSet.rowCount == 10000).assertEqual(true);
                resultSet.close();
                done();
            } catch (error) {
                console.info("logMessage contact_data_query_stability_test_1500: error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contact_data_delete_stability_test_1600
     * @tc.name    Delete 10000 pieces of data in contact_data table to see if they can be deleted successfully
     * @tc.desc    Function test
     */
        it("contact_data_delete_stability_test_1600", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('contact_data_delete_stability_test_1600 : start ! DAHelper = ' + DAHelper);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            try {
                var deleteCode = await DAHelper.delete(contactDataUri, condition);
                sleep(2000);
                console.info("logMessage contact_data_delete_stability_test_1600: deleteCode = " + deleteCode);
                expect(deleteCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info('logMessage contact_data_delete_stability_test_1600: delete error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  calllog_insert_stability_test_100
     * @tc.name    Add 10000 pieces of data to the callog table to see if they can be successfully inserted
     * @tc.desc    Function test
     */
        it("calllog_insert_stability_test_100", 0, async function (done) {
            console.info("---------logMessage calllog_insert_stability_test_100 is starting!----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var listAddBluk = [];
            for (var i = 0; i < 1000; i++) {
                var add = {
                    "phone_number": "1511002" + i
                };
                listAddBluk[i] = add;
            }
            try {
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                sleep(1000);
                console.info("logMessage calllog_insert_stability_test_100: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage calllog_insert_stability_test_100: batchInsert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  calllog_update_stability_test_200
     * @tc.name    The callog table updates the data whose ID is not equal to 0 to see whether they can be updated
     * successfully
     * @tc.desc    Function test
     */
        it("calllog_update_stability_test_200", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var updateValues = {
                "answer_state": "1"
            };
            try {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.greaterThan("id", "0");
                var updateCode = await DAHelper.update(calllogUri, updateValues, condition);
                sleep(1000);
                console.info("logMessage calllog_update_stability_test_200: updateCode = " + updateCode);
                expect(updateCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage calllog_update_stability_test_200: update error = " + error);
                done();
            }
            ;
        });

        /**
     * @tc.number  calllog_query_stability_test_300
     * @tc.name    The callog table queries 10000 pieces of data to see whether they can be queried successfully
     * @tc.desc    Function test
     */
        it("calllog_query_stability_test_300", 0, async function (done) {
            var tag = "calllog_query_stability_test_300";
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info(tag + ': start ! DAHelper = ' + DAHelper);
            var resultColumns = ["id"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.notEqualTo("id", 0);

            try {
                var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                sleep(5000);
                console.info(tag + ' : logMessage : rowCount' + resultSet.rowCount);
                expect(resultSet.rowCount == 10000).assertTrue();
                resultSet.close();
                done();
            } catch (error) {
                console.info("logMessage calllog_query_stability_test_300: error = " + error);
                done()
            }
        });

        /**
     * @tc.number  calllog_delete_stability_test_400
     * @tc.name    Delete 10000 pieces of data in callog table to see if they can be deleted successfully
     * @tc.desc    Function test
     */
        it("calllog_delete_stability_test_400", 0, async function (done) {
            var tag = "calllog_delete_stability_test_400";
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info(tag + ': start ! DAHelper = ' + DAHelper);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            var deleteCode = await DAHelper.delete(calllogUri, condition);
            sleep(5000);
            console.info(tag + " : logMessage : deleteCode = " + deleteCode);
            expect(deleteCode == 0).assertTrue();
            done();
        });

        /**
     * @tc.number  voicemail_insert_stability_test_500
     * @tc.name    Add 10000 pieces of data to the voicemail table to see if they can be successfully inserted
     * @tc.desc    Function test
     */
        it("voicemail_insert_stability_test_500", 0, async function (done) {
            console.info("---------logMessage voicemail_insert_stability_test_500 is starting!----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var listAddBluk = [];
            for (var i = 0; i < 1000; i++) {
                var add = {
                    "phone_number": "12345" + i
                };
                listAddBluk[i] = add;
            }
            try {
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                sleep(1000);
                console.info("logMessage voicemail_insert_stability_test_500: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage voicemail_insert_stability_test_500: batchInsert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_update_stability_test_600
     * @tc.name    The voicemail table updates the data whose ID is not equal to 0 to see whether they can be updated
     * successfully
     * @tc.desc    Function test
     */
        it("voicemail_update_stability_test_600", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var updateValues = {
                "origin_type": "test"
            };
            try {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.notEqualTo("id", 0);
                var updateCode = await DAHelper.update(voicemailUri, updateValues, condition);
                sleep(1000);
                console.info("logMessage voicemail_update_stability_test_600: updateCode = " + updateCode);
                expect(updateCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage voicemail_update_stability_test_600: update error = " + error);
                done();
            }
            ;
        });

        /**
     * @tc.number  voicemail_query_stability_test_700
     * @tc.name    The voicemail table queries 10000 pieces of data to see whether they can be queried successfully
     * @tc.desc    Function test
     */
        it("voicemail_query_stability_test_700", 0, async function (done) {
            if(utils.notCheck){
                expect(true).assertTrue();
                done();
                return;
            }
            var tag = "voicemail_query_stability_test_700";
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info(tag + ': start ! DAHelper = ' + DAHelper);
            var resultColumns = ["id"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            try {
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                sleep(1000);
                console.info(tag + ' : resultSet.rowCount = ' + resultSet.rowCount);
                expect(resultSet.rowCount == 10000).assertEqual(true);
                resultSet.close();
                done();
            } catch (error) {
                console.info("logMessage voicemail_query_stability_test_700: error = " + error);
                done()
            }
        });

        /**
     * @tc.number  voicemail_delete_stability_test_800
     * @tc.name    Delete 10000 pieces of data in voicemail table to see if they can be deleted successfully
     * @tc.desc    Function test
     */
        it("voicemail_delete_stability_test_800", 0, async function (done) {
            var tag = "voicemail_delete_stability_test_800";
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info(tag + ': start ! DAHelper = ' + DAHelper);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            try {
                var deleteCode = await DAHelper.delete(voicemailUri, condition);
                sleep(2000);
                console.info(tag + " : logMessage : deleteCode = " + deleteCode);
                expect(deleteCode == 0).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage voicemail_delete_stability_test_800: error = " + error);
                done();
            }
        });

        afterAll(async function () {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('Stability : start ! DAHelper = ' + DAHelper);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.notEqualTo("id", "0");
            try {
                var resultColumns = ["id"];
                var conditionDelete = new ohosDataAbility.DataAbilityPredicates();
                conditionDelete.greaterThan("id", "0");
                var count = 0;
                var deleteCount = 9999;
                while (count < deleteCount) {
                    var result = await DAHelper.query(deletedUri, resultColumns, conditionDelete);
                    console.info("Stability : result.rowCount = " + result.rowCount);
                    count = result.rowCount;
                    result.close();
                    sleep(5000);
                }
                var deleteCode = await DAHelper.delete(deletedUri, condition);
                console.info("Stability afterAll logMessage DeleteContact: deleteCode = " + deleteCode);
            } catch (error) {
                console.info('Stability afterAll logMessage DeleteContact: delete error = ' + error);
            }
        });
    })
}