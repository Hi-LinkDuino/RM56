/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

/* If a new value is added, add it from the end. */
var Color;
(function (Color) {
  Color["White"] = "#ffffffff";
  Color["Black"] = "#ff000000";
  Color["Blue"] = "#ff0000ff";
  Color["Brown"] = "#ffa52a2a";
  Color["Gray"] = "#ff808080";
  Color["Green"] = "#ff008000";
  Color["Grey"] = "#ff808080";
  Color["Orange"] = "#ffffa500";
  Color["Pink"] = "#ffffc0cb";
  Color["Red"] = "#ffff0000";
  Color["Yellow"] = "#ffffff00";
})(Color || (Color = {}));

var TextAlign;
(function (TextAlign) {
  TextAlign[TextAlign["Start"] = 0] = "Start";
  TextAlign[TextAlign["Center"] = 1] = "Center";
  TextAlign[TextAlign["End"] = 2] = "End";
})(TextAlign || (TextAlign = {}));

var DataPanelType;
(function (DataPanelType) {
  DataPanelType[DataPanelType["Line"] = 0] = "Line";
  DataPanelType[DataPanelType["Circle"] = 4] = "Circle";
})(DataPanelType || (DataPanelType = {}));

var FontWeight;
(function (FontWeight) {
  FontWeight["Light"] = "light";
  FontWeight["Lighter"] = "lighter";
  FontWeight["Normal"] = "normal";
  FontWeight["Regular"] = "regular";
  FontWeight["Medium"] = "medium";
  FontWeight["Bold"] = "bold";
  FontWeight["Bolder"] = "bolder";
})(FontWeight || (FontWeight = {}));

var Curve;
(function (Curve) {
  Curve["Linear"] = "linear";
  Curve["Ease"] = "ease";
  Curve["EaseIn"] = "ease-in";
  Curve["EaseOut"] = "ease-out";
  Curve["EaseInOut"] = "ease-in-out";
  Curve["FastOutSlowIn"] = "fast-out-slow-in";
  Curve["LinearOutSlowIn"] = "linear-out-slow-in";
  Curve["FastOutLinearIn"] = "fast-out-linear-in";
  Curve["ExtremeDeceleration"] = "extreme-deceleration";
  Curve["Sharp"] = "sharp";
  Curve["Rhythm"] = "rhythm";
  Curve["Smooth"] = "smooth";
  Curve["Friction"] = "friction";
})(Curve || (Curve = {}));

var FillMode;
(function (FillMode) {
  FillMode[FillMode["None"] = 0] = "none";
  FillMode[FillMode["Forwards"] = 1] = "forwards";
  FillMode[FillMode["Backwards"] = 2] = "backwards";
  FillMode[FillMode["Both"] = 3] = "both";
})(FillMode || (FillMode = {}));

var PlayMode;
(function (PlayMode) {
  PlayMode["Normal"] = "normal";
  PlayMode["Alternate"] = "alternate";
  PlayMode["Reverse"] = "reverse";
  PlayMode["AlternateReverse"] = "alternate-reverse";
})(PlayMode || (PlayMode = {}));

var BorderStyle;
(function (BorderStyle) {
  BorderStyle[BorderStyle["Solid"] = 0] = "Solid";
  BorderStyle[BorderStyle["Dashed"] = 1] = "Dashed";
  BorderStyle[BorderStyle["Dotted"] = 2] = "Dotted";
})(BorderStyle || (BorderStyle = {}));

var LineCapStyle;
(function (LineCapStyle) {
  LineCapStyle[LineCapStyle["Butt"] = 0] = "Butt";
  LineCapStyle[LineCapStyle["Round"] = 1] = "Round";
  LineCapStyle[LineCapStyle["Square"] = 2] = "Square";
})(LineCapStyle || (LineCapStyle = {}));

var ButtonType;
(function (ButtonType) {
  ButtonType[ButtonType["Normal"] = 0] = "Normal";
  ButtonType[ButtonType["Capsule"] = 1] = "Capsule";
  ButtonType[ButtonType["Circle"] = 2] = "Circle";
  ButtonType[ButtonType["Arc"] = 4] = "Arc";
})(ButtonType || (ButtonType = {}));

var DevicePosition;
(function (DevicePosition) {
  DevicePosition[DevicePosition["Front"] = 0] = "Front";
  DevicePosition[DevicePosition["Back"] = 1] = "Back";
})(DevicePosition || (DevicePosition = {}));

