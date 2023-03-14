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

let exportKeyFrist
let exportKeySecond
let handle = {}
let handle1
let handle2
let finishDataFrist
let finishDataSecond
let srcData63 = Data.Date63KB
let srcData63Kb = stringToUint8Array(srcData63)

let HksKeyAlg = {
	HKS_ALG_DH: 103,
	HKS_ALG_AES: 20,
	HKS_ALG_HMAC: 50,
	HKS_ALG_X25519: 102,
}

let HksKeyPurpose = {
	HKS_KEY_PURPOSE_AGREE: 256,
	HKS_KEY_PURPOSE_DERIVE: 16,
	HKS_KEY_PURPOSE_ENCRYPT: 1,
	HKS_KEY_PURPOSE_DECRYPT: 2,
	HKS_KEY_PURPOSE_MAC: 128,
}

let HksKeySize = {
	HKS_DH_KEY_SIZE_128: 128,
	HKS_DH_KEY_SIZE_192: 192,
	HKS_DH_KEY_SIZE_256: 256,
	HKS_DH_KEY_SIZE_512: 512,
	HKS_DH_KEY_SIZE_2048: 2048,
	HKS_DH_KEY_SIZE_3072: 3072,
	HKS_DH_KEY_SIZE_4096: 4096,
	HKS_CURVE25519_KEY_SIZE_256: 256,
}
let HksKeyStorageType = {
	HKS_STORAGE_TEMP: 0,
	HKS_STORAGE_PERSISTENT: 1,
}

let HksTagType = {
	HKS_TAG_TYPE_UINT: 2 << 28,
	HKS_TAG_TYPE_BOOL: 4 << 28,
	HKS_TAG_TYPE_BYTES: 5 << 28,
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

let HksKeyPadding = {
	HKS_PADDING_NONE: 0,
	HKS_PADDING_OAEP: 1,
	HKS_PADDING_PSS: 2,
	HKS_PADDING_PKCS1_V1_5: 3,
	HKS_PADDING_PKCS5: 4,
	HKS_PADDING_PKCS7: 5,
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
	HKS_TAG_KEY_STORAGE_FLAG: HksTagType.HKS_TAG_TYPE_UINT | 1002,
	HKS_TAG_IS_KEY_ALIAS: HksTagType.HKS_TAG_TYPE_BOOL | 1001,
	HKS_TAG_KEY_ALIAS: HksTagType.HKS_TAG_TYPE_BYTES | 23,
}

let HuksAgree003 = {
	HuksKeyAlgX25519: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_X25519,
	},
	HuksKeyPurposeAGREE: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_AGREE,
	},
	HuksKeyCURVE25519Size256: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_CURVE25519_KEY_SIZE_256,
	},
	HuksKeyBLOCKMODE: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_CBC,
	},
	HuksKeyPADDING: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_NONE,
	},
	HuksKeyDIGEST: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_NONE,
	},
	HuksKeySTORAGE: {
		tag: HksTag.HKS_TAG_KEY_STORAGE_FLAG,
		value: HksKeyStorageType.HKS_STORAGE_PERSISTENT,
	},
	HuksKeyISKEYALIAS: { tag: HksTag.HKS_TAG_IS_KEY_ALIAS, value: true },
	HuksKeyDIGESTNONE: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_NONE,
	},
	HuksKeyDIGESTSHA1: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA1,
	},
	HuksKeyDIGESTSHA224: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA224,
	},
	HuksKeyDIGESTSHA256: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA256,
	},
	HuksKeyDIGESTSHA384: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA384,
	},
	HuksKeyDIGESTSHA512: {
		tag: HksTag.HKS_TAG_DIGEST,
		value: HksKeyDigest.HKS_DIGEST_SHA512,
	},
	HuksKeyPADDINGNONE: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_NONE,
	},
	HuksKeyPADDINGPKCS7: {
		tag: HksTag.HKS_TAG_PADDING,
		value: HksKeyPadding.HKS_PADDING_PKCS7,
	},
	HuksKeyBLOCKMODECBC: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_CBC,
	},
	HuksKeyBLOCKMODECCM: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_CCM,
	},
	HuksKeyBLOCKMODEECB: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_ECB,
	},
	HuksKeyBLOCKMODECTR: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_CTR,
	},
	HuksKeyBLOCKMODEGCM: {
		tag: HksTag.HKS_TAG_BLOCK_MODE,
		value: HksCipherMode.HKS_MODE_GCM,
	},
	HuksKeyALGORITHMAES: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_AES,
	},
	HuksKeyALGORITHMHMAC: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_HMAC,
	},
	HuksKeySIZE256: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_DH_KEY_SIZE_256,
	},
	HuksKeySIZE128: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_DH_KEY_SIZE_128,
	},
	HuksKeySIZE192: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_DH_KEY_SIZE_192,
	},
	HuksKeyAlgDH: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_DH,
	},
	HuksKeyAlgAES: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_AES,
	},
	HuksKeyAlgHMAC: {
		tag: HksTag.HKS_TAG_ALGORITHM,
		value: HksKeyAlg.HKS_ALG_HMAC,
	},
	HuksKeyPurposeDERIVE: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_DERIVE,
	},
	HuksKeyPurposeMAC: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_MAC,
	},
	HuksKeyPurposeENCRYPTDECRYPT: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value:
			HksKeyPurpose.HKS_KEY_PURPOSE_ENCRYPT |
			HksKeyPurpose.HKS_KEY_PURPOSE_DECRYPT,
	},
	HuksKeyPurposeX25519: {
		tag: HksTag.HKS_TAG_PURPOSE,
		value: HksKeyPurpose.HKS_KEY_PURPOSE_AGREE,
	},
	HuksKeyDHSize2048: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_DH_KEY_SIZE_2048,
	},
	HuksKeyDHSize3072: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_DH_KEY_SIZE_3072,
	},
	HuksKeyDHSize4096: {
		tag: HksTag.HKS_TAG_KEY_SIZE,
		value: HksKeySize.HKS_DH_KEY_SIZE_4096,
	},
}

