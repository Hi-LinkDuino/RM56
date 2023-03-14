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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0901
	 * @tc.name testTexImage2D_1_04
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_04 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0902
	 * @tc.name testTexImage2D_1_05
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_05 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0903
	 * @tc.name testTexImage2D_1_06
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_06 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0904
	 * @tc.name testTexImage2D_1_07
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_07 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0911
	 * @tc.name testTexSubImage2D_1_01
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_01 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0912
	 * @tc.name testTexSubImage2D_1_02
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_02 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0913
	 * @tc.name testTexSubImage2D_1_03
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_03 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0914
	 * @tc.name testTexSubImage2D_1_04
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_04 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0915
	 * @tc.name testTexSubImage2D_1_05
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_05 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0916
	 * @tc.name testTexSubImage2D_1_06
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexSubImage2D_1_06 test start ...66');
		const buf = new ArrayBuffer(8);
		const view = new DataView(buf, 0);
		view[0] = 2;
		gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, 512, 512, -gl.RED, -gl.FLOAT,
			view);
		const ttexSubImage2DError = gl.getError();
		console.info("ttexSubImage2DError: " + ttexSubImage2DError);
		expect(ttexSubImage2DError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0924
	 * @tc.name testCopyBufferSubData_1_02
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_02 test start ...66');
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0925
	 * @tc.name testCopyBufferSubData_1_03
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_03 test start ...66');
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
		gl2.copyBufferSubData(gl.COPY_WRITE_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0926
	 * @tc.name testCopyBufferSubData_1_04
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_04 test start ...66');
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
		gl2.copyBufferSubData(gl.TRANSFORM_FEEDBACK_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0927
	 * @tc.name testCopyBufferSubData_1_05
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_05 test start ...66');
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
		gl2.copyBufferSubData(gl.UNIFORM_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0928
	 * @tc.name testCopyBufferSubData_1_06
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_06 test start ...66');
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
		gl2.copyBufferSubData(gl.PIXEL_PACK_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0929
	 * @tc.name testCopyBufferSubData_1_07
	 * @tc.desc Test copyBufferSubData.
	 */
	it('testCopyBufferSubData_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCopyBufferSubData_1_07 test start ...66');
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
		gl2.copyBufferSubData(gl.PIXEL_UNPACK_BUFFER, gl.ARRAY_BUFFER, 0, 0, length);
		const copyBufferSubDataError = gl.getError();
		console.info("copyBufferSubDataError: " + copyBufferSubDataError);
		expect(copyBufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0931
	 * @tc.name testGetBufferSubData_1_02
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.COPY_READ_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0932
	 * @tc.name testGetBufferSubData_1_03
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.COPY_WRITE_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0933
	 * @tc.name testGetBufferSubData_1_04
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_04 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.TRANSFORM_FEEDBACK_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0934
	 * @tc.name testGetBufferSubData_1_05
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_05 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.UNIFORM_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0935
	 * @tc.name testGetBufferSubData_1_06
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_06 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.PIXEL_PACK_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0936
	 * @tc.name testGetBufferSubData_1_07
	 * @tc.desc Test getBufferSubData.
	 */
	it('testGetBufferSubData_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetBufferSubData_1_07 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		const vertices = [1, 2];
		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
		var arrBuffer = new ArrayBuffer(vertices.length * Float32Array.BYTES_PER_ELEMENT);
		gl2.getBufferSubData(gl.PIXEL_UNPACK_BUFFER, 0, new Float32Array(arrBuffer), 0, 0);
		const getBufferSubDataError = gl.getError();
		console.info("getBufferSubDataError: " + getBufferSubDataError);
		expect(getBufferSubDataError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0937
	 * @tc.name testFramebufferTextureLayer_1_01
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_01 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.DRAW_FRAMEBUFFER, gl2.COLOR_ATTACHMENT0, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0938
	 * @tc.name testFramebufferTextureLayer_1_02
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_02 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.READ_FRAMEBUFFER, gl2.COLOR_ATTACHMENT0, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0939
	 * @tc.name testFramebufferTextureLayer_1_03
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_03 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.FRAMEBUFFER, gl2.DEPTH_ATTACHMENT, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0940
	 * @tc.name testFramebufferTextureLayer_1_04
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_04 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.FRAMEBUFFER, gl2.STENCIL_ATTACHMENT, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0941
	 * @tc.name testFramebufferTextureLayer_1_05
	 * @tc.desc Test framebufferTextureLayer.
	 */
	it('testFramebufferTextureLayer_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testFramebufferTextureLayer_1_05 test start ...66');
		const texture = gl.createTexture();
		gl.bindTexture(gl2.TEXTURE_2D_ARRAY, texture);
		gl2.texStorage3D(gl2.TEXTURE_2D_ARRAY, 1, gl2.RGBA8, 256, 256, 8);
		gl.texParameteri(gl2.TEXTURE_CUBE_MAP, gl2.TEXTURE_MAG_FILTER, gl2.NEAREST);
		const framebuffer = gl.createFramebuffer();
		gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
		gl2.framebufferTextureLayer(gl.FRAMEBUFFER, gl2.DEPTH_STENCIL_ATTACHMENT, texture, 0, 8)
		const framebufferTextureLayerError = gl.getError();
		console.info("framebufferTextureLayerError: " + framebufferTextureLayerError);
		expect(framebufferTextureLayerError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0942
	 * @tc.name testInvalidateFramebuffer_1_01
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_01 test start ...66');
		gl2.invalidateFramebuffer(gl.DRAW_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0943
	 * @tc.name testInvalidateFramebuffer_1_02
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_02 test start ...66');
		gl2.invalidateFramebuffer(gl.FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0944
	 * @tc.name testInvalidateFramebuffer_1_03
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_03 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.DEPTH_ATTACHMENT]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0945
	 * @tc.name testInvalidateFramebuffer_1_04
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_04 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.STENCIL_ATTACHMENT, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0946
	 * @tc.name testInvalidateFramebuffer_1_05
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_05 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.DEPTH_STENCIL_ATTACHMENT, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0947
	 * @tc.name testInvalidateFramebuffer_1_06
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_06 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.DEPTH_ATTACHMENT, gl.STENCIL_ATTACHMENT]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0948
	 * @tc.name testInvalidateFramebuffer_1_01
	 * @tc.desc Test invalidateFramebuffer.
	 */
	it('testInvalidateFramebuffer_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateFramebuffer_1_01 test start ...66');
		gl2.invalidateFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1]);
		const invalidateFramebufferError = gl.getError();
		console.info("invalidateFramebufferError: " + invalidateFramebufferError);
		expect(invalidateFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0949
	 * @tc.name testInvalidateSubFramebuffer_1_01
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_01 test start ...66');
		gl2.invalidateSubFramebuffer(gl.FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0950
	 * @tc.name testInvalidateSubFramebuffer_1_02
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_02 test start ...66');
		gl2.invalidateSubFramebuffer(gl.DRAW_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.COLOR_ATTACHMENT1],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0951
	 * @tc.name testInvalidateSubFramebuffer_1_03
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_03 test start ...66');
		gl2.invalidateSubFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.DEPTH_ATTACHMENT, gl.COLOR_ATTACHMENT1],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0952
	 * @tc.name testInvalidateSubFramebuffer_1_04
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_04 test start ...66');
		gl2.invalidateSubFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.COLOR_ATTACHMENT0, gl.STENCIL_ATTACHMENT],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0953
	 * @tc.name testInvalidateSubFramebuffer_1_05
	 * @tc.desc Test invalidateSubFramebuffer.
	 */
	it('testInvalidateSubFramebuffer_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testInvalidateSubFramebuffer_1_05 test start ...66');
		gl2.invalidateSubFramebuffer(gl.READ_FRAMEBUFFER,
			[gl.DEPTH_STENCIL_ATTACHMENT, gl.STENCIL_ATTACHMENT],
			0, 0, 256, 256);
		const invalidateSubFramebufferError = gl.getError();
		console.info("invalidateSubFramebufferError: " + invalidateSubFramebufferError);
		expect(invalidateSubFramebufferError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0954
	 * @tc.name testRenderbufferStorageMultisample_1_01
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_01 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R8, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0955
	 * @tc.name testRenderbufferStorageMultisample_1_02
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_02 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R8UI, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0956
	 * @tc.name testRenderbufferStorageMultisample_1_03
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_03 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R8I, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0957
	 * @tc.name testRenderbufferStorageMultisample_1_04
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_04 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R16UI, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0958
	 * @tc.name testRenderbufferStorageMultisample_1_05
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_05 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R16I, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0959
	 * @tc.name testRenderbufferStorageMultisample_1_06
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_06 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R32UI, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0960
	 * @tc.name testRenderbufferStorageMultisample_1_07
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_07 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.R32I, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0961
	 * @tc.name testRenderbufferStorageMultisample_1_08
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample_1_08', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample_1_08 test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.RG8, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0962
	 * @tc.name testTexStorage2D_1_01
	 * @tc.desc Test texStorage2D.
	 */
	it('testTexStorage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexStorage2D_1_01 test start ...66');
		gl2.texStorage2D(gl.TEXTURE_CUBE_MAP, 1, gl.RGB8, 256, 256);
		const texStorage2DError = gl.getError();
		console.info("texStorage2DError: " + texStorage2DError);
		expect(texStorage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0963
	 * @tc.name testBindBufferBase_1_01
	 * @tc.desc Test bindBufferBase.
	 */
	it('testBindBufferBase_1_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testBindBufferBase_1_01 test start ...66');
			let buffer = gl.createBuffer();
			gl2.bindBufferBase(gl.UNIFORM_BUFFER, 0, buffer);
			const bindBufferBaseError = gl.getError();
			console.info("bindBufferBaseError: " + bindBufferBaseError);
			expect(bindBufferBaseError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferBaseError has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0964
	 * @tc.name bindBufferRange_1_01
	 * @tc.desc Test bindBufferRange.
	 */
	it('bindBufferRange_1_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL bindBufferRange_1_01 test start ...66');
			const buffer = gl.createBuffer();
			gl2.bindBufferRange(gl.UNIFORM_BUFFER, 1, buffer, 0, 4);
			const bindBufferRangeError = gl.getError();
			console.info("bindBufferRangeError: " + bindBufferRangeError);
			expect(bindBufferRangeError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("bindBufferRange has failed for " + e)
			expect(null).assertFail()
		}
	})



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0965
	 * @tc.name testVertexAttrib2f2
	 * @tc.desc Test vertexAttrib2f.
	 */
	it('testVertexAttrib2f2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2f test start ...66');
			gl.vertexAttrib2f(1, 10.0, 10.0);
			const vertexAttrib2fError = gl.getError();
			console.info("vertexAttrib2fError: " + vertexAttrib2fError);
			expect(vertexAttrib2fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib2f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0966
	 * @tc.name testVertexAttrib3f2
	 * @tc.desc Test vertexAttrib3f.
	 */
	it('testVertexAttrib3f2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testvertexAttrib3f test start ...66');
			console.info('jsWebGL testvertexAttrib3f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib3f(1, 10.0, 10.0, 10.0);
			const vertexAttrib3fError = gl.getError();
			console.info("vertexAttrib3fError: " + vertexAttrib3fError);
			expect(vertexAttrib3fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib3f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0967
	 * @tc.name testVertexAttrib4f2
	 * @tc.desc Test vertexAttrib4f.
	 */
	it('testVertexAttrib4f2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4f test start ...66');
			console.info('jsWebGL testVertexAttrib4f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib4f(1, 10.0, 10.0, 10.0, 10.0);
			const vertexAttrib4fError = gl.getError();
			console.info("vertexAttrib4fError: " + vertexAttrib4fError);
			expect(vertexAttrib4fError).assertEqual(gl.NO_ERROR);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testvertexAttrib4f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0968
	 * @tc.name testVertexAttrib1f2
	 * @tc.desc Test vertexAttrib1f.
	 */
	it('testVertexAttrib1f2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib1f test start ...66');
			console.info('jsWebGL testVertexAttrib1f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib1f(-1, 10.0);
			const vertexAttrib1fError = gl.getError();
			console.info("vertexAttrib1fError: " + vertexAttrib1fError);
			expect(vertexAttrib1fError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib1f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0969
	 * @tc.name testVertexAttrib2f3
	 * @tc.desc Test vertexAttrib2f.
	 */
	it('testVertexAttrib2f3', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2f test start ...66');
			gl.vertexAttrib2f(-1, 10.0, 10.0);
			const vertexAttrib2fError = gl.getError();
			console.info("vertexAttrib2fError: " + vertexAttrib2fError);
			expect(vertexAttrib2fError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib2f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0970
	 * @tc.name testVertexAttrib3f3
	 * @tc.desc Test vertexAttrib3f.
	 */
	it('testVertexAttrib3f3', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testvertexAttrib3f test start ...66');
			console.info('jsWebGL testvertexAttrib3f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib3f(-1, 10.0, 10.0, 10.0);
			const vertexAttrib3fError = gl.getError();
			console.info("vertexAttrib3fError: " + vertexAttrib3fError);
			expect(vertexAttrib3fError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib3f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0971
	 * @tc.name testVertexAttrib4f22
	 * @tc.desc Test vertexAttrib4f.
	 */
	it('testVertexAttrib4f22', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4f test start ...66');
			console.info('jsWebGL testVertexAttrib4f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib4f(-1, 10.0, 10.0, 10.0, 10.0);
			const vertexAttrib4fError = gl.getError();
			console.info("vertexAttrib4fError: " + vertexAttrib4fError);
			expect(vertexAttrib4fError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testvertexAttrib4f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0972
	 * @tc.name testVertexAttribPointer2
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			gl.vertexAttribPointer(1, 4, gl.BYTE, true, 20, 12);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0973
	 * @tc.name testVertexAttribPointer3
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer3', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			gl.vertexAttribPointer(2, 2, gl.UNSIGNED_SHORT, true, 20, 16);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0974
	 * @tc.name testVertexAttribPointer4
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer4', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			console.info("webgltest into enableVertexAttribArray");
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
			const locPosition = gl.getAttribLocation(programObj, 'position');
			gl.vertexAttribPointer(locPosition, 3, gl.FLOAT, false, 20, 0);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0975
	 * @tc.name testVertexAttribPointer5
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer5', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			console.info("webgltest into enableVertexAttribArray");
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
			const locNormal = gl.getAttribLocation(programObj, 'normal');
			gl.vertexAttribPointer(locNormal, 4, gl.BYTE, true, 20, 12);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0976
	 * @tc.name testVertexAttribPointer6
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer6', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
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
			const locTexUV = gl.getAttribLocation(programObj, 'texUV');
			gl.vertexAttribPointer(locTexUV, 2, gl.UNSIGNED_SHORT, true, 20, 16);
			const vertexAttribPointerError = gl.getError();
			console.info("vertexAttribPointerError: " + vertexAttribPointerError);
			expect(vertexAttribPointerError).assertEqual(gl.INVALID_VALUE);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttribPointer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0977
	 * @tc.name testTexImage3D2
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D2', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.R8, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, 0);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0978
	 * @tc.name testTexImage3D3
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D3', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.LUMINANCE, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0979
	 * @tc.name testTexImage3D4
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D4', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.RGB8UI, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, 0);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0980
	 * @tc.name testTexSubImage3D
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D test start ...66');
			console.info('jsWebGL testTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.RGB8UI, gl.UNSIGNED_BYTE,
				1);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0981
	 * @tc.name testTexSubImage3D
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D test start ...66');
			console.info('jsWebGL testTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.R8, gl.UNSIGNED_BYTE, 1);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0982
	 * @tc.name testTexSubImage3D
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D test start ...66');
			console.info('jsWebGL testTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.LUMINANCE, gl
				.UNSIGNED_BYTE, 1);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0983
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 0, buffer);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0984
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 0, buffer, 0, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0985
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 0, gl2.PIXEL_UNPACK_BUFFER,
				0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0986
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, false, 0x83F0, 512, 512, 0, buffer);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0987
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, false, 0x83F0, 512, 512, 0, buffer, 0, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0988
	 * @tc.name testCompressedTexImage3D
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D test start ...66');
			console.info('jsWebGL testCompressedTexImage3D test start ...' + JSON.stringify(gl));
			gl2.compressedTexImage3D(gl.TEXTURE_3D, false, 0x83F0, 512, 512, 0, gl2
				.PIXEL_UNPACK_BUFFER, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0989
	 * @tc.name testFrontFace2
	 * @tc.desc Test frontFace.
	 */
	it('testFrontFace2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL frontFace test start ...' + JSON.stringify(gl));
		gl.enable(gl.CULL_FACE);
		gl.cullFace(gl.FRONT_AND_BACK);
		gl.frontFace(gl.CCW);
		const frontFaceParameter = gl.getParameter(gl.CULL_FACE_MODE);
		console.info('frontFace -> frontFaceParameter' + frontFaceParameter);
		gl.disable(gl.CULL_FACE);
		expect(frontFaceParameter).assertEqual(gl.FRONT_AND_BACK);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0990
	 * @tc.name testGenerateMipmap2
	 * @tc.desc Test generateMipmap.
	 */
	it('testGenerateMipmap2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL generateMipmap test start ...' + JSON.stringify(gl));
		gl.generateMipmap(gl2.TEXTURE_CUBE_MAP);
		const error = gl.getError();
		console.info('generateMipmap error ' + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0993
	 * @tc.name testGetFramebufferAttachmentParameter2
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0994
	 * @tc.name testGetFramebufferAttachmentParameter3
	 * @tc.desc Test getFramebufferAttachmentParameter.
	 */
	it('testGetFramebufferAttachmentParameter3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.RENDERBUFFER,
			renderBuffer);
		const framebuffer = gl.getFramebufferAttachmentParameter(gl.FRAMEBUFFER, gl
			.DEPTH_STENCIL_ATTACHMENT,
			gl.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
		console.info('Framebuffer' + framebuffer);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0995
	 * @tc.name testGetShaderPrecisionFormat1
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl.LOW_FLOAT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0996
	 * @tc.name testGetShaderPrecisionFormat2
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl
			.HIGH_FLOAT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0997
	 * @tc.name testGetShaderPrecisionFormat3
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl.LOW_INT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0998
	 * @tc.name testGetShaderPrecisionFormat4
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.FRAGMENT_SHADER, gl
			.MEDIUM_INT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0999
	 * @tc.name testGetTexParameter3
	 * @tc.desc Test getTexParameter.
	 */
	it('testGetTexParameter3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getTexParameter test start ...' + JSON.stringify(gl));
		gl.texParameterf(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		const texParameterValue = gl.getTexParameter(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MAG_FILTER);
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(gl.LINEAR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1000
	 * @tc.name testGetActiveUniformBlockParameter1
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter(program, blockIndex,
			gl.UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getActiveUniformBlockParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});
})