var ImageFit;
(function (ImageFit) {
  ImageFit[ImageFit["Fill"] = 0] = "Fill";
  ImageFit[ImageFit["Contain"] = 1] = "Contain";
  ImageFit[ImageFit["Cover"] = 2] = "Cover";
  ImageFit[ImageFit["Auto"] = 3] = "Auto";
  ImageFit[ImageFit["None"] = 5] = "None";
  ImageFit[ImageFit["ScaleDown"] = 6] = "ScaleDown";
})(ImageFit || (ImageFit = {}));

var ImageRepeat;
(function (ImageRepeat) {
  ImageRepeat[ImageRepeat["NoRepeat"] = 0] = "NoRepeat";
  ImageRepeat[ImageRepeat["X"] = 1] = "X";
  ImageRepeat[ImageRepeat["Y"] = 2] = "Y";
  ImageRepeat[ImageRepeat["XY"] = 3] = "XY";
})(ImageRepeat || (ImageRepeat = {}));

var ImageSize;
(function (ImageSize) {
  ImageSize[ImageSize["Contain"] = 0] = "Contain";
  ImageSize[ImageSize["Cover"] = 1] = "Cover";
  ImageSize[ImageSize["Auto"] = 2] = "Auto";
})(ImageSize || (ImageSize = {}));

var ImageRenderMode;
(function (ImageRenderMode) {
  ImageRenderMode[ImageRenderMode["Original"] = 0] = "Original";
  ImageRenderMode[ImageRenderMode["Template"] = 1] = "Template";
})(ImageRenderMode || (ImageRenderMode = {}));

var ImageInterpolation;
(function (ImageInterpolation) {
  ImageInterpolation[ImageInterpolation["None"] = 0] = "None";
  ImageInterpolation[ImageInterpolation["Low"] = 1] = "Low";
  ImageInterpolation[ImageInterpolation["Medium"] = 2] = "Medium";
  ImageInterpolation[ImageInterpolation["High"] = 3] = "High";
})(ImageInterpolation || (ImageInterpolation = {}));

var FontStyle;
(function (FontStyle) {
  FontStyle[FontStyle["Normal"] = 0] = "Normal";
  FontStyle[FontStyle["Italic"] = 1] = "Italic";
})(FontStyle || (FontStyle = {}));

var FlexAlign;
(function (FlexAlign) {
  FlexAlign[FlexAlign["Start"] = 1] = "Start";
  FlexAlign[FlexAlign["Center"] = 2] = "Center";
  FlexAlign[FlexAlign["End"] = 3] = "End";
  FlexAlign[FlexAlign["Stretch"] = 4] = "Stretch";
  FlexAlign[FlexAlign["Baseline"] = 5] = "Baseline";
  FlexAlign[FlexAlign["SpaceBetween"] = 6] = "SpaceBetween";
  FlexAlign[FlexAlign["SpaceAround"] = 7] = "SpaceAround";
  FlexAlign[FlexAlign["SpaceEvenly"] = 8] = "SpaceEvenly";
})(FlexAlign || (FlexAlign = {}));

var VerticalAlign;
(function (VerticalAlign) {
  VerticalAlign[VerticalAlign["Top"] = 1] = "Top";
  VerticalAlign[VerticalAlign["Center"] = 2] = "Center";
  VerticalAlign[VerticalAlign["Bottom"] = 3] = "Bottom";
})(VerticalAlign || (VerticalAlign = {}));

var HorizontalAlign;
(function (HorizontalAlign) {
  HorizontalAlign[HorizontalAlign["Start"] = 1] = "Start";
  HorizontalAlign[HorizontalAlign["Center"] = 2] = "Center";
  HorizontalAlign[HorizontalAlign["End"] = 3] = "End";
})(HorizontalAlign || (HorizontalAlign = {}));

var ItemAlign;
(function (ItemAlign) {
  ItemAlign[ItemAlign["Auto"] = 0] = "Auto";
  ItemAlign[ItemAlign["Start"] = 1] = "Start";
  ItemAlign[ItemAlign["Center"] = 2] = "Center";
  ItemAlign[ItemAlign["End"] = 3] = "End";
  ItemAlign[ItemAlign["Stretch"] = 4] = "Stretch";
  ItemAlign[ItemAlign["Baseline"] = 5] = "Baseline";
})(ItemAlign || (ItemAlign = {}));

var Align;
(function (Align) {
  Align[Align["Start"] = 4] = "Start";
  Align[Align["Center"] = 2] = "Center";
  Align[Align["End"] = 5] = "End";
})(Align || (Align = {}));