let HuksOptions63kb = {
	properties: new Array(
		HuksAgree003.HuksKeyAlgX25519,
		HuksAgree003.HuksKeyPurposeAGREE,
		HuksAgree003.HuksKeyCURVE25519Size256,
		HuksAgree003.HuksKeyDIGEST,
		HuksAgree003.HuksKeyPADDING,
		HuksAgree003.HuksKeyBLOCKMODE
	),
	inData: srcData63Kb,
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
async function publicAgreeGenFunc(srcKeyAlies, HuksOptions) {
	await huks
		.generateKey(srcKeyAlies, HuksOptions)
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

async function publicAgreeExport1Func(srcKeyAlies, HuksOptions, exportKey) {
	await huks
		.exportKey(srcKeyAlies, HuksOptions)
		.then((data) => {
			console.log(`test exportKey data: ${JSON.stringify(data)}`)
			if (exportKey == 1) {
				exportKeyFrist = data.outData
			} else {
				exportKeySecond = data.outData
			}
		})
		.catch((err) => {
			console.log(
				'test exportKey err information: ' + JSON.stringify(err)
			)
			expect(null).assertFail()
		})
}

async function publicAgreeInitFunc(srcKeyAlies, HuksOptions) {
	await huks
		.init(srcKeyAlies, HuksOptions)
		.then((data) => {
			console.log(`test init data ${JSON.stringify(data)}`)
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

async function publicAgreeUpdateFunc(HuksOptions, exportKey) {
	let _inData = HuksOptions.inData
	if (exportKey == 1) {
		HuksOptions.inData = exportKeySecond
	} else {
		HuksOptions.inData = exportKeyFrist
	}
	await huks
		.update(handle, HuksOptions)
		.then((data) => {
			console.log(`test update data ${JSON.stringify(data)}`)
			expect(data.errorCode == 0).assertTrue()
		})
		.catch((err) => {
			console.log('test update err information: ' + JSON.stringify(err))
			expect(null).assertFail()
		})
	HuksOptions.inData = _inData
}

async function publicAgreeFinishAbortFunc(
	huksOptionsFinish,
	thirdInderfaceName,
	finishData
) {
	if (thirdInderfaceName == 'finish') {
		console.log(
			`test befor finish huksOptionsFinish ${JSON.stringify(
				huksOptionsFinish
			)}`
		)
		await huks
			.finish(handle, huksOptionsFinish)
			.then((data) => {
				console.log(`test finish data ${JSON.stringify(data)}`)
				if (finishData == 1) {
					finishDataFrist = data.outData
				} else {
					finishDataSecond = data.outData
				}
				expect(data.errorCode == 0).assertTrue()
			})
			.catch((err) => {
				console.log(
					'test finish err information: ' + JSON.stringify(err)
				)
				expect(null).assertFail()
			})
	} else {
		let huksOptionsAbort = new Array({
			tag: HksTag.HKS_TAG_KEY_STORAGE_FLAG,
			value: HksKeyStorageType.HKS_STORAGE_TEMP,
		})
		await huks
			.abort(handle, huksOptionsAbort)
			.then((data) => {
				console.log(`test abort data ${JSON.stringify(data)}`)
				expect(data.errorCode == 0).assertTrue()
			})
			.catch((err) => {
				console.log(
					'test abort err information: ' + JSON.stringify(err)
				)
				expect(null).assertFail()
			})
	}
}

async function publicAgreeDeleteFunc(srcKeyAlies, HuksOptions) {
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

async function publicAgreeFunc(
	srcKeyAliesFirst,
	srcKeyAliesSecond,
	HuksOptions,
	huksOptionsFinish,
	thirdInderfaceName
) {
	try {
		await publicAgreeGenFunc(srcKeyAliesFirst, HuksOptions)
		await publicAgreeGenFunc(srcKeyAliesSecond, HuksOptions)
		await publicAgreeExport1Func(srcKeyAliesFirst, HuksOptions, 1)
		await publicAgreeExport1Func(srcKeyAliesSecond, HuksOptions, 2)

		await publicAgreeInitFunc(srcKeyAliesFirst, HuksOptions)
		await publicAgreeUpdateFunc(HuksOptions, 1)
		await publicAgreeFinishAbortFunc(
			huksOptionsFinish,
			thirdInderfaceName,
			1
		)

		let _huksOptionsFinish = huksOptionsFinish
		let huksOptionsFinishSecond = _huksOptionsFinish
		huksOptionsFinishSecond.properties.splice(6, 1, {
			tag: HksTag.HKS_TAG_KEY_ALIAS,
			value: stringToUint8Array(srcKeyAliesSecond + 'final'),
		})

		await publicAgreeInitFunc(srcKeyAliesSecond, HuksOptions)
		await publicAgreeUpdateFunc(HuksOptions, 2)
		await publicAgreeFinishAbortFunc(
			huksOptionsFinishSecond,
			thirdInderfaceName,
			2
		)

		await publicAgreeDeleteFunc(srcKeyAliesFirst, HuksOptions)
		await publicAgreeDeleteFunc(srcKeyAliesSecond + 'final', HuksOptions)
		await publicAgreeDeleteFunc(srcKeyAliesSecond, HuksOptions)
	} catch (e) {
		expect(null).assertFail()
	}
}

describe('SecurityHuksAgreeX25519PromiseJsunit', function () {
	it('testAgreeX25519Finish63KBAgree001', 0, async function (done) {
		const srcKeyAliesFirst =
			'testAgreeX25519Size256Finish63KBAgreeKeyAlias_01_001'
		const srcKeyAliesSecond =
			'testAgreeX25519Size256Finish63KBAgreeKeyAlias_02_001'
		let huksOptionsFinish = {
			properties: new Array(
				HuksAgree003.HuksKeySTORAGE,
				HuksAgree003.HuksKeyISKEYALIAS,
				HuksAgree003.HuksKeyALGORITHMAES,
				HuksAgree003.HuksKeySIZE256,
				HuksAgree003.HuksKeyPurposeENCRYPTDECRYPT,
				HuksAgree003.HuksKeyDIGESTNONE,
				{
					tag: HksTag.HKS_TAG_KEY_ALIAS,
					value: stringToUint8Array(srcKeyAliesFirst),
				},
				HuksAgree003.HuksKeyPADDINGNONE,
				HuksAgree003.HuksKeyBLOCKMODEECB
			),
			inData: srcData63Kb,
		}
		await publicAgreeFunc(
			srcKeyAliesFirst,
			srcKeyAliesSecond,
			HuksOptions63kb,
			huksOptionsFinish,
			'finish'
		)
		done()
	})
})
