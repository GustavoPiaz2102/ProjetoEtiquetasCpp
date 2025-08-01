cmake_minimum_required(VERSION 3.10)
project(compilated_program)

set(CMAKE_CXX_STANDARD 17)

# Pacotes necessários
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)
#pkg_check_modules(GPIOD REQUIRED libgpiod)  # <---- Adicionado libgpiod
#find_package(raspicam REQUIRED)  # <---- Adicionado raspicam (comentado)

# Arquivos fontes
file(GLOB SOURCES "*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/imguiexemple.cpp")

file(GLOB IMGUI_SOURCES 
    "libs/imgui/imgui.cpp"
    "libs/imgui/imgui_draw.cpp"
    "libs/imgui/imgui_tables.cpp"
    "libs/imgui/imgui_widgets.cpp"
    "libs/imgui/backends/imgui_impl_glfw.cpp"
    "libs/imgui/backends/imgui_impl_opengl3.cpp"
    "model/implement/*.cpp"
    "controller/*.cpp"
    "view/*.cpp"
)

# Executável
add_executable(compilated_program ${SOURCES} ${IMGUI_SOURCES})

# Includes
target_include_directories(compilated_program PRIVATE
    libs/imgui
    libs/imgui/backends
    ${GLFW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${TESSERACT_INCLUDE_DIRS}
    #${GPIOD_INCLUDE_DIRS}          # <---- Inclui libgpiod
    #${raspicam_INCLUDE_DIRS}      # <---- Inclui raspicam (comentado)
)

# Bibliotecas
target_link_libraries(compilated_program
    glfw
    OpenGL::GL
    ${GLEW_LIBRARIES}
    X11
    pthread
    dl
    ${OpenCV_LIBS}
    ${TESSERACT_LIBRARIES}
    #${GPIOD_LIBRARIES}             # <---- Link libgpiod
    #${raspicam_CV_LIBS}          # <---- Link raspicam (comentado)
)