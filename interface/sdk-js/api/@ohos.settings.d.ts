/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
import { AsyncCallback } from './basic';
import { DataAbilityHelper } from './ability/dataAbilityHelper';

/**
 * This module provides settings data access abilities.
 *
 * @since 7
 * @syscap SystemCapability.Applications.settings.Core
 * @import import settings from '@ohos.settings'
 * @permission N/A
 */
declare namespace settings {
  /**
   * Provides methods for setting time and date formats.
   *
   * @since 7
   */
  namespace date {
    /**
     * Indicates the date format.
     *
     * <p>The formats {@code mm/dd/yyyy}, {@code dd/mm/yyyy}, and {@code yyyy/mm/dd} are available.
     *
     * @since 7
     */
    const DATE_FORMAT: string

    /**
     * Specifies whether the time is displayed in 12-hour or 24-hour format.
     *
     * <p>If the value is {@code 12}, the 12-hour format is used. If the value is {@code 24}, the 24-hour format
     * is used.
     *
     * @since 7
     */
    const TIME_FORMAT: string

    /**
     * Specifies whether the date, time, and time zone are automatically obtained from the Network
     * Identity and Time Zone (NITZ).
     *
     * <p>If the value is {@code true}, the information is automatically obtained from NITZ.
     * If the value is {@code false}, the information is not obtained from NITZ.
     *
     * @since 7
     */
    const AUTO_GAIN_TIME: string

    /**
     * Specifies whether the time zone is automatically obtained from NITZ.
     *
     * <p>If the value is {@code true}, the information is automatically obtained from NITZ. If the value
     * is {@code false}, the information is not obtained from NITZ.
     *
     * @since 7
     */
    const AUTO_GAIN_TIME_ZONE: string
  }

    /**
     * Provides methods for setting the display effect, including the font size, screen brightness, screen rotation,
     * animation factor, and display color.
     *
     * @since 7
     */
  namespace display {
    /**
     * Indicates the scaling factor of fonts, which is a float number.
     *
     * @since 7
     */
    const FONT_SCALE: string

    /**
     * Indicates the screen brightness. The value ranges from 0 to 255.
     *
     * @since 7
     */
    const SCREEN_BRIGHTNESS_STATUS: string

    /**
     * Specifies whether automatic screen brightness adjustment is enabled.
     *
     * <p>If the value is {@code 1}, automatic adjustment is enabled. If the value is {@code 0}, automatic
     * adjustment is disabled.
     *
     * @since 7
     */
    const AUTO_SCREEN_BRIGHTNESS: string

    /**
     * Indicates the value of {@code AUTO_SCREEN_BRIGHTNESS} when automatic screen brightness adjustment is used.
     *
     * @since 7
     */
    const AUTO_SCREEN_BRIGHTNESS_MODE: number

    /**
     * Indicates the value of {@code AUTO_SCREEN_BRIGHTNESS} when manual screen brightness adjustment is used.
     *
     * @since 7
     */
    const MANUAL_SCREEN_BRIGHTNESS_MODE: number

    /**
     * Indicates the duration that the device waits before going to sleep after a period of inactivity, in
     * milliseconds.
     *
     * @since 7
     */
    const SCREEN_OFF_TIMEOUT: string

    /**
     * Indicates the screen rotation when no other policy is available.
     *
     * <p>This constant is invalid when auto-rotation is enabled. When auto-rotation is disabled, the following
     * values are available:
     *
     * <ul>
     * <li>{@code 0} - The screen rotates 0 degrees.
     * <li>{@code 1} - The screen rotates 90 degrees.
     * <li>{@code 2} - The screen rotates 180 degrees.
     * <li>{@code 3} - The screen rotates 270 degrees.
     * </ul>
     *
     * @since 7
     */
    const DEFAULT_SCREEN_ROTATION: string

    /**
     * Indicates the scaling factor for the animation duration.
     *
     * <p>This affects the start delay and duration of all such animations. If the value is {@code 0},
     * the animation ends immediately. The default value is {@code 1}.
     *
     * @since 7
     */
    const ANIMATOR_DURATION_SCALE: string

    /**
     * Indicates the scaling factor for transition animations.
     * If the value is {@code 0}, transition animations are disabled.
     *
     * @since 7
     */
    const TRANSITION_ANIMATION_SCALE: string

