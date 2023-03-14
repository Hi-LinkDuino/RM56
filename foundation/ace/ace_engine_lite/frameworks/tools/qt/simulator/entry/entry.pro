TEMPLATE = app
CONFIG += console c++11
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES -= UNICODE
DEFINES += QT_COMPILER
DEFINES += Q_OBJECT
DEFINES += QT_DEPRECATED_WARNINGS \
"ENABLE_ICU=1" \
"ENABLE_VECTOR_FONT=1"
DESTDIR = ../libs

ROOT_PATH = ../../../../../../../..
FOUNDATION_PATH = ../../../../../../..
ACELITE_PATH = ../../../../..
ACELITE_FRAMEWORK_PATH = ../../../..
UIDESTDIR = $${ROOT_PATH}/foundation/graphic/ui/tools/qt/simulator

INCLUDEPATH += \
    $${ACELITE_PATH}/interfaces/innerkits/builtin/async \
    $${ACELITE_PATH}/interfaces/innerkits/builtin/base \
    $${ACELITE_PATH}/interfaces/innerkits/builtin/jsi \
    $${ACELITE_FRAMEWORK_PATH}/include/base \
    $${FOUNDATION_PATH}/graphic/utils/interfaces/innerkits \
    $${FOUNDATION_PATH}/graphic/utils/interfaces/kits \
    $${FOUNDATION_PATH}/../third_party/freetype/include \
    $${FOUNDATION_PATH}/graphic/ui/interfaces/innerkits \
    $${FOUNDATION_PATH}/graphic/ui/frameworks \
    $${FOUNDATION_PATH}/graphic/ui/test/framework \
    $${FOUNDATION_PATH}/graphic/ui/interfaces/kits \
    $${FOUNDATION_PATH}/graphic/ui/interfaces/innerkits/common \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/uitest \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/display \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/indev \
    $${ACELITE_FRAMEWORK_PATH}/targets/simulator \
    $${ACELITE_FRAMEWORK_PATH}/include/context/ \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/utils \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock/jsthread \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock/vsyncthread \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock/amsthread \
    $${ACELITE_FRAMEWORK_PATH}/tools/qt/simulator/jsfwk/targets/simulator/mock/timerthread \
    $${FOUNDATION_PATH}/../utils/native/lite/timer_task/include/

HEADERS += \
    $${FOUNDATION_PATH}/graphic/ui/interfaces/innerkits/font/ui_font_vector.h \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/config.h \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/display/monitor.h \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/display/task_thread.h \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/display/gui_thread.h \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/indev/mouse_input.h \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/indev/mousewheel_input.h \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/uitest/main_widget.h \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/uitest/ui_mainwidget.h \
    $${ACELITE_FRAMEWORK_PATH}/include/context/js_ability.h \
    $${FOUNDATION_PATH}/graphic/ui/test/framework/common/ui_text_language.h \
    child_widget.h \
    simulator_config.h

SOURCES += \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/display/gui_thread.cpp \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/display/monitor.cpp \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/display/task_thread.cpp \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/indev/mouse_input.cpp \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/indev/mousewheel_input.cpp \
    $${FOUNDATION_PATH}/graphic/ui/test/framework/common/ui_text_language.cpp \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/drivers/indev/key_input.cpp \
    main_widget.cpp \
    child_widget.cpp \
    main.cpp \
    simulator_config.cpp

LIBS += $$DESTDIR/jsfwk.dll
LIBS += $$DESTDIR/jerryScript.dll
LIBS += $$UIDESTDIR/libs/libui.dll

COPY_DEST = $$replace(OUT_PWD, /, \\)
UILIBS = $$FOUNDATION_PATH/graphic/ui/tools/qt/simulator/libs
UI_LIBS = $$replace(UILIBS, /, \\)
QMAKE_POST_LINK += copy $$UI_LIBS\\freetype.dll $$COPY_DEST\\..\\libs\\freetype.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\icu.dll $$COPY_DEST\\..\\libs\\icu.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\libjpeg.dll $$COPY_DEST\\..\\libs\\libjpeg.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\libpng.dll $$COPY_DEST\\..\\libs\\libpng.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\libui.dll $$COPY_DEST\\..\\libs\\libui.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\giflib.dll $$COPY_DEST\\..\\libs\\giflib.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\qrcodegen.dll $$COPY_DEST\\..\\libs\\qrcodegen.dll &
QMAKE_POST_LINK += copy $$UI_LIBS\\zlib.dll $$COPY_DEST\\..\\libs\\zlib.dll
