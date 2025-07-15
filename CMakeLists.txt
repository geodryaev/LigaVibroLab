cmake_minimum_required(VERSION 3.5)
project(VibroProcessing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QWT_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/include")
set(QWT_LIBRARY_DIR "${VCPKG_INSTALLED_DIR}/x64-windows/lib")
set(QWT_LIBRARY_DIR_D "${VCPKG_INSTALLED_DIR}/x64-windows/debug/lib")
set(QWT_LIB_DEBUG "${QWT_LIBRARY_DIR_D}/qwtd.lib")
set(QWT_LIB_RELEASE "${QWT_LIBRARY_DIR}/qwt.lib")

# Убедись, что путь к Qt 6 правильно указан через CMAKE_PREFIX_PATH при вызове cmake
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

add_subdirectory(QXlsx)



add_executable(VibroProcessing
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
)

target_compile_definitions(VibroProcessing PRIVATE
    QT_DEPRECATED_WARNINGS
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

target_include_directories(VibroProcessing PRIVATE QXlsx)
target_include_directories(VibroProcessing PRIVATE ${QWT_INCLUDE_DIR})

target_link_directories(VibroProcessing PRIVATE ${QWT_LIBRARY_DIR})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")


target_link_libraries(VibroProcessing PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    debug ${QWT_LIB_DEBUG}
    optimized ${QWT_LIB_RELEASE}
    QXlsx
)


