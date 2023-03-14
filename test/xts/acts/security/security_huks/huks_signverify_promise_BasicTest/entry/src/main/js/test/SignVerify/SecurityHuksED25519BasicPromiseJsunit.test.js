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

let finishOutData
let handle = {}
let exportKey
let srcData65 = Data.Data65b
let srcData65Kb = stringToUint8Array(srcData65)
let srcData63 = Data.Data63b
let srcData63Kb = stringToUint8Array(srcData63)

let HksKeyAlg = {
	HKS_ALG_ED25519: 102,
}

let HksKeyPurpose = {
	HKS_KEY_PURPOSE_SIGN: 4,
	HKS_KEY_PURPOSE_VERIFY: 8,
}

let HksKeySize = {
	HKS_CURVE25519_KEY_SIZE_256: 256,
	HKS_ED25519_KEY_SIZE_512: 512,
}

let HksTagType = {
	HKS_TAG_TYPE_UINT: 2 << 28,
}

let HksTag = {
	HKS_TAG_ALGORITHM: HksTagType.HKS_TAG_TYPE_UINT | 1,
	HKS_TAG_PURPOSE: HksTagType.HKS_TAG_TYPE_UINT | 2,
	HKS_TAG_KEY_SIZE: HksTagType.HKS_TAG_TYPE_UINT | 3,
	HKS_TAG_DIGEST: HksTagType.HKS_TAG_TYPE_UINT | 4,
	HKS_TAG_AGREE_PUBLIC_KEY: HksTagType.HKS_TAG_TYPE_BYTES | 22,
}

let HksKeyDigest = {
	HKS_DIGEST_NONE: 0,
	HKS_DIGEST_MD5: 1,
	HKS_DIGEST_SHA1: 10,
	HKS_DIGEST_SHA224: 11,
	HKS_DIGEST_SHA256: 12,
	HKS_DIGEST_SHA384: 13,
	HKS_DIGEST_SHA512: 14,
}

let HuksSignVerify004 = {
	HuksKeySIZE: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_ED25519_KEY_SIZE_512,
	},
	HuksKeyAlgED25519: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_ED25519,
	},
	HuksKeyED25519Size256: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_CURVE25519_KEY_SIZE_256,
	},
	HuksKeyED25519PurposeSIGN: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_SIGN,
	},
	HuksKeyED25519PurposeVERIFY: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_VERIFY,
	},
	HuksKeyRSAPurposeSINGVERIFY: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value:
			HksKeyPurpose.HKS_KEY_PURPOSE_SIGN |
			HksKeyPurpose.HKS_KEY_PURPOSE_VERIFY,
	},
	HuksTagDigestSHA1: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA1,
	},
}

function stringToUint8Array(str) {
	var arr = []
	for (var i = 0, j = str.length; i < j; ++i) {
		arr.push(str.charCodeAt(i))
	}
	var tmpUint8Array = new Uint8Array(arr)
	return tmpUint8Array
}

function uint8ArrayToString(fileData) {
	var dataString = ''
	for (var i = 0; i < fileData.length; i++) {
		dataString += String.fromCharCode(fileData[i])
	}
	return dataString
}

