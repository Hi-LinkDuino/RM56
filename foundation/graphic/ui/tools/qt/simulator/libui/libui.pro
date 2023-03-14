CONFIG -= qt

TEMPLATE = lib

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
    CJSON_HIDE_SYMBOLS \
    "ENABLE_SHAPING=0" \
    "ENABLE_ICU=1" \
    "ENABLE_VECTOR_FONT=1" \
    "ENABLE_BITMAP_FONT=0" \
    "ENABLE_MULTI_FONT=0" \
    "ENABLE_STATIC_FONT=0" \
    "GRAPHIC_ENABLE_LINECAP_FLAG=1" \
    "GRAPHIC_ENABLE_LINEJOIN_FLAG=1" \
    "GRAPHIC_ENABLE_ELLIPSE_FLAG=1" \
    "GRAPHIC_ENABLE_BEZIER_ARC_FLAG=1" \
    "GRAPHIC_ENABLE_ARC_FLAG=1" \
    "GRAPHIC_ENABLE_ROUNDEDRECT_FLAG=1" \
    "GRAPHIC_ENABLE_DASH_GENERATE_FLAG=1" \
    "GRAPHIC_ENABLE_BLUR_EFFECT_FLAG=1" \
    "GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG=1" \
    "GRAPHIC_ENABLE_GRADIENT_FILL_FLAG=1" \
    "GRAPHIC_ENABLE_PATTERN_FILL_FLAG=1" \
    "GRAPHIC_ENABLE_DRAW_IMAGE_FLAG=1" \
    "GRAPHIC_ENABLE_DRAW_TEXT_FLAG=1" \
    "DEFAULT_ANIMATION=1"


DEFINES += QT_COMPILER

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = ../libs

