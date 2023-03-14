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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0502
	 * @tc.name testGetInternalformatParameter
	 * @tc.desc Test getInternalformatParameter.
	 */
	it('testGetInternalformatParameter', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetInternalformatParameter test start ...66');
		var samples = gl2.getInternalformatParameter(gl.RENDERBUFFER, gl2.RGBA8, gl.SAMPLES);
		console.info("samples: " + samples);
		const int32list = [-1, -1, -1];
		expect(samples.toString() != int32list.toString()).assertEqual(true);
		//判断数据类型是否正确
		//        expect(samples.constructor).assertEqual(int32list.constructor);
		//判断数据值是否正确
		//        expect(samples.toString()).assertEqual(int32list.toString());
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0503
	 * @tc.name testGetInternalformatParameter_01
	 * @tc.desc Test getInternalformatParameter.
	 */
	it('testGetInternalformatParameter_01', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testGetInternalformatParameter_01 test start ...66');
		var samples = gl2.getInternalformatParameter(gl.RENDERBUFFER, gl2.RGBA8, gl.SAMPLES);
		console.info("samples: " + samples);
		const int32list = new Int32Array([-1, -1, -1]);
		expect(samples.constructor).assertEqual(int32list.constructor);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0504
	 * @tc.name testRenderbufferStorageMultisample
	 * @tc.desc Test renderbufferStorageMultisample.
	 */
	it('testRenderbufferStorageMultisample', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testRenderbufferStorageMultisample test start ...66');
		gl2.renderbufferStorageMultisample(gl.RENDERBUFFER, 4, gl.RBGA4, 256, 256);
		const renderbufferStorageMultisampleError = gl.getError();
		console.info("renderbufferStorageMultisampleError: " + renderbufferStorageMultisampleError);
		expect(renderbufferStorageMultisampleError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0505
	 * @tc.name testTexStorage2D
	 * @tc.desc Test texStorage2D.
	 */
	it('testTexStorage2D', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTexStorage2D test start ...66');
		console.info('jsWebGL testTexStorage2D test start ...' + JSON.stringify(gl));
		gl2.texStorage2D(gl.TEXTURE_2D, 1, gl.RGB8, 256, 256);
		const texStorage2DError = gl.getError();
		console.info("texStorage2DError: " + texStorage2DError);
		expect(texStorage2DError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0506
	 * @tc.name testTransformFeedbackVaryings
	 * @tc.desc Test transformFeedbackVaryings.
	 */
	it('testTransformFeedbackVaryings', 0, async function(done) {
		//initContext();
		console.info('jsWebGL testTransformFeedbackVaryings test start ...66');
		console.info('jsWebGL testTransformFeedbackVaryings test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		var transformFeedbackOutputs = ['gl_Position', 'anotherOutput'];
		gl2.transformFeedbackVaryings(program, transformFeedbackOutputs, gl.INTERLEAVED_ATTRIBS);
		const transformFeedbackVaryingsError = gl.getError();
		console.info("transformFeedbackVaryingsError: " + transformFeedbackVaryingsError);
		expect(transformFeedbackVaryingsError).assertEqual(0);
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0508
	 * @tc.name testPauseTransformFeedback
	 * @tc.desc Test pauseTransformFeedback.
	 */
	it('testPauseTransformFeedback', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testPauseTransformFeedback test start ...66');
			console.info('jsWebGL testPauseTransformFeedback test start ...' + JSON.stringify(gl));
			const transformFeedback = gl2.createTransformFeedback();
			gl2.bindTransformFeedback(gl.TRANSFORM_FEEDBACK, transformFeedback);
			gl2.beginTransformFeedback(gl2.TRIANGLES);
			gl2.pauseTransformFeedback();
			const pauseTransformFeedbackError = gl.getError();
			console.info("pauseTransformFeedbackError: " + pauseTransformFeedbackError);
			expect(pauseTransformFeedbackError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testPauseTransformFeedback has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0509
	 * @tc.name testResumeTransformFeedback
	 * @tc.desc Test resumeTransformFeedback.
	 */
	it('testResumeTransformFeedback', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testPauseTransformFeedback test start ...66');
			console.info('jsWebGL testPauseTransformFeedback test start ...' + JSON.stringify(gl));
			gl2.resumeTransformFeedback();
			const resumeTransformFeedbackError = gl.getError();
			console.info("resumeTransformFeedbackError: " + resumeTransformFeedbackError);
			expect(resumeTransformFeedbackError).assertEqual(gl.INVALID_OPERATION);
			//deleteContext();
			done();
		} catch (e) {
			console.log("resumeTransformFeedbackError has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0510
	 * @tc.name testBindBufferBase
	 * @tc.desc Test bindBufferBase.
	 */
	it('testBindBufferBase', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testBindBufferBase test start ...66');
			console.info('jsWebGL testBindBufferBase test start ...' + JSON.stringify(gl));
			let buffer = gl.createBuffer();
			gl2.bindBufferBase(gl.TRANSFORM_FEEDBACK_BUFFER, 0, buffer);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0511
	 * @tc.name testGetIndexedParameter
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter test start ...66');
			console.info('jsWebGL testGetIndexedParameter test start ...' + JSON.stringify(gl));
			const WebGLBufferobject = gl2.getIndexedParameter(gl.TRANSFORM_FEEDBACK_BUFFER_BINDING,
				0);
			const isBuffer = gl.isBuffer(WebGLBufferobject);
			expect(isBuffer).assertEqual(false);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0512
	 * @tc.name testGetIndexedParameter_01
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_01 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_01 test start ...' + JSON.stringify(gl));
			const reurGLsizeiptr = gl2.getIndexedParameter(gl.TRANSFORM_FEEDBACK_BUFFER_SIZE, 0);
			const typeGLsizeiptr = typeof(reurGLsizeiptr);
			expect(typeGLsizeiptr).assertInstanceOf('String');
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0513
	 * @tc.name testGetIndexedParameter_02
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_02 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_02 test start ...' + JSON.stringify(gl));
			const retuGLintptr = gl2.getIndexedParameter(gl.TRANSFORM_FEEDBACK_BUFFER_START, 0);
			const typeGLintptr = typeof(retuGLintptr);
			expect(typeGLintptr).assertInstanceOf('String');
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0514
	 * @tc.name testGetIndexedParameter_03
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_03', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_03 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_03 test start ...' + JSON.stringify(gl));
			const WebGLBufferobject = gl2.getIndexedParameter(gl.UNIFORM_BUFFER_BINDING, 0);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0515
	 * @tc.name testGetIndexedParameter_04
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_04', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_04 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_04 test start ...' + JSON.stringify(gl));
			const reurGLsizeiptr = gl2.getIndexedParameter(gl.UNIFORM_BUFFER_SIZE, 0);
			const typeGLsizeiptr = typeof(reurGLsizeiptr);
			expect(typeGLsizeiptr).assertInstanceOf('String');
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_04 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0516
	 * @tc.name testGetIndexedParameter_05
	 * @tc.desc Test getIndexedParameter.
	 */
	it('testGetIndexedParameter_05', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetIndexedParameter_05 test start ...66');
			console.info('jsWebGL testGetIndexedParameter_05 test start ...' + JSON.stringify(gl));
			const retuGLintptr = gl2.getIndexedParameter(gl.UNIFORM_BUFFER_START, 0);
			const typeGLintptr = typeof(retuGLintptr);
			expect(typeGLintptr).assertInstanceOf('String');
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetIndexedParameter_05 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0517
	 * @tc.name testGetUniformIndices
	 * @tc.desc Test getUniformIndices.
	 */
	it('testGetUniformIndices', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetUniformIndices test start ...66');
			console.info('jsWebGL testGetUniformIndices test start ...' + JSON.stringify(gl));
			const program = gl.createProgram();
			const retuGLintptr = gl2.getUniformIndices(program, ['UBORed', 'UBOGreen', 'UBOBlue']);
			console.log("testGetUniformIndices has failed for " + retuGLintptr)
			const int32list = [-1, -1, -1];
			console.log("testGetUniformIndices has failed for " + int32list)
			//判断数据类型是否正确
			expect(retuGLintptr.constructor).assertEqual(int32list.constructor);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetUniformIndices has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0518
	 * @tc.name testGetUniformIndices_01
	 * @tc.desc Test getUniformIndices.
	 */
	it('testGetUniformIndices_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetUniformIndices_01 test start ...66');
			const program = gl.createProgram();
			const retuGLintptr = gl2.getUniformIndices(program, ['UBORed', 'UBOGreen', 'UBOBlue']);
			console.log("testGetUniformIndices has failed for " + retuGLintptr)
			const int32list = [-1, -1, -1];
			console.log("testGetUniformIndices_01 has failed for " + int32list)
			//判断数据值是否正确
			expect(retuGLintptr.toString() != int32list.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetUniformIndices has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0519
	 * @tc.name testGetActiveUniforms
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms test start ...66');
			console.info('jsWebGL testGetActiveUniforms test start ...' + JSON.stringify(gl));
			//        const programObj = createProgram(gl);


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



			const uniformIndices = [1, 2, 3];
			const uniformType = gl2.getActiveUniforms(programObj, uniformIndices, gl2.UNIFORM_TYPE)
			const glenumList = [-1, -1, -1];
			expect(uniformType.toString() != glenumList.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0520
	 * @tc.name testGetActiveUniforms_01
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_01 test start ...66');
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

			const uniformIndices = [1, 2, 3];
			const uniformType = gl2.getActiveUniforms(programObj, uniformIndices, gl2.UNIFORM_TYPE)
			const glenumList = [-1, -1, -1];
			expect(uniformType.constructor).assertEqual(glenumList.constructor);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0521
	 * @tc.name testGetActiveUniforms_02
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_02 test start ...66');
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

			const uniformIndices = [4, 5, 6];
			const uniformSize = gl2.getActiveUniforms(programObj, uniformIndices, gl2.UNIFORM_SIZE)
			const gluintList = [-1, 0, 0];
			expect(uniformSize.toString() != gluintList.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0522
	 * @tc.name testGetActiveUniforms_03
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_03', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_03 test start ...66');
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
			const uniformIndices = [7, 8, 9];
			const uniformBlockIndex = gl2.getActiveUniforms(programObj, uniformIndices, gl2
				.UNIFORM_BLOCK_INDEX)
			const glintlist = [-1, 0, 0];
			expect(uniformBlockIndex.toString() != glintlist.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_03 has failed for " + e)
			expect(null).assertFail()
		}
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0523
	 * @tc.name testGetActiveUniforms_04
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_04', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_04 test start ...66');
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
			const uniformIndices = [1, 2, 3];
			const uniformOffset = gl2.getActiveUniforms(programObj, uniformIndices, gl2
				.UNIFORM_OFFSET)
			const glintlist = [-1, 0, 0];
			expect(uniformOffset.toString() != glintlist.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_04 has failed for " + e)
			expect(null).assertFail()
		}
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0524
	 * @tc.name testGetActiveUniforms_05
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_05', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_05 test start ...66');
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
			const uniformIndices = [1, 2, 3];
			const uniformStride = gl2.getActiveUniforms(programObj, uniformIndices, gl2
				.UNIFORM_ARRAY_STRIDE)
			const glintlist = [-1, 0, 0];
			expect(uniformStride.toString() != glintlist.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_05 has failed for " + e)
			expect(null).assertFail()
		}
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0525
	 * @tc.name testGetActiveUniforms_06
	 * @tc.desc Test getActiveUniforms.
	 */
	it('testGetActiveUniforms_06', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testGetActiveUniforms_06 test start ...66');
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
			const uniformIndices = [1, 2, 3];
			const uniformMatrixStride = gl2.getActiveUniforms(programObj, uniformIndices, gl2
				.UNIFORM_MATRIX_STRIDE)
			const glbooleanlist = [-1, 0, 0];
			expect(uniformMatrixStride.toString() != glbooleanlist.toString()).assertEqual(true);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testGetActiveUniforms_06 has failed for " + e)
			expect(null).assertFail()
		}
		done();
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0527
	 * @tc.name testUseProgram
	 * @tc.desc Test useProgram.
	 */
	it('testUseProgram', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testUseProgram test start ...66');
			console.info('jsWebGL testUseProgram test start ...' + JSON.stringify(gl));
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
			const program = gl.createProgram();
			console.log("testUseProgram has failed for " + program)
			const useProgramError1 = gl.getError();
			console.info("useProgramError: " + useProgramError1);
			const renderBufferValue1 = gl.getParameter(gl.CURRENT_PROGRAM);
			console.log("testUseProgram has failed for " + program)
			gl.attachShader(program, vertexShader);
			gl.attachShader(program, fragmentShader);
			gl.linkProgram(program);
			gl.useProgram(program);
			const useProgramError = gl.getError();
			console.info("useProgramError: " + useProgramError);
			expect(useProgramError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testUseProgram has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0528
	 * @tc.name testValidateProgram
	 * @tc.desc Test validateProgram.
	 */
	it('testValidateProgram', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testValidateProgram test start ...66');
			console.info('jsWebGL testValidateProgram test start ...' + JSON.stringify(gl));
			const program = gl.createProgram();
			gl.validateProgram(program);
			const validateProgramError = gl.getError();
			console.info("validateProgramError: " + validateProgramError);
			expect(validateProgramError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testValidateProgram has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0529
	 * @tc.name testVertexAttrib1f
	 * @tc.desc Test vertexAttrib1f.
	 */
	it('testVertexAttrib1f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib1f test start ...66');
			console.info('jsWebGL testVertexAttrib1f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib1f(1, 10.0);
			const vertexAttrib1fError = gl.getError();
			console.info("vertexAttrib1fError: " + vertexAttrib1fError);
			expect(vertexAttrib1fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib1f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0530
	 * @tc.name testVertexAttrib2f
	 * @tc.desc Test vertexAttrib2f.
	 */
	it('testVertexAttrib2f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2f test start ...66');
			gl.vertexAttrib2f(1, 10.0);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0531
	 * @tc.name testVertexAttrib3f
	 * @tc.desc Test vertexAttrib3f.
	 */
	it('testVertexAttrib3f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testvertexAttrib3f test start ...66');
			console.info('jsWebGL testvertexAttrib3f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib3f(1, 10.0);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0532
	 * @tc.name testVertexAttrib4f
	 * @tc.desc Test vertexAttrib4f.
	 */
	it('testVertexAttrib4f', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4f test start ...66');
			console.info('jsWebGL testVertexAttrib4f test start ...' + JSON.stringify(gl));
			gl.vertexAttrib4f(1, 10.0);
			const vertexAttrib4fError = gl.getError();
			console.info("vertexAttrib4fError: " + vertexAttrib4fError);
			expect(vertexAttrib4fError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testvertexAttrib4f has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0533
	 * @tc.name testVertexAttrib1fv
	 * @tc.desc Test vertexAttrib1fv.
	 */
	it('testVertexAttrib1fv', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib1fv test start ...66');
			console.info('jsWebGL testVertexAttrib1fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib1fv(1, floatArray);
			const vertexAttrib1fvError = gl.getError();
			console.info("vertexAttrib1fvError: " + vertexAttrib1fvError);
			expect(vertexAttrib1fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib1fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0534
	 * @tc.name testVertexAttrib2fv
	 * @tc.desc Test vertexAttrib2fv.
	 */
	it('testVertexAttrib2fv', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib2fv test start ...66');
			console.info('jsWebGL testVertexAttrib2fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib2fv(1, floatArray);
			const vertexAttrib2fvError = gl.getError();
			console.info("vertexAttrib2fvError: " + vertexAttrib2fvError);
			expect(vertexAttrib2fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib2fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0535
	 * @tc.name testVertexAttrib3fv
	 * @tc.desc Test vertexAttrib3fv.
	 */
	it('testVertexAttrib3fv', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib3fv test start ...66');
			console.info('jsWebGL testVertexAttrib3fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib3fv(1, floatArray);
			const vertexAttrib3fvError = gl.getError();
			console.info("vertexAttrib3fvError: " + vertexAttrib3fvError);
			expect(vertexAttrib3fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib3fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0536
	 * @tc.name testVertexAttrib4fv
	 * @tc.desc Test vertexAttrib4fv.
	 */
	it('testVertexAttrib4fv', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttrib4fv test start ...66');
			console.info('jsWebGL testVertexAttrib4fv test start ...' + JSON.stringify(gl));
			const floatArray = new Float32Array([10.0, 5.0, 2.0]);
			gl.vertexAttrib4fv(1, floatArray);
			const vertexAttrib4fvError = gl.getError();
			console.info("vertexAttrib4fvError: " + vertexAttrib4fvError);
			expect(vertexAttrib4fvError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testVertexAttrib4fv has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0537
	 * @tc.name testVertexAttribPointer
	 * @tc.desc Test vertexAttribPointer.
	 */
	it('testVertexAttribPointer', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testVertexAttribPointer test start ...66');
			console.info('jsWebGL testVertexAttribPointer test start ...' + JSON.stringify(gl));
			gl.vertexAttribPointer(0, 3, gl.FLOAT, false, 20, 0);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0538
	 * @tc.name testViewport
	 * @tc.desc Test viewport.
	 */
	it('testViewport', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testViewport test start ...66');
			console.info('jsWebGL testViewport test start ...' + JSON.stringify(gl));
			gl.viewport(1, 2, 3, 4);
			const viewportError = gl.getError();
			console.info("viewportError: " + viewportError);
			expect(viewportError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testViewport has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0539
	 * @tc.name testTexStorage3D
	 * @tc.desc Test texStorage3D.
	 */
	it('testTexStorage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexStorage3D test start ...66');
			console.info('jsWebGL testTexStorage3D test start ...' + JSON.stringify(gl));
			gl2.texStorage3D(gl.TEXTURE_3D, 1, gl.RGB8, 256, 256, 256);
			const texStorage3DError = gl.getError();
			console.info("texStorage3DError: " + texStorage3DError);
			expect(texStorage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexStorage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0540
	 * @tc.name testTexImage3D
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D test start ...66');
			console.info('jsWebGL testTexImage3D test start ...' + JSON.stringify(gl));
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.RGBA, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, 0);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0541
	 * @tc.name testTexImage3D_01
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D_01 test start ...66');
			console.info('jsWebGL testTexImage3D_01 test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.RGBA, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
				buffer);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0542
	 * @tc.name testTexImage3D_02
	 * @tc.desc Test texImage3D.
	 */
	it('testTexImage3D_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexImage3D_02 test start ...66');
			console.info('jsWebGL testTexImage3D_02 test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.texImage3D(gl2.TEXTURE_3D, 0, gl.RGBA, 1, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
				buffer, 0);
			const texImage3DError = gl.getError();
			console.info("texImage3DError: " + texImage3DError);
			expect(texImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexImage3D_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0543
	 * @tc.name testTexSubImage3D
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D test start ...66');
			console.info('jsWebGL testTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.RGBA, gl.UNSIGNED_BYTE, 1);
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
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0544
	 * @tc.name testTexSubImage3D_01
	 * @tc.desc Test texSubImage3D.
	 */
	it('testTexSubImage3D_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testTexSubImage3D_01 test start ...66');
			console.info('jsWebGL testTexSubImage3D_01 test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(16);
			const view = new DataView(buffer, 12, 4);
			gl2.texSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 200, 200, 1, gl.RGBA, gl.UNSIGNED_BYTE,
				buffer, 0);
			const texSubImage3DError = gl.getError();
			console.info("texSubImage3DError: " + texSubImage3DError);
			expect(texSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testTexSubImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0545
	 * @tc.name testCopyTexSubImage3D
	 * @tc.desc Test copyTexSubImage3D.
	 */
	it('testCopyTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCopyTexSubImage3D test start ...66');
			console.info('jsWebGL testCopyTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.copyTexSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 0, 0, 16, 16);
			const copyTexSubImage3DError = gl.getError();
			console.info("copyTexSubImage3DError: " + copyTexSubImage3DError);
			expect(copyTexSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCopyTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0546
	 * @tc.name testCompressedTexImage3D_01
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D_01 test start ...66');
			console.info('jsWebGL testCompressedTexImage3D_01 test start ...' + JSON.stringify(gl));
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 512, 0, gl2
				.PIXEL_UNPACK_BUFFER, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0547
	 * @tc.name testCompressedTexImage3D_02
	 * @tc.desc Test compressedTexImage3D.
	 */
	it('testCompressedTexImage3D_02', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexImage3D_02 test start ...66');
			console.info('jsWebGL testCompressedTexImage3D_02 test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexImage3D(gl.TEXTURE_3D, 0, 0x83F0, 512, 512, 512, 0, buffer, 0, 0);
			const compressedTexImage3DError = gl.getError();
			console.info("compressedTexImage3DError: " + compressedTexImage3DError);
			expect(compressedTexImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexImage3D_02 has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0548
	 * @tc.name testCompressedTexSubImage3D
	 * @tc.desc Test compressedTexSubImage3D.
	 */
	it('testCompressedTexSubImage3D', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexSubImage3D test start ...66');
			console.info('jsWebGL testCompressedTexSubImage3D test start ...' + JSON.stringify(gl));
			gl2.compressedTexSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 512, 512, 512, gl2
				.COMPRESSED_R11_EAC,
				gl2.PIXEL_UNPACK_BUFFER, 0);
			const compressedTexSubImage3DError = gl.getError();
			console.info("compressedTexSubImage3DError: " + compressedTexSubImage3DError);
			expect(compressedTexSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexSubImage3D has failed for " + e)
			expect(null).assertFail()
		}
	})

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0549
	 * @tc.name testCompressedTexSubImage3D_01
	 * @tc.desc Test compressedTexSubImage3D.
	 */
	it('testCompressedTexSubImage3D_01', 0, async function(done) {
		//initContext();
		try {
			console.info('jsWebGL testCompressedTexSubImage3D test start ...66');
			console.info('jsWebGL testCompressedTexSubImage3D test start ...' + JSON.stringify(gl));
			const buffer = new ArrayBuffer(8);
			gl2.compressedTexSubImage3D(gl.TEXTURE_3D, 0, 0, 0, 0, 512, 512, 512, gl2
				.COMPRESSED_R11_EAC, buffer, 0, 256);
			const compressedTexSubImage3DError = gl.getError();
			console.info("compressedTexSubImage3DError: " + compressedTexSubImage3DError);
			expect(compressedTexSubImage3DError).assertEqual(0);
			//deleteContext();
			done();
		} catch (e) {
			console.log("testCompressedTexSubImage3D_01 has failed for " + e)
			expect(null).assertFail()
		}
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0550
	 * @tc.name testGetActiveAttribError
	 * @tc.desc Test getActiveAttrib.
	 */
	it('testGetActiveAttribError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getActiveAttrib test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const info = gl.getActiveAttrib();
		expect(info).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0551
	 * @tc.name testGetActiveUniformError
	 * @tc.desc Test getActiveUniform.
	 */
	it('testGetActiveUniformError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getActiveUniform test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		const info = gl.getActiveUniform(program);
		expect(info).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0552
	 * @tc.name testGetAttachedShadersError
	 * @tc.desc Test getAttachedShaders.
	 */
	it('testGetAttachedShadersError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getAttachedShaders test start ...' + JSON.stringify(gl));
		const shaderArray = gl.getAttachedShaders();
		console.info('getAttachedShaders shaderArray' + shaderArray);
		expect(shaderArray).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0553
	 * @tc.name testGetBufferParameterError
	 * @tc.desc Test getBufferParameter.
	 */
	it('testGetBufferParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getBufferParameter test start ...' + JSON.stringify(gl));
		const buffer = gl.createBuffer();
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, 1024, gl.STATIC_DRAW);
		const bufferSize = gl.getBufferParameter(gl.ARRAY_BUFFER);
		expect(bufferSize).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0554
	 * @tc.name testGetParameterError
	 * @tc.desc Test getParameter.
	 */
	it('testGetParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		var textureParameter = gl.getParameter();
		console.info("activeTexture --> getParameter: " + textureParameter);
		expect(textureParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0555
	 * @tc.name testGetError_error
	 * @tc.desc Test getParameter.
	 */
	it('testGetError_error', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getParameter test start ...' + JSON.stringify(gl));
		gl.enable(gl.SCISSOR_TEST);
		const scissorErrorCode = gl.scissor(0);
		console.info("scissorErrorCode: " + scissorErrorCode);
		expect(scissorErrorCode).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0556
	 * @tc.name testGetFramebufferAttachmentParameterError
	 * @tc.desc Test frontFace.
	 */
	it('testGetFramebufferAttachmentParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getFramebufferAttachmentParameter test start ...' + JSON.stringify(
			gl));
		const framebuffer = gl.getFramebufferAttachmentParameter(gl
			.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
		console.info('Framebuffer' + framebuffer);
		expect(framebuffer).assertEqual(undefined);
		done();
	});




	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0557
	 * @tc.name testGetProgramInfoLog
	 * @tc.desc Test getProgramInfoLog.
	 */
	it('testGetProgramInfoLog', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getProgramInfoLog test start ...' + JSON.stringify(gl));
		const program = gl.createProgram();
		// Attach pre-existing shaders
		gl.attachShader(program, VSHADER_SOURCE);
		gl.attachShader(program, FSHADER_SOURCE);
		gl.linkProgram(program);
		gl.validateProgram(program);
		const info = gl.getProgramInfoLog();
		gl.useProgram(program);
		expect(info).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0558
	 * @tc.name testGetShaderPrecisionFormatError
	 * @tc.desc Test getShaderPrecisionFormat.
	 */
	it('testGetShaderPrecisionFormatError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderPrecisionFormat test start ...' + JSON.stringify(gl));
		const shaderPrecisionFormat = gl.getShaderPrecisionFormat();
		console.info('shaderPrecisionFormat' + JSON.stringify(shaderPrecisionFormat));
		expect(shaderPrecisionFormat).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0559
	 * @tc.name testGetShaderInfoLogError
	 * @tc.desc Test getShaderInfoLog.
	 */
	it('testGetShaderInfoLogError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderInfoLog test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(shader, "shaderCode");

		/* compile shader source code. */
		gl.compileShader(shader);
		const message = gl.getShaderInfoLog();
		console.info("getShaderInfoLog message: " + message);
		const type = (message === null);
		expect(type).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0560
	 * @tc.name testGetShaderSource
	 * @tc.desc Test getShaderSource.
	 */
	it('testGetShaderSourceError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getShaderSource test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);
		gl.shaderSource(shader, 'originalSource');

		var source = gl.getShaderSource();
		console.info("getShaderSource source: " + source);
		expect(source).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0561
	 * @tc.name testGetVertexAttribFirst
	 * @tc.desc Test getVertexAttrib.
	 */
	it('testGetVertexAttribFirstError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttrib test start ...' + JSON.stringify(gl));
		const type = gl.getVertexAttrib(0);
		console.info("getVertexAttrib: type" + type);
		expect(type).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0562
	 * @tc.name testGetVertexAttribOffset
	 * @tc.desc Test getVertexAttribOffset.
	 */
	it('testGetVertexAttribOffsetError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL getVertexAttribOffset test start ...' + JSON.stringify(gl));
		const index = gl.getVertexAttribOffset();
		console.info("getVertexAttribOffset: index" + index);
		expect(index).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0563
	 * @tc.name testIsBufferError
	 * @tc.desc Test isBuffer.
	 */
	it('testIsBufferError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isBuffer test start ...' + JSON.stringify(gl));
		const buffer = gl.createBuffer();
		const isBuffer1 = gl.isBuffer(buffer);
		console.info('isBuffer1' + isBuffer1);
		expect(isBuffer1).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0564
	 * @tc.name testIsEnabledError
	 * @tc.desc Test isEnabled.
	 */
	it('testIsEnabledError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL isEnabled test start ...' + JSON.stringify(gl));
		gl.disable(gl.STENCIL_TEST);
		const isEnabledValue2 = gl.isEnabled(gl.STENCIL_TEST);
		console.info("isEnabledValue2: " + isEnabledValue2);
		expect(isEnabledValue2).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0565
	 * @tc.name testRenderbufferStorageError
	 * @tc.desc Test renderbufferStorage.
	 */
	it('testRenderbufferStorageError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL renderbufferStorage test start ...' + JSON.stringify(gl));
		const returnValue = gl.renderbufferStorage(256, 256);
		console.info("getRenderbufferParameter returnValue: " + returnValue);
		expect(returnValue).assertEqual(undefined);
		done();
	});



	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0566
	 * @tc.name testSampleCoverageError
	 * @tc.desc Test sampleCoverage.
	 */
	it('testSampleCoverageError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL sampleCoverage test start ...' + JSON.stringify(gl));
		//initContext();
		console.info('jsWebGL sampleCoverage test start ...' + JSON.stringify(gl));

		const sampleCoverageInvert = gl.sampleCoverage();

		console.info("sampleCoverageInvert: " + sampleCoverageInvert);

		expect(sampleCoverageInvert).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0567
	 * @tc.name testScissorError
	 * @tc.desc Test scissor.
	 */
	it('testScissorError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL scissor test start ...' + JSON.stringify(gl));
		const scissorValue = gl.scissor(200);
		console.info("scissorValue: " + scissorValue);
		expect(scissorValue).assertEqual(undefined);

		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0568
	 * @tc.name testShaderSourceError
	 * @tc.desc Test shaderSource.
	 */
	it('testShaderSourceError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL shaderSource test start ...' + JSON.stringify(gl));
		var shader = gl.createShader(gl.VERTEX_SHADER);
		var source = gl.shaderSource(shader);
		console.info("shaderSource source: " + source);
		expect(source).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0569
	 * @tc.name testStencilFuncError
	 * @tc.desc Test stencilFunc.
	 */
	it('testStencilFuncError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFunc test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		const stencilFuncParameter = gl.stencilFunc(gl.LESS);
		console.info("stencilFunc stencilFuncParameter: " + stencilFuncParameter);
		expect(stencilFuncParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0570
	 * @tc.name testStencilFuncSeparateError
	 * @tc.desc Test stencilFuncSeparate.
	 */
	it('testStencilFuncSeparateError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilFuncSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		const stencilFuncSeparateParameter = gl.stencilFuncSeparate(gl.FRONT);
		console.info("stencilFuncSeparate stencilFuncSeparateParameter: " +
			stencilFuncSeparateParameter);
		expect(stencilFuncSeparateParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0571
	 * @tc.name testStencilMaskError
	 * @tc.desc Test stencilMask.
	 */
	it('testStencilMaskError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilMask test start ...' + JSON.stringify(gl));
		const stencilMaskParameter = gl.stencilMask();
		console.info("stencilMask stencilMaskParameter: " + stencilMaskParameter);
		expect(stencilMaskParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0572
	 * @tc.name testStencilMaskSeparateError
	 * @tc.desc Test stencilMaskSeparate.
	 */
	it('testStencilMaskSeparateError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilMaskSeparate test start ...' + JSON.stringify(gl));
		const stencilMaskSeparateParameter = gl.stencilMaskSeparate(gl.FRONT);
		console.info("stencilMaskSeparate stencilMaskSeparateParameter: " +
			stencilMaskSeparateParameter);
		expect(stencilMaskSeparateParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0573
	 * @tc.name testStencilOpError
	 * @tc.desc Test stencilOp.
	 */
	it('testStencilOpError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOp test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		const stencilOpParameter = gl.stencilOp();
		console.info("stencilOp stencilOpParameter: " + stencilOpParameter);
		expect(stencilOpParameter).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0574
	 * @tc.name testStencilOpSeparate
	 * @tc.desc Test stencilOpSeparate.
	 */
	it('testStencilOpSeparateError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL stencilOpSeparate test start ...' + JSON.stringify(gl));
		gl.enable(gl.STENCIL_TEST);
		const stencilOpSeparateParameter = gl.stencilOpSeparate(gl.FRONT, gl.INCR, gl.DECR, gl
			.INVERT);
		console.info("stencilOpSeparate stencilOpSeparateParameter: " + stencilOpSeparateParameter);
		expect(stencilOpSeparateParameter).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0575
	 * @tc.name testTexParameterfError
	 * @tc.desc Test texParameterf.
	 */
	it('testTexParameterfError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameterf test start ...' + JSON.stringify(gl));
		const texParameterValue = gl.texParameterf();
		console.info('texParameterValue' + texParameterValue);
		expect(texParameterValue).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0576
	 * @tc.name testTexParameteriError
	 * @tc.desc Test texParameteri.
	 */
	it('testTexParameteriError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL texParameteri test start ...' + JSON.stringify(gl));
		const texParameterValue = gl.texParameteri();
		console.info('texParameterfValue' + texParameterValue);
		expect(texParameterValue).assertEqual(undefined);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0577
	 * @tc.name testGetUniformBlockIndexError
	 * @tc.desc Test getUniformBlockIndex.
	 */
	it('testGetUniformBlockIndexError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getUniformBlockIndex test start ...' + JSON.stringify(gl2));
		const program = gl.createProgram();
		const blockIndex = gl2.getUniformBlockIndex(program);
		gl2.uniformBlockBinding(program, blockIndex, 1);
		console.info("uniformBlockBinding blockIndex: " + blockIndex);
		expect(blockIndex).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0578
	 * @tc.name testGetActiveUniformBlockParameterError
	 * @tc.desc Test getActiveUniformBlockParameter.
	 */
	it('testGetActiveUniformBlockParameterError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockParameter test start ...' + JSON.stringify(
			gl2));
		const program = globalFunction();
		const uniformLocation = gl.getUniformLocation(program, "a_Position");
		const int32Array = new Int32Array([1, 2]);
		gl2.uniform1iv(uniformLocation, int32Array);
		const blockIndex = gl2.getUniformBlockIndex(program, 'a_Position');
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockSize = gl2.getActiveUniformBlockParameter();
		console.info("getActiveUniformBlockParameter blockSize" + blockSize);
		expect(blockSize).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0579
	 * @tc.name testGetActiveUniformBlockNameError
	 * @tc.desc Test getActiveUniformBlockName.
	 */
	it('testGetActiveUniformBlockNameError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 getActiveUniformBlockName test start ...' + JSON.stringify(gl2));
		const program = gl.createProgram();
		const blockIndex = gl2.getUniformBlockIndex(program, 'UBOData');
		console.info("getActiveUniformBlockName blockIndex" + blockIndex);

		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockName = gl2.getActiveUniformBlockName(program);
		console.info("blockName" + blockName);
		expect(blockName).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0580
	 * @tc.name testUniformBlockBindingError
	 * @tc.desc Test uniformBlockBinding.
	 */
	it('testUniformBlockBindingError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 uniformBlockBinding test start ...' + JSON.stringify(gl2));
		const program = gl.createProgram();
		const blockIndex = gl2.getUniformBlockIndex(program);
		gl2.uniformBlockBinding(program, blockIndex, 1);
		const blockName = gl2.getActiveUniformBlockName(program);
		console.info("blockName" + blockName);

		expect(blockName).assertEqual(undefined);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0581
	 * @tc.name testCreateVertexArrayError
	 * @tc.desc Test createVertexArray.
	 */
	it('testCreateVertexArrayError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 createVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0582
	 * @tc.name testBindVertexArray
	 * @tc.desc Test bindVertexArray.
	 */
	it('testBindVertexArrayError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bindVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.bindVertexArray();
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0583
	 * @tc.name testDeleteVertexArray
	 * @tc.desc Test deleteVertexArray.
	 */
	it('testDeleteVertexArrayError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 deleteVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		gl2.bindVertexArray(vao);
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.deleteVertexArray(vao);
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(false);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0584
	 * @tc.name testIsVertexArrayError
	 * @tc.desc Test bindVertexArray.
	 */
	it('testIsVertexArray', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 isVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.bindVertexArray(vao);
		const isVertexArray = gl2.isVertexArray(vao);
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(true);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0585
	 * @tc.name testIsVertexArrayError
	 * @tc.desc Test bindVertexArray.
	 */
	it('testIsVertexArrayError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 isVertexArray test start ...' + JSON.stringify(gl2));
		const vao = gl2.createVertexArray();
		const isVertexArray1 = gl2.isVertexArray(vao);
		console.info("isVertexArray1" + isVertexArray1);
		gl2.bindVertexArray();
		const isVertexArray = gl2.isVertexArray();
		console.info("isVertexArray" + isVertexArray);
		expect(isVertexArray).assertEqual(false);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0586
	 * @tc.name testTexImage2DError
	 * @tc.desc Test texImage2D.
	 */
	it('testTexImage2DError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texImage2D test start ...' + JSON.stringify(gl2));
		gl2.texImage2D(-gl.TEXTURE_2D, -0, -32, -512, -512, -0, -32, -32, -new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test texImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(1281);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0587
	 * @tc.name testTexSubImage2DError
	 * @tc.desc Test texSubImage2D.
	 */
	it('testTexSubImage2DError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 texSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.texSubImage2D(-gl.TEXTURE_2D, -0, -0, -0, -512, -512, -gl.RED, gl.FLOAT, -
			new ArrayBuffer(16), -0);
		const errorCode = gl.getError();
		console.info("webgl2test texSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(1280);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0588
	 * @tc.name testCompressedTexImage2DError
	 * @tc.desc Test compressedTexImage2D.
	 */
	it('testCompressedTexImage2DError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexImage2D(-gl.TEXTURE_2D, -0, -0x83F3, -512, -512, -0, -gl
			.PIXEL_UNPACK_BUFFER, -0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(1280);
		done();
	});

	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0589
	 * @tc.name testCompressedTexSubImage2DError
	 * @tc.desc Test compressedTexSubImage2D.
	 */
	it('testCompressedTexSubImage2DError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 compressedTexSubImage2D test start ...' + JSON.stringify(gl2));
		gl2.compressedTexSubImage2D(-gl.TEXTURE_2D, -0, -256, -256, -512, -512, -0x83F3, -gl
			.PIXEL_UNPACK_BUFFER, 0);
		const errorCode = gl.getError();
		console.info("webgl2test compressedTexSubImage2D getError: " + errorCode);
		expect(errorCode).assertEqual(1281);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0590
	 * @tc.name testBufferSubDataError
	 * @tc.desc Test bufferData.
	 */
	it('testBufferSubDataError', 0, async function(done) {
		//initContext();
		console.info('jsWebGL2 bufferData test start ...' + JSON.stringify(gl2));
		gl2.bufferSubData(-gl.COPY_READ_BUFFER, -512, -new ArrayBuffer(8));
		const errorCode = gl.getError();
		console.info("webgl2test bufferSubData getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0591
	 * @tc.name webgl_test_getContextAttributes2
	 * @tc.desc Test getContextAttributes.
	 */
	it('webgl_test_getContextAttributes2', 0, async function(done) {
		//initContext();
		console.info("webgltest into getContextAttributes");
		const getContextAttributesValue = gl.getContextAttributes("error");
		console.info("webgltest getContextAttributes: " + getContextAttributesValue);
		expect(getContextAttributesValue).assertEqual(null);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0592
	 * @tc.name webgl_test_isContextLost2
	 * @tc.desc Test isContextLost.
	 */
	it('webgl_test_isContextLost2', 0, async function(done) {
		//initContext();
		console.info("webgltest into isContextLost");
		const isContextLostValue = gl.isContextLost("error");
		console.info("webgltest isContextLost: " + isContextLostValue);
		expect(isContextLostValue).assertEqual(false);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0593
	 * @tc.name webgl_test_getSupportedExtensions2
	 * @tc.desc Test getSupportedExtensions.
	 */
	it('webgl_test_getSupportedExtensions2', 0, async function(done) {
		//initContext();
		console.info("webgltest into getSupportedExtensions");
		const getSupportedExtensionsValue = gl.getSupportedExtensions("error");
		console.info("webgltest getSupportedExtensions: " + getSupportedExtensionsValue);
		expect(typeof(getSupportedExtensionsValue)).assertEqual('object');
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0595
	 * @tc.name webgl_test_getExtension2
	 * @tc.desc Test getExtension.
	 */
	it('webgl_test_getExtension2', 0, async function(done) {
		//initContext();
		console.info("webgltest into getExtension");
		const getExtensionValue = gl.getExtension("error");
		console.info("webgltest getExtension: " + getExtensionValue);
		expect(getExtensionValue).assertEqual(undefined);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0596
	 * @tc.name webgl_test_activeTexture2
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture2', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE8B4C);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33984);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0597
	 * @tc.name webgl_test_activeTexture_12
	 * @tc.desc Test activeTexture.
	 */
	it('webgl_test_activeTexture_12', 0, async function(done) {
		//initContext();
		console.info("webgltest into activeTexture");
		gl.activeTexture(gl.TEXTURE1);
		const activeTextureParameter = gl.getParameter(gl.ACTIVE_TEXTURE);
		console.info("webgltest activeTexture --> getParameter: " + activeTextureParameter);
		expect(activeTextureParameter).assertEqual(33985);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0598
	 * @tc.name webgl_test_attachShader2
	 * @tc.desc Test attachShader.
	 */
	it('webgl_test_attachShader2', 0, async function(done) {
		//initContext();
		console.info("webgltest into attachShader");

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
		gl.attachShader(programObj, 'vertexShader');
		gl.attachShader(programObj, 'fragmentShader');
		gl.linkProgram(programObj);
		gl.useProgram(programObj);

		let errorCode = gl.getError();
		console.info("webgltest attachShader getError: " + errorCode);
		expect(errorCode).assertEqual(gl.INVALID_OPERATION);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0599
	 * @tc.name webgl_test_bindAttribLocation2
	 * @tc.desc Test bindAttribLocation.
	 */
	it('webgl_test_bindAttribLocation2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindAttribLocation");

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

		gl.bindAttribLocation(programObj, 1, 'V_color');

		let errorCode = gl.getError();
		console.info("webgltest bindAttribLocation getError: " + errorCode);
		expect(errorCode).assertEqual(gl.NO_ERROR);
		//deleteContext();
		done();
	});


	/**
	 * @tc.number GRAPHIC_FUNCTION_JS_WEBGL_TESTWEBGL_0600
	 * @tc.name webgl_test_bindBuffer2
	 * @tc.desc Test bindBuffer.
	 */
	it('webgl_test_bindBuffer2', 0, async function(done) {
		//initContext();
		console.info("webgltest into bindBuffer");

		var buffer = gl.createBuffer();

		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);

		//bind后isBuffer为true
		const isBufferValue = gl.isBuffer(buffer);
		console.info("isBufferValue: " + isBufferValue);
		expect(isBufferValue).assertEqual(true);

		//deleteContext();
		done();
	});
})
