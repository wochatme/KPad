project(libuiview)

add_definitions(-D_UNICODE -DUNICODE -DNOMINMAX)

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
	accessibility/native_view_accessibility_win.cpp
	activex/ax_host.cpp
	animation/bounds_animator.cpp
	controls/activex/flash_view.cpp
	controls/activex/html_view.cpp
	controls/activex/mediaplay_view.cpp
	controls/activex/silverlight_view.cpp
	controls/activex/webbrowser_view.cpp
	controls/button/button.cpp
	controls/button/button_dropdown.cpp
	controls/button/checkbox.cpp
	controls/button/custom_button.cpp
	controls/button/image_button.cpp
	controls/button/menu_button.cpp
	controls/button/native_button.cpp.del
	controls/button/native_button_win.cpp.del
	controls/button/press_button.cpp
	controls/button/radio_button.cpp
	controls/button/text_button.cpp
	controls/combobox/combobox.cpp
	controls/combobox/native_combobox_view.cpp
	controls/combobox/native_combobox_win.cpp
	controls/focusable_border.cpp
	controls/image_view.cpp
	controls/label.cpp
	controls/link.cpp
	controls/listbox/listbox.cpp
	controls/listbox/native_listbox_win.cpp
	controls/menu/menu.cpp
	controls/menu/menu_2.cpp
	controls/menu/menu_config.cpp
	controls/menu/menu_controller.cpp
	controls/menu/menu_delegate.cpp
	controls/menu/menu_host.cpp
	controls/menu/menu_host_root_view.cpp
	controls/menu/menu_item_view.cpp
	controls/menu/menu_model_adapter.cpp
	controls/menu/menu_runner.cpp
	controls/menu/menu_scroll_view_container.cpp
	controls/menu/menu_separator.cpp
	controls/menu/menu_win.cpp
	controls/menu/native_menu_win.cpp
	controls/menu/submenu_view.cpp
	controls/native/native_view_host.cpp
	controls/native/native_view_host_view.cpp
	controls/native/native_view_host_win.cpp
	controls/native_control.cpp
	controls/native_control_win.cpp
	controls/progress_bar.cpp
	controls/richedit/rich_view.cpp
	controls/scrollbar/bitmap_scroll_bar.cpp
	controls/scrollbar/native_scroll_bar.cpp
	controls/scrollbar/native_scroll_bar_win.cpp
	controls/scrollbar/scroll_bar.cpp
	controls/scroll_view.cpp
	controls/separator.cpp
	controls/single_split_view.cpp
	controls/tabbed_pane/native_tabbed_pane_win.cpp
	controls/tabbed_pane/tabbed_pane.cpp
	controls/table/group_table_view.cpp
	controls/table/native_table_win.cpp
	controls/table/table_view.cpp
	controls/table/table_view2.cpp
	controls/textfield/native_textfield_win.cpp
	controls/textfield/textfield.cpp
	controls/throbber.cpp
	controls/tree/tree_view.cpp
	event/event.cpp
	event/event_utils_win.cpp
	focus/accelerator_handler.cpp
	focus/focus_manager.cpp
	focus/focus_manager_factory.cpp
	focus/focus_search.cpp
	focus/view_storage.cpp
	focus/widget_focus_manager.cpp
	ime/input_method_win.cpp
	layout/box_layout.cpp
	layout/fill_layout.cpp
	layout/grid_layout.cpp
	layout/layout_manager.cpp
	widget/aero_tooltip_manager.cpp
	widget/child_window_message_processor.cpp
	widget/default_theme_provider.cpp
	widget/drop_helper.cpp
	widget/drop_target_win.cpp
	widget/monitor_win.cpp
	widget/native_widget_view.cpp
	widget/native_widget_views.cpp
	widget/native_widget_win.cpp
	widget/root_view.cpp
	widget/tooltip_manager.cpp
	widget/tooltip_manager_win.cpp
	widget/widget.cpp
	widget/widget_delegate.cpp
	widget/window_manager.cpp
	window/client_view.cpp
	window/custom_frame_view.cpp
	window/dialog_client_view.cpp
	window/dialog_delegate.cpp
	window/native_frame_view.cpp
	window/non_client_view.cpp
	window/window_shape.cpp
	)

source_group("Source Files\\accessibility" REGULAR_EXPRESSION "accessibility/.*\\.cpp")
source_group("Source Files\\activex" REGULAR_EXPRESSION "activex/.*\\.cpp")
source_group("Source Files\\animation" REGULAR_EXPRESSION "animation/.*\\.cpp")
source_group("Source Files\\event" REGULAR_EXPRESSION "event/.*\\.cpp")
source_group("Source Files\\focus" REGULAR_EXPRESSION "focus/.*\\.cpp")
source_group("Source Files\\ime" REGULAR_EXPRESSION "ime/.*\\.cpp")
source_group("Source Files\\layout" REGULAR_EXPRESSION "layout/.*\\.cpp")
source_group("Source Files\\widget" REGULAR_EXPRESSION "widget/.*\\.cpp")
source_group("Source Files\\window" REGULAR_EXPRESSION "window/.*\\.cpp")
source_group("Source Files\\controls" REGULAR_EXPRESSION "accessibility/.*\\.cpp")
source_group("Source Files\\controls\\activex" REGULAR_EXPRESSION "controls/activex/.*\\.cpp")
source_group("Source Files\\controls\\button" REGULAR_EXPRESSION "controls/button/.*\\.cpp")
source_group("Source Files\\controls\\combobox" REGULAR_EXPRESSION "controls/combobox/.*\\.cpp")
source_group("Source Files\\controls\\listbox" REGULAR_EXPRESSION "controls/listbox/.*\\.cpp")
source_group("Source Files\\controls\\menu" REGULAR_EXPRESSION "controls/menu/.*\\.cpp")
source_group("Source Files\\controls\\native" REGULAR_EXPRESSION "controls/native/.*\\.cpp")
source_group("Source Files\\controls\\richedit" REGULAR_EXPRESSION "controls/richedit/.*\\.cpp")
source_group("Source Files\\controls\\scrollbar" REGULAR_EXPRESSION "controls/scrollbar/.*\\.cpp")
source_group("Source Files\\controls\\tabbed_pane" REGULAR_EXPRESSION "controls/tabbed_pane/.*\\.cpp")
source_group("Source Files\\controls\\table" REGULAR_EXPRESSION "controls/table/.*\\.cpp")
source_group("Source Files\\controls\\textfield" REGULAR_EXPRESSION "controls/textfield/.*\\.cpp")
source_group("Source Files\\controls\\tree" REGULAR_EXPRESSION "controls/tree/.*\\.cpp")

add_library(${PROJECT_NAME} ${LIBUIVIEW_SRC})

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
           
