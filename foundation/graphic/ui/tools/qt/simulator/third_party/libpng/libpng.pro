CONFIG -= qt

TEMPLATE = lib
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
    ../../../../../../../../third_party/libpng/png.c \
    ../../../../../../../../third_party/libpng/pngerror.c \
    ../../../../../../../../third_party/libpng/pngget.c \
    ../../../../../../../../third_party/libpng/pngmem.c \
    ../../../../../../../../third_party/libpng/pngpread.c \
    ../../../../../../../../third_party/libpng/pngread.c \
    ../../../../../../../../third_party/libpng/pngrio.c \
    ../../../../../../../../third_party/libpng/pngrtran.c \
    ../../../../../../../../third_party/libpng/pngrutil.c \
    ../../../../../../../../third_party/libpng/pngset.c \
    ../../../../../../../../third_party/libpng/pngtrans.c \
    ../../../../../../../../third_party/libpng/pngwio.c \
    ../../../../../../../../third_party/libpng/pngwrite.c \
    ../../../../../../../../third_party/libpng/pngwtran.c \
    ../../../../../../../../third_party/libpng/pngwutil.c

INCLUDEPATH += \
    ../../../../../../../../third_party/libpng \
    ../../../../../../../../third_party/zlib

LIBS += $${DESTDIR}/zlib.dll
