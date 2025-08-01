# =================== VERSÃO FINAL COM JSON (MAIS SIMPLES) ===================
cmake_minimum_required(VERSION 3.16)
project(ProjetoEtiquetasCpp LANGUAGES CXX)

# --- Configurações do Compilador ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Encontrar Dependências do Sistema (Sem YAML-CPP!) ---
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Tesseract REQUIRED)

# --- Organizar Arquivos Fonte ---
# Adicionamos 'libs' aos includes para que ele encontre 'json.hpp'
include_directories(libs/imgui libs/imgui/backends libs)

file(GLOB_RECURSE SOURCES
    "main.cpp"
    "controller/*.cpp"
    "model/implement/*.cpp"
    "view/*.cpp"
    "libs/imgui/*.cpp"
    "libs/imgui/backends/imgui_impl_glfw.cpp"
    "libs/imgui/backends/imgui_impl_opengl3.cpp"
)

# --- Criar Executável ---
add_executable(compilated_program ${SOURCES})

# --- Incluir Diretórios ---
# Não precisamos mais de uma seção separada, pois já fizemos acima.
# Mas vamos manter para as outras bibliotecas por boa prática.
target_include_directories(compilated_program PRIVATE
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${Tesseract_INCLUDE_DIRS}
)

# --- Linkar Bibliotecas (Sem YAML-CPP!) ---
target_link_libraries(compilated_program PRIVATE
    Threads::Threads
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OpenCV_LIBS}
    ${Tesseract_LIBRARIES}
)

message(STATUS "Configurado para usar a biblioteca JSON header-only. Sem linkagem extra necessária!")
