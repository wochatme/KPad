project(libskia)

file(GLOB LIBSKIA_SRC 
	ext/SkMemory_new_handler.cpp
	ext/bitmap_platform_device_win.cpp
	ext/convolver.cpp
	ext/google_logging.cpp
	ext/image_operations.cpp
	ext/platform_canvas.cpp
	ext/platform_canvas_win.cpp
	ext/platform_device_win.cpp
	ext/skia_utils_win.cpp
	ext/vector_canvas.cpp
	ext/vector_platform_device_emf_win.cpp
	)

add_library(${PROJECT_NAME} ${LIBSKIA_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR} 
           ${CMAKE_SOURCE_DIR}
           )
           