SOURCES += \
    ../../../../../utils/frameworks/diagram/depiction/depict_curve.cpp \
    ../../../../../utils/frameworks/diagram/rasterizer/rasterizer_cells_antialias.cpp \
    ../../../../../utils/frameworks/diagram/rasterizer/rasterizer_scanline_antialias.cpp \
    ../../../../../utils/frameworks/diagram/rasterizer/rasterizer_scanline_clip.cpp \
    ../../../../../utils/frameworks/graphic_timer.cpp \
    ../../../../../utils/frameworks/trans_affine.cpp \
    ../../../../../utils/frameworks/diagram/vertexgenerate/vertex_generate_dash.cpp \
    ../../../../../utils/frameworks/diagram/vertexgenerate/vertex_generate_stroke.cpp \
    ../../../../../utils/frameworks/diagram/vertexprimitive/geometry_arc.cpp \
    ../../../../../utils/frameworks/diagram/vertexprimitive/geometry_bezier_arc.cpp \
    ../../../../../utils/frameworks/diagram/vertexprimitive/geometry_curves.cpp \
    ../../../../../utils/frameworks/diagram/vertexprimitive/geometry_shorten_path.cpp \
    ../../../../frameworks/animator/animator.cpp \
    ../../../../frameworks/animator/animator_manager.cpp \
    ../../../../frameworks/animator/easing_equation.cpp \
    ../../../../frameworks/animator/interpolation.cpp \
    ../../../../frameworks/common/graphic_startup.cpp \
    ../../../../frameworks/common/image_decode_ability.cpp \
    ../../../../frameworks/common/image.cpp \
    ../../../../frameworks/common/input_device_manager.cpp \
    ../../../../frameworks/common/screen.cpp \
    ../../../../frameworks/common/task.cpp \
    ../../../../frameworks/common/text.cpp \
    ../../../../frameworks/common/spannable_string.cpp \
    ../../../../frameworks/common/typed_text.cpp \
    ../../../../frameworks/components/root_view.cpp \
    ../../../../frameworks/components/text_adapter.cpp \
    ../../../../frameworks/components/ui_abstract_clock.cpp \
    ../../../../frameworks/components/ui_abstract_progress.cpp \
    ../../../../frameworks/components/ui_abstract_scroll.cpp \
    ../../../../frameworks/components/ui_abstract_scroll_bar.cpp \
    ../../../../frameworks/components/ui_analog_clock.cpp \
    ../../../../frameworks/components/ui_arc_label.cpp \
    ../../../../frameworks/components/ui_arc_scroll_bar.cpp \
    ../../../../frameworks/components/ui_axis.cpp \
    ../../../../frameworks/components/ui_box_progress.cpp \
    ../../../../frameworks/components/ui_box_scroll_bar.cpp \
    ../../../../frameworks/components/ui_button.cpp \
    ../../../../frameworks/components/ui_canvas.cpp \
    ../../../../frameworks/components/ui_chart.cpp \
    ../../../../frameworks/components/ui_checkbox.cpp \
    ../../../../frameworks/components/ui_circle_progress.cpp \
    ../../../../frameworks/components/ui_dialog.cpp \
    ../../../../frameworks/components/ui_digital_clock.cpp \
    ../../../../frameworks/components/ui_image_animator.cpp \
    ../../../../frameworks/components/ui_image_view.cpp \
    ../../../../frameworks/components/ui_label.cpp \
    ../../../../frameworks/components/ui_label_button.cpp \
    ../../../../frameworks/components/ui_list.cpp \
    ../../../../frameworks/components/ui_picker.cpp \
    ../../../../frameworks/components/ui_qrcode.cpp \
    ../../../../frameworks/components/ui_radio_button.cpp \
    ../../../../frameworks/components/ui_repeat_button.cpp \
    ../../../../frameworks/components/ui_scroll_view.cpp \
    ../../../../frameworks/components/ui_slider.cpp \
    ../../../../frameworks/components/ui_swipe_view.cpp \
    ../../../../frameworks/components/ui_texture_mapper.cpp \
    ../../../../frameworks/components/ui_time_picker.cpp \
    ../../../../frameworks/components/ui_toggle_button.cpp \
    ../../../../frameworks/components/ui_view.cpp \
    ../../../../frameworks/components/ui_view_group.cpp \
    ../../../../frameworks/components/ui_extend_image_view.cpp \
    ../../../../frameworks/dock/focus_manager.cpp \
    ../../../../frameworks/core/render_manager.cpp \
    ../../../../frameworks/core/task_manager.cpp \
    ../../../../frameworks/default_resource/check_box_res.cpp \
    ../../../../frameworks/dfx/event_injector.cpp \
    ../../../../frameworks/dfx/key_event_injector.cpp \
    ../../../../frameworks/dfx/performance_task.cpp \
    ../../../../frameworks/dfx/point_event_injector.cpp \
    ../../../../frameworks/dfx/ui_dump_dom_tree.cpp \
    ../../../../frameworks/dfx/ui_view_bounds.cpp \
    ../../../../frameworks/dock/input_device.cpp \
    ../../../../frameworks/dock/key_input_device.cpp \
    ../../../../frameworks/dock/pointer_input_device.cpp \
    ../../../../frameworks/dock/rotate_input_device.cpp \
    ../../../../frameworks/dock/screen_device_proxy.cpp \
    ../../../../frameworks/dock/vibrator_manager.cpp \
    ../../../../frameworks/dock/virtual_input_device.cpp \
    ../../../../frameworks/engines/gfx/gfx_engine_manager.cpp \
    ../../../../frameworks/draw/clip_utils.cpp \
    ../../../../frameworks/draw/draw_arc.cpp \
    ../../../../frameworks/draw/draw_curve.cpp \
    ../../../../frameworks/draw/draw_image.cpp \
    ../../../../frameworks/draw/draw_label.cpp \
    ../../../../frameworks/draw/draw_line.cpp \
    ../../../../frameworks/draw/draw_rect.cpp \
    ../../../../frameworks/draw/draw_triangle.cpp \
    ../../../../frameworks/draw/draw_utils.cpp \
    ../../../../frameworks/events/event.cpp \
    ../../../../frameworks/font/base_font.cpp \
    ../../../../frameworks/font/font_ram_allocator.cpp \
    ../../../../frameworks/font/glyphs_cache.cpp \
    ../../../../frameworks/font/glyphs_file.cpp \
    ../../../../frameworks/font/glyphs_manager.cpp \
    ../../../../frameworks/font/ui_font.cpp \
    ../../../../frameworks/font/ui_font_adaptor.cpp \
    ../../../../frameworks/font/ui_font_allocator.cpp \
    ../../../../frameworks/font/ui_font_bitmap.cpp \
    ../../../../frameworks/font/ui_font_builder.cpp \
    ../../../../frameworks/font/ui_font_cache.cpp \
    ../../../../frameworks/font/ui_font_vector.cpp \
    ../../../../frameworks/font/ui_line_break.cpp \
    ../../../../frameworks/font/ui_multi_font_manager.cpp \
    ../../../../frameworks/font/ui_text_shaping.cpp \
    ../../../../frameworks/imgdecode/cache_manager.cpp \
    ../../../../frameworks/imgdecode/file_img_decoder.cpp \
    ../../../../frameworks/imgdecode/image_load.cpp \
    ../../../../frameworks/layout/flex_layout.cpp \
    ../../../../frameworks/layout/grid_layout.cpp \
    ../../../../frameworks/layout/list_layout.cpp \
    ../../../../frameworks/render/render_base.cpp \
    ../../../../frameworks/render/render_pixfmt_rgba_blend.cpp \
    ../../../../frameworks/themes/theme.cpp \
    ../../../../frameworks/themes/theme_manager.cpp \
    ../../../../../utils/frameworks/color.cpp \
    ../../../../../utils/frameworks/geometry2d.cpp \
    ../../../../../utils/frameworks/graphic_math.cpp \
    ../../../../../utils/frameworks/graphic_performance.cpp \
    ../../../../../utils/frameworks/hal_tick.cpp \
    ../../../../../utils/frameworks/mem_api.cpp \
    ../../../../../utils/frameworks/style.cpp \
    ../../../../../utils/frameworks/sys_info.cpp \
    ../../../../../utils/frameworks/transform.cpp \
    ../../../../../utils/frameworks/version.cpp \
    ../../../../../../../third_party/bounds_checking_function/src/memset_s.c \
    ../../../../../../../third_party/cJSON/cJSON.c \
    ../../../../frameworks/render/render_scanline.cpp \

