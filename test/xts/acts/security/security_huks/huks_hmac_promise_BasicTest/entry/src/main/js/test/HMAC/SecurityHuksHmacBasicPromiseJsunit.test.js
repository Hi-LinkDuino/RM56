/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

import { describe, it, expect } from 'deccjsunit/index'
import huks from '@ohos.security.huks'
import Data from '../../../../../../../utils/data.json'

var handle = {}
var handle1
var handle2
let srcData63Kb = Data.Date63KB
let srcData65Kb = Data.Date65KB
let HksKeyPurpose = {
	HKS_KEY_PURPOSE_MAC: 128,
}

let HksKeyAlg = {
	HKS_ALG_HMAC: 50,
}

let HksTagType = {
	HKS_TAG_TYPE_UINT: 2 << 28,
}

let HksTag = {
	HKS_TAG_ALGORITHM: HksTagType.HKS_TAG_TYPE_UINT | 1,
	HKS_TAG_PURPOSE: HksTagType.HKS_TAG_TYPE_UINT | 2,
	HKS_TAG_KEY_SIZE: HksTagType.HKS_TAG_TYPE_UINT | 3,
	HKS_TAG_DIGEST: HksTagType.HKS_TAG_TYPE_UINT | 4,
}

let HksKeyDigest = {
	HKS_DIGEST_SHA1: 10,
	HKS_DIGEST_SHA224: 11,
	HKS_DIGEST_SHA256: 12,
	HKS_DIGEST_SHA384: 13,
	HKS_DIGEST_SHA512: 14,
}

let HksKeySize = {
	HKS_RSA_KEY_SIZE_512: 512,
}

let HuksHmac = {
	HuksKeyAlg: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_HMAC,
	},
	HuksKeyPurpose: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_MAC,
	},
	HuksKeySIZE: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_RSA_KEY_SIZE_512,
	},
	HuksTagDigestSHA1: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA1,
	},
	HuksTagDigestSHA224: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA224,
	},
	HuksTagDigestSHA256: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA256,
	},
	HuksTagDigestSHA384: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA384,
	},
	HuksTagDigestSHA512: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA512,
	},
}

function stringToArray(str) {
	var arr = []
	for (var i = 0, j = str.length; i < j; ++i) {
		arr.push(str.charCodeAt(i))
	}
	return arr
}