    /**
     * Indicates the scaling factor for normal window animations.
     * If the value is {@code 0}, window animations are disabled.
     *
     * @since 7
     */
    const WINDOW_ANIMATION_SCALE: string

    /**
     * Specifies whether display color inversion is enabled.
     *
     * <p>If the value is {@code 1}, display color inversion is enabled. If the value is {@code 0}, display color
     * inversion is disabled.
     *
     * @since 7
     */
    const DISPLAY_INVERSION_STATUS: string
  }

  /**
   * Provides methods for setting general information about devices, including the device name, startup wizard,
   * airplane mode, debugging information, accessibility feature switch, and touch exploration status.
   *
   * @since 7
   */
  namespace general {
    /**
     * Specifies whether the startup wizard has been run.
     *
     * <p>If the value is {@code 0}, the startup wizard has not been run. If the value is not {@code 0}, the startup
     * wizard has been run.
     *
     * @since 7
     */
    const SETUP_WIZARD_FINISHED: string

    /**
     * Specifies what happens after the user presses the call end button if the user is not in a call.
     *
     * <ul>
     * <li>{@code 0} - Nothing happens.
     * <li>{@code 1} - The home screen is displayed.
     * <li>{@code 2} - The device enters the sleep state and the screen is locked.
     * <li>{@code 3} - The home screen is displayed. If the user is already on the home screen, the device enters
     * the sleep state.
     * </ul>
     *
     * @since 7
     */
    const END_BUTTON_ACTION: string

    /**
     * Specifies whether the accelerometer is used to change screen orientation, that is, whether auto-rotation is
     * enabled.
     *
     * <p>The value {@code 1} indicates that the accelerometer is enabled by default, and {@code 0} indicates that
     * the accelerometer is disabled by default.
     *
     * @since 7
     */
    const ACCELEROMETER_ROTATION_STATUS: string

    /**
     * Specifies whether airplane mode is enabled.
     *
     * <p>If the value is {@code 1}, airplane mode is enabled. If the value is {@code 0}, airplane mode is disabled.
     *
     * @since 7
     */
    const AIRPLANE_MODE_STATUS: string

    /**
     * Specifies whether the device is provisioned.
     *
     * <p>On a multi-user device with a single system user, the screen may be locked when the value is {@code true}.
     * In addition, other abilities cannot be started on the system user unless they are marked to display over
     * the screen lock.
     *
     * @since 7
     */
    const DEVICE_PROVISION_STATUS: string

    /**
     * Specifies whether the hard disk controller (HDC) on USB devices is enabled.
     *
     * <p>If the value is {@code true}, the HDC is enabled. If the value is {@code false}, the HDC is disabled.
     *
     * @since 7
     */
    const HDC_STATUS: string

    /**
     * Indicates the number of boot operations after the device is powered on.
     *
     * @since 7
     */
    const BOOT_COUNTING: string

    /**
     * Specifies whether contact metadata synchronization is enabled.
     *
     * <p>If the value is {@code true}, synchronization is enabled. If the value is {@code false},
     * synchronization is disabled.
     *
     * @since 7
     */
    const CONTACT_METADATA_SYNC_STATUS: string

    /**
     * Specifies whether developer options are enabled.
     *
     * <p>If the value is {@code true}, developer options are enabled.
     * If the value is {@code false}, developer options are disabled.
     *
     * @since 7
     */
    const DEVELOPMENT_SETTINGS_STATUS: string

    /**
     * Indicates the device name.
     *
     * @since 7
     */
    const DEVICE_NAME: string

    /**
     * Specifies whether USB mass storage is enabled.
     *
     * <p>If the value is {@code true}, USB mass storage is enabled.
     * If the value is {@code false}, USB mass storage is disabled.
     *
     * @since 7
     */
    const USB_STORAGE_STATUS: string

    /**
     * Specifies whether the device waits for the debugger when starting an application to debug.
     *
     * <p>If the value is {@code 1}, the device waits for the debugger.
     * If the value is {@code 0}, the system does not wait for the debugger, and so the application runs normally.
     *
     * @since 7
     */
    const DEBUGGER_WAITING: string

    /**
     * Indicates the bundle name of the application to debug.
     *
     * @since 7
     */
    const DEBUG_APP_PACKAGE: string

