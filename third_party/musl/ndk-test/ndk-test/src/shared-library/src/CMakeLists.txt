SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)

############################################################
# Create a library
############################################################

#Generate the shared library from the library sources
ADD_LIBRARY(hello_shared_library SHARED 
	Hello.cpp
	)
ADD_LIBRARY(hello::library ALIAS hello_shared_library)

TARGET_INCLUDE_DIRECTORIES(hello_shared_library
	PUBLIC 
	${PROJECT_SOURCE_DIR}/include
	)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
ADD_EXECUTABLE(hello_shared_binary
	main.cpp
	)

# link the new hello_library target with the hello_binary target
TARGET_LINK_LIBRARIES( hello_shared_binary
	PRIVATE 
	hello::library
	)	
