/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_CANVAS_COMPONENT_H
#define OHOS_ACELITE_CANVAS_COMPONENT_H

#include "acelite_config.h"

#if (FEATURE_COMPONENT_CANVAS == 1)
#include "component.h"
#include "non_copyable.h"
#include "ui_canvas.h"
#include <string>
namespace OHOS {
namespace ACELite {
enum ArgsCount : uint8_t { NUM_1 = 1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6 };
enum ArgsIndex : uint8_t { IDX_0, IDX_1, IDX_2, IDX_3, IDX_4, IDX_5 };
struct AttrMap {
    ACE_DISALLOW_COPY_AND_MOVE(AttrMap);
    const char *attrName;
    jerry_external_handler_t setterName;
    jerry_external_handler_t getterName;
};
struct MethodMap {
    ACE_DISALLOW_COPY_AND_MOVE(MethodMap);
    const char *methodName;
    jerry_external_handler_t callbackName;
};

class CanvasComponent final : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(CanvasComponent);
    CanvasComponent() = delete;
    CanvasComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *styleManager);
    ~CanvasComponent() override {}

protected:
    bool CreateNativeViews() override;
    void ReleaseNativeViews() override;
    UIView *GetComponentRootView() const override;

private:
    void GetSubFont(const char *font, const uint8_t index, char *&subFont) const;
    bool FormatArcAngle(double sAngle,
                        double eAngle,
                        int16_t &startAngle,
                        int16_t &endAngle,
                        bool counterClockwise) const;
						
    static void RegisterAttributeFunc(jerry_value_t canvas2ddom,
                                      const char *attributeName,
                                      jerry_external_handler_t setterHandler,
                                      jerry_external_handler_t getterHandler);

    static void RegisterDrawMethodFunc(jerry_value_t canvas2ddom,
                                       const char *drawMethodName,
                                       jerry_external_handler_t handler);

    static jerry_value_t GetDom(const jerry_value_t func,
                                const jerry_value_t dom,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static jerry_value_t FillStyleSetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t FillStyleGetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t StrokeStyleSetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);