    /**
     * Specifies whether any accessibility feature is enabled.
     *
     * <p>If the value is {@code 1}, the accessibility feature is enabled. If the value is {@code 0}, the
     * accessibility feature is disabled.
     *
     * @since 7
     */
    const ACCESSIBILITY_STATUS: string

    /**
     * Indicates the list of accessibility features that have been activated.
     *
     * @since 7
     */
    const ACTIVATED_ACCESSIBILITY_SERVICES: string

    /**
     * Indicates the default geographical location that can be used by the browser. Multiple geographical locations
     * are separated by spaces.
     *
     * @since 7
     */
    const GEOLOCATION_ORIGINS_ALLOWED: string

    /**
     * Specifies whether an application should attempt to skip all introductory hints at the first startup. This is
     * intended for temporary users or users who are familiar with the environment.
     *
     * <p>If the value is {@code 1}, the application attempts to skip all introductory hints at the first startup.
     * If the value is {@code 0}, the application does not skip introductory hints at the first startup.
     *
     * @since 7
     */
    const SKIP_USE_HINTS: string

    /**
     * Indicates whether touch exploration is enabled.
     *
     * <p>If the value is {@code 1}, touch exploration is enabled. If the value is {@code 0}, touch exploration is
     * disabled.
     *
     * @since 7
     */
    const TOUCH_EXPLORATION_STATUS: string
  }

  /**
   * Provides methods for setting information about input methods, including automatic capitalization, automatic
   * punctuation, autocorrect, password presentation, input method engine, and input method subtypes.
   *
   * @since 7
   */
  namespace input {
    /**
     * Indicates the default input method and its ID.
     *
     * @since 7
     */
    const DEFAULT_INPUT_METHOD: string

    /**
     * Indicates the default input method keyboard type and its ID.
     *
     * @since 7
     */
    const ACTIVATED_INPUT_METHOD_SUB_MODE: string

    /**
     * Indicates the list of input methods that have been activated.
     *
     * <p>The list is a string that contains the IDs of activated input methods. The IDs are separated by colons
     * (:), and keyboardTypes of an input method are separated by semicolons (;). An example format is
     * {@code ima0:keyboardType0;keyboardType1;ima1:ima2:keyboardTypes0}. The type of <b>imaID</b> is ElementName,
     * and the type of <b>keyboard</b> is int.
     *
     * @since 7
     */
    const ACTIVATED_INPUT_METHODS: string

    /**
     * Specifies whether the input method selector is visible.
     *
     * <p>If the value is {@code 1}, the input method selector is visible. If the value is {@code 0}, the input
     * method selector is invisible.
     *
     * @since 7
     */
    const SELECTOR_VISIBILITY_FOR_INPUT_METHOD: string

    /**
     * Specifies whether automatic capitalization is enabled for the text editor.
     *
     * <p>If the value is {@code 0}, automatic capitalization is disabled. If the value {@code 1}, automatic
     * capitalization is enabled.
     *
     * @since 7
     */
    const AUTO_CAPS_TEXT_INPUT: string

    /**
     * Specifies whether automatic punctuation is enabled for the text editor. Automatic punctuation enables the
     * text editor to convert two spaces into a period (.) and a space.
     *
     * <p>If the value is {@code 0}, automatic punctuation is disabled. If the value {@code 1}, automatic
     * punctuation is enabled.
     *
     * @since 7
     */
    const AUTO_PUNCTUATE_TEXT_INPUT: string

    /**
     * Specifies whether autocorrect is enabled for the text editor. Autocorrect enables the text editor to correct
     * typos.
     *
     * <p>If the value is {@code 0}, autocorrect is disabled. If the value {@code 1}, autocorrect is enabled.
     *
     * @since 7
     */
    const AUTO_REPLACE_TEXT_INPUT: string

    /**
     * Specifies whether password presentation is enabled in the text editor. Password presentation enables the
     * text editor to show password characters when the user types them.
     *
     * <p>If the value is {@code 0}, password presentation is disabled. If the value {@code 1}, password
     * presentation is enabled.
     *
     * @since 7
     */
    const SHOW_PASSWORD_TEXT_INPUT: string
  }

