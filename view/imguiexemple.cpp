/*#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "../model/capture.h"

class Interface {
private:
    GLFWwindow* window = NULL;
    GLuint texture_id = 0;
    int image_width = 0;
    int image_height = 0;
    bool should_close = false;

public:
    Interface() {}
    ~Interface() {
        if (texture_id != 0) {
            glDeleteTextures(1, &texture_id);
        }
        finalizar_janela();
    }

    bool iniciar_janela() {
        if (!glfwInit()) {
            std::cerr << "Falha ao inicializar GLFW\n";
            return false;
        }
        
        window = glfwCreateWindow(1920, 1080, "Menu", NULL, NULL);
        if (!window) {
            glfwTerminate();
            std::cerr << "Falha ao criar janela GLFW\n";
            return false;
        }
        
        glfwMakeContextCurrent(window);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
        
        return true;
    }

    bool finalizar_janela() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        if (window) {
            glfwDestroyWindow(window);
            glfwTerminate();
            window = NULL;
        }
        return true;
    }

    void process_events() {
        glfwPollEvents();
        should_close = glfwWindowShouldClose(window);
    }

    bool shouldClose() const {
        return should_close;
    }

    void begin_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void end_frame() {
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    void menu(int& selected_option) {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Main", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground);

        ImGui::Text("Escolha um botão:");
        
        if (ImGui::Button("Rodar Sistema", ImVec2(800, 400))) {
            selected_option = 0;
        }

        O valor dentro do same line e do Dummy é o espaçamento entre os widgets!!
        
        ImGui::SameLine(0,10.0f);
        if (ImGui::Button("Atualizar Data", ImVec2(800, 400))) {
            selected_option = 1;
        }
        ImGui::Dummy(ImVec2(0,10.0f));
        if (ImGui::Button("Sair", ImVec2(800, 400))) {
            selected_option = 2;
        }

        ImGui::End();
    }

    bool atulizar_frame(const cv::Mat& frame) {
        if (frame.empty()) {
            std::cerr << "Frame vazio recebido!" << std::endl;
            return false;
        }

        cv::Mat image_rgba;
        if (frame.channels() == 1) {
            cv::cvtColor(frame, image_rgba, cv::COLOR_GRAY2RGBA);
        } else if (frame.channels() == 3) {
            cv::cvtColor(frame, image_rgba, cv::COLOR_BGR2RGBA);
        } else if (frame.channels() == 4) {
            image_rgba = frame.clone();
        } else {
            ImGui::Text("Formato de imagem não suportado!");
            return false;
        }

        image_width = image_rgba.cols;
        image_height = image_rgba.rows;

        if (texture_id == 0) {
            glGenTextures(1, &texture_id);
        }

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 
                    0, GL_RGBA, GL_UNSIGNED_BYTE, image_rgba.data);

        float aspect_ratio = (float)image_width / (float)image_height;
        float display_width = ImGui::GetIO().DisplaySize.x * 0.8f;
        float display_height = display_width / aspect_ratio;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Camera View", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus);

        //Posição da imagem
        
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 1.1*display_width) * 0.5f);
        ImGui::SetCursorPosY((ImGui::GetWindowHeight() - 1.1*display_height) * 0.5f);
        ImGui::Image((ImTextureID)(intptr_t)texture_id, ImVec2(display_width, display_height));
        ImGui::SameLine();
        ImGui::Text("Texto Dectado Vai Aqui!!!!");
        bool return_to_menu = false;
        if (ImGui::Button("Voltar ao Menu", ImVec2(200, 50))) {
            return_to_menu = true;
        }

        ImGui::End();
        
        return return_to_menu;
    }
};

int main() {
    Interface interface;
    if (!interface.iniciar_janela()) {
        return -1;
    }

    int selected_option = -1;
    Capture camera(0);
    while (!interface.shouldClose()) {
        interface.process_events();
        interface.begin_frame();

        if (selected_option == -1 || selected_option == 1) {
            interface.menu(selected_option);
        } 
        else if (selected_option == 2) { // Sair
            break;
        }
        else if (selected_option == 0) { // Rodar Sistema
            cv::Mat frame = camera.captureImage();
            if (!frame.empty()) {
                if (interface.atulizar_frame(frame)) {
                    selected_option = -1; // Voltar ao menu
                }
            }
        }

        interface.end_frame();
    }

    return 0;
}*/