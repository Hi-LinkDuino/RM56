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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0401
	 * @tc.name testBufferData_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.INVALID_ENUM);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0402
	 * @tc.name testBufferData_02
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.SRC_COLOR);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0403
	 * @tc.name testBufferData_03
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_03', 0, async function(done) {
		console.info('jsWebGL testBufferData_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.SRC_ALPHA);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0404
	 * @tc.name testBufferData_04
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_04 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.SRC_ALPHA);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0405
	 * @tc.name testBufferData_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0406
	 * @tc.name testBufferData_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0407
	 * @tc.name testBufferData_1_02
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.ONE_MINUS_SRC_ALPHA);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0408
	 * @tc.name testBufferData_1_03
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.ONE_MINUS_DST_ALPHA);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0409
	 * @tc.name testBufferData_1_04
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_1_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, -1, gl.INVALID_VALUE);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0410
	 * @tc.name testBufferSubData
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0411
	 * @tc.name testBufferSubData_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.TRIANGLE_FAN);
		gl.bufferSubData(gl.TRIANGLE_FAN, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0412
	 * @tc.name testBufferSubData_2
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_2 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0413
	 * @tc.name testBufferSubData_3
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_3 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.TRIANGLE_STRIP);
		gl.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0414
	 * @tc.name testBufferSubData_4
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_4 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.TRIANGLE_STRIP);
		gl.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0415
	 * @tc.name testCompressedTexImage2D
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0416
	 * @tc.name testCompressedTexImage2D_01
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_01 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_3D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_3D, [0, 0], 0x83F3, 512, 512, -1, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0417
	 * @tc.name testCompressedTexImage2D_02
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_02 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_3D, texture);
		gl.compressedTexImage2D(gl.SRC_COLOR, -1, 0x83F3, -1, [-1, -1], 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0418
	 * @tc.name testCompressedTexImage2D_03
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_03 test start ...66');
		var program = gl.createProgram();
		gl.bindTexture(gl.TEXTURE_3D, program);
		gl.compressedTexImage2D(gl.TEXTURE_3D, 0, [0x83F3, 0x83F3], 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0419
	 * @tc.name testCompressedTexImage2D_04
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_04 test start ...66');
		var framebuffer = gl.createFramebuffer();
		gl.bindTexture(gl.TEXTURE_3D, framebuffer);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x0307, 512, 512, 0, "abc");
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0420
	 * @tc.name testCompressedTexSubImage2D_1
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_2D, 0, 256, 256, 512, 512, 0x83F3, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0421
	 * @tc.name testReadPixels
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight, gl.RGBA, gl
			.UNSIGNED_BYTE, pixels);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0422
	 * @tc.name testReadPixels_01
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_01 test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight, gl.RGBA, gl.DST_COLOR,
			pixels);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0423
	 * @tc.name testReadPixels_02
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_02 test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight, gl.RGBA, gl.DST_COLOR,
			0);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0424
	 * @tc.name testReadPixels_03
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_03 test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight, -1, gl.DST_COLOR, -2);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0425
	 * @tc.name testReadPixels_04
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixels_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testReadPixels_04 test start ...66');
		var pixels = new Uint8Array(gl.drawingBufferWidth * gl.drawingBufferHeight * 4);
		gl.readPixels(-1, -1, gl.drawingBufferWidth, gl.drawingBufferHeight, -1, gl.CONSTANT_COLOR,
			0);
		const readPixelsError = gl.getError();
		console.info("readPixelsError: " + readPixelsError);
		expect(readPixelsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0426
	 * @tc.name testTexImage2D
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D test start ...66');
		console.info('jsWebGL testTexImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0427
	 * @tc.name testTexImage2D_01
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_01 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, [0, 0], gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0428
	 * @tc.name testTexImage2D_02
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_02 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl.CONSTANT_ALPHA, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0429
	 * @tc.name testTexImage2D_03
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_03 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl.FUNC_SUBTRACT, -1);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0430
	 * @tc.name testTexImage2D_04
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_04 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.DST_COLOR, 512, 512, 0, gl.RGB8, gl.CONSTANT_ALPHA, -1);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0431
	 * @tc.name testTexSubImage2D
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D test start ...66');
		console.info('jsWebGL testTexSubImage2D test start ...' + JSON.stringify(gl));
		const buffer = new ArrayBuffer(16);
		const z = new Float32Array(buffer, 0, 4);
		gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT, z, 0);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0432
	 * @tc.name testUniform1fv
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform1fv(uniformlocationObj, flaot32list);
		const uniform1fvError = gl.getError();
		console.info("uniform1fvError: " + uniform1fvError);
		const errorCode = gl.getError();
		expect(uniform1fvError).assertEqual(0);

		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0433
	 * @tc.name testUniform1fv_1
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fv_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1fv_1 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position1");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform1fv(uniformlocationObj, flaot32list);
		const uniform1fvError = gl.getError();
		console.info("uniform1fvError: " + uniform1fvError);
		expect(uniform1fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0435
	 * @tc.name testUniform1fv_3
	 * @tc.desc Test uniform1fv.
	 */
	it('testUniform1fv_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1fv_3 test start ...66');
		const programobject = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programobject, "a_Position1");
		const flaot32list = new Float32Array([-1, -2]);
		gl.uniform1fv(uniformlocationObj, flaot32list);
		const uniform1fvError = gl.getError();
		console.info("uniform1fvError: " + uniform1fvError);
		const errorCode = gl.getError();
		expect(uniform1fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0437
	 * @tc.name testUniform2fv
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "123");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform2fv(uniformlocationObj, flaot32list);
		const uniform2fvError = gl.getError();
		console.info("uniform2fvError: " + uniform2fvError);
		expect(uniform2fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0439
	 * @tc.name testUniform2fv_2
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2fv_2 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "123");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform2fv(uniformlocationObj, flaot32list);
		const uniform2fvError = gl.getError();
		console.info("uniform2fvError: " + uniform2fvError);
		expect(uniform2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0441
	 * @tc.name testUniform2fv_4
	 * @tc.desc Test uniform2fv.
	 */
	it('testUniform2fv_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2fv_4 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position1");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform2fv(uniformlocationObj, flaot32list);
		const uniform2fvError = gl.getError();
		console.info("uniform2fvError1: " + uniform2fvError);
		expect(uniform2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0442
	 * @tc.name testUniform3fv
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform3fv(uniformlocationObj, flaot32list);
		const uniform3fvError = gl.getError();
		console.info("uniform3fvError: " + uniform3fvError);
		expect(uniform3fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0443
	 * @tc.name testUniform3fv_1
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3fv_1 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform3fv(uniformlocationObj, flaot32list);
		const uniform3fvError = gl.getError();
		console.info("uniform3fvError: " + uniform3fvError);
		expect(uniform3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0446
	 * @tc.name testUniform3fv_4
	 * @tc.desc Test uniform3fv.
	 */
	it('testUniform3fv_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3fv_4 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform3fv(uniformlocationObj, flaot32list);
		const uniform3fvError = gl.getError();
		console.info("uniform3fvError: " + uniform3fvError);
		expect(uniform3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0447
	 * @tc.name testUniform4fv
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform4fv(uniformlocationObj, flaot32list);
		const uniform4fvError = gl.getError();
		console.info("uniform4fvError: " + uniform4fvError);
		expect(uniform4fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0448
	 * @tc.name testUniform4fv_01
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4fv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform4fv(uniformlocationObj, flaot32list);
		const uniform4fvError = gl.getError();
		console.info("uniform4fvError: " + uniform4fvError);
		expect(uniform4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0451
	 * @tc.name testUniform4fv_04
	 * @tc.desc Test uniform4fv.
	 */
	it('testUniform4fv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4fv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniform4fv(uniformlocationObj, flaot32list);
		const uniform4fvError = gl.getError();
		console.info("uniform4fvError: " + uniform4fvError);
		expect(uniform4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0452
	 * @tc.name testUniform1iv
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1iv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform1iv(uniformlocationObj, int32list);
		const uniform1ivError = gl.getError();
		console.info("uniform1ivError: " + uniform1ivError);
		expect(uniform1ivError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0453
	 * @tc.name testUniform1iv_01
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform1iv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform1iv(uniformlocationObj, int32list);
		const uniform1ivError = gl.getError();
		console.info("uniform1ivError: " + uniform1ivError);
		expect(uniform1ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0457
	 * @tc.name testUniform2iv
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2iv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform2iv(uniformlocationObj, int32list);
		const uniform2ivError = gl.getError();
		console.info("testUniform2ivError: " + uniform2ivError);
		expect(uniform2ivError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0458
	 * @tc.name testUniform2iv_01
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2iv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform2iv(uniformlocationObj, int32list);
		const uniform2ivError = gl.getError();
		console.info("testUniform2ivError: " + uniform2ivError);
		expect(uniform2ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0461
	 * @tc.name testUniform2iv_04
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform2iv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform2iv(uniformlocationObj, int32list);
		const uniform2ivError = gl.getError();
		console.info("testUniform2ivError: " + uniform2ivError);
		expect(uniform2ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0462
	 * @tc.name testUniform3iv
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3iv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform3iv(uniformlocationObj, int32list);
		const uniform3ivError = gl.getError();
		console.info("testUniform3ivError: " + uniform3ivError);
		expect(uniform3ivError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0463
	 * @tc.name testUniform3iv_01
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3iv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform3iv(uniformlocationObj, int32list);
		const uniform3ivError = gl.getError();
		console.info("testUniform3ivError: " + uniform3ivError);
		expect(uniform3ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0466
	 * @tc.name testUniform3iv_04
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform3iv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform3iv(uniformlocationObj, int32list);
		const uniform3ivError = gl.getError();
		console.info("testUniform3ivError: " + uniform3ivError);
		expect(uniform3ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0467
	 * @tc.name testUniform4iv
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0468
	 * @tc.name testUniform4iv_01
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0469
	 * @tc.name testUniform4iv_02
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv_02 test start ...66');
		const buffer = gl.createBuffer();
		const uniformlocationObj = gl.getUniformLocation(buffer, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0470
	 * @tc.name testUniform4iv_03
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv_03 test start ...66');
		const buffer = gl.createBuffer();
		const uniformlocationObj = gl.getUniformLocation(buffer, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0471
	 * @tc.name testUniform4iv_04
	 * @tc.desc Test Uniform4uiv.
	 */
	it('testUniform4iv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniform4iv_03 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const int32list = new Int32Array([1, 2]);
		gl.uniform4iv(uniformlocationObj, int32list);
		const uniform4ivError = gl.getError();
		console.info("testUniform4ivError: " + uniform4ivError);
		expect(uniform4ivError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0472
	 * @tc.name testUniformMatrix2fv
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix2fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix2fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix2fvError = gl.getError();
		console.info("uniformMatrix2fvError: " + uniformMatrix2fvError);
		expect(uniformMatrix2fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0473
	 * @tc.name testUniformMatrix2fv_01
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix2fv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix2fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix2fvError = gl.getError();
		console.info("uniformMatrix2fvError: " + uniformMatrix2fvError);
		expect(uniformMatrix2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0475
	 * @tc.name testUniformMatrix2fv_03
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix2fv_03 test start ...66');
		const buffer = gl.createBuffer();
		const uniformlocationObj = gl.getUniformLocation(buffer, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix2fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix2fvError = gl.getError();
		console.info("uniformMatrix2fvError: " + uniformMatrix2fvError);
		expect(uniformMatrix2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0476
	 * @tc.name testUniformMatrix2fv_04
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix2fv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix2fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix2fvError = gl.getError();
		console.info("uniformMatrix2fvError: " + uniformMatrix2fvError);
		expect(uniformMatrix2fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0477
	 * @tc.name testUniformMatrix3fv
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0478
	 * @tc.name testUniformMatrix3fv_01
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0479
	 * @tc.name testUniformMatrix3fv_02
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv_02 test start ...66');
		const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		const uniformlocationObj = gl.getUniformLocation(fragmentShader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0480
	 * @tc.name testUniformMatrix3fv_03
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv_03 test start ...66');
		const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		const uniformlocationObj = gl.getUniformLocation(fragmentShader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0481
	 * @tc.name testUniformMatrix3fv_04
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix3fv_04 test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		gl.uniformMatrix3fv(uniformlocationObj, true, flaot32list);
		const uniformMatrix3fvError = gl.getError();
		console.info("uniformMatrix3fvError: " + uniformMatrix3fvError);
		expect(uniformMatrix3fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0482
	 * @tc.name testUniformMatrix4fv
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv test start ...66');
		const programObj = createProgram(gl);
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0483
	 * @tc.name testUniformMatrix4fv_01
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv_01 test start ...66');
		const programObj = gl.createProgram();
		const uniformlocationObj = gl.getUniformLocation(programObj, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0484
	 * @tc.name testUniformMatrix4fv_02
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv_02 test start ...66');
		const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		const uniformlocationObj = gl.getUniformLocation(fragmentShader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0485
	 * @tc.name testUniformMatrix4fv_03
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv_03 test start ...66');
		var fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
		const uniformlocationObj = gl.getUniformLocation(fragmentShader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0486
	 * @tc.name testUniformMatrix4fv_04
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testUniformMatrix4fv_04 test start ...66');
		const shader = gl.createShader(gl.VERTEX_SHADER);
		const uniformlocationObj = gl.getUniformLocation(shader, "a_Position");
		const flaot32list = new Float32Array([1, 2]);
		const a = gl.uniformMatrix4fv(uniformlocationObj, true, flaot32list);
		console.info("a: " + a);
		const uniformMatrix4fvError = gl.getError();
		console.info("uniformMatrix4fvError: " + uniformMatrix4fvError);
		expect(uniformMatrix4fvError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0487
	 * @tc.name testCopyBufferSubData
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData test start ...66');
		console.info('jsWebGL testCopyBufferSubData test start ...' + JSON.stringify(gl));
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.COPY_READ_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0488
	 * @tc.name testCopyBufferSubData_01
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_01 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);

		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.ONE_MINUS_CONSTANT_ALPHA, gl.ONE_MINUS_CONSTANT_ALPHA, 0, 0, 0);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0489
	 * @tc.name testCopyBufferSubData_02
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_02 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);

		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.SRC_ALPHA_SATURATE, gl.SRC_ALPHA_SATURATE, 0, 0, 0);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0490
	 * @tc.name testCopyBufferSubData_03
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_03 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.SRC_ALPHA_SATURATE, gl.SRC_ALPHA_SATURATE, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0491
	 * @tc.name testCopyBufferSubData_04
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_04 test start ...66');
		const srcBuffer = gl.createBuffer();
		const dstBuffer = gl.createBuffer();
		const vertices = [1, 2];
		const data = new Float32Array(vertices);
		const length = vertices.length * 4;
		gl.bindBuffer(gl.ARRAY_BUFFER, srcBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
		gl.bindBuffer(gl.COPY_READ_BUFFER, srcBuffer);
		gl.bindBuffer(gl.ARRAY_BUFFER, dstBuffer);
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(length), gl.STATIC_DRAW);
		gl2.copyBufferSubData(gl.SRC_ALPHA_SATURATE, gl.SRC_ALPHA_SATURATE, 0, 0, gl
			.ONE_MINUS_CONSTANT_COLOR);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0492
	 * @tc.name testGetBufferSubData
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.ARRAY_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0493
	 * @tc.name testGetBufferSubData_01
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.SRC_ALPHA_SATURATE, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0494
	 * @tc.name testGetBufferSubData_02
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.FUNC_SUBTRACT, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0495
	 * @tc.name testGetBufferSubData_03
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.BLEND_DST_RGB, 0, new Int32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0496
	 * @tc.name testGetBufferSubData_04
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_04 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.BLEND_SRC_ALPHA);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.ARRAY_BUFFER, false, new Int32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0497
	 * @tc.name testBlitFramebuffer
	 * @tc.desc Test blitFramebuffer.
	 */
	it('testBlitFramebuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBlitFramebuffer test start ...66');
		console.info('jsWebGL testBlitFramebuffer test start ...' + JSON.stringify(gl));
		gl2.blitFramebuffer(0, 0, 400, 1080, 0, 0, 400, 1080, gl2.COLOR_BUFFER_BIT, gl2.NEAREST);
		const blitFramebufferError = gl.getError();
		console.info("blitFramebufferError: " + blitFramebufferError);
		expect(blitFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0498
	 * @tc.name testFramebufferTextureLayer
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer test start ...66');
		console.info('jsWebGL testFramebufferTextureLayer test start ...' + JSON.stringify(gl));
		var texture = gl.createTexture();
		gl2.framebufferTextureLayer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0,
			texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0499
	 * @tc.name testInvalidateFramebuffer
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer test start ...66');
		console.info('jsWebGL testInvalidateFramebuffer test start ...' + JSON.stringify(gl));
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0500
	 * @tc.name testInvalidateSubFramebuffer
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer test start ...66');
		console.info('jsWebGL testInvalidateSubFramebuffer test start ...' + JSON.stringify(gl));
		gl2.invalidateSubFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})
})
