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

let HksKeySize = {
	HKS_RSA_KEY_SIZE_512: 512,
}

let HksKeyDigest = {
	HKS_DIGEST_SHA1: 10,
	HKS_DIGEST_SHA224: 11,
	HKS_DIGEST_SHA256: 12,
	HKS_DIGEST_SHA384: 13,
	HKS_DIGEST_SHA512: 14,
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

let srcData63Kb = Data.Date63KB
let srcData65Kb = Data.Date65KB

function stringToArray(str) {
	var arr = []
	for (var i = 0, j = str.length; i < j; ++i) {
		arr.push(str.charCodeAt(i))
	}
	return arr
}

function uint8ArrayToString(fileData) {
	var dataString = ''
	for (var i = 0; i < fileData.length; i++) {
		dataString += String.fromCharCode(fileData[i])
	}
	return dataString
}

var handle = {}
var handleStart
var handleEnd

function generateKey(srcKeyAlies, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.generateKey(srcKeyAlies, HuksOptions, function (err, data) {
			try {
				if (err.code !== 0) {
					console.log(
						'test generateKey err information: ' +
							JSON.stringify(err)
					)
					reject(err)
				} else {
					resolve(data)
				}
			} catch (e) {
				console.log(
					'test generateKey err information: ' + JSON.stringify(e)
				)
				reject(e)
			}
		})
	})
}

function init(srcKeyAlies, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.init(srcKeyAlies, HuksOptions, function (err, data) {
			try {
				if (err.code !== 0) {
					console.log(
						'test init err information: ' + JSON.stringify(err)
					)
					reject(err)
				} else {
					handleStart = data.handle1
					handleEnd = data.handle2
					handle = {
						handle1: handleStart,
						handle2: handleEnd,
					}
					resolve(data)
				}
			} catch (e) {
				console.log('test init err information: ' + JSON.stringify(e))
				reject(e)
			}
		})
	})
}

function update(handle, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.update(handle, HuksOptions, function (err, data) {
			try {
				if (err.code !== 0) {
					console.log(
						'test update err information: ' + JSON.stringify(err)
					)
					reject(err)
				} else {
					resolve(data)
				}
			} catch (e) {
				console.log('test update err information: ' + JSON.stringify(e))
				reject(e)
			}
		})
	})
}

function finish(handle, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.finish(handle, HuksOptions, function (err, data) {
			try {
				if (err.code !== 0) {
					console.log(
						'test finish err information: ' + JSON.stringify(err)
					)
					reject(err)
				} else {
					resolve(data)
				}
			} catch (e) {
				console.log('test finish err information: ' + JSON.stringify(e))
				reject(e)
			}
		})
	})
}

function abort(handle, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.abort(handle, HuksOptions, function (err, data) {
			try {
				if (err.code !== 0) {
					console.log(
						'test abort err information: ' + JSON.stringify(err)
					)
					reject(err)
				} else {
					resolve(data)
				}
			} catch (e) {
				console.log('test abort err information: ' + JSON.stringify(e))
				reject(e)
			}
		})
	})
}

function deleteKey(srcKeyAlies, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.deleteKey(srcKeyAlies, HuksOptions, function (err, data) {
			try {
				if (err.code !== 0) {
					console.log(
						'test deleteKey err information: ' + JSON.stringify(err)
					)
					reject(err)
				} else {
					resolve(data)
				}
			} catch (e) {
				console.log(
					'test deleteKey err information: ' + JSON.stringify(e)
				)
				reject(e)
			}
		})
	})
}

async function publicHmacUpdate(HuksOptions) {
	let dateSize = 64 * 1024
	let huksOptionsInData = HuksOptions.inData
	let inDataArray = stringToArray(HuksOptions.inData)

	if (HuksOptions.inData.length <= dateSize) {
		HuksOptions.inData = new Uint8Array(inDataArray)
		await update(handle, HuksOptions)
			.then((data) => {
				console.log(`test update data: ${data}`)
			})
			.catch((err) => {
				console.log(
					'test update err information: ' + JSON.stringify(err)
				)
				expect(null).assertFail()
			})
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
				.then((data) => {
					console.log(`test update data: ${data}`)
				})
				.catch((err) => {
					console.log(
						'test update err information: ' + JSON.stringify(err)
					)
					expect(null).assertFail()
				})
		}
		if (remainder !== 0) {
			HuksOptions.inData = new Uint8Array(
				stringToArray(huksOptionsInData).slice(
					dateSize * count,
					inDataArray.length
				)
			)
			console.log(`test update HuksOptions.inData ${HuksOptions.inData}`)
			await update(handle, HuksOptions)
				.then((data) => {
					console.log(`test update data: ${data}`)
				})
				.catch((err) => {
					console.log(
						'test update err information: ' + JSON.stringify(err)
					)
					expect(null).assertFail()
				})
		}
	}
}

