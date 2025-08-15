# ProjetoEtiquetasCpp

## Gerenciamento e Detecção de Etiquetas de Produtos




## Apresentação

O `ProjetoEtiquetasCpp` é uma solução robusta desenvolvida em C++ para o gerenciamento e detecção de etiquetas de produtos. Ele integra funcionalidades de visão computacional, como OCR (Optical Character Recognition) e pré-processamento de imagens, com um sistema de validação de dados e uma interface gráfica intuitiva. O objetivo principal é automatizar o processo de leitura e validação de informações contidas em etiquetas, otimizando fluxos de trabalho em ambientes que dependem da precisão desses dados. **Um diferencial crucial deste projeto é a sua capacidade de integrar a impressão de etiquetas com a subsequente detecção de erros e validação dos dados impressos através de visão computacional, garantindo a qualidade e a conformidade do produto final.**

Este projeto é modular, dividido em componentes como `Model`, `Controller` e `View`, seguindo o padrão arquitetural MVC (Model-View-Controller). Isso garante uma separação clara de responsabilidades, facilitando a manutenção, escalabilidade e futuras expansões. A `Model` lida com a lógica de negócios, incluindo a captura de imagens, pré-processamento, OCR e validação dos dados. A `Controller` gerencia o fluxo de dados entre a `Model` e a `View`, enquanto a `View` é responsável pela interação com o usuário através de uma interface gráfica.

### Principais Funcionalidades:

*   **Captura de Imagens:** Módulo dedicado à aquisição de imagens das etiquetas.
*   **Pré-processamento de Imagens:** Técnicas avançadas para melhorar a qualidade da imagem antes do OCR, garantindo maior precisão na detecção de texto.
*   **OCR (Optical Character Recognition):** Extração de texto a partir das imagens processadas.
*   **Validação de Dados:** Verificação dos dados extraídos contra padrões predefinidos (Lote, Fabricação, Validade) para garantir a conformidade.
*   **Interface Gráfica do Usuário (GUI):** Uma interface amigável para interação do usuário, configuração de parâmetros e visualização de resultados.
*   **Gerenciamento de Configurações:** Persistência de configurações e dados de validação em arquivos, permitindo personalização e reutilização.
*   **Integração com Impressão e Validação:** Capacidade de iniciar processos de impressão e, em seguida, validar os dados impressos usando visão computacional para detecção de erros.

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
*   **`impress`:** Gerencia o processo de impressão das etiquetas, enviando os dados validados para a impressora.

### 2. Controller

A camada `Controller` atua como o intermediário entre a `Model` e a `View`, gerenciando o fluxo de informações e as ações do usuário. Suas principais funções incluem:

*   **`controller`:** Inicia e coordena a sequência de passos do aplicativo. Ele recebe os valores e comandos da interface do usuário (`View`) e os encaminha para os módulos apropriados na `Model` (para detecção, validação, impressão ou mudança de padrão). Também gerencia a persistência de dados de configuração e o controle do fluxo de impressão e validação.
*   **`detector`:** Controla o processo de detecção completo. Ele orquestra a captura da imagem, o pré-processamento, a execução do OCR e a validação contínua dos resultados. O `detector` retorna o resultado da validação para o `controller`, que pode então atualizar a interface ou tomar outras ações com base nesse resultado, **incluindo a sinalização de erros de impressão ou validação**.

### 3. View

A camada `View` é responsável pela interação com o usuário e pela apresentação dos dados. Ela é implementada através de uma interface gráfica do usuário (GUI) e suas principais responsabilidades são:

*   **`interface`:** Fornece a interface visual para o usuário interagir com o sistema. Ela exibe os resultados da detecção e validação, permite a configuração de parâmetros (como padrões de validação, configurações de impressão) e coleta as entradas do usuário, que são então passadas para a `Controller` para processamento. **Também é responsável por exibir mensagens de erro ou sucesso relacionadas à impressão e validação.**

### Fluxo de Operação Detalhado (Impressão e Validação):

1.  **Configuração de Parâmetros:** O usuário configura os parâmetros de impressão através da interface, incluindo tamanho da etiqueta (60mm x 40mm por padrão), espaçamento, densidade, velocidade, posições dos textos e escalas.

2.  **Início da Impressão:** O usuário, através da `interface` (View), inicia o processo de impressão de uma ou mais etiquetas, fornecendo os dados de Lote, Fabricação e Validade.

3.  **Construção do Comando de Impressão:** A classe `Impress` na `Model` constrói um comando específico da impressora (formato ZPL/EPL) que inclui:
    *   Configurações físicas da etiqueta (`SIZE`, `GAP`, `DENSITY`, `SPEED`, `DIRECTION`)
    *   Comandos de texto formatados para cada campo (Lote, Fabricação, Validade) com suas respectivas posições, fontes e escalas
    *   Comando de impressão com a quantidade especificada

