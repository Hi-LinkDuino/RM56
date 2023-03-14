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
    ../../../../../../../../third_party/icu/icu4c/source/common/cmemory.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/cstring.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/ubidi.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/ubidi_props.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/ubidiln.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/ubidiwrt.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/uchar.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/udataswp.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/uinvchar.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/ustring.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/utf_impl.cpp \
    ../../../../../../../../third_party/icu/icu4c/source/common/utrie2.cpp

INCLUDEPATH += \
    ../../../../../../../../third_party/icu/icu4c/source/common
