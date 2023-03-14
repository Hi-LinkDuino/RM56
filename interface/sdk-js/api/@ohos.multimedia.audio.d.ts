/*
* Copyright (C) 2021 Huawei Device Co., Ltd.
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

import {ErrorCallback, AsyncCallback, Callback} from './basic';

/**
 * @name audio
 * @since 7
 * @import import audio from '@ohos.multimedia.audio'
 */
declare namespace audio {

  /**
   * Obtains an AudioManager instance.
   * @return AudioManager object.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  function getAudioManager(): AudioManager;

  /**
   * Obtains an AudioCapturer instance. This method uses an asynchronous callback to return the capturer instance.
   * @param options Capturer configurations.
   * @param callback Callback used to return the audio capturer instance.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Capturer
   */
  function createAudioCapturer(options: AudioCapturerOptions, callback: AsyncCallback<AudioCapturer>): void;

  /**
   * Obtains an AudioCapturer instance. This method uses a promise to return the capturer instance.
   * @param options Capturer configurations.
   * @return Promise used to return the audio capturer instance.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Capturer
   */
  function createAudioCapturer(options: AudioCapturerOptions): Promise<AudioCapturer>;

  /**
   * Obtains an AudioRenderer instance. This method uses an asynchronous callback to return the renderer instance.
   * @param options Renderer configurations.
   * @param callback Callback used to return the audio renderer instance.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  function createAudioRenderer(options: AudioRendererOptions, callback: AsyncCallback<AudioRenderer>): void;

  /**
   * Obtains an AudioRenderer instance. This method uses a promise to return the renderer instance.
   * @param options Renderer configurations.
   * @return Promise used to return the audio renderer instance.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  function createAudioRenderer(options: AudioRendererOptions): Promise<AudioRenderer>;

  /**
   * Enumerates the audio states.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  enum AudioState {
    /**
     * Invalid state.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STATE_INVALID = -1,
    /**
     * Create new instance state.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STATE_NEW,
    /**
     * Prepared state.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STATE_PREPARED,
    /**
     * Running state.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STATE_RUNNING,
    /**
     * Stopped state.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STATE_STOPPED,
    /**
     * Released state.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STATE_RELEASED,
    /**
     * Paused state.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STATE_PAUSED
  }

  /**
   * Enumerates audio stream types.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Volume
   */
  enum AudioVolumeType {
    /**
     * Audio streams for voice calls.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    VOICE_CALL = 0,
    /**
     * Audio streams for ringtones.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    RINGTONE = 2,
    /**
     * Audio streams for media purpose.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    MEDIA = 3,
    /**
     * Audio stream for voice assistant.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    VOICE_ASSISTANT = 9,
  }

  /**
   * Enumerates audio device flags.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Device
   */
  enum DeviceFlag {
    /**
     * Output devices.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    OUTPUT_DEVICES_FLAG = 1,
    /**
     * Input devices.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    INPUT_DEVICES_FLAG = 2,
    /**
     * All devices.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    ALL_DEVICES_FLAG = 3,
  }

  /**
   * Enumerates device roles.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Device
   */
  enum DeviceRole {
    /**
     * Input role.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    INPUT_DEVICE = 1,
    /**
     * Output role.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    OUTPUT_DEVICE = 2,
  }

  /**
   * Enumerates device types.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Device
   */
  enum DeviceType {
    /**
     * Invalid device.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    INVALID = 0,
    /**
     * Built-in earpiece.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    EARPIECE = 1,
    /**
     * Built-in speaker.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    SPEAKER = 2,
    /**
     * Wired headset, which is a combination of a pair of earpieces and a microphone.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    WIRED_HEADSET = 3,
    /**
     * A pair of wired headphones.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    WIRED_HEADPHONES = 4,
    /**
     * Bluetooth device using the synchronous connection oriented link (SCO).
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    BLUETOOTH_SCO = 7,
    /**
     * Bluetooth device using advanced audio distribution profile (A2DP).
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    BLUETOOTH_A2DP = 8,
    /**
     * Built-in microphone.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    MIC = 15,
    /**
     * USB audio headset.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    USB_HEADSET = 22,
  }

  /**
   * Enumerates the active device types.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Device
   */
   enum ActiveDeviceType {
    /**
     * Speaker.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    SPEAKER = 2,
    /**
     * Bluetooth device using the SCO link.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    BLUETOOTH_SCO = 7,
  }

  /**
   * Enumerates ringer modes.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Communication
   */
  enum AudioRingMode {
    /**
     * Silent mode.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Communication
     */
    RINGER_MODE_SILENT = 0,
    /**
     * Vibration mode.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Communication
     */
    RINGER_MODE_VIBRATE = 1,
    /**
     * Normal mode.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Communication
     */
    RINGER_MODE_NORMAL = 2,
  }

