TEMPLATE = subdirs
CONFIG += ordered

FOUNDATION_PATH = ../../../../../..
COPY_DEST = $$replace(OUT_PWD, /, \\)
system("mkdir $$COPY_DEST\\..\\simulator\\font")
system("copy ..\\..\\..\\..\\..\\..\\graphic\\ui\\tools\\qt\\simulator\\font\\SourceHanSansSC-Regular.otf $$COPY_DEST\\..\\simulator\\font")


SUBDIRS += \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/third_party/freetype \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/third_party/harfbuzz \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/third_party/icu \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/third_party/libjpeg \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/third_party/zlib \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/third_party/libpng \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/third_party/giflib \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/third_party/qrcodegen \
    $${FOUNDATION_PATH}/graphic/ui/tools/qt/simulator/libui \
    cjson \
    jerryScript \
    jsfwk \
    entry/entry.pro

