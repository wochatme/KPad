cmake_minimum_required(VERSION 3.20)

project(KPad)

set(CMAKE_CXX_STANDARD 20)
add_subdirectory(win)
add_subdirectory(base)
add_subdirectory(skia)
add_subdirectory(third_party/skia)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT kpad-win64)

