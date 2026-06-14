cmake_minimum_required(VERSION 3.20)

project(MetalSlothEngine VERSION 1.0 LANGUAGES C CXX OBJC OBJCXX)

# The engine uses std::unordered_map::contains (C++20), plus std::optional /
# std::filesystem.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Emit compile_commands.json so clangd (nvim LSP) resolves metal-cpp headers etc.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Homebrew-installed GLFW (ships glfw3Config.cmake). Covers both the
# Apple-silicon (/opt/homebrew) and Intel (/usr/local) Homebrew prefixes.
list(APPEND CMAKE_PREFIX_PATH /opt/homebrew /usr/local)
find_package(glfw3 REQUIRED)

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.mm"
    "${CMAKE_SOURCE_DIR}/src/component_types/*.cpp")

set(EXTERNAL_SOURCES
    external/AAPLMathUtilities.cpp
    external/stb/stb_image.cpp
    external/ufbx/ufbx.c
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_impl_glfw.cpp
    external/imgui/imgui_impl_metal.mm)

add_executable(MetalSlothEngine ${ENGINE_SOURCES} ${EXTERNAL_SOURCES})

target_include_directories(MetalSlothEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/component_types
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/metal-cpp)

# The Objective-C++ bridges (imgui_impl_metal, glfw_bridge) are written against
# ARC (they use __bridge casts). The metal-cpp .cpp files manage memory
# manually, but they aren't ObjC so ARC doesn't apply to them.
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/src/glfw_bridge.mm
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_impl_metal.mm
    PROPERTIES COMPILE_OPTIONS "-fobjc-arc")

target_link_libraries(MetalSlothEngine PRIVATE glfw)

target_link_libraries(MetalSlothEngine PRIVATE
    "-framework Foundation"
    "-framework Cocoa"
    "-framework Metal"
    "-framework MetalKit"
    "-framework QuartzCore"
    "-framework IOKit"
    "-framework CoreVideo")

# ---------------------------------------------------------------------------
# Metal shaders -> default.metallib (loaded at runtime via newDefaultLibrary())
# ---------------------------------------------------------------------------
# Requires the Metal Toolchain:  xcodebuild -downloadComponent MetalToolchain
file(GLOB SHADER_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/shaders/*.metal")
set(METALLIB "${CMAKE_BINARY_DIR}/default.metallib")

add_custom_command(
    OUTPUT ${METALLIB}
    COMMAND xcrun -sdk macosx metal ${SHADER_SOURCES} -o ${METALLIB}
    DEPENDS ${SHADER_SOURCES}
    COMMENT "Compiling Metal shaders -> default.metallib"
    VERBATIM)
add_custom_target(shaders DEPENDS ${METALLIB})
add_dependencies(MetalSlothEngine shaders)

# newDefaultLibrary() looks for default.metallib next to the executable
# (the CLI "main bundle" resource path), so make sure it lands there.
add_custom_command(TARGET MetalSlothEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${METALLIB} $<TARGET_FILE_DIR:MetalSlothEngine>
    COMMENT "Placing default.metallib next to the executable"
    VERBATIM)

# Assets are loaded with paths relative to the current working directory
# (e.g. "assets/X Bot.fbx"), so run the binary from the repo root:
#     cmake -S . -B build -G Ninja && cmake --build build
#     ./build/MetalSlothEngine
