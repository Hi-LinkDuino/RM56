CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

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
    ../../../../../../../../third_party/harfbuzz/src/hb-aat-layout.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-aat-map.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-blob.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-buffer-serialize.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-buffer.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-common.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-face.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-fallback-shape.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-font.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-map.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-number.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-cff1-table.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-cff2-table.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-face.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-font.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-layout.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-map.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-math.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-meta.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-metrics.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-name.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-arabic.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-default.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-hangul.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-hebrew.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-indic-table.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-indic.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-khmer.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-myanmar.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-syllabic.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-thai.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-use.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-complex-vowel-constraints.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-fallback.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape-normalize.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-shape.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-tag.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ot-var.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-set.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-shape-plan.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-shape.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-shaper.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-static.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-subset-cff2.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-subset-cff-common.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-ucd.cc \
    ../../../../../../../../third_party/harfbuzz/src/hb-unicode.cc

INCLUDEPATH += \
    ../../../../../../../../third_party/harfbuzz/src
