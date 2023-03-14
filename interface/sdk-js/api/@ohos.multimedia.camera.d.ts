/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {ErrorCallback, AsyncCallback} from './basic';
import { Context } from './app/context';

/**
 * @name camera
 * @syscap SystemCapability.Multimedia.Camera.Core
 * @import import camera from '@ohos.multimedia.camera';
 * @since 9
 */
declare namespace camera {

  /**
   * Creates a CameraManager instance.
   * @param context Current application context.
   * @param callback Callback used to return the CameraManager instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function getCameraManager(context: Context, callback: AsyncCallback<CameraManager>): void;

  /**
   * Creates a CameraManager instance.
   * @param context Current application context.
   * @return Promise used to return the CameraManager instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function getCameraManager(context: Context): Promise<CameraManager>;

  /**
   * Enum for camera status.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum CameraStatus {
    /**
     * Appear status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_STATUS_APPEAR = 0,
    /**
     * Disappear status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_STATUS_DISAPPEAR,
    /**
     * Available status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_STATUS_AVAILABLE,
    /**
     * Unavailable status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_STATUS_UNAVAILABLE
  }

  /**
   * Camera manager object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface CameraManager  {
    /**
     * Gets all camera descriptions.
     * @param callback Callback used to return the array of supported cameras.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getCameras(callback: AsyncCallback<Array<Camera>>): void;

    /**
     * Gets all camera descriptions.
     * @return Promise used to return an array of supported cameras.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getCameras(): Promise<Array<Camera>>;

    /**
     * Creates a CameraInput instance by camera id.
     * @param cameraId Camera ID used to create the instance.
     * @param callback Callback used to return the CameraInput instance.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     * @permission ohos.permission.CAMERA
     */
    createCameraInput(cameraId: string, callback: AsyncCallback<CameraInput>): void;

    /**
     * Creates a CameraInput instance by camera id.
     * @param cameraId Camera ID used to create the instance.
     * @return Promise used to return the CameraInput instance.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     * @permission ohos.permission.CAMERA
     */
    createCameraInput(cameraId: string): Promise<CameraInput>;

    /**
     * Creates a CameraInput instance by camera position and type.
     * @param position Target camera position.
     * @param type Target camera type.
     * @param callback Callback used to return the CameraInput instance.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     * @permission ohos.permission.CAMERA
     */
    createCameraInput(position: CameraPosition, type: CameraType, callback: AsyncCallback<CameraInput>): void;

    /**
     * Creates a CameraInput instance by camera position and type.
     * @param position Target camera position.
     * @param type Target camera type.
     * @return Promise used to return the CameraInput instance.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     * @permission ohos.permission.CAMERA
     */
    createCameraInput(position: CameraPosition, type: CameraType): Promise<CameraInput>;

