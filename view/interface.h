#ifndef INTERFACE_H
#define INTERFACE_H
/*
Includes:
*/
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "../model/capture.h"

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
    bool atulizar_frame(const cv::Mat& frame);
    /**
     * @brief Atualiza o texto exibido na interface.
     * @details Este método atualiza o texto detectado na interface gráfica.
     * @param[in] text Texto a ser exibido.
     * @returns `true` se a operação foi bem-sucedida, `false` em caso de erro.
     */
    bool atualizar_texto(std::string text) ;
    };

#endif // INTERFACE_H
