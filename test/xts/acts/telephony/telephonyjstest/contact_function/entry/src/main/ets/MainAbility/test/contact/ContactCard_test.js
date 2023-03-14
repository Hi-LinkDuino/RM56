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

const URI_CONTACTS = "dataability:///com.ohos.contactsdataability";
const profilecontactUri = "dataability:///com.ohos.contactsdataability/profile/contact";
const profileRawContactUri = "dataability:///com.ohos.contactsdataability/profile/raw_contact";
const profileContactDataUri = "dataability:///com.ohos.contactsdataability/profile/contact_data";
const profileGroupUri = "dataability:///com.ohos.contactsdataability/profile/groups";
const profileBlocklistUri = "dataability:///com.ohos.contactsdataability/profile/contact_blocklist";
const profilesearchContactUri = "dataability:///com.ohos.contactsdataability/profile/search_contact";
const profileDeletedContactsUri = "dataability:///com.ohos.contactsdataability/profile/deleted_raw_contact";
const profileDeletedRawContactRecord =
    "dataability:///com.ohos.contactsdataability/profile/deleted_raw_contact_record";

export default function ContactCardTest() {
    describe('ContactCardTest', function () {
        console.info("-------ContactsTest is starting!--------");

        function sleep(numberMillis) {
            var now = new Date();
            var exitTime = now.getTime() + numberMillis;
            while (true) {
                now = new Date();
                if (now.getTime() > exitTime)
                return;
            }
        }

        async function contactsQuery(map, tag, uri) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': contactsQuery start ! DAHelper = ' + DAHelper);
            var resultColumns = [];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", map.get("id"));
            try {
                var resultSet = await DAHelper.query(uri, resultColumns, condition);
                sleep(2000);
                    if (resultSet.rowCount > 0 && resultSet.goToFirstRow()) {
                        do {
                            for (var [key, value] of map) {
                                var dbresult = resultSet.getString(resultSet.getColumnIndex(key));
                                console.info(tag + ':logMessage contactsQuery key = ' + key + ' dbresult :' + dbresult +
                                ' value : ' + value);
                                expect(value == dbresult).assertEqual(true);
                            }
                        } while (resultSet.goToNextRow());
                    }

                resultSet.close();
            } catch (error) {
                console.info(tag + "logMessage contactsQuery: error = " + error);
            }
        }

        async function queryBatch(map, tag, size) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': queryBatch start ! DAHelper = ' + DAHelper);
            var resultColumns = [];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", map.get("raw_contact_id"));
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                sleep(1000);
                console.info(tag + ': queryBatch start ! resultSet.rowCount = ' + resultSet.rowCount);
                expect(resultSet.rowCount == size).assertEqual(true);
                resultSet.close();
            } catch (error) {
                console.info(tag + "logMessage resultSet.rowCount: error = " + error);
            }
        }

        async function queryIdForDelete(map, tag, uri) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': queryIdForDelete start ! DAHelper = ' + DAHelper);
            var resultColumns = common.getCallLogResultColumns();
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", map.get("id"));
            try {
                var resultSet = await DAHelper.query(uri, resultColumns, condition);
                sleep(1000);
                expect(resultSet.goToFirstRow() == false).assertTrue();
                console.info(tag + " :logMessage queryIdForDelete: goToFirstRow " + resultSet.goToFirstRow());
                resultSet.close();
            } catch (error) {
                console.info(tag + " :logMessage queryIdForDelete: error = " + error);
            }
        }

        async function deleteAll(uri, tag) {
            try {
                var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.greaterThan("id", "0");
                var deleteCode = await DAHelper.delete(uri, condition);
                console.info(tag + ': deleteAll deleteCode = ' + deleteCode);
                expect(deleteCode == 0).assertTrue();
            } catch (error) {
                console.info(tag + ': deleteAll error = ' + error);
            }
        }

        async function deleteRawContact(tag) {
            try {
                var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.greaterThan("id", "0");
                condition.and();
                condition.equalTo("is_deleted", "0");
                var deleteCode = await DAHelper.delete(profileRawContactUri, condition);
                console.info(tag + ': deleteRawContact deleteCode = ' + deleteCode);
                expect(deleteCode == 0).assertTrue();
                sleep(1000);
                var conditionAll = new ohosDataAbility.DataAbilityPredicates();
                conditionAll.greaterThan("id", "0");
                var code = await DAHelper.delete(profileDeletedContactsUri, conditionAll);
                console.info(tag + ': Completely delete code = ' + code);
                expect(code == 0).assertTrue();
            } catch (error) {
                console.info(tag + ': deleteRawContact error = ' + error);
            }
        }

        async function insertData(rawContactId, type, value, position) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            var contactDataValues1 =
                {
                    "raw_contact_id": rawContactId, "content_type": type, "detail_info": value, "position": position
                };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                console.info("logMessage : contactDataId " + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage : contact_data name insert error = " + error);
            }
            return contactDataId;
        }

        /**
     * @tc.number  contactCard_insert_test_100
     * @tc.name    Insert contact data
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_100", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_100 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, common.getProfileRawContact());
                sleep(1000);
                console.info('logMessage contactCard_insert_test_100: rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var map = common.getProfileRawContactMap();
                map.set("id", rawContactId.toString());
                await contactsQuery(map, "contactCard_insert_test_100", profileRawContactUri);
                await deleteRawContact("contactCard_insert_test_100");
                done();
            } catch (error) {
                console.info('logMessage contactCard_insert_test_100: insert error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insert_test_400
     * @tc.name    Insert contact details (name, company, position, mobile phone number)
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_400", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_400 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocheng",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insert_test_400: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await dataNameInsert(DAHelper, rawContactId);
                await dataCompanyInsert(DAHelper, rawContactId);
                await dataPhoneInsert(DAHelper, rawContactId);
                await deleteRawContact("contactCard_insert_test_400");
                await deleteAll(profileContactDataUri, "contactCard_insert_test_400");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_400: raw_contact insert error = " + error);
                done();
            }
        });

        async function dataNameInsert(DAHelper, rawContactId) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactId,
                "content_type": "name",
                "detail_info": "xiaocheng"
            };
            try {
                var contactDataId1 = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                console.info("logMessage contactCard_insert_test_400: contactDataId1 " + contactDataId1);
                expect(contactDataId1 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId1.toString());
                map.set("detail_info", "xiaocheng");
                map.set("raw_contact_id", rawContactId.toString());
                await contactsQuery(map, "contactCard_insert_test_400", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_400: contact_data name insert error = " + error);
                done();
            }
        }

        async function dataCompanyInsert(DAHelper, rawContactId) {
            var contactDataValues2 = {
                "raw_contact_id": rawContactId,
                "content_type": "organization",
                "detail_info": "TT",
                "position": "developer"
            };
            try {
                var contactDataId2 = await DAHelper.insert(profileContactDataUri, contactDataValues2);
                console.info("logMessage contactCard_insert_test_400: contactDataId2 " + contactDataId2);
                expect(contactDataId2 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId2.toString());
                map.set("detail_info", "TT");
                map.set("position", "developer");
                contactsQuery(map, "contactCard_insert_test_400", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_400: contact_data company insert error = " + error);
                done();
            }
        }

        async function dataPhoneInsert(DAHelper, rawContactId) {
            var contactDataValues3 = {
                "raw_contact_id": rawContactId,
                "content_type": "phone",
                "detail_info": "12345678"
            };
            try {
                var contactDataId3 = await DAHelper.insert(profileContactDataUri, contactDataValues3);
                sleep(1000);
                console.info("logMessage contactCard_insert_test_400: contactDataId3 " + contactDataId3);
                expect(contactDataId3 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId3.toString());
                map.set("detail_info", "12345678");
                await contactsQuery(map, "contactCard_insert_test_400", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_400: contact_data phone insert error = " + error);
                done();
            }
        }

        /**
     * @tc.number  contactCard_insert_test_500
     * @tc.name    Insert contact details (nickname, email, home)
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_500", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_500 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocheng",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insert_test_500: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await dataEmailInsert(DAHelper, rawContactId);
                await dataNickNameInsert(DAHelper, rawContactId);
                await dataPostalAddressInsert(DAHelper, rawContactId);
                await deleteRawContact("contactCard_insert_test_500");
                await deleteAll(profileContactDataUri, "contactCard_insert_test_500");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_500: raw_contact insert error = " + error);
                done();
            }
        });

        async function dataEmailInsert(DAHelper, rawContactId) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactId,
                "content_type": "email",
                "detail_info": "166@163.com"
            };
            try {
                var contactDataId1 = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                sleep(1000);
                console.info("logMessage contactCard_insert_test_500: contactDataId1 " + contactDataId1);
                expect(contactDataId1 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId1.toString());
                map.set("detail_info", "166@163.com");
                await contactsQuery(map, "contactCard_insert_test_500_1", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_500: contact_data email insert error = " + error);
            }
        }

        async function dataNickNameInsert(DAHelper, rawContactId) {
            var contactDataValues2 = {
                "raw_contact_id": rawContactId,
                "content_type": "nickname",
                "detail_info": "xiaocheng"
            };
            try {
                var contactDataId2 = await DAHelper.insert(profileContactDataUri, contactDataValues2);
                console.info("logMessage contactCard_insert_test_500: contactDataId2 " + contactDataId2);
                expect(contactDataId2 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId2.toString());
                map.set("detail_info", "xiaocheng");
                await contactsQuery(map, "contactCard_insert_test_500_2", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_500: contact_data nickname insert error = " + error);
            }
        }

        async function dataPostalAddressInsert(DAHelper, rawContactId) {
            var contactDataValues3 = {
                "raw_contact_id": rawContactId,
                "content_type": "postal_address",
                "detail_info": "NanJingSoftRoad"
            };
            try {
                var contactDataId3 = await DAHelper.insert(profileContactDataUri, contactDataValues3);
                console.info("logMessage contactCard_insert_test_500: contactDataId3 " + contactDataId3);
                expect(contactDataId3 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId3.toString());
                map.set("detail_info", "NanJingSoftRoad");
                await contactsQuery(map, "contactCard_insert_test_500_3", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_500: contact_data postal_address insert error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_insert_test_600
     * @tc.name    Insert contact details (remarks, AIM, birthday)
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_600", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_600 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocheng",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues)
                console.info("logMessage contactCard_insert_test_600: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await dataNoteInsert(DAHelper, rawContactId);
                await dataAimInsert(DAHelper, rawContactId);
                await dataEventInsert(DAHelper, rawContactId);
                await deleteRawContact("contactCard_insert_test_600");
                await deleteAll(profileContactDataUri, "contactCard_insert_test_600");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_600: raw_contact insert error = " + error);
                done();
            }
        });

        async function dataNoteInsert(DAHelper, rawContactId) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactId,
                "content_type": "note",
                "detail_info": "javaGangster"
            };
            try {
                var contactDataId1 = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                console.info("logMessage contactCard_insert_test_600: contactDataId1 " + contactDataId1);
                expect(contactDataId1 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId1.toString());
                map.set("detail_info", "javaGangster");
                await contactsQuery(map, "contactCard_insert_test_600_1", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_600: contact_data note insert error = " + error);
                done();
            }
        }

        async function dataAimInsert(DAHelper, rawContactId) {
            var contactDataValues2 = {
                "raw_contact_id": rawContactId, "content_type": "im", "detail_info": "aaaaa"
            };
            try {
                var contactDataId2 = await DAHelper.insert(profileContactDataUri, contactDataValues2);
                console.info("logMessage contactCard_insert_test_600: contactDataId2 " + contactDataId2);
                expect(contactDataId2 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId1.toString());
                map.set("detail_info", "aaaaa");
                await contactsQuery(map, "contactCard_insert_test_600_2", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_600: contact_data aim insert error = " + error);
            }
        }

        async function dataEventInsert(DAHelper, rawContactId) {
            var contactDataValues3 = {
                "raw_contact_id": rawContactId,
                "content_type": "contact_event",
                "detail_info": "19960229"
            };
            try {
                var contactDataId3 = await DAHelper.insert(profileContactDataUri, contactDataValues3);
                console.info("logMessage contactCard_insert_test_600: contactDataId3 " + contactDataId3);
                expect(contactDataId3 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId1.toString());
                map.set("detail_info", "19960229");
                await contactsQuery(map, "contactCard_insert_test_600_3", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_600: contact_data event insert error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_insert_test_700
     * @tc.name    Insert contact details (URL, assistant, group)
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_700", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_700 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocheng",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insert_test_700: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await dataWebsiteInsert(DAHelper, rawContactId);
                await dataRelationInsert(DAHelper, rawContactId);
                await dataWebsiteInsert(DAHelper, rawContactId);
                await deleteRawContact("contactCard_insert_test_700");
                await deleteAll(profileContactDataUri, "contactCard_insert_test_700");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_700: raw_contact insert error = " + error);
                done();
            }
        });

        async function dataWebsiteInsert(DAHelper, rawContactId) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactId,
                "content_type": "website",
                "detail_info": "www.com"
            };
            try {
                var contactDataId1 = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                sleep(1000);
                console.info("logMessage contactCard_insert_test_700: contactDataId1 " + contactDataId1);
                expect(contactDataId1 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId1.toString());
                map.set("detail_info", "www.com");
                await contactsQuery(map, "contactCard_insert_test_700_1", profileContactDataUri);
                await contactDataRelationInsert();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_700: contact_data website insert error = " + error);
            }
        }

        async function dataRelationInsert(DAHelper, rawContactId) {
            var contactDataValues2 = {
                "raw_contact_id": rawContactId,
                "content_type": "relation",
                "detail_info": "Secretary"
            };
            try {
                var contactDataId2 = await DAHelper.insert(profileContactDataUri, contactDataValues2);
                sleep(1000);
                console.info("logMessage contactCard_insert_test_700: contactDataId2 " + contactDataId2);
                expect(contactDataId2 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId2.toString());
                map.set("detail_info", "Secretary");
                await contactsQuery(map, "contactCard_insert_test_700_2", profileContactDataUri);
                await contactDataGroupInsert();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_700: contact_data relation insert error = " + error);
            }
        }

        async function dataGroupInsert(DAHelper, rawContactId) {
            var contactDataValues3 = {
                "raw_contact_id": rawContactId,
                "content_type": "group_membership",
                "detail_info": "1"
            };
            try {
                var contactDataId3 = await DAHelper.insert(profileContactDataUri, contactDataValues3);
                sleep(1000);
                console.info("logMessage contactCard_insert_test_700: contactDataId3 " + contactDataId3);
                expect(contactDataId3 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId3.toString());
                map.set("detail_info", "1");
                await contactsQuery(map, "contactCard_insert_test_700_3", profileContactDataUri);
                done();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_700: contact_data group insert error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_insert_test_800
     * @tc.name    Insert contact details (phone ringtone)
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_800", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_800 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocheng",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insert_test_800: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await dataMiscInsert(DAHelper, rawContactId);
                await deleteRawContact("contactCard_insert_test_800");
                await deleteAll(profileContactDataUri, "contactCard_insert_test_800");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_800: raw_contact insert error = " + error);
                done();
            }
        });

        async function dataMiscInsert(DAHelper, rawContactId) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactId,
                "content_type": "contact_misc",
                "detail_info": "1234"
            };
            try {
                var contactDataId1 = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                sleep(1000);
                console.info("logMessage contactCard_insert_test_800: contactDataId1 " + contactDataId1);
                expect(contactDataId1 > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId1.toString());
                map.set("detail_info", "1234");
                await contactsQuery(map, "contactCard_insert_test_800_1", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_800: contact_data misc insert error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_900
     * @tc.name    Update contact data
     * @tc.desc    Function test
     */
        it("contactCard_update_test_900", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_900 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "licheng",
                "company": "TT",
                "position": "developer",
                "favorite": 1,
                "phonetic_name": "licheng||lc"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_900: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await rawContactUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_900");
                await deleteAll(profileContactDataUri, "contactCard_update_test_900");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_900: raw_contact insert error = " + error);
                done();
            }
        });

        async function rawContactUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "display_name": "dacheng",
                "company": "oo",
                "position": "Testers",
                "favorite": 0,
                "phonetic_name": "dacheng||dc"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileRawContactUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_900: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactId.toString());
                map.set("display_name", "dacheng");
                map.set("company", "oo");
                map.set("position", "Testers");
                map.set("favorite", "0");
                map.set("phonetic_name", "dacheng||dc");
                await contactsQuery(map, "contactCard_update_test_900", profileRawContactUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_900: raw_contact update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_1200
     * @tc.name    Update contact details (name)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_1200", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_1200 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_1200: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforedataNameInsert(DAHelper, rawContactId);
                await dataNameUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_1200");
                await deleteAll(profileContactDataUri, "contactCard_update_test_1200");
                done();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1200: raw_contact insert error = ' + error);
                done();
            }
        });

        async function beforedataNameInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId, "content_type": "name", "detail_info": "xiaoyuan"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1200: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1200: contact_data insert error = ' + error);
            }
        }

        async function dataNameUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "dayuan"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1200: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactId.toString());
                map.set("display_name", "dayuan");
                await contactsQuery(map, "contactCard_update_test_1200", profileRawContactUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_1200: contact_data update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_1300
     * @tc.name    Update contact details (company, position)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_1300", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_1300 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_1300: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforedataCompanyInsert(DAHelper, rawContactId);
                await dataCompanyUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_1300");
                await deleteAll(profileContactDataUri, "contactCard_update_test_1300");
                done();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1300: raw_contact insert error = ' + error);
                done();
            }
        });

        async function beforedataCompanyInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "organization",
                "detail_info": "TT",
                "position": "Testers",
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1300: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1300: contact_data insert error = ' + error);
            }
        }

        async function dataCompanyUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "OOOO", "position": "developer"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1300: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactId.toString());
                map.set("company", "OOOO");
                map.set("position", "developer");
                await contactsQuery(map, "contactCard_update_test_1300", profileRawContactUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_1300: contact_data update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_1400
     * @tc.name    Update contact details (mobile phone number)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_1400", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_1400 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_1400: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforeDataPhoneInsert(DAHelper, rawContactId);
                await dataPhoneUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_1400");
                await deleteAll(profileContactDataUri, "contactCard_update_test_1400");
                done();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1400: raw_contact insert error = ' + error);
                done();
            }
        });

        async function beforeDataPhoneInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId, "content_type": "phone", "detail_info": "123456"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1400: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1400: contact_data insert error = ' + error);
            }
        }

        async function dataPhoneUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "99663355"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1400: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "99663355");
                await contactsQuery(map, "contactCard_update_test_1400", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_1400: contact_data update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_1500
     * @tc.name    Update contact details (nickname)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_1500", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_1500 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_1500: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforedataNickNameInsert(DAHelper, rawContactId);
                await dataNickNameUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_1500");
                await deleteAll(profileContactDataUri, "contactCard_update_test_1500");
                done();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1500: raw_contact insert error = ' + error);
                done();
            }
        });

        async function beforedataNickNameInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "nickname",
                "detail_info": "xiaoyuan"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1500: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1500: contact_data insert error = ' + error);
            }
        }

        async function dataNickNameUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "fengyuan"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1500: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "fengyuan");
                await contactsQuery(map, "contactCard_update_test_1500", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_1500: contact_data update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_1600
     * @tc.name    Update contact details (email)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_1600", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_1600 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_1600: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforeDataEmailInsert(DAHelper, rawContactId);
                await dataEmailUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_1600");
                await deleteAll(profileContactDataUri, "contactCard_update_test_1600");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_1600: raw_contact insert error = " + error);
                done();
            }
        });

        async function beforeDataEmailInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "email",
                "detail_info": "166@163.com"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1600: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1600: contact_data insert error = ' + error);
            }
        }

        async function dataEmailUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "199@163.com"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition)
                sleep(1000);
                console.info('logMessage contactCard_update_test_1600: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "199@163.com");
                await contactsQuery(map, "contactCard_update_test_1600", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_1600: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_1700
     * @tc.name    Update Contact Details (Residential)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_1700", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_1700 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_1700: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforeDataPostalInsert(DAHelper, rawContactId);
                await dataPostalUpdate();
                await deleteRawContact("contactCard_update_test_1700");
                await deleteAll(profileContactDataUri, "contactCard_update_test_1700");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_1700: raw_contact insert error = " + error);
                done();
            }
        });

        async function beforeDataPostalInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "postal_address",
                "detail_info": "NanJing"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1700: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_update_test_1700: contact_data insert error = " + error);
            }
        }

        async function dataPostalUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "BeiJing"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1700: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "BeiJing");
                await contactsQuery(map, "contactCard_update_test_1700", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_1700: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_1800
     * @tc.name    Update contact details (remarks)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_1800", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_1800 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_1800: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforedataNoteInsert(DAHelper, rawContactId);
                await dataNoteUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_1800");
                await deleteAll(profileContactDataUri, "contactCard_update_test_1800");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_1800: raw_contact insert error = " + error);
                done();
            }
        });

        async function beforedataNoteInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId, "content_type": "note", "detail_info": "Gangster"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1800: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_1800: contact_data insert error = ' + error);
            }
        }

        async function dataNoteUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "God"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1800: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "God");
                await contactsQuery(map, "contactCard_update_test_1800", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_1800: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_1900
     * @tc.name    Update Contact Details (AIM)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_1900", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_1900 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues)
                console.info("logMessage contactCard_update_test_1900: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforeDataAimInsert(DAHelper, rawContactId);
                await dataAimUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_1900");
                await deleteAll(profileContactDataUri, "contactCard_update_test_1900");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_1900: raw_contact insert error = " + error);
                done();
            }
        });

        async function beforeDataAimInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId, "content_type": "im", "detail_info": "aaaaa"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1900: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_update_test_1900: contact_data insert error = " + error);
            }
        }

        async function dataAimUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "bbbbb"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_1900: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "bbbbb");
                await contactsQuery(map, "contactCard_update_test_1900", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_1900: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_2000
     * @tc.name    Update Contact Details (Birthday)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_2000", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_2000 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_2000: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforeDataEventInsert(DAHelper, rawContactId);
                await dataEventUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_2000");
                await deleteAll(profileContactDataUri, "contactCard_update_test_2000");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_2000: raw_contact insert error = " + error);
                done();
            }
        });

        async function beforeDataEventInsert(DAHelper, rawContactId) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactId,
                "content_type": "contact_event",
                "detail_info": "19960229"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                sleep(1000);
                console.info('logMessage contactCard_update_test_2000: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_2000: contact_data insert error = ' + error);
            }
        }

        async function dataEventUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "19971021"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_2000: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "19971021");
                await contactsQuery(map, "contactCard_update_test_2000", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_2000: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_2100
     * @tc.name    Update contact details (website)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_2100", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_2100 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_2100: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforeDataWebsiteInsert(DAHelper, rawContactId);
                await dataWebsiteUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_2100");
                await deleteAll(profileContactDataUri, "contactCard_update_test_2100");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_2100: raw_contact insert error = " + error);
                done();
            }
        });

        async function beforeDataWebsiteInsert(DAHelper, rawContactId) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactId,
                "content_type": "website",
                "detail_info": "www.com"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                sleep(1000);
                console.info('logMessage contactCard_update_test_2100: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_2100: contact_data insert error = ' + error);
            }
        }

        async function dataWebsiteUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "www.123.com"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_2100: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "www.123.com");
                await contactsQuery(map, "contactCard_update_test_2100", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_2100: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_2200
     * @tc.name    Update contact details (assistant)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_2200", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_2200 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_2200: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforeDataRelationInsert(DAHelper, rawContactId);
                await dataRelationUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_2200");
                await deleteAll(profileContactDataUri, "contactCard_update_test_2200");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_2200: raw_contact insert error = " + error);
                done();
            }
        });

        async function beforeDataRelationInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "relation",
                "detail_info": "Secretary"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                console.info('logMessage contactCard_update_test_2200: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_2200: contact_data insert error = ' + error);
            }
        }

        async function dataRelationUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "spouse"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_2200: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "spouse");
                await contactsQuery(map, "contactCard_update_test_2200", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_2200: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_2300
     * @tc.name    Update contact detail data table (group)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_2300", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_2300 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_update_test_2300: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforeDataGroupInsert(DAHelper, rawContactId);
                await dataGroupUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_2300");
                await deleteAll(profileContactDataUri, "contactCard_update_test_2300");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_2300: raw_contact insert error = " + error);
                done();
            }
        });

        async function beforeDataGroupInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "group_membership",
                "detail_info": "2"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info('logMessage contactCard_update_test_2300: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_2300: contact_data insert error = ' + error);
            }
        }

        async function dataGroupUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "7"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_2300: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "3");
                await contactsQuery(map, "contactCard_update_test_2300", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_2300: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_update_test_2400
     * @tc.name    Update contact detail data table (phone ringtone)
     * @tc.desc    Function test
     */
        it("contactCard_update_test_2400", 0, async function (done) {
            console.info("------logMessage contactCard_update_test_2400 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoyuan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_update_test_2400: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await beforeDataMiscInsert(DAHelper, rawContactId);
                await dataMiscUpdate(DAHelper, rawContactId);
                await deleteRawContact("contactCard_update_test_2400");
                await deleteAll(profileContactDataUri, "contactCard_update_test_2400");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_2400: raw_contact insert error = " + error);
                done();
            }
        });

        async function beforeDataMiscInsert(DAHelper, rawContactId) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactId,
                "content_type": "contact_misc",
                "detail_info": "1234"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                sleep(1000);
                console.info('logMessage contactCard_update_test_2400: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage contactCard_update_test_2400: contact_data insert error = ' + error);
            }
        }

        async function dataMiscUpdate(DAHelper, rawContactId) {
            var updateValues = {
                "detail_info": "999"
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                sleep(1000);
                console.info('logMessage contactCard_update_test_2400: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "999");
                await contactsQuery(map, "contactCard_update_test_2300", profileContactDataUri);
            } catch (error) {
                console.info('logMessage contactCard_update_test_2400: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_favorite_test_2500
     * @tc.name    Favorite contacts
     * @tc.desc    Function test
     */
        it("contactCard_favorite_test_2500", 0, async function (done) {
            console.info("-------logMessage contactCard_favorite_test_2500 is starting!------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaocai", "company": "TT", "position": "Testers"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues)
                console.info('logMessage contactCard_favorite_test_2500: rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await updateFavorite(DAHelper, rawContactId);
                await deleteRawContact("contactCard_favorite_test_2500");
                done();
            } catch (error) {
                console.info('logMessage contactCard_favorite_test_2500: raw_contact insert error = ' + error);
                done();
            }
        });

        async function updateFavorite(DAHelper, rawContactId) {
            var updateValues = {
                "favorite": 1
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileRawContactUri, updateValues, condition);
                sleep(1000);
                console.info("logMessage contactCard_favorite_test_2500: updateCode = " + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactId.toString());
                map.set("display_name", "xiaocai");
                map.set("company", "TT");
                map.set("position", "Testers");
                map.set("favorite", "1");
                await contactsQuery(map, "contactCard_favorite_test_2500", profileRawContactUri);
            } catch (error) {
                console.info('logMessage contactCard_favorite_test_2500: update error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_unfavorite_test_2600
     * @tc.name    Unfavorite contacts
     * @tc.desc    Function test
     */
        it("contactCard_unfavorite_test_2600", 0, async function (done) {
            console.info("-------logMessage contactCard_unfavorite_test_2600 is starting!------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var rawContactValues = {
                "display_name": "xiaohong", "company": "TT", "position": "Testers", "favorite": 1
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                console.info('logMessage contactCard_unfavorite_test_2600: rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await updateUnFavorite(DAHelper, rawContactId);
                await deleteRawContact("contactCard_unfavorite_test_2600");
                done();
            } catch (error) {
                console.info('logMessage contactCard_unfavorite_test_2600: raw_contact insert error = ' + error);
                done();
            }
        });

        async function updateUnFavorite(DAHelper, rawContactId) {
            var updateValues = {
                "favorite": 0
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactId.toString());
            try {
                var updateCode = await DAHelper.update(profileRawContactUri, updateValues, condition);
                sleep(1000);
                console.info("logMessage contactCard_unfavorite_test_2600: updateCode = " + updateCode);
                expect(updateCode == 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactId.toString());
                map.set("display_name", "xiaohong");
                map.set("company", "TT");
                map.set("position", "Testers");
                map.set("favorite", "0");
                await contactsQuery(map, "contactCard_favorite_test_2500", profileRawContactUri);
            } catch (error) {
                console.info("logMessage contactCard_unfavorite_test_2600: update error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_insert_test_2700
     * @tc.name    Insert contact black list
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_2700", 0, async function (done) {
            console.info("-------logMessage contactCard_insert_test_2700 is starting!------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var blocklistId = await DAHelper.insert(profileBlocklistUri, common.getProfileBlockList());
                sleep(1000);
                console.info('logMessage contactCard_insert_test_2700: blocklistId = ' + blocklistId);
                expect(blocklistId > 0).assertTrue();
                var map = common.getProfileBlockListMap();
                map.set("id", blocklistId.toString());
                await contactsQuery(map, "contactCard_insert_test_2700", profileBlocklistUri);
                await deleteAll(profileBlocklistUri, "contactCard_insert_test_2700");
                done();
            } catch (error) {
                console.info('logMessage contactCard_insert_test_2700: blocklist insert error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_delete_test_2800
     * @tc.name    Delete contact black list
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_2800", 0, async function (done) {
            console.info("-------logMessage contactCard_delete_test_2800 is starting!------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var blocklistId = await DAHelper.insert(profileBlocklistUri, common.getProfileBlockList());
                sleep(1000);
                console.info('logMessage contactCard_delete_test_2800: blocklistId = ' + blocklistId);
                expect(blocklistId > 0).assertTrue();
                await blocklistDelete();
                done();
            } catch (error) {
                console.info('logMessage contactCard_delete_test_2800: blocklist insert error = ' + error);
                done();
            }

            async function blocklistDelete() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", blocklistId.toString());
                try {
                    var deleteCode = await DAHelper.delete(profileBlocklistUri, condition);
                    sleep(1000);
                    console.info("logMessage contactCard_delete_test_2800: deleteCode = " + deleteCode);
                    expect(deleteCode == 0).assertTrue();
                    var map = new Map();
                    map.set("id", blocklistId.toString());
                    await queryIdForDelete(map, "contactCard_delete_test_2800", profileBlocklistUri);
                } catch (error) {
                    console.info('logMessage contactCard_delete_test_2800: delete error = ' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  contactCard_insert_test_2900
     * @tc.name    Insert contact group
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_2900", 0, async function (done) {
            console.info("-------logMessage contactCard_insert_test_2900 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var groupValues = {
                "group_name": "TestersThird"
            };
            try {
                var groupId = await DAHelper.insert(profileGroupUri, groupValues);
                sleep(1000);
                console.info("logMessage contactCard_insert_test_2900: groupId =" + groupId);
                expect(groupId > 0).assertTrue();
                await groupRawContactInsert(DAHelper);
                await deleteRawContact("contactCard_insert_test_2900");
                await deleteAll(profileGroupUri, "contactCard_insert_test_2900");
                await deleteAll(profileContactDataUri, "contactCard_insert_test_2900");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_2900: group insert error =" + error);
                done();
            }
        });

        async function groupRawContactInsert(DAHelper) {
            var rawContactValues = {
                "display_name": "liyu"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_insert_test_2900: rawContactId =" + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await groupDataInsert(DAHelper, rawContactId);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_2900: raw_contact insert error =" + error);
            }
        }

        async function groupDataInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "group_membership",
                "detail_info": groupId.toString()
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues)
                sleep(1000);
                console.info("logMessage contactCard_insert_test_2900: contactDataId =" + contactDataId);
                expect(contactDataId > 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId);
                map.set("raw_contact_id", rawContactId.toString());
                map.set("detail_info", groupId.toString());
                await contactsQuery(map, "contactCard_insert_test_2900", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_insert_test_2900: contact_data insert error =" + error);
            }
        }

        /**
     * @tc.number  contactCard_delete_test_3000
     * @tc.name    Delete contact group
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_3000", 0, async function (done) {
            console.info("-------logMessage contactCard_delete_test_3000 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var groupValues = {
                "group_name": "TestersFourth"
            };
            try {
                var groupId = await DAHelper.insert(profileGroupUri, groupValues);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_3000: groupId =" + groupId);
                expect(groupId > 0).assertTrue();
                await groupAfterRawContactInsert(DAHelper);
                await deleteRawContact("contactCard_delete_test_3000");
                await deleteAll(profileGroupUri, "contactCard_delete_test_3000");
                await deleteAll(profileContactDataUri, "contactCard_delete_test_3000");
                done();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_3000: group insert error =" + error);
                done();
            }
        });

        async function groupAfterRawContactInsert(DAHelper) {
            var rawContactValues = {
                "display_name": "liyuchen"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_3000: rawContactId =" + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await deleteGroupContactDataInsert(DAHelper, rawContactId);
            } catch (error) {
                console.info("logMessage contactCard_delete_test_3000: raw_contact insert error =" + error);
            }
        }

        async function deleteGroupContactDataInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "group_membership",
                "detail_info": groupId.toString()
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                console.info("logMessage contactCard_delete_test_3000: contactDataId =" + contactDataId);
                expect(contactDataId > 0).assertTrue();
                await deleteGroup(DAHelper, contactDataId);
            } catch (error) {
                console.info("logMessage contactCard_delete_test_3000: contact_data insert error =" + error);
            }
        }

        async function deleteGroup(DAHelper, contactDataId) {
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", contactDataId.toString());
            try {
                var deleteCode = await DAHelper.delete(profileContactDataUri, condition);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_3000: deleteCode =" + deleteCode);
                expect(deleteCode == 0).assertTrue();
                var map = new Map();
                map.set("id", contactDataId.toString());
                await queryIdForDelete(map, "contactCard_delete_test_3000", profileContactDataUri);
            } catch (error) {
                console.info("logMessage contactCard_delete_test_3000: delete error =" + error);
            }
        }

        /**
     * @tc.number  contactCard_query_test_3100
     * @tc.name    Query basic information of a single contact
     * @tc.desc    Function test
     */
        it("contactCard_query_test_3100", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_3100 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoyu"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3100: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await rawContactQuery(DAHelper, rawContactId);
                await deleteRawContact("contactCard_query_test_3100");
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3100: raw_contact insert error = " + error);
                done();
            }
        });

        async function rawContactQuery(DAHelper, rawContactId) {
            var resultColumns = ["id", "display_name"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                sleep(2000);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_query_test_3100: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == rawContactId).assertEqual(true);
                        console.info('logMessage contactCard_query_test_3100: displayName = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == "xiaoyu").assertEqual(true);
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3100: query error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_query_test_3200
     * @tc.name    Query basic information of multiple contacts
     * @tc.desc    Function test
     */
        it("contactCard_query_test_3200", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_3200 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues1 = {
                "display_name": "xiaoyu"
            };
            try {
                var rawContactIdFirst = await DAHelper.insert(profileRawContactUri, rawContactValues1);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3200: rawContactId = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3200: raw_contact_1 insert error = " + error);
                done();
            }

            var rawContactValues2 = {
                "display_name": "xiaohong"
            };
            try {
                var rawContactIdSecond = await DAHelper.insert(profileRawContactUri, rawContactValues2);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3200: rawContactId = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
                await queryTwoRaw(DAHelper, rawContactIdFirst, rawContactIdSecond);
                await deleteRawContact("contactCard_query_test_3200");
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3200: raw_contact_2 insert error = " + error);
                done();
            }
        });

        async function queryTwoRaw(DAHelper, rawContactIdFirst, rawContactIdSecond) {
            var resultColumns = ["id", "display_name"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactIdFirst.toString());
            condition.or();
            condition.equalTo("id", rawContactIdSecond.toString());
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                sleep(1000);
                console.info('logMessage contactCard_query_test_3200: goToFirstRow' + resultSet.goToFirstRow());
                expect(resultSet.goToFirstRow()).assertEqual(true);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info("logMessage contactCard_query_test_3200: columnNames :" + resultSet.columnNames);
                        console.info("logMessage contactCard_query_test_3200: columnCount:" + resultSet.columnCount);
                        console.info('logMessage contactCard_query_test_3200: id = ' + resultSet.getString(0));
                        console.info('logMessage contactCard_query_test_3200: displayName = ' + resultSet.getString(1));
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_query_test_3200: query error' + error);
            }
        }

        /**
     * @tc.number  contactCard_query_test_3300
     * @tc.name    Query basic information of all contacts
     * @tc.desc    Function test
     */
        it("contactCard_query_test_3300", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_3300 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues1 = {
                "display_name": "xiaoyu"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues1);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3300: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3300: raw_contact_1 insert error = " + error);
                done();
            }
            var resultColumns = ["id", "display_name"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_query_test_3300: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == rawContactId.toString()).assertEqual(true);
                        console.info('logMessage contactCard_query_test_3300: displayName = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == "xiaoyu").assertEqual(true);
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
                await deleteRawContact("contactCard_query_test_3300");
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3300: query error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_query_test_3400
     * @tc.name    Query all information of a single contact
     * @tc.desc    Function test
     */
        it("contactCard_query_test_3400", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_3400 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoyuzhou",
                "company": "TT",
                "position": "Testers",
                "favorite": 1,
                "phonetic_name": "xiaoyu||xy"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3400: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await queryAllInformation(DAHelper, rawContactId);
                await deleteRawContact("contactCard_query_test_3400");
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3400: raw_contact insert error = " + error);
                done();
            }
        });

        async function queryAllInformation(DAHelper, rawContactId) {
            var resultColumns = [];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    do {
                        var id = resultSet.getString(resultSet.getColumnIndex("id"));
                        console.info('logMessage contactCard_query_test_3400: id = ' + id);
                        expect(id == rawContactId).assertEqual(true);
                        var favorite = resultSet.getString(resultSet.getColumnIndex("favorite"));
                        console.info('logMessage contactCard_query_test_3400: favorite = ' + favorite);
                        expect(favorite == "1").assertEqual(true);
                        var phoneticName = resultSet.getString(resultSet.getColumnIndex("phonetic_name"));
                        console.info('logMessage contactCard_query_test_3400: phoneticName = ' + phonetic_name);
                        expect(phoneticName == "xiaoyu||xy").assertEqual(true);
                        var company = resultSet.getString(resultSet.getColumnIndex("company"));
                        console.info('logMessage contactCard_query_test_3400: company = ' + company);
                        expect(company == "TT").assertEqual(true);
                        var displayName = resultSet.getString(resultSet.getColumnIndex("display_name"));
                        console.info('logMessage contactCard_query_test_3400: displayName = ' + displayName);
                        expect(displayName == "xiaoyuzhou").assertEqual(true);
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_query_test_3400: query error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_query_test_3500
     * @tc.name    Query group member ability
     * @tc.desc    Function test
     */
        it("contactCard_query_test_3500", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_3500 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var rawContactValues = {
                "display_name": "xiaoyuzhou"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3500: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3500: raw_contact insert error = " + error);
                done();
            }
            var groupValues = {
                "group_name": "BossBoard3500"
            };
            try {
                var groupId = await DAHelper.insert(profileGroupUri, groupValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3500: groupId = " + groupId);
                expect(groupId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3500: group insert error = " + error);
                done();
            }
            await groupBossBoardInsert(DAHelper, rawContactId, groupId);
            await deleteRawContact("contactCard_query_test_3500");
            await deleteAll(profileGroupUri, "contactCard_query_test_3500");
            await deleteAll(profileContactDataUri, "contactCard_query_test_3500");
            done();
        });

        async function groupBossBoardInsert(DAHelper, rawContactId, groupId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "group_membership",
                "detail_info": groupId.toString()
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3500: contactDataId = " + contactDataId);
                expect(contactDataId > 0).assertTrue();
                await groupBossBoardQuery(DAHelper, rawContactId, groupId);
            } catch (error) {
                console.info("logMessage contactCard_query_test_3500: query error = " + error);
            }
        }

        async function groupBossBoardQuery(DAHelper, rawContactId, groupId) {
            var resultColumns = ["raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            // type_id = 9 data is group
            condition.equalTo("type_id", "9");
            condition.and();
            condition.equalTo("detail_info", groupId.toString());
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_query_test_3500: raw_contact_id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == rawContactId).assertEqual(true);
                        console.info('logMessage contactCard_query_test_3500: detail_info = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == groupId.toString()).assertEqual(true);
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3500: query error :" + error);
            }
        }

        /**
     * @tc.number  contactCard_query_test_3600
     * @tc.name    Query favorite contacts
     * @tc.desc    Function test
     */
        it("contactCard_query_test_3600", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_3600 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var rawContactValues = {
                "display_name": "xiaoyuzhou", "favorite": 1
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3600: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await queryFavorite();
                await deleteRawContact("contactCard_query_test_3600");
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3600: raw_contact insert error = " + error);
                done();
            }

            async function queryFavorite() {
                var resultColumns = ["id", "display_name"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("favorite", "1");
                condition.and();
                condition.equalTo("is_deleted", "0");
                try {
                    var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                    sleep(1000);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info('logMessage contactCard_query_test_3600: id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == rawContactId.toString()).assertTrue();
                            console.info(
                                'logMessage contactCard_query_test_3600: displayName = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "xiaoyuzhou").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info("logMessage contactCard_query_test_3600: query error :" + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  contactCard_query_test_3700
     * @tc.name    Query recent contact ability
     * @tc.desc    Function test
     */
        it("contactCard_query_test_3700", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_3700 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var rawContactValues = {
                "display_name": "xiaoyuzhou", "lastest_contacted_time": 60
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3700: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await queryContacted();
                await deleteRawContact("contactCard_query_test_3700");
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3700: raw_contact insert error = " + error);
                done();
            }

            async function queryContacted() {
                var resultColumns = ["id", "display_name"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.lessThan("lastest_contacted_time", "100");
                condition.and();
                condition.equalTo("is_deleted", "0");
                try {
                    var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                    sleep(1000);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info('logMessage contactCard_query_test_3700: id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == rawContactId.toString()).assertTrue();
                            console.info(
                                'logMessage contactCard_query_test_3700: displayName = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "xiaoyuzhou").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info('logMessage contactCard_query_test_3700: query error' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  contactCard_query_test_3800
     * @tc.name    Query recently deleted contacts
     * @tc.desc    Function test
     */
        it("contactCard_query_test_3800", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_3800 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var rawContactValues = {
                "display_name": "xiaocai"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3800: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await rawContactDelete(DAHelper, rawContactId);
                await deletedRawContactQuery(DAHelper, rawContactId);
                await deleteAll(profileDeletedContactsUri, "contactCard_query_test_3800")
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3800: raw_contact insert error = " + error);
                done();
            }
        });

        async function rawContactDelete(DAHelper, rawContactId) {
            var condition1 = new ohosDataAbility.DataAbilityPredicates();
            condition1.equalTo("id", rawContactId.toString());
            try {
                var deleteCode = await DAHelper.delete(profileRawContactUri, condition1);
                sleep(1000);
                console.info('logMessage contactCard_query_test_3800: deleteCode = ' + deleteCode);
                expect(deleteCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3800: raw_contact delete error = " + error);
                done();
            }
        }

        async function deletedRawContactQuery(DAHelper, rawContactId) {
            var resultColumns = ["id", "raw_contact_id", "display_name", "is_deleted"];
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            condition2.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(deletedprofileRawContactUri, resultColumns, condition2);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    console.info('logMessage contactCard_query_test_3800: id = ' + resultSet.getString(0));
                    console.info('logMessage contactCard_query_test_3800: raw_contact_id = ' + resultSet.getString(1));
                    expect(resultSet.getString(1) == rawContactId.toString()).assertTrue();
                    console.info('logMessage contactCard_query_test_3800: displayName = ' + resultSet.getString(2));
                    expect(resultSet.getString(2) == "xiaocai").assertTrue();
                    console.info('logMessage contactCard_query_test_3800: is_deleted = ' + resultSet.getString(3));
                    expect(resultSet.getString(3) == "1").assertTrue();
                }
                resultSet.close();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3800: query error :" + error);
                done();
            }
        }

        /**
     * @tc.number  contactCard_query_test_3900
     * @tc.name    Query all mobile phone numbers
     * @tc.desc    Function test
     */
        it("contactCard_query_test_3900", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_3900 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var rawContactValues = {
                "display_name": "xiaocai"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3900: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await queryPhoneInsert();
                await phoneQuery(rawContactId);
                await deleteRawContact("contactCard_query_test_3900");
                await deleteAll(profileContactDataUri, "contactCard_query_test_3900");
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3900: raw_contact insert error = " + error);
                done();
            }
        });

        async function queryPhoneInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "phone",
                "detail_info": "1853696321"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_3900: contactDataId = " + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_query_test_3900: contact_data insert error = " + error);
            }
        }

        async function phoneQuery(DAHelper, rawContactId) {
            var resultColumns = ["raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            // type_id = 5 data is phone
            condition.equalTo("content_type", "phone");
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_query_test_3900: raw_contact_id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == rawContactId.toString()).assertTrue();
                        console.info('logMessage contactCard_query_test_3900: detail_info = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == "1853696321").assertTrue();
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_query_test_3900: query error' + error);
            }
        }

        /**
     * @tc.number  contactCard_query_test_4000
     * @tc.name    Query all mailbox capabilities
     * @tc.desc    Function test
     */
        it("contactCard_query_test_4000", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_4000 is starting!------------");

            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoyu"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_4000: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await queryEmailInsert(DAHelper, rawContactId);
                await emailQuery(DAHelper, contactDataId, rawContactId);
                await deleteRawContact("contactCard_query_test_4000");
                await deleteAll(profileContactDataUri, "contactCard_query_test_4000");
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_4000: raw_contact insert error = " + error);
                done();
            }
        });

        async function queryEmailInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "email",
                "detail_info": "16658@163.com"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_4000: contactDataId = " + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_query_test_4000: contact_data insert error = " + error);
            }
        }

        async function emailQuery(DAHelper, contactDataId, rawContactId) {
            var resultColumns = ["id", "raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            // type_id = 1 data content_type is  email
            condition.equalTo("type_id", "1").limitAs(3).orderByDesc("id");
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_query_test_4000: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == contactDataId.toString()).assertTrue();
                        console.info('logMessage contactCard_query_test_4000: raw_contact_id = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == rawContactId.toString()).assertTrue();
                        console.info('logMessage contactCard_query_test_4000: detail_info = ' + resultSet.getString(2));
                        expect(resultSet.getString(2) == "16658@163.com").assertTrue();
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info("logMessage contactCard_query_test_4000: query error :" + error);
            }
        }

        /**
     * @tc.number  contactCard_query_test_4100
     * @tc.name    Query data information of a single contact
     * @tc.desc    Function test
     */
        it("contactCard_query_test_4100", 0, async function (done) {
            console.info("-----------logMessage contactCard_query_test_4100 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoyu"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_4100: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await queryTwoDataInsert(DAHelper, rawContactId);
                await queryTwoDataInsertCompany(DAHelper, rawContactId);
                await queryTwo(DAHelper, rawContactId);
                await deleteRawContact("contactCard_query_test_4100");
                await deleteAll(profileContactDataUri, "contactCard_query_test_4100");
                del
                done();
            } catch (error) {
                console.info("logMessage contactCard_query_test_4100: raw_contact insert error = " + error);
                done();
            }
        });

        async function queryTwoDataInsert(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId, "content_type": "name", "detail_info": "xiaoyu"
            };
            try {
                var contactDataId1 = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info("logMessage contactCard_query_test_4100: contactDataId = " + contactDataId1);
                expect(contactDataId1 > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_query_test_4100: contact_data name insert error = " + error);
            }
        }

        async function queryTwoDataInsertCompany(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "organization",
                "detail_info": "OOOO",
                "position": "Testers"
            };
            try {
                var contactDataId2 = await DAHelper.insert(profileContactDataUri, contactDataValues);
                console.info("logMessage contactCard_query_test_4100: contactDataId = " + contactDataId2);
                expect(contactDataId2 > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_query_test_4100: contact_data company insert error = " + error);
            }
        }

        async function queryTwo(DAHelper, rawContactId) {
            var resultColumns = ["id", "raw_contact_id", "detail_info", "position"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                sleep(1000);
                var array = [{
                                 detail_info: "xiaoyu", position: ""
                             }, {
                                 detail_info: "OOOO", position: "Testers"
                             }];
                var i = 0;
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info("logMessage contactCard_query_test_4100: columnNames :" + resultSet.columnNames);
                        console.info("logMessage contactCard_query_test_4100: columnCount:" + resultSet.columnCount);
                        console.info('logMessage contactCard_query_test_4100: id = ' + resultSet.getString(0));
                        console.info('logMessage contactCard_query_test_4100: raw_contact_id = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == rawContactId.toString()).assertEqual(true);
                        console.info('logMessage contactCard_query_test_4100: detail_info = ' + resultSet.getString(2));
                        expect(resultSet.getString(2) == array[i].detail_info).assertEqual(true);
                        console.info('logMessage contactCard_query_test_4100: position = ' + resultSet.getString(3));
                        expect(resultSet.getString(3) == array[i].position).assertEqual(true);
                        i++;
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info("logMessage contactCard_query_test_4100: query error :" + error);
            }
        }

        /**
     * @tc.number  contactCard_delete_test_4400
     * @tc.name    Delete the basic raw data of the contact and record the deleted basic raw data
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_4400", 0, async function (done) {
            console.info("-------logMessage contactCard_delete_test_4400 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaoming"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_4400: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await deleteRaw(DAHelper, rawContactId);
                await queryDelete(DAHelper, rawContactId);
                done();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4400: raw_contact insert error :" + error);
                done();
            }
        });

        async function deleteRaw(DAHelper, rawContactId) {
            var condition1 = new ohosDataAbility.DataAbilityPredicates();
            condition1.equalTo("id", rawContactId.toString());
            try {
                var deleteCode = await DAHelper.delete(profileRawContactUri, condition1);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_4400: deleteCode = " + deleteCode);
                expect(deleteCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4400: delete error = " + error);
            }
        }

        async function queryDelete(DAHelper, rawContactId) {
            var resultColumns = ["id", "delete_time", "display_name"];
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            condition2.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(deletedprofileRawContactUri, resultColumns, condition2);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    console.info("logMessage contactCard_delete_test_4400: columnNames:" + resultSet.columnNames);
                    console.info("logMessage contactCard_delete_test_4400: columnCount:" + resultSet.columnCount);
                    console.info('logMessage contactCard_delete_test_4400: id = ' + resultSet.getString(0));
                    console.info('logMessage contactCard_delete_test_4400: delete_time = ' + resultSet.getString(1));
                    console.info('logMessage contactCard_delete_test_4400: displayName = ' + resultSet.getString(2));
                    expect(resultSet.getString(2) == "xiaoming").assertTrue();
                }
                resultSet.close();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4400: query error:" + error);
            }
        }

        /**
     * @tc.number  contactCard_delete_test_4700
     * @tc.name    Delete detailed contact data and record detailed data
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_4700", 0, async function (done) {
            console.info("-------logMessage contactCard_delete_test_4700 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaozhang", "company": "OOOO", "position": "TestManager"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_4700: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await queryDeleteCompanyData(DAHelper, rawContactId);
                await rawContactDelete(DAHelper, rawContactId);
                await companyRawContactDeleteQuery(DAHelper, rawContactId);
                await deleteAll(profileContactDataUri, "contactCard_delete_test_4700");
                done();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4700: raw_contact insert error:" + error);
                done();
            }
        });

        async function queryDeleteCompanyData(DAHelper, rawContactId) {
            var contactDataValues = {
                "raw_contact_id": rawContactId,
                "content_type": "organization",
                "detail_info": "OOOO",
                "position": "TestManager"
            };
            try {
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_4700: contactDataId = " + contactDataId);
                expect(contactDataId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4700: contact_data insert error:" + error);
            }
        }

        async function rawContactDelete(DAHelper, rawContactId) {
            var condition1 = new ohosDataAbility.DataAbilityPredicates();
            condition1.equalTo("id", rawContactId.toString());
            try {
                var deleteCode = await DAHelper.delete(profileRawContactUri, condition1);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_4700: deleteCode = " + deleteCode);
                expect(deleteCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4700: delete error = " + error);
                done();
            }
        }

        async function companyRawContactDeleteQuery(DAHelper, rawContactId) {
            var resultColumns = ["id", "display_name", "backup_data"];
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            condition2.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(deletedprofileRawContactUri, resultColumns, condition2);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    console.info("logMessage contactCard_delete_test_4700: columnNames:" + resultSet.columnNames);
                    console.info("logMessage contactCard_delete_test_4700: columnCount:" + resultSet.columnCount);
                    console.info('logMessage contactCard_delete_test_4700: id = ' + resultSet.getString(0));
                    console.info('logMessage contactCard_delete_test_4700: displayName = ' + resultSet.getString(1));
                    expect(resultSet.getString(1) == "xiaozhang").assertEqual(true);
                    console.info('logMessage contactCard_delete_test_4700: backup_data = ' + resultSet.getString(2));
                }
                resultSet.close();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4700: query error:" + error);
            }
        }

        /**
     * @tc.number  contactCard_delete_test_4800
     * @tc.name    Delete and record basic data of a single contact
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_4800", 0, async function (done) {
            console.info("-------logMessage contactCard_delete_test_4800 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaopeng"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_4800: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDelete(DAHelper, rawContactId);
                await deletedRawContactQuery(DAHelper, rawContactId)
                await deleteAll(profileContactDataUri, "contactCard_delete_test_4800");
                done();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4800: raw_contact insert error:" + error);
                done();
            }
        });

        async function contactDelete(DAHelper, rawContactId) {
            var condition1 = new ohosDataAbility.DataAbilityPredicates();
            condition1.equalTo("name_raw_contact_id", rawContactId.toString());
            try {
                var deleteCode = await DAHelper.delete(profilecontactUri, condition1);
                sleep(1000);
                console.info("logMessage contactCard_delete_test_4800: deleteCode = " + deleteCode);
                expect(deleteCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4800: delete error = " + error);
            }
        }

        async function deletedRawContactQuery(DAHelper, rawContactId) {
            var resultColumns = ["id", "delete_time", "display_name"];
            var condition2 = new ohosDataAbility.DataAbilityPredicates();
            condition2.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(deletedprofileRawContactUri, resultColumns, condition2);
                sleep(1000)
                if (resultSet.goToFirstRow()) {
                    console.info("logMessage contactCard_delete_test_4800: columnNames:" + resultSet.columnNames);
                    console.info("logMessage contactCard_delete_test_4800: columnCount:" + resultSet.columnCount);
                    console.info('logMessage contactCard_delete_test_4800: id = ' + resultSet.getString(0));
                    console.info('logMessage contactCard_delete_test_4800: delete_time = ' + resultSet.getString(1));
                    console.info('logMessage contactCard_delete_test_4800: displayName = ' + resultSet.getString(2));
                    expect(resultSet.getString(2) == "xiaopeng").assertTrue();
                }
                resultSet.close();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_4800: query error:" + error);
            }
        }

        /**
     * @tc.number  contactCard_batchinsert_test_4900
     * @tc.name    Insert contacts in batches(raw_contact)
     * @tc.desc    Function test
     */
        it("contactCard_batchinsert_test_4900", 0, async function (done) {
            console.info("--------logMessage contactCard_batchinsert_test_4900 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var batchInsertCode = await DAHelper.batchInsert(profileRawContactUri, common.getContactBatchCompany());
                sleep(1000);
                console.info("logMessage contactCard_batchinsert_test_4900: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                var resultColumns = [];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("company", "TT4900");
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                console.info(
                    'contactCard_batchinsert_test_4900 : queryBatch start ! resultSet.rowCount = ' + resultSet.rowCount);
                expect(resultSet.rowCount == 5).assertEqual(true);
                resultSet.close();
                await deleteRawContact("contactCard_batchinsert_test_4900");
                done();
            } catch (error) {
                console.info("logMessage contactCard_batchinsert_test_4900: batchInsert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_batchinsert_test_5000
     * @tc.name    Insert contact_data in batches of address book
     * @tc.desc    Function test
     */
        it("contactCard_batchinsert_test_5000", 0, async function (done) {
            console.info("--------logMessage contactCard_batchinsert_test_5000  start!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var rawContactValues = {
                "display_name": "xiaozhi"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info("logMessage contactCard_batchinsert_test_5000: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataBatchInsert(rawContactId);
                done();
            } catch (error) {
                console.info("logMessage contactCard_batchinsert_test_5000: raw_contact insert error = " + error);
                done();
            }
            async function contactDataBatchInsert(rawContactId) {
                try {
                    var batchInsertCode =
                        await DAHelper.batchInsert(profileContactDataUri, common.getContactBatchCompanyTwo());
                    sleep(1000);
                    console.info("logMessage contactCard_batchinsert_test_5000: batchInsertCode = " + batchInsertCode);
                    expect(batchInsertCode == 0).assertTrue();
                    var map = new Map();
                    map.set("raw_contact_id", rawContactId.toString());
                    await queryBatch(map, "contactCard_batchinsert_test_5000", 5);
                    await deleteRawContact("contactCard_batchinsert_test_5000");
                    done();
                } catch (error) {
                    console.info("logMessage contactCard_batchinsert_test_5000: batchInsert error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  contactCard_batchinsert_test_5400
     * @tc.name    Batch operation of contact black list
     * @tc.desc    Function test
     */
        it("contactCard_batchinsert_test_5400", 0, async function (done) {
            console.info("--------logMessage contactCard_batchinsert_test_5400 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var batchInsertCode = await DAHelper.batchInsert(profileBlocklistUri, common.getPhoneNumberBatch());
                sleep(1000);
                console.info("logMessage contactCard_batchinsert_test_5400: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                await query(5);
                await blocklistDelete();
                done();
            } catch (error) {
                console.info("logMessage contactCard_batchinsert_test_5400: batchInsert error = " + error);
                done();
            }

            async function blocklistDelete() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("phone_number", "85525040585400");
                try {
                    var deleteCode = await DAHelper.delete(profileBlocklistUri, condition);
                    console.info("logMessage contactCard_batchinsert_test_5400: deleteCode = " + deleteCode);
                    expect(deleteCode == 0).assertTrue();
                    await query(0);
                    done();
                } catch (error) {
                    console.info("logMessage contactCard_batchinsert_test_5400: delete error = " + error);
                    done();
                }
            }

            async function query(size) {
                var resultColumns = [];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("phone_number", "85525040585400");
                try {
                    var resultSet = await DAHelper.query(profileBlocklistUri, resultColumns, condition);
                    sleep(1000);
                    console.info('contactCard_batchinsert_test_5400 : queryBatch start ! resultSet.rowCount = ' +
                    resultSet.rowCount);
                    expect(resultSet.rowCount == size).assertEqual(true);
                    resultSet.close();
                } catch (error) {
                    console.info(" contactCard_batchinsert_test_5400 logMessage resultSet.rowCount: error = " + error);
                }
            }
        });

        /**
     * @tc.number  contactCard_update_test_5300
     * @tc.name    Bulk favorites and unfavorites of contacts
     * @tc.desc    Function test
     */
        it("contactCard_update_test_5300", 0, async function (done) {
            console.info("--------logMessage contactCard_update_test_5300 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValuesOne = {
                "display_name": "fengyuan", "favorite": 1
            };
            var rawContactValuesTwo = {
                "display_name": "xiaoli", "favorite": 1
            };
            var rawContactValuesThree = {
                "display_name": "xiaoma", "favorite": 1
            };
            var listAddBluk = []
            listAddBluk[0] = rawContactValuesOne;
            listAddBluk[1] = rawContactValuesTwo;
            listAddBluk[2] = rawContactValuesThree;
            try {
                var batchInsertCode = await DAHelper.batchInsert(profileRawContactUri, listAddBluk);
                sleep(1000);
                console.info("logMessage contactCard_update_test_5300: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                await rawContactUpdateUnFavorites(DAHelper);
                await queryUnFavorites(DAHelper);
                await deleteRawContact("contactCard_update_test_5300");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_5300: batchInsert error = " + error);
                done();
            }
        });

        async function rawContactUpdateUnFavorites(DAHelper) {
            var updateValues = {
                "favorite": 0
            };
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("display_name", "fengyuan");
            condition.or();
            condition.equalTo("display_name", "xiaoma");
            try {
                var updateCode = await DAHelper.update(profileRawContactUri, updateValues, condition);
                sleep(1000);
                console.info("logMessage contactCard_update_test_5300: updateCode = " + updateCode);
                expect(updateCode == 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_update_test_5300: update error = " + error);
            }
        }

        async function queryUnFavorites(DAHelper) {
            var resultColumns = [];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("display_name", "fengyuan");
            condition.or();
            condition.equalTo("display_name", "xiaoma");
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                sleep(1000);
                console.info(
                    'contactCard_update_test_5300 : queryBatch start ! resultSet.rowCount = ' + resultSet.rowCount);
                expect(resultSet.rowCount == 2).assertEqual(true);
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var dbresult = resultSet.getString(resultSet.getColumnIndex("favorite"));
                            console.info(' contactCard_update_test_5300 :logMessage contactsQuery dbresult :' + dbresult +
                            ' value : ' + value);
                            expect(dbresult == "0").assertEqual(true);
                        } while (resultSet.goToNextRow());
                    }
                }
                resultSet.close();
            } catch (error) {
                console.info(" contactCard_batchinsert_test_5400 logMessage resultSet.rowCount: error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_batchinsert_test_5500
     * @tc.name    Contacts are added to and removed from groups in batches
     * @tc.desc    Function test
     */
        it("contactCard_batchinsert_test_5500", 0, async function (done) {
            console.info("--------logMessage contactCard_batchinsert_test_5500 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var groupValues = {
                "group_name": "ManagerGroup"
            };
            try {
                var groupId = await DAHelper.insert(profileGroupUri, groupValues);
                console.info("logMessage contactCard_batchinsert_test_5500: groupId = " + groupId);
                expect(groupId > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_batchinsert_test_5500: group insert error = " + error);
                done();
            }

            try {
                var rawContactValues1 = {
                    "display_name": "xiaohuang"
                };
                var rawContactIdFirst = await DAHelper.insert(profileRawContactUri, rawContactValues1);
                console.info("logMessage contactCard_batchinsert_test_5500: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_batchinsert_test_5500: raw_contact_1 insert error = " + error);
                done();
            }

            try {
                var rawContactValues2 = {
                    "display_name": "xiaolv"
                };
                var rawContactIdSecond = await DAHelper.insert(profileRawContactUri, rawContactValues2);
                console.info("logMessage contactCard_batchinsert_test_5500: rawContactIdSecond = " + rawContactIdSecond);
                expect(rawContactIdSecond > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_batchinsert_test_5500: raw_contact_2 insert error = " + error);
                done();
            }

            try {
                var rawContactValues3 = {
                    "display_name": "xiaobai"
                };
                var rawContactIdThird = await DAHelper.insert(profileRawContactUri, rawContactValues3);
                console.info("logMessage contactCard_batchinsert_test_5500: rawContactIdThird = " + rawContactIdThird);
                expect(rawContactIdThird > 0).assertTrue();
            } catch (error) {
                console.info("logMessage contactCard_batchinsert_test_5500: raw_contact_3 insert error = " + error);
                done();
            }
            await threeDataInsert(DAHelper, rawContactIdFirst, rawContactIdSecond, rawContactIdThird, groupId);
            await deleteRawContact("contactCard_batchinsert_test_5500");
            await deleteAll(profileGroupUri, "contactCard_batchinsert_test_5500");
            await deleteAll(profileContactDataUri, "contactCard_batchinsert_test_5500");
            done();
        });

        async function threeDataInsert(DAHelper, rawContactIdFirst, rawContactIdSecond, rawContactIdThird, groupId) {
            var contactDataValues1 = {
                "raw_contact_id": rawContactIdFirst,
                "content_type": "group_membership",
                "detail_info": groupId.toString()
            };
            var contactDataValues2 = {
                "raw_contact_id": rawContactIdSecond,
                "content_type": "group_membership",
                "detail_info": groupId.toString()
            };
            var contactDataValues3 = {
                "raw_contact_id": rawContactIdThird,
                "content_type": "group_membership",
                "detail_info": groupId.toString()
            };
            var listAddBluk = [];
            listAddBluk[0] = contactDataValues1;
            listAddBluk[1] = contactDataValues2;
            listAddBluk[2] = contactDataValues3;
            try {
                var batchInsertCode = await DAHelper.batchInsert(profileContactDataUri, listAddBluk);
                sleep(1000);
                console.info("logMessage contactCard_batchinsert_test_5500: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == 0).assertTrue();
                await contactDataDelete(DAHelper, rawContactIdFirst, rawContactIdThird);
            } catch (error) {
                console.info("logMessage contactCard_batchinsert_test_5500: batchInsert error = " + error);
            }
        }

        async function contactDataDelete(DAHelper, rawContactIdFirst, rawContactIdThird) {
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactIdFirst.toString());
            condition.or();
            condition.equalTo("raw_contact_id", rawContactIdThird.toString());
            try {
                var deleteCode = await DAHelper.delete(profileContactDataUri, condition);
                sleep(1000);
                console.info("logMessage contactCard_batchinsert_test_5500: deleteCode = " + deleteCode);
                expect(deleteCode == 0).assertTrue();
                await queryTwoData(DAHelper, rawContactIdFirst, rawContactIdThird);
            } catch (error) {
                console.info("logMessage contactCard_batchinsert_test_5500: delete error = " + error);
            }
        }

        async function queryTwoData(DAHelper, rawContactIdFirst, rawContactIdThird) {
            var resultColumns = [];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactIdFirst.toString());
            condition.or();
            condition.equalTo("raw_contact_id", rawContactIdThird.toString());
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                sleep(1000);
                console.info(
                    'contactCard_batchinsert_test_5500 : queryBatch start ! resultSet.rowCount = ' + resultSet.rowCount);
                expect(resultSet.rowCount == 0).assertEqual(true);
                resultSet.close();
            } catch (error) {
                console.info(" contactCard_batchinsert_test_5400 logMessage resultSet.rowCount: error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_pinyin_query_test_200
     * @tc.name    Contact name to pinyin (Chinese Simplified)
     * @tc.desc    Function test
     */
        it("contactCard_pinyin_query_test_200", 0, async function (done) {
            console.info("--------logMessage contactCard_pinyin_query_test_200 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "小黄"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                console.info('logMessage contactCard_pinyin_query_test_200 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataInsert();
                await deleteRawContact("contactCard_pinyin_query_test_100");
                await deleteAll(profileContactDataUri, "contactCard_pinyin_query_test_100");
                done();
            } catch (error) {
                console.info("logMessage contactCard_pinyin_query_test_200: raw_contact insert error = " + error);
                done();
            }

            async function contactDataInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId,
                    "content_type": "name",
                    "detail_info": "xiaohuang"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    console.info('logMessage contactCard_pinyin_query_test_200 contactDataId = ' + contactDataId);
                    expect(contactDataId > 0).assertTrue();
                    await searchQuery(DAHelper, rawContactId);
                } catch (error) {
                    console.info('logMessage contactCard_pinyin_query_test_200 contact_data insert error = ' + error);
                    done();
                }
            }
        });

        async function searchQuery(DAHelper, rawContactId) {
            var resultColumns = ["id", "search_name"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(profilesearchContactUri, resultColumns, condition);
                var map = common.getSearchMap();
                map.set("raw_contact_id", rawContactId.toString());
                if (resultSet.goToFirstRow()) {
                    do {
                        for (var [key, value] of map) {
                            var dbresult = resultSet.getString(resultSet.getColumnIndex(key));
                            console.info(tag + ':contactCard_pinyin_query_test_200 key = ' + key +
                            ' dbresult :' + dbresult + ' value : ' + value);
                            expect(value == dbresult).assertEqual(true);
                        }
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_pinyin_query_test_200 query error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_pinyin_query_test_100
     * @tc.name    Contact name to pinyin (Chinese Traditional)
     * @tc.desc    Function test
     */
        it("contactCard_pinyin_query_test_100", 0, async function (done) {
            console.info("--------logMessage contactCard_pinyin_query_test_100 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "鍵盤"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_pinyin_query_test_100 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataInsert();
                await deleteRawContact("contactCard_pinyin_query_test_100");
                await deleteAll(profileContactDataUri, "contactCard_pinyin_query_test_100");
                done();
            } catch (error) {
                console.info('logMessage contactCard_pinyin_query_test_100 raw_contact insert error = ' + error);
                done();
            }

            async function contactDataInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId, "content_type": "name", "detail_info": "鍵盤"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    sleep(1000);
                    console.info('logMessage contactCard_pinyin_query_test_100 contactDataId = ' + contactDataId);
                    expect(contactDataId > 0).assertTrue();
                    await searchContactQueryChinese(DAHelper, rawContactId);
                } catch (error) {
                    console.info('logMessage contactCard_pinyin_query_test_100 contact_data insert error = ' + error);
                    done();
                }
            }
        });

        async function searchContactQueryChinese(DAHelper, rawContactId) {
            var resultColumns = ["raw_contact_id", "search_name"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(profilesearchContactUri, resultColumns, condition);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_pinyin_query_test_100: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == rawContactId.toString()).assertEqual(true);
                        console.info(
                            'logMessage contactCard_pinyin_query_test_100: search_name = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == "鍵盤||jianpan||jp").assertEqual(true);
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_pinyin_query_test_100 query error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_pinyin_query_test_300
     * @tc.name    Contact name to pinyin (English)
     * @tc.desc    Function test
     */
        it("contactCard_pinyin_query_test_300", 0, async function (done) {
            console.info("--------logMessage contactCard_pinyin_query_test_300 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "Tom"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_pinyin_query_test_300 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataInsert();
                await deleteRawContact("contactCard_pinyin_query_test_300");
                done();
            } catch (error) {
                console.info('logMessage contactCard_pinyin_query_test_300 raw_contact insert error = ' + error);
                done();
            }

            async function contactDataInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId, "content_type": "name", "detail_info": "Tom"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    console.info('logMessage contactCard_pinyin_query_test_300 contactDataId = ' + contactDataId);
                    expect(contactDataId > 0).assertTrue();
                    await searchContactQuery(DAHelper, rawContactId);
                } catch (error) {
                    console.info('logMessage contactCard_pinyin_query_test_300 contact_data insert error = ' + error);
                    done();
                }
            }
        });

        async function searchContactQuery(DAHelper, rawContactId) {
            var resultColumns = ["raw_contact_id", "search_name"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(profilesearchContactUri, resultColumns, condition);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_pinyin_query_test_300: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == rawContactId.toString()).assertTrue();
                        console.info(
                            'logMessage contactCard_pinyin_query_test_300: search_name = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == "Tom||Tom||Tom").assertTrue();
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_pinyin_query_test_300 query error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_fuzzyquery_test_400
     * @tc.name    Contact fuzzy search query interface (name)
     * @tc.desc    Function test
     */
        it("contactCard_fuzzyquery_test_400", 0, async function (done) {
            console.info("--------logMessage contactCard_fuzzyquery_test_400 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "400xiaoming400"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_fuzzyquery_test_400 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await rawContactNameQuery();
                await deleteRawContact("contactCard_fuzzyquery_test_400");
                done();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_400 raw_contact insert error = ' + error);
                done();
            }

            async function rawContactNameQuery() {
                var resultColumns = ["id", "display_name"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.like("display_name", "%xiaoming40%");
                condition.and();
                condition.equalTo("is_deleted", "0");
                try {
                    var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info('logMessage contactCard_fuzzyquery_test_400: id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == rawContactId.toString()).assertEqual(true);
                            console.info(
                                'logMessage contactCard_fuzzyquery_test_400: displayName = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "400xiaoming400").assertEqual(true);
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info('logMessage contactCard_fuzzyquery_test_400 query error = ' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  contactCard_fuzzyquery_test_500
     * @tc.name    Contact fuzzy search query interface (name pinyin, pinyin name)
     * @tc.desc    Function test
     */
        it("contactCard_fuzzyquery_test_500", 0, async function (done) {
            console.info("--------logMessage contactCard_fuzzyquery_test_500 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            var rawContactValues = {
                "display_name": "500xiaoming500", "phonetic_name": "500xiaoming500||xm"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_fuzzyquery_test_500 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await rawContactPhoneticNameQuery();
                await deleteRawContact("contactCard_fuzzyquery_test_500");
                done();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_500 raw_contact insert error = ' + error);
                done();
            }

            async function rawContactPhoneticNameQuery() {
                var resultColumns = ["id", "display_name", "phonetic_name"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.like("phonetic_name", "%xiaoming%");
                condition.and();
                condition.equalTo("is_deleted", "0");
                condition.or();
                condition.like("phonetic_name", "%xm%");
                condition.and();
                condition.equalTo("is_deleted", "0");
                try {
                    var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                    if (resultSet.goToFirstRow()) {
                        do {
                            console.info('logMessage contactCard_fuzzyquery_test_500: id = ' + resultSet.getString(0));
                            expect(resultSet.getString(0) == rawContactId.toString()).assertTrue();
                            console.info(
                                'logMessage contactCard_fuzzyquery_test_500: displayName = ' + resultSet.getString(1));
                            expect(resultSet.getString(1) == "500xiaoming500").assertTrue();
                            console.info(
                                'logMessage contactCard_fuzzyquery_test_500: phoneticName = ' + resultSet.getString(2));
                            expect(resultSet.getString(2) == "500xiaoming500||xm").assertTrue();
                        } while (resultSet.goToNextRow());
                    }
                    resultSet.close();
                } catch (error) {
                    console.info('logMessage contactCard_fuzzyquery_test_500 query error = ' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  contactCard_fuzzyquery_test_600
     * @tc.name    Contact fuzzy search query interface (company, position)
     * @tc.desc    Function test
     */
        it("contactCard_fuzzyquery_test_600", 0, async function (done) {
            console.info("--------logMessage contactCard_fuzzyquery_test_600 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoming", "company": "TT600", "position": "Testers600"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_fuzzyquery_test_600 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await rawContactCompanyQuery(DAHelper, rawContactId);
                await deleteRawContact("contactCard_fuzzyquery_test_600");
                done();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_600 raw_contact insert error = ' + error);
                done();
            }
        });

        async function rawContactCompanyQuery(DAHelper, rawContactId) {
            var resultColumns = ["id", "display_name", "company", "position"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.like("company", "%TT6%");
            condition.and();
            condition.equalTo("is_deleted", "0");
            condition.or();
            condition.like("position", "%Testers6%");
            condition.and();
            condition.equalTo("is_deleted", "0");
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_fuzzyquery_test_600: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == rawContactId.toString()).assertEqual(true);
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_600: displayName = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == "xiaoming").assertEqual(true);
                        console.info('logMessage contactCard_fuzzyquery_test_600: company = ' + resultSet.getString(2));
                        expect(resultSet.getString(2) == "TT600").assertEqual(true);
                        console.info('logMessage contactCard_fuzzyquery_test_600: position = ' + resultSet.getString(3));
                        expect(resultSet.getString(3) == "Testers600").assertEqual(true);
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_600 query error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_fuzzyquery_test_700
     * @tc.name    Contact fuzzy search query interface (mobile phone number)
     * @tc.desc    Function test
     */
        it("contactCard_fuzzyquery_test_700", 0, async function (done) {
            console.info("--------logMessage contactCard_fuzzyquery_test_700 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoming"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_fuzzyquery_test_700 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataPhoneInsert();
                await deleteRawContact("contactCard_fuzzyquery_test_700");
                await deleteAll(profileContactDataUri, "contactCard_fuzzyquery_test_700");
                done();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_700 raw_contact insert error = ' + error);
                done();
            }

            async function contactDataPhoneInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId,
                    "content_type": "phone",
                    "detail_info": "14528963"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    sleep(1000);
                    console.info('logMessage contactCard_fuzzyquery_test_700 contactDataId  = ' + contactDataId);
                    expect(contactDataId > 0).assertTrue();
                    await fuzzyQueryPhone(DAHelper, rawContactId, contactDataId);
                } catch (error) {
                    console.info('logMessage contactCard_fuzzyquery_test_700 contact_data insert error = ' + contactDataId);
                    done();
                }
            }
        });

        async function fuzzyQueryPhone(DAHelper, rawContactId, contactDataId) {
            var resultColumns = ["id", "raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            // type_id = 5 data is phone
            condition.like("detail_info", "%52896%");
            condition.and();
            condition.equalTo("type_id", "5");
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_fuzzyquery_test_700: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == contactDataId.toString()).assertEqual(true);
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_700: raw_contact_id = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == rawContactId.toString()).assertEqual(true);
                        console.info('logMessage contactCard_fuzzyquery_test_700: detail_info = ' + resultSet.getString(2));
                        expect(resultSet.getString(2) == "14528963").assertEqual(true);
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_700 query error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_fuzzyquery_test_800
     * @tc.name    Contact fuzzy search query interface (nickname)
     * @tc.desc    Function test
     */
        it("contactCard_fuzzyquery_test_800", 0, async function (done) {
            console.info("--------logMessage contactCard_fuzzyquery_test_800 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoming"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_fuzzyquery_test_800 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactNickNameInsert();
                await deleteRawContact("contactCard_fuzzyquery_test_800");
                await deleteAll(profileContactDataUri, "contactCard_fuzzyquery_test_800");
                done();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_800 raw_contact insert error = ' + error);
                done();
            }

            async function contactNickNameInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId,
                    "content_type": "nickname",
                    "detail_info": "daming"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    console.info('logMessage contactCard_fuzzyquery_test_800 contactDataId = ' + contactDataId);
                    expect(contactDataId > 0).assertTrue();
                    await fuzzyQueryPhoneNickName(DAHelper, rawContactId, contactDataId);
                } catch (error) {
                    console.info('logMessage contactCard_fuzzyquery_test_800 contact_data insert error = ' + contactDataId);
                    done();
                }
            }
        });

        async function fuzzyQueryPhoneNickName(DAHelper, rawContactId, contactDataId) {
            var resultColumns = ["id", "raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            // type_id = 3 data is nickname
            condition.like("detail_info", "%daming%");
            condition.and();
            condition.equalTo("type_id", "3");
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_fuzzyquery_test_800: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == contactDataId.toString()).assertTrue();
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_800: raw_contact_id = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == rawContactId.toString()).assertTrue();
                        console.info('logMessage contactCard_fuzzyquery_test_800: detail_info = ' + resultSet.getString(2));
                        expect(resultSet.getString(2) == "daming").assertTrue();
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_800 query error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_fuzzyquery_test_900
     * @tc.name    Contact fuzzy search query interface (mailbox)
     * @tc.desc    Function test
     */
        it("contactCard_fuzzyquery_test_900", 0, async function (done) {
            console.info("--------logMessage contactCard_fuzzyquery_test_900 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoming"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_fuzzyquery_test_900 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataEmailInsert();
                await deleteRawContact("contactCard_fuzzyquery_test_900");
                await deleteAll(profileContactDataUri, "contactCard_fuzzyquery_test_900");
                done();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_900 raw_contact insert error = ' + error);
                done();
            }

            async function contactDataEmailInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId,
                    "content_type": "email",
                    "detail_info": "1564@163.com"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    console.info('logMessage contactCard_fuzzyquery_test_900 ; = ' + contactDataId);
                    expect(contactDataId > 0).assertTrue();
                    await fuzzyQueryEmail(DAHelper, rawContactId, contactDataId);
                } catch (error) {
                    console.info('logMessage contactCard_fuzzyquery_test_900 contact_data insert error = ' + error);
                    done();
                }
            }
        });

        async function fuzzyQueryEmail(DAHelper, rawContactId, contactDataId) {
            var resultColumns = ["id", "raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            // type_id = 1 data is email
            condition.like("detail_info", "%1564%");
            condition.and();
            condition.equalTo("type_id", "1");
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_fuzzyquery_test_900: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == contactDataId.toString()).assertTrue();
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_900: raw_contact_id = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == rawContactId.toString()).assertTrue();
                        console.info('logMessage contactCard_fuzzyquery_test_900: detail_info = ' + resultSet.getString(2));
                        expect(resultSet.getString(2) == "1564@163.com").assertTrue();
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_900 query error = ' + error);
            }
        }

        /**
     * @tc.number  contactCard_fuzzyquery_test_1000
     * @tc.name    Contact fuzzy search query interface (residential)
     * @tc.desc    Function test
     */
        it("contactCard_fuzzyquery_test_1000", 0, async function (done) {
            console.info("--------logMessage contactCard_fuzzyquery_test_1000 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoming"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_fuzzyquery_test_1000 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataPostalInsert();
                await deleteRawContact("contactCard_fuzzyquery_test_1000");
                await deleteAll(profileContactDataUri, "contactCard_fuzzyquery_test_1000");
                done();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_1000 raw_contact insert error = ' + error);
                done();
            }

            async function contactDataPostalInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId,
                    "content_type": "postal_address",
                    "detail_info": "nanjing1000"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    sleep(1000);
                    console.info('logMessage contactCard_fuzzyquery_test_1000 contactDataId = ' + contactDataId);
                    expect(contactDataId > 0).assertTrue();
                    await fuzzyQueryAddress(DAHelper, rawContactId, contactDataId);
                } catch (error) {
                    console.info('logMessage contactCard_fuzzyquery_test_1000 contact_data insert error = ' + error);
                    done();
                }
            }
        });

        async function fuzzyQueryAddress(DAHelper, rawContactId, contactDataId) {
            var resultColumns = ["id", "raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            // type_id = 1 data is postal_address
            condition.like("detail_info", "%nanjing10%");
            condition.and();
            condition.equalTo("type_id", "7");
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                sleep(1000);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_fuzzyquery_test_1000: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == contactDataId.toString()).assertTrue();
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_1000: raw_contact_id = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == rawContactId.toString()).assertTrue();
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_1000: detail_info = ' + resultSet.getString(2));
                        expect(resultSet.getString(2) == "nanjing1000").assertTrue();
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_1000: query error' + error);
            }
        }

        /**
     * @tc.number  contactCard_fuzzyquery_test_1100
     * @tc.name    Contact fuzzy search query interface (remarks)
     * @tc.desc    Function test
     */
        it("contactCard_fuzzyquery_test_1100", 0, async function (done) {
            console.info("--------logMessage contactCard_fuzzyquery_test_1100 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoming"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage contactCard_fuzzyquery_test_1100 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataNoteInsert();
                await deleteRawContact("contactCard_fuzzyquery_test_1100");
                await deleteAll(profileContactDataUri, "contactCard_fuzzyquery_test_1100");
                done();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_1100 raw_contact insert error = ' + error);
                done();
            }

            async function contactDataNoteInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId,
                    "content_type": "note",
                    "detail_info": "java1100"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    sleep(1000);
                    console.info('logMessage contactCard_fuzzyquery_test_1100 contactDataId = ' + contactDataId);
                    expect(contactDataId > 0).assertTrue();
                    await fuzzyQueryNote(DAHelper, rawContactId, contactDataId);
                } catch (error) {
                    console.info('logMessage contactCard_fuzzyquery_test_1100 contact_data insert error = ' + error);
                    done();
                }
            }
        });

        async function fuzzyQueryNote(DAHelper, rawContactId, contactDataId) {
            var resultColumns = ["id", "raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            // type_id = 10 data is note
            condition.like("detail_info", "%java11%");
            condition.and();
            condition.equalTo("type_id", "10");
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_fuzzyquery_test_1100: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == contactDataId.toString()).assertEqual(true);
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_1100: raw_contact_id = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == rawContactId.toString()).assertEqual(true);
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_1100: detail_info = ' + resultSet.getString(2));
                        expect(resultSet.getString(2) == "java1100").assertEqual(true);
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_1100: query error' + error);
            }
        }

        /**
     * @tc.number  contactCard_fuzzyquery_test_1200
     * @tc.name    Contact fuzzy search query interface (AIM)
     * @tc.desc    Function test
     */
        it("contactCard_fuzzyquery_test_1200", 0, async function (done) {
            console.info("--------logMessage contactCard_fuzzyquery_test_1200 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoming"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                console.info('logMessage contactCard_fuzzyquery_test_1200 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataAimInsert();
                await deleteRawContact("contactCard_fuzzyquery_test_1200");
                await deleteAll(profileContactDataUri, "contactCard_fuzzyquery_test_1200");
                done();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_1200 raw_contact insert error = ' + error);
                done();
            }

            async function contactDataAimInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId,
                    "content_type": "im",
                    "detail_info": "aaaa1200"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    console.info('logMessage contactCard_fuzzyquery_test_1200 ; = ' + contactDataId);
                    expect(contactDataId > 0).assertTrue();
                    await fuzzyQueryIM(DAHelper, rawContactId, contactDataId);
                } catch (error) {
                    console.info('logMessage contactCard_fuzzyquery_test_1200 contact_data insert error = ' + error);
                    done();
                }
            }
        });

        async function fuzzyQueryIM(DAHelper, rawContactId, contactDataId) {
            var resultColumns = ["id", "raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            // type_id = 10 data is im account number
            condition.like("detail_info", "%aa12%");
            condition.and();
            condition.equalTo("type_id", "2");
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                if (resultSet.goToFirstRow()) {
                    do {
                        console.info('logMessage contactCard_fuzzyquery_test_1200: id = ' + resultSet.getString(0));
                        expect(resultSet.getString(0) == contactDataId.toString()).assertEqual(true);
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_1200: raw_contact_id = ' + resultSet.getString(1));
                        expect(resultSet.getString(1) == rawContactId.toString()).assertEqual(true);
                        console.info(
                            'logMessage contactCard_fuzzyquery_test_1200: detail_info = ' + resultSet.getString(2));
                        expect(resultSet.getString(2) == "aaaa1200").assertEqual(true);
                    } while (resultSet.goToNextRow());
                }
                resultSet.close();
            } catch (error) {
                console.info('logMessage contactCard_fuzzyquery_test_1200: query error' + error);
            }
        }

        /**
     * @tc.number  abnormal_contactCard_insert_test_200
     * @tc.name    Abnormal use case, an incorrect field is passed in when inserting
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_insert_test_200", 0, async function (done) {
            console.info("------------abnormal_contactCard_insert_test_200 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_names": "xiaoshan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                console.info("logMessage abnormal_contactCard_insert_test_200: rawContactId = " + rawContactId);
                expect(rawContactId).assertEqual(-1);
                done();
            } catch (error) {
                console.info('logMessage abnormal_contactCard_insert_test_200: raw_contact insert error' + error);
                done();
            }
        });

        /**
     * @tc.number  abnormal_contactCard_insert_test_300
     * @tc.name    Abnormal use case, an incorrect table name is passed in when inserting
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_insert_test_300", 0, async function (done) {
            console.info("------------abnormal_contactCard_insert_test_300 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var errorUri = "dataability:///com.ohos.calllogability/contacts/raw_contacts";

            var rawContactValues = {
                "display_name": "xiaoshan"
            };
            try {
                var rawContactId = await DAHelper.insert(errorUri, rawContactValues);
                console.info("logMessage abnormal_contactCard_insert_test_300: rawContactId = " + rawContactId);
                expect(rawContactId == -1).assertTrue();
                done();
            } catch (error) {
                console.info('logMessage abnormal_contactCard_insert_test_300: raw_contact insert error' + error);
                done();
            }
        });

        /**
     * @tc.number  abnormal_contactCard_update_test_1100
     * @tc.name    Abnormal use case, an incorrect field is passed in when updating
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_update_test_1100", 0, async function (done) {
            console.info("------------abnormal_contactCard_update_test_1100 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaoshan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                console.info("logMessage abnormal_contactCard_update_test_1100: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await abnormalUpdate();
                await deleteRawContact("abnormal_contactCard_update_test_1100");
                done();
            } catch (error) {
                console.info('logMessage abnormal_contactCard_update_test_1100: raw_contact insert error' + error);
                done();
            }

            async function abnormalUpdate() {
                var updateValues = {
                    "display_names": "xiaosan"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", rawContactId.toString());
                try {
                    var updataCode = await DAHelper.update(profileRawContactUri, updateValues, condition)
                    console.info("logMessage abnormal_contactCard_update_test_1100: updataCode = " + updataCode);
                    expect(updataCode == -1).assertTrue();
                    var map = new Map();
                    map.set("id", rawContactId.toString());
                    map.set("display_name", "xiaoshan");
                    await contactsQuery(map, "abnormal_contactCard_update_test_1100", profileRawContactUri);
                } catch (error) {
                    console.info('logMessage abnormal_contactCard_update_test_1100: update error' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_contactCard_update_test_1000
     * @tc.name    Abnormal use case, an incorrect table name is passed in when updating
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_update_test_1000", 0, async function (done) {
            console.info("------------abnormal_contactCard_update_test_1000 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var errorUri = "dataability:///com.ohos.calllogability/contacts/raw_contacts";

            var rawContactValues = {
                "display_name": "xiaoshan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                console.info("logMessage abnormal_contactCard_update_test_1000: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await abnormalUpdate();
                await deleteRawContact("abnormal_contactCard_update_test_1100");
                done();
            } catch (error) {
                console.info('logMessage abnormal_contactCard_update_test_1000: raw_contact insert error' + error);
                done();
            }

            async function abnormalUpdate() {
                var updateValues = {
                    "display_name": "xiaosan"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", rawContactId.toString());
                try {
                    var updataCode = await DAHelper.update(errorUri, updateValues, condition);
                    console.info("logMessage abnormal_contactCard_update_test_1000: updataCode = " + updataCode);
                    expect(updataCode == -1).assertTrue();
                    var map = new Map();
                    map.set("id", rawContactId.toString());
                    map.set("display_name", "xiaoshan");
                    await contactsQuery(map, "abnormal_contactCard_update_test_1000", profileRawContactUri);
                } catch (error) {
                    console.info("logMessage abnormal_contactCard_update_test_1000: updata error = " + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_contactCard_delete_test_4500
     * @tc.name    Abnormal use case, an incorrect field is passed in when deleting
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_delete_test_4500", 0, async function (done) {
            console.info("------------abnormal_contactCard_delete_test_4500 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaozhi"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                console.info("logMessage abnormal_contactCard_delete_test_4500: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await abnormalDelete();
                await deleteRawContact("abnormal_contactCard_delete_test_4500");
                done();
            } catch (error) {
                console.info('logMessage abnormal_contactCard_delete_test_4500: raw_contact insert error' + error);
                done();
            }

            async function abnormalDelete() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", rawContactId.toString());
                try {
                    var deleteCode = await DAHelper.delete(profileRawContactUri, condition);
                    console.info("logMessage abnormal_contactCard_delete_test_4500: deleteCode = " + deleteCode);
                    expect(deleteCode == -1).assertTrue();
                    var map = new Map();
                    map.set("id", rawContactId.toString());
                    map.set("display_name", "xiaozhi");
                    await contactsQuery(map, "abnormal_contactCard_delete_test_4500", profileRawContactUri);
                    done();
                } catch (error) {
                    console.info('logMessage abnormal_contactCard_delete_test_4500: delete error' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_contactCard_delete_test_4600
     * @tc.name    Abnormal use case, an incorrect table name is passed in when deleting
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_delete_test_4600", 0, async function (done) {
            console.info("------------abnormal_contactCard_delete_test_4600 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var errorUri = "dataability:///com.ohos.calllogability/contacts/raw_contacts";

            var rawContactValues = {
                "display_name": "xiaoshan"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                console.info("logMessage abnormal_contactCard_delete_test_4600: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await abnormalDelete();
                await deleteRawContact("abnormal_contactCard_delete_test_4600");
                done();
            } catch (error) {
                console.info('logMessage abnormal_contactCard_delete_test_4600: raw_contact insert error' + error);
                done();
            }

            async function abnormalDelete() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", rawContactId.toString());
                try {
                    var deleteCode = await DAHelper.delete(errorUri, condition);
                    console.info("logMessage abnormal_contactCard_delete_test_4600: deleteCode = " + deleteCode);
                    expect(deleteCode == -1).assertTrue();
                    var map = new Map();
                    map.set("id", rawContactId.toString());
                    map.set("display_name", "xiaoshan");
                    await contactsQuery(map, "abnormal_contactCard_delete_test_4600", profileRawContactUri);
                    done();
                } catch (error) {
                    console.info('logMessage abnormal_contactCard_delete_test_4600: delete error' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_contactCard_query_test_4200
     * @tc.name    Abnormal use case, an incorrect field is passed in when querying
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_query_test_4200", 0, async function (done) {
            console.info("------------abnormal_contactCard_query_test_4200 is starting!-----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaozhicheng"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues)
                console.info("logMessage abnormal_contactCard_query_test_4200: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await abnormalQuery();
                await deleteRawContact("abnormal_contactCard_query_test_4200");
                done();
            } catch (error) {
                console.info('logMessage abnormal_contactCard_query_test_4200: raw_contact insert error' + error);
                done();
            }

            async function abnormalQuery() {
                var resultColumns = ["id", "display_name"];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("ids", rawContactId.toString());
                try {
                    var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                    expect(resultSet.goToFirstRow()).assertEqual(false);
                    console.info(
                        'logMessage abnormal_contactCard_query_test_4200: goToFirstRow' + resultSet.goToFirstRow());
                    resultSet.close();
                } catch (error) {
                    console.info('logMessage abnormal_contactCard_query_test_4200: query error = ' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_contactCard_batchinsert_test_5100
     * @tc.name    Abnormal use cases, when adding data in batches, some of them failed, check the processing logic
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_batchinsert_test_5100", 0, async function (done) {
            console.info("--------logMessage abnormal_contactCard_batchinsert_test_5100 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var addBulkValue1 = {
                "display_name": "zhangsan"
            };
            var addBulkValue2 = {
                "display_names": "lisi"
            };
            var addBulkValue3 = {
                "display_name": "wangwu"
            };
            var addBulkValue4 = {
                "display_names": "laoliu"
            };
            var addBulkValue5 = {
                "display_name": "xiaozhang"
            };
            var listAddBluk = [];
            listAddBluk[0] = addBulkValue1;
            listAddBluk[1] = addBulkValue2;
            listAddBluk[2] = addBulkValue3;
            listAddBluk[3] = addBulkValue4;
            listAddBluk[4] = addBulkValue5;
            try {
                var batchInsertCode = await DAHelper.batchInsert(profileRawContactUri, listAddBluk);
                console.info("logMessage abnormal_contactCard_batchinsert_test_5100: batchInsertCode = " + batchInsertCode);
                expect(batchInsertCode == -1).assertTrue();
                done();
            } catch (error) {
                console.info("logMessage abnormal_contactCard_batchinsert_test_5100: batch insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  abnormal_contactCard_insert_data_5300
     * @tc.name    insert_data error no rawContactId
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_insert_data_5300", 0, async function (done) {
            console.info("--------logMessage abnormal_contactCard_insert_data_5300 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaotian"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                sleep(1000);
                console.info('logMessage abnormal_contactCard_insert_data_5300 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataAimInsert();
                await deleteRawContact("abnormal_contactCard_insert_data_5300");
                done();
            } catch (error) {
                console.info('logMessage abnormal_contactCard_insert_data_5300 raw_contact insert error = ' + error);
                done();
            }

            async function contactDataAimInsert() {
                var contactDataValues = {
                    "content_type": "im", "detail_info": "aaaa"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    console.info('logMessage abnormal_contactCard_insert_data_5300 ; = ' + contactDataId);
                    expect(contactDataId == -1).assertTrue();
                } catch (error) {
                    console.info('logMessage abnormal_contactCard_insert_data_5300 contact_data insert error = ' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  abnormal_contactCard_insert_data_5400
     * @tc.name    insert_data error no content_type
     * @tc.desc    Function test
     */
        it("abnormal_contactCard_insert_data_5400", 0, async function (done) {
            console.info("--------logMessage abnormal_contactCard_insert_data_5400 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var rawContactValues = {
                "display_name": "xiaotian"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, rawContactValues);
                console.info('logMessage abnormal_contactCard_insert_data_5400 rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await contactDataAimInsert();
                await deleteRawContact("abnormal_contactCard_insert_data_5400");
                done();
            } catch (error) {
                console.info('logMessage abnormal_contactCard_insert_data_5400 raw_contact insert error = ' + error);
                done();
            }

            async function contactDataAimInsert() {
                var contactDataValues = {
                    "raw_contact_id": rawContactId, "detail_info": "aaaa"
                };
                try {
                    var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                    console.info('logMessage abnormal_contactCard_insert_data_5400 ; = ' + contactDataId);
                    expect(contactDataId == -1).assertTrue();
                } catch (error) {
                    console.info('logMessage abnormal_contactCard_insert_data_5400 contact_data insert error = ' + error);
                    done();
                }
            }
        });

        /**
     * @tc.number  contactCard_delete_test_5500
     * @tc.name    batchDelete
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_5500", 0, async function (done) {
            console.info("--------logMessage contactCard_delete_test_5500 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, common.getProfileRawContact());
                console.info("logMessage contactCard_delete_test_5500: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var map = common.getProfileRawContactMap();
                map.set("id", rawContactId.toString());
                await contactsQuery(map, "contactCard_delete_test_5500", profileRawContactUri);
                await executeBatch();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_5500:  insert error = " + error);
                done();
            }
            async function executeBatch() {
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", rawContactId.toString());
                DAHelper.executeBatch(URI_CONTACTS, [{
                                                         uri: profileRawContactUri,
                                                         type: featureAbility.DataAbilityOperationType.TYPE_DELETE,
                                                         predicates: condition,
                                                         expectedCount: 0,
                                                         PredicatesBackReferences: {},
                                                         interrupted: true,
                                                     }],
                    (error, data) => {
                        console.info(
                            "logMessage contactCard_delete_test_5500: executeBatch data = " + JSON.stringify(data));
                        console.info("logMessage contactCard_delete_test_5500: data_3 = " + data);
                        console.info("logMessage contactCard_delete_test_5500:  data_1= " + data[0].count);
                        expect(data[0].count == 0).assertTrue();
                        done();
                    });
            }
        });

        /**
     * @tc.number  contactCard_update_test_5600
     * @tc.name    batchUpdate
     * @tc.desc    Function test
     */
        it("contactCard_update_test_5600", 0, async function (done) {
            console.info("--------logMessage contactCard_update_test_5600 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, common.getProfileRawContact());
                console.info("logMessage contactCard_update_test_5600: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var map = common.getProfileRawContactMap();
                map.set("id", rawContactId.toString());
                await contactsQuery(map, "contactCard_update_test_5600", profileRawContactUri);
                await executeBatch();
            } catch (error) {
                console.info("logMessage contactCard_update_test_5600:  insert error = " + error);
                done();
            }

            async function executeBatch() {
                var updateValues = {
                    "display_name": "xiaoxiaoxiao"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", rawContactId.toString());
                DAHelper.executeBatch(URI_CONTACTS, [{
                                                         uri: profileRawContactUri,
                                                         type: featureAbility.DataAbilityOperationType.TYPE_UPDATE,
                                                         valuesBucket: updateValues,
                                                         predicates: condition,
                                                         expectedCount: 0,
                                                         PredicatesBackReferences: {},
                                                         interrupted: true,
                                                     }],
                    (error, data) => {
                        console.info(
                            "logMessage contactCard_update_test_5600: executeBatch data = " + JSON.stringify(data));
                        console.info("logMessage contactCard_update_test_5600: data_3 = " + data);
                        console.info("logMessage contactCard_update_test_5600:  data_1= " + data[0].count);
                        expect(data[0].count == 0).assertTrue();
                        done();
                    });
            }
        });

        /**
     * @tc.number  contactCard_insert_test_5700
     * @tc.name    Insert Multiple raw_contact essential information
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_5700", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_5700 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, common.getProfileRawContact());
                sleep(1000);
                console.info('logMessage contactCard_insert_test_5700: rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var map = common.getProfileRawContactMap();
                map.set("id", rawContactId.toString());
                await contactsQuery(map, "contactCard_insert_test_5700", profileRawContactUri);
                var rawContactIdOne = await DAHelper.insert(profileRawContactUri, common.getProfileRawContact());
                sleep(1000);
                console.info('logMessage contactCard_insert_test_5700: rawContactIdOne = ' + rawContactIdOne);
                expect(rawContactIdOne > 0).assertTrue();
                var map = common.getProfileRawContactMap();
                map.set("id", rawContactIdOne.toString());
                await contactsQuery(map, "contactCard_insert_test_5700", profileRawContactUri);
                var rawContactIdTwo = await DAHelper.insert(profileRawContactUri, common.getProfileRawContact());
                sleep(1000);
                console.info('logMessage contactCard_insert_test_5700: rawContactIdTwo = ' + rawContactIdTwo);
                expect(rawContactIdTwo > 0).assertTrue();
                var map = common.getProfileRawContactMap();
                map.set("id", rawContactIdTwo.toString());
                await contactsQuery(map, "contactCard_insert_test_5700", profileRawContactUri);
                await deleteRawContact("contactCard_insert_test_5700");
                done();
            } catch (error) {
                console.info('logMessage contactCard_insert_test_5700: insert error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insert_test_5800
     * @tc.name    Add 20000 bytes of basic contact information and verify whether the text insertion is successful
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_5800", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_5800 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var name;
            for (var i = 0; i < 20000; i++) {
                name += "i";
            }
            var contcatvalues = {
                "display_name": name
            }
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, contcatvalues);
                sleep(1000);
                console.info('logMessage contactCard_insert_test_5800: rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var map = new Map();
                map.set("id", rawContactId.toString());
                map.set("display_name", name);
                await contactsQuery(map, "contactCard_insert_test_5800", profileRawContactUri);
                await deleteRawContact("contactCard_insert_test_5800");
                done();
            } catch (error) {
                console.info('logMessage contactCard_insert_test_5800: insert error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insert_test_5900
     * @tc.name    Add multiple contact details and verify that the insertion was successful
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_5900", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_5900 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var contcatvalues = {
                "display_name": "xiao5900"
            }
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, contcatvalues);
                console.info('logMessage contactCard_insert_test_5900: rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var contactDataValues1 = {
                    "raw_contact_id": rawContactId,
                    "content_type": "phone",
                    "detail_info": "19960229"
                };
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues1);
                console.info('logMessage contactCard_insert_test_5900: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();

                var map = new Map();
                map.set("id", contactDataId.toString());
                map.set("detail_info", "19960229");
                await contactsQuery(map, "contactCard_insert_test_5900", profileContactDataUri);

                var rawContactIdOne = await DAHelper.insert(profileRawContactUri, contcatvalues);
                console.info('logMessage contactCard_insert_test_5900: rawContactId = ' + rawContactIdOne);
                expect(rawContactIdOne > 0).assertTrue();
                var contactDataValues2 = {
                    "raw_contact_id": rawContactIdOne,
                    "content_type": "phone",
                    "detail_info": "111111032"
                };
                var contactDataId1 = await DAHelper.insert(profileContactDataUri, contactDataValues2);
                console.info('logMessage contactCard_insert_test_5900: contactDataId = ' + contactDataId1);
                expect(contactDataId1 > 0).assertTrue();

                var map = new Map();
                map.set("id", contactDataId1.toString());
                map.set("detail_info", "111111032");
                await contactsQuery(map, "contactCard_insert_test_5900", profileContactDataUri);

                await deleteRawContact("contactCard_insert_test_5900");
                await deleteAll(profileContactDataUri, "contactCard_insert_test_5900");
                done();
            } catch (error) {
                console.info('logMessage contactCard_insert_test_5900: insert error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insert_test_6000
     * @tc.name    Add the basic information of a single contact and verify whether the insertion is successful (name,
     * Pinyin, company, position, favorite)
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_6000", 0, async function (done) {
            console.info("------logMessage contactCard_insert_test_6000 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "licheng",
                "company": "TT",
                "position": "developer",
                "favorite": 1,
                "phonetic_name": "licheng||lc"
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insert_test_6000: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();

                var map = new Map();
                map.set("id", rawContactId.toString());
                map.set("display_name", "licheng");
                map.set("company", "TT");
                map.set("position", "developer");
                map.set("favorite", "1");
                map.set("phonetic_name", "licheng||lc");
                await contactsQuery(map, "contactCard_insert_test_6000", profileRawContactUri);
                await deleteRawContact("contactCard_insert_test_6000");
                await deleteAll(profileContactDataUri, "contactCard_insert_test_6000");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_6000: raw_contact insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insertData_test_6100
     * @tc.name    To contact_data Add a full field data to the data table and verify whether the insertion is
     * successful
     * @tc.desc    Function test
     */
        it("contactCard_insertData_test_6100", 0, async function (done) {
            console.info("------logMessage contactCard_insertData_test_6100 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocai",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insertData_test_6100: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var contactDataValues = common.getProfileContactData();
                contactDataValues["raw_contact_id"] = rawContactId;
                console.info('logMessage contactCard_insertData_test_6100: contactDataValues["raw_contact_id"] = ' +
                contactDataValues.raw_contact_id);
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                console.info('logMessage contactCard_insertData_test_6100: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
                var map = common.getProfileContactDataMap();
                map.set("id", contactDataId.toString());
                map.set("raw_contact_id", rawContactId.toString());
                await contactsQuery(map, "contactCard_insertData_test_6100", profileContactDataUri);
                await deleteRawContact("contactCard_insertData_test_6100");
                await deleteAll(profileContactDataUri, "contactCard_insertData_test_6100");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insertData_test_6100: raw_contact insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_update_test_6200
     * @tc.name    Modify the basic information of multiple contacts and verify whether the modification is successful
     * @tc.desc    Function test
     */
        it("contactCard_update_test_6200", 0, async function (done) {
            console.info("--------logMessage contactCard_update_test_6200 is starting!------------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var insertRawContactValues = {
                    "display_name": "xiaocai",
                };
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                expect(rawContactId > 0).assertTrue();
                console.info("logMessage contactCard_update_test_6200: rawContactId = " + rawContactId);
                var insertRawContactValuesOne = {
                    "display_name": "xiaocai1",
                };
                var rawContactIdOne = await DAHelper.insert(profileRawContactUri, insertRawContactValuesOne);
                expect(rawContactIdOne > 0).assertTrue();
                console.info("logMessage contactCard_update_test_6200: rawContactIdOne = " + rawContactIdOne);
                var insertRawContactValuesTwo = {
                    "display_name": "xiaocai1",
                };
                var rawContactIdTwo = await DAHelper.insert(profileRawContactUri, insertRawContactValuesTwo);
                expect(rawContactIdTwo > 0).assertTrue();
                console.info("logMessage contactCard_update_test_6200: rawContactIdTwo = " + rawContactIdTwo);

                var updateValues = {
                    "display_name": "xiaosan"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", rawContactId.toString());
                condition.or();
                condition.equalTo("id", rawContactIdOne.toString());
                condition.or();
                condition.equalTo("id", rawContactIdTwo.toString());
                var updataCode = await DAHelper.update(profileRawContactUri, updateValues, condition);
                console.info("logMessage contactCard_update_test_6200: updataCode = " + updataCode);
                expect(updataCode == 0).assertTrue();
                sleep(1000);
                await queryUpdateThree(
                    "contactCard_update_test_6200", DAHelper, rawContactId, rawContactIdOne, rawContactIdTwo);
                await deleteRawContact("contactCard_update_test_6200");
                done();
            } catch (error) {
                console.info("logMessage contactCard_update_test_6200:  insert error = " + error);
                done();
            }
        });

        async function queryUpdateThree(tag, DAHelper, rawContactId, rawContactIdOne, rawContactIdTwo) {

            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': contactsQuery start ! DAHelper = ' + DAHelper);
            var resultColumns = ["display_name"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactId.toString());
            condition.or();
            condition.equalTo("id", rawContactIdOne.toString());
            condition.or();
            condition.equalTo("id", rawContactIdTwo.toString());
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var name = resultSet.getString(resultSet.getColumnIndex("display_name"));
                            expect(name == "xiaosan").assertEqual(true);
                        } while (resultSet.goToNextRow());
                    }
                }
                resultSet.close();
            } catch (error) {
                console.info(tag + "logMessage contactsQuery: error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_UpdateRawContcat_test_6300
     * @tc.name    Update raw_contact The full field data of the contact table and verify whether the modification is
     * successful
     * @tc.desc    Function test
     */
        it("contactCard_UpdateRawContcat_test_6300", 0, async function (done) {
            console.info("------logMessage contactCard_UpdateRawContcat_test_6300 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, common.getProfileRawContact());
                console.info("logMessage contactCard_UpdateRawContcat_test_6300: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                sleep(2000);
                var updateValues = common.getProfileRawContactUpdate();
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", rawContactId.toString());
                var updataCode = await DAHelper.update(profileRawContactUri, updateValues, condition);
                console.info('logMessage contactCard_UpdateRawContcat_test_6300: updataCode = ' + updataCode);
                sleep(3000);
                var map = common.getProfileRawContactUpdateMap();
                map.set("id", rawContactId.toString());
                await contactsQuery(map, "contactCard_UpdateRawContcat_test_6300", profileRawContactUri);
                await deleteRawContact("contactCard_UpdateRawContcat_test_6300");
                await deleteAll(profileContactDataUri, "contactCard_UpdateRawContcat_test_6300");
                done();
            } catch (error) {
                console.info("logMessage contactCard_UpdateRawContcat_test_6300: raw_contact insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insertDataUpdate_test_6400
     * @tc.name    Update contact_data table, and verify whether the modification is successful
     * @tc.desc    Function test
     */
        it("contactCard_insertDataUpdate_test_6400", 0, async function (done) {
            console.info("------logMessage contactCard_insertDataUpdate_test_6400 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocai",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insertDataUpdate_test_6400: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();

                var contactDataValues = common.getProfileContactData();
                contactDataValues["raw_contact_id"] = rawContactId;
                console.info('logMessage contactCard_insertDataUpdate_test_6400: contactDataValues.raw_contact_id = ' +
                contactDataValues.raw_contact_id);
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                console.info('logMessage contactCard_insertDataUpdate_test_6400: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();

                var updateValues = common.getProfileContactDataUpdate();
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", contactDataId.toString());
                var updataCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                console.info('logMessage contactCard_insertDataUpdate_test_6400: updataCode = ' + updataCode);
                sleep(1000);
                var map = common.getProfileContactDataUpdateMap();
                map.set("id", contactDataId.toString());
                map.set("raw_contact_id", rawContactId.toString());
                await contactsQuery(map, "contactCard_insertDataUpdate_test_6400", profileContactDataUri);
                await deleteRawContact("contactCard_insertDataUpdate_test_6400");
                await deleteAll(profileContactDataUri, "contactCard_insertDataUpdate_test_6400");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insertDataUpdate_test_6400: raw_contact insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insertDataUpdateAll_test_6500
     * @tc.name    Update all contact details and verify that the modification was successful
     * @tc.desc    Function test
     */
        it("contactCard_insertDataUpdateAll_test_6500", 0, async function (done) {
            console.info("------logMessage contactCard_insertDataUpdateAll_test_6500 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocai",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insertDataUpdateAll_test_6500: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                await insertData(rawContactId, "name", "xiaocai", "");
                await insertData(rawContactId, "phone", "6500", "");
                await insertData(rawContactId, "organization", "TTTTT", "Deve");
                await insertData(rawContactId, "nickname", "xxxxcai", "");
                await insertData(rawContactId, "email", "111@fox.com", "");
                await insertData(rawContactId, "postal_address", "dddd", "");
                await insertData(rawContactId, "note", "caicai", "");
                await insertData(rawContactId, "im", "999999999", "");
                await insertData(rawContactId, "contact_event", "1125", "");
                await insertData(rawContactId, "website", "wwww.xxx.com", "");
                await insertData(rawContactId, "relation", "xiaobai", "");
                await insertData(rawContactId, "group_membership", 1, "");
                await insertData(rawContactId, "contact_misc", "5678", "");
                var updateValues = {
                    "detail_info": "xiaocai"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("raw_contact_id", rawContactId.toString());
                var updataCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                console.info('logMessage contactCard_insertDataUpdateAll_test_6500: updataCode = ' + updataCode);
                sleep(1000);
                await queryUpdateAllData("contactCard_insertDataUpdateAll_test_6500", DAHelper, rawContactId);
                await deleteRawContact("contactCard_insertDataUpdateAll_test_6500");
                await deleteAll(profileContactDataUri, "contactCard_insertDataUpdateAll_test_6500");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insertDataUpdateAll_test_6500: raw_contact insert error = " + error);
                done();
            }
        });

        async function queryUpdateAllData(tag, DAHelper, rawContactId) {

            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': contactsQuery start ! DAHelper = ' + DAHelper);
            var resultColumns = ["detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("row_contact_id", rawContactId.toString());
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var detail_info = resultSet.getString(resultSet.getColumnIndex("detail_info"));
                            expect(detail_info == "xiaocai").assertEqual(true);
                        } while (resultSet.goToNextRow());
                    }
                }
                resultSet.close();
            } catch (error) {
                console.info(tag + "logMessage contactsQuery: error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_insertFavorite_test_6600
     * @tc.name    Add multiple contacts to favorites and verify that the favorites field is changed
     * @tc.desc    Function test
     */
        it("contactCard_insertFavorite_test_6600", 0, async function (done) {
            console.info("------logMessage contactCard_insertFavorite_test_6600 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            try {
                var insertRawContactValues = {
                    "display_name": "xiaocai", "favorite": 1
                };
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insertFavorite_test_6600: rawContactId = " + rawContactId);

                var insertRawContactValuesOne = {
                    "display_name": "xiaotian", "favorite": 1
                };
                var rawContactIdOne = await DAHelper.insert(profileRawContactUri, insertRawContactValuesOne);
                console.info("logMessage contactCard_insertFavorite_test_6600: rawContactId = " + rawContactIdOne);

                var insertRawContactValuesTwo = {
                    "display_name": "xiaoli", "favorite": 1
                };
                var rawContactIdTwo = await DAHelper.insert(profileRawContactUri, insertRawContactValuesTwo);
                console.info("logMessage contactCard_insertFavorite_test_6600: rawContactId = " + rawContactIdTwo);

                sleep(1000);
                await queryThreeFavorite(
                    "contactCard_insertFavorite_test_6600", DAHelper, rawContactId, rawContactIdOne, rawContactIdTwo, 1);
                await deleteRawContact("contactCard_insertFavorite_test_6600");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insertFavorite_test_6600: raw_contact insert error = " + error);
                done();
            }
        });

        async function queryThreeFavorite(tag, DAHelper, rawContactId, rawContactIdOne, rawContactIdTwo, favoritevalue) {

            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': queryThreeFavorite start ! DAHelper = ' + DAHelper);
            var resultColumns = ["favorite"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactId.toString());
            condition.or();
            condition.equalTo("id", rawContactIdOne.toString());
            condition.or();
            condition.equalTo("id", rawContactIdTwo.toString());
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var favorite = resultSet.getInt(resultSet.getColumnIndex("favorite"));
                            expect(favorite == favoritevalue).assertEqual(true);
                        } while (resultSet.goToNextRow());
                    }
                }
                resultSet.close();
            } catch (error) {
                console.info(tag + "logMessage queryThreeFavorite: error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_unFavorite_test_6700
     * @tc.name    Remove favorites from multiple contacts and verify that the favorites field has changed
     * @tc.desc    Function test
     */
        it("contactCard_unFavorite_test_6700", 0, async function (done) {
            console.info("------logMessage contactCard_unFavorite_test_6700 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            try {
                var insertRawContactValues = {
                    "display_name": "xiaocai", "favorite": 1
                };
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_unFavorite_test_6700: rawContactId = " + rawContactId);

                var insertRawContactValuesOne = {
                    "display_name": "xiaotian", "favorite": 1
                };
                var rawContactIdOne = await DAHelper.insert(profileRawContactUri, insertRawContactValuesOne);
                console.info("logMessage contactCard_unFavorite_test_6700: rawContactId = " + rawContactIdOne);

                var insertRawContactValuesTwo = {
                    "display_name": "xiaoli", "favorite": 1
                };
                var rawContactIdTwo = await DAHelper.insert(profileRawContactUri, insertRawContactValuesTwo);
                console.info("logMessage contactCard_unFavorite_test_6700: rawContactId = " + rawContactIdTwo);

                var updateValues = {
                    "favorite": 0
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("id", rawContactId.toString());
                condition.or();
                condition.equalTo("id", rawContactIdOne.toString());
                condition.or();
                condition.equalTo("id", rawContactIdTwo.toString());
                var updataCode = await DAHelper.update(profileRawContactUri, updateValues, condition);
                console.info("logMessage contactCard_unFavorite_test_6700: updataCode = " + updataCode);
                sleep(1000);
                await queryThreeFavorite(
                    "contactCard_unFavorite_test_6700", DAHelper, rawContactId, rawContactIdOne, rawContactIdTwo, 0);
                await deleteRawContact("contactCard_unFavorite_test_6700");
                done();
            } catch (error) {
                console.info("logMessage contactCard_unFavorite_test_6700: raw_contact insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insertContactBlocklist_test_6800
     * @tc.name    Add multiple records to the black list and verify whether the insertion is successful
     * @tc.desc    Function test
     */
        it("contactCard_insertContactBlocklist_test_6800", 0, async function (done) {
            console.info("--------logMessage contactCard_insertContactBlocklist_test_6800 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var batchInsertCode = await DAHelper.insert(profileBlocklistUri, common.getProfileBlockList());
                console.info("logMessage contactCard_insertContactBlocklist_test_6800: insert = " + batchInsertCode);
                var batchInsertCodeOne = await DAHelper.insert(profileBlocklistUri, common.getProfileBlockList());
                console.info("logMessage contactCard_insertContactBlocklist_test_6800: insert = " + batchInsertCodeOne);
                var batchInsertCodeTwo = await DAHelper.insert(profileBlocklistUri, common.getProfileBlockList());
                console.info("logMessage contactCard_insertContactBlocklist_test_6800: insert = " + batchInsertCodeTwo);
                sleep(1000);
                expect(batchInsertCode > 0).assertTrue();
                await query(3);
                await deleteAll(profileBlocklistUri, "contactCard_insertContactBlocklist_test_6800");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insertContactBlocklist_test_6800: batchInsert error = " + error);
                done();
            }

            async function query(size) {
                var resultColumns = [];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("phone_number", "123456789");
                try {
                    var resultSet = await DAHelper.query(profileBlocklistUri, resultColumns, condition);
                    sleep(1000);
                    console.info('contactCard_insertContactBlocklist_test_6800 :resultSet.goToFirstRow()  = ' +
                    resultSet.goToFirstRow());
                    console.info(
                        'contactCard_insertContactBlocklist_test_6800 : query result is  = ' + resultSet.rowCount == size);
                    expect(resultSet.rowCount == size).assertEqual(true);
                    resultSet.close();
                } catch (error) {
                    console.info(" contactCard_insertContactBlocklist_test_6800  error = " + error);
                }
            }
        });

        /**
     * @tc.number  contactCard_removeContactBlocklist_test_6900
     * @tc.name    Delete multiple records from the black list and verify whether the deletion is successful
     * @tc.desc    Function test
     */
        it("contactCard_removeContactBlocklist_test_6900", 0, async function (done) {
            console.info("--------logMessage contactCard_removeContactBlocklist_test_6900 is starting!-------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var batchInsertCode = await DAHelper.insert(profileBlocklistUri, common.getProfileBlockList());
                console.info(
                    "logMessage contactCard_removeContactBlocklist_test_6900: batchInsertCode = " + batchInsertCode);
                var batchInsertCodeOne = await DAHelper.insert(profileBlocklistUri, common.getProfileBlockList());
                console.info("logMessage contactCard_removeContactBlocklist_test_6900: insert = " + batchInsertCodeOne);
                var batchInsertCodeTwo = await DAHelper.insert(profileBlocklistUri, common.getProfileBlockList());
                console.info("logMessage contactCard_removeContactBlocklist_test_6900: insert = " + batchInsertCodeTwo);
                sleep(1000);
                expect(batchInsertCode > 0).assertTrue();
                await deleteAll(profileBlocklistUri, "contactCard_removeContactBlocklist_test_6900");
                await query(0);
                done();
            } catch (error) {
                console.info("logMessage contactCard_removeContactBlocklist_test_6900: batchInsert error = " + error);
                done();
            }
            async function query(size) {
                var resultColumns = [];
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("phone_number", "123456789");
                try {
                    var resultSet = await DAHelper.query(profileBlocklistUri, resultColumns, condition);
                    console.info('contactCard_removeContactBlocklist_test_6900 : resultSet  = ' + resultSet);
                    expect(resultSet.rowCount == size).assertEqual(true);
                    resultSet.close();
                } catch (error) {
                    console.info(" contactCard_removeContactBlocklist_test_6900  error = " + error);
                }
            }
        });

        /**
     * @tc.number  contactCard_insertGroup_test_7000
     * @tc.name    Add multiple contacts to favorites and verify that the favorites field is changed
     * @tc.desc    Function test
     */
        it("contactCard_insertGroup_test_7000", 0, async function (done) {
            console.info("------logMessage contactCard_insertGroup_test_7000 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            try {
                var insertRawContactValues = {
                    "display_name": "xiaocai",
                };
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insertGroup_test_7000: rawContactId = " + rawContactId);
                await insertData(rawContactId, "group_membership", 1, "");

                var insertRawContactValuesOne = {
                    "display_name": "xiaotian",
                };
                var rawContactIdOne = await DAHelper.insert(profileRawContactUri, insertRawContactValuesOne);
                console.info("logMessage contactCard_insertGroup_test_7000: rawContactId = " + rawContactIdOne);
                await insertData(rawContactIdOne, "group_membership", 1, "");
                var insertRawContactValuesTwo = {
                    "display_name": "xiaoli",
                };
                var rawContactIdTwo = await DAHelper.insert(profileRawContactUri, insertRawContactValuesTwo);
                console.info("logMessage contactCard_insertGroup_test_7000: rawContactId = " + rawContactIdTwo);
                await insertData(rawContactIdTwo, "group_membership", 1, "");
                sleep(1000);
                await queryThreeGroup(
                    "contactCard_insertGroup_test_7000", DAHelper, rawContactId, rawContactIdOne, rawContactIdTwo, 1);
                await deleteRawContact("contactCard_insertGroup_test_7000");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insertGroup_test_7000: raw_contact insert error = " + error);
                done();
            }
        });

        async function queryThreeGroup(tag, DAHelper, rawContactId, rawContactIdOne, rawContactIdTwo, groupId) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': queryThreeGroup start ! DAHelper = ' + DAHelper);
            var resultColumns = ["detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", rawContactId.toString());
            condition.or();
            condition.equalTo("id", rawContactIdOne.toString());
            condition.or();
            condition.equalTo("id", rawContactIdTwo.toString());
            try {
                var resultSet = await DAHelper.query(profileRawContactUri, resultColumns, condition);
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var group = resultSet.getInt(resultSet.getColumnIndex("detail_info"));
                            expect(group == groupId).assertEqual(true);
                        } while (resultSet.goToNextRow());
                    }
                }
                resultSet.close();
            } catch (error) {
                console.info(tag + "logMessage queryThreeGroup: error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_insert_test_7100
     * @tc.name    Add a full field data to the groups table and verify whether the insertion is successful
     * @tc.desc    Function test
     */
        it("contactCard_insert_test_7100", 0, async function (done) {
            console.info("---------logMessage contactCard_insert_test_7100 is starting!----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var groupId = await DAHelper.insert(profileGroupUri, common.getProfileGroup());
                console.info("logMessage contactCard_insert_test_7100: groupId = " + groupId);
                expect(groupId > 0).assertTrue();
                var map = common.getProfileGroupMap();
                map.set("id", groupId.toString());
                await contactsQuery(map, "contactCard_insert_test_7100", profileGroupUri);
                await deleteAll(profileGroupUri, "contactCard_insert_test_7100");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insert_test_7100: group insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_delete_test_7200
     * @tc.name    Add a full field data to the groups table and verify whether the insertion is successful
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_7200", 0, async function (done) {
            console.info("------logMessage contactCard_delete_test_7200 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            try {
                var insertRawContactValues = {
                    "display_name": "xiaocai",
                };
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_delete_test_7200: rawContactId = " + rawContactId);
                await insertData(rawContactId, "group_membership", 1, "");

                var insertRawContactValuesOne = {
                    "display_name": "xiaotian",
                };
                var rawContactIdOne = await DAHelper.insert(profileRawContactUri, insertRawContactValuesOne);
                console.info("logMessage contactCard_delete_test_7200: rawContactId = " + rawContactIdOne);
                await insertData(rawContactIdOne, "group_membership", 1, "");
                var insertRawContactValuesTwo = {
                    "display_name": "xiaoli",
                };
                var rawContactIdTwo = await DAHelper.insert(profileRawContactUri, insertRawContactValuesTwo);
                console.info("logMessage contactCard_delete_test_7200: rawContactId = " + rawContactIdTwo);
                await insertData(rawContactIdTwo, "group_membership", 1, "");
                sleep(1000);
                await deleteAll(profileContactDataUri, "contactCard_delete_test_7200");
                sleep(1000);
                await queryZeroGroup(
                    "contactCard_delete_test_7200", DAHelper, rawContactId, rawContactIdOne, rawContactIdTwo);
                await deleteRawContact("contactCard_delete_test_7200");
                done();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_7200: raw_contact insert error = " + error);
                done();
            }
        });

        async function queryZeroGroup(tag, DAHelper, rawContactId, rawContactIdOne, rawContactIdTwo) {

            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': queryZeroGroup start ! DAHelper = ' + DAHelper);
            var resultColumns = ["detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            condition.or();
            condition.equalTo("raw_contact_id", rawContactIdOne.toString());
            condition.or();
            condition.equalTo("raw_contact_id", rawContactIdTwo.toString());
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                console.info(tag + ': queryZeroGroup! resultSet.rowCount  = ' + resultSet.rowCount);
                expect(resultSet.rowCount == 0).assertEqual(true);
                resultSet.close();
            } catch (error) {
                console.info(tag + "logMessage queryZeroGroup: error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_delete_test_7300
     * @tc.name    Delete the full field data of a groups table and verify whether the deletion is successful
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_7300", 0, async function (done) {
            console.info("---------logMessage contactCard_delete_test_7300 is starting!----------");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var groupId = await DAHelper.insert(profileGroupUri, common.getProfileGroup());
                console.info("logMessage contactCard_delete_test_7300: groupId = " + groupId);
                expect(groupId > 0).assertTrue();
                var map = common.getProfileGroupMap();
                map.set("id", groupId.toString());
                await contactsQuery(map, "contactCard_delete_test_7300", profileGroupUri);
                await deleteAll(profileGroupUri, "contactCard_delete_test_7300");
                done();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_7300: group insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_delete_test_7400
     * @tc.name    Add a raw_contact all fields, delete raw_contact
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_7400", 0, async function (done) {
            console.info("------logMessage contactCard_delete_test_7400 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, common.getProfileRawContact());
                sleep(1000);
                console.info('logMessage contactCard_delete_test_7400: rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var map = common.getProfileRawContactMap();
                map.set("id", rawContactId.toString());
                map.set("version", "2");
                map.set("is_deleted", "1");
                await deleteAll(profileRawContactUri, "contact_Delete_test_7400");
                sleep(1000);
                await contactsQuery(map, "contactCard_delete_test_7400", profileRawContactUri);
                var conditionAll = new ohosDataAbility.DataAbilityPredicates();
                conditionAll.greaterThan("id", "0");
                var code = await DAHelper.delete(profileDeletedContactsUri, conditionAll);
                console.info('contactCard_delete_test_7400 : Completely delete code = ' + code);
                expect(code == 0).assertTrue();
                done();
            } catch (error) {
                console.info('logMessage contactCard_delete_test_7400: insert error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_delete_test_7500
     * @tc.name    Delete contact_data Single full field data in data table
     * @tc.desc    Function test
     */
        it("contactCard_delete_test_7500", 0, async function (done) {
            console.info("------logMessage contactCard_delete_test_7500 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocai",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_delete_test_7500: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var contactDataValues = common.getProfileContactData();
                contactDataValues["raw_contact_id"] = rawContactId;
                var contactDataId = await DAHelper.insert(profileContactDataUri, contactDataValues);
                console.info('logMessage contactCard_delete_test_7500: contactDataId = ' + contactDataId);
                expect(contactDataId > 0).assertTrue();
                await deleteAll(profileContactDataUri, "contactCard_delete_test_7500");
                var map = new Map();
                map.set("id", contactDataId.toString());
                await queryIdForDelete(map, "contactCard_delete_test_7500", profileContactDataUri);
                await deleteRawContact("contactCard_delete_test_7500");
                done();
            } catch (error) {
                console.info("logMessage contactCard_delete_test_7500: raw_contact insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insertDataAll_test_7600
     * @tc.name    Add all details of a single contact and verify whether the insertion is successful (name, mobile
     * number, company, position, nickname, email, home, remarks, aim)
     * @tc.desc    Function test
     */
        it("contactCard_insertDataAll_test_7600", 0, async function (done) {
            console.info("------logMessage contactCard_insertDataAll_test_7600 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocai",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insertDataAll_test_7600: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var contactdata1 = await insertData(rawContactId, "name", "xiaocai", "");
                await queryAlldata("contactCard_insertDataAll_test_7600", DAHelper, rawContactId, contactdata1, "xiaocai");
                var contactdata2 = await insertData(rawContactId, "phone", "6500", "");
                await queryAlldata("contactCard_insertDataAll_test_7600", DAHelper, rawContactId, contactdata2, "6500");
                var contactdatd3 = await insertData(rawContactId, "organization", "TTTTT", "Deve");
                await queryAlldata("contactCard_insertDataAll_test_7600", DAHelper, rawContactId, contactdatd3, "TTTTT");
                var contactdatd4 = await insertData(rawContactId, "nickname", "xxxxcai", "");
                await queryAlldata("contactCard_insertDataAll_test_7600", DAHelper, rawContactId, contactdatd4, "xxxxcai");
                var contactdatd5 = await insertData(rawContactId, "email", "111@fox.com", "");
                await queryAlldata(
                    "contactCard_insertDataAll_test_7600", DAHelper, rawContactId, contactdatd5, "111@fox.com");
                var contactdatd6 = await insertData(rawContactId, "postal_address", "dddd", "");
                await queryAlldata("contactCard_insertDataAll_test_7600", DAHelper, rawContactId, contactdatd6, "dddd");
                var contactdatd7 = await insertData(rawContactId, "note", "caicai", "");
                await queryAlldata("contactCard_insertDataAll_test_7600", DAHelper, rawContactId, contactdatd7, "caicai");
                var contactdatd8 = await insertData(rawContactId, "im", "999999999", "");
                await queryAlldata(
                    "contactCard_insertDataAll_test_7600", DAHelper, rawContactId, contactdatd8, "999999999");
                sleep(1000);
                await deleteRawContact("contactCard_insertDataAll_test_7600");
                await deleteAll(profileContactDataUri, "contactCard_insertDataAll_test_7600");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insertDataAll_test_7600: raw_contact insert error = " + error);
                done();
            }
        });

        async function queryAlldata(tag, DAHelper, rawContactId, contactdata, values) {
            var resultColumns = ["raw_contact_id", "detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("id", contactdata.toString());
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var id = resultSet.getString(resultSet.getColumnIndex("raw_contact_id"));
                            console.info(tag + " result raw_contact_id = " + id);
                            expect(id == rawContactId).assertEqual(true);
                            var info = resultSet.getString(resultSet.getColumnIndex("detail_info"));
                            console.info(tag + "result info = " + info + " value = " + values);
                            expect(info == values).assertEqual(true);
                        } while (resultSet.goToNextRow());
                    }
                }
                resultSet.close();
            } catch (error) {
                console.info(tag + " :logMessage queryIdForDelete: error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_insertDataAll_test_7700
     * @tc.name    Add all details of a single contact and verify whether the insertion is successful (name, mobile
     * number, company, position, nickname, email, home, remarks, aim, birthday, website, assistant, group, phone ring)
     * @tc.desc    Function test
     */
        it("contactCard_insertDataAll_test_7700", 0, async function (done) {
            console.info("------logMessage contactCard_insertDataAll_test_7700 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);

            var insertRawContactValues = {
                "display_name": "xiaocai",
            };
            try {
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insertDataAll_test_7700: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                var contactdata1 = await insertData(rawContactId, "name", "xiaocai", "");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdata1, "xiaocai");
                var contactdata2 = await insertData(rawContactId, "phone", "6500", "");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdata2, "6500");
                var contactdatd3 = await insertData(rawContactId, "organization", "TTTTT", "Deve");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd3, "TTTTT");
                var contactdatd4 = await insertData(rawContactId, "nickname", "xxxxcai", "");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd4, "xxxxcai");
                var contactdatd5 = await insertData(rawContactId, "email", "111@fox.com", "");
                await queryAlldata(
                    "contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd5, "111@fox.com");
                var contactdatd6 = await insertData(rawContactId, "postal_address", "dddd", "");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd6, "dddd");
                var contactdatd7 = await insertData(rawContactId, "note", "caicai", "");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd7, "caicai");
                var contactdatd8 = await insertData(rawContactId, "im", "999999999", "");
                await queryAlldata(
                    "contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd8, "999999999");
                var contactdatd9 = await insertData(rawContactId, "contact_event", "1125", "");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd9, "1125");
                var contactdatd10 = await insertData(rawContactId, "website", "wwww.xxx.com", "");
                await queryAlldata(
                    "contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd10, "wwww.xxx.com");
                var contactdatd11 = await insertData(rawContactId, "relation", "xiaobai", "");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd11, "xiaobai");
                var contactdatd12 = await insertData(rawContactId, "group_membership", 1, "");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd12, 1);
                var contactdatd13 = await insertData(rawContactId, "contact_misc", "5678", "");
                await queryAlldata("contactCard_insertDataAll_test_7700", DAHelper, rawContactId, contactdatd13, "5678");
                sleep(1000);
                await deleteRawContact("contactCard_insertDataAll_test_7700");
                await deleteAll(profileContactDataUri, "contactCard_insertDataAll_test_7700");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insertDataAll_test_7700: raw_contact insert error = " + error);
                done();
            }
        });

        /**
     * @tc.number  contactCard_insertDataUpdateMore_test_7800
     * @tc.name    Modify multiple contact details and verify whether the modification is successful
     * @tc.desc    Function test
     */
        it("contactCard_insertDataUpdateMore_test_7800", 0, async function (done) {
            console.info("------logMessage contactCard_insertDataUpdateMore_test_7800 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var insertRawContactValues = {
                    "display_name": "xiaocai"
                };
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insertDataUpdateMore_test_7800: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();

                var insertRawContactValues = {
                    "display_name": "xiaotian"
                };
                var rawContactIdFirst = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_insertDataUpdateMore_test_7800: rawContactIdFirst = " + rawContactIdFirst);
                expect(rawContactIdFirst > 0).assertTrue();
                await insertData(rawContactId, "name", "xiaotian", "");

                var updateValues = {
                    "detail_info": "xiaocai7800"
                };
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.equalTo("raw_contact_id", rawContactId.toString());
                condition.or();
                condition.equalTo("raw_contact_id", rawContactIdFirst.toString())
                var updataCode = await DAHelper.update(profileContactDataUri, updateValues, condition);
                console.info('logMessage contactCard_insertDataUpdateMore_test_7800: updataCode = ' + updataCode);
                sleep(1000);
                await insertDataUpdateMoreQuery(
                    "contactCard_insertDataUpdateMore_test_7800", DAHelper, rawContactId, rawContactIdFirst);
                await deleteRawContact("contactCard_insertDataUpdateMore_test_7800");
                await deleteAll(profileContactDataUri, "contactCard_insertDataUpdateMore_test_7800");
                done();
            } catch (error) {
                console.info("logMessage contactCard_insertDataUpdateMore_test_7800: raw_contact insert error = " + error);
                done();
            }
        });

        async function insertDataUpdateMoreQuery(tag, DAHelper, rawContactId, rawContactIdFirst) {
            var resultColumns = ["detail_info"];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.equalTo("raw_contact_id", rawContactId.toString());
            condition.or();
            condition.equalTo("raw_contact_id", rawContactIdFirst.toString())
            try {
                var resultSet = await DAHelper.query(profileContactDataUri, resultColumns, condition);
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var info = resultSet.getString(resultSet.getColumnIndex("detail_info"));
                            console.info(tag + "result info = " + info);
                            expect(info == "xiaocai7800").assertEqual(true);
                        } while (resultSet.goToNextRow());
                    }
                }
                resultSet.close();
            } catch (error) {
                console.info(tag + " :logMessage : error = " + error);
            }
        }

        /**
     * @tc.number  contactCard_deleterestore_test_7900
     * @tc.name    Modify multiple contact details and verify whether the modification is successful
     * @tc.desc    Function test
     */
        it("contactCard_deleterestore_test_7900", 0, async function (done) {
            console.info("------logMessage contactCard_deleterestore_test_7900 is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            try {
                var insertRawContactValues = {
                    "display_name": "xaioli7900"
                };
                var rawContactId = await DAHelper.insert(profileRawContactUri, insertRawContactValues);
                console.info("logMessage contactCard_deleterestore_test_7900: rawContactId = " + rawContactId);
                expect(rawContactId > 0).assertTrue();
                sleep(1000);
                var condition1 = new ohosDataAbility.DataAbilityPredicates();
                condition1.equalTo("id", rawContactId.toString());
                var deleteCode = await DAHelper.delete(profileRawContactUri, condition1);
                console.info("contactCard_deleterestore_test_7900 : deleteCode = " + deleteCode);
                expect(deleteCode == 0).assertTrue();
                sleep(1000);

                var condition2 = new ohosDataAbility.DataAbilityPredicates();
                condition2.equalTo("id", rawContactId.toString());
                var updatevalue = {
                    "is_deleted": 0
                };
                var updateCode = await DAHelper.update(profileRawContactUri, updatevalue, condition2);
                console.info('contactCard_deleterestore_test_7900 : update = ' + updateCode);
                sleep(1000);

                var condition3 = new ohosDataAbility.DataAbilityPredicates();
                condition3.equalTo("id", rawContactId.toString());
                var deleteCode = await DAHelper.delete(profileDeletedRawContactRecord, condition3);

                var map = new Map();
                map.set("id", rawContactId.toString());
                map.set("is_deleted", "0");
                await contactsQuery(map, "contactCard_deleterestore_test_7900", profileRawContactUri);
                done();
            } catch (error) {
                console.info("logMessage contactCard_deleterestore_test_7900: raw_contact insert error = " + error);
                done();
            }
        });

        afterAll(async function () {
            var tag = "Contacts_test_start_deleted";
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            var DAHelperContact = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': start ! DAHelperContact = ' + DAHelperContact);
            var deleteBlockList = await DAHelperContact.delete(profileBlocklistUri, condition);
            console.info(tag + " : logMessage : deleteBlockList = " + deleteBlockList);
            var deletedGroup = await DAHelperContact.delete(profileGroupUri, condition);
            console.info(tag + " : logMessage : deletedGroup = " + deletedGroup);
            var dataDeletedCode = await DAHelperContact.delete(profileContactDataUri, condition);
            sleep(2000);
            console.info(tag + " : logMessage : dataDeletedCode = " + dataDeletedCode);
            var rawContactDeleted = await DAHelperContact.delete(profileRawContactUri, condition);
            sleep(2000);
            console.info(tag + " : logMessage : rawContactDeleted = " + rawContactDeleted);
            var deleted = await DAHelperContact.delete(profileDeletedContactsUri, condition);
            sleep(2000);
            console.info(tag + " : logMessage : deleted = " + deleted);
        });
    });
}