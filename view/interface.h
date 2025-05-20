#ifndef INTERFACE_H
#define INTERFACE_H
/*
Includes:
*/
#include <GLFW/glfw3.h>
#include "../libs/imgui/imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <ctime> // pra pegar ano atual
#include <sstream>
#include "../model/capture.h"


// ===================== CONFIGURAÇÕES DE INTERFACE =====================

// Janela
#define JANELA_LARGURA         1920
#define JANELA_ALTURA          1080
#define JANELA_TITULO          "Menu"

// Botões principais
#define TAMANHO_BOTAO_LARG     950
#define TAMANHO_BOTAO_ALT      450
#define ESPACO_ENTRE_BOTOES    30.0f

// Botão menor (ex: "Voltar")
#define TAMANHO_BOTAO_PEQUENO_LARG  400
#define TAMANHO_BOTAO_PEQUENO_ALT   100

// Estilo
#define TAMANHO_BORDA_JANELA   0.0f
#define ESCALA_FONTE_MENU      3.0f
#define ESCALA_FONTE_DATA      3.0f

// Estilo dos combos
#define COMBO_LARGURA          550.0f
#define PADDING_FRAME          ImVec2(45, 80)
#define SPACING_ITEM           ImVec2(70, 70)
#define SPACING_INTERNO        ImVec2(25, 45)

// Cor de fundo (RGBA)
#define FUNDO_R 0.2f
#define FUNDO_G 0.2f
#define FUNDO_B 0.2f
#define FUNDO_A 1.0f



// Classe que define a interface gráfica com o usuário (GUI) utilizando GLFW, OpenGL e ImGui.
class Interface {
private:
    GLFWwindow* window = NULL;     /**< Ponteiro para a janela do GLFW. */
    GLuint texture_id = 0;         /**< ID da textura para exibição de imagens. */
    int image_width = 0;           /**< Largura da imagem exibida. */
    int image_height = 0;          /**< Altura da imagem exibida. */
    bool should_close = false;     /**< Flag para indicar se a janela deve ser fechada. */
    bool janela_iniciada = false; /**< Flag para indicar se a janela foi iniciada. */

public:
    /**
     * @brief Construtor da classe Interface.
     * @details Inicializa a interface gráfica, mas a janela não está criada até que `iniciar_janela` seja chamada.
     * @returns Não há retorno.
     */
    Interface();

    /**
     * @brief Destruidor da classe Interface.
     * @details Libera os recursos de textura e fecha a janela.
     * @returns Não há retorno.
     */
    ~Interface();

    /**
     * @brief Inicializa a janela GLFW, o contexto OpenGL e o ImGui.
     * @details Este método cria uma janela com resolução de 1920x1080, inicializa o OpenGL e o ImGui.
     * @returns `true` se a janela foi criada com sucesso, caso contrário, `false`.
     */
    bool iniciar_janela();

    /**
     * @brief Finaliza a janela GLFW e os recursos do ImGui.
     * @details Este método destrói a janela e libera os recursos relacionados ao OpenGL e ImGui.
     * @returns `true` se a finalização ocorreu corretamente.
     */
    bool finalizar_janela();

    /**
     * @brief Processa eventos de entrada (teclado, mouse) e verifica se a janela deve ser fechada.
     * @details Este método chama `glfwPollEvents` para processar os eventos da janela e atualiza o estado de fechamento.
     * @returns Não há retorno.
     */
    void process_events();

    /**
     * @brief Verifica se a janela foi fechada.
     * @details Este método retorna o estado da janela, indicando se ela deve ser fechada.
     * @returns `true` se a janela foi fechada, `false` caso contrário.
     */
    bool shouldClose() const;

    /**
     * @brief Inicia um novo frame para o ImGui.
     * @details Este método prepara o ImGui para um novo frame, permitindo a criação de novos widgets e interações.
     * @returns Não há retorno.
     */
    void begin_frame();

    /**
     * @brief Finaliza o frame do ImGui, renderizando e atualizando a janela.
     * @details Este método renderiza os dados do ImGui na tela e faz o swap de buffers.
     * @returns Não há retorno.
     */
    void end_frame();

    /**
     * @brief Cria o menu com botões para interagir com a interface.
     * @details Este método exibe uma janela com três botões para interagir: "Rodar Sistema", "Atualizar Data" e "Sair".
     * @param[in,out] selected_option Refere-se à opção selecionada pelo usuário: 0 para "Rodar Sistema", 1 para "Atualizar Data", 2 para "Sair".
     * @returns Não há retorno.
     */
    void menu(int& selected_option);

    /**
     * @brief Atualiza o frame com uma nova imagem da câmera e exibe na interface.
     * @details Este método converte o frame de imagem capturado para o formato RGBA e o exibe na interface do ImGui.
     * @param[in] frame Imagem capturada pela câmera (utilizando OpenCV).
     * @returns `true` se a operação foi bem-sucedida e o menu foi atualizado, `false` em caso de erro.
     */
    bool atualizar_frame(const cv::Mat& frame);
    /**
     * @brief Cria os frames da Interface grafica e combos para a seleção de data do usuario.
     * @details Este método fica em constante chamada até que tenha um true como retorno.
     * @param[in] selected_date Passado por referencia e alterado dentro da função para a data selecionada.
     * @returns `true` se a operação foi bem-sucedida, `false` em caso a operaçção não tenha sido concluida.
     */
    bool requisitar_data(std::string& selected_date);
    /**
     * @brief Formata a data para que fique em um padrão especifico.
     * @details Este método concatena strings e altera a entrada de mouth para uma sigla representante.
     * @param[in] day,mouth,year Passado para que seja formatado.
     * @returns `std::string` com a data formatada.
     */
    std::string FormatDate(int day, int month, int year);
    };

#endif // INTERFACE_H
