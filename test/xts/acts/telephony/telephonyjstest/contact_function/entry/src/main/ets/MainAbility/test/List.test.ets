/**
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

//contact import
import CalllogTest from './contact/Calllog_test.js'
import ContactCardTest from './contact/ContactCard_test.js'
import ContactsTest from './contact/Contacts_test.js'
import GroupsTest from './contact/Groups_test.js'
import ObjectInterfaceTest from './contact/ObjectInterface_test.js'
import RecoveryTest from './contact/recovery_test.js'
import VoicemailTest from './contact/Voicemail_test.js'
import ContactClassTest from './contact/ContactClass_test.js'
import ContacterTest from './contact/ContacterJsunit.ets'

export default function testsuite() {
  //contact
  ContactClassTest();
  ContacterTest();
  CalllogTest();
  ContactCardTest();
  ContactsTest();
  GroupsTest();
  ObjectInterfaceTest();
  RecoveryTest();
  VoicemailTest();
}