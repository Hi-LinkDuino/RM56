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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1001
	 * @tc.name testBufferDataFirst
	 * @tc.desc Test bufferData.
	 */
	it('testBufferDataFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.ELEMENT_ARRAY_BUFFER, 1024, gl2.DYNAMIC_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1002
	 * @tc.name testBufferData__2
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.ELEMENT_ARRAY_BUFFER, 1024, gl2.DYNAMIC_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1003
	 * @tc.name testBufferData__3
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.COPY_READ_BUFFER, 1024, gl2.STREAM_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1004
	 * @tc.name testBufferData__4
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData__4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.COPY_READ_BUFFER, 1024, gl2.STREAM_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1005
	 * @tc.name testBufferData__3
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl2.COPY_READ_BUFFER, 1024, gl2.STREAM_DRAW);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1006
	 * @tc.name testBufferSubData__1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl2.ELEMENT_ARRAY_BUFFER, 512, new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1007
	 * @tc.name testBufferSubData__2
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl.ARRAY_BUFFER, 512, new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1008
	 * @tc.name testBufferSubData__3
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl.COPY_WRITE_BUFFER, 512, new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1009
	 * @tc.name testTexImage2D__1
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 32, 512, 512, 0, 32, 32, view);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1010
	 * @tc.name testTexImage2D__2
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 32, 512, 512, 0, 32, 32, view);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1011
	 * @tc.name testTexImage2D__3
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 32, 512, 512, 0, 32, 32, view);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1012
	 * @tc.name testTexSubImage2D__1
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT,
			view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1013
	 * @tc.name testTexSubImage2D__2
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT,
			view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1014
	 * @tc.name testTexSubImage2D__3
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT,
			view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1015
	 * @tc.name testCompressedTexImage2D__1
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0x83F3, 512, 512, 0, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1016
	 * @tc.name testCompressedTexImage2D__2
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0x83F3, 512, 512, 0, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1017
	 * @tc.name testCompressedTexImage2D__3
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0x83F3, 512, 512, 0, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1018
	 * @tc.name testCompressedTexSubImage2D__1
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D__1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1019
	 * @tc.name testCompressedTexSubImage2D__2
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D__2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1020
	 * @tc.name testCompressedTexSubImage2D__3
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D__3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(gl2.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1021
	 * @tc.name testHint1
	 * @tc.desc Test hint.
	 */
	it('testHint1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL hint test start ...' + JSON.stringify(gl));
		gl.hint(gl.GENERATE_MIPMAP_HINT, gl.NICEST);
		gl.generateMipmap(gl2.TEXTURE_2D);
		const error = gl.getError();
		console.info('error' + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1022
	 * @tc.name testHint2
	 * @tc.desc Test hint.
	 */
	it('testHint2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL hint test start ...' + JSON.stringify(gl));
		gl.hint(gl.GENERATE_MIPMAP_HINT, gl.DONT_CARE);
		gl.generateMipmap(gl2.TEXTURE_2D);
		const error = gl.getError();
		console.info('error' + error);
		expect(error).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1023
	 * @tc.name testIsEnabled1
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabled1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));

		gl.enable(gl.CULL_FACE);
		const isEnabledValue1 = gl.isEnabled(gl.CULL_FACE);
		console.info("isEnabledValue1: " + isEnabledValue1);
		expect(isEnabledValue1).assertEqual(true);

		// false
		// 启用或说明给定的能力项，使用WebGLRenderingContext.enable()方法和WebGLRenderingContext.disable()方法：
		gl.disable(gl.CULL_FACE);
		const isEnabledValue2 = gl.isEnabled(gl.CULL_FACE);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1024
	 * @tc.name testIsEnabled2
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabled2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));

		gl.enable(gl.BLEND);
		const isEnabledValue1 = gl.isEnabled(gl.BLEND);
		console.info("isEnabledValue1: " + isEnabledValue1);
		expect(isEnabledValue1).assertEqual(true);

		// false
		// 启用或说明给定的能力项，使用WebGLRenderingContext.enable()方法和WebGLRenderingContext.disable()方法：
		gl.disable(gl.BLEND);
		const isEnabledValue2 = gl.isEnabled(gl.BLEND);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1025
	 * @tc.name testIsEnabled3
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabled3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));

		gl.enable(gl.POLYGON_OFFSET_FILL);
		const isEnabledValue1 = gl.isEnabled(gl.POLYGON_OFFSET_FILL);
		console.info("isEnabledValue1: " + isEnabledValue1);
		expect(isEnabledValue1).assertEqual(true);

		// false
		// 启用或说明给定的能力项，使用WebGLRenderingContext.enable()方法和WebGLRenderingContext.disable()方法：
		gl.disable(gl.POLYGON_OFFSET_FILL);
		const isEnabledValue2 = gl.isEnabled(gl.POLYGON_OFFSET_FILL);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1026
	 * @tc.name testIsEnabled4
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabled4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));

		gl.enable(gl.SAMPLE_COVERAGE);
		const isEnabledValue1 = gl.isEnabled(gl.SAMPLE_COVERAGE);
		console.info("isEnabledValue1: " + isEnabledValue1);
		expect(isEnabledValue1).assertEqual(true);

		// false
		// 启用或说明给定的能力项，使用WebGLRenderingContext.enable()方法和WebGLRenderingContext.disable()方法：
		gl.disable(gl.SAMPLE_COVERAGE);
		const isEnabledValue2 = gl.isEnabled(gl.SAMPLE_COVERAGE);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1027
	 * @tc.name testRenderbufferStorage1
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorage1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGB565, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.RGB565);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1028
	 * @tc.name testRenderbufferStorage2
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorage2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGB5_A1, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.RGB5_A1);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1029
	 * @tc.name testRenderbufferStorage3
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorage3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_STENCIL, 256, 256);

		const width = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_INTERNAL_FORMAT);
		expect(width).assertEqual(gl.RGBA4);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1030
	 * @tc.name testStencilFunc1
	 * @tc.desc Test stencilFunc.
	 */
	it('testStencilFunc1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFunc test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFunc(gl.NEVER, 0, 0b1110011);
		const stencilFuncParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFunc stencilFuncParameter: " + stencilFuncParameter);
		expect(stencilFuncParameter).assertEqual(gl.NEVER);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1031
	 * @tc.name testStencilFunc2
	 * @tc.desc Test stencilFunc.
	 */
	it('testStencilFunc2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFunc test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFunc(gl.EQUAL, 0, 0b1110011);
		const stencilFuncParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFunc stencilFuncParameter: " + stencilFuncParameter);
		expect(stencilFuncParameter).assertEqual(gl.EQUAL);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1032
	 * @tc.name testStencilFunc3
	 * @tc.desc Test stencilFunc.
	 */
	it('testStencilFunc3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFunc test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFunc(gl.GREATER, 0, 0b1110011);
		const stencilFuncParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFunc stencilFuncParameter: " + stencilFuncParameter);
		expect(stencilFuncParameter).assertEqual(gl.GREATER);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1033
	 * @tc.name testStencilFuncSeparate1
	 * @tc.desc Test stencilFuncSeparate.
	 */
	it('testStencilFuncSeparate1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFuncSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFuncSeparate(gl.BACK, gl.EQUAL, 0.2, 1110011);
		const stencilFuncSeparateParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFuncSeparate stencilFuncSeparateParameter: " +
			stencilFuncSeparateParameter);
		expect(stencilFuncSeparateParameter).assertEqual(gl.GREATER);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1034
	 * @tc.name testStencilFuncSeparate2
	 * @tc.desc Test stencilFuncSeparate.
	 */
	it('testStencilFuncSeparate2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFuncSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		gl.stencilFuncSeparate(gl.FRONT_AND_BACK, gl.LEQUAL, 0.2, 1110011);
		const stencilFuncSeparateParameter = gl.getParameter(gl.STENCIL_FUNC);
		console.info("stencilFuncSeparate stencilFuncSeparateParameter: " +
			stencilFuncSeparateParameter);
		expect(stencilFuncSeparateParameter).assertEqual(gl.LEQUAL);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1036
	 * @tc.name testStencilMaskSeparate2
	 * @tc.desc Test stencilMaskSeparate.
	 */
	it('testStencilMaskSeparate2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilMaskSeparate test start ...' + JSON.stringify(gl));
		gl.stencilMaskSeparate(gl.FRONT_AND_BACK, 110101);
		const stencilMaskSeparateParameter = gl.getParameter(gl.STENCIL_WRITEMASK);
		console.info("stencilMaskSeparate stencilMaskSeparateParameter: " +
			stencilMaskSeparateParameter);
		expect(stencilMaskSeparateParameter).assertEqual(110101);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1037
	 * @tc.name webgl_test_bindFramebuffer_1
	 * @tc.desc Test bindFramebuffer.
	 */
	it('webgl_test_bindFramebuffer_1', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindFramebuffer");

		var framebuffer = gl.createFramebuffer();

		const isFramebuffer1 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> isFramebuffer1: " + isFramebuffer1);
		expect(isFramebuffer1).assertEqual(false);

		// bind
		gl.bindFramebuffer(gl2.READ_FRAMEBUFFER, framebuffer);

		const isFrameBuffer2 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> bindFramebuffer --> isFramebuffer_2: " +
			isFrameBuffer2);
		expect(isFrameBuffer2).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1038
	 * @tc.name webgl_test_bindFramebuffer_2
	 * @tc.desc Test bindFramebuffer.
	 */
	it('webgl_test_bindFramebuffer_2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindFramebuffer");

		var framebuffer = gl.createFramebuffer();

		const isFramebuffer1 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> isFramebuffer1: " + isFramebuffer1);
		expect(isFramebuffer1).assertEqual(false);

		// bind
		gl.bindFramebuffer(gl2.DRAW_FRAMEBUFFER, framebuffer);

		const isFrameBuffer2 = gl.isFramebuffer(framebuffer);
		console.info("createFramebuffer --> bindFramebuffer --> isFramebuffer_2: " +
			isFrameBuffer2);
		expect(isFrameBuffer2).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1039
	 * @tc.name testGetRenderbufferParameter_1
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const height = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_HEIGHT);
		expect(height).assertEqual(256);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1040
	 * @tc.name testGetRenderbufferParameter_2
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_BLUE_SIZE);
		expect(size).assertEqual(4);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1041
	 * @tc.name testGetRenderbufferParameter_3
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_RED_SIZE);
		expect(size).assertEqual(4);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1042
	 * @tc.name testGetRenderbufferParameter_4
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_ALPHA_SIZE);
		expect(size).assertEqual(4);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1043
	 * @tc.name testGetRenderbufferParameter_5
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);
		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_DEPTH_SIZE);
		expect(size).assertEqual(0);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1044
	 * @tc.name testGetRenderbufferParameter_6
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl2.RENDERBUFFER, renderBuffer);
		gl2.renderbufferStorageMultisample(gl2.RENDERBUFFER, 4, gl.RBGA4, 256, 256);
		const size = gl.getRenderbufferParameter(gl2.RENDERBUFFER, gl2.RENDERBUFFER_SAMPLES);
		expect(size).assertEqual();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1045
	 * @tc.name testGetRenderbufferParameter_7
	 * @tc.desc Test getRenderbufferParameter.
	 */
	it('testGetRenderbufferParameter_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getRenderbufferParameter test start ...' + JSON.stringify(gl));
		const renderBuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderBuffer);
		gl.renderbufferStorage(gl.RENDERBUFFER, gl.RGBA4, 256, 256);

		const size = gl.getRenderbufferParameter(gl.RENDERBUFFER, gl.RENDERBUFFER_STENCIL_SIZE);
		expect(size).assertEqual(0);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1046
	 * @tc.name webgl_test_copyTexSubImage2D1
	 * @tc.desc Test copyTexSubImage2D.
	 */
	it('webgl_test_copyTexSubImage2D1', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexSubImage2D");

		gl.copyTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, 0, 0, 16, 16);
		let errorCode = gl.getError();
		console.info("webgltest copyTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1047
	 * @tc.name webgl_test_copyTexSubImage2D2
	 * @tc.desc Test copyTexSubImage2D.
	 */
	it('webgl_test_copyTexSubImage2D2', 0, async function(done) {
		//initContext();
		console.info("webgltest into copyTexSubImage2D");
		gl.copyTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, 0, 0, 16, 16);
		let errorCode = gl.getError();
		console.info("webgltest copyTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1048
	 * @tc.name webgl_test_drawElements_3
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements_3', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");

		gl.drawElements(gl.LINE_STRIP, 8, gl.UNSIGNED_BYTE, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1049
	 * @tc.name webgl_test_drawElements_4
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements_4', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");

		gl.drawElements(gl.POINTS, 8, gl.UNSIGNED_INT, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1050
	 * @tc.name webgl_test_drawElements_5
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements_5', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");

		gl.drawElements(gl.LINE_LOOP, 8, gl.UNSIGNED_INT, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1051
	 * @tc.name webgl_test_drawElements_6
	 * @tc.desc Test drawElements.
	 */
	it('webgl_test_drawElements_6', 0, async function(done) {
		//initContext();
		console.info("webgltest into drawElements");

		gl.drawElements(gl.LINES, 8, gl.UNSIGNED_INT, 0);

		let errorCode = gl.getError();
		console.info("webgltest drawElements getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1052
	 * @tc.name webgl_test_framebufferRenderbuffer1
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer1', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT1, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1053
	 * @tc.name webgl_test_framebufferRenderbuffer2
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer2', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT2, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1054
	 * @tc.name webgl_test_framebufferRenderbuffer3
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer3', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT3, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1055
	 * @tc.name webgl_test_framebufferRenderbuffer4
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer4', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT4, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1056
	 * @tc.name webgl_test_framebufferRenderbuffer5
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer5', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT5, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1057
	 * @tc.name webgl_test_framebufferRenderbuffer6
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer6', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT6, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1058
	 * @tc.name webgl_test_framebufferRenderbuffer7
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer7', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT7, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1059
	 * @tc.name webgl_test_framebufferRenderbuffer8
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer8', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT8, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1060
	 * @tc.name webgl_test_framebufferRenderbuffer9
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer9', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT9, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1061
	 * @tc.name webgl_test_framebufferRenderbuffer10
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer10', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT10, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1062
	 * @tc.name webgl_test_framebufferRenderbuffer11
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer11', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT11, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1063
	 * @tc.name webgl_test_framebufferRenderbuffer12
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer12', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT12, gl.RENDERBUFFER,
			renderbuffer);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1064
	 * @tc.name webgl_test_framebufferRenderbuffer13
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer13', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT13, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1065
	 * @tc.name webgl_test_framebufferRenderbuffer14
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer14', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT14, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1066
	 * @tc.name webgl_test_framebufferRenderbuffer15
	 * @tc.desc Test framebufferRenderbuffer.
	 */
	it('webgl_test_framebufferRenderbuffer15', 0, async function(done) {
		//initContext();
		console.info("webgltest into framebufferRenderbuffer");

		const renderbuffer = gl.createRenderbuffer();
		gl.framebufferRenderbuffer(gl.READ_FRAMEBUFFER, gl.COLOR_ATTACHMENT15, gl.RENDERBUFFER,
			renderbuffer);

		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1067
	 * @tc.name testGetParameter_1
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.BLEND_DST_ALPHA);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1068
	 * @tc.name testGetParameter_2
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.ALIASED_POINT_SIZE_RANGE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1069
	 * @tc.name testGetParameter_3
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.BLUE_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1070
	 * @tc.name testGetParameter_4
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.ALPHA_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1071
	 * @tc.name testGetParameter_5
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.COMPRESSED_TEXTURE_FORMATS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1072
	 * @tc.name testGetParameter_6
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.ELEMENT_ARRAY_BUFFER_BINDING);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1073
	 * @tc.name testGetParameter_7
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.FRONT_FACE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1074
	 * @tc.name testGetParameter_8
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.GREEN_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1075
	 * @tc.name testGetParameter_9
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.IMPLEMENTATION_COLOR_READ_FORMAT);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1076
	 * @tc.name testGetParameter_10
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.IMPLEMENTATION_COLOR_READ_TYPE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1077
	 * @tc.name testGetParameter_11
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_COMBINED_TEXTURE_IMAGE_UNITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1078
	 * @tc.name testGetParameter_12
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_CUBE_MAP_TEXTURE_SIZE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1079
	 * @tc.name testGetParameter_13
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_FRAGMENT_UNIFORM_VECTORS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1080
	 * @tc.name testGetParameter_14
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_14', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_RENDERBUFFER_SIZE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1081
	 * @tc.name testGetParameter_15
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_15', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_TEXTURE_IMAGE_UNITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1082
	 * @tc.name testGetParameter_16
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_16', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_TEXTURE_SIZE);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1083
	 * @tc.name testGetParameter_17
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_17', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_VARYING_VECTORS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1084
	 * @tc.name testGetParameter_18
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_18', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_VERTEX_ATTRIBS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1085
	 * @tc.name testGetParameter_19
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_19', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_VERTEX_TEXTURE_IMAGE_UNITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1086
	 * @tc.name testGetParameter_20
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_20', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.MAX_VERTEX_UNIFORM_VECTORS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1087
	 * @tc.name testGetParameter_21
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_21', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.POLYGON_OFFSET_FACTOR);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1088
	 * @tc.name testGetParameter_22
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_22', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.POLYGON_OFFSET_UNITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1089
	 * @tc.name testGetParameter_23
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_23', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.RED_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1090
	 * @tc.name testGetParameter_24
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_24', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.RENDERER);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1091
	 * @tc.name testGetParameter_25
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_25', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.SAMPLE_BUFFERS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1092
	 * @tc.name testGetParameter_26
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_26', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_FAIL);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1093
	 * @tc.name testGetParameter_27
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_27', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_FUNC);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1094
	 * @tc.name testGetParameter_28
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_28', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_PASS_DEPTH_FAIL);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1095
	 * @tc.name testGetParameter_29
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_29', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_PASS_DEPTH_PASS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1096
	 * @tc.name testGetParameter_30
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_30', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_REF);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1097
	 * @tc.name testGetParameter_31
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_31', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_VALUE_MASK);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1098
	 * @tc.name testGetParameter_32
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_32', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BACK_WRITEMASK);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1099
	 * @tc.name testGetParameter_33
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_33', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_BITS);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_FRAMEBUFFER_OPERATION);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_1100
	 * @tc.name testGetParameter_34
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameter_34', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.activeTexture(gl.TEXTURE0);
		var textureParameter = gl.getParameter(gl.STENCIL_PASS_DEPTH_FAIL);
		// returns "33984" (0x84C0, gl.TEXTURE0 enum value)
		console.info("activeTexture --> getParameter: " + textureParameter);
		let errorCode = gl.getError();
		console.info("webgltest framebufferRenderbuffer getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});
})
