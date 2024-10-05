project(libase)

file(GLOB LIBASE_SRC 
	at_exit.cpp
	base_paths.cpp
	base_switches.cpp
	callback_internal.cpp
	command_line.cpp
	cpu.cpp
	environment.cpp
	file_path.cpp
	file_util.cpp
	file_util_win.cpp
	file_version_info.cpp
	icu_utf.cpp
	lazy_instance.cpp
	logging.cpp
	message_loop.cpp
	message_loop_proxy.cpp
	message_loop_proxy_impl.cpp
	message_pump.cpp
	message_pump_default.cpp
	message_pump_win.cpp
	native_library.cpp
	path_service.cpp
	pickle.cpp
	platform_file.cpp
	base_process.cpp
	process_util.cpp
	rand_util.cpp
	scoped_temp_dir.cpp
	shared_memory.cpp
	stringprintf.cpp
	string_number_conversions.cpp
	string_piece.cpp
	string_split.cpp
	string_util.cpp
	sys_info.cpp
	sys_string_conversions.cpp
	task.cpp
	task_queue.cpp
	base_time.cpp
	timer.cpp
	utf_string_conversions.cpp
	utf_string_conversion_utils.cpp
	value.cpp
	value_conversions.cpp
	version.cpp
	vlog.cpp
	debug/debugger.cpp
	debug/stack_trace.cpp
	i18n/case_conversion.cpp
	i18n/rtl.cpp
	memory/ref_counted.cpp
	memory/ref_counted_memory.cpp
	memory/weak_ptr.cpp
	metric/field_trial.cpp
	metric/histogram.cpp
	metric/stats_counters.cpp
	synchronization/condition_variable.cpp
	synchronization/lock.cpp
	synchronization/lock_impl.cpp
	synchronization/waitable_event.cpp
	synchronization/waitable_event_watcher.cpp
	threading/non_thread_safe_impl.cpp
	threading/platform_thread.cpp
	threading/thread.cpp
	threading/thread_checker_impl.cpp
	threading/thread_local.cpp
	threading/thread_local_storage.cpp
	threading/thread_restrictions.cpp
	system_monitor/system_monitor.cpp
	win/metro.cpp
	win/object_watcher.cpp
	win/registry.cpp
	win/resource_util.cpp
	win/scoped_bstr.cpp
	win/scoped_variant.cpp
	win/shortcut.cpp
	win/windows_version.cpp
	win/win_util.cpp
	win/wrapped_window_proc.cpp
	)

source_group("Source Files\\debug" FILES debug/debugger.cpp debug/stack_trace.cpp)

add_library(${PROJECT_NAME} ${LIBASE_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_SOURCE_DIR} 
           ${CMAKE_SOURCE_DIR}
           )
           