  /**
   * Provides methods for setting network information, including the data roaming status, HTTP proxy configurations,
   * and preferred networks.
   *
   * @since 7
   */
  namespace network {
    /**
     * Specifies whether data roaming is enabled.
     *
     * <p>If the value is {@code true}, data roaming is enabled. If the value is {@code false},
     * data roaming is disabled.
     *
     * @since 7
     */
    const DATA_ROAMING_STATUS: string

    /**
     * Indicates the host name and port number of the global HTTP proxy.
     * The host name and port number are separated by a colon (:).
     *
     * @since 7
     */
    const HTTP_PROXY_CFG: string

    /**
     * Indicates the user preferences of the network to use.
     *
     * @since 7
     */
    const NETWORK_PREFERENCE_USAGE: string
  }

  /**
   * Provides methods for setting the answering mode of incoming and outgoing calls.
   *
   * @since 7
   */
  namespace phone {
    /**
     * Specifies whether real-time text (RTT) calling is enabled. If enabled, incoming and outgoing calls are
     * answered as RTT calls when supported by the device and carrier. If the value is {@code 1}, RTT calling is
     * enabled. If the value is {@code 0}, RTT calling is disabled.
     *
     * @since 7
     */
    const RTT_CALLING_STATUS: string
  }

  /**
   * Provides methods for setting the sound effect, including the ringtone, dial tone, alarm sound, notification tone,
   * and haptic feedback.
   *
   * @since 7
   */
  namespace sound {
    /**
     * Indicates whether the device vibrates when it is ringing for an incoming call.
     *
     * <p>This constant will be used by Phone and Settings applications. The value is of the boolean type.
     * This constant affects only the scenario where the device rings for an incoming call. It does not affect
     * any other application or scenario.
     *
     * @since 7
     */
    const VIBRATE_WHILE_RINGING: string

    /**
     * Indicates the storage area of the system default alarm.
     *
     * <p>You can obtain the URI of the system default alarm.
     *
     * @since 7
     */
    const DEFAULT_ALARM_ALERT: string

    /**
     * Indicates the type of the dual-tone multifrequency (DTMF) tone played when dialing.
     *
     * <p>The value {@code 0} indicates the normal short sound effect, and {@code 1} indicates the long sound
     * effect.
     *
     * @since 7
     */
    const DTMF_TONE_TYPE_WHILE_DIALING: string

    /**
     * Specifies whether the DTMF tone is played when dialing.
     *
     * <p>If the value is {@code 1}, the DTMF tone is played. If the value is {@code 0}, the DTMF tone is not
     * played.
     *
     * @since 7
     */
    const DTMF_TONE_WHILE_DIALING: string

    /**
     * Specifies which audio streams are affected by changes on the ringing mode and Do Not Disturb (DND) mode.
     *
     * <p>If you want a specific audio stream to be affected by changes of the ringing mode and DDN mode, set the
     * corresponding bit to {@code 1}.
     *
     * @since 7
     */
    const AFFECTED_MODE_RINGER_STREAMS: string

    /**
     * Specifies which audio streams are affected by the mute mode.
     *
     * <p>If you want a specific audio stream to remain muted in mute mode, set the corresponding bit to {@code 1}.
     *
     * @since 7
     */
    const AFFECTED_MUTE_STREAMS: string

    /**
     * Indicates the storage area of the system default notification tone.
     *
     * <p>You can obtain the URI of the system default notification tone.
     *
     * @since 7
     */
    const DEFAULT_NOTIFICATION_SOUND: string

    /**
     * Indicates the storage area of the system default ringtone.
     *
     * <p>You can obtain the URI of the system default ringtone.
     *
     * @since 7
     */
    const DEFAULT_RINGTONE: string

    /**
     * Specifies whether the sound effects are enabled.
     *
     * <p>If the value is {@code 0}, the sound effects are disabled. If the value is {@code 1}, the sound effects
     * are enabled.
     *
     * @since 7
     */
    const SOUND_EFFECTS_STATUS: string

    /**
     * Specifies whether the device vibrates for an event. This parameter is used inside the system.
     *
     * <p>If the value is {@code 1}, the device vibrates for an event. If the value is {@code 0}, the device does
     * not vibrate for an event.
     *
     * @since 7
     */
    const VIBRATE_STATUS: string

    /**
     * Indicates whether the device enables haptic feedback.
     *
     * <p>The value is of the boolean type.
     *
     * @since 7
     */
    const HAPTIC_FEEDBACK_STATUS: string
  }

