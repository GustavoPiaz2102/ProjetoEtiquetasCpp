#!/bin/bash

# Atualizar lista de pacotes
sudo apt update

# Instalar compilador e ferramentas básicas
sudo apt install -y build-essential cmake pkg-config

# Instalar dependências do projeto
sudo apt install -y \
    libglfw3-dev \
    libopengl-dev \
    libopencv-dev \
    libglew-dev \
    libtesseract-dev \
    libleptonica-dev \
    libx11-dev \
    tesseract-ocr

# Cria pasta libs se não existir
mkdir -p libs
cd libs

# Clona imgui se a pasta estiver vazia
if [ -z "$(ls -A imgui 2>/dev/null)" ]; then
    git clone https://github.com/ocornut/imgui.git imgui
fi

# Clona stb se a pasta estiver vazia
if [ -z "$(ls -A stb 2>/dev/null)" ]; then
    git clone https://github.com/nothings/stb.git stb
fi

cd ..

# Criar pasta local para tessdata (evita problemas de read-only)
mkdir -p ~/tessdata

# Baixar arquivos de idioma eng e por se não existirem
if [ ! -f ~/tessdata/eng.traineddata ]; then
    wget -qO ~/tessdata/eng.traineddata https://github.com/tesseract-ocr/tessdata_best/raw/main/eng.traineddata
fi

if [ ! -f ~/tessdata/por.traineddata ]; then
    wget -qO ~/tessdata/por.traineddata https://github.com/tesseract-ocr/tessdata_best/raw/main/por.traineddata
fi

# Configurar variável de ambiente TESSDATA_PREFIX para a sessão atual e permanente
export TESSDATA_PREFIX=~/tessdata/
if ! grep -q "TESSDATA_PREFIX" ~/.bashrc; then
    echo "export TESSDATA_PREFIX=~/tessdata/" >> ~/.bashrc
fi

# Verificar instalação do Tesseract
echo "Verificando instalação do Tesseract..."
tesseract --version
echo "Idiomas disponíveis:"
tesseract --list-langs

echo "Dependências instaladas, repositórios clonados e Tesseract configurado com sucesso!"
