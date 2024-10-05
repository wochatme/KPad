project(libuiview)

file(GLOB LIBUIVIEW_SRC 
	accelerator.cpp
	background.cpp
	border.cpp
	drag_utils.cpp
	layer_helper.cpp
	layer_property_setter.cpp
	metrics.cpp
	native_theme_painter.cpp
	paint_lock.cpp
	painter.cpp
	repeat_controller.cpp
	view.cpp
	view_constants.cpp
	)

# source_group("Source Files\\debug" FILES debug/debugger.cpp debug/stack_trace.cpp)

add_library(${PROJECT_NAME} ${LIBUIVIEW_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR} 
           ${CMAKE_SOURCE_DIR}
           )
           
