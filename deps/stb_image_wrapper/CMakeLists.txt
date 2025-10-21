cmake_minimum_required(VERSION 3.31)
project(stb_image)

set(CMAKE_CXX_STANDARD 20)

add_library(stb_image STATIC stb_image.cpp
        ../../src/main.cpp
        ../../src/MainWindow.cpp
        ../../src/MainWindow.h
        ../../src/ImageCanvas.cpp
        ../../src/ImageCanvas.h
        ../../src/ImageCanvas.cpp
        ../../src/ImageCanvas.h
        ../../src/Application.cpp
        ../../src/Application.h
        ../../src/Input.cpp
        ../../src/Input.h)

target_include_directories(stb_image PUBLIC ${ROOT_FOLDER}/deps/stb_image)

add_library(stb::image ALIAS stb_image)