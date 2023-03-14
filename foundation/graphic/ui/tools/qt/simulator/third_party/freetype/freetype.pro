CONFIG -= qt

TEMPLATE = lib
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
    FT2_BUILD_LIBRARY \
    FT_DEBUG_LEVEL_ERROR \
    FT_DEBUG_LEVEL_TRACE \
    FT_CONFIG_OPTION_USE_PNG \

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DESTDIR = ../../libs

SOURCES += \
    ../../../../../../../../third_party/freetype/src/autofit/autofit.c \
    ../../../../../../../../third_party/freetype/src/base/ftbase.c \
    ../../../../../../../../third_party/freetype/src/base/ftbbox.c \
    ../../../../../../../../third_party/freetype/src/base/ftbdf.c \
    ../../../../../../../../third_party/freetype/src/base/ftbitmap.c \
    ../../../../../../../../third_party/freetype/src/base/ftcid.c \
    ../../../../../../../../third_party/freetype/src/base/ftfstype.c \
    ../../../../../../../../third_party/freetype/src/base/ftgasp.c \
    ../../../../../../../../third_party/freetype/src/base/ftglyph.c \
    ../../../../../../../../third_party/freetype/src/base/ftgxval.c \
    ../../../../../../../../third_party/freetype/src/base/ftinit.c \
    ../../../../../../../../third_party/freetype/src/base/ftmm.c \
    ../../../../../../../../third_party/freetype/src/base/ftotval.c \
    ../../../../../../../../third_party/freetype/src/base/ftpatent.c \
    ../../../../../../../../third_party/freetype/src/base/ftpfr.c \
    ../../../../../../../../third_party/freetype/src/base/ftstroke.c \
    ../../../../../../../../third_party/freetype/src/base/ftsynth.c \
    ../../../../../../../../third_party/freetype/src/base/ftsystem.c \
    ../../../../../../../../third_party/freetype/src/base/fttype1.c \
    ../../../../../../../../third_party/freetype/src/base/ftwinfnt.c \
    ../../../../../../../../third_party/freetype/src/bdf/bdf.c \
    ../../../../../../../../third_party/freetype/src/cache/ftcache.c \
    ../../../../../../../../third_party/freetype/src/cff/cff.c \
    ../../../../../../../../third_party/freetype/src/cid/type1cid.c \
    ../../../../../../../../third_party/freetype/src/gzip/ftgzip.c \
    ../../../../../../../../third_party/freetype/src/lzw/ftlzw.c \
    ../../../../../../../../third_party/freetype/src/pcf/pcf.c \
    ../../../../../../../../third_party/freetype/src/pfr/pfr.c \
    ../../../../../../../../third_party/freetype/src/psaux/psaux.c \
    ../../../../../../../../third_party/freetype/src/pshinter/pshinter.c \
    ../../../../../../../../third_party/freetype/src/psnames/psmodule.c \
    ../../../../../../../../third_party/freetype/src/raster/raster.c \
    ../../../../../../../../third_party/freetype/src/sfnt/sfnt.c \
    ../../../../../../../../third_party/freetype/src/smooth/smooth.c \
    ../../../../../../../../third_party/freetype/src/truetype/truetype.c \
    ../../../../../../../../third_party/freetype/src/type1/type1.c \
    ../../../../../../../../third_party/freetype/src/type42/type42.c \
    ../../../../../../../../third_party/freetype/src/winfonts/winfnt.c \
    ../../../../../../../../third_party/freetype/builds/windows/ftdebug.c

INCLUDEPATH += \
    ../../../../../../../../third_party/freetype/include \
    ../../../../../../../../third_party/libpng

LIBS += $$OUT_PWD/../../libs/libpng.dll
