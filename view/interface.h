#ifndef INTERFACE_H
#define INTERFACE_H
/*
Includes:
*/
#include <GL/glew.h>  // Mudar para GLEW (compatível com o projeto)
#include <GLFW/glfw3.h>
#include "../libs/imgui/imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <sstream>
#include "../model/heaters/capture.h"
#include "../model/heaters/validator.h"
#include "../model/heaters/arquiver.h"
#include "../libs/stb/stb_image.h"
// ===================== CONFIGURAÇÕES DE INTERFACE =====================

// Janela
#define JANELA_LARGURA         1024
#define JANELA_ALTURA          600
#define JANELA_TITULO          "Menu"

// Botões principais
#define TAMANHO_BOTAO_LARG     493
#define TAMANHO_BOTAO_ALT      225
#define ESPACO_ENTRE_BOTOES    20.0f

// Botão menor
#define TAMANHO_BOTAO_PEQUENO_LARG  250
#define TAMANHO_BOTAO_PEQUENO_ALT   60

// Estilo
#define TAMANHO_BORDA_JANELA   1.0f
#define ESCALA_FONTE_MENU      2.0f
#define ESCALA_FONTE_DATA      2.0f

// Estilo dos combos
#define COMBO_LARGURA          300.0f
#define PADDING_FRAME          ImVec2(20, 40)
#define SPACING_ITEM           ImVec2(30, 30)
#define SPACING_INTERNO        ImVec2(15, 25)

// Cor de fundo (RGBA)
#define FUNDO_R 0.2f
#define FUNDO_G 0.2f
#define FUNDO_B 0.2f
#define FUNDO_A 1.0f
#define STB_IMAGE_IMPLEMENTATION


class Interface {
private:
    GLFWwindow* window = NULL;
    GLuint texture_id = 0;
    int image_width = 0;
    int image_height = 0;
    bool should_close = false;
    bool janela_iniciada = false;
    Validator& validator;
    int last_width = 0;
    int last_height = 0;
    int selected_lote = 0;
    int selected_ano = 0;
    int lotes[101];
    int anos[50];
    GLuint pboIds[2] = {0, 0};
    int pboIndex = 0;   
    size_t last_data_size = 0;
    bool pboInitialized = false;
    
    // Novos membros adicionados
    float resolution_scale;   // Controle de escala de resolução
    int max_display_width;    // Largura máxima padrão
    bool imprimindo = false;
    GLuint shutdownTexture = 0;


public:

    Interface(Validator& val);
    ~Interface();

    bool iniciar_janela();
    bool finalizar_janela();
    void process_events();
    bool shouldClose() const;
    void begin_frame();
    void end_frame();
    void menu(int& selected_option, int qntImp);
    bool atualizar_frame(const cv::Mat& frame);
    bool requisitar_data(std::string& selected_date, int tipo);
    bool requisitar_lt(std::string& selected_lt);
    std::string FormatDate(int day, int month, int year);
    void beginFullscreenWindow(const char* name);
    bool config_menu(Arquiver& arq);
    bool config_impress(int & value,bool *InstantImpress);
    bool GetImprimindo();
    void setImprimindo(bool value);
    bool PopUpError(const std::string& message); // faz um pop-up com uma mensagem de erro e um botão "OK"
    GLuint LoadTextureFromFile(const char* filename);
};
#endif // INTERFACE_H