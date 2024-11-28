cmake_minimum_required(VERSION 3.29)
project(TrustedInstallerElevation)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(TrustedInstallerElevation main.cpp)
set_target_properties(TrustedInstallerElevation PROPERTIES LINK_FLAGS "/MANIFEST /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")