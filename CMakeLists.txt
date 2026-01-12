cmake_minimum_required(VERSION 3.29.2)
project(Soundboard)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/imgui")

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
        ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
        src/GUI.cpp
        include/GUI.h
        src/audio.cpp
        include/audio.h
)


add_executable(Soundboard
        src/main.cpp
        src/validateFiles.cpp
        src/utils.cpp
        src/sound.cpp
        src/globals.cpp
        src/miniaudio.c
        src/GUI.cpp
        src/audio.cpp
        ${IMGUI_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d11
        dxgi
        d3dcompiler
        dwmapi
)

# Comment this out if i need the console to show
# set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)
