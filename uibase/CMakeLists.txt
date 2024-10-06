project(libuibase)

add_definitions(-D_UNICODE -DUNICODE -DNOMINMAX)

file(GLOB LIBUIBASE_SRC 
	view_prop.cpp
	theme_provider.cpp
	view_prop.cpp
	accessibility/accessible_view_state.cpp
	animation/animation.cpp
	animation/animation_container.cpp
	animation/linear_animation.cpp
	animation/multi_animation.cpp
	animation/slide_animation.cpp
	animation/throb_animation.cpp
	animation/tween.cpp
	clipboard/clipboard.cpp
	clipboard/clipboard_util_win.cpp
	clipboard/scoped_clipboard_writer.cpp
	compositor/compositor.cpp
	compositor/compositor_stub.cpp
	compositor/layer.cpp
	compositor/layer_animator.cpp
	dragdrop/drag_drop_types.cpp
	dragdrop/drag_source.cpp
	dragdrop/drop_target.cpp
	dragdrop/os_exchange_data.cpp
	dragdrop/os_exchange_data_provider_win.cpp
	gfx/interpolated_transform.cpp
	keycodes/keyboard_code_conversion_win.cpp
	l10n/l10n_font_util.cpp
	l10n/l10n_util.cpp
	l10n/l10n_util_win.cpp
	models/button_menu_item_model.cpp
	models/menu_model.cpp
	models/simple_menu_model.cpp
	models/table_model.cpp
	models/tree_model.cpp
	range/range.cpp
	resource/data_pack.cpp
	resource/resource_bundle.cpp
	shell_dialogs/base_shell_dialog.cpp
	shell_dialogs/base_shell_dialog_win.cpp
	shell_dialogs/selected_file_info.cpp
	shell_dialogs/select_file_dialog.cpp
	shell_dialogs/select_file_dialog_factory.cpp
	shell_dialogs/select_file_dialog_win.cpp
	shell_dialogs/select_file_policy.cpp
	text/text_elider.cpp
	win/hwnd_util.cpp
	win/mouse_wheel_util.cpp
	win/screen.cpp
	win/shell.cpp
	win/window_impl.cpp
	)

source_group("Source Files\\accessibility" REGULAR_EXPRESSION "accessibility/.*\\.cpp")
source_group("Source Files\\animation" REGULAR_EXPRESSION "animation/.*\\.cpp")
source_group("Source Files\\clipboard" REGULAR_EXPRESSION "clipboard/.*\\.cpp")
source_group("Source Files\\compositor" REGULAR_EXPRESSION "compositor/.*\\.cpp")
source_group("Source Files\\dragdrop" REGULAR_EXPRESSION "dragdrop/.*\\.cpp")
source_group("Source Files\\gfx" REGULAR_EXPRESSION "gfx/.*\\.cpp")
source_group("Source Files\\keycodes" REGULAR_EXPRESSION "keycodes/.*\\.cpp")
source_group("Source Files\\l10n" REGULAR_EXPRESSION "l10n/.*\\.cpp")
source_group("Source Files\\models" REGULAR_EXPRESSION "models/.*\\.cpp")
source_group("Source Files\\range" REGULAR_EXPRESSION "range/.*\\.cpp")
source_group("Source Files\\resource" REGULAR_EXPRESSION "resource/.*\\.cpp")
source_group("Source Files\\shell_dialogs" REGULAR_EXPRESSION "shell_dialogs/.*\\.cpp")
source_group("Source Files\\text" REGULAR_EXPRESSION "text/.*\\.cpp")
source_group("Source Files\\win" REGULAR_EXPRESSION "win/.*\\.cpp")

add_library(${PROJECT_NAME} ${LIBUIBASE_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR} 
           ${CMAKE_SOURCE_DIR}
           )
           
