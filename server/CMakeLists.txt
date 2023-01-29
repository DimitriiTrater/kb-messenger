cmake_minimum_required(VERSION 3.5)
project(kb-messenger-server LANGUAGES CXX)

set(CMAKE_CXX_STANDART 20)
set(CMAKE_CXX_STANDART_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED 
    COMPONENTS
        Core
        Network
)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED
    COMPONENTS
        Core
        Network
)

file(GLOB SOURCE_FILES "*.cpp")
file(GLOB HEADER_FILES "*.hpp")

add_executable(${PROJECT_NAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
)