    /**
     * Subscribes camera status change event callback.
     * @param type Event type.
     * @param callback Callback used to get the camera status change.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'cameraStatus', callback: AsyncCallback<CameraStatusInfo>): void;
  }

  /**
   * Camera status info.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface CameraStatusInfo {
    /**
     * Camera instance.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    camera: Camera;
    /**
     * Current camera status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    status: CameraStatus;
  }

  /**
   * Enum for camera position.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum CameraPosition {
    /**
     * Unspecified position.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_POSITION_UNSPECIFIED = 0,
    /**
     * Back position.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_POSITION_BACK,
    /**
     * Front position.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_POSITION_FRONT
  }

  /**
   * Enum for camera type.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum CameraType {
    /**
     * Unspecified camera type
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_TYPE_UNSPECIFIED = 0,

    /**
     * Wide camera
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_TYPE_WIDE_ANGLE,

    /**
     * Ultra wide camera
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_TYPE_ULTRA_WIDE,

    /**
     * Telephoto camera
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_TYPE_TELEPHOTO,

    /**
     * True depth camera
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_TYPE_TRUE_DEPTH
  }

  /**
   * Enum for camera connection type.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum ConnectionType {
    /**
     * Built-in camera.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_CONNECTION_BUILT_IN = 0,

    /**
     * Camera connected using USB
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_CONNECTION_USB_PLUGIN,

    /**
     * Remote camera
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    CAMERA_CONNECTION_REMOTE
  }

  /**
   * Camera object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface Camera {
    /**
     * Camera id attribute.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    readonly cameraId: string;
    /**
     * Camera position attribute.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    readonly cameraPosition: CameraPosition;
    /**
     * Camera type attribute.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    readonly cameraType: CameraType;
    /**
     * Camera connection type attribute.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    readonly connectionType: ConnectionType;
  }

  /**
   * Size parameter.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface Size {
    /**
     * Height.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    height: number;
    /**
     * Width.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    width: number;
  }

  /**
   * Camera input object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface CameraInput {
    /**
     * Gets camera id.
     * @param callback Callback used to return the camera ID.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getCameraId(callback: AsyncCallback<string>): void;

    /**
     * Gets camera id.
     * @return Promise used to return the camera ID.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getCameraId(): Promise<string>;

    /**
     * Check if device has flash light.
     * @param callback Callback used to return the flash light support status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    hasFlash(callback: AsyncCallback<boolean>): void;

    /**
     * Check if device has flash light.
     * @return Promise used to return the flash light support status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    hasFlash(): Promise<boolean>;

    /**
     * Checks whether a specified flash mode is supported.
     * @param flashMode Flash mode.
     * @param callback Callback used to return the flash light support status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    isFlashModeSupported(flashMode: FlashMode, callback: AsyncCallback<boolean>): void;

    /**
     * Checks whether a specified flash mode is supported.
     * @param flashMode Flash mode
     * @return Promise used to return flash mode support status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    isFlashModeSupported(flashMode: FlashMode): Promise<boolean>;

    /**
     * Gets current flash mode.
     * @param callback Callback used to return the current flash mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getFlashMode(callback: AsyncCallback<FlashMode>): void;

    /**
     * Gets current flash mode.
     * @return Promise used to return the flash mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getFlashMode(): Promise<FlashMode>;

    /**
     * Sets flash mode.
     * @param flashMode Target flash mode.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    setFlashMode(flashMode: FlashMode, callback: AsyncCallback<void>): void;

    /**
     * Sets flash mode.
     * @param flashMode Target flash mode.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    setFlashMode(flashMode: FlashMode): Promise<void>;

    /**
     * Checks whether a specified focus mode is supported.
     * @param afMode Focus mode.
     * @param callback Callback used to return the device focus support status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    isFocusModeSupported(afMode: FocusMode, callback: AsyncCallback<boolean>): void;

    /**
     * Checks whether a specified focus mode is supported.
     * @param afMode Focus mode.
     * @return Promise used to return the focus mode support status.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    isFocusModeSupported(afMode: FocusMode): Promise<boolean>;

    /**
     * Gets current focus mode.
     * @param callback Callback used to return the current focus mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getFocusMode(callback: AsyncCallback<FocusMode>): void;

    /**
     * Gets current focus mode.
     * @return Promise used to return the focus mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getFocusMode(): Promise<FocusMode>;

    /**
     * Sets focus mode.
     * @param afMode Target focus mode.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    setFocusMode(afMode: FocusMode, callback: AsyncCallback<void>): void;

    /**
     * Sets focus mode.
     * @param afMode Target focus mode.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    setFocusMode(afMode: FocusMode): Promise<void>;

    /**
     * Gets all supported zoom ratio range.
     * @param callback Callback used to return the zoom ratio range.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getZoomRatioRange(callback: AsyncCallback<Array<number>>): void;

    /**
     * Gets all supported zoom ratio range.
     * @return Promise used to return the zoom ratio range.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getZoomRatioRange(): Promise<Array<number>>;

    /**
     * Gets zoom ratio.
     * @param callback Callback used to return the current zoom ratio value.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getZoomRatio(callback: AsyncCallback<number>): void;

    /**
     * Gets zoom ratio.
     * @return Promise used to return the zoom ratio value.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    getZoomRatio(): Promise<number>;

    /**
     * Sets zoom ratio.
     * @param zoomRatio Target zoom ratio.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    setZoomRatio(zoomRatio: number, callback: AsyncCallback<void>): void;

    /**
     * Sets zoom ratio.
     * @param zoomRatio Target zoom ratio.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    setZoomRatio(zoomRatio: number): Promise<void>;

    /**
     * Releases instance.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Releases instance.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(): Promise<void>;

    /**
     * Subscribes focus status change event callback.
     * @param type Event type.
     * @param callback Callback used to get the focus state change.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'focusStateChange', callback: AsyncCallback<FocusState>): void;

    /**
     * Subscribes error event callback.
     * @param type Event type.
     * @param callback Callback used to get the camera input errors.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'error', callback: ErrorCallback<CameraInputError>): void;
  }

  /**
   * Enum for CameraInput error code.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum CameraInputErrorCode {
    /**
     * Unknown error.
     * @since 9
     */
    ERROR_UNKNOWN = -1
  }

  /**
   * Camera input error object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface CameraInputError extends Error {
    code: CameraInputErrorCode;
  }

  /**
   * Enum for flash mode.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum FlashMode {
    /**
     * Close mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FLASH_MODE_CLOSE = 0,
    /**
     * Open mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FLASH_MODE_OPEN,
    /**
     * Auto mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FLASH_MODE_AUTO,
    /**
     * Always open mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FLASH_MODE_ALWAYS_OPEN
  }

  /**
   * Enum for focus mode.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum FocusMode {
    /**
     * Manual mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FOCUS_MODE_MANUAL = 0,
    /**
     * Continuous auto mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FOCUS_MODE_CONTINUOUS_AUTO,
    /**
     * Auto mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FOCUS_MODE_AUTO,
    /**
     * Locked mode.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FOCUS_MODE_LOCKED
  }

  /**
   * Enum for focus state.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum FocusState {
    /**
     * Scan state.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FOCUS_STATE_SCAN = 0,
    /**
     * Focused state.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FOCUS_STATE_FOCUSED,
    /**
     * Unfocused state.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    FOCUS_STATE_UNFOCUSED
  }

  /**
   * Gets a CaptureSession instance.
   * @param context Current application context.
   * @param callback Callback used to return the CaptureSession instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function createCaptureSession(context: Context, callback: AsyncCallback<CaptureSession>): void;

  /**
   * Gets a CaptureSession instance.
   * @param context Current application context.
   * @return Promise used to return the CaptureSession instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function createCaptureSession(context: Context): Promise<CaptureSession>;

  /**
   * Capture session object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface CaptureSession {
    /**
     * Begin capture session config.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    beginConfig(callback: AsyncCallback<void>): void;

    /**
     * Begin capture session config.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    beginConfig(): Promise<void>;

    /**
     * Commit capture session config.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    commitConfig(callback: AsyncCallback<void>): void;

    /**
     * Commit capture session config.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    commitConfig(): Promise<void>;

    /**
     * Adds a camera input.
     * @param cameraInput Target camera input to add.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    addInput(cameraInput: CameraInput, callback: AsyncCallback<void>): void;

    /**
     * Adds a camera input.
     * @param cameraInput Target camera input to add.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    addInput(cameraInput: CameraInput): Promise<void>;

    /**
     * Adds a camera preview output.
     * @param previewOutput Target camera preview output to add.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    addOutput(previewOutput: PreviewOutput, callback: AsyncCallback<void>): void;

    /**
     * Adds a camera preview output.
     * @param previewOutput Target camera preview output to add.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    addOutput(previewOutput: PreviewOutput): Promise<void>;

    /**
     * Adds a camera photo output.
     * @param photoOutput Target camera photo output to add.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    addOutput(photoOutput: PhotoOutput, callback: AsyncCallback<void>): void;

    /**
     * Adds a camera photo output.
     * @param photoOutput Target camera photo output to add.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    addOutput(photoOutput: PhotoOutput): Promise<void>;

    /**
     * Adds a camera video output.
     * @param videoOutput Target camera video output to add.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    addOutput(videoOutput: VideoOutput, callback: AsyncCallback<void>): void;

    /**
     * Adds a camera video output.
     * @param videoOutput Target camera video output to add.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    addOutput(videoOutput: VideoOutput): Promise<void>;

    /**
     * Removes a camera input.
     * @param cameraInput Target camera input to remove.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    removeInput(cameraInput: CameraInput, callback: AsyncCallback<void>): void;

    /**
     * Removes a camera input.
     * @param cameraInput Target camera input to remove.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    removeInput(cameraInput: CameraInput): Promise<void>;

    /**
     * Removes a camera preview output.
     * @param previewOutput Target camera preview output to remove.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    removeOutput(previewOutput: PreviewOutput, callback: AsyncCallback<void>): void;

    /**
     * Removes a camera preview output.
     * @param previewOutput Target camera preview output to remove.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    removeOutput(previewOutput: PreviewOutput): Promise<void>;

    /**
     * Removes a camera photo output.
     * @param photoOutput Target camera photo output to remove.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    removeOutput(photoOutput: PhotoOutput, callback: AsyncCallback<void>): void;

    /**
     * Removes a camera photo output.
     * @param photoOutput Target camera photo output to remove.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    removeOutput(photoOutput: PhotoOutput): Promise<void>;

    /**
     * Removes a camera video output.
     * @param videoOutput Target camera video output to remove.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    removeOutput(videoOutput: VideoOutput, callback: AsyncCallback<void>): void;

    /**
     * Removes a camera video output.
     * @param videoOutput Target camera video output to remove.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    removeOutput(videoOutput: VideoOutput): Promise<void>;

    /**
     * Starts capture session.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    start(callback: AsyncCallback<void>): void;

    /**
     * Starts capture session.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    start(): Promise<void>;

    /**
     * Stops capture session.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    stop(callback: AsyncCallback<void>): void;

    /**
     * Stops capture session.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    stop(): Promise<void>;

    /**
     * Release capture session instance.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Release capture session instance.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(): Promise<void>;

    /**
     * Subscribes error event callback.
     * @param type Event type.
     * @param callback Callback used to get the capture session errors.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'error', callback: ErrorCallback<CaptureSessionError>): void;
  }

  /**
   * Enum for CaptureSession error code.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum CaptureSessionErrorCode {
    ERROR_UNKNOWN = -1
  }

  /**
   * Capture session error object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface CaptureSessionError extends Error {
    code: CaptureSessionErrorCode;
  }

  /**
   * Creates a PreviewOutput instance.
   * @param surfaceId Surface object id used in camera preview output.
   * @param callback Callback used to return the PreviewOutput instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function createPreviewOutput(surfaceId: string, callback: AsyncCallback<PreviewOutput>): void;

  /**
   * Creates a PreviewOutput instance.
   * @param surfaceId Surface object id used in camera preview output.
   * @return Promise used to return the PreviewOutput instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function createPreviewOutput(surfaceId: string): Promise<PreviewOutput>;

  /**
   * Preview output object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface PreviewOutput {
    /**
     * Release output instance.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Release output instance.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(): Promise<void>;

    /**
     * Subscribes frame start event callback.
     * @param type Event type.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'frameStart', callback: AsyncCallback<void>): void;

    /**
     * Subscribes frame end event callback.
     * @param type Event type.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'frameEnd', callback: AsyncCallback<void>): void;

    /**
     * Subscribes error event callback.
     * @param type Event type.
     * @param callback Callback used to get the preview output errors.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'error', callback: ErrorCallback<PreviewOutputError>): void;
  }

  /**
   * Enum for preview output error code.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum PreviewOutputErrorCode {
    ERROR_UNKNOWN = -1
  }

  /**
   * Preview output error object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface PreviewOutputError extends Error {
    code: PreviewOutputErrorCode;
  }

  /**
   * Creates a PhotoOutput instance.
   * @param surfaceId Surface object id used in camera photo output.
   * @param callback Callback used to return the PhotoOutput instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function createPhotoOutput(surfaceId: string, callback: AsyncCallback<PhotoOutput>): void;

  /**
   * Creates a PhotoOutput instance.
   * @param surfaceId Surface object id used in camera photo output.
   * @return Promise used to return the PhotoOutput instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function createPhotoOutput(surfaceId: string): Promise<PhotoOutput>;

  /**
   * Enumerates the image rotation angles.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum ImageRotation {
    /**
     * The capture image rotates 0 degrees.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    ROTATION_0 = 0,

    /**
     * The capture image rotates 90 degrees.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    ROTATION_90 = 90,

    /**
     * The capture image rotates 180 degrees.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    ROTATION_180 = 180,

    /**
     * The capture image rotates 270 degrees.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    ROTATION_270 = 270
  }

  /**
   * Enumerates the image quality levels.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum QualityLevel {
    /**
     * High image quality.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    QUALITY_LEVEL_HIGH = 0,

    /**
     * Medium image quality.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    QUALITY_LEVEL_MEDIUM,

    /**
     * Low image quality.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    QUALITY_LEVEL_LOW
  }

  /**
   * Photo capture options to set.
   * @since 9
   */
  interface PhotoCaptureSetting {
    /**
     * Photo image quality.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    quality?: QualityLevel;
    /**
     * Photo rotation.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    rotation?: ImageRotation;
  }

  /**
   * Photo output object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface PhotoOutput {
    /**
     * Start capture output.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    capture(callback: AsyncCallback<void>): void;

    /**
     * Start capture output.
     * @param setting Photo capture settings.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    capture(setting: PhotoCaptureSetting, callback: AsyncCallback<void>): void;

    /**
     * Start capture output.
     * @param setting Photo capture settings.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    capture(setting?: PhotoCaptureSetting): Promise<void>;

    /**
     * Release output instance.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Release output instance.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(): Promise<void>;

    /**
     * Subscribes capture start event callback.
     * @param type Event type.
     * @param callback Callback used to get the capture ID.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'captureStart', callback: AsyncCallback<number>): void;

    /**
     * Subscribes frame shutter event callback.
     * @param type Event type.
     * @param callback Callback used to get the frame shutter information.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'frameShutter', callback: AsyncCallback<FrameShutterInfo>): void;

    /**
     * Subscribes capture end event callback.
     * @param type Event type.
     * @param callback Callback used to get the capture end information.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'captureEnd', callback: AsyncCallback<CaptureEndInfo>): void;

    /**
     * Subscribes error event callback.
     * @param type Event type.
     * @param callback Callback used to get the photo output errors.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'error', callback: ErrorCallback<PhotoOutputError>): void;
  }

  /**
   * Frame shutter callback info.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface FrameShutterInfo {
    /**
     * Capture id.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    captureId: number;
    /**
     * Timestamp for frame.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    timestamp: number;
  }

  /**
   * Capture end info.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface CaptureEndInfo {
    /**
     * Capture id.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    captureId: number;
    /**
     * Frame count.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    frameCount: number;
  }

  /**
   * Enum for photo output error code.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum PhotoOutputErrorCode {
    ERROR_UNKNOWN = -1
  }

  /**
   * Photo output error object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface PhotoOutputError extends Error {
    code: PhotoOutputErrorCode;
  }

  /**
   * Creates a VideoOutput instance.
   * @param surfaceId Surface object id used in camera video output.
   * @param callback Callback used to return the VideoOutput instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function createVideoOutput(surfaceId: string, callback: AsyncCallback<VideoOutput>): void;

  /**
   * Creates a VideoOutput instance.
   * @param surfaceId Surface object id used in camera video output.
   * @return Promise used to return the VideoOutput instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  function createVideoOutput(surfaceId: string): Promise<VideoOutput>;

  /**
   * Video output object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface VideoOutput {
    /**
     * Start video output.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    start(callback: AsyncCallback<void>): void;

        /**
     * Start video output.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    start(): Promise<void>;

    /**
     * Stop video output.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    stop(callback: AsyncCallback<void>): void;

    /**
     * Stop video output.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    stop(): Promise<void>;

    /**
     * Release output instance.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Release output instance.
     * @return Promise used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    release(): Promise<void>;

    /**
     * Subscribes frame start event callback.
     * @param type Event type.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'frameStart', callback: AsyncCallback<void>): void;

    /**
     * Subscribes frame end event callback.
     * @param type Event type.
     * @param callback Callback used to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'frameEnd', callback: AsyncCallback<void>): void;

    /**
     * Subscribes error event callback.
     * @param type Event type.
     * @param callback Callback used to get the video output errors.
     * @since 9
     * @syscap SystemCapability.Multimedia.Camera.Core
     */
    on(type: 'error', callback: ErrorCallback<VideoOutputError>): void;
  }

  /**
   * Enum for video output error code.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  enum VideoOutputErrorCode {
    ERROR_UNKNOWN = -1
  }

  /**
   * Video output error object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Camera.Core
   */
  interface VideoOutputError extends Error {
    code: VideoOutputErrorCode;
  }
}

export default camera;
