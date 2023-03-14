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
    ../../../../../../../../third_party/libjpeg/cdjpeg.c \
    ../../../../../../../../third_party/libjpeg/jaricom.c \
    ../../../../../../../../third_party/libjpeg/jcapimin.c \
    ../../../../../../../../third_party/libjpeg/jcapistd.c \
    ../../../../../../../../third_party/libjpeg/jcarith.c \
    ../../../../../../../../third_party/libjpeg/jccoefct.c \
    ../../../../../../../../third_party/libjpeg/jccolor.c \
    ../../../../../../../../third_party/libjpeg/jcdctmgr.c \
    ../../../../../../../../third_party/libjpeg/jchuff.c \
    ../../../../../../../../third_party/libjpeg/jcinit.c \
    ../../../../../../../../third_party/libjpeg/jcmainct.c \
    ../../../../../../../../third_party/libjpeg/jcmarker.c \
    ../../../../../../../../third_party/libjpeg/jcmaster.c \
    ../../../../../../../../third_party/libjpeg/jcomapi.c \
    ../../../../../../../../third_party/libjpeg/jcparam.c \
    ../../../../../../../../third_party/libjpeg/jcprepct.c \
    ../../../../../../../../third_party/libjpeg/jcsample.c \
    ../../../../../../../../third_party/libjpeg/jctrans.c \
    ../../../../../../../../third_party/libjpeg/jdapimin.c \
    ../../../../../../../../third_party/libjpeg/jdapistd.c \
    ../../../../../../../../third_party/libjpeg/jdarith.c \
    ../../../../../../../../third_party/libjpeg/jdatadst.c \
    ../../../../../../../../third_party/libjpeg/jdatasrc.c \
    ../../../../../../../../third_party/libjpeg/jdcoefct.c \
    ../../../../../../../../third_party/libjpeg/jdcolor.c \
    ../../../../../../../../third_party/libjpeg/jddctmgr.c \
    ../../../../../../../../third_party/libjpeg/jdhuff.c \
    ../../../../../../../../third_party/libjpeg/jdinput.c \
    ../../../../../../../../third_party/libjpeg/jdmainct.c \
    ../../../../../../../../third_party/libjpeg/jdmarker.c \
    ../../../../../../../../third_party/libjpeg/jdmaster.c \
    ../../../../../../../../third_party/libjpeg/jdmerge.c \
    ../../../../../../../../third_party/libjpeg/jdpostct.c \
    ../../../../../../../../third_party/libjpeg/jdsample.c \
    ../../../../../../../../third_party/libjpeg/jdtrans.c \
    ../../../../../../../../third_party/libjpeg/jerror.c \
    ../../../../../../../../third_party/libjpeg/jfdctflt.c \
    ../../../../../../../../third_party/libjpeg/jfdctfst.c \
    ../../../../../../../../third_party/libjpeg/jfdctint.c \
    ../../../../../../../../third_party/libjpeg/jidctflt.c \
    ../../../../../../../../third_party/libjpeg/jidctfst.c \
    ../../../../../../../../third_party/libjpeg/jidctint.c \
    ../../../../../../../../third_party/libjpeg/jmemmgr.c \
    ../../../../../../../../third_party/libjpeg/jmemnobs.c \
    ../../../../../../../../third_party/libjpeg/jquant1.c \
    ../../../../../../../../third_party/libjpeg/jquant2.c \
    ../../../../../../../../third_party/libjpeg/jutils.c \
    ../../../../../../../../third_party/libjpeg/rdbmp.c \
    ../../../../../../../../third_party/libjpeg/rdcolmap.c \
    ../../../../../../../../third_party/libjpeg/rdgif.c \
    ../../../../../../../../third_party/libjpeg/rdppm.c \
    ../../../../../../../../third_party/libjpeg/rdrle.c \
    ../../../../../../../../third_party/libjpeg/rdswitch.c \
    ../../../../../../../../third_party/libjpeg/rdtarga.c \
    ../../../../../../../../third_party/libjpeg/transupp.c

INCLUDEPATH += \
    ../../../../../../../third_party/libjpeg
