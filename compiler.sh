#!/bin/bash

# Script de build para o projeto de OCR com OpenCV + Tesseract

# Configurações
BIN_NAME="prog"
BUILD_DIR="./build"
SRC_DIRS=(
    "main.cpp"
    "controller/controller.cpp" 
    "controller/detector.cpp"
    "model/preprocessor.cpp"
    "model/OCR.cpp"
    "model/capture.cpp"
)
INCLUDE_DIRS=("-Icontroller" "-Imodel")
PKG_DEPS="opencv4 gtkmm-3.0"
LIBS=("-ltesseract")

# Cria diretório de build se não existir
mkdir -p $BUILD_DIR

echo "Compilando o projeto..."

# Comando de compilação
g++ -std=c++11 "${SRC_DIRS[@]}" \
    -o "${BUILD_DIR}/${BIN_NAME}" \
    "${INCLUDE_DIRS[@]}" \
    $(pkg-config --cflags --libs $PKG_DEPS) \
    "${LIBS[@]}" \
    -Wall -Wextra -O2

# Verifica status da compilação
if [ $? -eq 0 ]; then
    echo -e "\033[32mCompilação concluída com sucesso!\033[0m"
    echo "Mudando permissões do executável..."
    chmod +x "${BUILD_DIR}/${BIN_NAME}"
    echo "Executando o programa..."
    cd $BUILD_DIR && ./$BIN_NAME
else
    echo -e "\033[31mErro na compilação!\033[0m"
    exit 1
fi