    static jerry_value_t StrokeStyleGetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);

    static jerry_value_t LineWidthSetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t LineWidthGetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t FontSetter(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);

    static jerry_value_t FontGetter(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);

    static jerry_value_t TextAlignSetter(const jerry_value_t func,
                                         const jerry_value_t dom,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t TextAlignGetter(const jerry_value_t func,
                                         const jerry_value_t contex,
                                         const jerry_value_t args[],
                                         const jerry_length_t argsNum);

    static jerry_value_t GlobalAlphaSetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);

    static jerry_value_t GlobalAlphaGetter(const jerry_value_t func,
                                           const jerry_value_t dom,
                                           const jerry_value_t args[],
                                           const jerry_length_t argsNum);

    static jerry_value_t FillRect(const jerry_value_t func,
                                  const jerry_value_t dom,
                                  const jerry_value_t args[],
                                  const jerry_length_t argsNum);

    static jerry_value_t StrokeRect(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);

    static jerry_value_t FillText(const jerry_value_t func,
                                  const jerry_value_t dom,
                                  const jerry_value_t args[],
                                  const jerry_length_t argsNum);

    static jerry_value_t BeginPath(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t MoveTo(const jerry_value_t func,
                                const jerry_value_t dom,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static jerry_value_t LineTo(const jerry_value_t func,
                                const jerry_value_t dom,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static jerry_value_t Rect(const jerry_value_t func,
                              const jerry_value_t dom,
                              const jerry_value_t args[],
                              const jerry_length_t argsNum);

    static jerry_value_t CleanRect(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t Arc(const jerry_value_t func,
                             const jerry_value_t dom,
                             const jerry_value_t args[],
                             const jerry_length_t argsNum);

    static jerry_value_t ClosePath(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t Stroke(const jerry_value_t func,
                                const jerry_value_t dom,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static jerry_value_t Fill(const jerry_value_t func,
                              const jerry_value_t dom,
                              const jerry_value_t args[],
                              const jerry_length_t argsNum);

    static jerry_value_t Rotate(const jerry_value_t func,
                                const jerry_value_t dom,
                                const jerry_value_t args[],
                                const jerry_length_t argsNum);

    static jerry_value_t Scale(const jerry_value_t func,
                               const jerry_value_t dom,
                               const jerry_value_t args[],
                               const jerry_length_t argsNum);

    static jerry_value_t Translate(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t Transform(const jerry_value_t func,
                                   const jerry_value_t dom,
                                   const jerry_value_t args[],
                                   const jerry_length_t argsNum);

    static jerry_value_t SetTransform(const jerry_value_t func,
                                      const jerry_value_t dom,
                                      const jerry_value_t args[],
                                      const jerry_length_t argsNum);

    static jerry_value_t CreateLInearGradient(const jerry_value_t func,
                                              const jerry_value_t dom,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum);

    static jerry_value_t CreateRadialGradient(const jerry_value_t func,
                                              const jerry_value_t dom,
                                              const jerry_value_t args[],
                                              const jerry_length_t argsNum);

    static jerry_value_t AddColorStop(const jerry_value_t func,
                                      const jerry_value_t dom,
                                      const jerry_value_t args[],
                                      const jerry_length_t argsNum);

    static jerry_value_t Save(const jerry_value_t func,
                              const jerry_value_t dom,
                              const jerry_value_t args[],
                              const jerry_length_t argsNum);

    static jerry_value_t Restore(const jerry_value_t func,
                                 const jerry_value_t dom,
                                 const jerry_value_t args[],
                                 const jerry_length_t argsNum);

    static jerry_value_t DrawCircle(const jerry_value_t func,
                                    const jerry_value_t dom,
                                    const jerry_value_t args[],
                                    const jerry_length_t argsNum);
    UICanvas canvas_;
    jerry_value_t dom_;
    jerry_value_t dashArray_;
    jerry_value_t measureTextObject_;
    jerry_value_t measureTextWidthString_;
    Paint paint_;
    UICanvas::FontStyle fontStyle_;
    char *fillStyleValue_;
    char *strokeStyleValue_;
    char *fontValue_;
    char *textAlignValue_;
    int16_t lineWidthValue_;

    char *lineCapValue_;
    char *lineJoinValue_;
    double miterLimitValue_;
    float lineDashOffsetValue_;

    char *colorStopValue_;

    static const char * const DEFAULT_FILLSTYLE;
    static const char * const DEFAULT_STROKESTYLE;
    static const char * const DEFAULT_TEXTALIGN;
    static const char * const DEFAULT_LINECAP;
    static const char * const DEFAULT_LINEJOIN;
    static const int16_t DEFAULT_MITERLIMIT;
    static const int16_t DEFAULT_LINEDASHOFFSET;

    static const AttrMap attrMap_[];
    static const MethodMap methodMap_[];
    static const char * const ATTR_FILLSTYLE;
    static const char * const ATTR_STROKESTYLE;
    static const char * const ATTR_LINEWIDTH;
    static const char * const ATTR_FONT;
    static const char * const ATTR_TEXTALIGN;

    static const char * const ATTR_LINECAP;
    static const char * const ATTR_LINEJOIN;
    static const char * const ATTR_MITERLIMIT;
    static const char * const ATTR_LINEDASHOFFSET;
    static const char * const ATTR_GLOBALALPHA;
    static const char * const ATTR_GLOBALCOMPOSITEOPERATION;

    static const char * const FUNC_GETdom;
    static const char * const FUNC_FILLRECT;
    static const char * const FUNC_STROKERECT;
    static const char * const FUNC_FILLTEXT;
    static const char * const FUNC_BEGINPATH;
    static const char * const FUNC_MOVETO;
    static const char * const FUNC_LINETO;
    static const char * const FUNC_RECT;
    static const char * const FUNC_ARC;
    static const char * const FUNC_CLOSEPATH;
    static const char * const FUNC_STROKE;
    static const char * const FUNC_FILL;
    static const char * const FUNC_SETLINEDASH;
    static const char * const FUNC_GETLINEDASH;
    static const char * const FUNC_STROKETEXT;
    static const char * const FUNC_ROTATE;
    static const char * const FUNC_SCALE;
    static const char * const FUNC_TRANSLATE;
    static const char * const FUNC_TRANFORM;
    static const char * const FUNC_SETTRANFORM;
    static const char * const FUNC_SAVE;
    static const char * const FUNC_RESTORE;
    static const char * const FUNC_DRAWCIRCLE;
    static const char * const FUNC_CREATELINEARGRADIENT;
    static const char * const FUNC_CREATERADIALGRADIENT;
    static const char * const FUNC_ADDCOLORSTOP;
};
} // namespace ACELite
} // namespace OHOS
#endif // FEATURE_COMPONENT_CANVAS
#endif // OHOS_ACELITE_CANVAS_COMPONENT_H