  /**
   * Enumerates the audio sample formats.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  enum AudioSampleFormat {
    /**
     * Invalid format.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_FORMAT_INVALID = -1,
    /**
     * Unsigned 8 format.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_FORMAT_U8 = 0,
    /**
     * Signed 16 bit integer, little endian.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_FORMAT_S16LE = 1,
    /**
     * Signed 24 bit integer, little endian.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_FORMAT_S24LE = 2,
    /**
     * Signed 32 bit integer, little endian.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_FORMAT_S32LE = 3,
  }

  /**
   * Enumerates the audio channel.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  enum AudioChannel {
    /**
     * Channel 1.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    CHANNEL_1 = 0x1 << 0,
    /**
     * Channel 2.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    CHANNEL_2 = 0x1 << 1
  }

  /**
   * Enumerates the audio sampling rate.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  enum AudioSamplingRate {
    /**
     * 8kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_8000 = 8000,
    /**
     * 11.025kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_11025 = 11025,
    /**
     * 12kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_12000 = 12000,
    /**
     * 16kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_16000 = 16000,
    /**
     * 22.05kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_22050 = 22050,
    /**
     * 24kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_24000 = 24000,
    /**
     * 32kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_32000 = 32000,
    /**
     * 44.1kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_44100 = 44100,
    /**
     * 48kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_48000 = 48000,
    /**
     * 64kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_64000 = 64000,
    /**
     * 96kHz sample rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SAMPLE_RATE_96000 = 96000
  }

  /**
   * Enumerates the audio encoding type.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  enum AudioEncodingType {
    /**
     * Invalid type.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    ENCODING_TYPE_INVALID = -1,
    /**
     * PCM encoding.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    ENCODING_TYPE_RAW = 0
  }

  /**
   * Enumerates the audio content type.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  enum ContentType {
    /**
     * Unknown content.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    CONTENT_TYPE_UNKNOWN = 0,
    /**
     * Speech content.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    CONTENT_TYPE_SPEECH = 1,
    /**
     * Music content.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    CONTENT_TYPE_MUSIC = 2,
    /**
     * Movie content.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    CONTENT_TYPE_MOVIE = 3,
    /**
     * Notification content.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    CONTENT_TYPE_SONIFICATION = 4,
    /**
     * Ringtone content.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    CONTENT_TYPE_RINGTONE = 5,
  }

  /**
   * Enumerates the stream usage.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  enum StreamUsage {
    /**
     * Unkown usage.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STREAM_USAGE_UNKNOWN = 0,
    /**
     * Media usage.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STREAM_USAGE_MEDIA = 1,
    /**
     * Voice communication usage.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STREAM_USAGE_VOICE_COMMUNICATION = 2,
    /**
     * Notification or ringtone usage.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    STREAM_USAGE_NOTIFICATION_RINGTONE = 6
  }

  /**
   * Describes audio stream information.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  interface AudioStreamInfo {
    /**
     * Sampling rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    samplingRate: AudioSamplingRate;
    /**
     * Audio channels.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    channels: AudioChannel;
    /**
     * Audio sample format.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    sampleFormat: AudioSampleFormat;
    /**
     * Audio encoding type.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    encodingType: AudioEncodingType;
  }

  /**
   * Describes audio renderer information.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  interface AudioRendererInfo {
    /**
     * Content type.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    content: ContentType;
    /**
     * Stream usage.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    usage: StreamUsage;
    /**
     * Audio renderer flags.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    rendererFlags: number;
  }

  /**
   * Describes audio renderer configuration options.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  interface AudioRendererOptions {
    /**
     * Stream information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    streamInfo: AudioStreamInfo;
    /**
     * Renderer information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    rendererInfo: AudioRendererInfo;
  }

  /**
   * Enumerates the audio renderer rates.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  enum AudioRendererRate {
    /**
     * Normal rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    RENDER_RATE_NORMAL = 0,
    /**
     * Double rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    RENDER_RATE_DOUBLE = 1,
    /**
     * Half rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    RENDER_RATE_HALF = 2
  }

  /**
   * Enumerates the interrupt types.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  enum InterruptType {
    /**
     * Audio playback interruption started.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_TYPE_BEGIN = 1,

    /**
     * Audio playback interruption ended.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_TYPE_END = 2
  }

  /**
   * Enumerates the interrupt hints.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  enum InterruptHint {
    /**
     * None.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_HINT_NONE = 0,
    /**
     * Resume the playback.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_HINT_RESUME = 1,

    /**
     * Paused/Pause the playback.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_HINT_PAUSE = 2,

    /**
     * Stopped/Stop the playback.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_HINT_STOP = 3,

    /**
     * Ducked the playback. (In ducking, the audio volume is reduced, but not silenced.)
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_HINT_DUCK = 4,

    /**
     * Unducked the playback.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_HINT_UNDUCK = 5,
  }

  /**
   * Enumerates the interrupt force types.
   * @since 9
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  enum InterruptForceType {
    /**
     * Forced action taken by system.
     * @since 9
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_FORCE = 0,
    /**
     * Share type, application can choose to take action or ignore.
     * @since 9
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    INTERRUPT_SHARE
  }

  /**
   * Describes the interrupt event received by the app when playback is interrupted.
   * @since 9
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  interface InterruptEvent {
    /**
     * Indicates whether the interruption has started or finished.
     * @since 9
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    eventType: InterruptType;

    /**
     * Indicates whether the action is taken by system or to be taken by the app.
     * @since 9
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    forceType: InterruptForceType;

    /**
     * Indicates the kind of action.
     * @since 9
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    hintType: InterruptHint;
  }

  /**
   * Enumerates interrupt action types.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  enum InterruptActionType {

    /**
     * Focus gain event.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    TYPE_ACTIVATED = 0,

    /**
     * Audio interruption event.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    TYPE_INTERRUPT = 1
  }

  /**
   * Enumerates device change types.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Device
   */
  enum DeviceChangeType {
    /**
     * Device connection.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    CONNECT = 0,

    /**
     * Device disconnection.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    DISCONNECT = 1,
  }

  /**
   * Enumerates audio scenes.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Communication
   */
  enum AudioScene {
    /**
     * Default audio scene
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Communication
     */
    AUDIO_SCENE_DEFAULT = 0,
    /**
     * Ringing audio scene
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Communication
     * @systemapi
     */
    AUDIO_SCENE_RINGING,
    /**
     * Phone call audio scene
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Communication
     * @systemapi
     */
    AUDIO_SCENE_PHONE_CALL,
    /**
     * Voice chat audio scene
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Communication
     */
    AUDIO_SCENE_VOICE_CHAT
  }