HEADERS += \
    ../../../../../utils/interfaces/innerkits/graphic_neon_pipeline.h \
    ../../../../../utils/interfaces/innerkits/graphic_neon_utils.h \
    ../../../../../utils/interfaces/innerkits/graphic_timer.h \
    ../../../../../utils/interfaces/kits/gfx_utils/color.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/common/common_basics.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/common/common_clip_operate.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/common/common_math.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/depiction/depict_adaptor_vertex_generate.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/depiction/depict_curve.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/depiction/depict_dash.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/depiction/depict_stroke.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/depiction/depict_transform.h \
   ../../../../../utils/interfaces/kits/gfx_utils/diagram/imagefilter/filter_blur.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/rasterizer/rasterizer_cells_antialias.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/rasterizer/rasterizer_scanline_antialias.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/rasterizer/rasterizer_scanline_clip.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/scanline/geometry_scanline.h \
     ../../../../../utils/interfaces/kits/gfx_utils/diagram/spancolorfill/fill_base.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/spancolorfill/fill_gradient.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/spancolorfill/fill_gradient_lut.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/spancolorfill/fill_interpolator.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/spancolorfill/fill_pattern_rgba.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertex_generate/vertex_generate_dash.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertex_generate/vertex_generate_stroke.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_arc.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_array.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_bezier_arc.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_curves.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_dda_line.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_ellipse.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_math_stroke.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_path_storage.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_plaindata_array.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_range_adapter.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_shorten_path.h \
    ../../../../../utils/interfaces/kits/gfx_utils/diagram/vertexprimitive/geometry_vertex_sequence.h \
    ../../../../frameworks/common/typed_text.h \
    ../../../../frameworks/core/render_manager.h \
    ../../../../frameworks/default_resource/check_box_res.h \
    ../../../../frameworks/dfx/key_event_injector.h \
    ../../../../frameworks/dfx/point_event_injector.h \
    ../../../../frameworks/dfx/ui_view_bounds.h \
    ../../../../frameworks/dock/input_device.h \
    ../../../../frameworks/dock/pointer_input_device.h \
    ../../../../frameworks/dock/virtual_input_device.h \
    ../../../../frameworks/draw/clip_utils.h \
    ../../../../frameworks/draw/draw_arc.h \
    ../../../../frameworks/draw/draw_curve.h \
    ../../../../frameworks/draw/draw_image.h \
    ../../../../frameworks/draw/draw_label.h \
    ../../../../frameworks/draw/draw_line.h \
    ../../../../frameworks/draw/draw_rect.h \
    ../../../../frameworks/draw/draw_triangle.h \
    ../../../../frameworks/draw/draw_utils.h \
    ../../../../frameworks/font/ui_font_adaptor.h \
    ../../../../frameworks/font/ui_multi_font_manager.h \
    ../../../../frameworks/imgdecode/cache_manager.h \
    ../../../../frameworks/imgdecode/file_img_decoder.h \
    ../../../../frameworks/imgdecode/image_load.h \
    ../../../../frameworks/render/render_base.h \
    ../../../../frameworks/render/render_buffer.h \
    ../../../../frameworks/render/render_pixfmt_rgba_blend.h \
    ../../../../frameworks/render/render_scanline.h \
    ../../../../interfaces/innerkits/common/graphic_startup.h \
    ../../../../interfaces/innerkits/common/image_decode_ability.h \
    ../../../../interfaces/innerkits/common/input_device_manager.h \
    ../../../../interfaces/innerkits/common/task_manager.h \
    ../../../../interfaces/innerkits/dock/focus_manager.h \
    ../../../../interfaces/innerkits/dock/rotate_input_device.h \
    ../../../../interfaces/innerkits/dock/vibrator_manager.h \
    ../../../../interfaces/innerkits/font/ui_font_builder.h \
    ../../../../interfaces/innerkits/engines/gfx/gfx_engine_manager.h \
    ../../../../interfaces/kits/animator/animator.h \
    ../../../../interfaces/kits/animator/easing_equation.h \
    ../../../../interfaces/kits/animator/interpolation.h \
    ../../../../interfaces/kits/common/image.h \
    ../../../../interfaces/kits/common/screen.h \
    ../../../../interfaces/kits/common/task.h \
    ../../../../interfaces/kits/common/text.h \
    ../../../../interfaces/kits/common/spannable_string.h \
    ../../../../interfaces/kits/components/abstract_adapter.h \
    ../../../../interfaces/kits/components/root_view.h \
    ../../../../interfaces/kits/components/text_adapter.h \
    ../../../../interfaces/kits/components/ui_abstract_clock.h \
    ../../../../interfaces/kits/components/ui_abstract_progress.h \
    ../../../../interfaces/kits/components/ui_abstract_scroll.h \
    ../../../../interfaces/kits/components/ui_analog_clock.h \
    ../../../../interfaces/kits/components/ui_arc_label.h \
    ../../../../interfaces/kits/components/ui_axis.h \
    ../../../../interfaces/kits/components/ui_box_progress.h \
    ../../../../interfaces/kits/components/ui_button.h \
    ../../../../interfaces/kits/components/ui_canvas.h \
    ../../../../interfaces/kits/components/ui_chart.h \
    ../../../../interfaces/kits/components/ui_checkbox.h \
    ../../../../interfaces/kits/components/ui_circle_progress.h \
    ../../../../interfaces/kits/components/ui_dialog.h \
    ../../../../interfaces/kits/components/ui_digital_clock.h \
    ../../../../interfaces/kits/components/ui_image_animator.h \
    ../../../../interfaces/kits/components/ui_image_view.h \
    ../../../../interfaces/kits/components/ui_label.h \
    ../../../../interfaces/kits/components/ui_label_button.h \
    ../../../../interfaces/kits/components/ui_list.h \
    ../../../../interfaces/kits/components/ui_picker.h \
    ../../../../interfaces/kits/components/ui_qrcode.h \
    ../../../../interfaces/kits/components/ui_radio_button.h \
    ../../../../interfaces/kits/components/ui_repeat_button.h \
    ../../../../interfaces/kits/components/ui_scroll_view.h \
    ../../../../interfaces/kits/components/ui_slider.h \
    ../../../../interfaces/kits/components/ui_swipe_view.h \
    ../../../../interfaces/kits/components/ui_texture_mapper.h \
    ../../../../interfaces/kits/components/ui_time_picker.h \
    ../../../../interfaces/kits/components/ui_toggle_button.h \
    ../../../../interfaces/kits/components/ui_view.h \
    ../../../../interfaces/kits/components/ui_view_group.h \
    ../../../../interfaces/kits/components/ui_extend_image_view.h \
    ../../../../interfaces/kits/dfx/event_injector.h \
    ../../../../interfaces/kits/dfx/ui_dump_dom_tree.h \
    ../../../../interfaces/kits/events/aod_callback.h \
    ../../../../interfaces/kits/events/cancel_event.h \
    ../../../../interfaces/kits/events/click_event.h \
    ../../../../interfaces/kits/events/drag_event.h \
    ../../../../interfaces/kits/events/event.h \
    ../../../../interfaces/kits/events/long_press_event.h \
    ../../../../interfaces/kits/events/press_event.h \
    ../../../../interfaces/kits/events/release_event.h \
    ../../../../interfaces/kits/events/virtual_device_event.h \
    ../../../../interfaces/kits/font/ui_font.h \
    ../../../../interfaces/kits/font/ui_font_header.h \
    ../../../../interfaces/kits/layout/flex_layout.h \
    ../../../../interfaces/kits/layout/grid_layout.h \
    ../../../../interfaces/kits/layout/layout.h \
    ../../../../interfaces/kits/layout/list_layout.h \
    ../../../../interfaces/kits/themes/theme.h \
    ../../../../interfaces/kits/themes/theme_manager.h \
    ../../../../interfaces/kits/window/window.h \
    ../../../../../utils/interfaces/innerkits/graphic_config.h \
    ../../../../../utils/interfaces/innerkits/graphic_locker.h \
    ../../../../../utils/interfaces/innerkits/graphic_semaphore.h \
    ../../../../../utils/interfaces/innerkits/hal_tick.h \
    ../../../../../utils/interfaces/kits/color.h \
    ../../../../../utils/interfaces/kits/file.h \
    ../../../../../utils/interfaces/kits/geometry2d.h \
    ../../../../../utils/interfaces/kits/graphic_assert.h \
    ../../../../../utils/interfaces/kits/graphic_log.h \
    ../../../../../utils/interfaces/kits/graphic_math.h \
    ../../../../../utils/interfaces/kits/graphic_types.h \
    ../../../../../utils/interfaces/kits/heap_base.h \
    ../../../../../utils/interfaces/kits/image_info.h \
    ../../../../../utils/interfaces/kits/input_event_info.h \
    ../../../../../utils/interfaces/kits/list.h \
    ../../../../../utils/interfaces/kits/mem_api.h \
    ../../../../../utils/interfaces/kits/rect.h \
    ../../../../../utils/interfaces/kits/style.h \
    ../../../../../utils/interfaces/kits/sys_info.h \
    ../../../../../utils/interfaces/kits/transform.h \
    ../../../../../utils/interfaces/kits/version.h \
    ../../../../../wms/interfaces/innerkits/lite_wm_type.h \
    ../../../../../../../third_party/bounds_checking_function/include/securec.h \
    ../../../../../../../third_party/bounds_checking_function/include/securectype.h \
    ../../../../../../../third_party/bounds_checking_function/src/input.inl \
    ../../../../../../../third_party/bounds_checking_function/src/output.inl \
    ../../../../../../../third_party/bounds_checking_function/src/secinput.h \
    ../../../../../../../third_party/bounds_checking_function/src/securecutil.h \
    ../../../../../../../third_party/bounds_checking_function/src/secureprintoutput.h \
    ../../../../../../../third_party/cJSON/cJSON.h \
    ../../../../../../../third_party/cJSON/cJSON_Utils.h \
    ../../../../../../../tools/developer_tools_lite/graphic_tool/iar_project/config/gpu_2d/graphic_config.h


