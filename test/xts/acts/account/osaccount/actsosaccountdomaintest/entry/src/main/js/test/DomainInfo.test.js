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
import account from '@ohos.account.osAccount'

const ERR_OSACCOUNT_SERVICE_INNER_DOMAIN_ALREADY_BIND_ERROR = 4653103;
describe('ActsOsAccountSystemTest', function () {

    /**
    * @tc.number     ActsAccountDomainTest_0100
    * @tc.name       Test createOsAccountForDomain getOsAccountLocalIdFromDomain pormise
    * @tc.desc       Test createOsAccountForDomain getOsAccountLocalIdFromDomain API functionality
    */
    it('ActsOsAccountDomainTest_0100', 0, async function (done) {
        console.debug("====>ActsOsAccountDomainTest_0100 start====");
        var osAccountManager = account.getAccountManager();
        console.debug("====>createosAccountFormDomain start====");
	 var osAccountInfo = await osAccountManager.createOsAccountForDomain(0,{
	     domain: "abc",
	     accountName: "abcdef",
	 });
	 var localId = osAccountInfo.localId;
	 console.debug("====>OsAccountInfo" + JSON.stringify(osAccountInfo));
	 expect(osAccountInfo.localName).assertEqual("abc/abcdef");
	 console.debug("====>createosAccountFromDomain end====");
	 console.debug("====>getOsAccountLocalIdFromDomain start====");
	 var Number = await osAccountManager.getOsAccountLocalIdFromDomain({
	     domain: "abc",
	     accountName: "abcdef",
	 });
	 expect(Number).assertEqual(localId);
	 console.debug("====>getOsAccountLocalIdFromDomain end====");
	 osAccountManager.removeOsAccount(localId);
        console.debug("====>ActsOsAccountDomainTest_0100 end====");
        done();
    });

    /**
    * @tc.number     ActsAccountDomainTest_0200
    * @tc.name       Test createOsAccountForDomain callback
    * @tc.desc       Test repeated call createOsAccountForDomain API functionality
    */
    it('ActsOsAccountDomainTest_0200', 0, async function (done) {
        console.debug("====>ActsOsAccountDomainTest_0200 start====");
        var osAccountManager = account.getAccountManager();
        console.debug("====>createosAccountFormDomain start====");
	 osAccountManager.createOsAccountForDomain(0,{
	     domain: "def",
	     accountName: "abcdef",
	 }, (err, osAccountInfo)=>{
	     console.debug("====>err" + JSON.stringify(err));
	     console.debug("====>OsAccountInfo" + JSON.stringify(osAccountInfo));
	     expect(err.code).assertEqual(0);
	     expect(osAccountInfo.localName).assertEqual("def/abcdef");
	     var localId = osAccountInfo.localId;
	     console.debug("====>createosAccountFormDomain end====");
	     console.debug("====>createosAccountFormDomain repeat start====");
	     osAccountManager.createOsAccountForDomain(0,{
	         domain: "def",
		  accountName: "abcdef",
	     }, (err,OsAccountInfo)=>{
	         console.debug("====>createOsAccountFormDomain repeat err : " + JSON.stringify(err));
		  expect(err.code).assertEqual(ERR_OSACCOUNT_SERVICE_INNER_DOMAIN_ALREADY_BIND_ERROR);
		  console.debug("====>createosAccountFormDomain repeat end====");		
		  osAccountManager.removeOsAccount(localId, (err)=>{
		      console.debug("====>removeOsAccount err:" + JSON.stringify(err));
		      expect(err.code).assertEqual(0);
		      console.debug("====>ActsOsAccountDomainTest_0200 end====");
		      done();
	             })	
	         });
	     });
	});	
});
   