var Alignment;
(function (Alignment) {
  Alignment[Alignment["TopStart"] = 0] = "TopStart";
  Alignment[Alignment["Top"] = 1] = "Top";
  Alignment[Alignment["TopEnd"] = 2] = "TopEnd";
  Alignment[Alignment["Start"] = 3] = "Start";
  Alignment[Alignment["Center"] = 4] = "Center";
  Alignment[Alignment["End"] = 5] = "End";
  Alignment[Alignment["BottomStart"] = 6] = "BottomStart";
  Alignment[Alignment["Bottom"] = 7] = "Bottom";
  Alignment[Alignment["BottomEnd"] = 8] = "BottomEnd";
})(Alignment || (Alignment = {}));

var TextOverflow;
(function (TextOverflow) {
  TextOverflow[TextOverflow["Clip"] = 0] = "Clip";
  TextOverflow[TextOverflow["Ellipsis"] = 1] = "Ellipsis";
  TextOverflow[TextOverflow["None"] = 2] = "None";
})(TextOverflow || (TextOverflow = {}));

var TextDecorationType;
(function (TextDecorationType) {
  TextDecorationType[TextDecorationType["None"] = 0] = "None";
  TextDecorationType[TextDecorationType["Underline"] = 1] = "Underline";
  TextDecorationType[TextDecorationType["Overline"] = 2] = "Overline";
  TextDecorationType[TextDecorationType["LineThrough"] = 3] = "LineThrough";
})(TextDecorationType || (TextDecorationType= {}));

var TextCase;
(function (TextCase) {
  TextCase[TextCase["Normal"] = 0] = "Normal";
  TextCase[TextCase["LowerCase"] = 1] = "LowerCase";
  TextCase[TextCase["UpperCase"] = 2] = "UpperCase";
})(TextCase || (TextCase = {}));

var NavigationType;
(function (NavigationType) {
  NavigationType[NavigationType["Push"] = 1] = "Push";
  NavigationType[NavigationType["Replace"] = 2] = "Replace";
  NavigationType[NavigationType["Back"] = 3] = "Back";
})(NavigationType || (NavigationType = {}));

var PanelType;
(function (PanelType) {
  PanelType[PanelType["Minibar"] = 0] = "Minibar";
  PanelType[PanelType["Foldable"] = 1] = "Foldable";
  PanelType[PanelType["Temporary"] = 2] = "Temporary";
})(PanelType || (PanelType = {}));

var PanelMode;
(function (PanelMode) {
  PanelMode[PanelMode["Mini"] = 0] = "Mini";
  PanelMode[PanelMode["Half"] = 1] = "Half";
  PanelMode[PanelMode["Full"] = 2] = "Full";
})(PanelMode || (PanelMode = {}));

var Axis;
(function (Axis) {
  Axis[Axis["Vertical"] = 0] = "Vertical";
  Axis[Axis["Horizontal"] = 1] = "Horizontal";
})(Axis || (Axis = {}));

var BarState;
(function (BarState) {
  BarState[BarState["Off"] = 0] = "Off";
  BarState[BarState["Auto"] = 1] = "Auto";
  BarState[BarState["On"] = 2] = "On";
})(BarState || (BarState = {}));

var EdgeEffect;
(function (EdgeEffect) {
  EdgeEffect[EdgeEffect["Spring"] = 0] = "Spring";
  EdgeEffect[EdgeEffect["Fade"] = 1] = "Fade";
  EdgeEffect[EdgeEffect["None"] = 2] = "None";
})(EdgeEffect || (EdgeEffect = {}));

var ScrollState;
(function (ScrollState) {
  ScrollState[ScrollState["Idle"] = 0] = "Idle";
  ScrollState[ScrollState["Scroll"] = 1] = "Scroll";
  ScrollState[ScrollState["Fling"] = 2] = "Fling";
})(ScrollState || (ScrollState = {}));

var TouchType;
(function (TouchType) {
  TouchType[TouchType["Down"] = 0] = "Down";
  TouchType[TouchType["Up"] = 1] = "Up";
  TouchType[TouchType["Move"] = 2] = "Move";
  TouchType[TouchType["Cancel"] = 3] = "Cancel";
})(TouchType || (TouchType = {}));

var MouseButton;
(function (MouseButton) {
  MouseButton[MouseButton["None"] = 0] = "None";
  MouseButton[MouseButton["Left"] = 1] = "Left";
  MouseButton[MouseButton["Right"] = 2] = "Right";
  MouseButton[MouseButton["Middle"] = 4] = "Middle";
  MouseButton[MouseButton["Back"] = 8] = "Back";
  MouseButton[MouseButton["Forward"] = 16] = "Forward";
})(MouseButton || (MouseButton = {}));