async function publicHmacGenFunc(srcKeyAlies, HuksOptions, thirdInderfaceName) {
	HuksOptions.properties.splice(1, 0, HuksHmac.HuksKeySIZE)
	console.log(
		'test before generateKey HuksOptions = ' + JSON.stringify(HuksOptions)
	)
	await generateKey(srcKeyAlies, HuksOptions)
		.then((data) => {
			console.log('test generateKey data = ' + JSON.stringify(data))
		})
		.catch((err) => {
			console.log(`test init err: " + ${JSON.stringify(err)}`)
			expect(null).assertFail()
		})

	HuksOptions.properties.splice(1, 1)
	console.log('test before init HuksOptions = ' + JSON.stringify(HuksOptions))
	await init(srcKeyAlies, HuksOptions)
		.then((data) => {
			console.log(`test init data: ${JSON.stringify(data)}`)
		})
		.catch((err) => {
			console.log(`test init err: " + ${JSON.stringify(err)}`)
			expect(null).assertFail()
		})
	console.log(
		'test before Update HuksOptions = ' + JSON.stringify(HuksOptions)
	)
	await publicHmacUpdate(HuksOptions)

	if (thirdInderfaceName == 'finish') {
		HuksOptions.inData = new Uint8Array(stringToArray('0'))
		console.log(
			'test before finish HuksOptions = ' + JSON.stringify(HuksOptions)
		)
		await finish(handle, HuksOptions)
			.then((data) => {
				console.log(`test update data: ${data}`)
			})
			.catch((err) => {
				console.log(
					'test update err information: ' + JSON.stringify(err)
				)
				expect(null).assertFail()
			})
	} else {
		console.log(
			'test before abort HuksOptions = ' + JSON.stringify(HuksOptions)
		)
		await abort(handle, HuksOptions)
			.then((data) => {
				console.log(`test abort data: ${data}`)
			})
			.catch((err) => {
				console.log(
					'test abort err information: ' + JSON.stringify(err)
				)
				expect(null).assertFail()
			})
	}
	HuksOptions.properties.splice(1, 0, HuksHmac.HuksKeySIZE)
	console.log(
		'test before deleteKey HuksOptions = ' + JSON.stringify(HuksOptions)
	)
	await deleteKey(srcKeyAlies, HuksOptions)
		.then((data) => {
			console.log(`test deleteKey data: ${data}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log(
				'test deleteKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
}

describe('SecurityHuksHmacCallbackJsunit', function () {
	it('testHmacSHA1001', 0, async function (done) {
		let srcKeyAlies = 'testHmacDigestSHA1KeyAlias001'
		let HuksOptions = {
			properties: new Array(
				HuksHmac.HuksKeyAlg,
				HuksHmac.HuksKeyPurpose,
				HuksHmac.HuksTagDigestSHA1
			),
			inData: srcData63Kb,
		}

		await publicHmacGenFunc(srcKeyAlies, HuksOptions, 'finish')
		done()
	})

	it('testHmacSHA1002', 0, async function (done) {
		let srcKeyAlies = 'testHmacDigestSHA1KeyAlias002'
		let HuksOptions = {
			properties: new Array(
				HuksHmac.HuksKeyAlg,
				HuksHmac.HuksKeyPurpose,
				HuksHmac.HuksTagDigestSHA1
			),
			inData: srcData63Kb,
		}
		await publicHmacGenFunc(srcKeyAlies, HuksOptions, 'abort')
		done()
	})
	it('testHmacSHA1003', 0, async function (done) {
		let srcKeyAlies = 'testHmacDigestSHA1KeyAlias003'
		let HuksOptions = {
			properties: new Array(
				HuksHmac.HuksKeyAlg,
				HuksHmac.HuksKeyPurpose,
				HuksHmac.HuksTagDigestSHA1
			),
			inData: srcData65Kb,
		}
		await publicHmacGenFunc(srcKeyAlies, HuksOptions, 'finish')
		done()
	})
	it('testHmacSHA1004', 0, async function (done) {
		let srcKeyAlies = 'testHmacDigestSHA1KeyAlias004'
		let HuksOptions = {
			properties: new Array(
				HuksHmac.HuksKeyAlg,
				HuksHmac.HuksKeyPurpose,
				HuksHmac.HuksTagDigestSHA1
			),
			inData: srcData65Kb,
		}
		await publicHmacGenFunc(srcKeyAlies, HuksOptions, 'abort')
		done()
	})
})
