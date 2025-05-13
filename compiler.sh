#!/bin/bash

# Configurações
BIN_NAME="prog"
BUILD_DIR="./build"
CMAKE_BUILD_TYPE="Release"  # Altere para "Debug" se necessário

# Cria diretório de build se não existir
mkdir -p $BUILD_DIR

echo -e "\033[34mConfigurando o projeto com CMake...\033[0m"

# Executa o CMake
cmake -S . -B $BUILD_DIR -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE

if [ $? -ne 0 ]; then
    echo -e "\033[31mErro na configuração do CMake!\033[0m"
    exit 1
fi

echo -e "\033[34mCompilando o projeto...\033[0m"

# Compila o projeto
cmake --build $BUILD_DIR --config $CMAKE_BUILD_TYPE

# Verifica status da compilação
if [ $? -eq 0 ]; then
    echo -e "\033[32mCompilação concluída com sucesso!\033[0m"
    
    # Mudando permissões do executável (opcional)
    chmod +x "${BUILD_DIR}/${BIN_NAME}"
    
    echo "Executando o programa..."
    cd $BUILD_DIR && ./$BIN_NAME
else
    echo -e "\033[31mErro na compilação!\033[0m"
    exit 1
fi