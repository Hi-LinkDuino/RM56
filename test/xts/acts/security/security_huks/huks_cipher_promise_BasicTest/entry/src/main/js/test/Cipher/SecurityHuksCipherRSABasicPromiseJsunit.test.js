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

let gInData64 =
	'RSA_64_ttttttttttttttttttttttttttttttttttttttttttttttttttttttttt'

let HksKeyAlg = {
	HKS_ALG_RSA: 1,
}

let HksKeyPurpose = {
	HKS_KEY_PURPOSE_ENCRYPT: 1,
	HKS_KEY_PURPOSE_DECRYPT: 2,
}

let HksKeyPadding = {
	HKS_PADDING_NONE: 0,
	HKS_PADDING_OAEP: 1,
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
}

let HuksCipher002 = {
	HuksKeyAlgRSA: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_RSA,
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
	HuksKeyRSASize512: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_RSA_KEY_SIZE_512,
	},
	HuksKeyRSAPADDINGNONE: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_NONE,
	},
	HuksKeyRSAPADDINGPKCS1V15: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_PKCS1_V1_5,
	},
	HuksKeyRSAPADDINGOAEP: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_OAEP,
	},
	HuksKeyRSABLOCKMODEECB: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_ECB,
	},
	HuksKeyRSADIGESTNONE: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_NONE,
	},
	HuksKeyRSADIGESTSHA1: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA1,
	},
	HuksKeyRSADIGESTSHA224: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA224,
	},
	HuksKeyRSADIGESTSHA256: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA256,
	},
	HuksKeyRSADIGESTSHA384: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA384,
	},
	HuksKeyRSADIGESTSHA512: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA512,
	},
}
let defaultData = '0'

let gInData64Array = stringToUint8Array(gInData64)

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

let encryptFinishData
let encryptedData
let inputInData
var handle = {}
var handle1
var handle2
let updateResult = new Array()
let exportKey
let importKey

let genHuksOptions = {
	properties: new Array(
		HuksCipher002.HuksKeyAlgRSA,
		HuksCipher002.HuksKeyPurpose,
		HuksCipher002.HuksKeyRSASize512
	),
	inData: new Uint8Array(defaultData),
}

