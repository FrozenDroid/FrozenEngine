cmake_minimum_required(VERSION 3.12)
project(OpenGLTest)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries")
set(SOURCES "${SRC_DIR}/main.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set(CMAKE_CXX_STANDARD 17)


# GLM
find_package(glm REQUIRED)
#target_link_libraries(${PROJECT_NAME} glm)

# GLFW
find_package(glfw3 REQUIRED)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
target_link_libraries(${PROJECT_NAME} glfw)


# GLAD
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# GLAD
set(STB_IMAGE_DIR "${LIB_DIR}/stb_image")
add_library("stb_image" "${STB_IMAGE_DIR}/src/stb_image.cpp")
target_include_directories("stb_image" PRIVATE "${STB_IMAGE_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${STB_IMAGE_DIR}/include")
target_link_libraries(${PROJECT_NAME} "stb_image" "${CMAKE_DL_LIBS}")

