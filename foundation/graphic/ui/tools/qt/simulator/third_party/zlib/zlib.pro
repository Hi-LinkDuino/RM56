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
    ../../../../../../../../third_party/zlib/adler32.c \
    ../../../../../../../../third_party/zlib/compress.c \
    ../../../../../../../../third_party/zlib/crc32.c \
    ../../../../../../../../third_party/zlib/deflate.c \
    ../../../../../../../../third_party/zlib/infback.c \
    ../../../../../../../../third_party/zlib/inffast.c \
    ../../../../../../../../third_party/zlib/inflate.c \
    ../../../../../../../../third_party/zlib/inftrees.c \
    ../../../../../../../../third_party/zlib/trees.c \
    ../../../../../../../../third_party/zlib/uncompr.c \
    ../../../../../../../../third_party/zlib/zutil.c

INCLUDEPATH += \
    . \
    ../../../../../../../../third_party/zlib
