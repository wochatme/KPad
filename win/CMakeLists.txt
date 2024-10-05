# the build for Windows X64
project(kpad-win64 CXX)

add_definitions(-D_UNICODE -DUNICODE -DNOMINMAX)

add_executable(${PROJECT_NAME} WIN32 
	App.cpp
	pch.cpp
	App.rc
	chrome/accessible_pane_view.cpp
	chrome/base_tab.cpp
	chrome/base_tab_strip.cpp
	chrome/bookmark_bar_view.cpp
	chrome/bookmark_node_data.cpp
	chrome/bookmark_tab_helper_delegate.cpp
	chrome/browser.cpp
	chrome/browser_bubble.cpp
	chrome/browser_bubble_host.cpp
	chrome/browser_frame.cpp
	chrome/browser_frame_views.cpp
	chrome/browser_frame_win.cpp
	chrome/browser_list.cpp
	chrome/browser_navigator.cpp
	chrome/browser_non_client_frame_view_factory_win.cpp
	chrome/browser_process.cpp
	chrome/browser_process_impl.cpp
	chrome/browser_root_view.cpp
	chrome/browser_shutdown.cpp
	chrome/browser_tab_strip_controller.cpp
	chrome/browser_view.cpp
	chrome/browser_view_layout.cpp
	chrome/chrome_main.cpp
	chrome/chrome_view_delegate.cpp
	chrome/cmd_scatter_button.cpp
	chrome/contents_container.cpp
	chrome/default_tab_handler.cpp
	chrome/defaults.cpp
	chrome/detachable_toolbar_view.cpp
	chrome/dock_info.cpp
	chrome/dragged_tab_controller.cpp
	chrome/dragged_tab_view.cpp
	chrome/event_utils.cpp
	chrome/glass_browser_frame_view.cpp
	chrome/infobar.cpp
	chrome/infobar_container.cpp
	chrome/infobar_container_view.cpp
	chrome/infobar_delegate.cpp
	chrome/location_bar_view.cpp
	chrome/location_icon_view.cpp
	chrome/native_tab_contents_view_win.cpp
	chrome/native_view_photobooth_win.cpp
	chrome/opaque_browser_frame_view.cpp
	chrome/page_navigator.cpp
	chrome/popup_non_client_frame_view.cpp
	chrome/profile.cpp
	chrome/profile_manager.cpp
	chrome/session_id.cpp
	chrome/tab.cpp
	chrome/tab_contents.cpp
	chrome/tab_contents_container_views.cpp
	chrome/tab_contents_delegate.cpp
	chrome/tab_contents_view.cpp
	chrome/tab_contents_view_views.cpp
	chrome/tab_contents_wrapper.cpp
	chrome/tab_contents_wrapper_delegate.cpp
	chrome/tab_icon_view.cpp
	chrome/tab_menu_model.cpp
	chrome/tab_renderer_data.cpp
	chrome/tab_strip.cpp
	chrome/tab_strip_factory.cpp
	chrome/tab_strip_model.cpp
	chrome/tab_strip_model_observer.cpp
	chrome/tab_strip_model_order_controller.cpp
	chrome/tab_strip_selection_model.cpp
	chrome/toolbar_model.cpp
	chrome/toolbar_view.cpp
	chrome/url.cpp
	)

source_group("Source Files\\chrome" REGULAR_EXPRESSION "chrome/.*\\.cpp")

# Specify the precompiled header(s)
set(PRECOMPILE_HEADER_FILES pch.h)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILE_HEADER_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} PRIVATE 
	${PROJECT_SOURCE_DIR} 
	${PROJECT_SOURCE_DIR}/wtl 
	${CMAKE_SOURCE_DIR}
	)

target_link_libraries(${PROJECT_NAME} libase)
target_link_libraries(${PROJECT_NAME} libskia)