var MouseAction;
(function (MouseAction) {
  MouseAction[MouseAction["None"] = 0] = "None";
  MouseAction[MouseAction["Press"] = 1] = "Press";
  MouseAction[MouseAction["Release"] = 2] = "Release";
  MouseAction[MouseAction["Move"] = 3] = "Move";
  MouseAction[MouseAction["Hover"] = 4] = "Hover";
})(MouseAction || (MouseAction = {}));

var KeyType;
(function (KeyType) {
  KeyType[KeyType["Down"] = 0] = "Down";
  KeyType[KeyType["Up"] = 1] = "Up";
  KeyType[KeyType["LongPress"] = 2] = "LongPress";
})(KeyType || (KeyType = {}));

var GestureMode;
(function (GestureMode) {
  GestureMode[GestureMode["Sequence"] = 0] = "Sequence";
  GestureMode[GestureMode["Parallel"] = 1] = "Parallel";
  GestureMode[GestureMode["Exclusive"] = 2] = "Exclusive";
})(GestureMode || (GestureMode = {}));

var PanDirection;
(function (PanDirection) {
  PanDirection[PanDirection["None"] = 0] = "None";
  PanDirection[PanDirection["Left"] = 1] = "Left";
  PanDirection[PanDirection["Right"] = 2] = "Right";
  PanDirection[PanDirection["Horizontal"] = 3] = "Horizontal";
  PanDirection[PanDirection["Up"] = 4] = "Up";
  PanDirection[PanDirection["Down"] = 8] = "Down";
  PanDirection[PanDirection["Vertical"] = 12] = "Vertical";
  PanDirection[PanDirection["All"] = 15] = "All";
})(PanDirection || (PanDirection = {}));

var SwipeDirection;
(function (SwipeDirection) {
  SwipeDirection[SwipeDirection["None"] = 0] = "None";
  SwipeDirection[SwipeDirection["Horizontal"] = 1] = "Horizontal";
  SwipeDirection[SwipeDirection["Vertical"] = 2] = "Vertical";
  SwipeDirection[SwipeDirection["All"] = 3] = "All";
})(SwipeDirection || (SwipeDirection = {}));

var GestureDirection;
(function (GestureDirection) {
  GestureDirection[GestureDirection["All"] = 0] = "All";
  GestureDirection[GestureDirection["Horizontal"] = 1] = "Horizontal";
  GestureDirection[GestureDirection["Vertical"] = 2] = "Vertical";
})(GestureDirection || (GestureDirection = {}));

var GestureMask;
(function (GestureMask) {
  GestureMask[GestureMask["Normal"] = 0] = "Normal";
  GestureMask[GestureMask["IgnoreInternal"] = 1] = "IgnoreInternal";
})(GestureMask || (GestureMask = {}));

var GesturePriority;
(function (GesturePriority) {
  GesturePriority[GesturePriority["Low"] = 0] = "Low";
  GesturePriority[GesturePriority["High"] = 1] = "High";
  GesturePriority[GesturePriority["Parallel"] = 2] = "Parallel";
})(GesturePriority || (GesturePriority = {}));

var Visibility;
(function (Visibility) {
  Visibility[Visibility["Visible"] = 0] = "Visible";
  Visibility[Visibility["Hidden"] = 1] = "Hidden";
  Visibility[Visibility["None"] = 2] = "None";
})(Visibility || (Visibility = {}));

var FlexDirection;
(function (FlexDirection) {
  FlexDirection[FlexDirection["Row"] = 0] = "Row";
  FlexDirection[FlexDirection["Column"] = 1] = "Column";
  FlexDirection[FlexDirection["RowReverse"] = 2] = "RowReverse";
  FlexDirection[FlexDirection["ColumnReverse"] = 3] = "ColumnReverse";
}) (FlexDirection || (FlexDirection = {}));

var FlexWrap;
(function (FlexWrap) {
  FlexWrap[FlexWrap["NoWrap"] = 0] = "NoWrap";
  FlexWrap[FlexWrap["Wrap"] = 1] = "Wrap";
  FlexWrap[FlexWrap["WrapReverse"] = 2] = "WrapReverse";
}) (FlexWrap || (FlexWrap = {}));