  /**
   * Implements audio volume and audio device management.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  interface AudioManager {
    /**
     * Sets the volume for a stream. This method uses an asynchronous callback to return the result.
     * @param volumeType Audio stream type.
     * @param volume Volume to set. The value range can be obtained by calling getMinVolume and getMaxVolume.
     * @param callback Callback used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     * @permission ohos.permission.ACCESS_NOTIFICATION_POLICY
     */
    setVolume(volumeType: AudioVolumeType, volume: number, callback: AsyncCallback<void>): void;
    /**
     * Sets the volume for a stream. This method uses a promise to return the result.
     * @param volumeType Audio stream type.
     * @param volume Volume to set. The value range can be obtained by calling getMinVolume and getMaxVolume.
     * @return Promise used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     * @permission ohos.permission.ACCESS_NOTIFICATION_POLICY
     */
    setVolume(volumeType: AudioVolumeType, volume: number): Promise<void>;
    /**
     * Obtains the volume of a stream. This method uses an asynchronous callback to return the query result.
     * @param volumeType Audio stream type.
     * @param callback Callback used to return the volume.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    getVolume(volumeType: AudioVolumeType, callback: AsyncCallback<number>): void;
    /**
     * Obtains the volume of a stream. This method uses a promise to return the query result.
     * @param volumeType Audio stream type.
     * @return Promise used to return the volume.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    getVolume(volumeType: AudioVolumeType): Promise<number>;
    /**
     * Obtains the minimum volume allowed for a stream. This method uses an asynchronous callback to return the query result.
     * @param volumeType Audio stream type.
     * @param callback Callback used to return the minimum volume.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    getMinVolume(volumeType: AudioVolumeType, callback: AsyncCallback<number>): void;
    /**
     * Obtains the minimum volume allowed for a stream. This method uses a promise to return the query result.
     * @param volumeType Audio stream type.
     * @return Promise used to return the minimum volume.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    getMinVolume(volumeType: AudioVolumeType): Promise<number>;
    /**
     * Obtains the maximum volume allowed for a stream. This method uses an asynchronous callback to return the query result.
     * @param volumeType Audio stream type.
     * @param callback Callback used to return the maximum volume.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    getMaxVolume(volumeType: AudioVolumeType, callback: AsyncCallback<number>): void;
    /**
     * Obtains the maximum volume allowed for a stream. This method uses a promise to return the query result.
     * @param volumeType Audio stream type.
     * @return Promise used to return the maximum volume.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    getMaxVolume(volumeType: AudioVolumeType): Promise<number>;
    /**
     * Obtains the audio devices with a specific flag. This method uses an asynchronous callback to return the query result.
     * @param deviceFlag Audio device flag.
     * @param callback Callback used to return the device list.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    getDevices(deviceFlag: DeviceFlag, callback: AsyncCallback<AudioDeviceDescriptors>): void;
    /**
     * Obtains the audio devices with a specific flag. This method uses a promise to return the query result.
     * @param deviceFlag Audio device flag.
     * @return Promise used to return the device list.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    getDevices(deviceFlag: DeviceFlag): Promise<AudioDeviceDescriptors>;
    /**
     * Mutes a stream. This method uses an asynchronous callback to return the result.
     * @param volumeType Audio stream type.
     * @param mute Mute status to set. The value true means to mute the stream, and false means the opposite.
     * @param callback Callback used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    mute(volumeType: AudioVolumeType, mute: boolean, callback: AsyncCallback<void>): void;
    /**
     * Mutes a stream. This method uses a promise to return the result.
     * @param volumeType Audio stream type.
     * @param mute Mute status to set. The value true means to mute the stream, and false means the opposite.
     * @return Promise used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    mute(volumeType: AudioVolumeType, mute: boolean): Promise<void>;
    /**
     * Checks whether a stream is muted. This method uses an asynchronous callback to return the query result.
     * @param volumeType Audio stream type.
     * @param callback Callback used to return the mute status of the stream. The value true means that the stream is
     * muted, and false means the opposite.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    isMute(volumeType: AudioVolumeType, callback: AsyncCallback<boolean>): void;
    /**
     * Checks whether a stream is muted. This method uses a promise to return the result.
     * @param volumeType Audio stream type.
     * @return Promise used to return the mute status of the stream. The value true means that the stream is muted,
     * and false means the opposite.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    isMute(volumeType: AudioVolumeType): Promise<boolean>;
    /**
     * Checks whether a stream is active. This method uses an asynchronous callback to return the query result.
     * @param volumeType Audio stream type.
     * @param callback Callback used to return the active status of the stream. The value true means that the stream is
     * active, and false means the opposite.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    isActive(volumeType: AudioVolumeType, callback: AsyncCallback<boolean>): void;
    /**
     * Checks whether a stream is active. This method uses a promise to return the query result.
     * @param volumeType Audio stream type.
     * @return Promise used to return the active status of the stream. The value true means that the stream is active,
     * and false means the opposite.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    isActive(volumeType: AudioVolumeType): Promise<boolean>;
    /**
     * Mutes or unmutes the microphone. This method uses an asynchronous callback to return the result.
     * @param mute Mute status to set. The value true means to mute the microphone, and false means the opposite.
     * @param callback Callback used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     * @permission ohos.permission.MICROPHONE
     */
    setMicrophoneMute(mute: boolean, callback: AsyncCallback<void>): void;
    /**
     * Mutes or unmutes the microphone. This method uses a promise to return the result.
     * @param mute Mute status to set. The value true means to mute the microphone, and false means the opposite.
     * @return Promise used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     * @permission ohos.permission.MICROPHONE
     */
    setMicrophoneMute(mute: boolean): Promise<void>;
    /**
     * Checks whether the microphone is muted. This method uses an asynchronous callback to return the query result.
     * @param Callback used to return the mute status of the microphone. The value true means that the microphone is
     * muted, and false means the opposite.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     * @permission ohos.permission.MICROPHONE
     */
    isMicrophoneMute(callback: AsyncCallback<boolean>): void;
    /**
     * Checks whether the microphone is muted. This method uses a promise to return the query result.
     * @return Promise used to return the mute status of the microphone. The value true means that the microphone is
     * muted, and false means the opposite.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     * @permission ohos.permission.MICROPHONE
     */
    isMicrophoneMute(): Promise<boolean>;
    /**
     * Sets the ringer mode. This method uses an asynchronous callback to return the result.
     * @param mode Ringer mode.
     * @param callback Callback used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Communication
     * @permission ohos.permission.ACCESS_NOTIFICATION_POLICY
     */
    setRingerMode(mode: AudioRingMode, callback: AsyncCallback<void>): void;
    /**
     * Sets the ringer mode. This method uses a promise to return the result.
     * @param mode Ringer mode.
     * @return Promise used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Communication
     * @permission ohos.permission.ACCESS_NOTIFICATION_POLICY
     */
    setRingerMode(mode: AudioRingMode): Promise<void>;
    /**
     * Obtains the ringer mode. This method uses an asynchronous callback to return the query result.
     * @param callback Callback used to return the ringer mode.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Communication
     */
    getRingerMode(callback: AsyncCallback<AudioRingMode>): void;
    /**
     * Obtains the ringer mode. This method uses a promise to return the query result.
     * @return Promise used to return the ringer mode.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Communication
     */
    getRingerMode(): Promise<AudioRingMode>;
    /**
     * Sets an audio parameter. This method uses an asynchronous callback to return the result.
     * @param key Key of the audio parameter to set.
     * @param value Value of the audio parameter to set.
     * @param callback Callback used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     * @permission ohos.permission.MODIFY_AUDIO_SETTINGS
     */
    setAudioParameter(key: string, value: string, callback: AsyncCallback<void>): void;
    /**
     * Sets an audio parameter. This method uses a promise to return the result.
     * @param key Key of the audio parameter to set.
     * @param value Value of the audio parameter to set.
     * @return Promise used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     * @permission ohos.permission.MODIFY_AUDIO_SETTINGS
     */
    setAudioParameter(key: string, value: string): Promise<void>;
    /**
     * Obtains the value of an audio parameter. This method uses an asynchronous callback to return the query result.
     * @param key Key of the audio parameter whose value is to be obtained.
     * @param callback Callback used to return the value of the audio parameter.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    getAudioParameter(key: string, callback: AsyncCallback<string>): void;
    /**
     * Obtains the value of an audio parameter. This method uses a promise to return the query result.
     * @param key Key of the audio parameter whose value is to be obtained.
     * @return Promise used to return the value of the audio parameter.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    getAudioParameter(key: string): Promise<string>;
    /**
     * Sets a device to the active state. This method uses an asynchronous callback to return the result.
     * @param deviceType Audio device type.
     * @param active Active status to set. The value true means to set the device to the active status, and false
     * means the opposite.
     * @param callback Callback used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    setDeviceActive(deviceType: ActiveDeviceType, active: boolean, callback: AsyncCallback<void>): void;
    /**
     * Sets a device to the active state. This method uses a promise to return the result.
     * @param deviceType Audio device type.
     * @param active Active status to set. The value true means to set the device to the active status, and false
     * means the opposite.
     * @return Promise used to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    setDeviceActive(deviceType: ActiveDeviceType, active: boolean): Promise<void>;
    /**
     * Checks whether a device is active. This method uses an asynchronous callback to return the query result.
     * @param deviceType Audio device type.
     * @param callback Callback used to return the active status of the device.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    isDeviceActive(deviceType: ActiveDeviceType, callback: AsyncCallback<boolean>): void;
    /**
     * Checks whether a device is active. This method uses a promise to return the query result.
     * @param deviceType Audio device type.
     * @return Promise used to return the active status of the device.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    isDeviceActive(deviceType: ActiveDeviceType): Promise<boolean>;
    /**
     * Listens for system volume change events. This method uses a callback to get volume change events.
     * @param callback Callback used to get the system volume change event.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Volume
     * @systemapi
     */
    on(type: 'volumeChange', callback: Callback<VolumeEvent>): void;
    /**
     * Listens for ringer mode change events. This method uses a callback to get ringer mode changes.
     * @param callback Callback used to get the updated ringer mode.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Communication
     * @systemapi
     */
    on(type: 'ringerModeChange', callback: Callback<AudioRingMode>): void;
    /**
     * Sets the audio scene mode to change audio strategies. This method uses an asynchronous callback to return the
     * result.
     * @param scene Audio scene mode.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Communication
     * @systemapi
     */
    setAudioScene(scene: AudioScene, callback: AsyncCallback<void> ): void;
    /**
     * Sets the audio scene mode to change audio strategies. This method uses a promise to return the result.
     * @param scene Audio scene mode.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Communication
     * @systemapi
     */
    setAudioScene(scene: AudioScene): Promise<void>;
    /**
     * Obtains the audio scene mode. This method uses an asynchronous callback to return the query result.
     * @param callback Callback used to return the audio scene mode.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Communication
     */
    getAudioScene(callback: AsyncCallback<AudioScene>): void;
    /**
     * Obtains the audio scene mode. This method uses a promise to return the query result.
     * @return Promise used to return the audio scene mode.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Communication
     */
    getAudioScene(): Promise<AudioScene>;
    /**
     * Subscribes to device change events. When a device is connected/disconnected, registered clients will receive
     * the callback.
     * @param callback Callback used to obtain the device update details.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    on(type: 'deviceChange', callback: Callback<DeviceChangeAction>): void;

    /**
     * UnSubscribes to device change events.
     * @param callback Callback used to obtain the device update details.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    off(type: 'deviceChange', callback?: Callback<DeviceChangeAction>): void;

    /**
     * Listens for audio interruption events. When the audio of an application is interrupted by another application,
     * the callback is invoked to notify the former application.
     * @param type Type of the event to listen for. Only the interrupt event is supported.
     * @param interrupt Parameters of the audio interruption event type.
     * @param callback Callback invoked for the audio interruption event.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    on(type: 'interrupt', interrupt: AudioInterrupt, callback: Callback<InterruptAction>): void;

    /**
     * Cancels the listening of audio interruption events.
     * @param type Type of the event to listen for. Only the interrupt event is supported.
     * @param interrupt Input parameters of the audio interruption event.
     * @param callback Callback invoked for the audio interruption event.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    off(type: 'interrupt', interrupt: AudioInterrupt, callback?: Callback<InterruptAction>): void;
  }

  /**
   * Describes an audio device.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Device
   */
  interface AudioDeviceDescriptor {
    /**
     * Audio device role.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    readonly deviceRole: DeviceRole;
    /**
     * Audio device type.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    readonly deviceType: DeviceType;
  }

  /**
   * Array of AudioDeviceDescriptors, which is read-only.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Device
   */
  type AudioDeviceDescriptors = Array<Readonly<AudioDeviceDescriptor>>;

