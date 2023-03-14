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

import { AsyncCallback } from './basic';
import { Callback } from './basic';

/**
 * Accessibility
 * @name Accessibility
 * @since 7
 * @syscap SystemCapability.BarrierFree.Accessibility.Core
 * @import basic,abilityInfo
 */
declare namespace accessibility {

  /**
   * The type of the Ability app.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type AbilityType = 'audible' | 'generic' | 'haptic' | 'spoken' | 'visual';

  /**
   * The action that the ability can execute.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type Action = 'accessibilityFocus' | 'clearAccessibilityFocus' | 'focus' | 'clearFocus' | 'clearSelection' |
    'click' | 'longClick' | 'cut' | 'copy' | 'paste' | 'select' | 'setText' | 'delete' |
    'scrollForward' | 'scrollBackward' | 'setSelection';

  /**
   * The type of the accessibility event.
   * @note windowsChange
   * @note windowContentChange
   * @note windowStateChange
   * @note announcement
   * @note notificationChange
   * @note textTraversedAtMove
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type EventType = 'accessibilityFocus' | 'accessibilityFocusClear' |
    'click' | 'longClick' | 'focus' | 'select' | 'hoverEnter' | 'hoverExit' |
    'textUpdate' | 'textSelectionUpdate' | 'scroll';

  /**
   * The change type of the windowsChange event.
   * @note It's used when received the {@code windowsChange} event.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type WindowUpdateType = 'add' | 'remove' | 'bounds' | 'active' | 'focus';

  /**
   * The type of the ability state.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type AbilityState = 'enable' | 'disable' | 'install';

  /**
   * The ability that accessibility subsystem support.
   * @note touchExplorer: Describes the capability to talkback.
   * magnification: Describes the capability to request to control the display magnification.
   * gesturesSimulation: Describes the capability to request to simulate the gesture.
   * windowContent: Describes the capability to search for the content of the active window.
   * filterKeyEvents: Describes the capability to request to filter key events.
   * fingerprintGesture: Describes the capability to request to fingerprint gesture.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type Capability = 'retrieve' | 'touchGuide' | 'keyEventObserver' | 'zoom' | 'gesture';

  /**
   * The granularity of text move.
   * @note The granularity of text move.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @since 7
   */
  type TextMoveUnit = 'char' | 'word' | 'line' | 'page' | 'paragraph';

  /**
   * Checks whether accessibility ability is enabled.
   * @since 7
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns {@code true} if the accessibility is enabled; returns {@code false} otherwise.
  */
  function isOpenAccessibility(callback: AsyncCallback<boolean>): void;
  function isOpenAccessibility(): Promise<boolean>;

  /**
   * Checks touch browser ability (which is used by talkback) is enabled.
   * @since 7
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Vision
   * @return Returns {@code true} if the touch browser is enabled; returns {@code false} otherwise.
  */
  function isOpenTouchGuide(callback: AsyncCallback<boolean>): void;
  function isOpenTouchGuide(): Promise<boolean>;

  /**
   * Queries the list of accessibility abilities.
   * @since 7
   * @param abilityType The type of the accessibility ability. {@code AbilityType} eg.spoken
   * @param stateType The state of the accessibility ability.  {@code AbilityState} eg.installed
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns the list of abilityInfos.
  */
  function getAbilityLists(abilityType: AbilityType, stateType: AbilityState,
    callback: AsyncCallback<Array<AccessibilityAbilityInfo>>): void;
  function getAbilityLists(abilityType: AbilityType,
    stateType: AbilityState): Promise<Array<AccessibilityAbilityInfo>>;

  /**
   * Send accessibility Event.
   * @since 7
   * @param event The object of the accessibility {@code EventInfo} .
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns {@code true} if success ; returns {@code false} otherwise.
   */
  function sendEvent(event: EventInfo, callback: AsyncCallback<void>): void;
  function sendEvent(event: EventInfo): Promise<void>;

  /**
   * Register the observe of the accessibility state changed.
   * @since 7
   * @param type state event type.
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns {@code true} if the register is success ; returns {@code false} otherwise.
   */
  function on(type: 'accessibilityStateChange', callback: Callback<boolean>): void;

  /**
   * Register the observe of the touchGuide state changed.
   * @since 7
   * @param type state event type.
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Vision
   * @return Returns {@code true} if the register is success ; returns {@code false} otherwise.
   */
  function on(type: 'touchGuideStateChange', callback: Callback<boolean>): void;

  /**
   * Deregister the observe of the accessibility state changed.
   * @since 7
   * @param type state event type
   * @param callback Asynchronous callback interface.
   * @syscap SystemCapability.BarrierFree.Accessibility.Core
   * @return Returns {@code true} if the deregister is success ; returns {@code false} otherwise.
   */
  function off(type: 'accessibilityStateChange', callback?: Callback<boolean>): void;

