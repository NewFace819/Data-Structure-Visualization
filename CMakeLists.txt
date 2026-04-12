cmake_minimum_required(VERSION 3.14)
project(DataStructureVisualizer)

# 1. SFML FetchContent - This downloads and builds SFML automatically
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

# Optimization: Don't build SFML examples or docs
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

# 2. Your Project Settings
include_directories(include)

# Collect all your source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(MySFMLApp main.cpp ${SOURCES})

# 3. Link against the SFML targets (Note: names are sfml-graphics, etc.)
target_link_libraries(MySFMLApp 
    sfml-graphics 
    sfml-window 
    sfml-system 
    sfml-audio 
    sfml-network
)

# 4. Copy Assets (Important for your images/fonts)
add_custom_command(TARGET MySFMLApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:MySFMLApp>/assets")