async function publicGenerateKeyFunc(srcKeyAlies, genHuksOptionsNONC) {
	console.log(
		`test Generate HuksOptions: ${JSON.stringify(genHuksOptionsNONC)}`
	)
	await huks
		.generateKey(srcKeyAlies, genHuksOptionsNONC)
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

async function publicExportKeyFunc(srcKeyAlies, genHuksOptionsNONC) {
	console.log(
		`test ExportKey HuksOptions: ${JSON.stringify(genHuksOptionsNONC)}`
	)
	await huks
		.exportKey(srcKeyAlies, genHuksOptionsNONC)
		.then((data) => {
			console.log(`test ExportKey data: ${JSON.stringify(data)}`)
			exportKey = data.outData
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log(
				'test ImportKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
}

async function publicImportKeyFunc(srcKeyAlies, HuksOptions) {
	HuksOptions.inData = exportKey
	console.log(`test importKey HuksOptions: ${JSON.stringify(HuksOptions)}`)
	await huks
		.importKey(srcKeyAlies, HuksOptions)
		.then((data) => {
			console.log(`test ImportKey data: ${JSON.stringify(data)}`)
			importKey = data.outData
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log(
				'test ImportKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
}

async function publicInitFunc(srcKeyAlies, HuksOptions) {
	console.log(`test init HuksOptions: ${JSON.stringify(HuksOptions)}`)
	await huks
		.init(srcKeyAlies, HuksOptions)
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

async function publicUpdateFunc(HuksOptions) {
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
		HuksOptions.inData = stringToUint8Array('0')
	} else {
		let count = Math.floor(Array.from(inDataArray).length / dateSize)
		let remainder = Array.from(inDataArray).length % dateSize
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
			HuksOptions.inData = huksOptionsInData
		}
		if (remainder !== 0) {
			HuksOptions.inData = new Uint8Array(
				Array.from(huksOptionsInData).slice(
					dateSize * count,
					uint8ArrayToString(inDataArray).length
				)
			)
			await update(handle, HuksOptions)
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

async function publicFinishAbortFunc(
	HuksOptions,
	thirdInderfaceName,
	isEncrypt
) {
	if (thirdInderfaceName == 'finish') {
		HuksOptions.outData = new Uint8Array(new Array(1024))
		HuksOptions.inData = new Uint8Array(new Array())
		await finish(HuksOptions, isEncrypt)
	} else if (thirdInderfaceName == 'abort') {
		await abort(HuksOptions)
	}
}

async function finish(HuksOptions, isEncrypt) {
	console.log('test before finish HuksOptions inData: ' + HuksOptions.inData)
	console.log(
		'test before finish HuksOptions outData: ' + HuksOptions.outData
	)
	console.log(
		'test before finish HuksOptions outData: ' + HuksOptions.outData.length
	)
	console.log(
		'test before finish HuksOptions properties: ' +
			JSON.stringify(HuksOptions.properties)
	)

	await huks
		.finish(handle, HuksOptions)
		.then((data) => {
			console.log(`test finish data: ${JSON.stringify(data)}`)
			if (isEncrypt) {
				updateResult = Array.from(data.outData)
				if (
					uint8ArrayToString(data.outData) ===
					uint8ArrayToString(encryptedData)
				) {
					console.log(
						`test finish Encrypt fail ${uint8ArrayToString(
							encryptedData
						)}`
					)
					console.log(
						`test finish Encrypt fail ${uint8ArrayToString(
							data.outData
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
							data.outData
						)}`
					)
					expect(data.errorCode == 0).assertTrue()
				}
			}
			if (!isEncrypt) {
				if (
					uint8ArrayToString(data.outData) ===
					uint8ArrayToString(encryptedData)
				) {
					console.log(
						`test finish Decrypt success ${uint8ArrayToString(
							encryptedData
						)}`
					)
					console.log(
						`test finish Decrypt success ${uint8ArrayToString(
							data.outData
						)}`
					)
					expect(data.errorCode == 0).assertTrue()
				} else {
					console.log(
						`test finish Encrypt fail ${uint8ArrayToString(
							encryptedData
						)}`
					)
					console.log(
						`test finish Encrypt fail ${uint8ArrayToString(
							data.outData
						)}`
					)
					expect(null).assertFail()
				}
			}
		})
		.catch((err) => {
			console.log('test finish err information: ' + JSON.stringify(err))
			expect(null).assertFail()
		})
}

async function abort(HuksOptions) {
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

async function publicDeleteKeyFunc(srcKeyAlies, genHuksOptionsNONC) {
	await huks
		.deleteKey(srcKeyAlies, genHuksOptionsNONC)
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

async function publicCipherFunc(
	srcKeyAlies,
	newSrcKeyAlies,
	genHuksOptionsNONC,
	HuksOptions,
	thirdInderfaceName,
	isEncrypt
) {
	inputInData = HuksOptions.inData
	try {
		updateResult = new Array()
		let KeyAlias = srcKeyAlies
		if (isEncrypt) {
			await publicGenerateKeyFunc(srcKeyAlies, genHuksOptionsNONC)
			encryptedData = HuksOptions.inData
			await publicExportKeyFunc(srcKeyAlies, genHuksOptionsNONC)
			await publicImportKeyFunc(newSrcKeyAlies, HuksOptions)
			KeyAlias = newSrcKeyAlies
		}
		HuksOptions.inData = inputInData
		await publicInitFunc(KeyAlias, HuksOptions)
		await publicUpdateFunc(HuksOptions)
		await publicFinishAbortFunc(HuksOptions, thirdInderfaceName, isEncrypt)
		if (!isEncrypt || (isEncrypt && thirdInderfaceName == 'abort')) {
			await publicDeleteKeyFunc(srcKeyAlies, genHuksOptionsNONC)
			await publicDeleteKeyFunc(newSrcKeyAlies, genHuksOptionsNONC)
		}
	} catch (e) {
		expect(null).assertFail()
	}
}

describe('SecurityHuksCipherRSAPromiseJsunit', function () {
	it('testCipherRSA001', 0, async function (done) {
		const srcKeyAlies = 'testCipherRSASize512PADDINGNONESHA256KeyAlias001'
		const newSrcKeyAlies =
			'testCipherRSASize512PADDINGNONESHA256NewKeyAlias001'
		genHuksOptions.properties.splice(
			3,
			1,
			HuksCipher002.HuksKeyRSABLOCKMODEECB
		)
		genHuksOptions.properties.splice(
			4,
			1,
			HuksCipher002.HuksKeyRSAPADDINGNONE
		)
		genHuksOptions.properties.splice(
			5,
			1,
			HuksCipher002.HuksKeyRSADIGESTSHA256
		)

		let HuksOptions = {
			properties: new Array(
				HuksCipher002.HuksKeyAlgRSA,
				HuksCipher002.HuksKeyPurposeENCRYPT,
				HuksCipher002.HuksKeyRSASize512,
				HuksCipher002.HuksKeyRSAPADDINGNONE,
				HuksCipher002.HuksKeyRSABLOCKMODEECB,
				HuksCipher002.HuksKeyRSADIGESTSHA256
			),
			inData: gInData64Array,
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			newSrcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'finish',
			true
		)
		HuksOptions = {
			properties: new Array(
				HuksCipher002.HuksKeyAlgRSA,
				HuksCipher002.HuksKeyPurposeDECRYPT,
				HuksCipher002.HuksKeyRSASize512,
				HuksCipher002.HuksKeyRSAPADDINGNONE,
				HuksCipher002.HuksKeyRSABLOCKMODEECB,
				HuksCipher002.HuksKeyRSADIGESTSHA256
			),
			inData: new Uint8Array(updateResult),
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			newSrcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'finish',
			false
		)
		done()
	})

	it('testCipherRSA002', 0, async function (done) {
		const srcKeyAlies = 'testCipherRSASize512PADDINGNONESHA256KeyAlias002'
		const newSrcKeyAlies =
			'testCipherRSASize512PADDINGNONESHA256NewKeyAlias001'
		let HuksOptions = {
			properties: new Array(
				HuksCipher002.HuksKeyAlgRSA,
				HuksCipher002.HuksKeyPurposeENCRYPT,
				HuksCipher002.HuksKeyRSASize512,
				HuksCipher002.HuksKeyRSAPADDINGNONE,
				HuksCipher002.HuksKeyRSABLOCKMODEECB,
				HuksCipher002.HuksKeyRSADIGESTSHA256
			),
			inData: gInData64Array,
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			newSrcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'abort',
			true
		)
		done()
	})

	it('testCipherRSA003', 0, async function (done) {
		const srcKeyAlies = 'testCipherRSASize512PADDINGNONESHA256KeyAlias001'
		const newSrcKeyAlies =
			'testCipherRSASize512PADDINGNONESHA256NewKeyAlias001'
		let HuksOptions = {
			properties: new Array(
				HuksCipher002.HuksKeyAlgRSA,
				HuksCipher002.HuksKeyPurposeENCRYPT,
				HuksCipher002.HuksKeyRSASize512,
				HuksCipher002.HuksKeyRSAPADDINGNONE,
				HuksCipher002.HuksKeyRSABLOCKMODEECB,
				HuksCipher002.HuksKeyRSADIGESTSHA256
			),
			inData: gInData64Array,
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			newSrcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'finish',
			true
		)
		HuksOptions = {
			properties: new Array(
				HuksCipher002.HuksKeyAlgRSA,
				HuksCipher002.HuksKeyPurposeDECRYPT,
				HuksCipher002.HuksKeyRSASize512,
				HuksCipher002.HuksKeyRSAPADDINGNONE,
				HuksCipher002.HuksKeyRSABLOCKMODEECB,
				HuksCipher002.HuksKeyRSADIGESTSHA256
			),
			inData: new Uint8Array(updateResult),
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			newSrcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'finish',
			false
		)
		done()
	})

	it('testCipherRSA004', 0, async function (done) {
		const srcKeyAlies = 'testCipherRSASize512PADDINGNONESHA256KeyAlias002'
		const newSrcKeyAlies =
			'testCipherRSASize512PADDINGNONESHA256NewKeyAlias001'
		let HuksOptions = {
			properties: new Array(
				HuksCipher002.HuksKeyAlgRSA,
				HuksCipher002.HuksKeyPurposeENCRYPT,
				HuksCipher002.HuksKeyRSASize512,
				HuksCipher002.HuksKeyRSAPADDINGNONE,
				HuksCipher002.HuksKeyRSABLOCKMODEECB,
				HuksCipher002.HuksKeyRSADIGESTSHA256
			),
			inData: gInData64Array,
			outData: stringToUint8Array('0'),
		}
		await publicCipherFunc(
			srcKeyAlies,
			newSrcKeyAlies,
			genHuksOptions,
			HuksOptions,
			'abort',
			true
		)
		done()
	})
})
