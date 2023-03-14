TEMPLATE = lib
CONFIG += staticlib
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DESTDIR = ../libs

ROOT_PATH = ../../../../../../../..

HEADERS = $${ROOT_PATH}/third_party/cJSON/cJSON.h
SOURCES += \
        $${ROOT_PATH}/third_party/cJSON/cJSON.c