var BlurStyle;
(function (BlurStyle) {
  BlurStyle[BlurStyle["SmallLight"] = 100] = "SmallLight";
  BlurStyle[BlurStyle["MediumLight"] = 101] = "MediumLight";
  BlurStyle[BlurStyle["LargeLight"] = 102] = "LargeLight";
  BlurStyle[BlurStyle["XlargeLight"] = 103] = "XlargeLight";
  BlurStyle[BlurStyle["SmallDark"] = 104] = "SmallDark";
  BlurStyle[BlurStyle["MediumDark"] = 105] = "MediumDark";
  BlurStyle[BlurStyle["LargeDark"] = 106] = "LargeDark";
  BlurStyle[BlurStyle["XlargeDark"] = 107] = "XlargeDark";
})(BlurStyle || (BlurStyle = {}));

var Edge;
(function (Edge) {
  Edge[Edge["Top"] = 0] = "Top";
  Edge[Edge["Center"] = 1] = "Center";
  Edge[Edge["Bottom"] = 2] = "Bottom";
  Edge[Edge["Baseline"] = 3] = "Baseline";
  Edge[Edge["Start"] = 4] = "Start";
  Edge[Edge["Middle"] = 5] = "Middle";
  Edge[Edge["End"] = 6] = "End";
})(Edge || (Edge = {}));

var LineJoinStyle;
(function (LineJoinStyle) {
  LineJoinStyle[LineJoinStyle["Miter"] = 0] = "Miter";
  LineJoinStyle[LineJoinStyle["Round"] = 1] = "Round";
  LineJoinStyle[LineJoinStyle["Bevel"] = 2] = "Bevel";
})(LineJoinStyle || (LineJoinStyle = {}));

var AnimationStatus;
(function (AnimationStatus) {
  AnimationStatus[AnimationStatus["Initial"] = 0] = "Initial";
  AnimationStatus[AnimationStatus["Running"] = 1] = "Running";
  AnimationStatus[AnimationStatus["Paused"] = 2] = "Paused";
  AnimationStatus[AnimationStatus["Stopped"] = 3] = "Stopped";
})(AnimationStatus || (AnimationStatus = {}));

var Motion;
(function (Motion) {
  Motion[Motion["SpringMotion"] = 0] = "SpringMotion";
  Motion[Motion["FrictionMotion"] = 1] = "FrictionMotion";
  Motion[Motion["ScrollMotion"] = 2] = "ScrollMotion";
})(Motion || (Motion = {}));

var ScrollDirection;
(function (ScrollDirection) {
  ScrollDirection[ScrollDirection["Vertical"] = 0] = "Vertical";
  ScrollDirection[ScrollDirection["Horizontal"] = 1] = "Horizontal";
  ScrollDirection[ScrollDirection["Free"] = 2] = "Free";
  ScrollDirection[ScrollDirection["None"] = 3] = "None";
}) (ScrollDirection || (ScrollDirection = {}));

var Sticky;
(function (Sticky) {
  Sticky[Sticky["None"] = 0] = "None";
  Sticky[Sticky["Normal"] = 1] = "Normal";
  Sticky[Sticky["Opacity"] = 2] = "Opacity";
})(Sticky || (Sticky = {}));

var Week;
(function (Week) {
  Week["Mon"] = 0b0000001;
  Week["Tue"] = 0b0000010;
  Week["Wed"] = 0b0000100;
  Week["Thur"] = 0b0001000;
  Week["Fri"] = 0b0010000;
  Week["Sat"] = 0b0100000;
  Week["Sun"] = 0b1000000;
})(Week || (Week = {}));

var FormDimension;
(function (FormDimension) {
  FormDimension["Dimension_1_2"] = 1;
  FormDimension["Dimension_2_2"] = 2;
  FormDimension["Dimension_2_4"] = 3;
  FormDimension["Dimension_4_4"] = 4;
})(FormDimension || (FormDimension = {}));

var TransitionType;
(function (TransitionType) {
  TransitionType["All"] = "All";
  TransitionType["Insert"] = "Insert";
  TransitionType["Delete"]  = "Delete";
})(TransitionType || (TransitionType = {}));

var Direction;
(function (Direction) {
  Direction["Ltr"] = "Ltr";
  Direction["Rtl"] = "Rtl";
  Direction["Auto"] = "Auto";
})(Direction || (Direction = {}));

var RouteType;
(function (RouteType) {
  RouteType[RouteType["None"] = 0] = "None";
  RouteType[RouteType["Push"] = 1] = "Push";
  RouteType[RouteType["Pop"] = 2] = "Pop";
})(RouteType || (RouteType = {}));

var FocusDirection;
(function (FocusDirection) {
  FocusDirection[FocusDirection["Up"] = 19] = "Up";
  FocusDirection[FocusDirection["Down"] = 20] = "Down";
  FocusDirection[FocusDirection["Left"] = 21] = "Left";
  FocusDirection[FocusDirection["Right"] = 22] = "Right";
  FocusDirection[FocusDirection["Tab"] = 61] = "Tab";
  FocusDirection[FocusDirection["ShiftTab"] = 200] = "ShiftTab";
})(FocusDirection || (FocusDirection = {}));

