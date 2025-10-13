cmake_minimum_required(VERSION 3.31)
project(Dazhbog)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_PREFIX_PATH "~/Qt/6.9.2/macos")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set("ROOT_FOLDER" ${PROJECT_SOURCE_DIR})

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/MainWindow.cpp
        src/ImageCanvas.cpp
        src/render/Renderer.cpp
        src/render/Camera.cpp
        src/Application.cpp
        src/scene/Scene.cpp
        src/render/Material.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(deps/glm)
add_subdirectory(deps/stb_image_wrapper)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt6::Widgets
        PRIVATE glm::glm
)
