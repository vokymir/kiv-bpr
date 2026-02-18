cmake_minimum_required(VERSION 3.25)

# Simulation of Self-Organized Criticality
project(SSOC
    VERSION 0.0.1
    LANGUAGES C CXX # C is for igraph library
)

# =====
# Global Configuration
# =====

# compile_commands.json (for Neovim/LSP)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =====
# Dependencies
# =====
# igraph and imgui, but imgui also needs backend (sdl+opengl)
# specification stored in vcpkg.json

find_package(igraph CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# --- Dear ImGui ---

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")

# Check if ImGui exists to avoid obscure errors
if(NOT EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(FATAL_ERROR
        "ImGui not found at ${IMGUI_DIR}. Did you forget git submodules?"
    )
endif()

add_library(imgui STATIC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(imgui SYSTEM PUBLIC
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
    OpenGL::GL
)

# C++ standard for ImGui
target_compile_features(imgui PUBLIC cxx_std_17)

# =====
# Main Target (SSOC)
# =====

file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.hpp"
)

# add_executable(sim_soc ${SOURCES})
if (WIN32)
    # make it GUI app (without console) on windows
    add_executable(sim_soc WIN32 ${SOURCES})
    set_target_properties(sim_soc PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
else()
    add_executable(sim_soc ${SOURCES})
endif()

set_target_properties(sim_soc PROPERTIES
    OUTPUT_NAME "ssoc"
)

target_include_directories(sim_soc PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

target_link_libraries(sim_soc PRIVATE
    imgui
    igraph::igraph
)

# =====
# Compiler Settings
# =====

set_target_properties(sim_soc PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# =====
# Output Directory
# =====

# Moving the binary to ./bin in the source root.
set_target_properties(sim_soc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

# =====
# Strict debug + no optimalizations (down)
# =====

if(MSVC)
    target_compile_options(sim_soc PRIVATE
        $<$<CONFIG:Debug>:
            /W4                 # Baseline strict
            /w14242 /w14265     # Specific useful high-level warnings
            /w14287 /w14296
            /w14311 /w14826
            /permissive-        # Standards conformance
            /WX                 # Warnings as errors (Optional: remove if too annoying)
        >
    )
else()
    target_compile_options(sim_soc PRIVATE
        $<$<CONFIG:Debug>:
            -Wall -Wextra       # Reasonable strictness
            -Wpedantic          # ISO C++ conformance
            -Wshadow            # Local variable shadows another
            -Wnon-virtual-dtor  # Missing virtual destructor
            -Wold-style-cast    # C-style casts in C++
            -Wcast-align        # Potential alignment issues
            -Wunused            # Unused variables/functions
            -Woverloaded-virtual # Hiding virtual functions
            -Wconversion        # Implicit type conversions that lose data
            -Wsign-conversion   # Signed/unsigned comparison/conversion
            -Wnull-dereference  # Detect potential null pointer usage
            -Wdouble-promotion  # Float to double promotion
            -Wformat=2          # Strict printf-style check
            # -Werror             # Warnings as errors
        >
    )
endif()

# Sanitizers and Debug levels (Linux/macOS)
if(NOT MSVC)
    target_compile_options(sim_soc PRIVATE
        $<$<CONFIG:Debug>:
            -g3 -O0
            -fsanitize=address,undefined,leak
            -fno-omit-frame-pointer
            -fstack-protector-all
        >
    )
    target_link_options(sim_soc PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address,undefined,leak>
    )
endif()

# =====
# Dev Tools (Neovim support)
# =====

# Symlink compile_commands.json to root for LSP support
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_command(TARGET sim_soc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        "${CMAKE_BINARY_DIR}/compile_commands.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json"
        COMMENT "Symlinking compile_commands.json to source root"
        VERBATIM
    )
endif()