INCLUDEPATH += \
    ../../../../frameworks \
    ../../../../frameworks/diagram/include \
    ../../../../../utils/frameworks/windows \
    ../../../../../utils/interfaces/innerkits \
    ../../../../../utils/interfaces/kits \
    ../../../../../ui/interfaces/innerkits \
    ../../../../../ui/interfaces/kits \
    ../../../../../../../third_party/bounds_checking_function/include \
    ../../../../../../../third_party/harfbuzz/src \
    ../../../../../../../third_party/icu/icu4c/source/common \
    ../../../../../../../third_party/freetype/include \
    ../../../../../../../third_party/cJSON \
    ../../../../../../../third_party/libjpeg \
    ../../../../../../../third_party/libpng \
    ../../../../../../../third_party/qrcodegen/cpp \
    ../../../../../../../third_party/giflib

LIBS += $$OUT_PWD/../libs/libpng.dll
LIBS += $$OUT_PWD/../libs/libjpeg.dll
LIBS += $$OUT_PWD/../libs/qrcodegen.dll
LIBS += $$OUT_PWD/../libs/freetype.dll
LIBS += $$OUT_PWD/../libs/libharfbuzz.a
LIBS += $$OUT_PWD/../libs/icu.dll
LIBS += $$OUT_PWD/../libs/giflib.dll
