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

let HksKeyAlg = {
	HKS_ALG_RSA: 1,
}

let HksKeyPurpose = {
	HKS_KEY_PURPOSE_SIGN: 4,
	HKS_KEY_PURPOSE_VERIFY: 8,
}

let HksKeyPadding = {
	HKS_PADDING_PSS: 2,
	HKS_PADDING_PKCS1_V1_5: 3,
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

let HksKeySize = {
	HKS_RSA_KEY_SIZE_512: 512,
	HKS_RSA_KEY_SIZE_768: 768,
	HKS_RSA_KEY_SIZE_1024: 1024,
	HKS_RSA_KEY_SIZE_2048: 2048,
	HKS_RSA_KEY_SIZE_3072: 3072,
	HKS_RSA_KEY_SIZE_4096: 4096,
}

let HksTagType = {
	HKS_TAG_TYPE_UINT: 2 << 28,
	HKS_TAG_TYPE_BYTES: 5 << 28,
}

let HksTag = {
	HKS_TAG_ALGORITHM: HksTagType.HKS_TAG_TYPE_UINT | 1,
	HKS_TAG_PURPOSE: HksTagType.HKS_TAG_TYPE_UINT | 2,
	HKS_TAG_KEY_SIZE: HksTagType.HKS_TAG_TYPE_UINT | 3,
	HKS_TAG_DIGEST: HksTagType.HKS_TAG_TYPE_UINT | 4,
	HKS_TAG_PADDING: HksTagType.HKS_TAG_TYPE_UINT | 5,
	HKS_TAG_AGREE_PUBLIC_KEY: HksTagType.HKS_TAG_TYPE_BYTES | 22,
}

let HuksSignVerify001 = {
	HuksKeySIZE: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_RSA_KEY_SIZE_512,
	},
	HuksKeyAlgRSA: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_RSA,
	},
	HuksKeyRSASize512: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_RSA_KEY_SIZE_512,
	},
	HuksKeyRSASize768: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_RSA_KEY_SIZE_768,
	},
	HuksKeyRSASize1024: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_RSA_KEY_SIZE_1024,
	},
	HuksKeyRSASize2048: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_RSA_KEY_SIZE_2048,
	},
	HuksKeyRSASize3072: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_RSA_KEY_SIZE_3072,
	},
	HuksKeyRSASize4096: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_RSA_KEY_SIZE_4096,
	},
	HuksKeyRSAPurposeSIGN: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_SIGN,
	},
	HuksKeyRSAPurposeVERIFY: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_VERIFY,
	},
	HuksKeyRSAPurposeSINGVERIFY: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value:
			HksKeyPurpose.HKS_KEY_PURPOSE_SIGN |
			HksKeyPurpose.HKS_KEY_PURPOSE_VERIFY,
	},
	HuksKeyRSAPADDINGPKCS1V15: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_PKCS1_V1_5,
	},
	HuksKeyRSAPADDINGPSS: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_PSS,
	},
	HuksTagPKCS1DigestMD5: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_MD5,
	},
	HuksTagPKCS1DigestNONE: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_NONE,
	},
	HuksTagPKCS1DigestSHA1: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA1,
	},
	HuksTagPKCS1DigestSHA224: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA224,
	},
	HuksTagPKCS1DigestSHA256: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA256,
	},
	HuksTagPKCS1DigestSHA384: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA384,
	},
	HuksTagPKCS1DigestSHA512: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA512,
	},
	HuksTagPSSDigestSHA1: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA1,
	},
	HuksTagPSSDigestSHA224: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA224,
	},
	HuksTagPSSDigestSHA256: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA256,
	},
	HuksTagPSSDigestSHA384: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA384,
	},
	HuksTagPSSDigestSHA512: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA512,
	},
}

let handle = {}
let finishOutData
let exportKey
let srcData65 = Data.Data65b
let srcData65Kb = stringToUint8Array(srcData65)

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
function stringToArray(str) {
	var arr = []
	for (var i = 0, j = str.length; i < j; ++i) {
		arr.push(str.charCodeAt(i))
	}
	return arr
}

async function publicGenerateKeyFunc(keyAlias, HuksOptions) {
	await generateKey(keyAlias, HuksOptions)
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

function generateKey(srcKeyAlies, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.generateKey(srcKeyAlies, HuksOptions, function (err, data) {
			console.log(`test generateKey data: ${JSON.stringify(data)}`)
			if (err.code !== 0) {
				console.log(
					'test generateKey err information: ' + JSON.stringify(err)
				)
				reject(err)
			} else {
				resolve(data)
			}
		})
	})
}

async function publicImportKey(keyAlias, HuksOptions) {
	let _InData = HuksOptions.inData
	HuksOptions.inData = finishOutData
	console.log(`test ImportKey keyAlias: ${keyAlias}`)
	console.log(`test ImportKey HuksOptions: ${JSON.stringify(HuksOptions)}`)
	await importKey(keyAlias, HuksOptions)
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

function importKey(srcKeyAlies, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.importKey(srcKeyAlies, HuksOptions, function (err, data) {
			console.log(`test importKey data: ${JSON.stringify(data)}`)
			if (err.code !== 0) {
				console.log(
					'test importKey err information: ' + JSON.stringify(err)
				)
				reject(err)
			} else {
				resolve(data)
			}
		})
	})
}

