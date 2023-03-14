set(LIB_SRC hello.c)
add_library(libhello STATIC ${LIB_SRC})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
set_target_properties(libhello PROPERTIES OUTPUT_NAME "hello")
