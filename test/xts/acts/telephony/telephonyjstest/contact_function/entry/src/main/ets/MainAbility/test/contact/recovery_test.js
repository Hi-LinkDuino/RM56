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
const rawContactUri = "dataability:///com.ohos.contactsdataability/contacts/raw_contact";
const backup = "dataability:///com.ohos.contactsdataability/contacts/backup";
const recover = "dataability:///com.ohos.contactsdataability/contacts/recover";
const deletedUri = "dataability:///com.ohos.contactsdataability/contacts/deleted_raw_contact";

export default function RecoveryTest() {
    describe('RecoveryTest', function () {
        function sleep(numberMillis) {
            var now = new Date();
            var exitTime = now.getTime() + numberMillis;
            while (true) {
                now = new Date();
                if (now.getTime() > exitTime)
                return;
            }
        }

        async function contactsQuery(tag, uri, size) {
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info(tag + ': contactsQuery start ! DAHelper = ' + DAHelper);
            var resultColumns = [];
            var condition = new ohosDataAbility.DataAbilityPredicates();
            condition.greaterThan("id", "0");
            try {
                var resultSet = await DAHelper.query(uri, resultColumns, condition);
                console.info(tag + ' contactsQuery resultSet.rowCount is = ' + resultSet.rowCount);
                console.info(tag + ' contactsQuery size is = ' + size);
                expect(resultSet.rowCount == size).assertEqual(true);
                if (resultSet.rowCount > 0) {
                    if (resultSet.goToFirstRow()) {
                        do {
                            var id = resultSet.getString(resultSet.getColumnIndex("id"));
                            console.info(tag + ':logMessage contactsQuery id = ' + id);
                            var name = resultSet.getString(resultSet.getColumnIndex("display_name"));
                            console.info(tag + ':logMessage contactsQuery name = ' + name);
                        } while (resultSet.goToNextRow())
                    }
                }
                resultSet.close();
            } catch (error) {
                console.info(tag + "logMessage contactsQuery: error = " + error);
            }
        }

        async function rawContactInsert(name) {
            console.info("------logMessage rawContactInsert is starting!-----");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            console.info('logMessage get DAHelper success! DAHelper = ' + DAHelper);
            var insertValue = {
                "display_name": name,
            }
            try {
                var rawContactId = await DAHelper.insert(rawContactUri, insertValue);
                console.info('logMessage rawContactInsert: rawContactId = ' + rawContactId);
                expect(rawContactId > 0).assertTrue();
            } catch (error) {
                console.info('logMessage rawContactInsert: insert error = ' + error);
            }
            return rawContactId;
        }

        async function deleteRawContact(tag) {
            try {
                var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
                var condition = new ohosDataAbility.DataAbilityPredicates();
                condition.greaterThan("id", "0");
                condition.and();
                condition.equalTo("is_deleted", "0");
                await DAHelper.delete(rawContactUri, condition);
                sleep(5000);
                var conditionAll = new ohosDataAbility.DataAbilityPredicates();
                conditionAll.greaterThan("id", "0");
                await DAHelper.delete(deletedUri, conditionAll);
            } catch (error) {
                console.info(tag + ': deleteRawContact error = ' + error);
            }
        }

        /**
     * @tc.number  recovery_test_100
     * @tc.name    backup database
     * @tc.desc    Function test
     */
        it("recovery_test_100", 0, async function (done) {
            await deleteRawContact("recovery_test_100");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            var updateValues = {};
            var condition = new ohosDataAbility.DataAbilityPredicates();
            try {
                var updateCode = await DAHelper.update(backup, updateValues, condition);
                sleep(2000);
                console.info('logMessage recovery_test_100: updateCode = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                sleep(2000);
                await contactsQuery("recovery_test_100", rawContactUri, 0);
                done();
            } catch (error) {
                console.info('logMessage recovery_test_100: raw_contact update error = ' + error);
                done();
            }
        });

        /**
     * @tc.number  recovery_test_200
     * @tc.n    ame    recovery database
     * @tc.desc    Function test
     */
        it("recovery_test_200", 0, async function (done) {
            await deleteRawContact("recovery_test_200");
            var DAHelper = featureAbility.acquireDataAbilityHelper(URI_CONTACTS);
            var updateValues = {};
            var condition = new ohosDataAbility.DataAbilityPredicates();
            rawContactInsert("liming");
            rawContactInsert("xiaolilili");
            sleep(2000);
            var updateCode = await DAHelper.update(backup, updateValues, condition);
            sleep(2000);
            console.info(' recovery_test_200: backup = ' + updateCode);
            expect(updateCode == 0).assertTrue();
            sleep(1000);
            try {
                var updateCode = await DAHelper.update(recover, updateValues, condition);
                sleep(3000);
                console.info('logMessage recovery_test_200: recover = ' + updateCode);
                expect(updateCode == 0).assertTrue();
                await contactsQuery("recovery_test_200", rawContactUri, 2);
                await deleteRawContact("recovery_test_200");
                done();
            } catch (error) {
                console.info('logMessage recovery_test_200: raw_contact update error = ' + error);
                done();
            }
        });
    });
}