cmake_minimum_required(VERSION 3.28)
project(TrustedInstallerElevation)

set(CMAKE_CXX_STANDARD 26)

add_executable(TrustedInstallerElevation WIN32 main.cpp)

set_property(TARGET TrustedInstallerElevation PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
set_target_properties(TrustedInstallerElevation PROPERTIES LINK_FLAGS "/MANIFEST /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")
