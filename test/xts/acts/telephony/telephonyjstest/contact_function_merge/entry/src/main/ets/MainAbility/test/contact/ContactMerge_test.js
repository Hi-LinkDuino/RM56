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

const URI_CONTACTS = "dataability:///com.ohos.contactsdataability";
const rawContactUri = "dataability:///com.ohos.contactsdataability/contacts/raw_contact";
const contactDataUri = "dataability:///com.ohos.contactsdataability/contacts/contact_data";

const autoMergeUri = "dataability:///com.ohos.contactsdataability/contacts/raw_contact/auto_merge";
const splitUri = "dataability:///com.ohos.contactsdataability/contacts/raw_contact/split_contact";
const manualMergeUri = "dataability:///com.ohos.contactsdataability/contacts/raw_contact/manual_merge";
const uriQueryMergeList = "dataability:///com.ohos.contactsdataability/contacts/raw_contact/query_merge_list";
const deletedUri = "dataability:///com.ohos.contactsdataability/contacts/deleted_raw_contact";

export default function ContactMergeTest() {
    describe('ContactMergeTest', function () {
        console.info("ContactMergeTest start!");

        function sleep(numberMillis) {
            var now = new Date();
            var exitTime = now.getTime() + numberMillis;
            while (true) {
                now = new Date();
                if (now.getTime() > exitTime)
                return;
            }
        }

        async function contactDataInsert1(DAHelper, rawContactIdFirst) {
            var contactDataValues = {
                "raw_contact_id": rawContactIdFirst, "content_type": "name", "detail_info": "xiaoli"
            };
            try {
                var contactDataId1 = await DAHelper.insert(contactDataUri, contactDataValues);
                sleep(1000);
                console.info("logMessage contact_auto_merge_and_spilt_test_100: contactDataId1 = " + contactDataId1);
                expect(contactDataId1 > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contact_auto_merge_and_spilt_test_100: contact_data_1 insert error = ' + error);
            }
        }

        async function contactDataInsert2(DAHelper, rawContactIdSecond) {
            var contactDataValues = {
                "raw_contact_id": rawContactIdSecond, "content_type": "name", "detail_info": "xiaoli"
            };
            try {
                var contactDataId2 = await DAHelper.insert(contactDataUri, contactDataValues);
                sleep(1000);
                console.info("logMessage contact_auto_merge_and_spilt_test_100: contactDataId2 = " + contactDataId2);
                expect(contactDataId2 > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contact_auto_merge_and_spilt_test_100: contact_data_2 insert error = ' + error);
            }
        }

        async function autoMerge(DAHelper, rawContactIdFirst) {
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var updateValues = {};
            try {
                sleep(4000);
                var autoMergeCode = await DAHelper.update(autoMergeUri, updateValues, condition);
                console.info("logMessage contact_auto_merge_and_spilt_test_100: autoMergeCode = " + autoMergeCode);
                expect(autoMergeCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, "contact_auto_merge_and_spilt_test_100", 2);
                sleep(2000);
                await contactSplit(DAHelper, rawContactIdFirst);
                await deleteAll(rawContactUri, "contact_auto_merge_and_spilt_test_100");
                await deleteAll(contactDataUri, "contact_auto_merge_and_spilt_test_100");
            } catch (error) {
                console.info("logMessage contact_auto_merge_and_spilt_test_100: auto_merge error = " + error);
            }
        }

        async function contactSplit(DAHelper, rawContactIdFirst) {
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            var array = [rawContactIdFirst.toString()];
            condition2.in("raw_contact_id", array);
            var updateValues2 = {};
            try {
                var splitCode = await DAHelper.update(splitUri, updateValues2, condition2);
                sleep(4000);
                console.info('logMessage contact_auto_merge_and_spilt_test_100 splitCode = ' + splitCode);
                expect(splitCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, "contact_auto_merge_and_spilt_test_100", 1);
            } catch (error) {
                console.info('logMessage contact_auto_merge_and_spilt_test_100 split error = ' + error);
            }
        }

        async function deleteAll(uri, tag) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            var deleteCode = await DAHelper.delete(uri, condition);
            console.info(tag + ': deleteAll deleteCode = ' + deleteCode);
            expect(deleteCode == 0).assertTrue();
        }

        async function contactMergeQuery(map, tag, size) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ' : contactMergeQuery start ! DAHelper = ' + DAHelper);
            var resultColumns = ["contact_id"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            console.info(tag + ' : map_id  = ' + map.get("id"));
            condition.equalTo("id", map.get("id"));
            try {
                var resultSetID = await DAHelper.query(rawContactUri, resultColumns, condition);
                sleep(1000);
                var conatctID;
                if (resultSetID.goToFirstRow()) {
                    do {
                        conatctID = resultSetID.getString(resultSetID.getColumnIndex("contact_id"));
                        console.info(tag + ' : contact_id  = ' + conatctID);
                    } while (resultSetID.goToNextRow());
                }
                resultSetID.close();
                var pre = new ohosDataAbility.DataAbilityPredicates();
                pre.equalTo("contact_id", conatctID);
                var newResultColumns = [];
                var resultSet = await DAHelper.query(rawContactUri, newResultColumns, pre);
                sleep(1000);
                console.info(tag + ' : contactMergeQuery start ! rowCount = ' + resultSet.rowCount);
                expect(resultSet.rowCount == size).assertEqual(true);
                resultSet.close();
            } catch (error) {
                console.info(tag + " : logMessage contactMergeQuery: error = " + error);
            }
        }

        async function updateContactData(updateDataValue, contactDataId, testName) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            var updateValues = {
                "detail_info": updateDataValue
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", contactDataId.toString());
            var code = await DAHelper.update(contactDataUri, updateValues, condition);
            console.info(testName + "  updateContactData: code = " + code);
            sleep(3000);
        }

        async function insertContactData(DAHelper, rawContactId, detailInfo, types, tag) {
            var size = detailInfo.length;
            for (var i = 0; i < size; i++) {
                var contactDataValues = {
                    "raw_contact_id": rawContactId.toString(),
                    "content_type": types[i],
                    "detail_info": detailInfo[i]
                };
                try {
                    var contactDataId = await DAHelper.insert(contactDataUri, contactDataValues);
                    sleep(2000);
                    expect(contactDataId > 0).assertTrue();
                    console.info(tag + ' logMessage :  insert contactDataId = ' + contactDataId);
                } catch (error) {
                    console.info('logMessage : contact_data_1 insert error = ' + error);
                }
            }
        }

        async function insertContactDataSingle(DAHelper, rawContactId, detailInfo, type) {
            var size = detailInfo.length;
            var contactDataValues = {
                "raw_contact_id": rawContactId, "content_type": type, "detail_info": detailInfo
            };
            try {
                var contactDataId1 = await DAHelper.insert(contactDataUri, contactDataValues);
                sleep(1000);
                expect(contactDataId1 > 0).assertTrue();
            } catch (error) {
                console.info('logMessage : contact_data_1 insert error = ' + error);
            }
            return contactDataId1;
        }

        async function manualMergeList(DAHelper, rawContactList, testName) {
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            condition2.in("raw_contact_id", rawContactList);
            var updateValues2 = {};
            try {
                var ManualMergeCode = await DAHelper.update(manualMergeUri, updateValues2, condition2);
                sleep(1000);
                console.info("logMessage " + testName + " ManualMergeCode = " + ManualMergeCode);
                expect(ManualMergeCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactList[0].toString());
                await contactMergeQuery(map, testName, rawContactList.length);
                await deleteAll(rawContactUri, testName);
                await deleteAll(contactDataUri, testName);
            } catch (error) {
                console.info("logMessage " + testName + " manual_merge error = " + error);
            }
        }

        /**
     * @tc.number  contact_autoMerge_and_spilt_test_100
     * @tc.name    Two contacts without mobile phone number with the same name are automatically merged and split
     * @tc.desc    Function test
     */
        it("contact_autoMerge_and_spilt_test_100", 0, async function (done) {
            console.info("------logMessage contact_auto_merge_and_spilt_test_100 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues1 = {
                "display_name": "xiaoli"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_auto_merge_and_spilt_test_100: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                await contactDataInsert1(DAHelper, rawContactIdFirst);
            } catch (error) {
                console.info('logMessage contact_auto_merge_and_spilt_test_100: raw_contact_1 insert error = ' + error);
                done();
            }

            var insertRawContactValues2 = {
                "display_name": "xiaoli"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_auto_merge_and_spilt_test_100: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                await contactDataInsert2(DAHelper, rawContactIdSecond);
            } catch (error) {
                console.info('logMessage contact_auto_merge_and_spilt_test_100: raw_contact_2 insert error = ' + error);
                done();
            }

            await autoMerge(DAHelper, rawContactIdFirst);
            done();
        });

        /**
     * @tc.number  contact_manualMerge_test_200
     * @tc.name    Two contacts with the same name, one with a mobile phone number and the other without a mobile phone
     * number, call the manual merge interface
     * @tc.desc    Function test
     */
        it("contact_manualMerge_test_200", 0, async function (done) {
            console.info("------logMessage contact_manualMerge_test_200 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues1 = {
                "display_name": "xiaoming"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_200: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                await manualMergeDataInsert(DAHelper, rawContactIdFirst);
            } catch (error) {
                console.info('logMessage contact_manualMerge_test_200: raw_contact_1 insert error = ' + error);
                done();
            }

            var insertRawContactValues2 = {
                "display_name": "xiaoming"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_200: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                await manualMergeDataInsertTwo(DAHelper, rawContactIdSecond);
            } catch (error) {
                console.info('logMessage contact_manualMerge_test_200: raw_contact_2 insert error = ' + error);
                done();
            }
            var rawContactIds = [rawContactIdFirst + "", rawContactIdSecond + ""];
            await manualMergeList(DAHelper, rawContactIds, "contact_query_merge_list_test_2000");
            done();
        });

        async function manualMergeDataInsert(DAHelper, rawContactIdFirst) {
            var contactDataValuesOne = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "name",
                "detail_info": "xiaoming"
            };
            var contactDataValuesTwo = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "phone",
                "detail_info": "123456789",
            };
            var listAddBluk = [];
            listAddBluk[0] = contactDataValuesOne;
            listAddBluk[1] = contactDataValuesTwo;
            try {
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_200: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contact_manualMerge_test_200: batch insert error = " + error)
            }
        }

        async function manualMergeDataInsertTwo(DAHelper, rawContactIdSecond) {
            var contactDataValues = {
                "raw_contact_id": rawContactIdSecond, "content_type": "name", "detail_info": "xiaoming"
            };
            try {
                var contactDataId = await DAHelper.insert(contactDataUri, contactDataValues);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_200: contactDataId = " + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contact_manualMerge_test_200: contact_data_2 insert error = " + error)
            }
        }

        /**
     * @tc.number  contact_autoMerge_test_300
     * @tc.name    Two contacts with the same name have a mobile phone number, and the mobile phone numbers are the
     * same. Call the automatic merge interface
     * @tc.desc    Function test
     */
        it("contact_autoMerge_test_300", 0, async function (done) {
            console.info("------logMessage contact_autoMerge_test_300 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues1 = {
                "display_name": "xiaohong"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_autoMerge_test_300: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                await autoMergeDataInsert(DAHelper, rawContactIdFirst);
            } catch (error) {
                console.info('logMessage contact_autoMerge_test_300: raw_contact_1 insert error = ' + error);
                done();
            }

            var insertRawContactValues2 = {
                "display_name": "xiaohong"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_autoMerge_test_300: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                await autoMergeDataInsertTwo(DAHelper, rawContactIdSecond);
            } catch (error) {
                console.info('logMessage contact_autoMerge_test_300: raw_contact_2 insert error = ' + error);
                done();
            }

            await autoMerger(DAHelper, rawContactIdFirst, "contact_autoMerge_test_300");
            done();
        });

        async function autoMergeDataInsert(DAHelper, rawContactIdFirst) {
            var contactDataValuesOne = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "name",
                "detail_info": "xiaohong"
            };
            var contactDataValuesTwo = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "phone",
                "detail_info": "123465",
            };
            var listAddBluk = [];
            listAddBluk[0] = contactDataValuesOne;
            listAddBluk[1] = contactDataValuesTwo;
            try {
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                sleep(1000);
                console.info("logMessage contact_autoMerge_test_300: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contact_autoMerge_test_300: batch insert1 error = " + error)
            }
        }

        async function autoMergeDataInsertTwo(DAHelper, rawContactIdSecond) {
            var contactDataValuesThree = {
                "raw_contact_id": rawContactIdSecond,
                "content_type": "name",
                "detail_info": "xiaohong"
            };
            var contactDataValuesFour = {
                "raw_contact_id": rawContactIdSecond,
                "content_type": "phone",
                "detail_info": "123465",
            };
            var listAddBluk2 = [];
            listAddBluk2[0] = contactDataValuesThree;
            listAddBluk2[1] = contactDataValuesFour;
            try {
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk2);
                sleep(1000);
                console.info("logMessage contact_autoMerge_test_300: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contact_autoMerge_test_300: batch insert2 error = " + error)
            }
        }

        async function autoMerger(DAHelper, rawContactIdFirst, testName) {
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var updateValues = {};
            try {
                var autoMergeCode = await DAHelper.update(autoMergeUri, updateValues, condition);
                sleep(1000);
                console.info("logMessage  " + testName + "autoMergeCode = " + autoMergeCode);
                expect(autoMergeCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, testName, 2);
                await deleteAll(rawContactUri, testName);
                await deleteAll(contactDataUri, testName);
            } catch (error) {
                console.info("logMessage " + testName + "auto_merge error =" + error);
            }
        }

        async function autoNotMerger(DAHelper, rawContactIdFirst, testName) {
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var updateValues = {};
            try {
                sleep(2000);
                var autoMergeCode = await DAHelper.update(autoMergeUri, updateValues, condition);
                console.info("logMessage  " + testName + "autoMergeCode = " + autoMergeCode);
                expect(autoMergeCode == -1).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, testName, 1);
                await deleteAll(rawContactUri, testName);
                await deleteAll(contactDataUri, testName);
            } catch (error) {
                console.info("logMessage " + testName + "auto_merge error =" + error);
            }
        }

        /**
     * @tc.number  contact_manualMerge_test_400
     * @tc.name    Two contacts with the same name have a mobile phone number, and the mobile phone numbers are
     * different. Call the manual merge interface
     * @tc.desc    Function test
     */
        it("contact_manualMerge_test_400", 0, async function (done) {
            console.info("------logMessage contact_manualMerge_test_400 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues1 = {
                "display_name": "xiaozhang"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_400: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                await noequalPhoneDataInsert(DAHelper, rawContactIdFirst)
            } catch (error) {
                console.info('logMessage contact_manualMerge_test_400: raw_contact_1 insert error = ' + error);
                done();
            }

            var insertRawContactValues2 = {
                "display_name": "xiaozhang"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_400: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                await noequalPhoneDataInsertTwo(DAHelper, rawContactIdSecond)
            } catch (error) {
                console.info('logMessage contact_manualMerge_test_400: raw_contact_2 insert error = ' + error);
                done();
            }

            await noequalManualMerge(DAHelper, rawContactIdFirst, rawContactIdSecond);
            done();
        });

        async function noequalPhoneDataInsert(DAHelper, rawContactIdFirst) {
            var contactDataValuesOne = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "name",
                "detail_info": "xiaozhang"
            };
            var contactDataValuesTwo = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "phone",
                "detail_info": "147852",
            };
            var listAddBluk = [];
            listAddBluk[0] = contactDataValuesOne;
            listAddBluk[1] = contactDataValuesTwo;
            try {
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_400: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contact_manualMerge_test_400: batch insert1 error = " + error)
            }
        }

        async function noequalPhoneDataInsertTwo(DAHelper, rawContactIdSecond) {
            var contactDataValuesThree = {
                "raw_contact_id": rawContactIdSecond,
                "content_type": "name",
                "detail_info": "xiaozhang"
            };
            var contactDataValuesFour = {
                "raw_contact_id": rawContactIdSecond,
                "content_type": "phone",
                "detail_info": "996369",
            };
            var listAddBluk2 = [];
            listAddBluk2[0] = contactDataValuesThree;
            listAddBluk2[1] = contactDataValuesFour;
            try {
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk2);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_400: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contact_manualMerge_test_400: batch insert2 error = " + error)
            }
        }

        async function noequalManualMerge(DAHelper, rawContactIdFirst, rawContactIdSecond) {
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            var id = [rawContactIdFirst.toString(), rawContactIdSecond.toString()];
            condition2.in("raw_contact_id", id);
            var updateValues2 = {};
            try {
                var ManualMergeCode = await DAHelper.update(manualMergeUri, updateValues2, condition2);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_400 ManualMergeCode = " + ManualMergeCode);
                expect(ManualMergeCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, "contact_manualMerge_test_400", 2);
                await deleteAll(rawContactUri, "contact_manualMerge_test_400");
                await deleteAll(contactDataUri, "contact_manualMerge_test_400");
            } catch (error) {
                console.info("logMessage contact_manualMerge_test_400 manual_merge error = " + error);
            }
        }

        /**
     * @tc.number  contact_manualMerge_test_500
     * @tc.name    Two contacts with the same name, one with two mobile phone numbers and the other without mobile phone
     * numbers, call the manual merge interface
     * @tc.desc    Function test
     */
        it("contact_manualMerge_test_500", 0, async function (done) {
            console.info("------logMessage contact_manualMerge_test_500 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues1 = {
                "display_name": "xiaozhou"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_500: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                await manualDataInsert(DAHelper, rawContactIdFirst)
            } catch (error) {
                console.info('logMessage contact_manualMerge_test_500: raw_contact_1 insert error = ' + error);
                done();
            }

            var insertRawContactValues2 = {
                "display_name": "xiaozhou"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_500: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                await manualDataInsertTwo(DAHelper, rawContactIdSecond)
            } catch (error) {
                console.info('logMessage contact_manualMerge_test_500: raw_contact_2 insert error = ' + error);
                done();
            }
            await hasOneNoPhoneManualMerge(DAHelper, rawContactIdFirst, rawContactIdSecond);
            done();
        });

        async function manualDataInsert(DAHelper, rawContactIdFirst) {
            var contactDataValuesOne = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "name",
                "detail_info": "xiaozhou"
            };
            var contactDataValuesTwo = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "phone",
                "detail_info": "123456789",
            };
            var contactDataValuesThree = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "phone",
                "detail_info": "996632",
            };
            var listAddBluk = [];
            listAddBluk[0] = contactDataValuesOne;
            listAddBluk[1] = contactDataValuesTwo;
            listAddBluk[2] = contactDataValuesThree;
            try {
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_500: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contact_manualMerge_test_500: batch insert1 error = " + error)
            }
        }

        async function manualDataInsertTwo(DAHelper, rawContactIdSecond) {
            var contactDataValuesFour = {
                "raw_contact_id": rawContactIdSecond,
                "content_type": "name",
                "detail_info": "xiaozhou"
            };
            try {
                var contactDataId = await DAHelper.insert(contactDataUri, contactDataValuesFour);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_500: contactDataId = " + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contact_manualMerge_test_500: contact_data insert error = " + error)
            }
        }

        async function hasOneNoPhoneManualMerge(DAHelper, rawContactIdFirst, rawContactIdSecond) {
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            var id = [rawContactIdFirst.toString(), rawContactIdSecond.toString()];
            condition2.in("raw_contact_id", id);
            var updateValues2 = {};
            try {
                var ManualMergeCode = await DAHelper.update(manualMergeUri, updateValues2, condition2);
                sleep(1000);
                console.info("logMessage contact_manualMerge_test_500 ManualMergeCode = " + ManualMergeCode);
                expect(ManualMergeCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, "contact_manualMerge_test_500", 2);
                await deleteAll(rawContactUri, "contact_manualMerge_test_500");
                await deleteAll(contactDataUri, "contact_manualMerge_test_500");
            } catch (error) {
                console.info("logMessage contact_manualMerge_test_500 manual_merge error = " + error);
            }
        }

        /**
     * @tc.number  contact_autoMerge_test_600
     * @tc.name    Batch add contacts, then call auto merge interface.
     * @tc.desc    Function test
     */
        it("contact_autoMerge_test_600", 0, async function (done) {
            console.info("------logMessage contact_autoMerge_test_600 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var insertRawContactValues1 = {
                    "display_name": "xiaoliu"
                };
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_autoMerge_test_600: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contact_autoMerge_test_600: raw_contact_1 insert error = ' + error);
                done();
            }

            try {
                var insertRawContactValues2 = {
                    "display_name": "xiaoliu"
                };
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_autoMerge_test_600: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contact_autoMerge_test_600: raw_contact_2 insert error = ' + error);
                done();
            }

            try {
                var insertRawContactValues3 = {
                    "display_name": "xiaoliu"
                };
                var rawContactIdThird = await DAHelper.insert(rawContactUri, insertRawContactValues3);
                sleep(1000);
                console.info("logMessage contact_autoMerge_test_600: rawContactIdThird = " + rawContactIdThird);
                expect(rawContactIdThird > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contact_autoMerge_test_600: raw_contact_3 insert error = ' + error);
                done();
            }

            await contactDataBatchInsert();
            done();
        });

        async function contactDataBatchInsert(DAHelper, rawContactIdFirst, rawContactIdSecond, rawContactIdThird) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactIdFirst, "content_type": "name", "detail_info": "xiaoliu"
            };
            var contactDataValues2 = {
                "raw_contact_id": rawContactIdSecond, "content_type": "name", "detail_info": "xiaoliu"
            };
            var contactDataValues3 = {
                "raw_contact_id": rawContactIdThird, "content_type": "name", "detail_info": "xiaoliu"
            };
            var listAddBluk = [];
            listAddBluk[0] = contactDataValues1;
            listAddBluk[1] = contactDataValues2;
            listAddBluk[2] = contactDataValues3;
            try {
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                sleep(2000);
                console.info("logMessage contact_autoMerge_test_600: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                await threeAutoMerger();
                await deleteAll(rawContactUri, "contact_autoMerge_test_600");
                await deleteAll(contactDataUri, "contact_autoMerge_test_600");
            } catch (error) {
                console.info('logMessage contact_autoMerge_test_600: raw_contact insert error = ' + error);
            }
        }

        async function threeAutoMerger(DAHelper, rawContactIdFirst) {
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var updateValues = {};
            try {
                var autoMergeCode = await DAHelper.update(autoMergeUri, updateValues, condition);
                sleep(1000);
                console.info("logMessage contact_autoMerge_test_600 autoMergeCode = " + autoMergeCode);
                expect(autoMergeCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, "contact_autoMerge_test_600", 3);
            } catch (error) {
                console.info("logMessage contact_autoMerge_test_600 auto_merge error = " + error);
            }
        }

        /**
     * @tc.number  contact_query_merge_list_test_700
     * @tc.name    Query contacts to be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_700", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_700 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "xiaowu"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_700: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                await queryContactDataInsert(DAHelper, rawContactIdFirst);
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_700: raw_contact_1 insert error = ' + error);
                done();
            }

            var insertRawContactValues2 = {
                "display_name": "xiaowu"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_700: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                await queryContactDataInsertTwo(DAHelper, rawContactIdSecond);
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_700: raw_contact_2 insert error = ' + error);
                done();
            }
            await queryMergeList(DAHelper, rawContactIdFirst, rawContactIdSecond, "contact_query_merge_list_test_700");
            done();
        });

        async function queryContactDataInsert(DAHelper, rawContactIdFirst) {
            var contactDataValues = {
                "raw_contact_id": rawContactIdFirst, "content_type": "name", "detail_info": "xiaowu"
            };
            try {
                var contactDataId1 = await DAHelper.insert(contactDataUri, contactDataValues);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_700: contactDataId1 = " + contactDataId1);
                expect(contactDataId1 > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_700: contact_data_1 insert error = ' + error);
            }
        }

        async function queryContactDataInsertTwo(DAHelper, rawContactIdSecond) {
            var contactDataValuesOne = {
                "raw_contact_id": rawContactIdSecond,
                "content_type": "name",
                "detail_info": "xiaowu"
            };
            var contactDataValuesTwo = {
                "raw_contact_id": rawContactIdSecond,
                "content_type": "phone",
                "detail_info": "002563"
            };
            var listAddBluk = [];
            listAddBluk[0] = contactDataValuesOne;
            listAddBluk[1] = contactDataValuesTwo;
            try {
                var batchInsertCode = await DAHelper.batchInsert(contactDataUri, listAddBluk);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_700: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_700: contact_data_2 insert error = ' + error);
            }
        }

        async function queryMergeList(DAHelper, rawContactIdFirst, rawContactIdSecond, testName) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var columns = [];
            var predicates = new ohosDataAbility.DataAbilityPredicates();
            try {
                sleep(3000);
                var resultSet = await DAHelper.query(uriQueryMergeList, columns, predicates);
                var array = [rawContactIdFirst, rawContactIdSecond];
                var i = 0;
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var rawContactId = resultSet.getInt(resultSet.getColumnIndex("raw_contact_id"));
                            console.info(testName + (array[i] == rawContactId));
                            expect(array[i] == rawContactId).assertTrue();
                            console.info(testName + rawContactId);
                            i++;
                        } while (resultSet.goToNextRow())
                    }
                }
                resultSet.close();
                await deleteAll(rawContactUri, testName);
                await deleteAll(contactDataUri, testName);
            } catch (error) {
                console.info(testName + ": query error = " + error);
            }
        }

        /**
     * @tc.number  contact_query_merge_list_test_800
     * @tc.name    Insert contacts with the same name and different mobile phone numbers,
     *             and check whether the database can query contacts that can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_800", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_800 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "xiaowuu"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_800: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["xiaowuu", "1854154"];
                var types = ["name", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_800");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_800: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "xiaowuu"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_800: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["xiaowuu", "1854154414242"];
                var types1 = ["name", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_800");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_800: raw_contact_2 insert error = ' + error);
                done();
            }
            var rawContactIds = [rawContactIdFirst + "", rawContactIdSecond + ""];
            await manualMergeList(DAHelper, rawContactIds, "contact_query_merge_list_test_800");
            done();
        });

        /**
     * @tc.number  contact_query_merge_list_test_900
     * @tc.name    Insert two contacts with different names to see if the database can
     *             query contacts that can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_900", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_900 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "xiaowusu"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_900: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["xiaowusu"];
                var types = ["name"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_900");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_900: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "kxiaowusu"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_900: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["kxiaowusu"];
                var types1 = ["name"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_900");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_900: raw_contact_2 insert error = ' + error);
                done();
            }
            await manualNotMerge(DAHelper, rawContactIdFirst, rawContactIdSecond, "contact_query_merge_list_test_900");
            done();
        });

        async function manualNotMerge(DAHelper, rawContactIdFirst, rawContactIdSecond, tag) {
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            var id = [rawContactIdFirst.toString(), rawContactIdSecond.toString()];
            condition2.in("raw_contact_id", id);
            var updateValues2 = {};
            try {
                var ManualMergeCode = await DAHelper.update(manualMergeUri, updateValues2, condition2);
                sleep(1000);
                console.info(tag + "logMessage ManualMergeCode = " + ManualMergeCode);
                expect(ManualMergeCode == -1).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, tag, 1);
                await deleteAll(rawContactUri, tag);
                await deleteAll(contactDataUri, tag);
            } catch (error) {
                console.info(tag + "logMessage manual_merge error = " + error);
            }
        }

        /**
     * @tc.number  contact_query_merge_list_test_1000
     * @tc.name    Insert contacts with the same name and phone number, and check whether
     *             the database can query contacts that can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_1000", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1000 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "xiaotiantian"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1000: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["xiaotiantian", "1854154414242441"];
                var types = ["name", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_1000");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1000: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "xiaotiantian"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1000: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["xiaotiantian", "1854154414242441"];
                var types1 = ["name", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_1000");
                sleep(2000);
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1000: raw_contact_2 insert error = ' + error);
                done();
            }
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var updateValues = {};
            try {
                sleep(4000);
                var autoMergeCode = await DAHelper.update(autoMergeUri, updateValues, condition);
                console.info("logMessage contact_query_merge_list_test_1000: autoMergeCode = " + autoMergeCode);
                expect(autoMergeCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, "contact_query_merge_list_test_1000", 2);
                await deleteAll(rawContactUri, "contact_query_merge_list_test_1000");
                await deleteAll(contactDataUri, "contact_query_merge_list_test_1000");
                done();
            } catch (error) {
                console.info("logMessage contact_auto_merge_and_spilt_test_100: auto_merge error = " + error);
            }
        });

        /**
       * @tc.number  contact_query_merge_list_test_1100
       * @tc.name    Insert contacts with different names and different mobile phone numbers,
                     and check whether the database can query contacts that can be merged
       * @tc.desc    Function test
       */
        it("contact_query_merge_list_test_1100", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1100 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "wsxiuklkk"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1100: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["wsxiuklkk", "56465465"];
                var types = ["name", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_1100");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1100: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "xiaolilili"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1100: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["xiaolilili", "8954598595"];
                var types1 = ["name", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_1100");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1100: raw_contact_2 insert error = ' + error);
                done();
            }
            await manualNotMerge(DAHelper, rawContactIdFirst, rawContactIdSecond, "contact_query_merge_list_test_1100");
            done();
        });

        /**
        * @tc.number  contact_query_merge_list_test_1200
        * @tc.name    Insert two contacts with the same name. Contacts A and B have two
                      same mobile phone numbers, and check whether the database can be merged
        * @tc.desc    Function test
        */
        it("contact_query_merge_list_test_1200", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1200 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "lwsxiuklkk"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1200: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["lwsxiuklkk", "122504", "122505"];
                var types = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_1200");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1200: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "lwsxiuklkk"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1200: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["lwsxiuklkk", "122504", "122505"];
                var types1 = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_1200");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1200: raw_contact_2 insert error = ' + error);
                done();
            }
            await autoMerger(DAHelper, rawContactIdFirst, "contact_query_merge_list_test_1200");
            done();
        });

        /**
     * @tc.number  contact_query_merge_list_test_1300
     * @tc.name    Insert two contacts with the same name. Contacts A and B have two
     *             identical mobile phone numbers. Check whether the database can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_1300", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1300 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "xiaowuuklkk"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1300: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["xiaowuuklkk", "56465465", "122504"];
                var types = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_1300");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1300: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "xiaowuuklkk"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1300: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["xiaowuuklkk", "8954598595", "1225054"];
                var types1 = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_1300");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1300: raw_contact_2 insert error = ' + error);
                done();
            }
            var rawContactIds = [rawContactIdFirst + "", rawContactIdSecond + ""];
            await manualMergeList(DAHelper, rawContactIds, "contact_query_merge_list_test_1300");
            done();
        });

        /**
     * @tc.number  contact_query_merge_list_test_1400
     * @tc.name    Insert two contacts with the same name. Contacts A and B have two mobile phone
     *             numbers, one of which is the same. Check whether the database can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_1400", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1400 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "limingm"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1400: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["limingm", "122504555", "1122505"];
                var types = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_1400");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1400: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "limingm"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1400: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["limingm", "122504555", "1225056"];
                var types1 = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_1400");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1400: raw_contact_2 insert error = ' + error);
                done();
            }
            var rawContactIds = [rawContactIdFirst + "", rawContactIdSecond + ""];
            await manualMergeList(DAHelper, rawContactIds, "contact_query_merge_list_test_1400");
            done();
        });

        /**
     * @tc.number  contact_query_merge_list_test_1500
     * @tc.name    Insert two contacts with the same name, contact A has two mobile phone numbers,
     *             and contact B has one mobile phone number which is the same as one of the mobile phone
     *             numbers of A, and check whether the database can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_1500", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1500 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "kplimingm"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1500: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["kplimingm", "7122504555", "1122505"];
                var types = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_1500");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1500: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "kplimingm"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1500: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["kplimingm", "7122504555"];
                var types1 = ["name", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_1500");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1500: raw_contact_2 insert error = ' + error);
                done();
            }
            var rawContactIds = [rawContactIdFirst + "", rawContactIdSecond + ""];
            await manualMergeList(DAHelper, rawContactIds, "contact_query_merge_list_test_1500");
            done();
        });

        /**
     * @tc.number  contact_query_merge_list_test_1600
     * @tc.name    Insert two contacts with the same name, contact A has two mobile phone numbers,
     *             and contact B has a mobile phone number different from that of A, check whether
     *             the database can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_1600", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1600 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "pkplimingm"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1600: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["pkplimingm", "87122504555", "11122505"];
                var types = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_1600");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1600: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "pkplimingm"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1600: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["pkplimingm", "7122554504555", "11122505"];
                var types1 = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_1600");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1600: raw_contact_2 insert error = ' + error);
                done();
            }
            var rawContactIds = [rawContactIdFirst + "", rawContactIdSecond + ""];
            await manualMergeList(DAHelper, rawContactIds, "contact_query_merge_list_test_1600");
            done();
        });

        /**
     * @tc.number  contact_query_merge_list_test_1700
     * @tc.name    Insert multiple manually merged contacts to check whether the database can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_1700", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1700 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "llllllk"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1700: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["llllllk", "87122504555", "11122505"];
                var types = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_1700");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1700: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "llllllk"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1700: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["llllllk", "7555", "02505"];
                var types1 = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_1700");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1700: raw_contact_2 insert error = ' + error);
                done();
            }
            var insertRawContactValues3 = {
                "display_name": "llllllk"
            };
            try {
                var rawContactIdThird = await DAHelper.insert(rawContactUri, insertRawContactValues3);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1700: rawContactIdSecond = " + rawContactIdThird);
                expect(rawContactIdThird > 0).assertTrue();
                var detailInfo1 = ["llllllk", "87555", "002505"];
                var types1 = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdThird, detailInfo1, types1, "contact_query_merge_list_test_1700");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1700: raw_contact_2 insert error = ' + error);
                done();
            }
            var rawContactIds = [rawContactIdFirst + "", rawContactIdSecond + "", rawContactIdThird + ""];
            await manualMergeList(DAHelper, rawContactIds, "contact_query_merge_list_test_1700");
            done();
        });

        /**
     * @tc.number  contact_query_merge_list_test_1800
     * @tc.name    Import multiple automatically merged contacts to check whether the database can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_1800", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1800 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "jggbgbk"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1800: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["jggbgbk", "85555", "996174"];
                var types = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "contact_query_merge_list_test_1800");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1800: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "jggbgbk"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1800: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["jggbgbk", "85555", "996174"];
                var types1 = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "contact_query_merge_list_test_1800");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1800: raw_contact_2 insert error = ' + error);
                done();
            }
            var insertRawContactValues3 = {
                "display_name": "jggbgbk"
            };
            try {
                var rawContactIdThird = await DAHelper.insert(rawContactUri, insertRawContactValues3);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1800: rawContactIdSecond = " + rawContactIdThird);
                expect(rawContactIdThird > 0).assertTrue();
                var detailInfo1 = ["jggbgbk", "85555", "996174"];
                var types1 = ["name", "phone", "phone"];
                await insertContactData(DAHelper, rawContactIdThird, detailInfo1, types1, "contact_query_merge_list_test_1800");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1800: raw_contact_2 insert error = ' + error);
                done();
            }
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var updateValues = {};
            try {
                var autoMergeCode = await DAHelper.update(autoMergeUri, updateValues, condition);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1800 autoMergeCode = " + autoMergeCode);
                expect(autoMergeCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactIdFirst.toString());
                await contactMergeQuery(map, "contact_query_merge_list_test_1800", 3);
                await deleteAll(rawContactUri, "contact_query_merge_list_test_1800");
                await deleteAll(contactDataUri, "contact_query_merge_list_test_1800");
            } catch (error) {
                console.info("contact_query_merge_list_test_1800 auto_merge error =" + error);
            }
            done();
        });

        /**
     * @tc.number  contact_query_merge_list_test_1900
     * @tc.name    Insert two contacts with the same name, contact A has two mobile phone numbers,
     *             and contact B has a mobile phone number different from that of A, check whether
     *             the database can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_1900", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_1900 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "owjiuh"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1900: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var dataId = insertContactDataSingle(DAHelper, rawContactIdFirst, "owjiuh", "name");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1900: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "owjiuh"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_1900: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var dataId = insertContactDataSingle(DAHelper, rawContactIdFirst, "owjiuh", "name");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_1900: raw_contact_2 insert error = ' + error);
                done();
            }
            await updateContactData("owjiuh111", dataId, "contact_query_merge_list_test_1900");
            await autoNotMerger(DAHelper, rawContactIdFirst, "contact_query_merge_list_test_1900");
            done();
        });

        /**
     * @tc.number  contact_query_merge_list_test_2000
     * @tc.name    Insert two automatically merged contacts, modify the phone number of A, and then
     *             merge the contacts manually to check whether the database can be merged
     *             the database can be merged
     * @tc.desc    Function test
     */
        it("contact_query_merge_list_test_2000", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage contact_query_merge_list_test_2000 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "ollwjiuh"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_2000: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                await insertContactDataSingle(DAHelper, rawContactIdFirst, "ollwjiuh", "name");
                var dataIdTwo = await insertContactDataSingle(DAHelper, rawContactIdFirst, "8554544", "phone");
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_2000: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "ollwjiuh"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage contact_query_merge_list_test_2000: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                await insertContactDataSingle(DAHelper, rawContactIdSecond, "ollwjiuh", "name");
                await insertContactDataSingle(DAHelper, rawContactIdSecond, "8554544", "phone");
                sleep(2000);
            } catch (error) {
                console.info('logMessage contact_query_merge_list_test_2000: raw_contact_2 insert error = ' + error);
                done();
            }
            await updateContactData("8554544444", dataIdTwo, "contact_query_merge_list_test_2000");
            sleep(3000);
            var rawContactIds = [rawContactIdFirst + "", rawContactIdSecond + ""];
            await manualMergeList(DAHelper, rawContactIds, "contact_query_merge_list_test_2000");
            done();
        });

        /**
     * @tc.number  abnormal_merge_Update_test_2100
     * @tc.name    Insert two contacts with different names for automatic and manual merging operations
     * @tc.desc    Function test
     */
        it("abnormal_merge_Update_test_2100", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage abnormal_merge_Update_test_2100 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "2100mergeTest"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2100: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["2100mergeTest"];
                var types = ["name"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "abnormal_merge_Update_test_2100");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2100: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "2100mergeTestTwo"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2100: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["2100mergeTestTwo"];
                var types1 = ["name"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "abnormal_merge_Update_test_2100");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2100: raw_contact_2 insert error = ' + error);
                done();
            }
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var updateValues = {};
            var autoMergeCode = await DAHelper.update(autoMergeUri, updateValues, condition);
            console.info("logMessage abnormal_merge_Update_test_2100 autoMergeCode = " + autoMergeCode);
            expect(autoMergeCode == -1).assertTrue();
            await manualNotMerge(DAHelper, rawContactIdFirst, rawContactIdSecond, "abnormal_merge_Update_test_2100");
            done();
        });

        /**
     * @tc.number  abnormal_merge_Update_test_2200
     * @tc.name    Insert two contacts with same names for automatic and manual merging operations
     * @tc.desc    Function test
     */
        it("abnormal_merge_Update_test_2200", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage abnormal_merge_Update_test_2200 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "2200mergeTest"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2200: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["2200mergeTest", "45544"];
                var types = ["name", "phone"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "abnormal_merge_Update_test_2200");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2200: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "2200mergeTestTwo"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2200: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["2200mergeTestTwo", "55134865"];
                var types1 = ["name", "phone"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "abnormal_merge_Update_test_2200");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2200: raw_contact_2 insert error = ' + error);
                done();
            }
            await queryMergeListError(DAHelper, rawContactIdFirst, rawContactIdSecond, "abnormal_merge_Update_test_2200");
            done();
        });

        async function queryMergeListError(DAHelper, rawContactIdFirst, rawContactIdSecond, testName) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var columns = ["display_names"];
            var predicates = new ohosDataAbility.DataAbilityPredicates();
            try {
                sleep(3000);
                var resultSet = await DAHelper.query(uriQueryMergeList, columns, predicates);
                console.info(testName + 'resultSet.rowCount  = ' + resultSet.rowCount);
                expect(resultSet.rowCount == 0).assertTrue();
                resultSet.close();
                await deleteAll(rawContactUri, testName);
                await deleteAll(contactDataUri, testName);
            } catch (error) {
                console.info(testName + ": query error = " + error);
            }
        }

        /**
     * @tc.number  abnormal_merge_Update_test_2300
     * @tc.name    Insert two contacts with different names, and then automatically merge them to
     *             see if the database can be merged
     * @tc.desc    Function test
     */
        it("abnormal_merge_Update_test_2300", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage abnormal_merge_Update_test_2300 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "2300mergeTest"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2300: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["2300mergeTest"];
                var types = ["name"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "abnormal_merge_Update_test_2300");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2300: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "2300mergeTestName"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2300: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["2300mergeTestName"];
                var types1 = ["name"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "abnormal_merge_Update_test_2300");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2300: raw_contact_2 insert error = ' + error);
                done();
            }
            await autoNotMerger(DAHelper, rawContactIdFirst, "abnormal_merge_Update_test_2300");
            done();
        });

        /**
     * @tc.number  abnormal_merge_Update_test_2400
     * @tc.name    Insert two contacts with different names, and then automatically merge them to
     *             see if the database can be merged
     * @tc.desc    Function test
     */
        it("abnormal_merge_Update_test_2400", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage abnormal_merge_Update_test_2400 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "2400mergeTest"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2400: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["2400mergeTest"];
                var types = ["namee"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "abnormal_merge_Update_test_2400");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2400: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "2400mergeTest"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2400: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["2400mergeTest"];
                var types1 = ["name"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "abnormal_merge_Update_test_2400");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2400: raw_contact_2 insert error = ' + error);
                done();
            }
            await autoNotMerger(DAHelper, rawContactIdFirst, "abnormal_merge_Update_test_2400");
            done();
        });

        /**
     * @tc.number  abnormal_merge_Update_test_2500
     * @tc.name    Pass 0 after merging to split
     * @tc.desc    Function test
     */
        it("abnormal_merge_Update_test_2500", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage abnormal_merge_Update_test_2500 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "2500mergeTest"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2500: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["2500mergeTest"];
                var types = ["name"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "abnormal_merge_Update_test_2500");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2500: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "2500mergeTest"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2500: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["2500mergeTest"];
                var types1 = ["name"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "abnormal_merge_Update_test_2500");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2500: raw_contact_2 insert error = ' + error);
                done();
            }
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var updateValues = {};
            var autoMergeCode = await DAHelper.update(autoMergeUri, updateValues, condition);
            sleep(1000);
            console.info("logMessage abnormal_merge_Update_test_2500 autoMergeCode = " + autoMergeCode);
            expect(autoMergeCode == 0).assertTrue();
            var map = new Map();
            map.set("id", rawContactIdFirst.toString());
            await contactNotSplit(DAHelper, 0, "abnormal_merge_Update_test_2500");
            await contactMergeQuery(map, "abnormal_merge_Update_test_2500", 2);
            await deleteAll(rawContactUri, "abnormal_merge_Update_test_2500");
            await deleteAll(contactDataUri, "abnormal_merge_Update_test_2500");
            done();
        });

        /**
     * @tc.number  abnormal_merge_Update_test_2600
     * @tc.name    Pass -1 after merging to split
     * @tc.desc    Function test
     */
        it("abnormal_merge_Update_test_2600", 0, async function (done) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('before delete_all: start ! DAHelper = ' + DAHelper);
            console.info("------logMessage abnormal_merge_Update_test_2600 is starting!-----");
            var insertRawContactValues1 = {
                "display_name": "2600mergeTest"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(rawContactUri, insertRawContactValues1);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2600: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                var detailInfo = ["2600mergeTest"];
                var types = ["name"];
                await insertContactData(DAHelper, rawContactIdFirst, detailInfo, types, "abnormal_merge_Update_test_2600");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2600: raw_contact_1 insert error = ' + error);
                done();
            }
            var insertRawContactValues2 = {
                "display_name": "2600mergeTest"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(rawContactUri, insertRawContactValues2);
                sleep(1000);
                console.info("logMessage abnormal_merge_Update_test_2600: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                var detailInfo1 = ["2600mergeTest"];
                var types1 = ["name"];
                await insertContactData(DAHelper, rawContactIdSecond, detailInfo1, types1, "abnormal_merge_Update_test_2600");
            } catch (error) {
                console.info('logMessage abnormal_merge_Update_test_2600: raw_contact_2 insert error = ' + error);
                done();
            }
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var updateValues = {};
            var autoMergeCode = await DAHelper.update(autoMergeUri, updateValues, condition);
            sleep(1000);
            console.info("logMessage abnormal_merge_Update_test_2600 autoMergeCode = " + autoMergeCode);
            expect(autoMergeCode == 0).assertTrue();
            var map = new Map();
            map.set("id", rawContactIdFirst.toString());
            await contactNotSplit(DAHelper, -1, "abnormal_merge_Update_test_2600");
            await contactMergeQuery(map, "abnormal_merge_Update_test_2600", 2);
            await deleteAll(rawContactUri, "abnormal_merge_Update_test_2600");
            await deleteAll(contactDataUri, "abnormal_merge_Update_test_2600");
            done();
        });

        async function contactNotSplit(DAHelper, rawContactIdFirst, testName) {
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            var array = [rawContactIdFirst.toString()];
            condition2.in("raw_contact_id", array);
            var updateValues2 = {};
            try {
                var splitCode = await DAHelper.update(splitUri, updateValues2, condition2);
                sleep(4000);
                console.info(testName + 'logMessage  splitCode = ' + splitCode);
                expect(splitCode == -1).assertTrue();
            } catch (error) {
                console.info(testName + 'logMessage  split error = ' + error);
            }
        }

        afterAll(async function () {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('merge afterAll delete_All: start ! DAHelper = ' + DAHelper);
            var condition = new ohosDataAbility.DataAbilityPredicates();
            var deletedAll = await DAHelper.delete(deletedUri, condition);
            sleep(4000);
            console.info('merge afterAll delete_All : deletedAll ! DAHelper = ' + deletedAll);
        });
    });
}