async function publicGenerateKeyFunc(keyAlias, HuksOptions) {
	await huks
		.generateKey(keyAlias, HuksOptions)
		.then((data) => {
			console.log(`test generateKey data: ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log(
				'test generateKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
}

async function publicImportKey(keyAlias, HuksOptions) {
	let _InData = HuksOptions.inData
	HuksOptions.inData = finishOutData
	console.log(`test ImportKey keyAlias: ${keyAlias}`)
	console.log(`test ImportKey HuksOptions: ${JSON.stringify(HuksOptions)}`)
	await huks
		.importKey(keyAlias, HuksOptions)
		.then((data) => {
			console.log(`test ImportKey data: ${JSON.stringify(data)}`)
		})
		.catch((err) => {
			console.log(
				'test exportKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
	HuksOptions.inData = _InData
}

async function publicExportKey(keyAlias, HuksOptions) {
	await huks
		.exportKey(keyAlias, HuksOptions)
		.then((data) => {
			console.log(`test exportKey data: ${JSON.stringify(data)}`)
			finishOutData = data.outData
		})
		.catch((err) => {
			console.log(
				'test exportKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
}

async function publicInitFunc(keyAlias, HuksOptions) {
	await huks
		.init(keyAlias, HuksOptions)
		.then((data) => {
			console.log(`test init data: ${JSON.stringify(data)}`)
			handle = {
				handle1: data.handle1,
				handle2: data.handle2,
			}
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log('test init err information: ' + JSON.stringify(err))
			expect(null).assertFail()
		})
}

async function publicUpdateFunc(HuksOptions) {
	let dateSize = 64
	let huksOptionsInData = HuksOptions.inData
	let inDataArray = HuksOptions.inData
	if (uint8ArrayToString(inDataArray).length < dateSize) {
		await update(handle, HuksOptions)
		HuksOptions.inData = huksOptionsInData
	} else {
		let count = Math.floor(
			uint8ArrayToString(inDataArray).length / dateSize
		)
		let remainder = uint8ArrayToString(inDataArray).length % dateSize
		console.log(
			`test before update length: ${
				uint8ArrayToString(inDataArray).length
			}`
		)
		console.log(`test before update count: ${count}`)
		console.log(`test before update remainder: ${remainder}`)
		for (let i = 0; i < count; i++) {
			HuksOptions.inData = stringToUint8Array(
				uint8ArrayToString(huksOptionsInData).slice(
					dateSize * i,
					dateSize * (i + 1)
				)
			)
			await update(handle, HuksOptions)
			HuksOptions.inData = huksOptionsInData
		}
		if (remainder !== 0) {
			HuksOptions.inData = stringToUint8Array(
				uint8ArrayToString(huksOptionsInData).slice(
					dateSize * count,
					uint8ArrayToString(inDataArray).length
				)
			)
			await update(handle, HuksOptions)
			HuksOptions.inData = huksOptionsInData
		}
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

async function publicFinishFunc(HuksOptions) {
	await huks
		.finish(handle, HuksOptions)
		.then((data) => {
			console.log(`test finish data: ${JSON.stringify(data)}`)
			exportKey = data.outData
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log('test finish err information: ' + JSON.stringify(err))
			expect(null).assertFail()
		})
}

async function publicAbortFucn(HuksOptions) {
	await huks
		.abort(handle, HuksOptions)
		.then((data) => {
			console.log(`test abort data: ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log('test abort err information: ' + JSON.stringify(err))
			expect(null).assertFail()
		})
}

async function publicDeleteKeyFunc(KeyAlias, HuksOptions) {
	await huks
		.deleteKey(KeyAlias, HuksOptions)
		.then((data) => {
			console.log(`test deleteKey data: ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log(
				'test deleteKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
}

async function publicSignVerifyFunc(
	srcKeyAlies,
	newSrcKeyAlies,
	HuksOptions,
	thirdInderfaceName,
	isSING
) {
	try {
		let keyAlias = srcKeyAlies
		if (isSING) {
			HuksOptions.properties.splice(
				1,
				1,
				HuksSignVerify004.HuksKeyRSAPurposeSINGVERIFY
			)
			console.log(
				`test publicSignVerifyFunc GenerateHuksOptions: ${JSON.stringify(
					HuksOptions
				)}`
			)
			await publicGenerateKeyFunc(keyAlias, HuksOptions)
			HuksOptions.properties.splice(
				1,
				1,
				HuksSignVerify004.HuksKeyED25519PurposeSIGN
			)
		} else {
			keyAlias = newSrcKeyAlies
			await publicImportKey(keyAlias, HuksOptions)
		}
		console.log(`test init HuksOptions: ${JSON.stringify(HuksOptions)}`)
		await publicInitFunc(keyAlias, HuksOptions)
		await publicUpdateFunc(HuksOptions)
		if (thirdInderfaceName == 'finish') {
			if (isSING) {
				HuksOptions.outData = new Uint8Array(new Array(1024).fill(''))
				console.log(
					`test before finish HuksOptions: ${HuksOptions.inData}`
				)
				console.log(
					`test before finish HuksOptions: ${HuksOptions.outData}`
				)
				await publicFinishFunc(HuksOptions)
				HuksOptions.properties.splice(
					1,
					1,
					HuksSignVerify004.HuksKeyRSAPurposeSINGVERIFY
				)
				console.log(
					`test before exportKey Gen_HuksOptions: ${JSON.stringify(
						HuksOptions
					)}`
				)
				await publicExportKey(keyAlias, HuksOptions)
			} else {
				HuksOptions.outData = exportKey
				console.log(
					`test before finish HuksOptions: ${HuksOptions.inData}`
				)
				console.log(
					`test before finish HuksOptions: ${HuksOptions.outData}`
				)
				await publicFinishFunc(HuksOptions)
			}
		} else {
			await publicAbortFucn(HuksOptions)
		}

		if (isSING && thirdInderfaceName == 'abort') {
			HuksOptions.properties.splice(
				1,
				1,
				HuksSignVerify004.HuksKeyRSAPurposeSINGVERIFY
			)
			await publicDeleteKeyFunc(srcKeyAlies, HuksOptions)
		} else if (!isSING) {
			HuksOptions.properties.splice(
				1,
				1,
				HuksSignVerify004.HuksKeyED25519PurposeVERIFY
			)
			await publicDeleteKeyFunc(newSrcKeyAlies, HuksOptions)
		}
	} catch (e) {
		expect(null).assertFail()
	}
}

describe('SecurityHuksSignVerifyED25519PromiseJsunit', function () {
	it('testSignVerifyED25519Size256SIGN101', 0, async function (done) {
		const srcKeyAlies = 'testSignVerifyED25519Size256SIGNKeyAlias101'
		let HuksOptions = {
			properties: new Array(
				HuksSignVerify004.HuksKeyAlgED25519,
				HuksSignVerify004.HuksKeyED25519PurposeSIGN,
				HuksSignVerify004.HuksKeyED25519Size256,
				HuksSignVerify004.HuksTagDigestSHA1
			),
			inData: srcData63Kb,
		}
		await publicSignVerifyFunc(
			srcKeyAlies,
			srcKeyAlies + 'New',
			HuksOptions,
			'finish',
			true
		)

		HuksOptions = {
			properties: new Array(
				HuksSignVerify004.HuksKeyAlgED25519,
				HuksSignVerify004.HuksKeyED25519PurposeVERIFY,
				HuksSignVerify004.HuksKeyED25519Size256,
				HuksSignVerify004.HuksTagDigestSHA1
			),
			inData: srcData63Kb,
		}
		await publicSignVerifyFunc(
			srcKeyAlies,
			srcKeyAlies + 'New',
			HuksOptions,
			'finish',
			false
		)
		finishOutData = 0
		exportKey = 0
		done()
	})

	it('testSignVerifyED25519Size256SIGN102', 0, async function (done) {
		const srcKeyAlies = 'testSignVerifyED25519Size256SIGNKeyAlias102'
		let HuksOptions = {
			properties: new Array(
				HuksSignVerify004.HuksKeyAlgED25519,
				HuksSignVerify004.HuksKeyED25519PurposeSIGN,
				HuksSignVerify004.HuksKeyED25519Size256,
				HuksSignVerify004.HuksTagDigestSHA1
			),
			inData: srcData63Kb,
		}
		await publicSignVerifyFunc(
			srcKeyAlies,
			srcKeyAlies + 'New',
			HuksOptions,
			'abort',
			true
		)

		finishOutData = 0
		exportKey = 0
		done()
	})

	it('testSignVerifyED25519Size256SIGN103', 0, async function (done) {
		const srcKeyAlies = 'testSignVerifyED25519Size256SIGNKeyAlias103'
		let HuksOptions = {
			properties: new Array(
				HuksSignVerify004.HuksKeyAlgED25519,
				HuksSignVerify004.HuksKeyED25519PurposeSIGN,
				HuksSignVerify004.HuksKeyED25519Size256,
				HuksSignVerify004.HuksTagDigestSHA1
			),
			inData: srcData65Kb,
		}
		await publicSignVerifyFunc(
			srcKeyAlies,
			srcKeyAlies + 'New',
			HuksOptions,
			'finish',
			true
		)
		HuksOptions = {
			properties: new Array(
				HuksSignVerify004.HuksKeyAlgED25519,
				HuksSignVerify004.HuksKeyED25519PurposeVERIFY,
				HuksSignVerify004.HuksKeyED25519Size256,
				HuksSignVerify004.HuksTagDigestSHA1
			),
			inData: srcData65Kb,
		}
		await publicSignVerifyFunc(
			srcKeyAlies,
			srcKeyAlies + 'New',
			HuksOptions,
			'finish',
			false
		)

		finishOutData = 0
		exportKey = 0
		done()
	})

	it('testSignVerifyED25519Size256SIGN104', 0, async function (done) {
		const srcKeyAlies = 'testSignVerifyED25519Size256SIGNKeyAlias104'
		let HuksOptions = {
			properties: new Array(
				HuksSignVerify004.HuksKeyAlgED25519,
				HuksSignVerify004.HuksKeyED25519PurposeSIGN,
				HuksSignVerify004.HuksKeyED25519Size256,
				HuksSignVerify004.HuksTagDigestSHA1
			),
			inData: srcData65Kb,
		}
		await publicSignVerifyFunc(
			srcKeyAlies,
			srcKeyAlies + 'New',
			HuksOptions,
			'abort',
			true
		)
		finishOutData = 0
		exportKey = 0
		done()
	})
})