  /**
   * Deregister the observe of the touchGuide state changed.
   * @since 7
   * @param type state event type
   * @param callback Asynchronous callback interface.
   * @return Returns {@code true} if the deregister is success ; returns {@code false} otherwise.
   */
   function off(type: 'touchGuideStateChange', callback?: Callback<boolean>): void;

  /**
   * Get the captions manager.
   * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
   * @since 8
   * @return Returns the captions manager.
   */
   function getCaptionsManager(): CaptionsManager;

   /**
    * Indicates the captions manager.
    * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
    * @since 8
    */
   interface CaptionsManager {
     /**
      * Indicates whether captions are enabled.
      */
     enabled: boolean;
     /**
      * Indicates the style of captions.
      */
     style: CaptionsStyle;
 
     /**
      * Register the observe of the enable state.
      */
     on(type: 'enableChange', callback: Callback<boolean>): void;
     /**
      * Register the observer of the style.
      */
     on(type: 'styleChange', callback: Callback<CaptionsStyle>): void;
     /**
      * Deregister the observe of the enable state.
      */
     off(type: 'enableChange', callback?: Callback<boolean>): void;
     /**
      * Deregister the observer of the style.
      */
     off(type: 'styleChange', callback?: Callback<CaptionsStyle>): void;
   }
 
   /**
    * Indicates the edge type of the captions font.
    * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
    * @since 8
    */
   type CaptionsFontEdgeType = 'none' | 'raised' | 'depressed' | 'uniform' | 'dropShadow';
   /**
    * Indicates the font family of captions.
    * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
    * @since 8
    */
   type CaptionsFontFamily = 'default' | 'monospacedSerif' | 'serif' |
     'monospacedSansSerif' | 'sansSerif' | 'casual' | 'cursive' | 'smallCapitals';
   /**
    * Indicates the style of captions.
    * @syscap SystemCapability.BarrierFree.Accessibility.Hearing
    * @since 8
    */
   interface CaptionsStyle {
     /**
      * Indicates the font family of captions.
      */
     fontFamily: CaptionsFontFamily;
     /**
      * Indicates the font scaling of captions.
      */
     fontScale: number;
     /**
      * Indicates the font color of captions.
      */
     fontColor: number | string;
     /**
      * Indicates the edge type of the captions font.
      */
     fontEdgeType: CaptionsFontEdgeType;
     /**
      * Indicates the background color of captions.
      */
     backgroundColor: number | string;
     /**
      * Indicates the window color of captions.
      */
     windowColor: number | string;
   }

   /**
    * Indicates the info of accessibility.
    * @syscap SystemCapability.BarrierFree.Accessibility.Core
    * @since 7
    */
  interface AccessibilityAbilityInfo {
    /**
     * The ability id.
     * @since 7
     */
    readonly id: string;

    /* The ability name.
     * @since 7
     */
    readonly name: string;

    /* The bundle name of the ability.
     * @since 7
     */
    readonly bundleName: string;

    /**
     * The type of the ability.
     * @since 7
     */
    readonly abilityTypes: Array<AbilityType>;

    /**
     * The capabilities of the ability.
     * @since 7
     */
    readonly capabilities: Array<Capability>;

    /**
     * The description of the ability.
     * @since 7
     */
    readonly description: string;

    /**
     * The events which the accessibility ability wants to observe.
     * @since 7
     */
    readonly eventTypes: Array<EventType>;

  }

  /**
    * Indicates the info of events.
    * @syscap SystemCapability.BarrierFree.Accessibility.Core
    * @since 7
    */
  class EventInfo {
    constructor(jsonObject);
    /**
     * The type of an accessibility event.
     * @since 7
     */
    type: EventType;

    /**
     * The type of the window change event.
     * @since 7
     */
    windowUpdateType?: WindowUpdateType;

    /**
     * The bundle name of the target application.
     * @since 7
     */
    bundleName: string;

    /**
     * The type of the event source component,such as button, chart.
     * @since 7
     */
    componentType?: string;

    /** The page id of the event source.
     * @since 7
     */
    pageId ?: number;

    /**
     * The accessibility event description.
     * @since 7
     */
    description?: string;

    /**
     * The action that triggers the accessibility event, for example, clicking or focusing a view.
     * @since 7
     */
    triggerAction: Action;

    /**
     * The movement step used for reading texts.
     * @since 7
     */
    textMoveUnit?: TextMoveUnit;

    /**
     * The content list.
     * @note
     * @since 7
     */
    contents?: Array<string>;

    /**
     * The content changed before.
     * @since 7
     */
    lastContent?: string;

    /**
     * The start index of listed items on the screen.
     * @since 7
     */
    beginIndex?: number;

    /**
     * The index of the current item on the screen.
     * @since 7
     */
    currentIndex?: number;

    /**
     * The end index of listed items on the screen.
     * @since 7
     */
    endIndex?: number;

    /**
     * The total of the items.
     * @note talkback used it when scroll.
     * @since 7
     */
    itemCount?: number;
  }
}
export default accessibility;