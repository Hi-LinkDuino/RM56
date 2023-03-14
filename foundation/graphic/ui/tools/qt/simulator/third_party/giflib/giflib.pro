CONFIG -= qt

TEMPLATE = lib
#DEFINES += GIFLIB_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = ../../libs

SOURCES += \
    ../../../../../../../../third_party/giflib/dgif_lib.c \
    ../../../../../../../../third_party/giflib/egif_lib.c \
    ../../../../../../../../third_party/giflib/getarg.c \
    ../../../../../../../../third_party/giflib/gif_err.c \
    ../../../../../../../../third_party/giflib/gif_font.c \
    ../../../../../../../../third_party/giflib/gif_hash.c \
    ../../../../../../../../third_party/giflib/gif2rgb.c \
    ../../../../../../../../third_party/giflib/gifalloc.c \
    ../../../../../../../../third_party/giflib/openbsd-reallocarray.c \
    ../../../../../../../../third_party/giflib/qprintf.c \
    ../../../../../../../../third_party/giflib/quantize.c

HEADERS += \
    ../../../../../../../../third_party/giflib/getarg.h \
    ../../../../../../../../third_party/giflib/gif_hash.h \
    ../../../../../../../../third_party/giflib/gif_lib.h \
    ../../../../../../../../third_party/giflib/gif_lib_private.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
