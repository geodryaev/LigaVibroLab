cmake_minimum_required(VERSION 3.21)
project(LigaVibroLab VERSION 0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Svg REQUIRED)

add_subdirectory(QXlsx)

# РУЧНЫЕ ПУТИ (оставь только если не можешь использовать find_package)
set(QWT_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/include")
set(QWT_LIBRARY_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/lib")
set(QWT_LIBRARY_DIR_D "${VCPKG_INSTALLED_DIR}/x64-windows/debug/lib")
# Удобнее генераторные выражения вместо debug/optimized:
set(QWT_LIB $<$<CONFIG:Debug>:${QWT_LIBRARY_DIR_D}/qwtd.lib>$<$<CONFIG:Release>:${QWT_LIBRARY_DIR}/qwt.lib>)

add_executable(VibroProcessing WIN32
  main.cpp
  mainwindow.cpp
  vibrodata.cpp
  mainwindow.h
  vibrodata.h
  mainwindow.ui
  report.h
  report.cpp
  stepvibro.h stepvibro.cpp
  calcamplitud.h
  calcamplitud.cpp
  calcamplitud.ui
  correctinput.h correctinput.cpp correctinput.ui
  supportmodul.h supportmodul.cpp supportmodul.ui
  resources.qrc
)

target_include_directories(VibroProcessing PRIVATE ${QWT_INCLUDE_DIR})

if(MSVC)
  target_compile_options(VibroProcessing PRIVATE $<$<CONFIG:Debug>:/Zi>)
  target_link_options(VibroProcessing PRIVATE $<$<CONFIG:Debug>:/DEBUG>)
endif()

target_link_libraries(VibroProcessing PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::Network
  Qt6::Svg
  ${QWT_LIB}
  QXlsx
)
