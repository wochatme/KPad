project(libuibase)

add_definitions(-D_UNICODE -DUNICODE -DNOMINMAX)

file(GLOB LIBUIBASE_SRC 
	theme_provider.cpp
	view_prop.cpp
	)

# source_group("Source Files\\debug" FILES debug/debugger.cpp debug/stack_trace.cpp)

add_library(${PROJECT_NAME} ${LIBUIBASE_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR} 
           ${CMAKE_SOURCE_DIR}
           )
           