var SlideEffect;
(function (SlideEffect) {
  SlideEffect[SlideEffect["None"] = 0] = "None";
  SlideEffect[SlideEffect["Left"] = 1] = "Left";
  SlideEffect[SlideEffect["Right"] = 2] = "Right";
  SlideEffect[SlideEffect["Top"] = 3] = "Top";
  SlideEffect[SlideEffect["Bottom"] = 4] = "Bottom";
})(SlideEffect || (SlideEffect = {}));

var GradientDirection;
(function (GradientDirection) {
  GradientDirection[GradientDirection["Left"] = 0] = "Left";
  GradientDirection[GradientDirection["Top"] = 1] = "Top";
  GradientDirection[GradientDirection["Right"] = 2] = "Right";
  GradientDirection[GradientDirection["Bottom"] = 3] = "Bottom";
  GradientDirection[GradientDirection["LeftTop"] = 4] = "Bottom";
  GradientDirection[GradientDirection["LeftBottom"] = 5] = "Bottom";
  GradientDirection[GradientDirection["RightTop"] = 6] = "Bottom";
  GradientDirection[GradientDirection["RightBottom"] = 7] = "Bottom";
  GradientDirection[GradientDirection["None"] = 8] = "None";
})(GradientDirection || (GradientDirection = {}));

var BarMode;
(function (BarMode) {
  BarMode["Fixed"] = "Fixed";
  BarMode["Scrollable"] = "Scrollable";
})(BarMode || (BarMode = {}));

var SizeType;
(function (SizeType) {
  SizeType[SizeType["Auto"] = 0] = "Auto";
  SizeType[SizeType["XS"] = 1] = "XS";
  SizeType[SizeType["SM"] = 2] = "SM";
  SizeType[SizeType["MD"] = 3] = "MD";
  SizeType[SizeType["LG"] = 4] = "LG";
})(SizeType || (SizeType = {}));

var ColorMode;
(function (ColorMode) {
    ColorMode[ColorMode["LIGHT"] = 0] = "LIGHT";
    ColorMode[ColorMode["DARK"] = 1] = "DARK";
})(ColorMode || (ColorMode = {}));
var LayoutDirection;
(function (LayoutDirection) {
    LayoutDirection[LayoutDirection["RTL"] = 0] = "RTL";
    LayoutDirection[LayoutDirection["LTR"] = 1] = "LTR";
})(LayoutDirection || (LayoutDirection = {}));

function isSystemplugin(shortName, moduleType) {
  const plugin = moduleType === 'ohos' ? globalThis.ohosplugin : globalThis.systemplugin;
  if (typeof (plugin) !== 'undefined') {
    var target = plugin;
    for (let key of shortName.split('.')) {
      target = target[key];
      if (!target) {
        break;
      }
    }
    return typeof (target) !== 'undefined';
  }
}

var BarPosition;
(function (BarPosition) {
  BarPosition[BarPosition["Start"] = 0] = "Start";
  BarPosition[BarPosition["End"] = 1] = "End";
})(BarPosition || (BarPosition = {}));

var SharedTransitionEffectType;
(function (SharedTransitionEffectType) {
  SharedTransitionEffectType[SharedTransitionEffectType["Static"] = 0] = "Static";
  SharedTransitionEffectType[SharedTransitionEffectType["Exchange"] = 1] = "Exchange";
})(SharedTransitionEffectType || (SharedTransitionEffectType = {}));

function $r(id, type, ...params){
  return {"id": id, "type": type, "params": params};
}

function $rawfile(fileName){
  return {"id": 0, "type": 30000, "params": [fileName]};
}

var IndexerAlign;
(function (IndexerAlign) {
  IndexerAlign[IndexerAlign["Left"] = 0] = "Left";
  IndexerAlign[IndexerAlign["Right"] = 1] = "Right";
})(IndexerAlign || (IndexerAlign = {}));

