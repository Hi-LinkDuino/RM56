/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSAlphabetIndexerAttribute extends CommonMethod<AlphabetIndexerAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): AlphabetIndexerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): AlphabetIndexerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: { arrayValue: Array<string>; selected: number }): AlphabetIndexerAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSAnimatorAttribute extends CommonMethod<AnimatorAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: string): AnimatorAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSBadgeAttribute extends CommonMethod<BadgeAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): BadgeAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): BadgeAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: BadgeParamWithNumber | BadgeParamWithString): BadgeAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSBlankAttribute extends CommonMethod<BlankAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(min?: number | string): BlankAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): BlankAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): BlankAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSButtonAttribute extends CommonMethod<ButtonAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  createWithChild(label?: ResourceStr, options?: ButtonOptions): ButtonAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  createWithLabel(label?: ResourceStr, options?: ButtonOptions): ButtonAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ButtonAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ButtonAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSCalendarAttribute {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: {
    date: {
      year: number;
      month: number;
      day: number;
    };
    currentData: MonthData;
    preData: MonthData;
    nextData: MonthData;
    controller?: CalendarController;
  }): CalendarAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): CalendarAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): CalendarAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSCanvasAttribute extends CommonMethod<CanvasAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(context?: CanvasRenderingContext2D): CanvasAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): CanvasAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): CanvasAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSCheckboxAttribute extends CommonMethod<CheckboxAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: CheckboxOptions): CheckboxAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): CheckboxAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): CheckboxAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSCheckboxGroupAttribute extends CommonMethod<CheckboxGroupAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: CheckboxGroupOptions): CheckboxGroupAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): CheckboxGroupAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): CheckboxGroupAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSCircleAttribute extends CommonShapeMethod<CircleAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: CircleOptions): CircleAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): CircleAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSColumnAttribute extends CommonMethod<ColumnAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { space?: string | number }): ColumnAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ColumnAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ColumnAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSColumnSplitAttribute extends CommonMethod<ColumnSplitAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): ColumnSplitAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ColumnSplitAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ColumnSplitAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSCounterAttribute extends CommonMethod<CounterAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): CounterAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): CounterAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): CounterAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSDataPanelAttribute extends CommonMethod<DataPanelAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options: DataPanelOptions): DataPanelAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): DataPanelAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): DataPanelAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSDatePickerAttribute extends CommonMethod<DatePickerAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: DatePickerOptions): DatePickerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): DatePickerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): DatePickerAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSDividerAttribute extends CommonMethod<DividerAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): DividerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): DividerAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSEllipseAttribute extends CommonShapeMethod<EllipseAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { width?: string | number; height?: string | number }): EllipseAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): EllipseAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSFlexAttribute extends CommonMethod<FlexAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: FlexOptions): FlexAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): FlexAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): FlexAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSForEachInterface {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(
    arr: Array<any>,
    itemGenerator: (item: any, index?: number) => void,
    keyGenerator?: (item: any, index?: number) => string,
  ): ForEachInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ForEachInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ForEachInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSFormComponentAttribute extends CommonMethod<FormComponentAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: {
    id: number;
    name: string;
    bundle: string;
    ability: string;
    module: string;
    dimension?: FormDimension;
    temporary?: boolean;
  }): FormComponentAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): FormComponentAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSGaugeAttribute extends CommonMethod<GaugeAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options: { value: number; min?: number; max?: number }): GaugeAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): GaugeAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSGridAttribute extends CommonMethod<GridAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(scroller?: Scroller): GridAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): GridAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): GridAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSGridItemAttribute extends CommonMethod<GridItemAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): GridItemAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): GridItemAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): GridItemAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSGridContainerAttribute extends ColumnAttribute {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: GridContainerOptions): GridContainerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): GridContainerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): GridContainerAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSImageAttribute extends CommonMethod<ImageAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(src: string | PixelMap | Resource): ImageAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ImageAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSImageAnimatorAttribute extends CommonMethod<ImageAnimatorAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): ImageAnimatorAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ImageAnimatorAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSLazyForEachInterface {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(
    dataSource: IDataSource,
    itemGenerator: (item: any, index?: number) => void,
    keyGenerator?: (item: any, index?: number) => string,
  ): LazyForEachInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): LazyForEachInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): LazyForEachInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSLineAttribute extends CommonShapeMethod<LineAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { width?: string | number; height?: string | number }): LineAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): LineAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSListAttribute extends CommonMethod<ListAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { initialIndex?: number; space?: number | string; scroller?: Scroller }): ListAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ListAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ListAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSListItemAttribute extends CommonMethod<ListItemAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: string): ListItemAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ListItemAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ListItemAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSLoadingProgressAttribute extends CommonMethod<LoadingProgressAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): LoadingProgressAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): LoadingProgressAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  color(value: ResourceColor): LoadingProgressAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSMarqueeAttribute extends CommonMethod<MarqueeAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: { start: boolean; step?: number; loop?: number; fromStart?: boolean; src: string }): MarqueeAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): MarqueeAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): MarqueeAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSNavigationAttribute extends CommonMethod<NavigationAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): NavigationAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): NavigationAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): NavigationAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSNavigatorAttribute extends CommonMethod<NavigatorAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { target: string; type?: NavigationType }): NavigatorAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): NavigatorAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): NavigatorAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPageTransitionEnterInterface extends CommonTransition<PageTransitionEnterInterface> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: {
    type?: RouteType;
    duration?: number;
    curve?: Curve | string;
    delay?: number;
  }): PageTransitionEnterInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPageTransitionExitInterface extends CommonTransition<PageTransitionExitInterface> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: {
    type?: RouteType;
    duration?: number;
    curve?: Curve | string;
    delay?: number;
  }): PageTransitionExitInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPanelAttribute extends CommonMethod<PanelAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(show: boolean): PanelAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): PanelAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): PanelAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPathAttribute extends CommonShapeMethod<PathAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { width?: number | string; height?: number | string; commands?: string }): PathAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): PathAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): PathAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPatternLockAttribute extends CommonMethod<TSPatternLockAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(controller?: PatternLockController): TSPatternLockAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): TSPatternLockAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): TSPatternLockAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPieceAttribute extends CommonMethod<PieceAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: { content: string; icon?: string }): PieceAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): PieceAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): PieceAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPluginComponentAttribute extends CommonMethod<PluginComponentAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: { template: PluginComponentTemplate; data: any }): PluginComponentAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): PluginComponentAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): PluginComponentAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPolygonAttribute extends CommonShapeMethod<PolygonAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { width?: string | number; height?: string | number }): PolygonAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): PolygonAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPolylineAttribute extends CommonShapeMethod<PolylineAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { width?: string | number; height?: string | number }): PolylineAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): PolylineAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSProgressAttribute extends CommonMethod<ProgressAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options: ProgressOptions): ProgressAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ProgressAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSQRCodeAttribute extends CommonMethod<QRCodeAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: string): QRCodeAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): QRCodeAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): QRCodeAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSRadioAttribute extends CommonMethod<RadioAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options: RadioOptions): RadioAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): RadioAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSRatingAttribute extends CommonMethod<RatingAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: { rating: number; indicator?: boolean }): RatingAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): RatingAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): RatingAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSRectAttribute extends CommonShapeMethod<RectAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(
    value?:
      | {
          width?: number | string;
          height?: number | string;
          radius?: number | string | Array<any>;
        }
      | {
          width?: number | string;
          height?: number | string;
          radiusWidth?: number | string;
          radiusHeight?: number | string;
        },
  ): RectAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): RectAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSRefreshAttribute extends CommonMethod<RefreshAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: { refreshing: boolean; offset?: number | string; friction?: number | string }): RefreshAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): RefreshAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): RefreshAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSRowAttribute extends CommonMethod<RowAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { space?: string | number }): RowAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): RowAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): RowAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSRowSplitAttribute extends CommonMethod<RowSplitAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): RowSplitAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): RowSplitAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): RowSplitAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSScrollAttribute extends CommonMethod<ScrollAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(scroller?: Scroller): ScrollAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ScrollAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ScrollAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSScrollBarAttribute extends CommonMethod<ScrollBarAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: ScrollBarOptions): ScrollBarAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ScrollBarAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ScrollBarAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSSearchAttribute extends CommonMethod<SearchAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: {
    value?: string;
    placeholder?: string;
    icon?: string;
    controller?: SearchController;
  }): SearchAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): SearchAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): SearchAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSSelectAttribute extends CommonMethod<SelectAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options: Array<SelectOptions>): SelectAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): SelectAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): SelectAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSShapeAttribute extends CommonMethod<ShapeAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: PixelMap): ShapeAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ShapeAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ShapeAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSSheetAttribute extends CommonMethod<SheetAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): SheetAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): SheetAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): SheetAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSSliderAttribute extends CommonMethod<SliderAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: SliderOptions): SliderAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): SliderAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSSpanAttribute extends CommonMethod<SpanAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: string | Resource): SpanAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): SpanAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSStackAttribute extends CommonMethod<StackAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { alignContent?: Alignment }): StackAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): StackAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): StackAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSStepperAttribute extends CommonMethod<StepperAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { index?: number }): StepperAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): StepperAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): StepperAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSStepperItemAttribute extends CommonMethod<StepperItemAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): StepperItemAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): StepperItemAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): StepperItemAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSSwiperAttribute extends CommonMethod<SwiperAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(controller?: SwiperController): SwiperAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): SwiperAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): SwiperAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSTabContentAttribute extends CommonMethod<TabContentAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(): TabContentAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): TabContentAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): TabContentAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSTabsAttribute extends CommonMethod<TabsAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { barPosition?: BarPosition; index?: number; controller?: TabsController }): TabsAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): TabsAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): TabsAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSTextAttribute extends CommonMethod<TextAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(content?: string | Resource): TextAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): TextAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): TextAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSTextAreaAttribute extends CommonMethod<TextAreaAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: TextAreaOptions): TextAreaAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): TextAreaAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSTextClockAttribute extends CommonMethod<TextClockAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: { timeZoneOffset?: number; controller?: TextClockController }): TextClockAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): TextClockAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): TextClockAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSTextInputAttribute extends CommonMethod<TextInputAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: TextInputOptions): TextInputAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): TextInputAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSTextPickerAttribute extends CommonMethod<TextPickerAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: TextPickerOptions): TextPickerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): TextPickerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): TextPickerAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSTextTimerAttribute extends CommonMethod<TextTimerAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options?: TextTimerOptions): TextTimerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): TextTimerAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): TextTimerAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSToggleAttribute extends CommonMethod<ToggleAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(options: { type: ToggleType; isOn?: boolean }): ToggleAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): ToggleAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): ToggleAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSVideoAttribute extends CommonMethod<VideoAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: {
    src?: string | Resource;
    previewUri?: string | PixelMap | Resource;
    controller?: VideoController;
  }): VideoAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): VideoAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSWebAttribute extends CommonMethod<WebAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: WebOptions): WebAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): WebAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSXComponentAttribute extends CommonMethod<XComponentAttribute> {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value: {
    id: string;
    type: string;
    libraryname?: string;
    controller?: XComponentController;
  }): XComponentAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): XComponentAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSRichTextAttribute {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(content: string): RichTextAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): RichTextAttribute;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  debugLine(value: string): RichTextAttribute;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSTapGestureInterface {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { count?: number; fingers?: number }): TapGestureInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): TapGestureInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSLongPressGestureInterface {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { fingers?: number; repeat?: boolean; duration?: number }): LongPressGestureInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): LongPressGestureInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPanGestureInterface {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(
    value?:
      | {
          fingers?: number;
          direction?: PanDirection;
          distance?: number;
        }
      | PanGestureOptions,
  ): PanGestureInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): PanGestureInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSSwipeGestureInterface {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { fingers?: number; direction?: SwipeDirection; speed?: number }): SwipeGestureInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): SwipeGestureInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSPinchGestureInterface {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { fingers?: number; distance?: number }): PinchGestureInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): PinchGestureInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSRotationGestureInterface {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(value?: { fingers?: number; angle?: number }): RotationGestureInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): RotationGestureInterface;
}

/**
 * Used for TS compiler.
 * @ignore
 * @systemapi
 * @since 8
 */
declare class TSGestureGroupInterface {
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  create(mode: GestureMode, ...gesture: GestureType[]): GestureGroupInterface;
  /**
   * Used for TS compiler.
   * @ignore
   * @systemapi
   * @since 8
   */
  pop(): GestureGroupInterface;
}
