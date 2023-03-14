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

let IV = '0000000000000000'
let AAD = '0000000000000000'
let NONCE = '000000000000'
let AEAD = '0000000000000000'

let HksKeyAlg = {
	HKS_ALG_AES: 20,
}

let HksKeyPurpose = {
	HKS_KEY_PURPOSE_ENCRYPT: 1,
	HKS_KEY_PURPOSE_DECRYPT: 2,
}

let HksKeyPadding = {
	HKS_PADDING_NONE: 0,
	HKS_PADDING_PKCS7: 5,
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
	HKS_AES_KEY_SIZE_128: 128,
	HKS_AES_KEY_SIZE_192: 192,
	HKS_AES_KEY_SIZE_256: 256,
}

let HksTagType = {
	HKS_TAG_TYPE_UINT: 2 << 28,
	HKS_TAG_TYPE_BYTES: 5 << 28,
}

let HksCipherMode = {
	HKS_MODE_ECB: 1,
	HKS_MODE_CBC: 2,
	HKS_MODE_CTR: 3,
	HKS_MODE_OFB: 4,
	HKS_MODE_CCM: 31,
	HKS_MODE_GCM: 32,
}

let HksTag = {
	HKS_TAG_ALGORITHM: HksTagType.HKS_TAG_TYPE_UINT | 1,
	HKS_TAG_PURPOSE: HksTagType.HKS_TAG_TYPE_UINT | 2,
	HKS_TAG_KEY_SIZE: HksTagType.HKS_TAG_TYPE_UINT | 3,
	HKS_TAG_DIGEST: HksTagType.HKS_TAG_TYPE_UINT | 4,
	HKS_TAG_PADDING: HksTagType.HKS_TAG_TYPE_UINT | 5,
	HKS_TAG_BLOCK_MODE: HksTagType.HKS_TAG_TYPE_UINT | 6,
	HKS_TAG_ASSOCIATED_DATA: HksTagType.HKS_TAG_TYPE_BYTES | 8,
	HKS_TAG_NONCE: HksTagType.HKS_TAG_TYPE_BYTES | 9,
	HKS_TAG_IV: HksTagType.HKS_TAG_TYPE_BYTES | 10,
	HKS_TAG_AGREE_PUBLIC_KEY: HksTagType.HKS_TAG_TYPE_BYTES | 22,
	HKS_TAG_AE_TAG: HksTagType.HKS_TAG_TYPE_BYTES | 10009,
}

let HuksCipher001 = {
	HuksKeyAlgAES: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_AES,
	},
	HuksKeyPurpose: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value:
			HksKeyPurpose.HKS_KEY_PURPOSE_ENCRYPT |
			HksKeyPurpose.HKS_KEY_PURPOSE_DECRYPT,
	},
	HuksKeyPurposeENCRYPT: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_ENCRYPT,
	},
	HuksKeyPurposeDECRYPT: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_DECRYPT,
	},
	HuksKeyAESSize128: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_AES_KEY_SIZE_128,
	},
	HuksKeyAESSize192: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_AES_KEY_SIZE_192,
	},
	HuksKeyAESSize256: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_AES_KEY_SIZE_256,
	},
	HuksKeyAESPADDINGNONE: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_NONE,
	},
	HuksKeyAESPADDINGPKCS7: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_PKCS7,
	},
	HuksKeyAESBLOCKMODE: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_CBC,
	},
	HuksKeyAESBLOCKMODECTR: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_CTR,
	},
	HuksKeyAESBLOCKMODEECB: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_ECB,
	},
	HuksKeyAESDIGESTNONE: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_NONE,
	},
}
let defaultData = '0'

let srcData65 =
	'Hks_AES_Cipher_Test_00000000000000000000000000000000000000000000000000000_string'
let srcData65Kb = stringToUint8Array(srcData65)

let srcData63 = 'Hks_AES_Cipher_Test_000000000000000000000_string'
let srcData63Kb = stringToUint8Array(srcData63)

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

