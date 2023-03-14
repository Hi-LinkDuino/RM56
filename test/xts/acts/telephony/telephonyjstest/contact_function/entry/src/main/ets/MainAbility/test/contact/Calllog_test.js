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

const URI_CALLLOG = "dataability:///com.ohos.calllogability";
const calllogUri = "dataability:///com.ohos.calllogability/calls/calllog";
import common from './common.js';

export default function CalllogTest() {
    describe('CalllogTest', function () {
        console.log("----------CalllogTest is starting!----------");

        async function calllogQueryForALL(map, tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info(tag + ': calllogQueryByInsert start ! DAHelper = ' + DAHelper);
            var resultColumns = common.getCallLogResultColumns();
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", map.get("id"));
            try {
                var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                if (resultSet.rowCount > 0 && resultSet.goToFirstRow()) {
                    do {
                        for (var [key, value] of map) {
                            var dbresult = resultSet.getString(resultSet.getColumnIndex(key));
                            console.info(tag + ' : logMessage calllogQueryByInsert key = ' + key +
                            ' dbResult :' + dbresult + ' value : ' + value);
                            console.info(tag + ' : logMessage calllogQueryByInsert value ' + (value == dbresult));
                            expect(value == dbresult).assertEqual(true);
                        }
                    } while (resultSet.goToNextRow());
                }
                console.info(tag + " :logMessage calllogQueryByInsert: end");
                resultSet.close();
            } catch (error) {
                console.info(tag + " :logMessage calllogQueryByInsert: error = " + error);
            }
        }

        async function calllogQueryForDelete(map, tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info(tag + ': calllogQueryForDelete start ! DAHelper = ' + DAHelper);
            var resultColumns = common.getCallLogResultColumns();
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", map.get("id"));
            try {
                var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                expect(resultSet.goToFirstRow() == false).assertTrue();
                console.info(tag + " :logMessage calllogQueryForDelete: goToFirstRow " + resultSet.goToFirstRow());
                resultSet.close();
            } catch (error) {
                console.info(tag + " :logMessage calllogQueryForDelete: error = " + error);
            }
        }

        async function calllogQueryForBatchInsert(array, tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info(tag + ': calllogQueryForBatchInsert start ! DAHelper = ' + DAHelper);
            var resultColumns = common.getCallLogResultColumns();
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("phone_number", array[0].get("phone_number"));
            try {
                var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                var size = array.size();
                console.info(tag + ' : logMessage calllogQueryForBatchInsert: size' + size);
                expect(resultSet.rowCount == size).assertEqual(true);
                var i = 0;
                if (resultSet.rowCount > 0 && resultSet.goToFirstRow()) {
                    do {
                        for (var [key, value] of array[i]) {
                            var dbresult = resultSet.getString(resultSet.getColumnIndex(key));
                            console.info(tag + ' : logMessage calllogQueryForBatchInsert dbresult :' + dbresult +
                            ' value : ' + value);
                            console.info(tag + ' : logMessage calllogQueryForBatchInsert value ' + (value == dbresult));
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

        async function callLogDelete(tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            var deleteCode = await DAHelper.delete(calllogUri, condition);
            console.info(tag + ': callLogDelete deleteCode = ' + deleteCode);
            expect(deleteCode == 0).assertTrue();
        }

        /**
     * @tc.number  calllog_insert_test_100
     * @tc.name    Insert call log
     * @tc.desc    Function test
     */
        it("calllog_insert_test_100", 0, async function (done) {
            console.info("--------logMessage calllog_insert_test_100 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var calllogId = await DAHelper.insert(calllogUri, common.getCallLogInsert());
                console.info("logMessage calllog_insert_test_100: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                var map = common.getCallLogInsertMap()
                map.set("id", calllogId.toString());
                await calllogQueryForALL(map, "calllog_insert_test_100");
                await callLogDelete("calllog_insert_test_100");
                done();
            } catch (error) {
                console.info("logMessage calllog_insert_test_100: calllog insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  calllog_update_test_400
     * @tc.name    Single call log update
     * @tc.desc    Function test
     */
        it("calllog_update_test_400", 0, async function (done) {
            console.info("--------logMessage calllog_update_test_400 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            try {
                var calllogId = await DAHelper.insert(calllogUri, common.getCallLogInsert());
                console.info("logMessage calllog_update_test_400: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await updateOneCalllog();
                done();
            } catch (error) {
                console.info("logMessage calllog_update_test_400: calllog insert error = " + error);
                done();
            }
            ;

            async function updateOneCalllog() {
                try {
                    var condition = new ohosDataAbility.DataAbilityPredicates();
                    condition.equalTo("id", calllogId.toString());
                    var updateCode = await DAHelper.update(calllogUri, common.getCallLogUpdate(), condition);
                    console.info("logMessage calllog_update_test_400: updateCode = " + updateCode);
                    expect(updateCode == 0).assertTrue();
                    var map = common.getCallLogUpdateMap();
                    map.set("id", calllogId.toString());
                    await calllogQueryForALL(map, "calllog_update_test_400");
                    await callLogDelete("calllog_update_test_400");
                } catch (error) {
                    console.info("logMessage calllog_update_test_400: update error = " + error);
                    done();
                }
                ;
            }
        });

        /**
     * @tc.number  calllog_update_test_500
     * @tc.name    Update all call logs
     * @tc.desc    Function test
     */
        it("calllog_update_test_500", 0, async function (done) {
            console.info("--------logMessage calllog_update_test_500 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "1122445566", "ring_duration": "500"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage calllog_update_test_500: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await updateAllCalllog();
                done();
            } catch (error) {
                console.info("logMessage calllog_update_test_500: calllog insert error = " + error);
                done();
            }
            ;
            async function updateAllCalllog() {
                var updateValues = {
                    "answer_state": "1"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.greaterThan("id", "0");
                try {
                    var updateCode = await DAHelper.update(calllogUri, updateValues, condition)
                    console.info("logMessage calllog_update_test_500: updateCode = " + updateCode);
                    expect(updateCode == 0).assertTrue();
                    var map = new Map([["phone_number", "1122445566"], ["ring_duration", "500"]])
                    map.set("id", calllogId.toString());
                    map.set("answer_state", "1");
                    await calllogQueryForALL(map, "calllog_update_test_500");
                    await callLogDelete("calllog_update_test_500");
                } catch (error) {
                    console.info("logMessage calllog_update_test_500: update error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  calllog_delete_test_1300
     * @tc.name    Delete call log
     * @tc.desc    Function test
     */
        it("calllog_delete_test_1300", 0, async function (done) {
            console.info("--------logMessage calllog_delete_test_1300 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "123456", "ring_duration": "200"
            };
            try {
                var callLogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage calllog_delete_test_1300: callLogId = " + callLogId);
                expect(callLogId > 0).assertTrue();
                await deleteOneCallLog();
                done();
            } catch (error) {
                console.info("logMessage calllog_delete_test_1300: calllog insert error = " + error);
                done();
            }

            async function deleteOneCallLog() {
                try {
                    var condition = new ohosDataAbility.DataAbilityPredicates();
                    condition.equalTo("id", calllogId.toString());
                    var deleteCode = await DAHelper.delete(calllogUri, condition);
                    console.info("logMessage calllog_delete_test_1300: deleteCode = " + deleteCode);
                    expect(deleteCode == 0).assertTrue();
                    var map = new Map();
                    map.set("id", calllogId.toString());
                    await calllogQueryForDelete(map, "calllog_delete_test_1300");
                } catch (error) {
                    console.info("logMessage calllog_delete_test_1300: delete error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  calllog_batchInset_test_1600
     * @tc.name    Insert call logs in batches
     * @tc.desc    Function test
     */
        it("calllog_batchInset_test_1600", 0, async function (done) {
            console.info("--------logMessage calllog_batchInset_test_1600 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var addBulkValue1 = {
                "phone_number": "1600", "ring_duration": "333"
            };
            var addBulkValue2 = {
                "phone_number": "1600", "ring_duration": "600"
            };
            var addBulkValue3 = {
                "phone_number": "1600", "ring_duration": "600"
            };
            var addBulkValue4 = {
                "phone_number": "1600", "ring_duration": "600"
            };
            var addBulkValue5 = {
                "phone_number": "1600", "ring_duration": "600"
            };
            var listAddBluk = [];
            listAddBluk[0] = addBulkValue1;
            listAddBluk[1] = addBulkValue2;
            listAddBluk[2] = addBulkValue3;
            listAddBluk[3] = addBulkValue4;
            listAddBluk[4] = addBulkValue5;
            try {
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                console.info("logMessage calllog_batchInset_test_1600: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                await calllogQueryForBatchInsert(common.getCallLogBatchInsert(), "calllog_batchInset_test_1600");
                await callLogDelete("calllog_batchInset_test_1600");
                done();
            } catch (error) {
                console.info("logMessage calllog_batchInset_test_1600: error = " + error);
                done();
            }
        });

        /**
     * @tc.number  calllog_query_test_1200
     * @tc.name    Query all call logs
     * @tc.desc    Function test
     */
        it("calllog_query_test_1200", 0, async function (done) {
            console.info("------------calllog_query_test_1200  is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "123566456", "ring_duration": "200"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage calllog_query_test_1200: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                var map = new Map([["phone_number", "123566456"], ["ring_duration", "200"]]);
                map.set("id", calllogId.toString());
                await calllogQueryForALL(map, "calllog_query_test_1200");
                await callLogDelete("calllog_query_test_1200");
                done();
            } catch (error) {
                console.info("logMessage calllog_query_test_1200: calllog insert error = " + error);
            }
        });

        /**
     * @tc.number  calllog_query_test_1100
     * @tc.name    Query call logs of combined conditions
     * @tc.desc    Function test
     */
        it("calllog_query_test_1100", 0, async function (done) {
            console.info("------------calllog_query_test_1100 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                "phone_number": "44325008", "ring_duration": "100"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage calllog_query_test_1100: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await queryCalllog();
                await callLogDelete("calllog_query_test_1100");
                done()
            } catch (error) {
                console.info("logMessage calllog_query_test_1100: calllog insert error = " + error);
                done();
            }

            async function queryCalllog() {
                var resultColumns = ["id", "phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.greaterThan("id", "0");
                condition.and();
                condition.lessThan("ring_duration", "200").orderByAsc("id");
                try {
                    var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info("logMessage calllog_query_test_1100: columnNames:" + resultSet.columnNames);
                            console.info("logMessage calllog_query_test_1100: columnCount:" + resultSet.columnCount);
                            console.info('logMessage calllog_query_test_1100: id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == calllogId.toString()).assertTrue();
                            console.info('logMessage calllog_query_test_1100: phone_number = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "44325008").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info("logMessage calllog_query_test_1100: query error:" + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  calllog_query_test_800
     * @tc.name    Query a single call log
     * @tc.desc    Function test
     */
        it("calllog_query_test_800", 0, async function (done) {
            console.info("------------calllog_query_test_800 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "88888888888", "ring_duration": "100"
            };
            try {
                var callLogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage calllog_query_test_800: callLogId = " + callLogId);
                expect(callLogId > 0).assertTrue();
                await queryOneCallLog();
                await callLogDelete("calllog_query_test_800");
                done();
            } catch (error) {
                console.info("logMessage calllog_query_test_800: calllog insert error = " + error);
                done();
            }

            async function queryOneCallLog() {
                var resultColumns = ["id", "phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", calllogId.toString());
                try {
                    var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info("logMessage calllog_query_test_800: columnNames:" + resultSet.columnNames);
                            console.info("logMessage calllog_query_test_800: columnCount:" + resultSet.columnCount);
                            console.info('logMessage calllog_query_test_800: id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == calllogId.toString()).assertTrue();
                            console.info('logMessage calllog_query_test_800: phone_number = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "88888888888").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info("logMessage calllog_query_test_800: query error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  calllog_fuzzyquery_test_100
     * @tc.name    Fuzzy query ability based on mobile phone number (beginning of a certain field)
     * @tc.desc    Function test
     */
        it("calllog_fuzzyquery_test_100", 0, async function (done) {
            console.info("------------calllog_fuzzyquery_test_100 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "196320147"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage calllog_fuzzyquery_test_100: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await queryByStartsWithPhoneNumber();
                await callLogDelete("calllog_fuzzyquery_test_100");
                done();
            } catch (error) {
                console.info("logMessage calllog_fuzzyquery_test_100: calllog insert error = " + error);
                done();
            }

            async function queryByStartsWithPhoneNumber() {
                var resultColumns = ["id", "phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.like("phone_number", "196%");
                try {
                    var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info("logMessage calllog_fuzzyquery_test_100: columnNames:" + resultSet.columnNames);
                            console.info("logMessage calllog_fuzzyquery_test_100: columnCount:" + resultSet.columnCount);
                            console.info('logMessage calllog_fuzzyquery_test_100: id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == calllogId.toString()).assertTrue();
                            console.info(
                                'logMessage calllog_fuzzyquery_test_100: phone_number = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "196320147").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info("logMessage calllog_fuzzyquery_test_100: query error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  calllog_fuzzyquery_test_200
     * @tc.name    Fuzzy query ability based on mobile phone number (at the end of a certain field)
     * @tc.desc    Function test
     */
        it("calllog_fuzzyquery_test_200", 0, async function (done) {
            console.info("------------calllog_fuzzyquery_test_200 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "126467821"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage calllog_fuzzyquery_test_200: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await queryByEndWithPhoneNumber();
                await callLogDelete("calllog_fuzzyquery_test_200");
                done();
            } catch (error) {
                console.info("logMessage calllog_fuzzyquery_test_200: calllog insert error = " + error);
                done();
            }

            async function queryByEndWithPhoneNumber() {
                var resultColumns = ["id", "phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.like("phone_number", "%821");
                try {
                    var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info("logMessage calllog_fuzzyquery_test_200: columnNames:" + resultSet.columnNames);
                            console.info("logMessage calllog_fuzzyquery_test_200: columnCount:" + resultSet.columnCount);
                            console.info('logMessage calllog_fuzzyquery_test_200: id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == calllogId.toString()).assertTrue();
                            console.info(
                                'logMessage calllog_fuzzyquery_test_200: phone_number = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "126467821").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info("logMessage calllog_fuzzyquery_test_200: query error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  calllog_fuzzyquery_test_300
     * @tc.name    Fuzzy query ability based on mobile phone number (including a certain field)
     * @tc.desc    Function test
     */
        it("calllog_fuzzyquery_test_300", 0, async function (done) {
            console.info("------------calllog_fuzzyquery_test_300 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "1234567855557568"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage calllog_fuzzyquery_test_300: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await queryByPhoneNumberContainsField();
                await callLogDelete("calllog_fuzzyquery_test_300");
                done();
            } catch (error) {
                console.info("logMessage calllog_fuzzyquery_test_300: calllog insert error = " + error);
                done();
            }

            async function queryByPhoneNumberContainsField() {
                var resultColumns = ["id", "phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.like("phone_number", "%555%");
                try {
                    var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info("logMessage calllog_fuzzyquery_test_300: columnNames:" + resultSet.columnNames);
                            console.info("logMessage calllog_fuzzyquery_test_300: columnCount:" + resultSet.columnCount);
                            console.info('logMessage calllog_fuzzyquery_test_300: id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == calllogId.toString()).assertTrue();
                            console.info(
                                'logMessage calllog_fuzzyquery_test_300: phone_number = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "1234567855557568").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info("logMessage calllog_fuzzyquery_test_300: query error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_calllog_insert_test_200
     * @tc.name    Abnormal use case, an incorrect field is passed in when inserting data
     * @tc.desc    Function test
     */
        it("abnormal_calllog_insert_test_200", 0, async function (done) {
            console.info("------------abnormal_calllog_insert_test_200 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_numbers": "1111111111"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage abnormal_calllog_insert_test_200: calllogId = " + calllogId);
                expect(calllogId == -1).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_insert_test_200: calllog insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  abnormal_calllog_insert_test_300
     * @tc.name    Abnormal use case, an incorrect table name is passed in when inserting data
     * @tc.desc    Function test
     */
        it("abnormal_calllog_insert_test_300", 0, async function (done) {
            console.info("------------abnormal_calllog_insert_test_300 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var errorUri = "dataability:///com.ohos.calllogability/calls/calllogs";

            var insertValues = {
                "phone_number": "456789410"
            };
            try {
                var calllogId = await DAHelper.insert(errorUri, insertValues);
                console.info("logMessage abnormal_calllog_insert_test_300: calllogId = " + calllogId);
                expect(calllogId == -1).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_insert_test_300: calllog insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  abnormal_calllog_update_test_600
     * @tc.name    Abnormal use case, passing in a wrong field when updating data
     * @tc.desc    Function test
     */
        it("abnormal_calllog_update_test_600", 0, async function (done) {
            console.info("------------abnormal_calllog_update_test_600 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "1020202020"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage abnormal_calllog_update_test_600: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await abnormalUpdate();
                await callLogDelete("abnormal_calllog_update_test_600");
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_update_test_600: calllog insert error = " + error);
                done();
            }

            async function abnormalUpdate() {
                var updateValues = {
                    "phone_numbers": "22001122"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", calllogId.toString());
                try {
                    var updataCode = await DAHelper.update(calllogUri, updateValues, condition);
                    console.info("logMessage abnormal_calllog_update_test_600: updataCode = " + updataCode);
                    expect(updataCode == -1).assertTrue();
                    var map = new Map();
                    map.set("id", calllogId.toString());
                    map.set("phone_number", "1020202020");
                    await calllogQueryForALL(map, "abnormal_calllog_update_test_600");
                } catch (error) {
                    console.info("logMessage abnormal_calllog_update_test_600: update error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_calllog_update_test_700
     * @tc.name    Abnormal use case, an incorrect table name is passed in when updating data
     * @tc.desc    Function test
     */
        it("abnormal_calllog_update_test_700", 0, async function (done) {
            console.info("------------abnormal_calllog_update_test_700 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var errorUri = "dataability:///com.ohos.calllogability/calls/calllogs";

            var insertValues = {
                "phone_number": "9952364"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage abnormal_calllog_update_test_700: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await abnormalUpdate();
                await callLogDelete("abnormal_calllog_update_test_700");
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_update_test_700: calllog insert error = " + error);
                done();
            }

            async function abnormalUpdate() {
                var updateValues = {
                    "phone_numbers": "22001122"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", calllogId.toString());
                try {
                    var updataCode = await DAHelper.update(errorUri, updateValues, condition);
                    console.info("logMessage abnormal_calllog_update_test_700: updataCode = " + updataCode);
                    expect(updataCode == -1).assertTrue();
                    var map = new Map();
                    map.set("id", calllogId.toString());
                    map.set("phone_number", "9952364");
                    await calllogQueryForALL(map, "abnormal_calllog_update_test_700");
                    done();
                } catch (error) {
                    console.info("logMessage abnormal_calllog_update_test_700: update error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_calllog_delete_test_1400
     * @tc.name    Abnormal use case, passing in a wrong field when deleting data
     * @tc.desc    Function test
     */
        it("abnormal_calllog_delete_test_1400", 0, async function (done) {
            console.info("------------abnormal_calllog_delete_test_1400 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "789130"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage abnormal_calllog_delete_test_1400: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await abnormalDelete();
                await callLogDelete("abnormal_calllog_delete_test_1400");
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_delete_test_1400: calllog insert error = " + error);
                done();
            }

            async function abnormalDelete() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", calllogId.toString());
                try {
                    var deleteCode = await DAHelper.delete(calllogUri, condition);
                    console.info("logMessage abnormal_calllog_delete_test_1400: deleteCode = " + deleteCode);
                    expect(deleteCode == -1).assertTrue();
                    var map = new Map();
                    map.set("id", calllogId.toString());
                    map.set("phone_number", "789130");
                    await calllogQueryForALL(map, "abnormal_calllog_delete_test_1400");
                } catch (error) {
                    console.info("logMessage abnormal_calllog_delete_test_1400: delete error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_calllog_delete_test_1500
     * @tc.name    Abnormal use case, passing in an incorrect table name when deleting data
     * @tc.desc    Function test
     */
        it("abnormal_calllog_delete_test_1500", 0, async function (done) {
            console.info("------------abnormal_calllog_delete_test_1500 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var errorUri = "dataability:///com.ohos.calllogability/calls/calllogs";

            var insertValues = {
                "phone_number": "1564721"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage abnormal_calllog_delete_test_1500: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await abnormalDelete();
                await callLogDelete("abnormal_calllog_delete_test_1500");
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_delete_test_1500: calllog insert error = " + error);
                done();
            }

            async function abnormalDelete() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", calllogId.toString());
                try {
                    var deleteCode = await DAHelper.delete(errorUri, condition);
                    console.info("logMessage abnormal_calllog_delete_test_1500: deleteCode = " + deleteCode);
                    expect(deleteCode == -1).assertTrue();
                    done()
                    var map = new Map();
                    map.set("id", calllogId.toString());
                    map.set("phone_number", "1564721");
                    await calllogQueryForALL(map, "abnormal_calllog_delete_test_1500");
                } catch (error) {
                    console.info("logMessage abnormal_calllog_delete_test_1500: delete error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_calllog_query_test_900
     * @tc.name    Abnormal use case, an incorrect field is passed in when querying
     * @tc.desc    Function test
     */
        it("abnormal_calllog_query_test_900", 0, async function (done) {
            console.info("------------abnormal_calllog_query_test_900 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertValues = {
                "phone_number": "1564721"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage abnormal_calllog_query_test_900: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await abnormalQuery();
                await callLogDelete("abnormal_calllog_query_test_900");
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_query_test_900: calllog insert error = " + error);
                done();
            }

            async function abnormalQuery() {
                var resultColumns = ["id", "phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", calllogId.toString());
                try {
                    var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                    console.info('logMessage abnormal_calllog_query_test_900: goToFirstRow' + resultSet.goToFirstRow());
                    expect(resultSet.goToFirstRow()).assertEqual(false);
                    resultSet.close();
                } catch (error) {
                    console.info("logMessage abnormal_calllog_query_test_900:query error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_calllog_batchinsert_test_1700
     * @tc.name    abnormal_batchinsert
     * @tc.desc    Function test
     */
        it("abnormal_calllog_batchinsert_test_1700", 0, async function (done) {
            console.info("--------logMessage abnormal_calllog_batchinsert_test_1700 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
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
                var batchInsertCode = await DAHelper.batchInsert(calllogUri, listAddBluk);
                console.info("logMessage abnormal_calllog_batchinsert_test_1700: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == -1).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_batchinsert_test_1700: batchInsert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  calllog_delete_test_1800
     * @tc.name    batchDelete
     * @tc.desc    Function test
     */
        it("calllog_delete_test_1800", 0, async function (done) {
            console.info("--------logMessage calllog_delete_test_1800 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var calllogId = await DAHelper.insert(calllogUri, common.getCallLogInsert());
                console.info("logMessage calllog_delete_test_1800: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                var map = common.getCallLogInsertMap()
                map.set("id", calllogId.toString());
                await calllogQueryForALL(map, "calllog_delete_test_1800");
                await executeBatch();
            } catch (error) {
                console.info("logMessage calllog_delete_test_1800: calllog insert error = " + error);
                done();
            }

            async function executeBatch() {
                console.info("logMessage calllog_delete_test_1800:  executeBatch start ");
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", calllogId.toString());
                DAHelper.executeBatch(URI_CALLLOG, [{
                                                        uri: calllogUri,
                                                        type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                                                        predicates: condition,
                                                        expectedCount: 0,
                                                        PredicatesBackReferences: {},
                                                        interrupted: true,
                                                    }],
                    (error, data) => {
                        console.info("logMessage calllog_delete_test_1800: executeBatch data = " + JSON.stringify(data));
                        console.info("logMessage calllog_delete_test_1800: data_3 = " + data);
                        console.info("logMessage calllog_delete_test_1800:  data_1= " + data[0].count);
                        expect(data[0].count == 0).assertTrue();
                        done();
                    });
            };
        });

        /**
     * @tc.number  calllog_update_test_1900
     * @tc.name    batchUpdate
     * @tc.desc    Function test
     */
        it("calllog_update_test_1900", 0, async function (done) {
            console.info("--------logMessage calllog_update_test_1900 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var calllogId = await DAHelper.insert(calllogUri, common.getCallLogInsert());
                console.info("logMessage calllog_update_test_1900: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                var map = common.getCallLogInsertMap()
                map.set("id", calllogId.toString());
                await calllogQueryForALL(map, "calllog_update_test_1900");
                await executeBatch();
            } catch (error) {
                console.info("logMessage calllog_update_test_1900: calllog insert error = " + error);
                done();
            }

            async function executeBatch() {
                var updateValues = {
                    "phone_number": "123456789"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", calllogId.toString());
                DAHelper.executeBatch(URI_CALLLOG, [{
                                                        uri: calllogUri,
                                                        type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                                                        valuesBucket: updateValues,
                                                        predicates: condition,
                                                        expectedCount: 0,
                                                        PredicatesBackReferences: {},
                                                        interrupted: true,
                                                    }],
                    (error, data) => {
                        console.info("logMessage calllog_update_test_1900: executeBatch data = " + JSON.stringify(data));
                        console.info("logMessage calllog_update_test_1900: data_3 = " + data);
                        console.info("logMessage calllog_update_test_1900:  data_1= " + data[0].count);
                        expect(data[0].count == 0).assertTrue();
                        done();
                    });
            };
        });

        /**
     * @tc.number  calllog_Delete_test_2000
     * @tc.name    calllog Delete
     * @tc.desc    Function test
     */
        it("calllog_Delete_test_2000", 0, async function (done) {
            console.info("--------logMessage calllog_Delete_test_2000 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                "phone_number": "123456", "ring_duration": "200"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                var calllogIdTwo = await DAHelper.insert(calllogUri, insertValues);
                var calllogIdThree = await DAHelper.insert(calllogUri, insertValues);
                expect(calllogId > 0).assertTrue();
                expect(calllogIdTwo > 0).assertTrue();
                expect(calllogIdThree > 0).assertTrue();
                await deleteCalllog();
                done();
            } catch (error) {
                console.info("logMessage calllog_Delete_test_2000: calllog insert error = " + error);
                done();
            }

            async function deleteCalllog() {
                try {
                    var condition = new ohosDataAbility.DataAbilityPredicates();
                    condition.equalTo("id", calllogId.toString());
                    condition.or();
                    condition.equalTo("id", calllogIdTwo.toString());
                    condition.or();
                    condition.equalTo("id", calllogIdThree.toString());
                    var deleteCode = await DAHelper.delete(calllogUri, condition);
                    console.info("logMessage calllog_Delete_test_2000: deleteCode = " + deleteCode);
                    expect(deleteCode == 0).assertTrue();
                    var resultColumns = [];
                    var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                    expect(resultSet.rowCount == 0).assertTrue();
                    resultSet.close();
                } catch (error) {
                    console.info("logMessage calllog_Delete_test_2000: delete error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  calllog_Delete_test_2100
     * @tc.name    calllog Delete
     * @tc.desc    Function test
     */
        it("calllog_Delete_test_2100", 0, async function (done) {
            console.info("--------logMessage calllog_Delete_test_2100 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var calllogId = await DAHelper.insert(calllogUri, common.getCallLogInsert());
                console.info("logMessage calllog_Delete_test_2100: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                var map = common.getCallLogInsertMap()
                map.set("id", calllogId.toString());
                await calllogQueryForALL(map, "calllog_Delete_test_2100");
                await callLogDelete("calllog_Delete_test_2100");
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", calllogId);
                var resultColumns = [];
                var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                expect(resultSet.rowCount == 0).assertTrue();
                resultSet.close();
                done();
            } catch (error) {
                console.info("logMessage calllog_Delete_test_2100: calllog insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  calllog_queryContains_test_2200
     * @tc.name    calllog Delete
     * @tc.desc    Function test
     */
        it("calllog_queryContains_test_2200", 0, async function (done) {
            console.info("--------logMessage calllog_queryContains_test_2200 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                "phone_number": "123456789220099999"
            }
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage calllog_queryContains_test_2200: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();

                var resultColumns = ["phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.contains("phone_number", "99999");
                var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var phone = resultSet.getString(resultSet.getColumnIndex("phone_number"));
                            console.info("logMessage calllog_queryContains_test_2200: phone is = " + phone);
                            expect(phone == "123456789220099999").assertEqual(true);
                        } while (resultSet.goToNextRow());
                    }
                }
                resultSet.close();
                await callLogDelete();
                done();
            } catch (error) {
                console.info("logMessage calllog_queryContains_test_2200: calllog insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  abnormal_calllog_update_test_2300
     * @tc.name    Update all call records, pass in a field that does not exist
     * @tc.desc    Function test
     */
        it("abnormal_calllog_update_test_2300", 0, async function (done) {
            console.info("--------logMessage abnormal_calllog_update_test_2300 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                phone_number: "15748646546", display_name: "name2300"
            }
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                var calllogIdTwo = await DAHelper.insert(calllogUri, insertValues);
                expect(calllogId > 0).assertTrue();
                expect(calllogIdTwo > 0).assertTrue();
                await updateOneCalllog();
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_update_test_2300: calllog insert error = " + error);
                done();
            }
            ;
            async function updateOneCalllog() {
                try {
                    var insertValues = {
                        phone_number: "15748646546", display_names: "nameUpdateError2300"
                    };
                    var condition =
                        new ohosDataAbility.DataAbilityPredicates();
                    var updateCode = await DAHelper.update(calllogUri, insertValues, condition);
                    console.info("logMessage abnormal_calllog_update_test_2300: updateCode = " + updateCode);
                    expect(updateCode == -1).assertTrue();
                    var map = new Map();
                    map.set("id", calllogId.toString());
                    map.set("display_name", "name2300");
                    await calllogQueryForALL(map, "abnormal_calllog_update_test_2300");
                    map.set("id", calllogIdTwo.toString());
                    await calllogQueryForALL(map, "abnormal_calllog_update_test_2300");
                    await callLogDelete("calllog_update_test_2300");
                } catch (error) {
                    console.info("logMessage abnormal_calllog_update_test_2300: update error = " + error);
                    done();
                }
                ;
            }
        });

        /**
     * @tc.number  abnormal_calllog_query_test_2400
     * @tc.name    When querying call records based on combined conditions, incorrect field
     *             names and non-existent data are passed in
     * @tc.desc    Function test
     */
        it("abnormal_calllog_query_test_2400", 0, async function (done) {
            console.info("--------logMessage abnormal_calllog_query_test_2400 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                phone_number: "15748646546", display_name: "name2300"
            }
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                var calllogIdTwo = await DAHelper.insert(calllogUri, insertValues);
                expect(calllogId > 0).assertTrue();
                expect(calllogIdTwo > 0).assertTrue();
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", calllogId.toString() + "ksks");
                var resultColumns = ["display_names"];
                var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                console.info("abnormal_calllog_query_test_2400: resultSet.rowCount = " + resultSet.rowCount);
                expect(resultSet.rowCount == -1).assertTrue();
                resultSet.close();
                await callLogDelete("abnormal_calllog_query_test_2400");
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_query_test_2400: calllog insert error = " + error);
                done();
            }
            ;
        });

        /**
     * @tc.number  abnormal_calllog_query_test_2500
     * @tc.name    When querying all call records based on combined conditions, incorrect field
     *             names and non-existent data are passed in
     * @tc.desc    Function test
     */
        it("abnormal_calllog_query_test_2500", 0, async function (done) {
            console.info("--------logMessage abnormal_calllog_query_test_2500 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                phone_number: "15748646546", display_name: "name2300"
            }
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info('abnormal_calllog_query_test_2500 calllogId = ' + calllogId);
                var calllogIdTwo = await DAHelper.insert(calllogUri, insertValues);
                console.info('abnormal_calllog_query_test_2500 calllogIdTwo = ' + calllogIdTwo);
                expect(calllogId > 0).assertTrue();
                expect(calllogIdTwo > 0).assertTrue();
                var condition = new ohosDataAbility.DataAbilityPredicates();
                var resultColumns = ["display_names"];
                var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                console.info('abnormal_calllog_query_test_2500 resultSet.goToFirstRow() = ' + resultSet.goToFirstRow());
                console.info('abnormal_calllog_query_test_2500 resultSet.rowCount = ' + resultSet.rowCount);
                expect(resultSet.goToFirstRow() == false).assertTrue();
                resultSet.close();
                await callLogDelete("abnormal_calllog_query_test_2500");
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_query_test_2500: calllog insert error = " + error);
                done();
            }
            ;
        });

        /**
     * @tc.number  abnormal_calllog_delete_test_2600
     * @tc.name    Batch call records are added, and some of them fail, depending on the processing logic
     * @tc.desc    Function test
     */
        it("abnormal_calllog_delete_test_2600", 0, async function (done) {
            console.info("--------logMessage abnormal_calllog_delete_test_2600 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                phone_number: "15748646546", display_name: "name2300"
            }
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                var calllogIdTwo = await DAHelper.insert(calllogUri, insertValues);
                expect(calllogId > 0).assertTrue();
                expect(calllogIdTwo > 0).assertTrue();
            } catch (error) {
                console.info("logMessage abnormal_calllog_delete_test_2600: calllog insert error = " + error);
                done();
            }
            ;
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("ids", calllogIdTwo.toString());
            condition.or();
            condition.equalTo("id", calllogId.toString());
            var code = await DAHelper.delete(calllogUri, condition);
            console.info("logMessage abnormal_calllog_delete_test_2600: code = " + code);
            expect(code == -1).assertTrue();
            condition.clear();
            var resultColumns = [];
            var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
            var count = 2;
            expect(resultSet.rowCount == count).assertTrue();
            resultSet.close();
            await callLogDelete("abnormal_calllog_delete_test_2600");
            done();
        });

        /**
     * @tc.number  abnormal_calllog_update_test_2700
     * @tc.name    When batch call records are modified, the wrong field or table name is passed in
     * @tc.desc    Function test
     */
        it("abnormal_calllog_delete_test_2700", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                phone_number: "270015748646546", display_name: "name2700"
            }
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                var calllogIdTwo = await DAHelper.insert(calllogUri, insertValues);
                expect(calllogId > 0).assertTrue();
                expect(calllogIdTwo > 0).assertTrue();
                await updateOneCalllog();
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_delete_test_2700: calllog insert error = " + error);
                done();
            }
            ;
            async function updateOneCalllog() {
                try {
                    var insertValues = {
                        phone_number: "270015748646546", display_names: "nameUpdateError2700"
                    };
                    var condition = new ohosDataAbility.DataAbilityPredicates();
                    condition.equalTo("ids", calllogIdTwo.toString());
                    condition.or();
                    condition.equalTo("id", calllogId.toString());
                    var updateCode = await DAHelper.update(calllogUri, insertValues, condition);
                    console.info("logMessage abnormal_calllog_delete_test_2700: updateCode = " + updateCode);
                    expect(updateCode == -1).assertTrue();
                    var map = new Map();
                    map.set("id", calllogId.toString());
                    map.set("display_name", "name2700");
                    await calllogQueryForALL(map, "abnormal_calllog_delete_test_2700");
                    map.set("id", calllogIdTwo.toString());
                    await calllogQueryForALL(map, "abnormal_calllog_delete_test_2700");
                    await callLogDelete("abnormal_calllog_delete_test_2700");
                } catch (error) {
                    console.info("logMessage abnormal_calllog_delete_test_2700: update error = " + error);
                    done();
                }
                ;
            }
        });

        /**
     * @tc.number  abnormal_calllog_query_test_2800
     * @tc.name    Fuzzy search query based on mobile phone number
     * @tc.desc    Function test
     */
        it("abnormal_calllog_query_test_2800", 0, async function (done) {
            console.info("------------abnormal_calllog_query_test_2800 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValues = {
                "phone_number": "196320147"
            };
            try {
                var calllogId = await DAHelper.insert(calllogUri, insertValues);
                console.info("logMessage abnormal_calllog_query_test_2800: calllogId = " + calllogId);
                expect(calllogId > 0).assertTrue();
                await queryByStartsWithPhoneNumberError();
                await callLogDelete("abnormal_calllog_query_test_2800");
                done();
            } catch (error) {
                console.info("logMessage abnormal_calllog_query_test_2800: calllog insert error = " + error);
                done();
            }

            async function queryByStartsWithPhoneNumberError() {
                var resultColumns = ["id", "phone_number"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.like("phone_numbers", "196%");
                try {
                    var resultSet = await DAHelper.query(calllogUri, resultColumns, condition);
                    console.info("logMessage abnormal_calllog_query_test_2800: resultSet.rowCount = " + resultSet.rowCount);
                    expect(resultSet.rowCount == -1).assertTrue();
                    resultSet.close();
                } catch (error) {
                    console.info("logMessage abnormal_calllog_query_test_2800: query error = " + error);
                    done();
                }
            }
        });

        afterAll(async function () {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CALLLOG);
            console.info('callllog afterAll logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            var deleteCode = await DAHelper.delete(calllogUri, condition);
            console.info('callllog afterAll end logMessage deleteCode = ' + deleteCode);
        });
    })
}
