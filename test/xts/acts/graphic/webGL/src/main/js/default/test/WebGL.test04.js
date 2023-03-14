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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0301
	 * @tc.name testUniform1iv3
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform1iv(uniformLocation, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0302
	 * @tc.name testUniform1iv4
	 * @tc.desc Test uniform1iv.
	 */
	it('testUniform1iv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform1iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform1iv(uniformLocation, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0303
	 * @tc.name testUniform2ivFirst
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2ivFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform2iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0304
	 * @tc.name testUniform2ivSecond
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2ivSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform2iv(uniformLocation, int32Array, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0305
	 * @tc.name testUniform2ivError
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2ivError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform2iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0306
	 * @tc.name testUniform2iv3
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform2iv(uniformLocation, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0307
	 * @tc.name testUniform2iv4
	 * @tc.desc Test uniform2iv.
	 */
	it('testUniform2iv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform2iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform2iv(uniformLocation, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0308
	 * @tc.name testUniform3ivFirst
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3ivFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform3iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0309
	 * @tc.name testUniform3ivSecond
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3ivSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform3iv(uniformLocation, int32Array, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0310
	 * @tc.name testUniform3ivError
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3ivError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform3iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0311
	 * @tc.name testUniform3iv3
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform3iv(uniformLocation, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0312
	 * @tc.name testUniform3iv4
	 * @tc.desc Test uniform3iv.
	 */
	it('testUniform3iv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform3iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform3iv(uniformLocation, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0313
	 * @tc.name testUniform4ivFirst
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4ivFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform4iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0314
	 * @tc.name testUniform4ivSecond
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4ivSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform4iv(uniformLocation, int32Array, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0315
	 * @tc.name testUniform4ivError
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4ivError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		const int32Array = new Int32Array([1, 2]);
		gl2.uniform4iv(uniformLocation, int32Array);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0316
	 * @tc.name testUniform4iv3
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform4iv(uniformLocation, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0317
	 * @tc.name testUniform4iv4
	 * @tc.desc Test uniform4iv.
	 */
	it('testUniform4iv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniform4iv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniform4iv(uniformLocation, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0318
	 * @tc.name testUniformMatrix4fvFirst
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fvFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix4fv(uniformLocation, true, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0319
	 * @tc.name testUniformMatrix4fv2
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix4fv(uniformLocation, true, flaot32list, 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0320
	 * @tc.name testUniformMatrix4fvError
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix4fv(uniformLocation, true, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0321
	 * @tc.name testUniformMatrix4fv3
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix4fv(uniformLocation, true, [1, 2]);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0322
	 * @tc.name testUniformMatrix4fv4
	 * @tc.desc Test uniformMatrix4fv.
	 */
	it('testUniformMatrix4fv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix4fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix4fv(uniformLocation, true, [1, 2], 0, 0);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0323
	 * @tc.name testUniformMatrix3fvFirst
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fvFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix3fv(uniformLocation, true, flaot32list);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0324
	 * @tc.name testUniformMatrix3fv2
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix3fv(uniformLocation, true, flaot32list, 0, 0);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0325
	 * @tc.name testUniformMatrix3fvError
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix3fv(uniformLocation, true, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0326
	 * @tc.name testUniformMatrix3fv3
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix3fv(uniformLocation, true, [1, 2]);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0327
	 * @tc.name testUniformMatrix3fv4
	 * @tc.desc Test uniformMatrix3fv.
	 */
	it('testUniformMatrix3fv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix3fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix3fv(uniformLocation, true, [1, 2], 0, 0);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0328
	 * @tc.name testUniformMatrix2fv1
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix2fv(uniformLocation, true, flaot32list);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0329
	 * @tc.name testUniformMatrix2fv2
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix2fv(uniformLocation, true, flaot32list, 0, 0);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0330
	 * @tc.name testUniformMatrix2fvError
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fvError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = gl.createProgram();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		var flaot32list = new Float32Array([1, 2]);
		gl2.uniformMatrix2fv(uniformLocation, true, flaot32list);
		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(1282);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0331
	 * @tc.name testUniformMatrix2fv3
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix2fv(uniformLocation, true, [1, 2]);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0332
	 * @tc.name testUniformMatrix2fv4
	 * @tc.desc Test uniformMatrix2fv.
	 */
	it('testUniformMatrix2fv4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformMatrix2fv test start ...' + JSON.stringify(gl2));
		const programObj = globalFunction();
		const uniformLocation = gl.getUniformLocation(programObj, "a_Position");

		gl2.uniformMatrix2fv(uniformLocation, true, [1, 2], 0, 0);

		const error = gl.getError();
		console.info("error: " + error);
		expect(error).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0333
	 * @tc.name testBufferData1
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData1', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferSize = gl.getBufferParameter(gl.ARRAY_BUFFER, gl.BUFFER_SIZE);
		console.info('bufferSize' + bufferSize);
		expect(bufferSize).assertEqual(1024);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0334
	 * @tc.name testBufferData2
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl2.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferUsage = gl.getBufferParameter(gl.ARRAY_BUFFER, gl.BUFFER_USAGE);
		console.info('bufferUsage' + bufferUsage);
		expect(bufferUsage).assertEqual(gl.STATIC_DRAW);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0335
	 * @tc.name testBufferData3
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.DYNAMIC_READ);
		const errorCode = gl.getError();
		console.info("webgltest bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0336
	 * @tc.name testBufferData4
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferData(gl.ELEMENT_ARRAY_BUFFER, new ArrayBuffer(8), gl.DYNAMIC_READ, 0, 0);
		const errorCode = gl.getError();
		console.info("webgl2test bufferData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0337
	 * @tc.name testBufferDataError
	 * @tc.desc Test bufferData.
	 */
	it('testBufferDataError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferData(1024, gl.STATIC_DRAW);
		const bufferSize = gl.getBufferParameter(gl.ARRAY_BUFFER, gl.BUFFER_SIZE);
		console.info('bufferSize' + bufferSize);
		const type = (bufferSize === 1024)
		expect(type).assertEqual(true);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0338
	 * @tc.name testBufferSubData
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0339
	 * @tc.name testBufferSubData2
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubData2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(gl.COPY_READ_BUFFER, 512, new ArrayBuffer(8), 0, 0);
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0340
	 * @tc.name testTexImage2D
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_2D, 0, 32, 512, 512, 0, 32, 32, view);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0341
	 * @tc.name testTexImage2D2
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D2', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		gl2.texImage2D(gl2.TEXTURE_2D, 0, gl.RGB8, 512, 512, 0, gl.RGB, gl.UNSIGNED_BYTE, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0342
	 * @tc.name testTexImage2D3
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2D3', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_2D, 0, gl.R16F, 512, 512, 0, gl.RED, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0343
	 * @tc.name testTexImage2D4
	 * @tc.desc Test frontFace.
	 */
	it('testTexImage2D4', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texImage2D(gl2.TEXTURE_2D, 0, gl.R16F, 512, 512, 0, gl.RED, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0344
	 * @tc.name testTexSubImage2D5
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D5', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0345
	 * @tc.name testTexSubImage2D6
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D6', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RED, gl.FLOAT, 0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0346
	 * @tc.name testTexSubImage2D7
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D7', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RGBA, gl.UNSIGNED_BYTE, view);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0347
	 * @tc.name testTexSubImage2D8
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2D8', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.texSubImage2D(gl2.TEXTURE_2D, 0, 0, 0, 512, 512, gl.RGBA, gl.UNSIGNED_BYTE, view);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0348
	 * @tc.name testCompressedTexImage2D9
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D9', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(gl2.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, gl.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0349
	 * @tc.name testCompressedTexImage2D11
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2D11', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.compressedTexImage2D(gl2.TEXTURE_2D, 0, 0x83F3, 512, 512, 0, view, 0, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0350
	 * @tc.name testCompressedTexSubImage2D12
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D12', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(gl2.TEXTURE_2D, 0, 256, 256, 512, 512, 0x83F3, gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0351
	 * @tc.name testCompressedTexSubImage2D13
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2D13', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.compressedTexSubImage2D(gl2.TEXTURE_2D, 0, 256, 256, 512, 512, 0x83F3, view, 0, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0352
	 * @tc.name testReadPixelsFirst
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsFirst', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_BYTE, view);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0353
	 * @tc.name testReadPixelsSecond
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsSecond', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_BYTE, 0);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0354
	 * @tc.name testReadPixelsThird
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsThird', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_BYTE, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0355
	 * @tc.name testReadPixelsError
	 * @tc.desc Test readPixels.
	 */
	it('testReadPixelsError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 readPixels test start ...' + JSON.stringify(gl2));
		var buffer = new ArrayBuffer(8);
		var view = new DataView(buffer, 0);
		view.setInt16(1, 42);
		const returnVale = gl2.readPixels(0, 0, 512, 512, gl2.RGBA, gl2.UNSIGNED_BYTE, view, 0);
		const errorCode = gl.getError();
		console.info("webgl2test readPixels getError: " + errorCode);
		const type =
			expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});

/*
 * ***************************************** GT ************************************************
 */

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0356
	 * @tc.name testIsFramebuffer
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testIsFramebuffer test start ...66');
			var framebuffer = gl.createFramebuffer();
			const isFramebuffer = gl.isFramebuffer(framebuffer);
			console.info("createFramebuffer --> isFramebuffer: " + isFramebuffer);
			expect(isFramebuffer).assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testIsFramebuffer has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0357
	 * @tc.name testIsFramebuffer_01
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testIsFramebuffer_01 test start ...66');
			var framebuffer = gl.createFramebuffer();
			gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
			const isFramebuffer = gl.isFramebuffer(framebuffer);
			console.info("createFramebuffer --> bindFramebuffer --> isFramebuffer: " +
				isFramebuffer);
			expect(isFramebuffer).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testIsFramebuffer_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0358
	 * @tc.name testIsFramebuffer_02
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testIsFramebuffer_02 test start ...66');
			var programobject = gl.createProgram();
			const isFramebuffer = gl.isFramebuffer(programobject);
			console.info("createFramebuffer --> isFramebuffer: " + isFramebuffer);
			expect(isFramebuffer).assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testIsFramebuffer_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0359
	 * @tc.name testIsFramebuffer_03
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsFramebuffer_03 test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER)
		const isFramebuffer = gl.isFramebuffer(shader);
		console.info("createFramebuffer --> isFramebuffer: " + isFramebuffer);
		expect(isFramebuffer).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0360
	 * @tc.name testIsFramebuffer_04
	 * @tc.desc Test isFramebuffer.
	 */
	it('testIsFramebuffer_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsFramebuffer_04 test start ...66');
		var renderbufferObject = gl.createRenderbuffer();
		const isFramebuffer = gl.isFramebuffer(renderbufferObject);
		console.info("createFramebuffer --> isFramebuffer: " + isFramebuffer);
		expect(isFramebuffer).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0361
	 * @tc.name testIsProgram
	 * @tc.desc Test isProgram.
	 */
	it('testIsProgram', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsProgram test start ...66');
		var program = gl.createProgram();
		const isProgram = gl.isProgram(program);
		console.info("createProgram --> isProgram: " + isProgram);
		expect(isProgram).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0362
	 * @tc.name testIsProgram_01
	 * @tc.desc Test isProgram.
	 */
	it('testIsProgram_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsProgram_01 test start ...66');
		var framebuffer = gl.createFramebuffer();
		const programError = gl.getError();
		console.info("createProgram --> programError: " + programError);
		const isProgram = gl.isProgram(framebuffer);
		console.info("createProgram --> isProgram: " + isProgram);
		expect(isProgram).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0363
	 * @tc.name testIsProgram_02
	 * @tc.desc Test isProgram.
	 */
	it('testIsProgram_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsProgram_02 test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		const programError = gl.getError();
		console.info("createProgram --> programError: " + programError);
		const isProgram = gl.isProgram(renderbuffer);
		console.info("createProgram --> isProgram: " + isProgram);
		expect(isProgram).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0364
	 * @tc.name testIsProgram_03
	 * @tc.desc Test isProgram.
	 */
	it('testIsProgram_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsProgram_03 test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER);
		const programError = gl.getError();
		console.info("createProgram --> programError: " + programError);
		const isProgram = gl.isProgram(shader);
		console.info("createProgram --> isProgram: " + isProgram);
		expect(isProgram).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0366
	 * @tc.name testIsRenderbuffer
	 * @tc.desc Test isRenderbuffer.
	 */
	it('testIsRenderbuffer', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsRenderbuffer test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		const isrenderbuffer = gl.isRenderbuffer(renderbuffer);
		console.info("createRenderbuffer --> isRenderbuffer: " + isrenderbuffer);
		expect(isrenderbuffer).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0367
	 * @tc.name testIsRenderbuffer_01
	 * @tc.desc Test isRenderbuffer.
	 */
	it('testIsRenderbuffer_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsRenderbuffer_01 test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		gl.bindRenderbuffer(gl.RENDERBUFFER, renderbuffer);
		const isrenderbuffer = gl.isRenderbuffer(renderbuffer);
		console.info("createRenderbuffer --> isRenderbuffer: " + isrenderbuffer);
		expect(isrenderbuffer).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0368
	 * @tc.name testIsRenderbuffer_02
	 * @tc.desc Test isRenderbuffer.
	 */
	it('testIsRenderbuffer_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsRenderbuffer_02 test start ...66');
		var framebuffer = gl.createFramebuffer();
		const isrenderbuffer = gl.isRenderbuffer(framebuffer);
		console.info("createRenderbuffer --> isRenderbuffer: " + isrenderbuffer);
		expect(isrenderbuffer).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0369
	 * @tc.name testIsRenderbuffer_03
	 * @tc.desc Test isRenderbuffer.
	 */
	it('testIsRenderbuffer_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsRenderbuffer_03 test start ...66');
		var programobject = gl.createProgram();
		const isrenderbuffer = gl.isRenderbuffer(programobject);
		console.info("createRenderbuffer --> isRenderbuffer: " + isrenderbuffer);
		expect(isrenderbuffer).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0371
	 * @tc.name testIsShader
	 * @tc.desc Test isShader.
	 */
	it('testIsShader', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER)
		const isShader = gl.isShader(shader);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0372
	 * @tc.name testIsShader_01
	 * @tc.desc Test isShader.
	 */
	it('testIsShader_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader_01 test start ...66');
		var shader = gl.createShader(gl.FRAGMENT_SHADER)
		const isShader = gl.isShader(shader);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0373
	 * @tc.name testIsShader_02
	 * @tc.desc Test isShader.
	 */
	it('testIsShader_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader_02 test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		const isShader = gl.isShader(renderbuffer);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0374
	 * @tc.name testIsShader_03
	 * @tc.desc Test isShader.
	 */
	it('testIsShader_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader_03 test start ...66');
		var framebuffer = gl.createFramebuffer();
		const isShader = gl.isShader(framebuffer);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0375
	 * @tc.name testIsShader_04
	 * @tc.desc Test isShader.
	 */
	it('testIsShader_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsShader_04 test start ...66');
		var programobject = gl.createProgram();
		const isShader = gl.isShader(programobject);
		console.info("createShader --> isShader: " + isShader);
		expect(isShader).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0376
	 * @tc.name testIsTexture
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture test start ...66');
		var texture = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, texture);
		const isTexture = gl.isTexture(texture);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(true);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0377
	 * @tc.name testIsTexture_01
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture_01 test start ...66');
		var texture = gl.createTexture();
		const isTexture = gl.isTexture(texture);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0378
	 * @tc.name testIsTexture_02
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture_02 test start ...66');
		var programobject = gl.createProgram();
		const isTexture = gl.isTexture(programobject);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0379
	 * @tc.name testIsTexture_03
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture_03 test start ...66');
		var renderbuffer = gl.createRenderbuffer();
		const isTexture = gl.isTexture(renderbuffer);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0380
	 * @tc.name testIsTexture_04
	 * @tc.desc Test isTexture.
	 */
	it('testIsTexture_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testIsTexture_04 test start ...66');
		var framebuffer = gl.createFramebuffer();
		const isTexture = gl.isTexture(framebuffer);
		console.info("createShader --> isTexture: " + isTexture);
		expect(isTexture).assertEqual(false);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0383
	 * @tc.name testLineWidth_01
	 * @tc.desc Test lineWidth.
	 */
	it('testLineWidth_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLineWidth_01 test start ...66');
		gl.lineWidth(-1);
		const windtherror = gl.getError();
		console.info("windtherror: " + windtherror);
		expect(windtherror).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0384
	 * @tc.name testLineWidth_02
	 * @tc.desc Test lineWidth.
	 */
	it('testLineWidth_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLineWidth_02 test start ...66');
		gl.lineWidth(0);
		const windtherror = gl.getError();
		console.info("windtherror: " + windtherror);
		expect(windtherror).assertEqual(gl.INVALID_VALUE);
		done();
	})


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0385
	 * @tc.name testLinkProgram
	 * @tc.desc Test linkProgram.
	 */
	it('testLinkProgram', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLinkProgram test start ...66');
		console.info('jsWebGL testLinkProgram test start ...' + JSON.stringify(gl));
		var shaderProg = gl.createProgram();
		gl.linkProgram(shaderProg);
		const linkProgramError = gl.getError();
		console.info("linkProgramError: " + linkProgramError);
		expect(linkProgramError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0388
	 * @tc.name testLinkProgram_03
	 * @tc.desc Test linkProgram.
	 */
	it('testLinkProgram_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLinkProgram_03 test start ...66');
		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.linkProgram(shader);
		const linkProgramError = gl.getError();
		console.info("linkProgramError: " + linkProgramError);
		expect(linkProgramError).assertEqual(gl.INVALID_OPERATION);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0389
	 * @tc.name testLinkProgram_04
	 * @tc.desc Test linkProgram.
	 */
	it('testLinkProgram_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testLinkProgram_04 test start ...66');
		gl.linkProgram("-123");
		const linkProgramError = gl.getError();
		console.info("linkProgramError: " + linkProgramError);
		expect(linkProgramError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0390
	 * @tc.name testPixelStorei
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei test start ...66');
		var tex = gl.createTexture();
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.pixelStorei(gl.PACK_ALIGNMENT, 4);
		gl.pixelStorei(gl.UNPACK_ALIGNMENT, 8);
		const packValue = gl.getParameter(gl.PACK_ALIGNMENT);
		const unpackValue = gl.getParameter(gl.UNPACK_ALIGNMENT);
		console.info("packValue: " + packValue);
		console.info("unpackValue: " + unpackValue);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0391
	 * @tc.name testPixelStorei_01
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_01 test start ...66');
		var tex = gl.createTexture();
		gl.pixelStorei(gl.UNPACK_ALIGNMENT, 8);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0392
	 * @tc.name testPixelStorei_02
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_02 test start ...66');
		var tex = gl.createTexture();
		gl.pixelStorei(gl.LINE_LOOP, 8);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0393
	 * @tc.name testPixelStorei_03
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_03 test start ...66');
		var tex = gl.createTexture();
		gl.pixelStorei(gl.ONE_MINUS_SRC_COLOR, 8);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0394
	 * @tc.name testPixelStorei_04
	 * @tc.desc Test pixelStorei.
	 */
	it('testPixelStorei_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPixelStorei_04 test start ...66');
		var tex = gl.createTexture();
		gl.pixelStorei(gl.ONE_MINUS_SRC_COLOR, -1);
		const pixelStoreiError = gl.getError();
		console.info("pixelStoreiError: " + pixelStoreiError);
		expect(pixelStoreiError).assertEqual(gl.INVALID_VALUE);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0395
	 * @tc.name testPolygonOffset
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset test start ...66');
		gl.enable(gl.POLYGON_OFFSET_FILL);
		gl.polygonOffset(2, 3);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0396
	 * @tc.name testPolygonOffset_01
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset_01 test start ...66');
		gl.enable(gl.TRIANGLE_STRIP);
		gl.polygonOffset(-2, 3);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0397
	 * @tc.name testPolygonOffset_02
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset_02', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset_02 test start ...66');
		gl.enable(gl.TRIANGLES);
		gl.polygonOffset(-2, -3);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0398
	 * @tc.name testPolygonOffset_03
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset_03', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset_03 test start ...66');
		gl.enable(gl.LINES);
		gl.polygonOffset(0, 0);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0399
	 * @tc.name testPolygonOffset_04
	 * @tc.desc Test polygonOffset.
	 */
	it('testPolygonOffset_04', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testPolygonOffset_04 test start ...66');
		gl.enable(gl.LINE_STRIP);
		gl.polygonOffset(0, 0);
		const polygonOffsetError = gl.getError();
		console.info("polygonOffsetError: " + polygonOffsetError);
		expect(polygonOffsetError).assertEqual(gl.INVALID_ENUM);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0400
	 * @tc.name testBufferData
	 * @tc.desc Test bufferData.
	 */
	it('testBufferData', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testBufferData test start ...66');
		console.info('jsWebGL testBufferData test start ...' + JSON.stringify(gl));
		var buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferDataError = gl.getError();
		console.info("bufferDataError: " + bufferDataError);
		expect(bufferDataError).assertEqual(0);
		done();
	})
})
