cmake_minimum_required(VERSION 3.10)
project(compilated_program)

set(CMAKE_CXX_STANDARD 17)

# Pacotes necessários
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)

# Usa pkg-config para encontrar o Tesseract
find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)

# Arquivos fontes
file(GLOB SOURCES "*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/imguiexemple.cpp") # remove exemplo se necessário

file(GLOB IMGUI_SOURCES 
    "model/implement/*.cpp"
    "controller/*.cpp"
    "view/*.cpp"
    "libs/imgui/*.cpp"
    "libs/imgui/backends/imgui_impl_glfw.cpp"
    "libs/imgui/backends/imgui_impl_opengl3.cpp"
)

# Executável
add_executable(compilated_program ${SOURCES} ${IMGUI_SOURCES})

# Includes
target_include_directories(compilated_program PRIVATE
    libs/imgui/
    libs/imgui/backends/
    ${GLFW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${TESSERACT_INCLUDE_DIRS}
)

# Bibliotecas
target_link_libraries(compilated_program
    glfw
    OpenGL::GL
    X11
    pthread
    dl
    ${OpenCV_LIBS}
    ${TESSERACT_LIBRARIES}
)
