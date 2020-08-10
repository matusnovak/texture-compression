cmake_minimum_required(VERSION 3.10)
include(FindPackageHandleStandardArgs)

project(TextureCompression)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

if(NOT TARGET Stb)
  find_path(STB_INCLUDE_DIR NAMES stb_image.h)
  mark_as_advanced(FORCE STB_INCLUDE_DIR)
  add_library(Stb INTERFACE IMPORTED)
  set_target_properties(Stb PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${STB_INCLUDE_DIR})
endif()

find_package_handle_standard_args(Stb DEFAULT_MSG STB_INCLUDE_DIR)


file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm glad::glad Stb)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

add_custom_command(
  TARGET ${PROJECT_NAME} PRE_BUILD 
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/lena.png $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