var DialogAlignment;
(function (DialogAlignment) {
  DialogAlignment[DialogAlignment["Top"] = 0] = "Top";
  DialogAlignment[DialogAlignment["Center"] = 1] = "Center";
  DialogAlignment[DialogAlignment["Bottom"] = 2] = "Bottom";
  DialogAlignment[DialogAlignment["Default"] = 3] = "Default";
  DialogAlignment[DialogAlignment["TopStart"] = 4] = "TopStart";
  DialogAlignment[DialogAlignment["TopEnd"] = 5] = "TopEnd";
  DialogAlignment[DialogAlignment["CenterStart"] = 6] = "CenterStart";
  DialogAlignment[DialogAlignment["CenterEnd"] = 7] = "CenterEnd";
  DialogAlignment[DialogAlignment["BottomStart"] = 8] = "BottomStart";
  DialogAlignment[DialogAlignment["BottomEnd"] = 9] = "BottomEnd";
})(DialogAlignment || (DialogAlignment = {}));

var EditMode;
(function (EditMode) {
  EditMode[EditMode["None"] = 0] = "None";
  EditMode[EditMode["Deletable"] = 1] = "Deletable";
  EditMode[EditMode["Movable"] = 2] = "Movable";
})(EditMode || (EditMode = {}));

var SwiperDisplayMode;
(function (SwiperDisplayMode) {
  SwiperDisplayMode[SwiperDisplayMode["Stretch"] = 0] = "Stretch";
  SwiperDisplayMode[SwiperDisplayMode["AutoLinear"] = 1] = "AutoLinear";
})(SwiperDisplayMode || (SwiperDisplayMode = {}));

var EdgeEffect;
(function (EdgeEffect) {
  EdgeEffect[EdgeEffect["Spring"] = 0] = "Spring";
  EdgeEffect[EdgeEffect["Fade"] = 1] = "Fade";
  EdgeEffect[EdgeEffect["None"] = 2] = "None";
})(EdgeEffect || (EdgeEffect = {}));

var DatePickerType;
(function (DatePickerType) {
  DatePickerType[DatePickerType["Time"] = 0] = "Time";
  DatePickerType[DatePickerType["Date"] = 1] = "Date";
})(DatePickerType || (DatePickerType = {}));

var InputType;
(function (InputType) {
  InputType[InputType["Normal"] = 0] = "Normal";
  InputType[InputType["Number"] = 2] = "Number";
  InputType[InputType["Email"] = 5] = "Email";
  InputType[InputType["Password"] = 7] = "Password";
})(InputType || (InputType = {}));

var EnterKeyType;
(function (EnterKeyType) {
  EnterKeyType[EnterKeyType["Go"] = 2] = "Go";
  EnterKeyType[EnterKeyType["Search"] = 3] = "Search";
  EnterKeyType[EnterKeyType["Send"] = 4] = "Send";
  EnterKeyType[EnterKeyType["Next"] = 5] = "Next";
  EnterKeyType[EnterKeyType["Done"] = 6] = "Done";
})(EnterKeyType || (EnterKeyType = {}));

var ItemState
(function (ItemState) {
  ItemState[ItemState["Normal"] = 0] = "Normal";
  ItemState[ItemState["Disabled"] = 1] = "Disabled";
  ItemState[ItemState["Waiting"] = 2] = "Waiting";
  ItemState[ItemState["Skip"] = 3] = "Skip";
})(ItemState || (ItemState = {}));

var NavigationTitleMode;
(function (NavigationTitleMode) {
  NavigationTitleMode[NavigationTitleMode["Free"] = 0] = "Free";
  NavigationTitleMode[NavigationTitleMode["Full"] = 1] = "Full";
  NavigationTitleMode[NavigationTitleMode["Mini"] = 2] = "Mini";
})(NavigationTitleMode || (NavigationTitleMode = {}));

var ResponseType;
(function (ResponseType) {
  ResponseType[ResponseType["RightClick"] = 0] = "RightClick";
  ResponseType[ResponseType["LongPress"] = 1] = "LongPress";
})(ResponseType || (ResponseType = {}));

var HoverEffect;
(function (HoverEffect) {
  HoverEffect[HoverEffect["Auto"] = 4] = "Auto";
  HoverEffect[HoverEffect["Scale"] = 2] = "Scale";
  HoverEffect[HoverEffect["Board"] = 3] = "Board";
  HoverEffect[HoverEffect["Highlight"] = 3] = "Highlight";
  HoverEffect[HoverEffect["None"] = 0] = "None";
})(HoverEffect || (HoverEffect = {}));

var ScrollBarDirection;
(function (ScrollBarDirection) {
  ScrollBarDirection[ScrollBarDirection["Vertical"] = 0] = "Vertical";
  ScrollBarDirection[ScrollBarDirection["Horizontal"] = 1] = "Horizontal";
  ScrollBarDirection[ScrollBarDirection["None"] = 2] = "None";
}) (ScrollBarDirection || (ScrollBarDirection = {}));

