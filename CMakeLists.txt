cmake_minimum_required(VERSION 3.10)
project(MySFMLProject)

# 1. Chỉ đường dẫn đến thư mục chứa các file cấu hình của SFML
# Lưu ý: Đường dẫn này trỏ vào thư mục chứa SFMLConfig.cmake
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/lib/cmake/SFML")

# 2. Tìm kiếm thư viện SFML
find_package(SFML 2.6 COMPONENTS graphics window system audio network REQUIRED)

# 3. Thêm file source code của bạn
include_directories(include)
set(SOURCES
    src/App.cpp
    src/Button.cpp
    src/CodeBox.cpp
    src/GraphGroup.cpp
    src/HashTableGroup.cpp
    src/InputBox.cpp
    src/LinkedListGroup.cpp
    src/MenuState.cpp
    src/Sidebar.cpp
    src/TreeGroup.cpp
    src/VisualizerState.cpp
)
add_executable(MySFMLApp main.cpp ${SOURCES})
# 4. Liên kết các thư viện SFML với file thực thi
target_link_libraries(MySFMLApp sfml-graphics sfml-window sfml-system)

# (Tùy chọn) Copy các file DLL vào thư mục build để có thể chạy được app
if(WIN32)
    add_custom_command(TARGET MySFMLApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2/bin"
        $<TARGET_FILE_DIR:MySFMLApp>)
endif()

# Thiết lập tự động Copy thư mục assets vào thư mục build (cạnh file .exe) mỗi lần Compile
add_custom_command(TARGET MySFMLApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:MySFMLApp>/assets")