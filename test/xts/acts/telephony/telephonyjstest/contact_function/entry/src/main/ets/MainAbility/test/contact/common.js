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
const contactUri = "dataability:///com.ohos.contactsdataability/contacts/contact";
const rawContactUri = "dataability:///com.ohos.contactsdataability/contacts/raw_contact";
const contactDataUri = "dataability:///com.ohos.contactsdataability/contacts/contact_data";
const groupUri = "dataability:///com.ohos.contactsdataability/contacts/groups";
const contactBlocklistUri = "dataability:///com.ohos.contactsdataability/contacts/contact_blocklist";
const deletedRawContactUri = "dataability:///com.ohos.contactsdataability/contacts/deleted_raw_contact";
const profileRawContactUri = "dataability:///com.ohos.contactsdataability/profile/raw_contact";
const profilecontactUri = "dataability:///com.ohos.contactsdataability/profile/contact";
const profileContactDataUri = "dataability:///com.ohos.contactsdataability/profile/contact_data";
const profileGroupUri = "dataability:///com.ohos.contactsdataability/profile/groups";
const profileBlocklistUri = "dataability:///com.ohos.contactsdataability/profile/contact_blocklist";
const profileDeletedContactsUri = "dataability:///com.ohos.contactsdataability/profile/deleted_raw_contact";

const URI_CALLLOG = "dataability:///com.ohos.calllogability";
const calllogUri = "dataability:///com.ohos.calllogability/calls/calllog";

