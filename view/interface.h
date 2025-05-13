#ifndef INTERFACE_H
#define INTERFACE_H

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream> 

/**
 * @class Interface
 * @brief Classe responsável por criar e gerenciar a interface gráfica com GLFW e ImGui.
 */
class Interface {
    public:
        /**
         * @brief Construtor padrão da classe Interface.
         */
        Interface();

        /**
         * @brief Destrutor da classe Interface.
         */
        ~Interface();

        /**
         * @brief Inicializa a janela GLFW e o contexto do ImGui.
         * 
         * Cria a janela principal, inicializa o contexto OpenGL e ImGui,
         * e configura os backends necessários.
         */
        void iniciar_janela();

        /**
         * @brief Finaliza e limpa os recursos utilizados pela janela e pelo ImGui.
         * 
         * Encerra o contexto ImGui, fecha a janela GLFW e termina o uso da biblioteca.
         */
        void finalizar_janela();

        /**
         * @brief Exibe o menu principal com opções de interação do usuário.
         * 
         * Cria um loop de renderização onde os botões são exibidos,
         * retornando um valor baseado na escolha do usuário.
         * 
         * @return int Código da ação selecionada:
         *         0 - Rodar Sistema
         *         1 - Atualizar Data
         *         2 - Sair
         */
        int menu();

    private:
        GLFWwindow* window; ///< Ponteiro para a janela GLFW.
};

#endif // INTERFACE_H
