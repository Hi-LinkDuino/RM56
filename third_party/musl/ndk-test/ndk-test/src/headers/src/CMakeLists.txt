# Create a sources variable with a link to all cpp files to compile
SET(SOURCE 
	Hello.cpp
	main.cpp
	)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)

# Add an executable with the above sources
ADD_EXECUTABLE(hello_headers ${SOURCE})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
TARGET_INCLUDE_DIRECTORIES(hello_headers
	PRIVATE
	${PROJECT_SOURCE_DIR}/include
	)
