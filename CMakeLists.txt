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
        src/ui/MainWindow.cpp
        src/ui/ImageCanvas.cpp
        src/render/Renderer.cpp
        src/render/Camera.cpp
        src/Application.cpp
        src/scene/Scene.cpp
        src/math/Geometry.cpp
        src/wallnut/Random.cpp
        src/wallnut/Random.h
        src/math/Hittable.cpp
        src/math/Hittable.h
        src/math/Interval.cpp
        src/math/Interval.h
        src/render/Material.cpp
        src/render/Material.h
        src/utils/Timer.cpp
        src/utils/Timer.h
        src/Input.h
        src/Input.cpp
        src/render/ImagePostProcessors.cpp
        src/render/ImagePostProcessors.h
        src/math/ColorUtils.h
        src/math/Random.h
        src/math/Random.cpp
        src/ui/RenderSettingsWidget.cpp
        src/ui/RenderSettingsWidget.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

set(TBB_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/oneTBB)
add_subdirectory(deps/glm)
add_subdirectory(deps/libpng)

target_link_libraries(${PROJECT_NAME}
        PRIVATE png_framework
        PRIVATE Qt6::Widgets
        PRIVATE glm::glm
        PRIVATE TBB::tbb
)