4.  **Envio para Impressora:** O comando é enviado diretamente para o dispositivo USB da impressora (`/dev/usb/lp0`) através de comunicação serial.

5.  **Controle de Tempo:** O sistema implementa um controle de tempo mínimo entre impressões (1 segundo) para evitar sobrecarga da impressora.

6.  **Captura Pós-Impressão:** Imediatamente após a impressão, o módulo `capture` da `Model` adquire uma imagem da etiqueta recém-impressa usando uma câmera (implementada via `raspcam`).

7.  **Processamento e OCR:** A imagem é então passada pelo `preprocessor` para otimização e, em seguida, pelo `ocr` (Tesseract) para extrair o texto da etiqueta impressa.

8.  **Validação por Visão Computacional:** O texto extraído é enviado ao módulo `validation` da `Model`, que compara os dados impressos com os dados originais que deveriam ter sido impressos. Esta etapa é crucial para a detecção de erros de impressão, caracteres ilegíveis ou posicionamento incorreto.

9.  **Detecção de Erros:** O `detector` na `Controller` analisa o resultado da validação. Se houver discrepâncias (erros de impressão, caracteres ilegíveis, posicionamento incorreto, etc.), o `detector` sinaliza o erro.

10. **Feedback ao Usuário:** A `Controller` informa a `interface` (View) sobre o status da validação. Em caso de erro, uma mensagem de alerta é exibida ao usuário, permitindo a intervenção ou correção. Em caso de sucesso, a impressão é confirmada.

11. **Persistência de Configurações:** Todas as configurações de impressão são automaticamente salvas no arquivo `data/config.txt` para reutilização em sessões futuras.

12. **Ciclo Contínuo:** Este ciclo se repete para cada etiqueta, garantindo que todas as impressões sejam validadas e que quaisquer erros sejam prontamente identificados.



## Execução

Após a compilação bem-sucedida, você pode executar o projeto de duas maneiras:

### 1. Usando o Script de Execução:

```bash
./run.sh
```

### 2. Executando Diretamente:

```bash
./build/ProjetoEtiquetasCpp
```

### Interface do Usuário:

Ao executar o programa, uma interface gráfica será apresentada com as seguintes opções:

*   **Detector:** Inicia o processo de detecção e validação de etiquetas em tempo real.
*   **Configurar Fabricação:** Define o padrão de data de fabricação para validação.
*   **Configurar Validade:** Define o padrão de data de validade para validação.
*   **Configurar Lote:** Define o padrão de lote para validação.
*   **Configurar Impressão:** Configura parâmetros de impressão e quantidade de etiquetas.
*   **Menu de Configuração:** Acesso a configurações avançadas do sistema.

### Estrutura de Arquivos:

*   **`data/config.txt`:** Arquivo de configuração que armazena os padrões de validação.
*   **`build/`:** Diretório contendo os arquivos compilados.
*   **`libs/`:** Bibliotecas externas utilizadas pelo projeto.
*   **`tests/`:** Testes unitários e de integração.

## Contribuição

Contribuições são bem-vindas! Para contribuir:

1. Faça um fork do projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

## Licença

Este projeto está sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

## Autor

Desenvolvido por [GustavoPiaz2102](https://github.com/GustavoPiaz2102)

---

**Nota:** Este projeto está em desenvolvimento ativo. Algumas funcionalidades podem estar em fase de implementação ou testes.


### Especificações Técnicas da Impressão:

#### Parâmetros Configuráveis:
*   **Tamanho da Etiqueta:** Padrão 60mm x 40mm (configurável)
*   **Espaçamento:** 2mm x 0mm entre etiquetas (configurável)
*   **Densidade:** 8 dpi (configurável de 1-15)
*   **Velocidade:** 4 ips (configurável de 1-14)
*   **Direção:** 1 (normal) ou 0 (invertida)

#### Posicionamento do Texto:
*   **Posição X:** 100 pixels (configurável)
*   **Posição Y Lote:** 100 pixels (configurável)
*   **Posição Y Fabricação:** 150 pixels (configurável)
*   **Posição Y Validade:** 200 pixels (configurável)
*   **Rotação:** 0° (configurável)
*   **Escala X/Y:** 1.0 (configurável)

#### Formato dos Dados Impressos:
*   **Lote:** "L.:[valor]"
*   **Fabricação:** "FAB.:[valor]"
*   **Validade:** "VAL.:[valor]"

#### Comunicação com Impressora:
*   **Interface:** USB (`/dev/usb/lp0`)
*   **Protocolo:** ZPL/EPL (Zebra Programming Language / Eltron Programming Language)
*   **Controle de Fluxo:** Tempo mínimo de 1 segundo entre impressões
*   **Tratamento de Erros:** Verificação de disponibilidade do dispositivo antes do envio
