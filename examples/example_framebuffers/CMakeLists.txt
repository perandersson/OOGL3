# Create a variable containing all .cpp files:
file(GLOB example_framebuffers_SOURCES ${EXAMPLES_DIR}/example_framebuffers/src/*.cpp)
include_directories (${ROOT_DIR}/pogl/include)
include_directories (${EXAMPLES_DIR}/examples_window/include)

# Add OpenGL package
find_package(OpenGL REQUIRED)

# Create an executable file from sources
add_executable(example_framebuffers ${example_framebuffers_SOURCES})

# Add link libraries
target_link_libraries(example_framebuffers ${OPENGL_LIBRARIES})
target_link_libraries(example_framebuffers examples_window)
target_link_libraries(example_framebuffers pogl)