  /**
   * Describes the volume event received by the app when the volume is changed.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Volume
   * @systemapi
   */
  interface VolumeEvent {
    /**
     * Volume type of the current stream.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    volumeType: AudioVolumeType;
    /**
     * Volume level.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    volume: number;
    /**
     * Whether to show the volume change in UI.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Volume
     */
    updateUi: boolean;
  }

  /**
   * Describes the callback invoked for audio interruption or focus gain events.When the audio of an application
   * is interrupted by another application, the callback is invoked to notify the former application.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  interface InterruptAction {

    /**
     * Event type.
     * The value TYPE_ACTIVATED means the focus gain event, and TYPE_INTERRUPT means the audio interruption event.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    actionType: InterruptActionType;

    /**
     * Type of the audio interruption event.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    type?: InterruptType;

    /**
     * Hint for the audio interruption event.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    hint?: InterruptHint;

    /**
     * Whether the focus is gained or released. The value true means that the focus is gained or released,
     * and false means that the focus fails to be gained or released.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    activated?: boolean;
  }

  /**
   * Describes input parameters of audio listening events.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  interface AudioInterrupt {

    /**
     * Audio stream usage type.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    streamUsage: StreamUsage;

    /**
     * Type of the media interrupted.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    contentType: ContentType;

    /**
     * Whether audio playback can be paused when it is interrupted.
     * The value true means that audio playback can be paused when it is interrupted, and false means the opposite.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    pauseWhenDucked: boolean;
  }

  /**
   * Describes the device change type and device information.
   * @since 7
   * @syscap SystemCapability.Multimedia.Audio.Device
   */
  interface DeviceChangeAction {
    /**
     * Device change type.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    type: DeviceChangeType;

    /**
     * Device information.
     * @since 7
     * @syscap SystemCapability.Multimedia.Audio.Device
     */
    deviceDescriptors: AudioDeviceDescriptors;
  }

