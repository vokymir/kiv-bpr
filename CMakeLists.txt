cmake_minimum_required(VERSION 3.16)

# Simulation of Self-Organized Criticality
project(ssoc.exe LANGUAGES C CXX)

# =====
# general settings
# =====

# C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# C (imgui, igraph)
set(CMAKE_C_STANDARD 11)

# bonus: might help on multi-platform shared/static libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# using with neovim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =====
# Dear ImGui
# =====

set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/external/imgui)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    # ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    # ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
   ${IMGUI_DIR}
   ${IMGUI_DIR}/backends
)

# =====
# igraph
# =====

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(IGRAPH_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

add_subdirectory(external/igraph)

# =====
# Simulation of Self-Organized Criticallity (SSOC)
# =====

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    igraph
)


# =====
# ./bin
# =====

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# =====
# compile_commands.jsoon
# =====

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(link_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${PROJECT_SOURCE_DIR}/compile_commands.json
    )
endif()









