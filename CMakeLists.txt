cmake_minimum_required(VERSION 3.22) #old 4.1
project(OrbitGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Disable unnecessary GLFW stuff
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add shaders
file(GLOB SHADERS "shaders/*.glsl")
# And Textures
set(TEXTURES
        textures/
)
foreach(shader ${SHADERS})
    file(COPY ${shader} DESTINATION ${CMAKE_BINARY_DIR}/shaders)
endforeach()
foreach(texture ${TEXTURES})
    file(COPY ${texture} DESTINATION ${CMAKE_BINARY_DIR}/textures)
endforeach()

# Add GLFW
add_subdirectory(glfw)
add_subdirectory(glad/include/glm)

# Add GLAD
add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)


# executable
add_executable(OrbitGL main.cpp
        shader.h
        stb_image.h
        texture_loader.cpp
        camera.h
)


# Link libraries
target_link_libraries(OrbitGL
        PRIVATE
        glfw
        glad
)
