/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
 
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'
import display from '@ohos.display'
import screenshot from '@ohos.screenshot'
import image from '@ohos.multimedia.image'
import screen from '@ohos.screen'

describe('screenshot_test', function () {
	
	beforeAll(function () {
	})
	beforeEach(function () {
	})
	afterEach(function () {
	})
	afterAll(function () {
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_001
	 * @tc.name       Test screenShotSave_Test_001
	 * @tc.desc       Set the screenshot area to be larger than the screen width and height
	 */
	it('screenShotSave_Test_001', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest1 begin');
		display.getDefaultDisplay().then((dsp) => {
			console.log('screenshotTest screenShotSaveTest1 display.getDefaultDisplay success, dsp :' + JSON.stringify(dsp));
			expect(dsp != null).assertTrue();
			var screenshotOptions = {
				screenRect: {
					left: 0,
					top: 0,
					width: 10000,
					height: 10000,
				},
				imageSize: {
					width: 300,
					height: 300
				},
				displayId: dsp.id
			};
			screenshot.save(screenshotOptions).then((data) => {
				console.log('screenshotTest screenShotSaveTest1 screenshot.save success, data :' + JSON.stringify(data));
				expect().assertFail();
				done();
			}, (err) => {
				console.log('screenshotTest screenShotSaveTest1 screenshot.save failed, err : ' + JSON.stringify(err));
				expect(err.code == 120).assertTrue();
				done();
			})
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest1 display.getDefaultDisplay failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_002
	 * @tc.name       Test screenShotSave_Test_002
	 * @tc.desc       Set the size of the screenshot area and the generated image to be larger than the screen width and height
	 */
	it('screenShotSave_Test_002', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest2 begin');
		display.getDefaultDisplay().then((dsp) => {
			console.log('screenshotTest screenShotSaveTest2 display.getDefaultDisplay success, dsp :' + JSON.stringify(dsp));
			expect(dsp != null).assertTrue();
			var screenshotOptions = {
				screenRect: {
					left: 0,
					top: 0,
					width: 10000,
					height: 10000,
				},
				imageSize: {
					width: 3000,
					height: 3000
				},
				displayId: dsp.id
			};
			screenshot.save(screenshotOptions).then((data) => {
				console.log('screenshotTest screenShotSaveTest2 screenshot.save success, data :' + JSON.stringify(data));
				expect().assertFail();
				done();
			}, (err) => {
				console.log('screenshotTest screenShotSaveTest2 screenshot.save failed, err : ' + JSON.stringify(err));
				expect(err.code == 120).assertTrue();
				done();
			})
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest2 display.getDefaultDisplay failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done()
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_003
	 * @tc.name       Test screenShotSave_Test_003
	 * @tc.desc       Set the screenshot area and the size of the generated image to be smaller than the screen width and height
	 */
	it('screenShotSave_Test_003', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest3 begin');
		display.getDefaultDisplay().then((dsp) => {
			console.log('screenshotTest screenShotSaveTest3 display.getDefaultDisplay success, dsp :' + JSON.stringify(dsp));
			expect(dsp != null).assertTrue();
			var screenshotOptions = {
				screenRect: {
					left: 0,
					top: 0,
					width: 100,
					height: 100,
				},
				imageSize: {
					width: 300,
					height: 300
				},
				displayId: dsp.id
			};
			screenshot.save(screenshotOptions).then((data) => {
				console.log('screenshotTest screenShotSaveTest3 screenshot.save success, data :' + JSON.stringify(data));
				expect(data != null).assertTrue();
				done();
			}, (err) => {
				console.log('screenshotTest screenShotSaveTest3 screenshot.save failed, err : ' + JSON.stringify(err));
				expect().assertFail();
				done();
			})
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest3 display.getDefaultDisplay failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_004
	 * @tc.name       Test screenShotSave_Test_004
	 * @tc.desc       Set the size of the generated image to be smaller than the screen width and height
	 */
	it('screenShotSave_Test_004', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest4 begin');
		display.getDefaultDisplay().then((dsp) => {
			console.log('screenshotTest screenShotSaveTest4 display.getDefaultDisplay success, dsp :' + JSON.stringify(dsp));
			expect(dsp != null).assertTrue();
			var screenshotOptions = {
				screenRect: {
					left: 0,
					top: 0,
					width: 100,
					height: 100,
				},
				imageSize: {
					width: 3000,
					height: 3000
				},
				displayId: dsp.id
			};
			screenshot.save(screenshotOptions).then((data) => {
				console.log('screenshotTest screenShotSaveTest4 screenshot.save success, data :' + JSON.stringify(data));
				expect(data != null).assertTrue();
				done();
			}, (err) => {
				console.log('screenshotTest screenShotSaveTest4 screenshot.save failed, err : ' + JSON.stringify(err));
				expect().assertFail();
				done();
			})
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest4 display.getDefaultDisplay failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_005
	 * @tc.name       Test screenShotSave_Test_005
	 * @tc.desc       Set the screenshot area to a negative value
	 */
	it('screenShotSave_Test_005', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest5 begin');
		display.getDefaultDisplay().then((dsp) => {
			console.log('screenshotTest screenShotSaveTest5 display.getDefaultDisplay success, dsp :' + JSON.stringify(dsp));
			expect(dsp != null).assertTrue();
			var screenshotOptions = {
				screenRect: {
					left: -50,
					top: -50,
					width: -100,
					height: -100,
				},
				imageSize: {
					width: 300,
					height: 300
				},
				displayId: dsp.id
			};
			screenshot.save(screenshotOptions).then((data) => {
				console.log('screenshotTest screenShotSaveTest5 screenshot.save success, data :' + JSON.stringify(data));
				expect().assertFail();
				done();
			}, (err) => {
				console.log('screenshotTest screenShotSaveTest5 screenshot.save failed, err : ' + JSON.stringify(err));
				expect(err.code == 120).assertTrue();
				done();
			})
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest5 display.getDefaultDisplay failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_006
	 * @tc.name       Test screenShotSave_Test_006
	 * @tc.desc       Set the screenshot area to 0
	 */
	it('screenShotSave_Test_006', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest6 begin');
		display.getDefaultDisplay().then((dsp) => {
			console.log('screenshotTest screenShotSaveTest6 display.getDefaultDisplay success, dsp :' + JSON.stringify(dsp));
			expect(dsp != null).assertTrue();
			var screenshotOptions = {
				screenRect: {
					left: 0,
					top: 0,
					width: 0,
					height: 0,
				},
				imageSize: {
					width: 300,
					height: 300
				},
				displayId: dsp.id
			};
			screenshot.save(screenshotOptions).then((data) => {
				console.log('screenshotTest screenShotSaveTest6 screenshot.save success, data :' + JSON.stringify(data));
				expect(data != null).assertTrue();
				done();
			}, (err) => {
				console.log('screenshotTest screenShotSaveTest6 screenshot.save failed, err : ' + JSON.stringify(err));
				expect().assertFail();
				done()
			})
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest6 display.getDefaultDisplay failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_007
	 * @tc.name       Test screenShotSave_Test_007
	 * @tc.desc       Set the generated image size to a negative value
	 */
	it('screenShotSave_Test_007', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest7 begin');
		display.getDefaultDisplay().then((dsp) => {
			console.log('screenshotTest screenShotSaveTest7 display.getDefaultDisplay success, dsp :' + JSON.stringify(dsp));
			expect(dsp != null).assertTrue();
			var screenshotOptions = {
				screenRect: {
					left: 0,
					top: 0,
					width: 100,
					height: 100,
				},
				imageSize: {
					width: -300,
					height: -300
				},
				displayId: dsp.id
			};
			screenshot.save(screenshotOptions).then((data) => {
				console.log('screenshotTest screenShotSaveTest7 screenshot.save success, data :' + JSON.stringify(data));
				expect().assertFail();
				done();
			}, (err) => {
				console.log('screenshotTest screenShotSaveTest7 screenshot.save failed, err : ' + JSON.stringify(err));
				expect(err.code == 120).assertTrue();
				done();
			})
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest7 display.getDefaultDisplay failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_008
	 * @tc.name       Test screenShotSave_Test_008
	 * @tc.desc       Set the generated image size to 0
	 */
	it('screenShotSave_Test_008', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest8 begin');
		display.getDefaultDisplay().then((dsp) => {
			console.log('screenshotTest screenShotSaveTest8 display.getDefaultDisplay success, dsp :' + JSON.stringify(dsp));
			expect(dsp != null).assertTrue();
			var screenshotOptions = {
				screenRect: {
					left: 0,
					top: 0,
					width: 100,
					height: 100,
				},
				imageSize: {
					width: 0,
					height: 0
				},
				displayId: dsp.id
			};
			screenshot.save(screenshotOptions).then((data) => {
				console.log('screenshotTest screenShotSaveTest8 screenshot.save success, data :' + JSON.stringify(data));
				expect(data != null).assertTrue();
				done();
			}, (err) => {
				console.log('screenshotTest screenShotSaveTest8 screenshot.save failed, err : ' + JSON.stringify(err));
				expect().assertFail();
				done();
			})
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest8 display.getDefaultDisplay failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_009
	 * @tc.name       Test screenShotSave_Test_009
	 * @tc.desc       Set the screenshot area and the size of the generated image to be equal to the screen width and height
	 */
	it('screenShotSave_Test_009', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest9 begin');
		display.getDefaultDisplay().then((dsp) => {
			console.log('screenshotTest screenShotSaveTest9 display.getDefaultDisplay success, dsp :' + JSON.stringify(dsp));
			expect(dsp != null).assertTrue();
			var screenshotOptions = {
				screenRect: {
					left: 0,
					top: 0,
					width: dsp.width,
					height: dsp.height
				},
				imageSize: {
					width: dsp.width,
					height: dsp.height
				},
				displayId: dsp.id
			};
			screenshot.save(screenshotOptions).then((data) => {
				console.log('screenshotTest screenShotSaveTest9 screenshot.save success, data :' + JSON.stringify(data));
				expect(data != null).assertTrue();
				done();
			}, (err) => {
				console.log('screenshotTest screenShotSaveTest9 screenshot.save failed, err : ' + JSON.stringify(err));
				expect().assertFail();
			})
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest9 display.getDefaultDisplay failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_010
	 * @tc.name       Test screenShotSave_Test_010
	 * @tc.desc       Take a screenshot of a screen that doesn't exist
	 */
	it('screenShotSave_Test_010', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest10 begin');
		var screenshotOptions = {
			screenRect: {
				left: 0,
				top: 0,
				width: 100,
				height: 100,
			},
			imageSize: {
				width: 300,
				height: 300
			},
			displayId: null
		};
		screenshot.save(screenshotOptions).then((data) => {
			console.log('screenshotTest screenShotSaveTest10 screenshot.save success, data :' + JSON.stringify(data));
			expect(data != null).assertTrue()
			done();
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest10 screenshot.save failed, err : ' + JSON.stringify(err));
			expect().assertFail();
			done();
		})
	})

	/**
	 * @tc.number     SUB_SCREENSHOT_SAVE_JSAPI_011
	 * @tc.name       Test screenShotSave_Test_011
	 * @tc.desc       Test screenshot.save API function test11
	 */
	it('screenShotSave_Test_011', 0, function (done) {
		console.log('screenshotTest screenShotSaveTest11 begin');
		var screenshotOptions = {
			screenRect: {
				left: 0,
				top: 0,
				width: 100,
				height: 100,
			},
			imageSize: {
				width: 300,
				height: 300
			},
			displayId: 1000
		};
		screenshot.save(screenshotOptions).then((data) => {
			console.log('screenshotTest screenShotSaveTest11 screenshot.save success, data :' + JSON.stringify(data));
			expect().assertFail();
			done();
		}, (err) => {
			console.log('screenshotTest screenShotSaveTest11 screenshot.save failed, err : ' + JSON.stringify(err));
			expect(err.code == 120).assertTrue();
			done();
		})
	})
})