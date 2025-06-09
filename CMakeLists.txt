cmake_minimum_required(VERSION 3.5)
project(VibroProcessing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Убедись, что путь к Qt 6 правильно указан через CMAKE_PREFIX_PATH при вызове cmake
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)

add_executable(VibroProcessing
    main.cpp
    mainwindow.cpp
    vibrodata.cpp

    mainwindow.h
    vibrodata.h

    mainwindow.ui
)

target_compile_definitions(VibroProcessing PRIVATE
    QT_DEPRECATED_WARNINGS
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")


target_link_libraries(VibroProcessing PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