async function publicExportKey(keyAlias, HuksOptions) {
	await exportkey(keyAlias, HuksOptions)
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

function exportkey(srcKeyAlies, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.exportKey(srcKeyAlies, HuksOptions, function (err, data) {
			console.log(`test exportKey data: ${JSON.stringify(data)}`)
			if (err.code !== 0) {
				console.log(
					'test exportKey err information: ' + JSON.stringify(err)
				)
				reject(err)
			} else {
				resolve(data)
			}
		})
	})
}

async function publicInitFunc(keyAlias, HuksOptions) {
	await init(keyAlias, HuksOptions)
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

function init(srcKeyAlies, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.init(srcKeyAlies, HuksOptions, function (err, data) {
			if (err.code !== 0) {
				console.log('test init err information: ' + JSON.stringify(err))
				reject(err)
			} else {
				resolve(data)
			}
		})
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
	await updateCallback(handle, HuksOptions)
		.then(async (data) => {
			console.log(`test update data ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log('test update err information: ' + err)
			expect(null).assertFail()
		})
}

function updateCallback(handle, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.update(handle, HuksOptions, function (err, data) {
			if (err.code !== 0) {
				console.log(
					'test update err information: ' + JSON.stringify(err)
				)
				reject(err)
			} else {
				resolve(data)
			}
		})
	})
}

async function publicFinishFunc(HuksOptions) {
	await finish(handle, HuksOptions)
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

function finish(handle, huksOptionsFinish) {
	return new Promise((resolve, reject) => {
		huks.finish(handle, huksOptionsFinish, function (err, data) {
			if (err.code !== 0) {
				console.log(
					'test generateKey err information: ' + JSON.stringify(err)
				)
				reject(err)
			} else {
				resolve(data)
			}
		})
	})
}

async function publicAbortFucn(HuksOptions) {
	await abort(handle, HuksOptions)
		.then((data) => {
			console.log(`test abort data: ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log('test abort err information: ' + JSON.stringify(err))
			expect(null).assertFail()
		})
}

function abort(handle, huksOptionsAbort) {
	return new Promise((resolve, reject) => {
		huks.abort(handle, huksOptionsAbort, function (err, data) {
			if (err.code !== 0) {
				console.log(
					'test abort err information: ' + JSON.stringify(err)
				)
				reject(err)
			} else {
				resolve(data)
			}
		})
	})
}

async function publicDeleteKeyFunc(KeyAlias, HuksOptions) {
	await deleteKey(KeyAlias, HuksOptions)
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

function deleteKey(srcKeyAlies, HuksOptions) {
	return new Promise((resolve, reject) => {
		huks.deleteKey(srcKeyAlies, HuksOptions, function (err, data) {
			if (err.code !== 0) {
				console.log(
					'test deleteKey err information: ' + JSON.stringify(err)
				)
				reject(err)
			} else {
				resolve(data)
			}
		})
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
				HuksSignVerify001.HuksKeyRSAPurposeSINGVERIFY
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
				HuksSignVerify001.HuksKeyRSAPurposeSIGN
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
					HuksSignVerify001.HuksKeyRSAPurposeSINGVERIFY
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
				HuksSignVerify001.HuksKeyRSAPurposeSINGVERIFY
			)
			await publicDeleteKeyFunc(srcKeyAlies, HuksOptions)
		} else if (!isSING) {
			HuksOptions.properties.splice(
				1,
				1,
				HuksSignVerify001.HuksKeyRSAPurposeVERIFY
			)
			await publicDeleteKeyFunc(newSrcKeyAlies, HuksOptions)
		}
	} catch (e) {
		expect(null).assertFail()
	}
}

describe('SecurityHuksSignVerifyRSACallbackJsunit', function () {
	it('testSignVerifyRSA103', 0, async function (done) {
		const srcKeyAlies = 'testSignVerifyRSASize512SIGNPKCS1MD5KeyAlias003'
		let HuksOptions = {
			properties: new Array(
				HuksSignVerify001.HuksKeyAlgRSA,
				HuksSignVerify001.HuksKeyRSAPurposeSIGN,
				HuksSignVerify001.HuksTagPKCS1DigestMD5,
				HuksSignVerify001.HuksKeyRSAPADDINGPKCS1V15,
				HuksSignVerify001.HuksKeyRSASize512
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
				HuksSignVerify001.HuksKeyAlgRSA,
				HuksSignVerify001.HuksKeyRSAPurposeVERIFY,
				HuksSignVerify001.HuksTagPKCS1DigestMD5,
				HuksSignVerify001.HuksKeyRSAPADDINGPKCS1V15,
				HuksSignVerify001.HuksKeyRSASize512
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
		finishOutData = 0
		exportKey = 0
		done()
	})
})
