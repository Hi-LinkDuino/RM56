SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
############################################################
# Create a library
############################################################

#Generate the static library from the library sources
ADD_LIBRARY(hello_static_library STATIC 
	Hello.cpp
	)

TARGET_INCLUDE_DIRECTORIES(hello_static_library
	PUBLIC 
	${PROJECT_SOURCE_DIR}/include
	)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
ADD_EXECUTABLE(hello_static_binary 
	main.cpp
	)

# link the new hello_library target with the hello_binary target
TARGET_LINK_LIBRARIES( hello_static_binary
	PRIVATE 
	hello_static_library
	)	
