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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0802
	 * @tc.name webgl_test_beginQuery22
	 * @tc.desc Test beginQuery.
	 */
	it('webgl_test_beginQuery22', 0, async function(done) {
		//initContext();
		console.info("webgltest into beginQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

		const currentQuery = gl2.getQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, gl2
			.CURRENT_QUERY);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0805
	 * @tc.name webgl_test_endQuery24
	 * @tc.desc Test endQuery.
	 */
	it('webgl_test_endQuery24', 0, async function(done) {
		//initContext();
		console.info("webgltest into endQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

		const currentQuery = gl2.getQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, gl2
			.CURRENT_QUERY);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);

		gl2.endQuery(0x8C2F);

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery2: " + isQuery2);
		expect(isQuery2).assertEqual(true);

		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0807
	 * @tc.name webgl_test_getQuery24
	 * @tc.desc Test endQuery.
	 */
	it('webgl_test_getQuery24', 0, async function(done) {
		//initContext();
		console.info("webgltest into endQuery");

		var query = gl2.createQuery();
		gl2.beginQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);

		const currentQuery = gl2.getQuery(gl2.TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, gl2
			.CURRENT_QUERY);
		const isQuery1 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery1: " + isQuery1);
		expect(isQuery1).assertEqual(true);

		gl2.endQuery(0x8C2F);

		const isQuery2 = gl2.isQuery(currentQuery);
		console.info("webgltest createQuery isQuery2: " + isQuery2);
		expect(isQuery2).assertEqual(true);

		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0808
	 * @tc.name webgl_test_bindSampler23
	 * @tc.desc Test bindSampler.
	 */
	it('webgl_test_bindSampler23', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindSampler");
		const sampler = gl2.createSampler();
		const isSampler1 = gl2.isSampler(sampler);
		console.info("webgltest createSampler isSampler1: " + isSampler1);
		expect(isSampler1).assertEqual(true);

		gl2.bindSampler(-1, sampler);
		const isSampler2 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler isSampler2: " + isSampler2);
		expect(isSampler2).assertEqual(true);

		gl2.deleteSampler(sampler);
		const isSampler3 = gl2.isSampler(sampler);
		console.info("webgltest createSampler bindSampler deleteSampler isSampler3: " + isSampler3);
		expect(isSampler3).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0809
	 * @tc.name webgl_test_samplerParameterf24
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf24', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_COMPARE_FUNC, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0810
	 * @tc.name webgl_test_samplerParameterf25
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf25', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_COMPARE_MODE, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0811
	 * @tc.name webgl_test_samplerParameterf26
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf26', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_MAX_LOD, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0812
	 * @tc.name webgl_test_samplerParameterf27
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf27', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0813
	 * @tc.name webgl_test_samplerParameterf28
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf28', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_MIN_LOD, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0814
	 * @tc.name webgl_test_samplerParameterf29
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf29', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_WRAP_R, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0815
	 * @tc.name webgl_test_samplerParameterf30
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf30', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_WRAP_S, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0816
	 * @tc.name webgl_test_samplerParameterf31
	 * @tc.desc Test samplerParameterf.
	 */
	it('webgl_test_samplerParameterf31', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameterf");

		let sampler = gl2.createSampler();
		//        gl2.bindSampler(0, sampler);
		gl2.samplerParameterf(sampler, gl.TEXTURE_WRAP_T, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameterf getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0817
	 * @tc.name webgl_test_samplerParameteri24
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri24', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_COMPARE_FUNC, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0818
	 * @tc.name webgl_test_samplerParameteri25
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri25', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_COMPARE_MODE, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0819
	 * @tc.name webgl_test_samplerParameteri26
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri26', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_MAX_LOD, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0820
	 * @tc.name webgl_test_samplerParameteri27
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri27', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0821
	 * @tc.name webgl_test_samplerParameteri28
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri28', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_MIN_LOD, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0822
	 * @tc.name webgl_test_samplerParameteri29
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri29', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_WRAP_R, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0823
	 * @tc.name webgl_test_samplerParameteri211
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri211', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_WRAP_S, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0824
	 * @tc.name webgl_test_samplerParameteri212
	 * @tc.desc Test samplerParameteri.
	 */
	it('webgl_test_samplerParameteri212', 0, async function(done) {
		//initContext();
		console.info("webgltest into samplerParameteri");
		let sampler = gl2.createSampler();
		gl2.samplerParameteri(sampler, gl.TEXTURE_WRAP_T, gl.NEAREST);

		let errorCode = gl.getError();
		console.info("webgltest samplerParameteri getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_ENUM);
		gl2.deleteSampler(sampler);
		//deleteContext();
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0825
	 * @tc.name testGetShaderPrecisionFormat
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormat', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat(gl.VERTEX_SHADER, gl.HIGH_INT);
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		const type = ((typeof shaderPrecisionFormat) === "object")
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0826
	 * @tc.name testGetActiveUniformBlockParameter_1
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter(program, blockIndex, gl2
			.UNIFORM_BLOCK_BINDING);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0827
	 * @tc.name testGetActiveUniformBlockParameter_2
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter_2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter(program, blockIndex, gl2
			.UNIFORM_BLOCK_ACTIVE_UNIFORMS);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0828
	 * @tc.name testGetActiveUniformBlockParameter_3
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter_3', 0, async function(done) {
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
			gl2.UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0829
	 * @tc.name testGetActiveUniformBlockParameter_4
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameter_4', 0, async function(done) {
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
			gl2.UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER);
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		const errorCode = gl.getError();
		console.info("jsWebGL getFramebufferAttachmentParameter errorCode: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_VALUE);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0830
	 * @tc.name testGetIndexedParameter_01_01
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_01_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_01_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_01 test start ...' + JSON.stringify(gl));
			const reurGLsizeiptr = gl2.getIndexedParameter(-gl.TRANSFORM_FEEDBACK_BUFFER_SIZE, -0);
			const typeGLsizeiptr = typeof(reurGLsizeiptr);
			expect(typeGLsizeiptr == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0831
	 * @tc.name testGetIndexedParameter_02_01
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_02_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_02_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_02 test start ...' + JSON.stringify(gl));
			const retuGLintptr = gl2.getIndexedParameter(-gl.TRANSFORM_FEEDBACK_BUFFER_START, -1);
			const typeGLintptr = typeof(retuGLintptr);
			expect(typeGLintptr == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0832
	 * @tc.name testGetIndexedParameter_03
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_03_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_03_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_03 test start ...' + JSON.stringify(gl));
			const WebGLBufferobject = gl2.getIndexedParameter(gl.UNIFORM_BUFFER_BINDING, -0);
			const isBuffer = gl.isBuffer(WebGLBufferobject);
			expect(isBuffer).assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_03 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0833
	 * @tc.name testGetIndexedParameter_04
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_04_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_04_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_04 test start ...' + JSON.stringify(gl));
			const reurGLsizeiptr = gl2.getIndexedParameter(-gl.UNIFORM_BUFFER_SIZE, null);
			const typeGLsizeiptr = typeof(reurGLsizeiptr);
			expect(typeGLsizeiptr == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_04 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0834
	 * @tc.name testGetIndexedParameter_05_01
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_05_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_05 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_05 test start ...' + JSON.stringify(gl));
			const retuGLintptr = gl2.getIndexedParameter(-gl.UNIFORM_BUFFER_START, -0);
			const typeGLintptr = typeof(retuGLintptr);
			expect(typeGLintptr == 'String').assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_05 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0838
	 * @tc.name testPixelStorei_1_02
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_02 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 4);
		const unpackValue = gl.getParameter(gl.UNPACK_FLIP_Y_WEBGL);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0839
	 * @tc.name testPixelStorei_1_03
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_03 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, 4);
		const unpackValue = gl.getParameter(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0840
	 * @tc.name testPixelStorei_1_04
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_04 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL, 4);
		const unpackValue = gl.getParameter(gl.UNPACK_COLORSPACE_CONVERSION_WEBGL);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0841
	 * @tc.name testPixelStorei_1_05
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_05 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.PACK_ROW_LENGTH, 4);
		const unpackValue = gl.getParameter(gl2.PACK_ROW_LENGTH);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0842
	 * @tc.name testPixelStorei_1_06
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_06 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.PACK_SKIP_PIXELS, 4);
		const unpackValue = gl.getParameter(gl2.PACK_SKIP_PIXELS);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0843
	 * @tc.name testPixelStorei_1_07
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_07 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.PACK_SKIP_ROWS, 4);
		const unpackValue = gl.getParameter(gl2.PACK_SKIP_ROWS);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0844
	 * @tc.name testPixelStorei_1_08
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_08', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_08 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_ROW_LENGTH, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_ROW_LENGTH);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0845
	 * @tc.name testPixelStorei_1_09
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_09', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_09 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_IMAGE_HEIGHT, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_IMAGE_HEIGHT);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0846
	 * @tc.name testPixelStorei_1_10
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_10', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_10 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_SKIP_PIXELS, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_SKIP_PIXELS);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0847
	 * @tc.name testPixelStorei_1_11
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_11 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_SKIP_ROWS, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_SKIP_ROWS);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0848
	 * @tc.name testPixelStorei_1_12
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_1_12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_1_12 test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl2.UNPACK_SKIP_IMAGES, 4);
		const unpackValue = gl.getParameter(gl2.UNPACK_SKIP_IMAGES);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0849
	 * @tc.name testBufferData_80
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_80', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_80 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0850
	 * @tc.name testBufferData_81
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_81', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_81 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_READ_BUFFER, buffer);
		gl.bufferData(gl2.COPY_READ_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0851
	 * @tc.name testBufferData_82
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_82', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_82 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_WRITE_BUFFER, buffer);
		gl.bufferData(gl2.COPY_WRITE_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0852
	 * @tc.name testBufferData_83
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_83', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_83 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.TRANSFORM_FEEDBACK_BUFFER, buffer);
		gl.bufferData(gl2.TRANSFORM_FEEDBACK_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0853
	 * @tc.name testBufferData_84
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_84', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_84 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.UNIFORM_BUFFER, buffer);
		gl.bufferData(gl2.UNIFORM_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0854
	 * @tc.name testBufferData_85
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_85', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_85 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_PACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_PACK_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0855
	 * @tc.name testBufferData_86
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_86', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_86 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0856
	 * @tc.name testBufferData_80_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_80_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_80_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, 1024, gl.DYNAMIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0857
	 * @tc.name testBufferData_81_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_81_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_81_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_READ_BUFFER, buffer);
		gl.bufferData(gl2.COPY_READ_BUFFER, 1024, gl.STREAM_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0858
	 * @tc.name testBufferData_82_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_82_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_82_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_WRITE_BUFFER, buffer);
		gl.bufferData(gl2.COPY_WRITE_BUFFER, 1024, gl2.STATIC_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0859
	 * @tc.name testBufferData_83_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_83_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_83_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.TRANSFORM_FEEDBACK_BUFFER, buffer);
		gl.bufferData(gl2.TRANSFORM_FEEDBACK_BUFFER, 1024, gl2.DYNAMIC_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0860
	 * @tc.name testBufferData_84_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_84_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_84_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.UNIFORM_BUFFER, buffer);
		gl.bufferData(gl2.UNIFORM_BUFFER, 1024, gl2.STREAM_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0861
	 * @tc.name testBufferData_85_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_85_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_85_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_PACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_PACK_BUFFER, 1024, gl2.STATIC_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0862
	 * @tc.name testBufferData_86_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_86_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_86_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, 1024, gl2.DYNAMIC_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0863
	 * @tc.name testBufferData_87_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_87_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_87_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, 1024, gl2.STREAM_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0864
	 * @tc.name testBufferData_80_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_80_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_80_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0865
	 * @tc.name testBufferData_81_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_81_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_81_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_READ_BUFFER, buffer);
		gl.bufferData(gl2.COPY_READ_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0866
	 * @tc.name testBufferData_82_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_82_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_82_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_WRITE_BUFFER, buffer);
		gl.bufferData(gl2.COPY_WRITE_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0867
	 * @tc.name testBufferData_83_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_83_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_83_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.TRANSFORM_FEEDBACK_BUFFER, buffer);
		gl.bufferData(gl2.TRANSFORM_FEEDBACK_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0868
	 * @tc.name testBufferData_84_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_84_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_84_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.UNIFORM_BUFFER, buffer);
		gl.bufferData(gl2.UNIFORM_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0869
	 * @tc.name testBufferData_85_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_85_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_85_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_PACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_PACK_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0870
	 * @tc.name testBufferData_86_1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_86_1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_86_1 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, new ArrayBuffer(8), gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0871
	 * @tc.name testBufferData_80_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_80_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_80_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.DYNAMIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0872
	 * @tc.name testBufferData_81_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_81_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_81_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_READ_BUFFER, buffer);
		gl.bufferData(gl2.COPY_READ_BUFFER, new ArrayBuffer(8), gl.STREAM_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0873
	 * @tc.name testBufferData_82_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_82_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_82_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.COPY_WRITE_BUFFER, buffer);
		gl.bufferData(gl2.COPY_WRITE_BUFFER, new ArrayBuffer(8), gl2.STATIC_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0874
	 * @tc.name testBufferData_83_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_83_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_83_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.TRANSFORM_FEEDBACK_BUFFER, buffer);
		gl.bufferData(gl2.TRANSFORM_FEEDBACK_BUFFER, new ArrayBuffer(8), gl2.DYNAMIC_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0875
	 * @tc.name testBufferData_84_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_84_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_84_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.UNIFORM_BUFFER, buffer);
		gl.bufferData(gl2.UNIFORM_BUFFER, new ArrayBuffer(8), gl2.STREAM_READ);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0876
	 * @tc.name testBufferData_85_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_85_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_85_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_PACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_PACK_BUFFER, new ArrayBuffer(8), gl2.STATIC_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0877
	 * @tc.name testBufferData_86_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_86_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_86_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, new ArrayBuffer(8), gl2.DYNAMIC_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0878
	 * @tc.name testBufferData_87_1_01
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData_87_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData_87_1_01 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl2.PIXEL_UNPACK_BUFFER, buffer);
		gl.bufferData(gl2.PIXEL_UNPACK_BUFFER, new ArrayBuffer(8), gl2.STREAM_COPY);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0879
	 * @tc.name testBufferSubData_1_02
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_02 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl.ARRAY_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0880
	 * @tc.name testBufferSubData_1_03
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_03 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl.ELEMENT_ARRAY_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0881
	 * @tc.name testBufferSubData_1_04
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_04 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.COPY_WRITE_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0882
	 * @tc.name testBufferSubData_1_05
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_05 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.TRANSFORM_FEEDBACK_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0883
	 * @tc.name testBufferSubData_1_06
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_06 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.UNIFORM_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0884
	 * @tc.name testBufferSubData_1_07
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_07', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_07 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.PIXEL_PACK_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0885
	 * @tc.name testBufferSubData_1_08
	 * @tc.desc Test bufferSubData.
	 */
	it('testBufferSubData_1_08', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferSubData_1_08 test start ...66');
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		gl.bufferSubData(gl2.PIXEL_UNPACK_BUFFER, 512, new ArrayBuffer(8));
		const bufferSubDataError = gl.getError();
		console.info("bufferSubDataError: " + bufferSubDataError);
		expect(bufferSubDataError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0886
	 * @tc.name testCompressedTexImage2D_1_01
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_01 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0887
	 * @tc.name testCompressedTexImage2D_1_02
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_02 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0888
	 * @tc.name testCompressedTexImage2D_1_03
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_03 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0889
	 * @tc.name testCompressedTexImage2D_1_04
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_04 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0890
	 * @tc.name testCompressedTexImage2D_1_05
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_05 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0891
	 * @tc.name testCompressedTexImage2D_1_06
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexImage2D_1_06 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0x83F3, 512, 512, 0, 0);
		const compressedTexImage2DError = gl.getError();
		console.info("compressedTexImage2DError: " + compressedTexImage2DError);
		expect(compressedTexImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0892
	 * @tc.name testCompressedTexSubImage2D_1_01
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_01 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0893
	 * @tc.name testCompressedTexSubImage2D_1_02
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_02 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0894
	 * @tc.name testCompressedTexSubImage2D_1_03
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_03 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0895
	 * @tc.name testCompressedTexSubImage2D_1_04
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_04 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0896
	 * @tc.name testCompressedTexSubImage2D_1_05
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_05', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_05 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0897
	 * @tc.name testCompressedTexSubImage2D_1_06
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D_1_06', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testCompressedTexSubImage2D_1_06 test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.compressedTexImage2D(gl.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, 0);
		gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 256, 256, 512, 512, 0x83F3,
			gl.PIXEL_UNPACK_BUFFER, 0);
		const compressedTexSubImage2DError = gl.getError();
		console.info("compressedTexSubImage2DError: " + compressedTexSubImage2DError);
		expect(compressedTexSubImage2DError).assertEqual(0);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0898
	 * @tc.name testTexImage2D_1_01
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_01 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0899
	 * @tc.name testTexImage2D_1_02
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_02 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0900
	 * @tc.name testTexImage2D_1_03
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D_1_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexImage2D_1_03 test start ...66');
		const buffer = new ArrayBuffer(8);
		const z = new Uint8Array(buffer, 1, 4);
		gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, gl.RGB8, 512, 512, 0, gl.RGB8, gl
			.UNSIGNED_BYTE, z);
		const texImage2DError = gl.getError();
		console.info("texImage2DError: " + texImage2DError);
		expect(texImage2DError).assertEqual(0);
		done();
	})
})
