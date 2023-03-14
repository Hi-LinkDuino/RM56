
include(${TOP_DIR}/engine/CMakeLists.txt)

set(GTEST_DIRS ${THIRD_PARTY_DIR}/gtest)
set(MOCKCPP_DIR ${THIRD_PARTY_DIR}/mockcpp)

include_directories(
        ${GTEST_ROOT_DIR}/include
        ${MOCKCPP_DIR}/include
)

set(ffmpeg_inc_path ${THIRD_PARTY_DIR}/ffmpeg/windows/include)
set(sdl_inc_path ${THIRD_PARTY_DIR}/SDL2.0/include)

if (MINGW)
set(ffmpeg_lib_path ${THIRD_PARTY_DIR}/ffmpeg/windows/lib)
set(sdl_lib_path ${THIRD_PARTY_DIR}/SDL2.0/windows/lib/x64)
set(gtest_lib_path ${GTEST_ROOT_DIR}/mingw64/lib/)
else()
    set(ffmpeg_lib_path ${THIRD_PARTY_DIR}/ffmpeg/linux/lib)
    set(ffmpeg_inc_path ${THIRD_PARTY_DIR}/ffmpeg/linux/include)
    set(sdl_lib_path ${THIRD_PARTY_DIR}/SDL2.0/linux/lib)
    set(sdl_inc_path ${THIRD_PARTY_DIR}/SDL2.0/linux/include)
    set(gtest_lib_path ${THIRD_PARTY_DIR}/gtest/linux/lib)
    set(curl_lib_path ${THIRD_PARTY_DIR}/curl/lib/linux)
    message(STATUS "ut ${ffmpeg_lib_path}")
endif()

include_directories(
        ${ffmpeg_inc_path}
        ${sdl_inc_path}
)
link_directories(
        ${ffmpeg_lib_path}
        ${sdl_lib_path}
        ${gtest_lib_path}
        ${curl_lib_path}
)

file(GLOB UT_TEST_SRCS ./*.cpp)

file(GLOB UT_TEST_PLUGINS ./plugins/*.cpp)

set(SRC
        ${HISTREAMER_SRCS}
        ${UT_TEST_SRCS}
        ${UT_TEST_PLUGINS}
        ${3RDPARTY_SRCS}
        ../main.cpp
        )

add_executable(HiStreamerUtTests ${SRC})

link_directories(
        ${MOCKCPP_DIR}/lib/
        /usr/local/lib
)

target_compile_definitions(HiStreamerUtTests PRIVATE UNIT_TEST)

if (MSVC)
    target_link_libraries(HiStreamerUtTests
            ${GTEST_ROOT_DIR}/lib/gtestd.lib
            ${MOCKCPP_DIR}/lib/mockcpp.lib
            pthreadVC2.lib
            )
elseif (MINGW)
    set(ffmpeg_path ${THIRD_PARTY_DIR}/ffmpeg/windows)
    set(sdl_path ${THIRD_PARTY_DIR}/SDL2.0/windows)
    target_link_libraries(HiStreamerUtTests
            # dl
            ${sdl_path}/lib/x64/SDL2.lib
            ${ffmpeg_path}/lib/avcodec.lib
            ${ffmpeg_path}/lib/swresample.lib
            ${ffmpeg_path}/lib/avformat.lib
            ${ffmpeg_path}/lib/avutil.lib
            ${ffmpeg_path}/lib/avdevice.lib
            ${ffmpeg_path}/lib/avfilter.lib
            ${ffmpeg_path}/lib/swscale.lib
            gtest
            ${MOCKCPP_DIR}/lib/libmockcpp_mingw8.a
            )
    message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
    file(GLOB ffmpeg_shared_libraries ${ffmpeg_path}/bin/*.dll)
    file(GLOB sdl_shared_libraries ${sdl_path}/lib/x64/*.dll)
    file(COPY ${ffmpeg_shared_libraries} DESTINATION ${CMAKE_BINARY_DIR}/_deps/histreamer-build/tests/ut)
    file(COPY ${sdl_shared_libraries} DESTINATION ${CMAKE_BINARY_DIR}/_deps/histreamer-build/tests/ut)
else ()
    target_link_libraries(HiStreamerUtTests
            dl
            ${ffmpeg_lib_path}/libavformat.a
            ${ffmpeg_lib_path}/libavcodec.a
            ${ffmpeg_lib_path}/libavdevice.a
            ${ffmpeg_lib_path}/libavfilter.a
            ${ffmpeg_lib_path}/libavutil.a
            ${ffmpeg_lib_path}/libswscale.a
            ${ffmpeg_lib_path}/libswresample.a
            ${ffmpeg_lib_path}/liblzma.a
            m
            /usr/lib/x86_64-linux-gnu/libz.so.1
            SDL2-2.0
            gtest
            gtest_main
            pthread
            ${MOCKCPP_DIR}/lib/libmockcpp.a
            curl
            )
endif ()
add_test(Test histreamer_ut)
enable_testing()