export default {
    data: {},
    getCallLogInsertMap:
    function () {
        var calllogInsert100Map = new Map([
            ["phone_number", "123456789"], ["display_name", "xxw"], ["voicemail_uri", "uri"],
            ["sim_type", "1"], ["is_hd", "2"], ["is_read", "1"], ["ring_duration", "900"],
            ["talk_duration", "200"], ["format_number", "123 456 789"], ["quicksearch_key", "2"],
            ["number_type", "2"], ["number_type_name", "tian"], ["begin_time", "200"], ["end_time", "300"],
            ["answer_state", "1"], ["create_time", "400"], ["number_location", "dizhi"], ["photo_id", "1"],
            ["photo_uri", "666"], ["country_iso_code", "2"], ["extra1", "test1"], ["extra2", "test2"],
            ["extra3", "test3"], ["extra4", "test4"], ["extra5", "test5"], ["extra6", "test6"]
        ]);
        return calllogInsert100Map;
    },
    getCallLogInsert:
    function () {
        var calllogInsert100 = {
            "phone_number": "123456789",
            "display_name": "xxw",
            "voicemail_uri": "uri",
            "sim_type": 1,
            "is_hd": 2,
            "is_read": 1,
            "ring_duration": 900,
            "talk_duration": 200,
            "format_number": "123 456 789",
            "quicksearch_key": "2",
            "number_type": 2,
            "number_type_name": "tian",
            "begin_time": 200,
            "end_time": 300,
            "answer_state": 1,
            "create_time": 400,
            "number_location": "dizhi",
            "photo_id": 1,
            "photo_uri": "666",
            "country_iso_code": 2,
            "extra1": "test1",
            "extra2": "test2",
            "extra3": "test3",
            "extra4": "test4",
            "extra5": "test5",
            "extra6": "test6",
        };
        return calllogInsert100;
    },
    getCallLogResultColumns:
    function () {
        var resultColumns = [
            "id", "phone_number", "display_name", "call_direction", "voicemail_uri", "sim_type", "is_hd", "is_read",
            "ring_duration", "talk_duration", "format_number", "quicksearch_key", "number_type", "number_type_name",
            "begin_time", "end_time", "answer_state", "create_time", "number_location", "photo_id", "photo_uri",
            "country_iso_code", "extra1", "extra2", "extra3", "extra4", "extra5", "extra6"
        ];
        return resultColumns;
    },
    getCallLogUpdate:
    function () {
        var updateValues = {
            "phone_number": "987654321",
            "display_name": "6666",
            "voicemail_uri": "7777",
            "sim_type": 4,
            "is_hd": 4,
            "is_read": 4,
            "ring_duration": 400,
            "talk_duration": 400,
            "format_number": "987 654 321",
            "quicksearch_key": "4",
            "number_type": 4,
            "number_type_name": "4tian",
            "begin_time": 400,
            "end_time": 400,
            "answer_state": 4,
            "create_time": 40000,
            "number_location": "4dizhi",
            "photo_id": 4,
            "photo_uri": "4444",
            "country_iso_code": 4,
            "extra1": "test7",
            "extra2": "test7",
            "extra3": "test7",
            "extra4": "test7",
            "extra5": "test7",
            "extra6": "test7",
        };
        return updateValues;
    },
    getCallLogUpdateMap:
    function () {
        var calllogInsert100Map = new Map([
            ["phone_number", "987654321"], ["display_name", "6666"], ["voicemail_uri", "7777"],
            ["sim_type", "4"], ["is_hd", "4"], ["is_read", "4"], ["ring_duration", "400"],
            ["talk_duration", "400"], ["format_number", "987 654 321"], ["quicksearch_key", "4"],
            ["number_type", "4"], ["number_type_name", "4tian"], ["begin_time", "400"], ["end_time", "400"],
            ["answer_state", "4"], ["create_time", "40000"], ["number_location", "4dizhi"],
            ["photo_id", "4"], ["photo_uri", "4444"], ["country_iso_code", "4"], ["extra1", "test7"],
            ["extra2", "test7"], ["extra3", "test7"], ["extra4", "test7"], ["extra5", "test7"],
            ["extra6", "test7"]
        ]);
        return calllogInsert100Map;
    },
    getCallLogBatchInsert:
    function () {
        var map = new Map([["phone_number", "1600"], ["ring_duration", "333"]]);
        var map2 = new Map([["phone_number", "1600"], ["ring_duration", "600"]]);
        var map3 = new Map([["phone_number", "1600"], ["ring_duration", "600"]]);
        var map4 = new Map([["phone_number", "1600"], ["ring_duration", "600"]]);
        var map5 = new Map([["phone_number", "1600"], ["ring_duration", "600"]]);
        var array = [map, map2, map3, map4, map5];
        return array;
    },
    getProfileRawContact:
    function () {
        var rawContact = {
            "photo_id": 2,
            "photo_file_id": 3,
            "is_transfer_voicemail": 1,
            "personal_ringtone": "test",
            "is_deleted": 0,
            "personal_notification_ringtone": "666",
            "photo_first_name": "7777",
            "account_id": 1,
            "version": 1,
            "sort": "6666",
            "contacted_count": 2,
            "lastest_contacted_time": 2,
            "favorite": 1,
            "favorite_order": "test",
            "phonetic_name": "testname",
            "phonetic_name_type": 1,
            "display_name": "licheng",
            "company": "TTTTT",
            "position": "Developer",
            "read_only": 2,
            "sort_first_letter": "testsort",
            "sync_id": 8,
            "syn_1": "syn_1",
            "syn_2": "syn_2",
            "syn_3": "syn_3",
        };
        return rawContact;
    },
    getProfileRawContactMap:
    function () {
        var rawContactMap = new Map([
            ["photo_id", "2"], ["photo_file_id", "3"], ["is_transfer_voicemail", "1"],
            ["personal_ringtone", "test"], ["is_deleted", "0"],
            [
                "personal_notification_ringtone",
                "666",
            ],
            [
                "photo_first_name",
                "7777",
            ],
            ["account_id", "1"], ["version", "1"], ["sort", "6666"], ["contacted_count", "2"],
            ["lastest_contacted_time", "2"], ["favorite", "1"], ["favorite_order", "test"],
            ["phonetic_name", "testname"], ["phonetic_name_type", "1"], ["display_name", "licheng"],
            ["company", "TTTTT"], ["position", "Developer"], ["read_only", "2"],
            ["sort_first_letter", "testsort"], ["merge_mode", "0"], ["is_need_merge", "1"],
            ["merge_status", "1"], ["is_merge_target", "0"], ["vibration_setting", "0"], ["sync_id", "8"],
            ["syn_1", "syn_1"], ["syn_2", "syn_2"], ["syn_3", "syn_3"]
        ]);
        return rawContactMap;
    },
    getProfileRawContactUpdate:
    function () {
        var rawContact = {
            "photo_id": 21,
            "photo_file_id": 31,
            "is_transfer_voicemail": 11,
            "personal_ringtone": "test1",
            "is_deleted": 0,
            "personal_notification_ringtone": "6661",
            "photo_first_name": "77771",
            "account_id": 1,
            "sort": "66661",
            "contacted_count": 21,
            "lastest_contacted_time": 21,
            "favorite": 11,
            "favorite_order": "test1",
            "phonetic_name": "testname1",
            "phonetic_name_type": 11,
            "display_name": "licheng1",
            "company": "TTTTT1",
            "position": "Developer1",
            "read_only": 21,
            "sort_first_letter": "testsort1",
            "sync_id": 81,
            "syn_1": "syn_11",
            "syn_2": "syn_21",
            "syn_3": "syn_31",
        };
        return rawContact;
    },
    getProfileRawContactUpdateMap:
    function () {
        var rawContactMap = new Map([
            ["photo_id", "21"], ["photo_file_id", "31"], ["is_transfer_voicemail", "11"],
            ["personal_ringtone", "test1"], ["is_deleted", "0"],
            [
                "personal_notification_ringtone",
                "6661",
            ],
            [
                "photo_first_name",
                "77771",
            ],
            ["account_id", "1"], ["version", "1"], ["sort", "66661"], ["contacted_count", "21"],
            ["lastest_contacted_time", "21"], ["favorite", "11"], ["favorite_order", "test1"],
            ["phonetic_name", "testname1"], ["phonetic_name_type", "11"], ["display_name", "licheng1"],
            ["company", "TTTTT1"], ["position", "Developer1"], ["read_only", "21"],
            ["sort_first_letter", "testsort1"], ["merge_mode", "0"], ["is_need_merge", "1"],
            ["merge_status", "1"], ["is_merge_target", "0"], ["vibration_setting", "0"], ["sync_id", "81"],
            ["syn_1", "syn_11"], ["syn_2", "syn_21"], ["syn_3", "syn_31"]
        ]);
        return rawContactMap;
    },
    getProfileContactData:
    function () {
        var contactData = {
            "content_type": "name",
            "detail_info": "xiaocai",
            "family_name": "family_name",
            "middle_name_phonetic": "middle_name_phonetic",
            "given_name": "given_name",
            "given_name_phonetic": "given_name_phonetic",
            "alias_detail_info": "alias_detail_info",
            "phonetic_name": "phonetic_name",
            "position": "Developer",
            "extend1": "extend1",
            "extend2": "extend2",
            "extend3": "extend3",
            "extend4": "extend4",
            "city": "city",
            "country": "country",
            "neighborhood": "neighborhood",
            "pobox": "pobox",
            "postcode": "postcode",
            "region": "region",
            "street": "street",
            "alpha_name": "alpha_name",
            "other_lan_last_name": "other_lan_last_name",
            "other_lan_first_name": "other_lan_first_name",
            "extend5": "extend5",
            "lan_style": "lan_style",
            "custom_data": "custom_data",
            "extend6": "extend6",
            "extend7": "extend7",
            "blob_data": "blob_data",
            "syn_1": "syn_1",
            "syn_2": "syn_2",
            "syn_3": "syn_3",
        };
        return contactData;
    },
    getProfileContactDataMap:
    function () {
        var ContactDataMap = new Map([
            ["detail_info", "xiaocai"], ["family_name", "family_name"],
            ["middle_name_phonetic", "middle_name_phonetic"], ["given_name", "given_name"],
            ["given_name_phonetic", "given_name_phonetic"], ["alias_detail_info", "alias_detail_info"],
            ["phonetic_name", "phonetic_name"], ["position", "position"], ["extend1", "extend1"],
            ["extend2", "extend2"], ["pobox", "pobox"], ["postcode", "postcode"], ["city", "city"],
            ["country", "country"], ["neighborhood", "neighborhood"],
            [
                "position",
                "Developer",
            ],
            ["region", "region"], ["street", "street"], ["alpha_name", "alpha_name"],
            ["other_lan_last_name", "other_lan_last_name"], ["other_lan_first_name", "other_lan_first_name"],
            ["extend5", "extend5"], ["lan_style", "lan_style"], ["custom_data", "custom_data"],
            ["extend6", "extend6"], ["extend7", "extend7"], ["blob_data", "blob_data"], ["syn_1", "syn_1"],
            [
                "syn_2",
                "syn_2",
            ],
            [
                "syn_3",
                "syn_3",
            ],
            ["read_only", "0"], ["version", "0"], ["is_preferred_number", "0"]
        ]);
        return ContactDataMap;
    },
    getProfileContactDataUpdate:
    function () {
        var contactData = {
            "detail_info": "xiaocai1",
            "family_name": "family_name1",
            "middle_name_phonetic": "middle_name_phonetic1",
            "given_name": "given_name1",
            "given_name_phonetic": "given_name_phonetic1",
            "alias_detail_info": "alias_detail_info1",
            "phonetic_name": "phonetic_name1",
            "position": "Developer1",
            "extend1": "extend11",
            "extend2": "extend21",
            "extend3": "extend31",
            "extend4": "extend41",
            "city": "city1",
            "country": "country1",
            "neighborhood": "neighborhood1",
            "pobox": "pobox1",
            "postcode": "postcode1",
            "region": "region1",
            "street": "street1",
            "alpha_name": "alpha_name1",
            "other_lan_last_name": "other_lan_last_name1",
            "other_lan_first_name": "other_lan_first_name1",
            "extend5": "extend51",
            "lan_style": "lan_style1",
            "custom_data": "custom_data1",
            "extend6": "extend61",
            "extend7": "extend71",
            "blob_data": "blob_data1",
            "syn_1": "syn_11",
            "syn_2": "syn_21",
            "syn_3": "syn_31",
        };
        return contactData;
    },
    getProfileContactDataUpdateMap:
    function () {
        var ContactDataMap = new Map([
            ["detail_info", "xiaocai1"], ["family_name", "family_name1"],
            ["middle_name_phonetic", "middle_name_phonetic1"], ["given_name", "given_name1"],
            ["given_name_phonetic", "given_name_phonetic1"], ["alias_detail_info", "alias_detail_info1"],
            ["phonetic_name", "phonetic_name1"], ["position", "position1"], ["extend1", "extend11"],
            ["extend2", "extend21"], ["pobox", "pobox1"], ["postcode", "postcode1"], ["city", "city1"],
            ["country", "country1"], ["neighborhood", "neighborhood1"],
            [
                "position",
                "Developer1",
            ],
            ["region", "region1"], ["street", "street1"], ["alpha_name", "alpha_name1"],
            ["other_lan_last_name", "other_lan_last_name1"], ["other_lan_first_name", "other_lan_first_name1"],
            ["extend5", "extend51"], ["lan_style", "lan_style1"], ["custom_data", "custom_data1"],
            ["extend6", "extend61"], ["extend7", "extend71"], ["blob_data", "blob_data1"],
            ["syn_1", "syn_11"],
            [
                "syn_2",
                "syn_21",
            ],
            [
                "syn_3",
                "syn_31",
            ],
            ["read_only", "0"], ["version", "1"], ["is_preferred_number", "0"]
        ]);
        return ContactDataMap;
    },
    getProfileGroup:
    function () {
        var group = {
            "account_id": 1,
            "group_name": "DeveloperFirsthGroup",
            "group_notes": "note",
            "group_ringtone": "test"
        };
        return group;
    },
    getProfileGroupMap:
    function () {
        var groupMap = new Map([
            ["account_id", "1"], ["group_name", "DeveloperFirsthGroup"], ["group_notes", "note"],
            ["is_deleted", "0"], ["group_ringtone", "test"], ["ringtone_modify_time", "0"],
            ["lastest_modify_time", "0"]
        ]);
        return groupMap;
    },
    getProfileBlockList:
    function () {
        var blockList = {
            "phone_number": "123456789", "content": "heimingdan"
        };
        return blockList;
    },
    getProfileBlockListMap:
    function () {
        var blockListMap = new Map([
            ["types", "0"], ["phone_number", "123456789"], ["content", "heimingdan"], ["time_stamp", "0"]
        ]);
        return blockListMap;
    },
    getProfileUpdateRawContact:
    function () {
        var rawContact = {
            "photo_id": 8,
            "photo_file_id": 8,
            "is_transfer_voicemail": 8,
            "personal_ringtone": "test8",
            "is_deleted": 1,
            "personal_notification_ringtone": "888",
            "photo_first_name": "9999",
            "account_id": 8,
            "version": 2,
            "sort": "8888",
            "contacted_count": 8,
            "lastest_contacted_time": 8,
            "favorite": 0,
            "favorite_order": "test8",
            "phonetic_name": "testname8",
            "phonetic_name_type": 8,
            "display_name": "licheng8",
            "company": "TTTTT8",
            "position": "Developer8",
            "read_only": 8,
            "sort_first_letter": "testsort8",
            "sync_id": 888,
            "syn_1": "syn_8",
            "syn_2": "syn_8",
            "syn_3": "syn_8",
        };
        return rawContact;
    },
    getProfileUpdateRawContactMap:
    function () {
        var rawContactMap = new Map([
            ["photo_id", "8"], ["photo_file_id", "8"], ["is_transfer_voicemail", "8"],
            ["personal_ringtone", "test8"], ["is_deleted", "1"],
            [
                "personal_notification_ringtone",
                "888",
            ],
            [
                "photo_first_name",
                "9999",
            ],
            ["account_id", "8"], ["version", "2"], ["sort", "8888"], ["contacted_count", "8"],
            ["lastest_contacted_time", "8"], ["favorite", "0"], ["favorite_order", "test8"],
            ["phonetic_name", "testname8"], ["phonetic_name_type", "8"], ["display_name", "licheng8"],
            ["company", "TTTTT8"], ["position", "Developer8"], ["read_only", "8"],
            ["sort_first_letter", "testsort8"], ["merge_mode", "0"], ["is_need_merge", "1"],
            ["merge_status", "1"], ["is_merge_target", "0"], ["vibration_setting", "0"], ["sync_id", "888"],
            ["syn_1", "syn_8"], ["syn_2", "syn_8"], ["syn_3", "syn_8"]
        ]);
        return rawContactMap;
    },
    getProfileUpdateContactData:
    function () {
        var contactData = {
            "detail_info": "BossCai",
            "family_name": "family_name8",
            "middle_name_phonetic": "middle_name_phonetic8",
            "given_name": "given_name8",
            "given_name_phonetic": "given_name_phonetic8",
            "alias_detail_info": "alias_detail_info8",
            "phonetic_name": "phonetic_name8",
            "position": "position8",
            "extend1": "extend8",
            "extend2": "extend8",
            "extend3": "extend8",
            "extend4": "extend8",
            "country": "country8",
            "neighborhood": "neighborhood8",
            "pobox": "pobox8",
            "city": "city8",
            "postcode": "postcode8",
            "region": "region8",
            "street": "street8",
            "alpha_name": "alpha_name8",
            "other_lan_last_name": "other_lan_last_name8",
            "other_lan_first_name": "other_lan_first_name8",
            "extend5": "extend8",
            "lan_style": "lan_style8",
            "custom_data": "custom_data8",
            "extend6": "extend8",
            "extend7": "extend8",
            "blob_data": "blob_data8",
            "syn_1": "syn_8",
            "syn_2": "syn_8",
            "syn_3": "syn_8"
        };
        return contactData;
    },
    getProfileUpdateContactDataMap:
    function () {
        var ContactDataMap = new Map([
            ["detail_info", "BossCai"], ["family_name", "family_name8"],
            ["middle_name_phonetic", "middle_name_phonetic8"], ["given_name", "given_name8"],
            ["given_name_phonetic", "given_name_phonetic8"], ["alias_detail_info", "alias_detail_info8"],
            ["phonetic_name", "phonetic_name8"], ["position", "position8"], ["extend1", "extend8"],
            ["extend2", "extend8"], ["pobox", "pobox8"], ["postcode", "postcode8"], ["city", "city8"],
            ["country", "country8"], ["neighborhood", "neighborhood8"],
            [
                "position",
                "position8",
            ],
            ["region", "region8"], ["street", "street8"], ["alpha_name", "alpha_name8"],
            ["other_lan_last_name", "other_lan_last_name8"], ["other_lan_first_name", "other_lan_first_name8"],
            ["extend5", "extend8"], ["lan_style", "lan_style8"], ["custom_data", "custom_data8"],
            ["extend6", "extend8"], ["extend7", "extend8"], ["blob_data", "blob_data8"], ["syn_1", "syn_8"],
            [
                "syn_2",
                "syn_8",
            ],
            [
                "syn_3",
                "syn_8",
            ],
            ["read_only", "0"], ["version", "1"], ["is_preferred_number", "0"]
        ]);
        return ContactDataMap;
    },
    getProfileUpdateGroup:
    function () {
        var group = {
            "account_id": 8,
            "group_name": "TestFifthGroup",
            "group_notes": "note8",
            "group_ringtone": "test8"
        };
        return group;
    },
    getProfileUpdateGroupMap:
    function () {
        var groupMap = new Map([
            ["account_id", "8"], ["group_name", "TestFifthGroup"], ["group_notes", "note8"],
            ["is_deleted", "0"], ["group_ringtone", "test8"], ["ringtone_modify_time", "0"],
            ["lastest_modify_time", "0"]
        ]);
        return groupMap;
    },
    getProfileUpdateBlockList:
    function () {
        var blockList = {
            "phone_number": "987654321",
            "content": "danminghei",
        };
        return blockList;
    },
    getProfileUpdateBlockListMap:
    function () {
        var blockListMap = new Map([
            ["types", "0"], ["phone_number", "987654321"], ["content", "danminghei"], ["time_stamp", "0"]
        ]);
        return blockListMap;
    },
    getVoiceMail:
    function () {
        var voicemail = {
            "phone_number": "66666",
            "quicksearch_key": "777",
            "display_name": "xiaoli",
            "voicemail_uri": "XXXX",
            "origin_type": "xxxxx",
            "create_time": 0
        };
        return voicemail;
    },
    getVoiceMailMap:
    function () {
        var map = new Map([
            ["phone_number", "66666"], ["quicksearch_key", "777"], ["display_name", "xiaoli"],
            ["voicemail_uri", "XXXX"], ["origin_type", "xxxxx"], ["create_time", "0"],
            ["voicemail_type", "0"], ["voice_file_size", "0"], ["voice_duration", "0"],
            ["voice_status", "0"]
        ]);
        return map;
    },
    getVoiceMailUpdate:
    function () {
        var voicemail = {
            "phone_number": "9999999",
            "quicksearch_key": "999",
            "display_name": "xiaocai",
            "voicemail_uri": "uuuu",
            "origin_type": "iiiii",
            "create_time": "999"
        };
        return voicemail;
    },
    getVoiceMailUpdateMap:
    function () {
        var map = new Map([
            ["phone_number", "9999999"], ["quicksearch_key", "999"], ["display_name", "xiaocai"],
            ["voicemail_uri", "uuuu"], ["origin_type", "iiiii"], ["create_time", "999"],
            ["voicemail_type", "0"], ["voice_file_size", "0"], ["voice_duration", "0"],
            ["voice_status", "0"]
        ]);
        return map;
    },
    getvoiceMailBatchInsert:
    function () {
        var map = new Map([["phone_number", "1600"], ["display_name", "xiaoming"], ["voice_status", "0"]]);
        var map2 = new Map([["phone_number", "1600"], ["display_name", "xiaohei"], ["voice_status", "1"]]);
        var map3 = new Map([["phone_number", "1600"], ["display_name", "xiaohong"], ["voice_status", "0"]]);
        var map4 = new Map([["phone_number", "1600"], ["display_name", "xiaohei"], ["voice_status", "0"]]);
        var map5 = new Map([["phone_number", "1600"], ["display_name", "xiaofen"], ["voice_status", "0"]]);
        var array = [map, map2, map3, map4, map5];
        return array;
    },
    getContactBatchCompany:
    function () {
        var rawContactValuesOne = {
            "display_name": "fengyuanyuan", "company": "TT4900", "position": "Testers"
        };
        var rawContactValuesTwo = {
            "display_name": "caiboss", "company": "TT4900", "position": "developer"
        };
        var rawContactValuesThree = {
            "display_name": "zhangsan", "company": "TT4900", "position": "developer"
        };
        var rawContactValuesFour = {
            "display_name": "lisi", "company": "TT4900", "position": "developer"
        };
        var rawContactValuesFive = {
            "display_name": "wangwu", "company": "TT4900", "position": "developer"
        };
        var listAddBluk = [];
        listAddBluk[0] = rawContactValuesOne;
        listAddBluk[1] = rawContactValuesTwo;
        listAddBluk[2] = rawContactValuesThree;
        listAddBluk[3] = rawContactValuesFour;
        listAddBluk[4] = rawContactValuesFive;
        return listAddBluk;
    },
    getContactBatchCompanyTwo:
    function () {
        var contactDataValuesOne = {
            "raw_contact_id": rawContactId,
            "content_type": "name",
            "detail_info": "xiaozhi"
        };
        var contactDataValuesTwo = {
            "raw_contact_id": rawContactId,
            "content_type": "organization",
            "detail_info": "TT",
            "position": "Testers"
        };
        var contactDataValuesThree = {
            "raw_contact_id": rawContactId,
            "content_type": "email",
            "detail_info": "15844@163.com"
        };
        var contactDataValuesFour = {
            "raw_contact_id": rawContactId,
            "content_type": "nickname",
            "detail_info": "xiaozhicheng"
        };
        var contactDataValuesFive = {
            "raw_contact_id": rawContactId,
            "content_type": "phone",
            "detail_info": "111885566"
        };
        var listAddBluk = [];
        listAddBluk[0] = contactDataValuesOne;
        listAddBluk[1] = contactDataValuesTwo;
        listAddBluk[2] = contactDataValuesThree;
        listAddBluk[3] = contactDataValuesFour;
        listAddBluk[4] = contactDataValuesFive;
        return listAddBluk;
    },
    getPhoneNumberBatch:
    function () {
        var contactBlacklistValuesOne = {
            "phone_number": "85525040585400"
        };
        var contactBlacklistValuesTwo = {
            "phone_number": "85525040585400"
        };
        var contactBlacklistValuesThree = {
            "phone_number": "85525040585400"
        };
        var contactBlacklistValuesFour = {
            "phone_number": "85525040585400"
        };
        var contactBlacklistValuesFive = {
            "phone_number": "85525040585400"
        };
        var listAddBluk = []
        listAddBluk[0] = contactBlacklistValuesOne;
        listAddBluk[1] = contactBlacklistValuesTwo;
        listAddBluk[2] = contactBlacklistValuesThree;
        listAddBluk[3] = contactBlacklistValuesFour;
        listAddBluk[4] = contactBlacklistValuesFive;
        return listAddBluk;
    },
    getSearchMap:
    function () {
        var map = new Map([
            ["account_id", "1"], ["search_name", "小黄||xiaohuang||xh"], ["display_name", "小黄"],
            ["phonetic_name", ""], ["photo_id", ""], ["photo_file_id", ""]
        ]);
        return map;
    }
}