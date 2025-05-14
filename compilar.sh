#!/bin/bash

# Para em caso de erro
set -e

# Cria pasta de build se não existir
mkdir -p build
cd build

# Gera arquivos de build
cmake ..

# Compila usando todos os núcleos disponíveis
echo "🔨 Compilando o projeto..."
make -j$(nproc)


# Volta para o diretório raiz (opcional)
cd ..

echo "✅ Compilação concluída com sucesso!"
# Executa o programa

./build/compilated_program

echo "🛠️  Executando o programa..."