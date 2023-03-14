SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)

############################################################
# Create a library
############################################################

#Generate the shared library from the library sources
ADD_LIBRARY(ndk_examples_inst STATIC
	Hello.cpp
	)

TARGET_INCLUDE_DIRECTORIES(ndk_examples_inst
	PUBLIC 
	${PROJECT_SOURCE_DIR}/include
	)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
ADD_EXECUTABLE(ndk_examples_inst_bin
	main.cpp
	)

# link the new hello_library target with the hello_binary target
TARGET_LINK_LIBRARIES(ndk_examples_inst_bin
	PRIVATE 
	ndk_examples_inst
	)

############################################################
# Install
############################################################

# Binaries
INSTALL(TARGETS ndk_examples_inst_bin
	DESTINATION bin)

# Library
# Note: may not work on windows
INSTALL(TARGETS ndk_examples_inst
	LIBRARY DESTINATION lib)

# Header files
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ 
	DESTINATION include)

# Config
INSTALL(FILES ndk-examples.conf
	DESTINATION etc)    