let updateResult = new Array()
let encryptFinishData
let encryptedData
var handle = {}
var handle1
var handle2

let genHuksOptions = {
	properties: new Array(
		HuksCipher001.HuksKeyAlgAES,
		HuksCipher001.HuksKeyPurpose
	),
	inData: new Uint8Array(defaultData),
}

async function publicGenerateKeyFunc(srcKeyAlies, genHuksOptionsNONECBC) {
	console.log(
		`test GenerateHuksOptions: ${JSON.stringify(genHuksOptionsNONECBC)}`
	)
	await generateKey(srcKeyAlies, genHuksOptionsNONECBC)
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

async function publicInitFunc(srcKeyAlies, HuksOptions) {
	console.log(`test init HuksOptions: ${JSON.stringify(HuksOptions)}`)
	await init(srcKeyAlies, HuksOptions)
		.then((data) => {
			console.log(`test init data: ${JSON.stringify(data)}`)
			handle1 = data.handle1
			handle2 = data.handle2
			handle = {
				handle1: handle1,
				handle2: handle2,
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

async function publicUpdateFunc(HuksOptions, thirdInderfaceName, isEncrypt) {
	console.log(
		`test update before handle: ${JSON.stringify(
			handle
		)} HuksOptions: ${JSON.stringify(HuksOptions)}`
	)
	let dateSize = 64
	let huksOptionsInData = HuksOptions.inData
	let inDataArray = HuksOptions.inData
	console.log(
		'test update finish HuksOptions inData: ' +
			Array.from(inDataArray).length
	)
	if (Array.from(inDataArray).length < dateSize) {
		await update(handle, HuksOptions)
		//        HuksOptions.inData = new Uint8Array(new Array());
		await publicFinishAbortFunc(
			HuksOptions,
			thirdInderfaceName,
			isEncrypt,
			0,
			huksOptionsInData.length
		)
	} else {
		let count = Math.floor(Array.from(inDataArray).length / dateSize)
		let remainder = Array.from(inDataArray).length % dateSize
		console.log('test count ' + count + 'remainder ' + remainder)
		for (let i = 0; i < count; i++) {
			HuksOptions.inData = new Uint8Array(
				Array.from(huksOptionsInData).slice(
					dateSize * i,
					dateSize * (i + 1)
				)
			)
			console.log(
				'test ' +
					uint8ArrayToString(
						new Uint8Array(
							Array.from(huksOptionsInData).slice(
								dateSize * i,
								dateSize * (i + 1)
							)
						)
					)
			)
			await update(handle, HuksOptions)
		}
		HuksOptions.inData = huksOptionsInData
		if (remainder !== 0) {
			HuksOptions.inData = new Uint8Array(
				Array.from(huksOptionsInData).slice(
					dateSize * count,
					uint8ArrayToString(inDataArray).length
				)
			)
			console.log(
				'test ' +
					uint8ArrayToString(
						new Uint8Array(
							Array.from(huksOptionsInData).slice(
								dateSize * count,
								uint8ArrayToString(inDataArray).length
							)
						)
					)
			)
		}
		await publicFinishAbortFunc(
			HuksOptions,
			thirdInderfaceName,
			isEncrypt,
			remainder,
			huksOptionsInData.length
		)
	}
}

async function update(handle, HuksOptions) {
	await updateCallback(handle, HuksOptions)
		.then(async (data) => {
			console.log(`test update data ${JSON.stringify(data)}`)
			if (updateResult.length !== 0) {
				updateResult = updateResult.concat(Array.from(data.outData))
			} else {
				updateResult = Array.from(data.outData)
			}
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

async function publicFinishAbortFunc(
	HuksOptions,
	thirdInderfaceName,
	isEncrypt,
	remainder,
	dataLength
) {
	if (thirdInderfaceName == 'finish') {
		HuksOptions.outData = new Uint8Array(
			new Array(encryptedData.length * 2)
		)
		console.log(`test remainder ${remainder}`)
		await finish(HuksOptions, isEncrypt)
	} else if (thirdInderfaceName == 'abort') {
		HuksOptions.outData = new Uint8Array(
			new Array(encryptedData.length * 2)
		)
		await abort(HuksOptions)
	}
}

async function finish(HuksOptions, isEncrypt) {
	await finishCallback(handle, HuksOptions)
		.then((data) => {
			console.log(`test finish data: ${JSON.stringify(data)}`)
			let finishData
			if (encryptedData.length > 64) {
				finishData = uint8ArrayToString(
					updateResult.concat(Array.from(data.outData))
				)
				updateResult = updateResult.concat(Array.from(data.outData))
			} else {
				finishData = uint8ArrayToString(updateResult)
			}
			if (isEncrypt) {
				if (finishData === uint8ArrayToString(encryptedData)) {
					console.log(
						`test finish Encrypt fail ${uint8ArrayToString(
							encryptedData
						)}`
					)
					console.log(
						`test finish Encrypt fail ${uint8ArrayToString(
							finishData
						)}`
					)
					expect(null).assertFail()
				} else {
					console.log(
						`test finish Encrypt success ${uint8ArrayToString(
							encryptedData
						)}`
					)
					console.log(
						`test finish Encrypt success ${uint8ArrayToString(
							finishData
						)}`
					)
					expect(data.errorCode == 0).assertTrue()
				}
			}
			if (!isEncrypt) {
				if (finishData === uint8ArrayToString(encryptedData)) {
					expect(data.errorCode == 0).assertTrue()
				} else {
					expect(null).assertFail()
				}
			}
		})
		.catch((err) => {
			console.log('test finish err information: ' + JSON.stringify(err))
			expect(null).assertFail()
		})
}

function finishCallback(handle, huksOptionsFinish) {
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

async function abort(HuksOptions) {
	await abortCallback(handle, HuksOptions)
		.then((data) => {
			console.log(`test abort data: ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log('test abort err information: ' + JSON.stringify(err))
			expect(null).assertFail()
		})
}

function abortCallback(handle, huksOptionsAbort) {
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

async function publicDeleteKeyFunc(srcKeyAlies, genHuksOptionsNONECBC) {
	await deleteKey(srcKeyAlies, genHuksOptionsNONECBC)
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

async function publicCipherFunc(
	srcKeyAlies,
	genHuksOptionsNONECBC,
	HuksOptions,
	thirdInderfaceName,
	isEncrypt
) {
	try {
		updateResult = new Array()
		if (isEncrypt) {
			await publicGenerateKeyFunc(srcKeyAlies, genHuksOptionsNONECBC)
			encryptedData = HuksOptions.inData
		}
		await publicInitFunc(srcKeyAlies, HuksOptions)
		await publicUpdateFunc(HuksOptions, thirdInderfaceName, isEncrypt)
		if (!isEncrypt || (isEncrypt && thirdInderfaceName == 'abort')) {
			await publicDeleteKeyFunc(srcKeyAlies, genHuksOptionsNONECBC)
		}
	} catch (e) {
		expect(null).assertFail()
	}
}

describe('SecurityHuksCipherAESCallbackJsunit', function () {
	it('testCipherAES001', 0, async function (done) {
		const srcKeyAlies = 'testCipherAESSize128PADDINGNONEMODECBCKeyAlias001'
		genHuksOptions.properties.splice(2, 1, HuksCipher001.HuksKeyAESSize128)
		genHuksOptions.properties.splice(
			3,
			1,
			HuksCipher001.HuksKeyAESBLOCKMODE
		)
		genHuksOptions.properties.splice(
			4,
			1,
			HuksCipher001.HuksKeyAESPADDINGNONE
		)
		let HuksOptions = {
			properties: new Array(
				HuksCipher001.HuksKeyAlgAES,
				HuksCipher001.HuksKeyPurposeENCRYPT,
				HuksCipher001.HuksKeyAESSize128,
				HuksCipher001.HuksKeyAESPADDINGNONE,
				HuksCipher001.HuksKeyAESBLOCKMODE,
				HuksCipher001.HuksKeyAESDIGESTNONE,
				{ tag: HksTag.HKS_TAG_IV, value: stringToUint8Array(IV) }
			),
			inData: srcData63Kb,
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'finish',
			true
		)
		HuksOptions = {
			properties: new Array(
				HuksCipher001.HuksKeyAlgAES,
				HuksCipher001.HuksKeyPurposeDECRYPT,
				HuksCipher001.HuksKeyAESSize128,
				HuksCipher001.HuksKeyAESPADDINGNONE,
				HuksCipher001.HuksKeyAESBLOCKMODE,
				HuksCipher001.HuksKeyAESDIGESTNONE,
				{ tag: HksTag.HKS_TAG_IV, value: stringToUint8Array(IV) }
			),
			inData: new Uint8Array(updateResult),
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'finish',
			false
		)
		done()
	})

	it('testCipherAES002', 0, async function (done) {
		const srcKeyAlies = 'testCipherAESSize128PADDINGNONEMODECBCKeyAlias002'
		let HuksOptions = {
			properties: new Array(
				HuksCipher001.HuksKeyAlgAES,
				HuksCipher001.HuksKeyPurposeENCRYPT,
				HuksCipher001.HuksKeyAESSize128,
				HuksCipher001.HuksKeyAESPADDINGNONE,
				HuksCipher001.HuksKeyAESBLOCKMODE,
				HuksCipher001.HuksKeyAESDIGESTNONE,
				{ tag: HksTag.HKS_TAG_IV, value: stringToUint8Array(IV) }
			),
			inData: new Uint8Array(updateResult),
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'abort',
			true
		)
		done()
	})

	it('testCipherAES003', 0, async function (done) {
		const srcKeyAlies = 'testCipherAESSize128PADDINGNONEMODECBCKeyAlias001'
		let HuksOptions = {
			properties: new Array(
				HuksCipher001.HuksKeyAlgAES,
				HuksCipher001.HuksKeyPurposeENCRYPT,
				HuksCipher001.HuksKeyAESSize128,
				HuksCipher001.HuksKeyAESPADDINGNONE,
				HuksCipher001.HuksKeyAESBLOCKMODE,
				HuksCipher001.HuksKeyAESDIGESTNONE,
				{ tag: HksTag.HKS_TAG_IV, value: stringToUint8Array(IV) }
			),
			inData: srcData63Kb,
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'finish',
			true
		)
		HuksOptions = {
			properties: new Array(
				HuksCipher001.HuksKeyAlgAES,
				HuksCipher001.HuksKeyPurposeDECRYPT,
				HuksCipher001.HuksKeyAESSize128,
				HuksCipher001.HuksKeyAESPADDINGNONE,
				HuksCipher001.HuksKeyAESBLOCKMODE,
				HuksCipher001.HuksKeyAESDIGESTNONE,
				{ tag: HksTag.HKS_TAG_IV, value: stringToUint8Array(IV) }
			),
			inData: new Uint8Array(updateResult),
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'finish',
			false
		)
		done()
	})

	it('testCipherAES004', 0, async function (done) {
		const srcKeyAlies = 'testCipherAESSize128PADDINGNONEMODECBCKeyAlias002'
		let HuksOptions = {
			properties: new Array(
				HuksCipher001.HuksKeyAlgAES,
				HuksCipher001.HuksKeyPurposeENCRYPT,
				HuksCipher001.HuksKeyAESSize128,
				HuksCipher001.HuksKeyAESPADDINGNONE,
				HuksCipher001.HuksKeyAESBLOCKMODE,
				HuksCipher001.HuksKeyAESDIGESTNONE,
				{ tag: HksTag.HKS_TAG_IV, value: stringToUint8Array(IV) }
			),
			inData: new Uint8Array(updateResult),
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'abort',
			true
		)
		done()
	})
})
