cmake_minimum_required(VERSION 3.29)
project(pause)

file(GLOB SOURCE_FILES *.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
