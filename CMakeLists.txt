cmake_minimum_required(VERSION 3.14)
project(DataStructureVisualizer)

if(MINGW AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/sfml-build/SFMLConfig.cmake")
    set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/sfml-build")
    set(SFML_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/_deps/sfml-build/lib")
else()
    set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/lib/cmake/SFML")
    set(SFML_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/bin")
endif()

find_package(SFML 2.6 COMPONENTS graphics window system audio network REQUIRED)

include_directories(include)

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

target_link_libraries(MySFMLApp sfml-graphics sfml-window sfml-system)

if(WIN32)
    add_custom_command(TARGET MySFMLApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SFML_RUNTIME_DIR}"
        $<TARGET_FILE_DIR:MySFMLApp>)
endif()

add_custom_command(TARGET MySFMLApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:MySFMLApp>/assets")