async function publicHmacGenFunc(srcKeyAlies, HuksOptions) {
	HuksOptions.properties.splice(1, 0, HuksHmac.HuksKeySIZE)
	await huks
		.generateKey(srcKeyAlies, HuksOptions)
		.then((data) => {
			console.log(`test generateKey data ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log(
				'test generateKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
	HuksOptions.properties.splice(1, 1)
}

async function publicHmacInitFunc(srcKeyAlies, HuksOptions) {
	await huks
		.init(srcKeyAlies, HuksOptions)
		.then(async (data) => {
			handle1 = data.handle1
			handle2 = data.handle2
			handle = {
				handle1: handle1,
				handle2: handle2,
			}
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log(`test init err: " + ${JSON.stringify(err)}`)
			expect(null).assertFail()
		})
}

async function publicHmacUpdateFunc(HuksOptions) {
	let dateSize = 64 * 1024
	let huksOptionsInData = HuksOptions.inData
	let inDataArray = stringToArray(HuksOptions.inData)
	if (inDataArray.length < dateSize) {
		HuksOptions.inData = new Uint8Array(inDataArray)
		await update(handle, HuksOptions)
		HuksOptions.inData = huksOptionsInData
	} else {
		let count = Math.floor(inDataArray.length / dateSize)
		let remainder = inDataArray.length % dateSize
		for (let i = 0; i < count; i++) {
			HuksOptions.inData = new Uint8Array(
				stringToArray(huksOptionsInData).slice(
					dateSize * i,
					dateSize * (i + 1)
				)
			)
			await update(handle, HuksOptions)
		}
		if (remainder !== 0) {
			HuksOptions.inData = new Uint8Array(
				stringToArray(huksOptionsInData).slice(
					dateSize * count,
					inDataArray.length
				)
			)
			await update(handle, HuksOptions)
		}
	}
}

async function publicHmacFinish(HuksOptions, thirdInderfaceName) {
	if (thirdInderfaceName == 'finish') {
		HuksOptions.inData = new Uint8Array(stringToArray('0'))
		await huks
			.finish(handle, HuksOptions)
			.then((data) => {
				console.log(`test finish data ${JSON.stringify(data)}`)
				expect(data.errorCode == 0).assertTrue()
			})
			.catch((err) => {
				console.log('test finish err information: ' + err)
				expect(null).assertFail()
			})
	} else if (thirdInderfaceName == 'abort') {
		await huks
			.abort(handle, HuksOptions)
			.then((data) => {
				console.log(`test abort data ${JSON.stringify(data)}`)
				expect(data.errorCode == 0).assertTrue()
			})
			.catch((err) => {
				console.log('test abort err information: ' + err)
				expect(null).assertFail()
			})
	}
}

async function publicHmacDelete(srcKeyAlies, HuksOptions) {
	HuksOptions.properties.splice(1, 0, HuksHmac.HuksKeySIZE)
	await huks
		.deleteKey(srcKeyAlies, HuksOptions)
		.then((data) => {
			console.log(`test deleteKey data ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log(
				'test deleteKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
}

async function publicHmacFunc(srcKeyAlies, HuksOptions, thirdInderfaceName) {
	try {
		await publicHmacGenFunc(srcKeyAlies, HuksOptions)
		await publicHmacInitFunc(srcKeyAlies, HuksOptions)
		await publicHmacUpdateFunc(HuksOptions)
		await publicHmacFinish(HuksOptions, thirdInderfaceName)
		await publicHmacDelete(srcKeyAlies, HuksOptions)
	} catch (e) {
		expect(null).assertFail()
	}
}

async function update(handle, HuksOptions) {
	console.log(`test update data ${JSON.stringify(HuksOptions)}`)
	await huks
		.update(handle, HuksOptions)
		.then(async (data) => {
			console.log(`test update data ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log('test update err information: ' + err)
			expect(null).assertFail()
		})
}

describe('SecurityHuksHmacBasicPromiseJsunit', function () {
	it('testHmac001', 0, async function (done) {
		const srcKeyAlies = 'testHmacDigestSHA1KeyAlias101'
		let HuksOptions = {
			properties: new Array(
				HuksHmac.HuksKeyAlg,
				HuksHmac.HuksKeyPurpose,
				HuksHmac.HuksTagDigestSHA1
			),
			inData: srcData63Kb,
		}
		await publicHmacFunc(srcKeyAlies, HuksOptions, 'finish')
		done()
	})

	it('testHmac002', 0, async function (done) {
		const srcKeyAlies = 'testHmacDigestSHA1KeyAlias102'
		let HuksOptions = {
			properties: new Array(
				HuksHmac.HuksKeyAlg,
				HuksHmac.HuksKeyPurpose,
				HuksHmac.HuksTagDigestSHA1
			),
			inData: srcData63Kb,
		}
		await publicHmacFunc(srcKeyAlies, HuksOptions, 'abort')
		done()
	})

	it('testHmac003', 0, async function (done) {
		const srcKeyAlies = 'testHmacDigestSHA1KeyAlias103'
		let HuksOptions = {
			properties: new Array(
				HuksHmac.HuksKeyAlg,
				HuksHmac.HuksKeyPurpose,
				HuksHmac.HuksTagDigestSHA1
			),
			inData: srcData65Kb,
		}
		await publicHmacFunc(srcKeyAlies, HuksOptions, 'finish')
		done()
	})

	it('testHmac004', 0, async function (done) {
		const srcKeyAlies = 'testHmacDigestSHA1KeyAlias104'
		let HuksOptions = {
			properties: new Array(
				HuksHmac.HuksKeyAlg,
				HuksHmac.HuksKeyPurpose,
				HuksHmac.HuksTagDigestSHA1
			),
			inData: srcData65Kb,
		}
		await publicHmacFunc(srcKeyAlies, HuksOptions, 'abort')
		done()
	})
})