  /**
   * Provides methods for setting information about text-to-speech (TTS) conversion, including the pitch, speech rate,
   * engine, and plug-ins.
   *
   * @since 7
   */
  namespace TTS {
    /**
     * Indicates the default pitch of the text-to-speech (TTS) engine.
     *
     * <p>100 = 1x. If the value is set to {@code 200}, the frequency is twice the normal sound frequency.
     *
     * @since 7
     */
    const DEFAULT_TTS_PITCH: string

    /**
     * Indicates the default speech rate of the TTS engine. 100 = 1x.
     *
     * @since 7
     */
    const DEFAULT_TTS_RATE: string

    /**
     * Indicates the default TTS engine.
     *
     * @since 7
     */
    const DEFAULT_TTS_SYNTH: string

    /**
     * Indicates the list of activated plug-in packages used for TTS. Multiple plug-in packages are separated by
     * spaces.
     *
     * @since 7
     */
    const ENABLED_TTS_PLUGINS: string
  }

  /**
   * Provides methods for setting radio network information, including information about Bluetooth, Wi-Fi, Near Field
   * Communication (NFC), and the airplane mode.
   *
   * @since 7
   */
  namespace wireless {
  /**
     * Specifies whether the device can be discovered or connected by other devices through Bluetooth.
     *
     * If the value is {@code 0}, the device cannot be connected or discovered. If the value is {@code 1}, the
     * device can be connected but cannot be discovered. If the value is {@code 2}, the device can be connected
     * and discovered.
     *
     * @since 7
     */
    const BLUETOOTH_DISCOVER_ABILITY_STATUS: string

    /**
     * Indicates the duration (in seconds) that the device can be discovered through Bluetooth.
     *
     * <p>After the duration expires, the device cannot be discovered through Bluetooth.
     *
     * @since 7
     */
    const BLUETOOTH_DISCOVER_TIMEOUT: string

    /**
     * Indicates the list of radio signals to be disabled when airplane mode is enabled. Multiple radio
     * signals are separated by commas (,).
     *
     * <ul>
     * <li>{@code BLUETOOTH_RADIO} - Bluetooth is disabled in airplane mode.
     * <li>{@code CELL_RADIO} - Cellular radio is disabled in airplane mode.
     * <li>{@code NFC_RADIO} - NFC is disabled in airplane mode.
     * <li>{@code WIFI_RADIO} - Wi-Fi is disabled in airplane mode.
     * </ul>
     *
     * @since 7
     */
    const AIRPLANE_MODE_RADIOS: string

    /**
     * Specifies whether Bluetooth is enabled.
     *
     * <p>If the value is {@code true}, Bluetooth is enabled. If the value is {@code false}, Bluetooth is disabled.
     *
     * @since 7
     */
    const BLUETOOTH_STATUS: string

    /**
     * A constant of {@code AIRPLANE_MODE_RADIOS} to indicate that Bluetooth is disabled in airplane mode.
     *
     * @since 7
     */
    const BLUETOOTH_RADIO: string

    /**
     * A constant of {@code AIRPLANE_MODE_RADIOS} to indicate that cellular radio is disabled in airplane mode.
     *
     * @since 7
     */
    const CELL_RADIO: string

    /**
     * A constant of {@code AIRPLANE_MODE_RADIOS} to indicate that NFC is disabled in airplane mode.
     *
     * @since 7
     */
    const NFC_RADIO: string

    /**
     * A constant of {@code AIRPLANE_MODE_RADIOS} to indicate that Wi-Fi is disabled in airplane mode.
     *
     * @since 7
     */
    const WIFI_RADIO: string

    /**
     * Specifies whether the Wi-Fi configuration created by the application of the device owner should be
     * locked down.
     *
     * <p>If the value is {@code true}, the Wi-Fi configuration should be locked down.
     * If the value is {@code false}, the Wi-Fi configuration should not be locked down.
     *
     * @since 7
     */
    const OWNER_LOCKDOWN_WIFI_CFG: string

    /**
     * Indicates the maximum number of attempts to obtain an IP address from the DHCP server.
     *
     * @since 7
     */
    const WIFI_DHCP_MAX_RETRY_COUNT: string

