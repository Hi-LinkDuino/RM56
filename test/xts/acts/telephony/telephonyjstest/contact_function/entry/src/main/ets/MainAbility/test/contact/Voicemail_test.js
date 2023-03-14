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
import {afterAll, afterEach, beforeAll, beforeEach, describe, expect, it} from 'deccjsunit/index'
import common from './common.js';

const URI_VOICEMAIL = "dataability:///com.ohos.voicemailability";
const voicemailUri = "dataability:///com.ohos.voicemailability/calls/voicemail";

export default function VoicemailTest() {
    describe('VoicemailTest', function () {
        console.log("----------VoicemailTest is starting!----------");

        async function voiceMailQuery(map, tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info(tag + ':calllogInsertQuery start ! DAHelper = ' + DAHelper);
            var resultColumns = [];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", map.get("id"));
            try {
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                if (resultSet.rowCount > 0 && resultSet.goToFirstRow()) {
                    do {
                        for (var [key, value] of map) {
                            var dbresult = resultSet.getString(resultSet.getColumnIndex(key));
                            console.info(tag + ' :logMessage voiceMailQuery key = ' + key + ' dbresult :' + dbresult +
                            ' value : ' + value);
                            expect(value == dbresult).assertTrue();
                        }
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info(tag + ":logMessage voiceMailQuery: error = " + error);
            }
        }

        async function voiceMailQueryForDelete(map, tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info(tag + ': voiceMailQueryForDelete start ! DAHelper = ' + DAHelper);
            var resultColumns = common.getCallLogResultColumns();
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", map.get("id"));
            try {
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                expect(resultSet.goToFirstRow() == false).assertTrue();
                console.info(tag + " :logMessage voiceMailQueryForDelete: goToFirstRow " + resultSet.goToFirstRow());
                resultSet.close();
            } catch (error) {
                console.info(tag + " :logMessage voiceMailQueryForDelete: error = " + error);
            }
        }

        async function voiceMailQueryError(map, tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info(tag + ': voiceMailQueryError start ! DAHelper = ' + DAHelper);
            var resultColumns = common.getCallLogResultColumns();
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("phone_number", map.get("phone_number"));
            try {
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                expect(resultSet.goToFirstRow() == false).assertTrue();
                console.info(tag + " :logMessage voiceMailQueryError: goToFirstRow " + resultSet.goToFirstRow());
                resultSet.close();
            } catch (error) {
                console.info(tag + " :logMessage voiceMailQueryError: error = " + error);
            }
        }

        async function voiceMailQueryForBatchInsert(array, tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info(tag + ': voiceMailQueryForBatchInsert start ! DAHelper = ' + DAHelper);
            var resultColumns = common.getCallLogResultColumns();
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("phone_number", array[0].get("phone_number"));
            try {
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                var size = array.size();
                console.info(tag + ' : logMessage voiceMailQueryForBatchInsert: size' + size);
                expect(resultSet.rowCount == size).assertEqual(true);
                var i = 0;
                if (resultSet.rowCount > 0 && resultSet.goToFirstRow()) {
                    do {
                        for (var [key, value] of array[i]) {
                            var dbresult = resultSet.getString(resultSet.getColumnIndex(key));
                            console.info(tag + ' : logMessage voiceMailQueryForBatchInsert dbresult :' + dbresult +
                            ' value : ' + value);
                            console.info(
                                tag + ' : logMessage voiceMailQueryForBatchInsert value ' + (value == dbresult));
                            expect(value == dbresult).assertTrue();
                        }
                        i++;
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info(tag + " :logMessage calllogQueryByInsert: error = " + error);
            }
        }

        async function voiceMailDelete(tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            var deleteCode = await DAHelper.delete(voicemailUri, condition);
            console.info(tag + ': voiceMailDelete deleteCode = ' + deleteCode);
            expect(deleteCode == 0).assertTrue();
        }

        /**
     * @tc.number  voicemail_insert_test_100
     * @tc.name    Insert voicemail data
     * @tc.desc    Function test
     */
        it("voicemail_insert_test_100", 0, async function (done) {
            console.info("---------logMessage voicemail_insert_test_100 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, common.getVoiceMail());
                console.info("logMessage voicemail_insert_test_100: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                var map = common.getVoiceMailMap();
                map.set("id", voicemailId.toString());
                await voiceMailQuery(map, "voicemail_insert_test_100");
                await voiceMailDelete("voicemail_insert_test_100");
                done();
            } catch (error) {
                console.info("logMessage voicemail_insert_test_100: voicemail insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_insert_test_200
     * @tc.name    Insert voicemail status
     * @tc.desc    Function test
     */
        it("voicemail_insert_test_200", 0, async function (done) {
            console.info("---------logMessage voicemail_insert_test_200 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)

            var stringValue = {
                "phone_number": "1557254", "voice_status": 1
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, stringValue)
                console.info("logMessage voicemail_insert_test_200: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                var map = new Map();
                map.set("id", voicemailId.toString());
                map.set("phone_number", "1557254");
                map.set("voice_status", "1");
                await voiceMailQuery(map, "voicemail_insert_test_200");
                await voiceMailDelete("voicemail_insert_test_200");
                done();
            } catch (error) {
                console.info("logMessage voicemail_insert_test_200: voicemail insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_insert_test_100000
     * @tc.name    Insert voicemail status
     * @tc.desc    Function test
     */
        it("voicemail_insert_test_100000", 0, async function (done) {
            console.info("---------logMessage voicemail_insert_test_100000 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)

            var stringValue = {
                "phone_number": "1557254", "voice_status": 0
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, stringValue)
                console.info("logMessage voicemail_insert_test_100000: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                var map = new Map();
                map.set("id", voicemailId.toString());
                map.set("phone_number", "1557254");
                map.set("voice_status", "0");
                await voiceMailQuery(map, "voicemail_insert_test_100000");
                await voiceMailDelete("voicemail_insert_test_100000");
                done();
            } catch (error) {
                console.info("logMessage voicemail_insert_test_100000: voicemail insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_update_test_1000
     * @tc.name    Update voicemail data
     * @tc.desc    Function test
     */
        it("voicemail_update_test_1000", 0, async function (done) {
            console.info("------------logMessage voicemail_update_test_1000 is starting!----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            try {
                var voicemailId = await DAHelper.insert(voicemailUri, common.getVoiceMail());
                console.info("logMessage voicemail_update_test_1000: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                await voicemailUpdate();
                await voiceMailDelete("voicemail_update_test_1000");
                done();
            } catch (error) {
                console.info("logMessage voicemail_update_test_1000: voicemail insert error = " + error);
                done();
            }

            async function voicemailUpdate() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                try {
                    var updateCode = await DAHelper.update(voicemailUri, common.getVoiceMailUpdate(), condition);
                    console.info("logMessage voicemail_update_test_1000: updateCode = " + updateCode);
                    expect(updateCode == 0).assertTrue();
                    var map = common.getVoiceMailUpdateMap();
                    map.set("id", voicemailId.toString());
                    await voiceMailQuery(map, "voicemail_update_test_1000");
                } catch (error) {
                    console.info("logMessage voicemail_update_test_1000: update error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  voicemail_update_test_900
     * @tc.name    Update voicemail status
     * @tc.desc    Function test
     */
        it("voicemail_update_test_900", 0, async function (done) {
            console.info("------------logMessage voicemail_update_test_900 is starting!----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "18862013", "voice_status": 0
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage voicemail_update_test_900: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                await voicemailUpdate();
                await voiceMailDelete("voicemail_update_test_900");
                done();
            } catch (error) {
                console.info("logMessage voicemail_update_test_900: voicemail insert error = " + error);
                done();
            }

            async function voicemailUpdate() {
                var updateValues = {
                    "voice_status": 1
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                try {
                    var updateCode = await DAHelper.update(voicemailUri, updateValues, condition);
                    console.info("logMessage voicemail_update_test_900: updateCode = " + updateCode);
                    expect(updateCode == 0).assertTrue();
                    var map = new Map();
                    map.set("id", voicemailId.toString());
                    map.set("voice_status", "1");
                    await voiceMailQuery(map, "voicemail_update_test_900");
                } catch (error) {
                    console.info("logMessage voicemail_update_test_900: update error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  voicemail_delete_test_500
     * @tc.name    Delete voicemail data
     * @tc.desc    Function test
     */
        it("voicemail_delete_test_500", 0, async function (done) {
            console.info("-------------logMessage voicemail_delete_test_500 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "100578615", "display_name": "zhangming"
            };
            try {
                var voiceMailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage voicemail_delete_test_500: voiceMailId = " + voiceMailId);
                expect(voiceMailId > 0).assertTrue();
                await VoicemailDelete();
                await voiceMailDelete("voicemail_delete_test_500");
                done();
            } catch (error) {
                console.info("logMessage voicemail_delete_test_500: voicemail insert error = " + error);
                done();
            }

            async function VoicemailDelete() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                try {
                    var deleteCode = await DAHelper.delete(voicemailUri, condition);
                    console.info('logMessage voicemail_delete_test_500: deleteCode = ' + deleteCode);
                    expect(deleteCode == 0).assertTrue();
                    var map = new Map();
                    map.set("id", voicemailId.toString());
                    await voiceMailQueryForDelete(map, "voicemail_delete_test_500");
                } catch (error) {
                    console.info("logMessage voicemail_delete_test_500: delete error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  voicemail_update_test_600
     * @tc.name    Delete voicemail status
     * @tc.desc    Function test
     */
        it("voicemail_update_test_600", 0, async function (done) {
            console.info("-------------logMessage voicemail_update_test_600 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues1 = {
                "phone_number": "100578615", "voice_status": 1
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues1);
                console.info("logMessage voicemail_update_test_600: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                await voicemailUpdate();
                await voiceMailDelete("voicemail_update_test_600");
                done();
            } catch (error) {
                console.info("logMessage voicemail_update_test_600: voicemail insert error = " + error);
                done();
            }

            async function voicemailUpdate() {
                var updateValues = {
                    "voice_status": 0
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                try {
                    var updateCode = await DAHelper.update(voicemailUri, updateValues, condition);
                    console.info('logMessage voicemail_update_test_600: deleteCode = ' + updateCode);
                    expect(updateCode == 0).assertTrue();
                    var map = new Map();
                    map.set("id", voicemailId.toString());
                    map.set("voice_status", "0");
                    await voiceMailQuery(map, "voicemail_update_test_600");
                } catch (error) {
                    console.info("logMessage voicemail_update_test_600: update error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  voicemail_batchInsert_test_1800
     * @tc.name    Insert voicemail data in batches
     * @tc.desc    Function test
     */
        it("voicemail_batchInsert_test_1800", 0, async function (done) {
            console.info("-------------logMessage voicemail_batchInsert_test_1800 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValuesOne = {
                "phone_number": "10086", "display_name": "xiaoming"
            };
            var insertValuesTwo = {
                "phone_number": "12580", "display_name": "xiaohei"
            };
            var insertValuesThree = {
                "phone_number": "123456", "display_name": "xiaohong"
            };
            var insertValuesFour = {
                "phone_number": "9996325", "display_name": "xiaohei"
            };
            var insertValuesFive = {
                "phone_number": "18862013", "display_name": "xiaofen"
            };
            var listAddBluk = [];
            listAddBluk[0] = insertValuesOne;
            listAddBluk[1] = insertValuesTwo;
            listAddBluk[2] = insertValuesThree;
            listAddBluk[3] = insertValuesFour;
            listAddBluk[4] = insertValuesFive;
            try {
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                console.info("logMessage voicemail_batchInsert_test_1800: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                await voiceMailDelete("voicemail_batchInsert_test_1800");
                done();
            } catch (error) {
                console.info("logMessage voicemail_batchInsert_test_1800: batchInsert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_batchInsert_test_1700
     * @tc.name    Insert voicemail status in batches
     * @tc.desc    Function test
     */
        it("voicemail_batchInsert_test_1700", 0, async function (done) {
            console.info("-------------logMessage voicemail_batchInsert_test_1700 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValuesOne = {
                "phone_number": "1600", "display_name": "xiaoming", "voice_status": 0
            };
            var insertValuesTwo = {
                "phone_number": "1600", "display_name": "xiaohei", "voice_status": 1
            };
            var insertValuesThree = {
                "phone_number": "1600", "display_name": "xiaohong", "voice_status": 0
            };
            var insertValuesFour = {
                "phone_number": "1600", "display_name": "xiaohei", "voice_status": 0
            };
            var insertValuesFive = {
                "phone_number": "1600", "display_name": "xiaofen", "voice_status": 1
            };
            var listAddBluk = [];
            listAddBluk[0] = insertValuesOne;
            listAddBluk[1] = insertValuesTwo;
            listAddBluk[2] = insertValuesThree;
            listAddBluk[3] = insertValuesFour;
            listAddBluk[4] = insertValuesFive;
            try {
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                console.info("logMessage voicemail_batchInsert_test_1700: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                var array = common.getvoiceMailBatchInsert();
                await voiceMailQueryForBatchInsert(array, "voicemail_batchInsert_test_1700");
                await voiceMailDelete("voicemail_batchInsert_test_1700");
                done();
            } catch (error) {
                console.info("logMessage voicemail_batchInsert_test_1700: batchInsert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_query_test_1400
     * @tc.name    Query voicemail data
     * @tc.desc    Function test
     */
        it("voicemail_query_test_1400", 0, async function (done) {
            console.info("------------voicemail_query_test_1400 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "8855220011", "display_name": "daming"
            };
            try {
                var voiceMailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage voicemail_query_test_1400: voiceMailId = " + voiceMailId);
                expect(voiceMailId > 0).assertTrue();
                await voiceMailQuery();
                await voiceMailDelete("voicemail_query_test_1400");
                done();
            } catch (error) {
                console.info("logMessage voicemail_query_test_1400: voicemial insert error = " + error);
                done();
            }
            async function voiceMailQuery() {
                var resultColumns = ["id", "display_name", "phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                try {
                    var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info('voicemail_query_test_1400 id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == voicemailId.toString()).assertTrue();
                            console.info('voicemail_query_test_1400 displayName = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "daming").assertTrue();
                            console.info('voicemail_query_test_1400 phone_number = ' + resultSet.getString(2));
                            expect(resultSet.getString(2) == "8855220011").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info("voicemail_query_test_1400 query error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  voicemail_query_test_1300
     * @tc.name    Query voicemail's status
     * @tc.desc    Function test
     */
        it("voicemail_query_test_1300", 0, async function (done) {
            console.info("------------voicemail_query_test_1300  is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                "phone_number": "1158220165", "display_name": "xiaoming", "voice_status": 1
            };
            try {
                var voiceMailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage voicemail_query_test_1300: voiceMailId = " + voiceMailId);
                expect(voiceMailId > 0).assertTrue();
                await voiceMailQuery();
                await voiceMailDelete("voicemail_query_test_1300");
                done();
            } catch (error) {
                console.info("logMessage voicemail_query_test_1300: voicemial insert error = " + error);
                done();
            }
            async function voiceMailQuery() {
                var resultColumns = ["id", "display_name", "phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("voice_status", "1");
                condition.and();
                condition.equalTo("phone_number", "1158220165");
                try {
                    var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info('voicemail_query_test_1300 id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == voicemailId.toString()).assertTrue();
                            console.info('voicemail_query_test_1300 displayName = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "xiaoming").assertTrue();
                            console.info('voicemail_query_test_1300 phone_number = ' + resultSet.getString(2));
                            expect(resultSet.getString(2) == "1158220165").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info("voicemail_query_test_1300 query error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_voicemail_insert_test_300
     * @tc.name    Abnormal use case, an incorrect field is passed in when inserting data
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_insert_test_300", 0, async function (done) {
            console.info("------------abnormal_voicemail_insert_test_300 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_numbers": "18942010321", "display_name": "xiaoming"
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage abnormal_voicemail_insert_test_300: voicemialId = " + voicemailId);
                expect(voicemailId == -1).assertTrue();
                done();
            } catch (error) {
                console.info("abnormal_voicemail_insert_test_300 insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  abnormal_voicemail_insert_test_400
     * @tc.name    Exception use case, an incorrect table name is passed in when inserting data
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_insert_test_400", 0, async function (done) {
            console.info("------------abnormal_voicemail_insert_test_400 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var errorUri = "dataability:///com.ohos.calllogability/calls/voicemails";

            var insertValues = {
                "phone_number": "18942010321", "display_name": "xiaoming"
            };
            try {
                var voicemialId = await DAHelper.insert(errorUri, insertValues)
                console.info("logMessage abnormal_voicemail_insert_test_400: voicemialId = " + voicemialId);
                expect(voicemialId == -1).assertTrue();
                var map = new Map();
                map.set("phone_number", "18942010321");
                await voiceMailQueryError(map, "abnormal_voicemail_insert_test_400");
                done();
            } catch (error) {
                console.info("abnormal_voicemail_insert_test_400 insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  abnormal_voicemail_update_test_1100
     * @tc.name    Abnormal use case, passing in a wrong field when updating data
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_update_test_1100", 0, async function (done) {
            console.info("------------abnormal_voicemail_update_test_1100 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "18942010321"
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage abnormal_voicemail_update_test_1100: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                await abnormalUpdate();
                await voiceMailDelete("abnormal_voicemail_update_test_1100");
                done();
            } catch (error) {
                console.info("abnormal_voicemail_update_test_1100 insert error = " + error);
                done();
            }
            async function abnormalUpdate() {
                var updateValues = {
                    "phone_numbers": "11111111111111111"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", voicemailId.toString());
                try {
                    var updataCode = await DAHelper.update(voicemailUri, updateValues, condition);
                    console.info("logMessage abnormal_voicemail_update_test_1100: updataCode = " + updataCode);
                    expect(updataCode == -1).assertTrue();
                    var map = new Map();
                    map.set("phone_number", "11111111111111111");
                    await voiceMailQueryError(map, "abnormal_voicemail_insert_test_400");
                } catch (error) {
                    console.info("logMessage abnormal_voicemail_update_test_1100: update error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_voicemail_update_test_1200
     * @tc.name    Abnormal use case, an incorrect table name is passed in when updating data
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_update_test_1200", 0, async function (done) {
            console.info("------------abnormal_voicemail_update_test_1200 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var errorUri = "dataability:///com.ohos.calllogability/calls/voicemails";

            var insertValues = {
                "phone_number": "9952364"
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage abnormal_voicemail_update_test_1200: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                await abnormalUpdate();
                await voiceMailDelete("abnormal_voicemail_update_test_1200");
                done();
            } catch (error) {
                console.info("logMessage abnormal_voicemail_update_test_1200: insert error = " + error);
                done();
            }

            async function abnormalUpdate() {
                var updateValues = {
                    "phone_number": "22001122"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                try {
                    var updataCode = await DAHelper.update(errorUri, updateValues, condition)
                    console.info("logMessage abnormal_voicemail_update_test_1200: updataCode = " + updataCode);
                    expect(updataCode == -1).assertTrue();
                    var map = new Map();
                    map.set("phone_number", "22001122");
                    await voiceMailQueryError(map, "abnormal_voicemail_insert_test_400");
                } catch (error) {
                    console.info("logMessage abnormal_voicemail_update_test_1200: update error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_voicemail_delete_test_700
     * @tc.name    Abnormal use case, passing in a wrong field when deleting data
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_delete_test_700", 0, async function (done) {
            console.info("------------abnormal_voicemail_delete_test_700 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "789130"
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage abnormal_voicemail_delete_test_700: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                await abnormalDelete();
                await voiceMailDelete("abnormal_voicemail_delete_test_700");
                done();
            } catch (error) {
                console.info("logMessage abnormal_voicemail_delete_test_700: insert error = " + error);
                done();
            }

            async function abnormalDelete() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", voicemailId.toString());
                try {
                    var deleteCode = await DAHelper.delete(voicemailUri, condition);
                    console.info("logMessage abnormal_voicemail_delete_test_700: deleteCode = " + deleteCode);
                    expect(deleteCode == -1).assertTrue();
                } catch (error) {
                    console.info("logMessage abnormal_voicemail_delete_test_700: delete error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_voicemail_delete_test_800
     * @tc.name    Abnormal use case, passing in an incorrect table name when deleting data
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_delete_test_800", 0, async function (done) {
            console.info("------------abnormal_voicemail_delete_test_800 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var errorUri = "dataability:///com.ohos.calllogability/calls/voicemails";

            var insertValues = {
                "phone_number": "1564721"
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage abnormal_voicemail_delete_test_800: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                await abnormalDelete();
                await voiceMailDelete("abnormal_voicemail_delete_test_800");
                done();
            } catch (error) {
                console.info("logMessage abnormal_voicemail_delete_test_800: insert error = " + error);
                done();
            }

            async function abnormalDelete() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                try {
                    var deleteCode = await DAHelper.delete(errorUri, condition);
                    console.info("logMessage abnormal_voicemail_delete_test_800: deleteCode = " + deleteCode);
                    expect(deleteCode == -1).assertTrue();
                } catch (error) {
                    console.info("logMessage abnormal_voicemail_delete_test_800: delete error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_voicemail_query_test_1500
     * @tc.name    Abnormal use case, an incorrect field is passed in when querying
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_query_test_1500", 0, async function (done) {
            console.info("------------abnormal_voicemail_query_test_1500 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "1564721"
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                console.info("logMessage abnormal_voicemail_query_test_1500: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                await abnormalQuery();
                await voiceMailDelete("abnormal_voicemail_query_test_1500");
                done();
            } catch (error) {
                console.info("logMessage abnormal_voicemail_query_test_1500: insert error = " + error);
                done();
            }

            async function abnormalQuery() {
                var resultColumns = ["id", "phone_number"];

                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", voicemailId.toString());
                try {
                    var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition)
                    console.info('logMessage abnormal_voicemail_query_test_1500: goToFirstRow' + resultSet.goToFirstRow());
                    expect(resultSet.goToFirstRow()).assertEqual(false);
                    resultSet.close();
                } catch (error) {
                    console.info('logMessage abnormal_voicemail_query_test_1500: query error = ' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_voicemail_batchinsert_test_1900
     * @tc.name    Abnormal use cases, when inserting data in batches, some of them failed, check the processing logic
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_batchinsert_test_1900", 0, async function (done) {
            console.info("--------logMessage abnormal_voicemail_batchinsert_test_1900 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var addBulkValue1 = {
                "phone_number": "5555555555", "ring_duration": "500"
            };
            var addBulkValue2 = {
                "phone_number": "1511002", "ring_duration": "100"
            };
            var addBulkValue3 = {
                "phone_numbers": "1521003", "ring_duration": "100"
            };
            var addBulkValue4 = {
                "phone_number": "1521004", "ring_durations": "100"
            };
            var addBulkValue5 = {
                "phone_number": "1521005", "ring_duration": "100"
            };
            var listAddBluk = [];
            listAddBluk[0] = addBulkValue1;
            listAddBluk[1] = addBulkValue2;
            listAddBluk[2] = addBulkValue3;
            listAddBluk[3] = addBulkValue4;
            listAddBluk[4] = addBulkValue5;
            try {
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk)
                console.info("logMessage abnormal_voicemail_batchinsert_test_1900: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == -1).assertTrue();
                done();
            } catch (error) {
                console.info('logMessage abnormal_voicemail_batchinsert_test_1900: batchInsert error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  voiceMail_delete_test_2000
     * @tc.name    batchDelete
     * @tc.desc    Function test
     */
        it("voiceMail_delete_test_2000", 0, async function (done) {
            console.info("--------logMessage voiceMail_delete_test_2000 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var voiceMailId = await DAHelper.insert(voicemailUri, common.getVoiceMail());
                console.info("logMessage voiceMail_delete_test_2000: voiceMailId = " + voiceMailId);
                expect(voiceMailId > 0).assertTrue();
                var map = common.getVoiceMailMap()
                map.set("id", voiceMailId.toString());
                await voiceMailQuery(map, "voiceMail_delete_test_2000");
                await executeBatch();
            } catch (error) {
                console.info("logMessage voiceMail_delete_test_2000: voiceMailId insert error = " + error);
                done();
            }
            async function executeBatch() {
                console.info("logMessage voiceMail_delete_test_2000:  executeBatch start ");
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voiceMailId.toString());
                DAHelper.executeBatch(URI_VOICEMAIL, [{
                                                          uri: voicemailUri,
                                                          type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                                                          predicates: condition,
                                                          expectedCount: 0,
                                                          PredicatesBackReferences: {},
                                                          interrupted: true,
                                                      }],
                    (error, data) => {
                        console.info("logMessage voiceMail_delete_test_2000: executeBatch data = " + JSON.stringify(data));
                        console.info("logMessage voiceMail_delete_test_2000: data_3 = " + data);
                        console.info("logMessage voiceMail_delete_test_2000:  data_1= " + data[0].count);
                        expect(data[0].count == 0).assertTrue();
                        done();
                    });
            }
        });

        /**
     * @tc.number  voiceMail_update_test_2100
     * @tc.name    batchUpdate
     * @tc.desc    Function test
     */
        it("voiceMail_update_test_2100", 0, async function (done) {
            console.info("--------logMessage voiceMail_update_test_2100 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var voiceMailId = await DAHelper.insert(voicemailUri, common.getVoiceMail());
                console.info("logMessage voiceMail_update_test_2100: voiceMailId = " + voiceMailId);
                expect(voiceMailId > 0).assertTrue();
                var map = common.getVoiceMailMap();
                map.set("id", voiceMailId.toString());
                await voiceMailQuery(map, "voiceMail_update_test_2100");
                await executeBatch();
            } catch (error) {
                console.info("logMessage voiceMail_update_test_2100: calllog insert error = " + error);
                done();
            }
            async function executeBatch() {
                var updateValues = {
                    "phone_number": "123456789"
                };
                console.info("logMessage voiceMail_update_test_2100:  executeBatch start ");
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voiceMailId.toString());
                DAHelper.executeBatch(URI_VOICEMAIL, [{
                                                          uri: voicemailUri,
                                                          type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                                                          valuesBucket: updateValues,
                                                          predicates: condition,
                                                          expectedCount: 0,
                                                          PredicatesBackReferences: {},
                                                          interrupted: true,
                                                      }],
                    (error, data) => {
                        console.info("logMessage voiceMail_update_test_2100: executeBatch data = " + JSON.stringify(data));
                        console.info("logMessage voiceMail_update_test_2100: data_3 = " + data);
                        console.info("logMessage voiceMail_update_test_2100:  data_1= " + data[0].count);
                        expect(data[0].count == 0).assertTrue();
                        done();
                    });
            }
        });

        /**
     * @tc.number  voicemail_insert_test_2200
     * @tc.name    Insert voicemail status 0
     * @tc.desc    Function test
     */
        it("voicemail_insert_test_2200", 0, async function (done) {
            console.info("---------logMessage voicemail_insert_test_2200 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)

            var stringValue = {
                "phone_number": "1557544452542200", "voice_status": 0
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, stringValue)
                console.info("logMessage voicemail_insert_test_2200: voicemailId = " + voicemailId);
                expect(voicemailId > 0).assertTrue();
                var map = new Map();
                map.set("id", voicemailId.toString());
                map.set("phone_number", "1557544452542200");
                map.set("voice_status", "0");
                await voiceMailQuery(map, "voicemail_insert_test_2200");
                await voiceMailDelete("voicemail_insert_test_2200");
                done();
            } catch (error) {
                console.info("logMessage voicemail_insert_test_2200: voicemail insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_insert_test_2300
     * @tc.name    Insert 3 pieces of voice mail data first, then delete 2 pieces of data,
     *             and verify whether the deletion is successful
     * @tc.desc    Function test
     */
        it("voicemail_insert_test_2300", 0, async function (done) {
            console.info("---------logMessage voicemail_insert_test_2300 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)

            var stringValue = {
                "phone_number": "155754445254", "voice_status": 0
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, stringValue)
                var voicemailIdOne = await DAHelper.insert(voicemailUri, stringValue)
                var voicemailIdTwo = await DAHelper.insert(voicemailUri, stringValue)
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                condition.or();
                condition.equalTo("id", voicemailIdOne.toString());
                var deleteCode = await DAHelper.delete(voicemailUri, condition);
                console.info("logMessage voicemail_insert_test_2300: deleteCode = " + deleteCode);
                var resultColumns = [];
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                expect(resultSet.rowCount == 0).assertTrue();
                resultSet.close();
                await voiceMailDelete("voicemail_insert_test_2300");
                done();
            } catch (error) {
                console.info("logMessage voicemail_insert_test_2300: voicemail insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_insert_test_2400
     * @tc.name    Insert 3 pieces of voice mail data first, then update 2 pieces of data,
     *             and verify whether the deletion is successful
     * @tc.desc    Function test
     */
        it("voicemail_insert_test_2400", 0, async function (done) {
            console.info("---------logMessage voicemail_insert_test_2400 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)

            var stringValue = {
                "phone_number": "155754445254", "voice_status": 0
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, stringValue)
                var voicemailIdOne = await DAHelper.insert(voicemailUri, stringValue)
                var voicemailIdTwo = await DAHelper.insert(voicemailUri, stringValue)
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                condition.or();
                condition.equalTo("id", voicemailIdOne.toString());
                var updateValue = {
                    "phone_number": "14541555754445254", "voice_status": 1
                };
                var updateCode = await DAHelper.update(voicemailUri, updateValue, condition);
                console.info("logMessage voicemail_insert_test_2400: deleteCode = " + updateCode);

                var map = new Map();
                map.set("id", voicemailId.toString());
                map.set("phone_number", "14541555754445254");
                map.set("voice_status", "1");
                await voiceMailQuery(map, "voicemail_insert_test_2400");

                var mapOne = new Map();
                mapOne.set("id", voicemailIdOne.toString());
                mapOne.set("phone_number", "14541555754445254");
                mapOne.set("voice_status", "1");
                await voiceMailQuery(mapOne, "voicemail_insert_test_2400");
                await voiceMailDelete("voicemail_insert_test_2400");
                done();
            } catch (error) {
                console.info("logMessage voicemail_insert_test_2400: voicemail insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_insert_test_2500
     * @tc.name    Query the voice mailbox with status 0 or id and return the query result
     * @tc.desc    Function test
     */
        it("voicemail_query_test_2500", 0, async function (done) {
            console.info("---------logMessage voicemail_query_test_2500 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)
            var stringValue = {
                "phone_number": "4454445254", "voice_status": 0
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, stringValue)
                var voicemailIdOne = await DAHelper.insert(voicemailUri, stringValue)
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.beginWrap();
                condition.equalTo("id", voicemailId.toString());
                condition.or();
                condition.equalTo("id", voicemailIdOne.toString());
                condition.endWrap();
                condition.and();
                condition.equalTo("voice_status", "0");
                var resultColumns = [];
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                expect(resultSet.rowCount == 2).assertTrue();
                resultSet.close();
                await voiceMailDelete("voicemail_query_test_2500");
                done();
            } catch (error) {
                console.info("logMessage voicemail_query_test_2500: voicemail insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  voicemail_query_test_2600
     * @tc.name    Query the voice mailbox and return the query result
     * @tc.desc    Function test
     */
        it("voicemail_query_test_2600", 0, async function (done) {
            console.info("---------logMessage voicemail_query_test_2600 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)
            var stringValue = {
                "phone_number": "445444445254", "voice_status": 0
            };
            try {
                var voicemailId = await DAHelper.insert(voicemailUri, stringValue);
                console.info('voicemail_query_test_2600 voicemailId = ' + voicemailId);
                var voicemailIdOne = await DAHelper.insert(voicemailUri, stringValue);
                console.info('voicemail_query_test_2600  voicemailIdOne = ' + voicemailIdOne);
                var condition = new ohosDataAbility.DataAbilityPredicates();
                var resultColumns = [];
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                expect(resultSet.rowCount == 2).assertTrue();
                resultSet.close();
                await voiceMailDelete("voicemail_query_test_2600");
                done();
            } catch (error) {
                console.info("logMessage voicemail_query_test_2600: voicemail insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  abnormal_voicemail_insert_test_2700
     * @tc.name    When the voicemail status is added, a non-existent field is passed in
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_insert_test_2700", 0, async function (done) {
            console.info("---------logMessage abnormal_voicemail_insert_test_2700 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)
            try {
                var insertValues = {
                    display_name: "2700name", voice_statuss: "1"
                };
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                expect(voicemailId == -1).assertTrue();
                var condition = new ohosDataAbility.DataAbilityPredicates();
                var resultColumns = [];
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                expect(resultSet.rowCount == 0).assertTrue();
                await voiceMailDelete("abnormal_voicemail_insert_test_2700");
                done();
            } catch (error) {
                done();
            }
        });

        /**
     * @tc.number  abnormal_voicemail_delete_test_2800
     * @tc.name    When deleting the voicemail status, pass in a non-existent field
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_delete_test_2800", 0, async function (done) {
            console.info("---------logMessage abnormal_voicemail_delete_test_2800 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)
            try {
                var insertValues = {
                    display_name: "2800name", voice_status: "1"
                };
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                expect(voicemailId > 0).assertTrue();
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                var updateValues = {
                    display_name: "2800name", voice_statuss: "0"
                };
                var updateCode = await DAHelper.update(voicemailUri, updateValues, condition);
                expect(updateCode == -1).assertTrue();
                var map = new Map();
                map.set("id", voicemailId.toString());
                map.set("display_name", "2800name");
                map.set("voice_status", "1");
                await voiceMailQuery(map, "abnormal_voicemail_delete_test_2800");
                await voiceMailDelete("abnormal_voicemail_delete_test_2800");
                done();
            } catch (error) {
                done();
            }
        });

        /**
     * @tc.number  abnormal_voicemail_update_test_2900
     * @tc.name    When update the voicemail status, pass in a non-existent field
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_update_test_2900", 0, async function (done) {
            console.info("---------logMessage abnormal_voicemail_update_test_2900 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)
            try {
                var insertValues = {
                    display_name: "2900name", voice_status: "1"
                };
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                expect(voicemailId > 0).assertTrue();
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", voicemailId.toString());
                var updateValues = {
                    display_name: "2900name", voice_statuss: "0"
                };
                var updateCode = await DAHelper.update(voicemailUri, updateValues, condition);
                expect(updateCode == -1).assertTrue();
                var map = new Map();
                map.set("id", voicemailId.toString());
                map.set("display_name", "2900name");
                map.set("voice_status", "1");
                await voiceMailQuery(map, "abnormal_voicemail_update_test_2900");
                await voiceMailDelete("abnormal_voicemail_update_test_2900");
                done();
            } catch (error) {
                done();
            }
        });

        /**
     * @tc.number  abnormal_voicemail_update_test_3000
     * @tc.name    When update the voicemail status, pass in a non-existent field
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_update_test_3000", 0, async function (done) {
            console.info("---------logMessage abnormal_voicemail_update_test_3000 is starting!---------")
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            console.info('logMessage uri = ' + URI_VOICEMAIL)
            try {
                var insertValues = {
                    display_name: "2900name", voice_status: "1"
                };
                var voicemailId = await DAHelper.insert(voicemailUri, insertValues);
                expect(voicemailId > 0).assertTrue();
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", voicemailId.toString());
                var resultColumns = [];
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                expect(resultSet.rowCount == -1).assertTrue();
                resultSet.close();
                await voiceMailDelete("abnormal_voicemail_update_test_3000");
                done();
            } catch (error) {
                done();
            }
        });

        /**
     * @tc.number   abnormal_voicemail_batchInsert_test_3100
     * @tc.name    Insert voicemail data in batches , pass in a non-existent field
     * @tc.desc    Function test
     */
        it("abnormal_voicemail_batchInsert_test_3100", 0, async function (done) {
            console.info("-------------logMessage abnormal_voicemail_batchInsert_test_3100 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValuesOne = {
                "voice_status": "0", "display_name": "xiaoming"
            };
            var insertValuesTwo = {
                "voice_status": "1", "display_name": "xiaohei"
            };
            var insertValuesThree = {
                "voice_statusss": "1", "display_name": "xiaohong"
            };
            var insertValuesFour = {
                "voice_status": "0", "display_name": "xiaohei"
            };
            var insertValuesFive = {
                "voice_status": "0", "display_name": "xiaofen"
            };
            var listAddBluk = [];
            listAddBluk[0] = insertValuesOne;
            listAddBluk[1] = insertValuesTwo;
            listAddBluk[2] = insertValuesThree;
            listAddBluk[3] = insertValuesFour;
            listAddBluk[4] = insertValuesFive;
            try {
                var batchInsertCode = await DAHelper.batchInsert(voicemailUri, listAddBluk);
                console.info("logMessage abnormal_voicemail_batchInsert_test_3100: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == -1).assertTrue();
                var condition = new ohosDataAbility.DataAbilityPredicates();
                var resultColumns = [];
                var resultSet = await DAHelper.query(voicemailUri, resultColumns, condition);
                expect(resultSet.rowCount == 0).assertTrue();
                resultSet.close();
                done();
            } catch (error) {
                console.info("logMessage abnormal_voicemail_batchInsert_test_3100: batchInsert error = " + error);
                done();
            }
        });

        afterAll(async function () {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_VOICEMAIL);
            console.info('voicemai afterAll logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            var deleteCode = await DAHelper.delete(voicemailUri, condition);
            console.info('voicemai afterAll logMessage deleteCode = ' + deleteCode);
        });
    })
}