  /**
   * Provides audio playback APIs.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Renderer
   */
  interface AudioRenderer {
    /**
     * Defines the current render state.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    readonly state: AudioState;
    /**
     * Obtains the renderer information provided while creating a renderer instance. This method uses an asynchronous
     * callback to return the result.
     * @param callback Callback used to return the renderer information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getRendererInfo(callback: AsyncCallback<AudioRendererInfo>): void;
    /**
     * Obtains the renderer information provided while creating a renderer instance. This method uses a promise to
     * return the result.
     * @return Promise used to return the renderer information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getRendererInfo(): Promise<AudioRendererInfo>;
    /**
     * Obtains the renderer stream information. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the stream information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getStreamInfo(callback: AsyncCallback<AudioStreamInfo>): void;
    /**
     * Obtains the renderer stream information. This method uses a promise to return the result.
     * @return Promise used to return the stream information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getStreamInfo(): Promise<AudioStreamInfo>;
    /**
     * Starts the renderer. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    start(callback: AsyncCallback<void>): void;
    /**
     * Starts the renderer. This method uses a promise to return the result.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    start(): Promise<void>;
    /**
     * Writes the buffer. This method uses an asynchronous callback to return the result.
     * @param buffer Buffer to be written.
     * @param callback Returns the number of bytes written if the operation is successful; returns an error code
     * otherwise.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    write(buffer: ArrayBuffer, callback: AsyncCallback<number>): void;
    /**
     * Writes the buffer. This method uses a promise to return the result.
     * @param buffer Buffer to be written.
     * @return Returns the number of bytes written if the operation is successful; returns an error code otherwise.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    write(buffer: ArrayBuffer): Promise<number>;
    /**
     * Obtains the timestamp in Unix epoch time (starts from January 1, 1970), in nanoseconds. This method uses an
     * asynchronous callback to return the result.
     * @param callback Callback used to return the timestamp.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getAudioTime(callback: AsyncCallback<number>): void;
    /**
     * Obtains the timestamp in Unix epoch time (starts from January 1, 1970), in nanoseconds. This method uses a
     * promise to return the result.
     * @return Promise used to return the timestamp.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getAudioTime(): Promise<number>;
    /**
     * Drains the playback buffer. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    drain(callback: AsyncCallback<void>): void;
    /**
     * Drains the playback buffer. This method uses a promise to return the result.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    drain(): Promise<void>;
    /**
     * Pauses rendering. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    pause(callback: AsyncCallback<void>): void;
    /**
     * Pauses rendering. This method uses a promise to return the result.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    pause(): Promise<void>;
    /**
     * Stops rendering. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    stop(callback: AsyncCallback<void>): void;
    /**
     * Stops rendering. This method uses a promise to return the result.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    stop(): Promise<void>;
    /**
     * Releases the renderer. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    release(callback: AsyncCallback<void>): void;
    /**
     * Releases the renderer. This method uses a promise to return the result.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    release(): Promise<void>;
    /**
     * Obtains a reasonable minimum buffer size in bytes for rendering. This method uses an asynchronous callback to
     * return the result.
     * @param callback Callback used to return the buffer size.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getBufferSize(callback: AsyncCallback<number>): void;
    /**
     * Obtains a reasonable minimum buffer size in bytes for rendering. This method uses a promise to return the result.
     * @return Promise used to return the buffer size.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getBufferSize(): Promise<number>;
    /**
     * Sets the render rate. This method uses an asynchronous callback to return the result.
     * @param rate Audio render rate.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    setRenderRate(rate: AudioRendererRate, callback: AsyncCallback<void>): void;
    /**
     * Sets the render rate. This method uses a promise to return the result.
     * @param rate Audio render rate.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    setRenderRate(rate: AudioRendererRate): Promise<void>;
    /**
     * Obtains the current render rate. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the audio render rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getRenderRate(callback: AsyncCallback<AudioRendererRate>): void;
    /**
     * Obtains the current render rate. This method uses a promise to return the result.
     * @return Promise used to return the audio render rate.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    getRenderRate(): Promise<AudioRendererRate>;
    /**
     * Listens for audio interrupt events. This method uses a callback to get interrupt events. The interrupt event is
     * triggered when audio playback is interrupted.
     * @param callback Callback used to listen for interrupt callback.
     * @since 9
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    on(type: 'interrupt', callback: Callback<InterruptEvent>): void;
    /**
     * Subscribes to mark reached events. When the number of frames rendered reaches the value of the frame parameter,
     * the callback is invoked.
     * @param frame Number of frames to trigger the event. The value must be greater than 0.
     * @param callback Callback invoked when the event is triggered.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    on(type: "markReach", frame: number, callback: (position: number) => {}): void;
    /**
     * Unsubscribes from mark reached events.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    off(type: "markReach"): void;
    /**
     * Subscribes to period reached events. When the period of frame rendering reaches the value of frame parameter,
     * the callback is invoked.
     * @param frame Period during which frame rendering is listened. The value must be greater than 0.
     * @param callback Callback invoked when the event is triggered.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    on(type: "periodReach", frame: number, callback: (position: number) => {}): void;
    /**
     * Unsubscribes from period reached events.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    off(type: "periodReach"): void;
    /**
     * Subscribes audio state change event callback.
     * @param callback Callback invoked when state change.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Renderer
     */
    on(type: "stateChange", callback: Callback<AudioState>): void;
  }