    /**
     * Indicates the maximum duration to hold a wake lock when waiting for the mobile data connection to
     * establish after the Wi-Fi connection is disconnected.
     *
     * @since 7
     */
    const WIFI_TO_MOBILE_DATA_AWAKE_TIMEOUT: string

    /**
     * Specifies whether Wi-Fi is enabled.
     *
     * <p>If the value is {@code true}, Wi-Fi is enabled. If the value is {@code false}, Wi-Fi is disabled.
     *
     * @since 7
     */
    const WIFI_STATUS: string

    /**
     * Specifies whether Wi-Fi watchdog is enabled.
     *
     * <p>If the value is {@code true}, Wi-Fi watchdog is enabled.
     * If the value is {@code false}, Wi-Fi watchdog is disabled.
     *
     * @since 7
     */
    const WIFI_WATCHDOG_STATUS: string
  }

  /**
   * Constructs a URI for a specific name-value pair for monitoring data of the ability that uses the Data
   * template.
   *
   * @param name Indicates the name of the setting to set.
   * @return Returns the corresponding URI; returns {@code null} if the URI does not exist.
   * @since 7
   */
  function getURI(name: string, callback: AsyncCallback<object>): void;
  function getURI(name: string): Promise<object>;

  /**
   * Obtains the value of a specified character string in the database.
   *
   * @param dataAbilityHelper Indicates the {@link ohos.aafwk.ability.DataAbilityHelper} used to access
   * the database.
   * @param name Indicates the name of the character string.
   * @return Returns the value of the character string in the table if any is found; returns {@code null}
   * otherwise.
   * @since 7
   */
  function getValue(dataAbilityHelper: DataAbilityHelper, name: string, callback: AsyncCallback<object>): void;
  function getValue(dataAbilityHelper: DataAbilityHelper, name: string): Promise<object>;

  /**
   * Saves a character string name and its value to the database.
   *
   * @param dataAbilityHelper Indicates the {@link ohos.aafwk.ability.DataAbilityHelper} used to access
   * the database.
   * @param name Indicates the name of the character string.
   * @param value Indicates the value of the character string.
   * @return Returns {@code true} if the operation is successful; returns {@code false} otherwise.
   * @since 7
   * @systemapi Hide this for inner system use.
   */
  function setValue(dataAbilityHelper: DataAbilityHelper, name: string, value: object, callback: AsyncCallback<boolean>): void;
  function setValue(dataAbilityHelper: DataAbilityHelper, name: string, value: object): Promise<boolean>;

  /**
   * Enables or disables airplane mode.
   *
   * @param enable Specifies whether to enable airplane mode. The value {@code true} means to enable airplane
   * mode, and {@code false} means to disable airplane mode.
   * @return Returns {@code true} if the operation is successful; returns {@code false} otherwise.
   * @since 7
   */
  function enableAirplaneMode(enable: boolean, callback: AsyncCallback<void>): void;
  function enableAirplaneMode(enable: boolean): Promise<void>;

  /**
   * Checks whether a specified application can show as float window.
   *
   * @param context Indicates the application context.
   * @return Returns {@code true} if the application can draw over other applications; returns {@code false}
   * otherwise.
   * @since 7
   */
  function canShowFloating(callback: AsyncCallback<boolean>): void;
  function canShowFloating(): Promise<boolean>;

  /**
   * get settingsdata uri(synchronization method)
   * @since 8
   * @param name Indicates the name of the setting to set.
   * @return Return settingsdata uri.
   */
   function getUriSync(name: string): string;

   /**
    * get value from settingsdata(synchronization method)
    * @since 8
    * @param dataAbilityHelper Indicates dataAbilityHelper instance
    * @param name Indicates the name of the character string.
    * @param defValue Indicates the default value of the character string.
    * @return settingsdata value
    */
   function getValueSync(dataAbilityHelper: DataAbilityHelper, name: string, defValue: string): string;
 
   /**
    * set settingsdata value(synchronization method)
    * @need permission ohos.permission.WRITE_SYSTEM_SETTING
    * @since 8
    * @param dataAbilityHelper Indicates dataAbilityHelper instance
    * @param name Indicates the name of the character string.
    * @param value Indicates the value of the character string.
    * @return Returns {@code true} if the operation is successful; returns {@code false} otherwise.
    */
   function setValueSync(dataAbilityHelper: DataAbilityHelper, name: string, value: string): boolean;
}

export default settings;