var Placement;
(function (Placement) {
  Placement[Placement["Left"] = 0] = "Left";
  Placement[Placement["Right"] = 1] = "Right";
  Placement[Placement["Top"] = 2] = "Top";
  Placement[Placement["Bottom"] = 3] = "Bottom";
  Placement[Placement["TopLeft"] = 4] = "TopLeft";
  Placement[Placement["TopRight"] = 5] = "TopRight";
  Placement[Placement["BottomLeft"] = 6] = "BottomLeft";
  Placement[Placement["BottomRight"] = 7] = "BottomRight";
}) (Placement || (Placement = {}));

var DialogStatus;
(function (DialogStatus) {
  DialogStatus[DialogStatus["Accept"] = 0] = "Accept";
  DialogStatus[DialogStatus["Cancel"] = 1] = "Cancel";
  DialogStatus[DialogStatus["Update"] = 2] = "Update";
})(DialogStatus || (DialogStatus = {}));

var SourceType;
(function (SourceType) {
  SourceType[SourceType["Unknown"] = 0] = "Unknown";
  SourceType[SourceType["Mouse"] = 1] = "Mouse";
  SourceType[SourceType["TouchScreen"] = 2] = "TouchScreen";
  SourceType[SourceType["Keyboard"] = 4] = "Keyboard";
})(SourceType || (SourceType = {}));

var SeekMode;
(function (SeekMode) {
  SeekMode[SeekMode["PreviousKeyframe"] = 0] = "PreviousKeyframe";
  SeekMode[SeekMode["NextKeyframe"] = 1] = "NextKeyframe";
  SeekMode[SeekMode["ClosestKeyframe"] = 2] = "ClosestKeyframe";
  SeekMode[SeekMode["Accurate"] = 3] = "Accurate";
})(SeekMode || (SeekMode = {}));

var SideBarContainerType;
(function (SideBarContainerType) {
  SideBarContainerType[SideBarContainerType["Overlay"] = 1] = "Overlay";
  SideBarContainerType[SideBarContainerType["Embed"] = 0] = "Embed";
})(SideBarContainerType || (SideBarContainerType = {}));

var PlaybackSpeed;
(function (PlaybackSpeed) {
  PlaybackSpeed["Speed_Forward_0_75_X"] = "0.75";
  PlaybackSpeed["Speed_Forward_1_00_X"] = "1.00";
  PlaybackSpeed["Speed_Forward_1_25_X"] = "1.25";
  PlaybackSpeed["Speed_Forward_1_75_X"] = "1.75";
  PlaybackSpeed["Speed_Forward_2_00_X"] = "2.00";
})(PlaybackSpeed || (PlaybackSpeed = {}));

var MixedMode;
(function (MixedMode) {
  MixedMode[MixedMode["ALL"] = 0] = "All";
  MixedMode[MixedMode["Compatible"] = 1] = "Compatible";
  MixedMode[MixedMode["None"] = 2] = "None";
})(MixedMode || (MixedMode = {}));

var HitTestType;
(function (HitTestType) {
  HitTestType[HitTestType["EditText"] = 0] = "EditText";
  HitTestType[HitTestType["Email"] = 1] = "Email";
  HitTestType[HitTestType["HttpAnchor"] = 2] = "HttpAnchor";
  HitTestType[HitTestType["HttpAnchorImg"] = 3] = "HttpAnchorImg";
  HitTestType[HitTestType["Img"] = 4] = "Img";
  HitTestType[HitTestType["Map"] = 5] = "Map";
  HitTestType[HitTestType["Phone"] = 6] = "Phone";
  HitTestType[HitTestType["Unknown"] = 7] = "Unknown";
})(HitTestType || (HitTestType = {}));


var ProgressType;
(function (ProgressType) {
  ProgressType[ProgressType["Linear"] = 0] = "Linear";
  ProgressType[ProgressType["Ring"] = 1] = "Ring";
  ProgressType[ProgressType["Eclipse"] = 2] = "Eclipse";
  ProgressType[ProgressType["ScaleRing"] = 3] = "ScaleRing";
  ProgressType[ProgressType["Capsule"] = 4] = "Capsule";
})(ProgressType || (ProgressType = {}));

var MessageLevel
(function (MessageLevel) {
  MessageLevel[MessageLevel["Debug"] = 1] = "Debug";
  MessageLevel[MessageLevel["Info"] = 2] = "Info";
  MessageLevel[MessageLevel["Warn"] = 3] = "Warn";
  MessageLevel[MessageLevel["Error"] = 4] = "Error";
  MessageLevel[MessageLevel["Log"] = 5] = "Log";
})(MessageLevel || (MessageLevel = {}));