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
import app from '@system.app'
import Context from '@ohos.napi_context'

import {
	describe,
	beforeAll,
	beforeEach,
	afterEach,
	afterAll,
	it,
	expect
} from 'deccjsunit/index'

describe('webgl1Test', function() {
	console.info('webgltest start');
	var gl;
	var gl2;

	var indices = new Uint16Array([0, 1, 2, 1, 3, 4]);
	var vertices = new Uint16Array([
		-0.5, 0.5, 0.0,
		0.0, 0.5, 0.0,
		-0.25, 0.25, 0.0,
		0.5, 0.5, 0.0,
		0.25, 0.25, 0.0,
	])

	//顶点着色器程序
	var VSHADER_SOURCE =
		"attribute vec4 a_Position;" +
		"void main() {" +
		//设置坐标
		"gl_Position = a_Position; " +
		//    "gl_PointSize = 10.0;" +
		"} ";

	//片元着色器
	var FSHADER_SOURCE =
		"void main() {" +
		//设置颜色
		"gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);" +
		"}";

	function globalFunction() {
		const vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.shaderSource(fragmentShader, FSHADER_SOURCE);
		gl.compileShader(fragmentShader);
		const programObj = gl.createProgram();
		console.info("testUseProgram has failed for " + programObj)
		const useProgramError1 = gl.getError();
		console.info("useProgramError: " + useProgramError1);
		const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
		console.info("testUseProgram has failed for " + renderBufferValue1)
		gl.attachShader(programObj, vertexShader);
		gl.attachShader(programObj, fragmentShader);
		gl.linkProgram(programObj);
		gl.useProgram(programObj);
		return programObj;
	}


	function createProgram(gl) {
		//顶点着色器程序
		var VSHADER_SOURCE =
			'attribute vec4 a_Position;\n' +
			'void main() {\n' +
			'  gl_Position = a_Position;\n' +
			'}\n';

		// 片元着色器程序
		var FSHADER_SOURCE =
			'precision mediump float;\n' +
			'uniform vec4 u_FragColor;\n' +
			'void main() {\n' +
			'  gl_FragColor = u_FragColor;\n' +
			'}\n';
		var vertexShader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(vertexShader, VSHADER_SOURCE);
		gl.compileShader(vertexShader);
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		gl.shaderSource(fragmentShader, FSHADER_SOURCE);
		gl.compileShader(fragmentShader);
		const programObj = gl.createProgram();
		console.log("testUseProgram has failed for " + programObj)
		const useProgramError1 = gl.getError();
		console.info("useProgramError: " + useProgramError1);
		const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
		console.log("testUseProgram has failed for " + renderBufferValue1)
		gl.attachShader(programObj, vertexShader);
		gl.attachShader(programObj, fragmentShader);
		gl.linkProgram(programObj);
		gl.useProgram(programObj);
		return programObj;
	}

	function initShaders(gl, vshader, fshader) {
		var program = createProgramExternal(gl, vshader, fshader);
		console.log("======createProgram program: " + JSON.stringify(program));

		if (!program) {
			console.log('Failed to create program');
			return false;
		}

		gl.useProgram(program);
		gl.program = program;

		return true;
	}

	function createProgramExternal(gl, vshader, fshader) {
		// Create shader object
		var vertexShader = loadShader(gl, gl.VERTEX_SHADER, vshader);
		console.log("======vertexShader: " + vertexShader);
		var fragmentShader = loadShader(gl, gl.FRAGMENT_SHADER, fshader);
		if (!vertexShader || !fragmentShader) {
			return null;
		}

		// Create a program object
		var program = gl.createProgram();
		console.log("======createProgram program: " + JSON.stringify(program));

		if (!program) {
			return null;
		}

		// Attach the shader objects
		gl.attachShader(program, vertexShader);
		gl.attachShader(program, fragmentShader);

		// Link the program object
		gl.linkProgram(program);

		// Check the result of linking
		var linked = gl.getProgramParameter(program, 0x8B82);
		console.log("======getProgramParameter linked: " + linked);

		const getUniformLocationValue = gl.getUniformLocation(program, "a_Position");
		console.log("======getUniformLocation: " + JSON.stringify(getUniformLocationValue));


		if (!linked) {
			var error = gl.getProgramInfoLog(program);
			console.log('Failed to link program: ' + error);
			gl.deleteProgram(program);
			gl.deleteShader(fragmentShader);
			gl.deleteShader(vertexShader);
			return null;
		}

		return program;
	}

	function loadShader(gl, type, source) {
		console.log("======into loadShader====");
		// Create shader object
		var shader = gl.createShader(type);
		if (shader == null) {
			console.log('unable to create shader');
			return null;
		}

		const isShaderValue = gl.isShader(shader);
		console.log('isShader: ' + isShaderValue);

		// Set the shader program
		gl.shaderSource(shader, source);

		// Compile the shader
		gl.compileShader(shader);

		// Check the result of compilation
		var compiled = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
		if (!compiled) {
			var error = gl.getShaderInfoLog(shader);
			console.log('Failed to compile shader: ' + error);
			gl.deleteShader(shader);
			return null;
		}

		var vertex = gl.getShaderParameter(shader, gl.VERTEX_SHADER);
		console.log('getShaderParameter VERTEX_SHADER: ' + vertex);


		return shader;
	}

	function initVertexBuffers(gl) {
		var vertices = new Float32Array([
			0.0, -1.0, -0.5, 0, 0.5, 0
		]);

		var n = 3; // 点的个数

		// 创建缓冲区对象
		var vertexBuffer = gl.createBuffer();
		if (!vertexBuffer) {
			console.log('Failed to create the buffer object');
			return -1;
		}

		// 将缓冲区对象绑定到目标
		gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
		// 向缓冲区对象写入数据
		gl.bufferData(gl.ARRAY_BUFFER, vertices.buffer, gl.STATIC_DRAW);

		var aPosition = gl.getAttribLocation(gl.program, 'a_Position');
		console.info("webgl# getAttribLocation getAttribLocation success:" + JSON.stringify(gl.program));
		console.info("webgl# getAttribLocation getAttribLocation success:" + aPosition);
		if (aPosition < 0) {
			console.log('Failed to get the storage location of a_Position');
			return -1;
		}
		// 将缓冲区对象分配给a_Position变量
		gl.vertexAttribPointer(aPosition, 2, gl.FLOAT, false, 0, 0);

		// 连接a_Position变量与分配给它的缓冲区对象
		gl.enableVertexAttribArray(aPosition);

		return n;
	}


	var float1 = new Float32Array([1.1, 1.2, 1.3, 1.4]);
	var int1 = new Int8Array([1, 1, 1, 1]);
	var uint1 = new Uint8Array([1, 1, 1, 1]);
	var float2 = [1.1, 1.2, 1.3, 1.4];
	var int2 = [1, 1, 1, 1];
	var uint2 = [1, 1, 1, 1];

	function initContext() {
		console.info('initContext start');
		// 获取canvas元素
		const el = global.el;
		const el2 = global.el2;
		// 获取webgl上下文
		gl = el.getContext('webgl');
		if (!gl) {
			console.log('webgltest Failed to get the rendering context for WebGL');
		}
		gl2 = el2.getContext('webgl2');
		if (!gl) {
			console.log('webgltest Failed to get the rendering context for WebGL2');
		}
		console.info('webgltest initContext finish');
	}

	function deleteContext() {
		if (gl != null) {
			gl = null;
			console.info("webgltest gl has null");
		}
		if (gl2 != null) {
			console.info("webgltest gl2 has null");
			gl2 = null;
		}
	}

	/**
	 * run before testClass
	 */
	beforeAll(async function(done) {
		console.info('webgltest beforeAll called');
		initContext();
		done();
	});

	/**
	 * run after testClass
	 */
	afterAll(async function(done) {
		console.info('webgltest afterEach called');
		deleteContext();
		done();
	})

	
	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1201
	 * @tc.name webgl_test_activeTexture26_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture26_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE26);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34010);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1202
	 * @tc.name webgl_test_activeTexture27_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture27_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE27);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34011);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1203
	 * @tc.name webgl_test_activeTexture28_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture28_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE28);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34012);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1204
	 * @tc.name webgl_test_activeTexture29_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture29_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE29);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34013);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1205
	 * @tc.name webgl_test_activeTexture30_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture30_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE30);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34014);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1206
	 * @tc.name webgl_test_activeTexture31_1
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture31_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE31);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(34015);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1207
	 * @tc.name webgl_test_blendEquation_7
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation_7', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl2.MIN);

		const blendEquationValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquation --> getParameter: " + blendEquationValue);
		expect(blendEquationValue).assertEqual(32775);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1208
	 * @tc.name webgl_test_blendEquation_2
	 * @tc.desc Test blendEquation.
	 */
	it('webgl_test_blendEquation_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into blendEquation");

		gl.blendEquation(gl2.MAX);

		const blendEquationValue = gl.getParameter(gl.BLEND_EQUATION_RGB);
		console.info("blendEquation --> getParameter: " + blendEquationValue);
		expect(blendEquationValue).assertEqual(32776);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1210
	 * @tc.name webgl_test_disable_40
	 * @tc.desc Test disable.
	 */
	it('webgl_test_disable_40', 0, async function(done) {
		//initContext();
		console.info("webgltest into disable");

		gl.disable(gl2.RASTERIZER_DISCARD);
		// 要检查功能是否被禁用，请使用以下WebGLRenderingContext.isEnabled()方法：
		const isEnabled = gl.isEnabled(gl.DITHER);
		// false
		console.info("webgltest disable isEnabled: " + isEnabled);
		expect(isEnabled).assertEqual(true);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1211
	 * @tc.name webgl_test_clientWaitSync_2
	 * @tc.desc Test clientWaitSync.
	 */
	it('webgl_test_clientWaitSync_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into clientWaitSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, gl.SYNC_FLUSH_COMMANDS_BIT, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);

		console.info("webgltest fenceSync clientWaitSync: " + status);

		expect(status).assertEqual(0);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1212
	 * @tc.name webgl_test_getSyncParameter_1
	 * @tc.desc Test getSyncParameter.
	 */
	it('webgl_test_getSyncParameter_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSyncParameter");

		console.info("webgltest into isSync");
		let sync = gl2.fenceSync(0x9117, 0);
		let status = gl2.clientWaitSync(sync, 0, 0);
		gl2.waitSync(sync, 0, -1);
		const isSync1 = gl2.isSync(sync);
		console.info("webgltest fenceSync isSync1: " + isSync1);
		expect(isSync1).assertEqual(true);
		const syncParameter = gl2.getSyncParameter(sync, 0x9112);

		console.info("webgltest fenceSync getSyncParameter: " + syncParameter);
		expect(syncParameter).assertEqual(gl2.SYNC_FENCE);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1213
	 * @tc.name testReadPixelsFirst_1
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_SHORT_5_6_5, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1214
	 * @tc.name testReadPixelsFirst_2
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_SHORT_4_4_4_4, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1215
	 * @tc.name testReadPixelsFirst_3
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_SHORT_5_5_5_1, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1216
	 * @tc.name testReadPixelsFirst_4
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.SHORT, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1217
	 * @tc.name testReadPixelsFirst_5
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.INT, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1218
	 * @tc.name testTexImage2D_10_1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_10_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.R8_SNORM, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1219
	 * @tc.name testTexImage2D_11_1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG8_SNORM, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1220
	 * @tc.name testTexImage2D_11_0
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_0', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB8_SNORM, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1221
	 * @tc.name testTexImage2D_11_1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA8_SNORM, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1222
	 * @tc.name testTexImage2D_11_2
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB10_A2UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1223
	 * @tc.name testTexImage2D_11_3
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.SRGB8, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1224
	 * @tc.name testTexImage2D_11_4
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.R32F, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1225
	 * @tc.name testTexImage2D_11_5
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG8I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1226
	 * @tc.name testTexImage2D_11_6
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG16I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1227
	 * @tc.name testTexImage2D_11_7
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG32I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1228
	 * @tc.name testTexImage2D_11_8
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RG32UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1229
	 * @tc.name testTexImage2D_11_9
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB8I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1230
	 * @tc.name testTexImage2D_11_10
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB16I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1231
	 * @tc.name testTexImage2D_11_11
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB16UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1232
	 * @tc.name testTexImage2D_11_12
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB32I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1233
	 * @tc.name testTexImage2D_11_13
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGB32UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1234
	 * @tc.name testTexImage2D_11_14
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_14', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA8I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1235
	 * @tc.name testTexImage2D_11_15
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_15', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA16I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1236
	 * @tc.name testTexImage2D_11_16
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_16', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA16UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1237
	 * @tc.name testTexImage2D_11_17
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_17', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA32I, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1238
	 * @tc.name testTexImage2D_11_18
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_18', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.RGBA32UI, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1239
	 * @tc.name testTexImage2D_11_19
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_11_19', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl2.DEPTH_COMPONENT, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE,
			z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1241
	 * @tc.name testRenderbufferStorageMultisample_2
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.DEPTH_COMPONENT24, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1242
	 * @tc.name testRenderbufferStorageMultisample_3
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.DEPTH_COMPONENT32F, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1243
	 * @tc.name testRenderbufferStorageMultisample_4
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.DEPTH24_STENCIL8, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1244
	 * @tc.name testRenderbufferStorageMultisample_5
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.DEPTH32F_STENCIL8, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1246
	 * @tc.name testTransformFeedbackVaryings_7
	 * @tc.desc Test transformFeedbackVaryings.
	 */
	it('testTransformFeedbackVaryings_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTransformFeedbackVaryings test start ...66');
		console.info('jsWebGL testTransformFeedbackVaryings test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		var transformFeedbackOutputs = ['gl_Position', 'anotherOutput'];
		gl2.transformFeedbackVaryings(program, transformFeedbackOutputs, gl.SEPARATE_ATTRIBS);
		const transformFeedbackVaryingsError = gl.getError();
		console.info("transformFeedbackVaryingsError: " + transformFeedbackVaryingsError);
		expect(transformFeedbackVaryingsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1247
	 * @tc.name testGetVertexAttrib_EnumSize
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttrib_EnumSize', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const bufferVertexAttrib = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_SIZE);
		const isBuffer = gl.isBuffer(bufferVertexAttrib);
		console.info("getVertexAttrib: bufferVertexAttrib" + bufferVertexAttrib);
		expect(isBuffer).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1248
	 * @tc.name testGetVertexAttrib_EnumStride
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttrib_EnumStride', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const bufferVertexAttrib = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_STRIDE);
		const isBuffer = gl.isBuffer(bufferVertexAttrib);
		console.info("getVertexAttrib: bufferVertexAttrib" + bufferVertexAttrib);
		expect(isBuffer).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1249
	 * @tc.name testGetVertexAttrib_EnumType
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttrib_EnumType', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const bufferVertexAttrib = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_TYPE);
		const isBuffer = gl.isBuffer(bufferVertexAttrib);
		console.info("getVertexAttrib: bufferVertexAttrib" + bufferVertexAttrib);
		expect(isBuffer).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1250
	 * @tc.name testGetVertexAttrib_EnumNormalized
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttrib_EnumNormalized', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const bufferVertexAttrib = gl.getVertexAttrib(0, gl.VERTEX_ATTRIB_ARRAY_NORMALIZED);
		const isBuffer = gl.isBuffer(bufferVertexAttrib);
		console.info("getVertexAttrib: bufferVertexAttrib" + bufferVertexAttrib);
		expect(isBuffer).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1251
	 * @tc.name testRenderbufferStorage_DepthComponent16
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorage_DepthComponent16', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.DEPTH_COMPONENT16);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1252
	 * @tc.name testRenderbufferStorage_StencilIndex8
	 * @tc.desc Test renderbufferStorage
	 */
	it('testRenderbufferStorage_StencilIndex8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.STENCIL_INDEX8, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.STENCIL_INDEX8);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1253
	 * @tc.name testStencilOp_KeepReplaceIncrWrap
	 * @tc.desc Test stencilOp
	 */
	it('testStencilOp_KeepReplaceIncrWrap', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOp test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilOp(gl.KEEP, gl.REPLACE, gl.INCR_WRAP);
		const stencilOpParameter = gl.getParameter(gl.STENCIL_FAIL);
		console.info("stencilOp stencilOpParameter: " + stencilOpParameter);
		expect(stencilOpParameter).assertEqual(gl.KEEP);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1254
	 * @tc.name testStencilOp_KeepReplaceDecrWrap
	 * @tc.desc Test stencilOp
	 */
	it('testStencilOp_KeepReplaceDecrWrap', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOp test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilOp(gl.KEEP, gl.REPLACE, gl.DECR_WRAP);
		const stencilOpParameter = gl.getParameter(gl.STENCIL_FAIL);
		console.info("stencilOp stencilOpParameter: " + stencilOpParameter);
		expect(stencilOpParameter).assertEqual(gl.KEEP);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1255
	 * @tc.name testTexParameterf_TextureBaseLevel
	 * @tc.desc Test texParameterf.
	 */
	it('testTexParameterf_TextureBaseLevel', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameterf test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl2.TEXTURE_BASE_LEVEL, gl.LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl2.TEXTURE_BASE_LEVEL);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1256
	 * @tc.name testTexParameterf_TextureMaxLevel
	 * @tc.desc Test texParameterf.
	 */
	it('testTexParameterf_TextureMaxLevel', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameterf test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl2.TEXTURE_MAX_LEVEL, gl.LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_2D, gl2.TEXTURE_MAX_LEVEL);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1257
	 * @tc.name testVertexAttribPointer_Short
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer_Short', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			gl.vertexAttribPointer(0, 3, gl.SHORT, false, 20, 0);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1258
	 * @tc.name testTexStorage3D_Srgb8
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Srgb8', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.SRGB8, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1259
	 * @tc.name testTexStorage3D_Rgba32f
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Rgba32f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGBA32F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1260
	 * @tc.name testTexStorage3D_Rgb32f
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Rgb32f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGB32F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1261
	 * @tc.name testTexStorage3D_Rgba16f
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Rgba16f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGBA16F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1262
	 * @tc.name testTexStorage3D_Rgb16f
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_Rgb16f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGB16F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1263
	 * @tc.name testTexStorage3D_R11FG11FB10F
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_R11FG11FB10F', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.R11F_G11F_B10F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1264
	 * @tc.name testTexStorage3D_RGB9E5
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_RGB9E5', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGB9_E5, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1265
	 * @tc.name testTexStorage3D_RGBA8UI
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_RGBA8UI', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RGBA8UI, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1266
	 * @tc.name testTexStorage3D_R32F
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_R32F', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.R32F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1267
	 * @tc.name testTexStorage3D_RG16F
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_RG16F', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RG16F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1268
	 * @tc.name testTexStorage3D_RG32F
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D_RG32F', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl2.TEXTURE_3D, 1, gl2.RG32F, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1269
	 * @tc.name testTexImage3D_SRGB8ALPHA8
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_SRGB8ALPHA8', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA, gl2
				.UNSIGNED_BYTE, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1270
	 * @tc.name testTexImage3D_HALFFLOAT
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_HALFFLOAT', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA, gl2.HALF_FLOAT,
				0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1271
	 * @tc.name testTexImage3D_UNSIGNED_INT_2_10_10_10_REV
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_UNSIGNED_INT_2_10_10_10_REV', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA,
				gl2.UNSIGNED_INT_2_10_10_10_REV, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1272
	 * @tc.name testTexImage3D_UNSIGNED_INT_10F_11F_11F_REV
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_UNSIGNED_INT_10F_11F_11F_REV', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA,
				gl2.UNSIGNED_INT_10F_11F_11F_REV, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1273
	 * @tc.name testTexImage3D_UNSIGNED_INT_5_9_9_9_REV
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_UNSIGNED_INT_5_9_9_9_REV', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA, gl2
				.UNSIGNED_INT_5_9_9_9_REV, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1274
	 * @tc.name testTexImage3D_UNSIGNED_INT_24_8
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_UNSIGNED_INT_24_8', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl2.SRGB8_ALPHA8, 1, 1, 1, 0, gl.RGBA, gl2
				.UNSIGNED_INT_24_8, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1275
	 * @tc.name testGetParameter_webgl2_45
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_45', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER0);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1276
	 * @tc.name testGetParameter_webgl2_46
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_46', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER1);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1277
	 * @tc.name testGetParameter_webgl2_47
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_47', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER2);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1278
	 * @tc.name testGetParameter_webgl2_48
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_48', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER3);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1279
	 * @tc.name testGetParameter_webgl2_49
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_49', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER4);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1280
	 * @tc.name testGetParameter_webgl2_50
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_50', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER5);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1281
	 * @tc.name testGetParameter_webgl2_51
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_51', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER6);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1282
	 * @tc.name testGetParameter_webgl2_52
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_52', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER8);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1283
	 * @tc.name testGetParameter_webgl2_53
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_53', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER9);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1284
	 * @tc.name testGetParameter_webgl2_54
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_54', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER10);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1285
	 * @tc.name testGetParameter_webgl2_55
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_55', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER11);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1286
	 * @tc.name testGetParameter_webgl2_56
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_56', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER12);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1287
	 * @tc.name testGetParameter_webgl2_57
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_57', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER13);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1288
	 * @tc.name testGetParameter_webgl2_58
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_58', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER14);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1289
	 * @tc.name testGetParameter_webgl2_59
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_59', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER15);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1290
	 * @tc.name testGetParameter_webgl2_60
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_webgl2_60', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl2.DRAW_BUFFER7);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1291
	 * @tc.name testGetFramebufferAttachmentParameter3_1
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1292
	 * @tc.name testGetFramebufferAttachmentParameter3_2
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_BLUE_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1293
	 * @tc.name testGetFramebufferAttachmentParameter3_3
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1294
	 * @tc.name testGetFramebufferAttachmentParameter3_4
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1295
	 * @tc.name testGetFramebufferAttachmentParameter3_5
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1296
	 * @tc.name testGetFramebufferAttachmentParameter3_6
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_GREEN_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1298
	 * @tc.name testGetFramebufferAttachmentParameter3_8
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_RED_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1299
	 * @tc.name testGetFramebufferAttachmentParameter3_9
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl2.FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1300
	 * @tc.name testGetFramebufferAttachmentParameter3_10
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		done();
	});
})