  /**
   * Enumerates source types.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  enum SourceType {
    /**
     * Invalid source type.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SOURCE_TYPE_INVALID = -1,
    /**
     * Mic source type.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SOURCE_TYPE_MIC = 0,
    /**
     * Voice communication source type.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    SOURCE_TYPE_VOICE_COMMUNICATION = 7
  }

  /**
   * Describes audio capturer information.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Core
   */
  interface AudioCapturerInfo {
    /**
     * Audio source type.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    source: SourceType;
    /**
     * Audio capturer flags.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Core
     */
    capturerFlags: number;
  }

  /**
   * Describes audio capturer configuration options.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Capturer
   */
  interface AudioCapturerOptions {
    /**
     * Stream information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    streamInfo: AudioStreamInfo;
    /**
     * Capturer information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    capturerInfo: AudioCapturerInfo;
  }

  /**
   * Provides APIs for audio recording.
   * @since 8
   * @syscap SystemCapability.Multimedia.Audio.Capturer
   */
  interface AudioCapturer {
    /**
     * Defines the current capture state.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    readonly state: AudioState;
    /**
     * Obtains the capturer information provided while creating a capturer instance. This method uses an asynchronous
     * callback to return the result.
     * @param callback Callback used to return the capturer information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    getCapturerInfo(callback: AsyncCallback<AudioCapturerInfo>): void;
    /**
     * Obtains the capturer information provided while creating a capturer instance. This method uses a promise to
     * return the result.
     * @return Promise used to return the capturer information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    getCapturerInfo(): Promise<AudioCapturerInfo>;

    /**
     * Obtains the capturer stream information. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the stream information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    getStreamInfo(callback: AsyncCallback<AudioStreamInfo>): void;
    /**
     * Obtains the capturer stream information. This method uses a promise to return the result.
     * @return Promise used to return the stream information.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    getStreamInfo(): Promise<AudioStreamInfo>;

    /**
     * Starts capturing. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    start(callback: AsyncCallback<void>): void;
    /**
     * Starts capturing. This method uses a promise to return the result.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    start(): Promise<void>;

    /**
     * Reads the buffer from the audio capturer. This method uses an asynchronous callback to return the result.
     * @param size Number of bytes to read.
     * @param isBlockingRead Whether the read operation should be blocked.
     * @param callback Callback used to return the buffer.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    read(size: number, isBlockingRead: boolean, callback: AsyncCallback<ArrayBuffer>): void;
    /**
     * Reads the buffer from the audio capturer. This method uses a promise to return the result.
     * @param size Number of bytes to read.
     * @param isBlockingRead Whether the read operation should be blocked.
     * @return Returns the buffer data read if the operation is successful; returns an error code otherwise.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    read(size: number, isBlockingRead: boolean): Promise<ArrayBuffer>;

    /**
     * Obtains the timestamp in Unix epoch time (starts from January 1, 1970), in nanoseconds. This method uses an
     * asynchronous callback to return the result.
     * @param callback Callback used to return the timestamp.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    getAudioTime(callback: AsyncCallback<number>): void;
    /**
     * Obtains the timestamp in Unix epoch time (starts from January 1, 1970), in nanoseconds. This method uses a
     * promise to return the result.
     * @return Promise used to return the timestamp.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    getAudioTime(): Promise<number>;

    /**
     * Stops capturing. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    stop(callback: AsyncCallback<void>): void;
    /**
     * Stops capturing. This method uses a promise to return the result.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    stop(): Promise<void>;

    /**
     * Releases the capturer. This method uses an asynchronous callback to return the result.
     * @param callback Callback used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    release(callback: AsyncCallback<void>): void;
    /**
     * Releases the capturer. This method uses a promise to return the result.
     * @return Promise used to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    release(): Promise<void>;

    /**
     * Obtains a reasonable minimum buffer size in bytes for capturing. This method uses an asynchronous callback to
     * return the result.
     * @param callback Callback used to return the buffer size.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    getBufferSize(callback: AsyncCallback<number>): void;
    /**
     * Obtains a reasonable minimum buffer size in bytes for capturing. This method uses a promise to return the result.
     * @return Promise used to return the buffer size.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    getBufferSize(): Promise<number>;

    /**
     * Subscribes to mark reached events. When the number of frames captured reaches the value of the frame parameter,
     * the callback is invoked.
     * @param frame Number of frames to trigger the event. The value must be greater than 0.
     * @param callback Callback invoked when the event is triggered.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    on(type: "markReach", frame: number, callback: (position: number) => {}): void;
    /**
     * Unsubscribes from the mark reached events.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    off(type: "markReach"): void;

    /**
     * Subscribes to period reached events. When the period of frame capturing reaches the value of frame parameter,
     * the callback is invoked.
     * @param frame Period during which frame capturing is listened. The value must be greater than 0.
     * @param callback Callback invoked when the event is triggered.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    on(type: "periodReach", frame: number, callback: (position: number) => {}): void;
    /**
     * Unsubscribes from period reached events.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    off(type: "periodReach"): void;

    /**
     * Subscribes audio state change event callback.
     * @param callback Callback used to listen for the audio state change event.
     * @since 8
     * @syscap SystemCapability.Multimedia.Audio.Capturer
     */
    on(type: "stateChange", callback: Callback<AudioState>): void;
  }
}

export default audio;
