set(system_service_headfiles
	${CMAKE_CURRENT_SOURCE_DIR}/include/parameter_ex.h
)

set(system_service_sources ${CMAKE_CURRENT_SOURCE_DIR}/parameter_ex.cpp)
if(WIN32)
	list(APPEND system_service_sources ${CMAKE_CURRENT_SOURCE_DIR}/platform/windows/parameter_ex.cpp)
elseif(UNIX)
	list(APPEND system_service_sources ${CMAKE_CURRENT_SOURCE_DIR}/platform/linux/parameter_ex.cpp)
endif()

target_sources(${PROJECT_NAME}
	PRIVATE
	${system_service_sources}
	${system_service_headfiles})
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)