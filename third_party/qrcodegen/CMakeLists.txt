cmake_minimum_required(VERSION 3.16.5)
set(QrcodegenSrc "${PROJECT_SOURCE_DIR}/third_party/qrcodegen")

include_directories(${QrcodegenSrc})

add_library(qrcodegen STATIC
    ${QrcodegenSrc}/cpp/qrcodegen.cpp
    )

target_include_directories(qrcodegen PUBLIC ${QrcodegenSrc}/cpp)
