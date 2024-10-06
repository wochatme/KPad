project(libuigfx)

add_definitions(-D_UNICODE -DUNICODE -DNOMINMAX)

file(GLOB LIBUIGFX_SRC 
	blit.cpp
	canvas.cpp
	canvas_skia.cpp
	canvas_skia_win.cpp
	color_analysis.cpp
	color_utils.cpp
	font.cpp
	gdi_util.cpp
	icon_util.cpp
	insets.cpp
	native_theme.cpp
	native_theme_win.cpp
	path.cpp
	path_win.cpp
	platform_font_win.cpp
	point.cpp
	rect.cpp
	scrollbar_size.cpp
	size.cpp
	skbitmap_operations.cpp
	skia_util.cpp
	transform.cpp
	)

# source_group("Source Files\\debug" FILES debug/debugger.cpp debug/stack_trace.cpp)

add_library(${PROJECT_NAME} ${LIBUIGFX_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR} 
           ${CMAKE_SOURCE_DIR}
           ${CMAKE_SOURCE_DIR}/third_party/skia/include/config
           ${CMAKE_SOURCE_DIR}/third_party/skia/include/core
           ${CMAKE_SOURCE_DIR}/third_party/skia/include/effects
           ${CMAKE_SOURCE_DIR}/third_party/skia/include/ports
           ${CMAKE_SOURCE_DIR}/third_party/skia/include/utils
           )
           
