cmake_minimum_required(VERSION 3.10)
project(ImGui_Example)

set(CMAKE_CXX_STANDARD 17)

# Encontra pacotes necessários
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenCV REQUIRED)

# Arquivos fonte
file(GLOB SOURCES "*.cpp")
file(GLOB IMGUI_SOURCES 
    "imgui/*.cpp"
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
    "../model/capture.cpp"
)

# Executável
add_executable(imgui_example ${SOURCES} ${IMGUI_SOURCES})

# Configura includes
target_include_directories(imgui_example PRIVATE
    imgui
    imgui/backends
    ${GLFW_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

# Linka bibliotecas
target_link_libraries(imgui_example
    glfw
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${OpenCV_LIBS}
    # Adicionais para Linux
    X11 pthread dl
)
