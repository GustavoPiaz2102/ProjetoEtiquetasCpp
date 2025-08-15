# ProjetoEtiquetasCpp

## Gerenciamento e Detecção de Etiquetas de Produtos




## Apresentação

O `ProjetoEtiquetasCpp` é uma solução robusta desenvolvida em C++ para o gerenciamento e detecção de etiquetas de produtos. Ele integra funcionalidades de visão computacional, como OCR (Optical Character Recognition) e pré-processamento de imagens, com um sistema de validação de dados e uma interface gráfica intuitiva. O objetivo principal é automatizar o processo de leitura e validação de informações contidas em etiquetas, otimizando fluxos de trabalho em ambientes que dependem da precisão desses dados.

Este projeto é modular, dividido em componentes como `Model`, `Controller` e `View`, seguindo o padrão arquitetural MVC (Model-View-Controller). Isso garante uma separação clara de responsabilidades, facilitando a manutenção, escalabilidade e futuras expansões. A `Model` lida com a lógica de negócios, incluindo a captura de imagens, pré-processamento, OCR e validação dos dados. A `Controller` gerencia o fluxo de dados entre a `Model` e a `View`, enquanto a `View` é responsável pela interação com o usuário através de uma interface gráfica.

### Principais Funcionalidades:

*   **Captura de Imagens:** Módulo dedicado à aquisição de imagens das etiquetas.
*   **Pré-processamento de Imagens:** Técnicas avançadas para melhorar a qualidade da imagem antes do OCR, garantindo maior precisão na detecção de texto.
*   **OCR (Optical Character Recognition):** Extração de texto a partir das imagens processadas.
*   **Validação de Dados:** Verificação dos dados extraídos contra padrões predefinidos (Lote, Fabricação, Validade) para garantir a conformidade.
*   **Interface Gráfica do Usuário (GUI):** Uma interface amigável para interação do usuário, configuração de parâmetros e visualização de resultados.
*   **Gerenciamento de Configurações:** Persistência de configurações e dados de validação em arquivos, permitindo personalização e reutilização.
*   **Integração com Impressão:** Capacidade de iniciar processos de impressão com base nos dados validados.

O `ProjetoEtiquetasCpp` é ideal para aplicações industriais, logísticas ou de controle de qualidade, onde a leitura rápida e precisa de etiquetas é crucial. Sua arquitetura flexível e o uso de C++ garantem alta performance e eficiência.




## Instalação

Para compilar e executar o `ProjetoEtiquetasCpp`, você precisará ter as seguintes ferramentas e bibliotecas instaladas em seu sistema:

*   **Compilador C++:** Um compilador compatível com C++11 ou superior (por exemplo, GCC ou Clang).
*   **CMake:** Um sistema de automação de build para gerenciar o processo de compilação.
*   **GLEW (OpenGL Extension Wrangler Library):** Uma biblioteca de extensão para OpenGL, utilizada para a interface gráfica.
*   **OpenCV:** Uma biblioteca de visão computacional, essencial para o pré-processamento de imagens e OCR.
*   **Tesseract OCR:** Um motor de OCR de código aberto, utilizado para a detecção de texto nas imagens.
*   **ImGui:** Uma biblioteca de GUI imediata, utilizada para a interface do usuário.

### Passos para Instalação e Compilação:

1.  **Clone o Repositório:**

    ```bash
    git clone https://github.com/GustavoPiaz2102/ProjetoEtiquetasCpp.git
    cd ProjetoEtiquetasCpp
    ```

2.  **Instale as Dependências (Exemplo para Ubuntu/Debian):**

    ```bash
    sudo apt update
    sudo apt install build-essential cmake libglew-dev libopencv-dev libleptonica-dev libtesseract-dev tesseract-ocr libglfw3-dev
    ```

    *Nota: As dependências podem variar ligeiramente dependendo da sua distribuição Linux ou sistema operacional. Certifique-se de instalar as versões de desenvolvimento (`-dev`) das bibliotecas.* 

3.  **Compile o Projeto:**

    O projeto inclui um script `compilar.sh` que automatiza o processo de compilação usando CMake. Execute-o na raiz do projeto:

    ```bash
    ./compilar.sh
    ```

    Alternativamente, você pode compilar manualmente:

    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

    Após a compilação, o executável será gerado no diretório `build` (ou na raiz do projeto, dependendo da configuração do CMake).




## Funcionamento

O `ProjetoEtiquetasCpp` opera com base em uma arquitetura MVC (Model-View-Controller), garantindo uma separação clara das responsabilidades e um fluxo de dados bem definido. Abaixo, detalhamos o funcionamento de cada componente:

### 1. Model

A camada `Model` é o coração da lógica de negócios e processamento de dados. Ela é responsável por:

*   **`capture`:** Realiza a captura de imagens das etiquetas, fornecendo os dados brutos para processamento.
*   **`preprocessor`:** Recebe a imagem capturada e aplica diversas técnicas de pré-processamento (como binarização, remoção de ruído, correção de perspectiva, etc.) para otimizar a qualidade da imagem para o OCR. Isso é crucial para garantir a precisão na detecção de texto.
*   **`ocr`:** Após o pré-processamento, este módulo utiliza o motor Tesseract OCR para extrair o texto da imagem. O texto detectado é então retornado para a validação.
*   **`validation`:** Recebe o texto extraído pelo OCR e o valida contra padrões predefinidos (Lote, Fabricação, Validade). Este módulo verifica se o formato e o conteúdo do texto estão em conformidade com as regras de negócio, garantindo a integridade dos dados.
*   **`library`:** Atua como um agregador de imports para a camada `Controller`, garantindo que todos os módulos necessários da `Model` estejam acessíveis para o gerenciamento do fluxo de dados.

### 2. Controller

A camada `Controller` atua como o intermediário entre a `Model` e a `View`, gerenciando o fluxo de informações e as ações do usuário. Suas principais funções incluem:

*   **`controller`:** Inicia e coordena a sequência de passos do aplicativo. Ele recebe os valores e comandos da interface do usuário (`View`) e os encaminha para os módulos apropriados na `Model` (para detecção, validação ou mudança de padrão). Também gerencia a persistência de dados de configuração.
*   **`detector`:** Controla o processo de detecção completo. Ele orquestra a captura da imagem, o pré-processamento, a execução do OCR e a validação contínua dos resultados. O `detector` retorna o resultado da validação para o `controller`, que pode então atualizar a interface ou tomar outras ações com base nesse resultado.

### 3. View

A camada `View` é responsável pela interação com o usuário e pela apresentação dos dados. Ela é implementada através de uma interface gráfica do usuário (GUI) e suas principais responsabilidades são:

*   **`interface`:** Fornece a interface visual para o usuário interagir com o sistema. Ela exibe os resultados da detecção e validação, permite a configuração de parâmetros (como padrões de validação, configurações de impressão) e coleta as entradas do usuário, que são então passadas para a `Controller` para processamento.

### Fluxo de Operação:

1.  O usuário interage com a `interface` (View), selecionando opções ou inserindo dados.
2.  A `Controller` recebe essas entradas e decide qual ação tomar, acionando os módulos relevantes na `Model`.
3.  A `Model` executa as operações necessárias (captura, pré-processamento, OCR, validação) e retorna os resultados para a `Controller`.
4.  A `Controller` processa os resultados e atualiza a `View` para refletir o novo estado do sistema ou exibir informações ao usuário.
5.  Este ciclo se repete continuamente, permitindo a detecção e validação em tempo real das etiquetas.

