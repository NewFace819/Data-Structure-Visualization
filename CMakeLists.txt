cmake_minimum_required(VERSION 3.14)
project(DataStructureVisualizer)

include(FetchContent)

include_directories(include)

set(USE_LOCAL_SFML_PACKAGE FALSE)
if(MSVC AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/lib/cmake/SFML/SFMLConfig.cmake")
    set(USE_LOCAL_SFML_PACKAGE TRUE)
    set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/lib/cmake/SFML")
    set(SFML_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/bin")
    find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
else()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS OFF CACHE BOOL "" FORCE)
    set(LOCAL_SFML_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/sfml-src")

    if(EXISTS "${LOCAL_SFML_SOURCE_DIR}/CMakeLists.txt")
        add_subdirectory("${LOCAL_SFML_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/sfml-build-embedded" EXCLUDE_FROM_ALL)
    else()
        FetchContent_Declare(
            SFML
            URL https://github.com/SFML/SFML/archive/refs/tags/2.6.2.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        FetchContent_MakeAvailable(SFML)
    endif()
endif()

set(SOURCES
    src/App.cpp
    src/Button.cpp
    src/CodeBox.cpp
    src/GraphGroup.cpp
    src/GraphSidebar.cpp
    src/HashTableGroup.cpp
    src/InputBox.cpp
    src/LinkedListGroup.cpp
    src/MenuState.cpp
    src/Sidebar.cpp
    src/TreeGroup.cpp
    src/VisualizerState.cpp
    src/Tree23.cpp
    src/Tree23Node.cpp
    src/Tree23Step.cpp
    src/VisualizerUI.cpp
    src/heap.cpp
    src/heapgroup.cpp
    src/graph/AStar.cpp
    src/graph/GraphModel.cpp
    src/graph/GraphTypes.cpp
    src/Tree23State.cpp
)

add_executable(MySFMLApp main.cpp ${SOURCES})

target_compile_features(MySFMLApp PRIVATE cxx_std_17)
target_link_libraries(MySFMLApp PRIVATE sfml-graphics sfml-window sfml-system)

if(WIN32 AND USE_LOCAL_SFML_PACKAGE)
    add_custom_command(TARGET MySFMLApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_RUNTIME_DIR}"
        $<TARGET_FILE_DIR:MySFMLApp>)
endif()

add_custom_command(TARGET MySFMLApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:MySFMLApp>/assets")
